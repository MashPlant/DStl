#pragma once
#include "Algorithm.h"
#include <memory>
namespace ds
{
	template<typename K, typename V, size_t BF>
	class BaseNaiveDB
	{
	protected:
		class BTree
		{
		public:
			typedef std::pair<K, V> value_type;
			friend BaseNaiveDB;
		private:
			struct Node
			{
				//ԭ���B��֮���Բ������飬��Ϊ�˱ܿ���������ʱԪ���Զ�����
				//Ȼ��pair����û���ڴ����ֻ�Ǽ򵥵��Զ�����������first��second
				//���޶�K,V�����ܹ����ڴ棬��ôbitwise����֮��ĸ����ǿ��Ա��ظ�������
				value_type values[2 * BF - 1];
				int child[2 * BF] = { 0 };
				int cnt = 0;
				bool isLeaf = true;
				const K &key(int i)const { return values[i].first; }
				V &val(int i)const { return values[i].second; }
			};
			typedef std::shared_ptr<Node> pointer;
			typedef std::pair<pointer, int> INode;
			INode root;

			std::function<bool(const value_type &lhs, const K&rhs)> pairComp = [](const value_type &lhs, const K&rhs) {return lhs.first < rhs; };
			template <typename T>
			static void shiftBy(T *ptr, int length, int shift) { memmove(ptr + shift, ptr, length * sizeof(T)); }
			void split(INode ix, INode iy, int where) //assert iy.first == read(ix.first.child[where])
			{
				pointer x = ix.first;
				INode iz = container->allocate();
				pointer y = iy.first, z = iz.first;

				z->isLeaf = y->isLeaf;
				memmove(z->values, y->values + BF, (BF - 1) * sizeof(value_type));
				memmove(z->child, y->child + BF, BF * sizeof(int));
				z->cnt = y->cnt = BF - 1;
				shiftBy(x->child + where + 1, x->cnt - where, 1);
				x->child[where + 1] = iz.second;
				shiftBy(x->values + where, x->cnt - where, 1);
				new(x->values + where) value_type(y->values[BF - 1]);
				++x->cnt;

				container->write(ix);
				container->write(iy);
				container->write(iz);
			}
			void insertEmpty(INode ix, const K& key, const V& val)
			{
				while (true)
				{
					pointer x = ix.first;
					int i = ds::lower_bound(x->values, x->values + x->cnt, key, pairComp) - x->values;
					if (i != x->cnt && !(key<x->values[i].first))//�ظ�����
						return;
					if (x->isLeaf)
					{
						shiftBy(x->values + i, x->cnt - i, 1);
						new(x->values + i) value_type({ key,val });
						++x->cnt;
						container->write(ix);
						break;
					}
					INode iy = container->read(x->child[i]);
					if (iy.first->cnt == 2 * BF - 1)
					{
						split(ix, iy, i);
						if (x->key(i) < key)
							++i, iy = container->read(x->child[i]);
						else if (!(key < x->key(i))) //�صú��
							return;
					}
					ix = iy;
				}
			}

