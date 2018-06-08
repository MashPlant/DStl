#pragma once
#include <iostream>
#include "Algorithm.h"

namespace ds
{
	//本来是不想给B树做顺序统计的
	//然而不知为何，随机数据跑的比别的树还要快
	//看起来很有潜力，那就顺便做个顺序统计
	template<typename K, typename V, size_t BF>
	class BTree
	{
	public:
		typedef std::pair<K, V> value_type;
	private:
		struct Node
		{
			//之所以不用数组或者new，仅仅是为了避开析构函数，从而能用memmove来操纵类对象
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
				for (int i = 0; i < keyNum; ++i) //断言只有这一段范围的对象是有效的
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
		//把where孩子分成两个节点，都作为x的孩子
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
			shiftBy(x->child + where + 1, x->keyNum - where, 1); //注意child本身keyNum多一个
			x->child[where + 1] = z;
			shiftBy(x->values + where, x->keyNum - where, 1);
			new(x->values + where) value_type(y->values[BF - 1]);

			const int moved = getSize(z);
			y->size -= moved + 1, z->size += moved;  //y失去了一堆孩子和一个key
			++x->keyNum;
		}
		void insertEmpty(Node *x, const K& key, const V& val)
		{
			while (true)
			{
				int i = ds::lower_bound(x->values, x->values + x->keyNum, key, pairComp) - x->values;
				if (i != x->keyNum && !(key<x->values[i].first))//重复插入
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
					else if (!(key < x->key(i))) //藏得很深啊
						return;
				}
				x = x->child[i];
			}
		}

		void merge(Node *x,int i) //将x的i孩子与i+1孩子合并，用x的i键作为分隔，这两个孩子都只有BF-1个孩子，合并后有2*BF-1个
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
			z->keyNum = 0; //析构函数里依赖keyNum来析构
			delete z;
		}
		void erase(Node *x, const K& key)
		{
			int i = ds::lower_bound(x->values, x->values + x->keyNum, key, pairComp) - x->values;
			if (i != x->keyNum && !(key<x->values[i].first)) //找到key了
			{
				if (x->isLeaf) //x是叶节点，直接删除
				{
					(x->values + i)->~value_type();
					shiftBy(x->values + i + 1, --x->keyNum - i, -1); //需要移动的内存是x->keyNum-i-1
					while (x)
						--x->size, x = x->p;
				}
				else
				{
					if (x->child[i]->keyNum >= BF) //前驱所在孩子有足够的孩子(以应对它的孩子的需求)
					{
						Node *y = x->child[i];
						while (!y->isLeaf)
							y = y->child[y->keyNum]; //找前驱
						x->values[i] = y->values[y->keyNum - 1]; //用等号！这里就是要释放x->values + i的资源
						erase(x->child[i], x->key(i)); //不能从y开始删!!!不一定有足够的孩子!!!
						//注意!!!不可以直接传y->key(y->keyNum - 1)
						//这个函数的key是传引用的，引用是由指针实现的，它指向的地址的内存被修改后，即使是常量引用也会变!!!!
						//调了一个小时才发现......
					}
					else if (x->child[i + 1]->keyNum >= BF) //后继所在孩子有足够的孩子
					{
						Node *y = x->child[i + 1];
						while (!y->isLeaf)
							y = y->child[0]; //找后继
						x->values[i] = y->values[0];
						erase(x->child[i + 1], x->key(i));
					}
					else //都没有,那么把这两个节点都合并到y中，并且挪动x的孩子和键
					{
						merge(x, i);
						if (root->keyNum == 0) //keyNum==0只是没有键了，但是还可能有一个孩子，这时根变成这个孩子
							root = x->child[i], root->p = nullptr;
						erase(x->child[i], key);
					}
				}
			}
			else if (!x->isLeaf)	//没有找到key,只要保证x->child[i]->keyNum足够多即可无脑递归，然而很难保证
			{
				if (x->child[i]->keyNum == BF - 1)
				{
					Node *y = x->child[i];
					if (i >= 1 && x->child[i - 1]->keyNum >= BF) //左兄弟，取走它的最大孩子
					{
						//找相邻的兄弟借节点，类似旋转操作,把x的一个键移入要删的key所在孩子，把它的兄弟的一个key和孩子移入x
						//但是从左还是右借并不完全一样，所以不能一概处理
						Node *z = x->child[i - 1];
						shiftBy(y->values, y->keyNum, 1);
						//是否需要考虑析构的问题？z的keyNum已经减了，不可能再去析构z->values[z->keyNum - 1]了
						//所以，value的构造必须要用new不能用=，从而避开=的资源释放
						//但是value的移动似乎应该是bitwise的，考虑std::move
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
					else if (i < x->keyNum && x->child[i + 1]->keyNum >= BF) //右兄弟,取走它的最小孩子
					{
						Node *z = x->child[i + 1];
						new(y->values + y->keyNum) value_type(std::move(x->values[i]));
						new(x->values + i) value_type(std::move(z->values[0]));
						if (!y->isLeaf) //y和z深度一样，isLeaf情况相同
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
					else //两个兄弟都没有节点借,那么将它与随便左右哪个兄弟合并，然而还是要特判一下
					{
						if (i != 0) --i;//i==0时，y与y+1合并仍放于y；否则y与y-1合并放于y-1
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
			//沿路向下分裂满节点,每次分裂成左右一半，孩子的中间key留在父亲节点中用于分隔两个新孩子
			//insertEmpty只保证了当前节点有空间(来容纳它的孩子的分裂)，不保证key需要去的孩子节点也有空间
			if (root->keyNum == 2 * BF - 1)
			{
				Node *x = new Node;
				x->isLeaf = false, x->child[0] = root, x->size = root->size; //+1操作由insertEmpty来做
				root->p = x, root = x;
				split(x, 0); //split接受参数：node的满子节点下标
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
					x = x->child[x->keyNum];//child本身就是[0,keyNum]的，比key多一个
				else if (!(key < lb->first)) //lower_bound保证*lb>=key,只需key>=*lb,即!(key<*lb)
					return &(lb->second);
				else x = x->child[lb - x->values]; //键的下标i,则i对应比键小的孩子，i+1对应比键大的，这里需要比键小的
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
			while (true) //有上面的检测，下面一定可以顺利返回
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
		BTree(const BTree &rhs) = delete; //实在是懒得写了...
		BTree &operator=(const BTree &rhs) = delete;
		~BTree() { clear(root); }
	};
}
