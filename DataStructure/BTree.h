#pragma once
#include <iostream>
#include "Algorithm.h"

namespace ds
{
	//�����ǲ����B����˳��ͳ�Ƶ�
	//Ȼ����֪Ϊ�Σ���������ܵıȱ������Ҫ��
	//����������Ǳ�����Ǿ�˳������˳��ͳ��
	template<typename K, typename V, size_t BF>
	class BTree
	{
	public:
		typedef std::pair<K, V> value_type;
	private:
		struct Node
		{
			//֮���Բ����������new��������Ϊ�˱ܿ������������Ӷ�����memmove�����������
			value_type *values = (value_type *)malloc((2 * BF - 1) * sizeof(value_type));
			Node *child[2 * BF] = { nullptr };
			Node *p = nullptr;
			int keyNum = 0, size = 0;
			bool isLeaf = true;
			const K &key(int i)const { return values[i].first; }
			V &val(int i)const { return values[i].second; }
			Node(Node *p = nullptr) :p(p) {}
			~Node()
			{
				for (int i = 0; i < keyNum; ++i) //����ֻ����һ�η�Χ�Ķ�������Ч��
					(values + i)->~value_type();
				free(values);
			}
		};
		Node *root = nullptr;
		static bool pairComp(const value_type &lhs, const K&rhs) { return lhs.first < rhs; }
		template <typename T>
		static void shiftBy(T *ptr, int length, int shift)
		{
			memmove(ptr + shift, ptr, length * sizeof(T));
		}
		static int getSize(Node *x)
		{
			if (!x)
				return 0;
			if (x->isLeaf)
				return x->keyNum;
			int nsz = x->keyNum; //x
			for (int i = 0; i <= x->keyNum; ++i)
				nsz += x->child[i]->size;
			return nsz;
		}
		//��where���ӷֳ������ڵ㣬����Ϊx�ĺ���
		void split(Node *x, int where)
		{
			Node *z = new Node(x);
			Node *y = x->child[where];
			z->isLeaf = y->isLeaf;
			memmove(z->values, y->values + BF, (BF - 1) * sizeof(value_type));
			if (!y->isLeaf)
			{
				memmove(z->child, y->child + BF, BF * sizeof(Node *));
				for (int i = 0; i < BF; ++i)
					z->child[i]->p = z;
			}
			z->keyNum = y->keyNum = BF - 1;
			shiftBy(x->child + where + 1, x->keyNum - where, 1); //ע��child����keyNum��һ��
			x->child[where + 1] = z;
			shiftBy(x->values + where, x->keyNum - where, 1);
			new(x->values + where) value_type(y->values[BF - 1]);

			const int moved = getSize(z);
			y->size -= moved + 1, z->size += moved;  //yʧȥ��һ�Ѻ��Ӻ�һ��key
			++x->keyNum;
		}
		void insertEmpty(Node *x, const K& key, const V& val)
		{
			while (true)
			{
				int i = ds::lower_bound(x->values, x->values + x->keyNum, key, pairComp) - x->values;
				if (i != x->keyNum && !(key<x->values[i].first))//�ظ�����
					return;
				if (x->isLeaf)
				{
					shiftBy(x->values + i, x->keyNum - i, 1);
					new(x->values + i) value_type({ key,val });
					++x->keyNum;
					while (x)
						++x->size, x = x->p;
					break;
				}
				if (x->child[i]->keyNum == 2 * BF - 1)
				{
					split(x, i);
					if (x->key(i) < key) ++i;
					else if (!(key < x->key(i))) //�صú��
						return;
				}
				x = x->child[i];
			}
		}