			void merge(INode ix, INode iy, INode iz, int i)
			{
				//assert iy��ix�ĵ�i�����ӣ�iz��ix�ĵ�i+1������
				//��x��i������i+1���Ӻϲ�����x��i����Ϊ�ָ������������Ӷ�ֻ��BF-1�����ӣ��ϲ�����2*BF-1��
				pointer x = ix.first;
				pointer y = iy.first, z = iz.first;

				y->cnt = 2 * BF - 1;
				new (y->values + BF - 1) value_type(std::move(x->values[i]));
				memmove(y->values + BF, z->values, (BF - 1) * sizeof(value_type));
				memmove(y->child + BF, z->child, BF * sizeof(int));
				shiftBy(x->values + i + 1, x->cnt - i - 1, -1);
				shiftBy(x->child + i + 2, x->cnt - i - 1, -1);
				--x->cnt;

				container->write(ix);
				container->write(iy);
				container->write(iz); //��ʵz�Ѿ�û����,Ӧ�ð�z�ڵ�����ɾ���ģ�������������
			}
			void erase(INode ix, const K& key)
			{
				pointer x = ix.first;
				int i = ds::lower_bound(x->values, x->values + x->cnt, key, pairComp) - x->values;
				if (i != x->cnt && !(key<x->values[i].first)) //�ҵ�key��
				{
					if (x->isLeaf) //x��Ҷ�ڵ㣬ֱ��ɾ��
					{
						(x->values + i)->~value_type();
						shiftBy(x->values + i + 1, --x->cnt - i, -1); //��Ҫ�ƶ����ڴ���x->cnt-i-1
					}
					else
					{
						INode iy, iz;
						if (iy = container->read(x->child[i]), iy.first->cnt >= BF) //ǰ�����ں������㹻�ĺ���(��Ӧ�����ĺ��ӵ�����)
						{
							pointer y = iy.first;
							while (!y->isLeaf)
								y = container->read(y->child[y->cnt]).first; //��ǰ��
							x->values[i] = y->values[y->cnt - 1];
							erase(iy, x->key(i));
						}
						else if (iz = container->read(x->child[i + 1]), iz.first->cnt >= BF) //������ں������㹻�ĺ���
						{
							pointer z = iz.first;
							while (!z->isLeaf)
								z = container->read(z->child[0]).first; //�Һ��
							x->values[i] = z->values[0];
							erase(iz, x->key(i));
						}
						else //��û��,��ô���������ڵ㶼�ϲ���y�У�����Ų��x�ĺ��Ӻͼ�
						{
							merge(ix, iy, iz, i);
							if (root.first->cnt == 0)
								root = iy;
							erase(iy, key);
						}
					}
				}
				else if (!x->isLeaf)	//û���ҵ�key
				{
					INode iy = container->read(x->child[i]), pre, nxt;
					pointer y = iy.first, z;
					if (y->cnt == BF - 1)
					{
						if (i >= 1 && (pre = container->read(x->child[i - 1]), pre.first->cnt >= BF)) //���ֵܣ�ȡ�����������
						{
							z = pre.first;

							shiftBy(y->values, y->cnt, 1);
							new(y->values) value_type(std::move(x->values[i - 1]));
							new(x->values + i - 1) value_type(std::move(z->values[z->cnt - 1]));
							shiftBy(y->child, y->cnt + 1, 1);
							y->child[0] = z->child[z->cnt];
							--z->cnt, ++y->cnt;

							container->write(pre); //x��y���ں�����ĩβ��write
							erase(iy, key);
						}
						else if (i < x->cnt && (nxt = container->read(x->child[i + 1]), nxt.first->cnt >= BF)) //���ֵ�,ȡ��������С����
						{
							z = nxt.first;

							new(y->values + y->cnt) value_type(std::move(x->values[i]));
							new(x->values + i) value_type(std::move(z->values[0]));
							y->child[y->cnt + 1] = z->child[0];
							shiftBy(z->values + 1, z->cnt - 1, -1);
							shiftBy(z->child + 1, z->cnt, -1);
							--z->cnt, ++y->cnt;

							container->write(nxt);
							erase(iy, key);
						}
						else //�����ֵܶ�û�нڵ��,��ô��������������ĸ��ֵܺϲ�
						{
							//��ʱpre��nxt(�������)���Ѿ�������
							if (i != 0)
							{
								nxt = iy;
								iy = pre;
								--i;
							}
							merge(ix, iy, nxt, i);
							if (root.first->cnt == 0)
								root = iy;
							erase(iy, key);
						}
					}
					else
						erase(iy, key);
				}
				container->write(ix);
			}
		public:
			void insert(const K& key, const V& val)
			{
				if (root.first->cnt == 2 * BF - 1)
				{
					INode ix = container->allocate(), tmp = root;
					root = ix;
					ix.first->isLeaf = false, ix.first->child[0] = tmp.second;
					split(ix, tmp, 0);
				}
				insertEmpty(root, key, val);
			}
			void insert(const value_type &kvpair) { insert(kvpair.first, kvpair.second); }
			void erase(const K& key) { erase(root, key); }
			std::shared_ptr<V> find(const K& key) //ԭ���İ汾�Ƿ���V*��Ȼ��ע�⵽�м�ڵ�(�ڲ����ļ�ʱ)�ᱻ���������ص���ָͨ���������
			{
				INode ix = root;
				while (true)
				{
					pointer x = ix.first;
					value_type* lb = ds::lower_bound(x->values, x->values + x->cnt, key, pairComp);
					if (lb < x->values + x->cnt && !(key < lb->first))
						return std::make_shared<V>(lb->second);
					if (x->isLeaf)
						return nullptr;
					ix = container->read(x->child[lb - x->values]);
				}
			}
			int getRootIndex() const { return root.second; }
			BaseNaiveDB *container;
			BTree(BaseNaiveDB *container) :container(container)
			{
				root = container->getRoot();
			}
			BTree(const BTree &rhs) = delete; //ʵ��������д��...
			BTree &operator=(const BTree &rhs) = delete;
			bool check(INode x)
			{
				bool ret = ((x == root) || x.first->cnt >= BF - 1) && (x.first->cnt <= 2 * BF - 1);
				if (!x.first->isLeaf)
					for (int i=0;i<=x.first->cnt && ret;++i)
						ret &= check(container->read(x.first->child[i]));
				return ret;
			}
		};
		typedef typename BTree::pointer pointer;
		typedef typename BTree::Node Node;
		typedef typename BTree::INode INode;
		virtual INode read(int) = 0;
		virtual INode allocate() = 0;
		virtual void write(INode) = 0;
		virtual INode getRoot() = 0; //��ʼ��ʱ����,�����ڴ�DBֱ��allocate�������ļ�DB�򲻿�
		BTree *bt;
	public:
		bool check(){return bt->check(bt->root);}
		BaseNaiveDB()
		{
			static_assert(std::is_nothrow_copy_assignable<K>::value, "Key must not manage memory\n");
			static_assert(std::is_nothrow_copy_assignable<V>::value, "Value must not manage memory\n");
		}
		void insert(const K& key, const V& val) { bt->insert(key, val); }
		std::shared_ptr<V> find(const K &key) { return bt->find(key); }
		void erase(const K &key) { bt->erase(key); }
		virtual ~BaseNaiveDB() {}
	};

