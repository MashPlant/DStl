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
				//原版的B树之所以不用数组，是为了避开数组析构时元素自动析构
				//然而pair本身没有内存管理，只是简单的自动析构掉它的first和second
				//又限定K,V都不能管理内存，那么bitwise操作之后的副本是可以被重复析构的
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
					if (i != x->cnt && !(key<x->values[i].first))//重复插入
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
						else if (!(key < x->key(i))) //藏得很深啊
							return;
					}
					ix = iy;
				}
			}

			void merge(INode ix, INode iy, INode iz, int i)
			{
				//assert iy是ix的第i个孩子，iz是ix的第i+1个孩子
				//将x的i孩子与i+1孩子合并，用x的i键作为分隔，这两个孩子都只有BF-1个孩子，合并后有2*BF-1个
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
				container->write(iz); //其实z已经没用了,应该把z节点整个删掉的，但是懒得做了
			}
			void erase(INode ix, const K& key)
			{
				pointer x = ix.first;
				int i = ds::lower_bound(x->values, x->values + x->cnt, key, pairComp) - x->values;
				if (i != x->cnt && !(key<x->values[i].first)) //找到key了
				{
					if (x->isLeaf) //x是叶节点，直接删除
					{
						(x->values + i)->~value_type();
						shiftBy(x->values + i + 1, --x->cnt - i, -1); //需要移动的内存是x->cnt-i-1
					}
					else
					{
						INode iy, iz;
						if (iy = container->read(x->child[i]), iy.first->cnt >= BF) //前驱所在孩子有足够的孩子(以应对它的孩子的需求)
						{
							pointer y = iy.first;
							while (!y->isLeaf)
								y = container->read(y->child[y->cnt]).first; //找前驱
							x->values[i] = y->values[y->cnt - 1];
							erase(iy, x->key(i));
						}
						else if (iz = container->read(x->child[i + 1]), iz.first->cnt >= BF) //后继所在孩子有足够的孩子
						{
							pointer z = iz.first;
							while (!z->isLeaf)
								z = container->read(z->child[0]).first; //找后继
							x->values[i] = z->values[0];
							erase(iz, x->key(i));
						}
						else //都没有,那么把这两个节点都合并到y中，并且挪动x的孩子和键
						{
							merge(ix, iy, iz, i);
							if (root.first->cnt == 0)
								root = iy;
							erase(iy, key);
						}
					}
				}
				else if (!x->isLeaf)	//没有找到key
				{
					INode iy = container->read(x->child[i]), pre, nxt;
					pointer y = iy.first, z;
					if (y->cnt == BF - 1)
					{
						if (i >= 1 && (pre = container->read(x->child[i - 1]), pre.first->cnt >= BF)) //左兄弟，取走它的最大孩子
						{
							z = pre.first;

							shiftBy(y->values, y->cnt, 1);
							new(y->values) value_type(std::move(x->values[i - 1]));
							new(x->values + i - 1) value_type(std::move(z->values[z->cnt - 1]));
							shiftBy(y->child, y->cnt + 1, 1);
							y->child[0] = z->child[z->cnt];
							--z->cnt, ++y->cnt;

							container->write(pre); //x和y会在函数的末尾被write
							erase(iy, key);
						}
						else if (i < x->cnt && (nxt = container->read(x->child[i + 1]), nxt.first->cnt >= BF)) //右兄弟,取走它的最小孩子
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
						else //两个兄弟都没有节点借,那么将它与随便左右哪个兄弟合并
						{
							//此时pre和nxt(如果存在)都已经读好了
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
			std::shared_ptr<V> find(const K& key) //原来的版本是返回V*，然而注意到中间节点(在操作文件时)会被析构，返回的普通指针毫无意义
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
			BTree(const BTree &rhs) = delete; //实在是懒得写了...
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
		virtual INode getRoot() = 0; //初始化时调用,对于内存DB直接allocate，对于文件DB则不可
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
			arr = new pointer[dbSize];  //arr=new...放在后面就挂了,目前我还不知道为什么
			bt = new BTree(this);
		}
		~MemoryNaiveDB() { delete []arr; }
	private:
		//Node **arr=new Node*[100000]; 不要混用智能指针和裸指针!!!!!
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
	class NaiveDB : public BaseNaiveDB<K, V, BF> //必须保存root的位置,那么root写在第一个int，最终存入文件时需要更新
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
			//注意到a+的写入只能在文件末尾，fseek无用，故不用a+
			//这里用r+，r+可以随机读写，不存在时不会创建
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