		void merge(Node *x,int i) //��x��i������i+1���Ӻϲ�����x��i����Ϊ�ָ������������Ӷ�ֻ��BF-1�����ӣ��ϲ�����2*BF-1��
		{
			Node *y = x->child[i], *z = x->child[i + 1];
			y->keyNum = 2 * BF - 1;
			new (y->values + BF - 1) value_type(std::move(x->values[i]));
			memmove(y->values + BF, z->values, (BF - 1) * sizeof(value_type));
			if (!y->isLeaf)
			{
				memmove(y->child + BF, z->child, BF * sizeof(Node *));
				for (int j = BF; j <= 2 * BF - 1; ++j)
					y->child[j]->p = y;
			}
			shiftBy(x->values + i + 1, x->keyNum - i - 1, -1);
			shiftBy(x->child + i + 2, x->keyNum - i - 1, -1);
			
			--x->keyNum;
			y->size = getSize(y);
			z->keyNum = 0; //��������������keyNum������
			delete z;
		}
		void erase(Node *x, const K& key)
		{
			int i = ds::lower_bound(x->values, x->values + x->keyNum, key, pairComp) - x->values;
			if (i != x->keyNum && !(key<x->values[i].first)) //�ҵ�key��
			{
				if (x->isLeaf) //x��Ҷ�ڵ㣬ֱ��ɾ��
				{
					(x->values + i)->~value_type();
					shiftBy(x->values + i + 1, --x->keyNum - i, -1); //��Ҫ�ƶ����ڴ���x->keyNum-i-1
					while (x)
						--x->size, x = x->p;
				}
				else
				{
					if (x->child[i]->keyNum >= BF) //ǰ�����ں������㹻�ĺ���(��Ӧ�����ĺ��ӵ�����)
					{
						Node *y = x->child[i];
						while (!y->isLeaf)
							y = y->child[y->keyNum]; //��ǰ��
						x->values[i] = y->values[y->keyNum - 1]; //�õȺţ��������Ҫ�ͷ�x->values + i����Դ
						erase(x->child[i], x->key(i)); //���ܴ�y��ʼɾ!!!��һ�����㹻�ĺ���!!!
						//ע��!!!������ֱ�Ӵ�y->key(y->keyNum - 1)
						//���������key�Ǵ����õģ���������ָ��ʵ�ֵģ���ָ��ĵ�ַ���ڴ汻�޸ĺ󣬼�ʹ�ǳ�������Ҳ���!!!!
						//����һ��Сʱ�ŷ���......
					}
					else if (x->child[i + 1]->keyNum >= BF) //������ں������㹻�ĺ���
					{
						Node *y = x->child[i + 1];
						while (!y->isLeaf)
							y = y->child[0]; //�Һ��
						x->values[i] = y->values[0];
						erase(x->child[i + 1], x->key(i));
					}
					else //��û��,��ô���������ڵ㶼�ϲ���y�У�����Ų��x�ĺ��Ӻͼ�
					{
						merge(x, i);
						if (root->keyNum == 0) //keyNum==0ֻ��û�м��ˣ����ǻ�������һ�����ӣ���ʱ������������
							root = x->child[i], root->p = nullptr;
						erase(x->child[i], key);
					}
				}
			}
			else if (!x->isLeaf)	//û���ҵ�key,ֻҪ��֤x->child[i]->keyNum�㹻�༴�����Եݹ飬Ȼ�����ѱ�֤
			{
				if (x->child[i]->keyNum == BF - 1)
				{
					Node *y = x->child[i];
					if (i >= 1 && x->child[i - 1]->keyNum >= BF) //���ֵܣ�ȡ�����������
					{
						//�����ڵ��ֵܽ�ڵ㣬������ת����,��x��һ��������Ҫɾ��key���ں��ӣ��������ֵܵ�һ��key�ͺ�������x
						//���Ǵ������ҽ貢����ȫһ�������Բ���һ�Ŵ���
						Node *z = x->child[i - 1];
						shiftBy(y->values, y->keyNum, 1);
						//�Ƿ���Ҫ�������������⣿z��keyNum�Ѿ����ˣ���������ȥ����z->values[z->keyNum - 1]��
						//���ԣ�value�Ĺ������Ҫ��new������=���Ӷ��ܿ�=����Դ�ͷ�
						//����value���ƶ��ƺ�Ӧ����bitwise�ģ�����std::move
						new(y->values) value_type(std::move(x->values[i - 1]));
						new(x->values + i - 1) value_type(std::move(z->values[z->keyNum - 1]));
						if (!y->isLeaf)
						{
							shiftBy(y->child, y->keyNum + 1, 1);
							y->child[0] = z->child[z->keyNum], y->child[0]->p = y;
						}
						
						--z->keyNum, ++y->keyNum;
						const int moved = getSize(y->child[0]) + 1;
						y->size += moved, z->size -= moved;

						erase(y, key);
					}
					else if (i < x->keyNum && x->child[i + 1]->keyNum >= BF) //���ֵ�,ȡ��������С����
					{
						Node *z = x->child[i + 1];
						new(y->values + y->keyNum) value_type(std::move(x->values[i]));
						new(x->values + i) value_type(std::move(z->values[0]));
						if (!y->isLeaf) //y��z���һ����isLeaf�����ͬ
						{
							y->child[y->keyNum + 1] = z->child[0], y->child[y->keyNum + 1]->p = y;
							shiftBy(z->child + 1, z->keyNum, -1);
						}
						shiftBy(z->values + 1, z->keyNum - 1, -1);
						
						--z->keyNum, ++y->keyNum;
						const int moved = getSize(y->child[y->keyNum]) + 1;
						y->size += moved, z->size -= moved;

						erase(y, key);
					}
					else //�����ֵܶ�û�нڵ��,��ô��������������ĸ��ֵܺϲ���Ȼ������Ҫ����һ��
					{
						if (i != 0) --i;//i==0ʱ��y��y+1�ϲ��Է���y������y��y-1�ϲ�����y-1
						y = x->child[i];
						merge(x, i);
						if (root->keyNum == 0)
							root = y, root->p = nullptr;
						erase(y, key);
					}
				}
				else 
					erase(x->child[i], key);
			}
		}
		template <typename Pred>
		void inorderWalk(Node *x,Pred pred)
		{
			const bool nl = !(x->isLeaf);
			for (int i = 0; i < x->keyNum; ++i)
			{
				if (nl) inorderWalk(x->child[i], pred);
				pred(x->key(i), x->val(i));
			}
			if (nl) inorderWalk(x->child[x->keyNum], pred);
		}
		void clear(Node *x)
		{
			if (x && !x->isLeaf)
				for (int i = 0; i <= x->keyNum; ++i)
					clear(x->child[i]);
			delete x;
		}
		void check(Node *x)
		{
			if (x->isLeaf)
				return;
			for (int i = 0; i <= x->keyNum; ++i)
			{
				if (getSize(x) != x->size)
					int a = 0;
				check(x->child[i]);
			}
		}
	public:
		template <typename Pred>
		void inorderWalk(Pred pred) { inorderWalk(root, pred); }
		void insert(const K& key, const V& val)
		{
			//��·���·������ڵ�,ÿ�η��ѳ�����һ�룬���ӵ��м�key���ڸ��׽ڵ������ڷָ������º���
			//insertEmptyֻ��֤�˵�ǰ�ڵ��пռ�(���������ĺ��ӵķ���)������֤key��Ҫȥ�ĺ��ӽڵ�Ҳ�пռ�
			if (root->keyNum == 2 * BF - 1)
			{
				Node *x = new Node;
				x->isLeaf = false, x->child[0] = root, x->size = root->size; //+1������insertEmpty����
				root->p = x, root = x;
				split(x, 0); //split���ܲ�����node�����ӽڵ��±�
			}
			insertEmpty(root, key, val);
		}
		void insert(const value_type &kvpair){ insert(kvpair.first, kvpair.second); }
		void erase(const K& key) { erase(root, key); }
		V* find(const K& key)
		{
			Node *x = root;
			while (x)
			{
				value_type* lb = ds::lower_bound(x->values, x->values + x->keyNum, key, pairComp);
				if (lb == x->values + x->keyNum)
					x = x->child[x->keyNum];//child�������[0,keyNum]�ģ���key��һ��
				else if (!(key < lb->first)) //lower_bound��֤*lb>=key,ֻ��key>=*lb,��!(key<*lb)
					return &(lb->second);
				else x = x->child[lb - x->values]; //�����±�i,��i��Ӧ�ȼ�С�ĺ��ӣ�i+1��Ӧ�ȼ���ģ�������Ҫ�ȼ�С��
			}
			return nullptr;
		}
		void clear()
		{
			clear(root);
			root = new Node;
		}
		value_type *kth(int k)
		{
			if (k < 1 || k > size())
				return nullptr;
			Node *x = root;
			while (true) //������ļ�⣬����һ������˳������
			{
				if (x->isLeaf)
					return &(x->values[k - 1]);
				for (int i = 0;i <= x->keyNum;++i)
				{
					const int csz = x->child[i]->size + 1;
					if (k == csz)
						return &(x->values[i]);
					if (k < csz)
					{
						x = x->child[i];
						break;
					}
					k -= csz;
				}
			}
		}
		value_type *begin() const
		{
			Node *x = root;
			while (x && x->child[0])
				x = x->child[0];
			return x->values;
		}
		value_type *end()const { return nullptr; }
		int size()const { return root->size; }
		bool empty() const { return root->keyNum == 0; }
		BTree() :root(new Node) {}
		BTree(const BTree &rhs) = delete; //ʵ��������д��...
		BTree &operator=(const BTree &rhs) = delete;
		~BTree() { clear(root); }
	};
}
