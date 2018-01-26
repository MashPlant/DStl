#pragma once
#include <iostream>
#include "Algorithm.h"

namespace ds
{
	template<typename K, typename V, size_t BF>
	class BTree
	{
	public:
		typedef std::pair<K, V> value_type;
	private:
		struct Node
		{
			//之所以不用数组或者new，仅仅是为了避开pair的析构函数，从而能用memmove来操纵类对象
			value_type *values = (value_type *)malloc((2 * BF - 1) * sizeof(value_type));
			Node *child[2 * BF] = { nullptr };
			int cnt = 0;
			bool isLeaf = true;
			const K &key(int i)const { return values[i].first; }
			V &val(int i)const { return values[i].second; }
			~Node()
			{
				for (int i = 0; i < cnt; ++i) //断言只有这一段范围的对象是有效的
					(values + i)->~value_type();
				free(values);
			}
		};
		Node *root = nullptr;
		std::function<bool(const value_type &lhs, const K&rhs)> pairComp = [](const value_type &lhs, const K&rhs) {return lhs.first < rhs; };
		Node* allocate()
		{
			return new Node;
		}
		template <typename T>
		static void shiftBy(T *ptr, int length, int shift)
		{
			memmove(ptr + shift, ptr, length * sizeof(T));
		}
		//把where孩子分成两个节点，都作为x的孩子
		void split(Node *x, int where)
		{
			Node *z = new Node;
			Node *y = x->child[where];
			z->isLeaf = y->isLeaf;
			memmove(z->values, y->values + BF, (BF - 1) * sizeof(value_type));
			memmove(z->child, y->child + BF, BF * sizeof(Node *));
			z->cnt = y->cnt = BF - 1;
			shiftBy(x->child + where + 1, x->cnt - where, 1); //注意child本身cnt多一个
			x->child[where + 1] = z;
			shiftBy(x->values + where, x->cnt - where, 1);
			new(x->values + where) value_type(y->values[BF - 1]);
			++x->cnt;
		}
		void insertEmpty(Node *x, const K& key, const V& val)
		{
			while (true)
			{
				int i = ds::lower_bound(x->values, x->values + x->cnt, key, pairComp) - x->values;
				if (i != x->cnt && !(key<x->values[i].first))//重复插入
					return;
				if (x->isLeaf)
				{
					shiftBy(x->values + i, x->cnt - i, 1);
					new(x->values + i) value_type({ key,val });
					++x->cnt;
					break;
				}
				if (x->child[i]->cnt == 2 * BF - 1)
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
			y->cnt = 2 * BF - 1;
			new (y->values + BF - 1) value_type(std::move(x->values[i]));
			memmove(y->values + BF, z->values, (BF - 1) * sizeof(value_type));
			memmove(y->child + BF, z->child, BF * sizeof(Node *));
			shiftBy(x->values + i + 1, x->cnt - i - 1, -1);
			shiftBy(x->child + i + 2, x->cnt - i - 1, -1);
			--x->cnt;
		}
		void erase(Node *x, const K& key)
		{
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
					if (x->child[i]->cnt >= BF) //前驱所在孩子有足够的孩子(以应对它的孩子的需求)
					{
						Node *y = x->child[i];
						while (!y->isLeaf)
							y = y->child[y->cnt]; //找前驱
						x->values[i] = y->values[y->cnt - 1]; //可以用等号！这里就是要释放x->values + i的资源
						erase(x->child[i], x->key(i)); //不能从y开始删!!!不一定有足够的孩子!!!
						//注意!!!不可以直接传y->key(y->cnt - 1)
						//这个函数的key是传引用的，引用是由指针实现的，它指向的地址的内存被修改后，即使是常量引用也会变!!!!
						//调了一个小时才发现......
					}
					else if (x->child[i + 1]->cnt >= BF) //后继所在孩子有足够的孩子
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
						if (root->cnt == 0) //cnt==0只是没有键了，但是还可能有一个孩子，这时根变成这个孩子
							root = x->child[i];
						erase(x->child[i], key);
					}
				}
			}
			else if (!x->isLeaf)	//没有找到key,只要保证x->child[i]->cnt足够多即可无脑递归，然而很难保证
			{
				if (x->child[i]->cnt == BF - 1)
				{
					Node *y = x->child[i];
					if (i >= 1 && x->child[i - 1]->cnt >= BF) //左兄弟，取走它的最大孩子
					{
						//找相邻的兄弟借节点，类似旋转操作,把x的一个键移入要删的key所在孩子，把它的兄弟的一个key和孩子移入x
						//但是从左还是右借并不完全一样，所以不能一概处理
						Node *z = x->child[i - 1];
						shiftBy(y->values, y->cnt, 1);
						//是否需要考虑析构的问题？z的cnt已经减了，不可能再去析构z->values[z->cnt - 1]了
						//所以，value的构造必须要用new不能用=，从而避开=的资源释放
						//但是value的移动似乎应该是bitwise的，考虑std::move
						new(y->values) value_type(std::move(x->values[i - 1]));
						new(x->values + i - 1) value_type(std::move(z->values[z->cnt - 1]));
						shiftBy(y->child, y->cnt + 1, 1);
						y->child[0] = z->child[z->cnt];
						--z->cnt, ++y->cnt;
						erase(y, key);
					}
					else if (i < x->cnt&&x->child[i + 1]->cnt >= BF) //右兄弟,取走它的最小孩子
					{
						Node *z = x->child[i + 1];
						new(y->values + y->cnt) value_type(std::move(x->values[i]));
						new(x->values + i) value_type(std::move(z->values[0]));
						y->child[y->cnt + 1] = z->child[0];
						shiftBy(z->values + 1, z->cnt - 1, -1);
						shiftBy(z->child + 1, z->cnt, -1);
						--z->cnt, ++y->cnt;
						erase(y, key);
					}
					else //两个兄弟都没有节点借,那么将它与随便左右哪个兄弟合并，然而还是要特判一下
					{
						if (i != 0) --i;//i==0时，y与y+1合并仍放于y；否则y与y-1合并放于y-1
						y = x->child[i];
						merge(x, i);
						if (root->cnt == 0)
							root = y;
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
			for (int i = 0; i < x->cnt; ++i)
			{
				if (nl) inorderWalk(x->child[i], pred);
				pred(x->key(i), x->val(i));
			}
			if (nl) inorderWalk(x->child[x->cnt], pred);
		}
		void clear(Node *x)
		{
			if (x && !x->isLeaf)
				for (int i = 0; i <= x->cnt; ++i)
					clear(x->child[i]);
			delete x;
		}
	public:
		template <typename Pred>
		void inorderWalk(Pred pred) { inorderWalk(root, pred); }
		void insert(const K& key, const V& val)
		{
			//沿路向下分裂满节点,每次分裂成左右一半，孩子的中间key留在父亲节点中用于分隔两个新孩子
			//insertEmpty只保证了当前节点有空间(来容纳它的孩子的分裂)，不保证key需要去的孩子节点也有空间
			if (root->cnt == 2 * BF - 1)
			{
				Node *x = new Node, *tmp = root;
				root = x;
				x->isLeaf = false, x->child[0] = tmp;
				split(x, 0); //split接受参数：node的满子节点下标
			}
			insertEmpty(root, key, val);
		}
		void insert(const value_type &kvpair) { insert(kvpair.first, kvpair.second); }
		void erase(const K& key) { erase(root, key); }
		V* find(const K& key)
		{
			Node *x = root;
			while (x)
			{
				value_type* lb = ds::lower_bound(x->values, x->values + x->cnt, key, pairComp);
				if (lb == x->values + x->cnt)
					x = x->child[x->cnt];//child本身就是[0,cnt]的，比key多一个
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
		BTree() :root(new Node) {}
		BTree(const BTree &rhs) = delete; //实在是懒得写了...
		BTree &operator=(const BTree &rhs) = delete;
		~BTree() { clear(root); }
	};

}