	template<typename K, typename V, size_t BF>
	class MemoryNaiveDB : public BaseNaiveDB<K, V, BF>
	{
	public:
		MemoryNaiveDB(int dbSize) :BaseNaiveDB()
		{
			arr = new pointer[dbSize];  //arr=new...���ں���͹���,Ŀǰ�һ���֪��Ϊʲô
			bt = new BTree(this);
		}
		~MemoryNaiveDB() { delete []arr; }
	private:
		//Node **arr=new Node*[100000]; ��Ҫ��������ָ�����ָ��!!!!!
		pointer *arr;
		int pos = 0;
		INode read(int i) override { return { arr[i],i }; }
		INode allocate() override
		{
			arr[pos++] = std::make_shared<Node>();
			return { arr[pos - 1],pos - 1 };
		}
		void write(INode ix) override { arr[ix.second] = ix.first; }
		INode getRoot() override { return allocate(); }
	};

	template<typename K, typename V, size_t BF>
	class NaiveDB : public BaseNaiveDB<K, V, BF> //���뱣��root��λ��,��ôrootд�ڵ�һ��int�����մ����ļ�ʱ��Ҫ����
	{
	private:
		FILE *db;
		int pos = 0, root = -1;
		const int offset = sizeof(int);
		const int nodeSize = sizeof(Node);
		INode read(int i) override
		{
			fseek(db, i * nodeSize + offset, SEEK_SET);
			auto ret = std::make_shared<Node>();
			fread(ret.get(), nodeSize, 1, db);
			return { ret,i };
		}
		INode allocate() override
		{
			return { std::make_shared<Node>() ,pos++ };
		}
		void write(INode ix) override
		{
			fseek(db, ix.second * nodeSize + offset, SEEK_SET);
			fwrite(ix.first.get(), nodeSize, 1, db);
		}
		INode getRoot() override
		{
			if (root == -1)
				return allocate();
			return read(root);
		}
	public:
		NaiveDB(const char *loc)
		{
			//ע�⵽a+��д��ֻ�����ļ�ĩβ��fseek���ã��ʲ���a+
			//������r+��r+���������д��������ʱ���ᴴ��
			db = fopen(loc, "rb+");
			if (!db)
				db = fopen(loc, "wb+");
			else
			{
				fseek(db, 0, SEEK_SET);
				fread(&root, offset, 1, db);
				fseek(db, 0, SEEK_END);
				pos = (ftell(db) - offset) / nodeSize;
			}
			//fwrite(&root, offset, 1, db);
			bt = new BTree(this);
		}
		~NaiveDB()
		{
			int root = bt->getRootIndex();
			fseek(db, 0, SEEK_SET);
			fwrite(&root, offset, 1, db);
			fclose(db);
		}
	};
}