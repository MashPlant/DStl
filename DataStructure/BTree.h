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
			//֮���Բ����������new��������Ϊ�˱ܿ�pair�������������Ӷ�����memmove�����������
			value_type *values = (value_type *)malloc((2 * BF - 1) * sizeof(value_type));
			Node *child[2 * BF] = { nullptr };
			int cnt = 0;
			bool isLeaf = true;
			const K &key(int i)const { return values[i].first; }
			V &val(int i)const { return values[i].second; }
			~Node()
			{
				for (int i = 0; i < cnt; ++i) //����ֻ����һ�η�Χ�Ķ�������Ч��
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
		//��where���ӷֳ������ڵ㣬����Ϊx�ĺ���
		void split(Node *x, int where)
		{
			Node *z = new Node;
			Node *y = x->child[where];
			z->isLeaf = y->isLeaf;
			memmove(z->values, y->values + BF, (BF - 1) * sizeof(value_type));
			memmove(z->child, y->child + BF, BF * sizeof(Node *));
			z->cnt = y->cnt = BF - 1;
			shiftBy(x->child + where + 1, x->cnt - where, 1); //ע��child����cnt��һ��
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
				if (i != x->cnt && !(key<x->values[i].first))//�ظ�����
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
					else if (!(key < x->key(i))) //�صú��
						return;
				}
				x = x->child[i];
			}
		}

		void merge(Node *x,int i) //��x��i������i+1���Ӻϲ�����x��i����Ϊ�ָ������������Ӷ�ֻ��BF-1�����ӣ��ϲ�����2*BF-1��
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
			if (i != x->cnt && !(key<x->values[i].first)) //�ҵ�key��
			{
				if (x->isLeaf) //x��Ҷ�ڵ㣬ֱ��ɾ��
				{
					(x->values + i)->~value_type();
					shiftBy(x->values + i + 1, --x->cnt - i, -1); //��Ҫ�ƶ����ڴ���x->cnt-i-1
				}
				else
				{
					if (x->child[i]->cnt >= BF) //ǰ�����ں������㹻�ĺ���(��Ӧ�����ĺ��ӵ�����)
					{
						Node *y = x->child[i];
						while (!y->isLeaf)
							y = y->child[y->cnt]; //��ǰ��
						x->values[i] = y->values[y->cnt - 1]; //�����õȺţ��������Ҫ�ͷ�x->values + i����Դ
						erase(x->child[i], x->key(i)); //���ܴ�y��ʼɾ!!!��һ�����㹻�ĺ���!!!
						//ע��!!!������ֱ�Ӵ�y->key(y->cnt - 1)
						//���������key�Ǵ����õģ���������ָ��ʵ�ֵģ���ָ��ĵ�ַ���ڴ汻�޸ĺ󣬼�ʹ�ǳ�������Ҳ���!!!!
						//����һ��Сʱ�ŷ���......
					}
					else if (x->child[i + 1]->cnt >= BF) //������ں������㹻�ĺ���
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
						if (root->cnt == 0) //cnt==0ֻ��û�м��ˣ����ǻ�������һ�����ӣ���ʱ������������
							root = x->child[i];
						erase(x->child[i], key);
					}
				}
			}
			else if (!x->isLeaf)	//û���ҵ�key,ֻҪ��֤x->child[i]->cnt�㹻�༴�����Եݹ飬Ȼ�����ѱ�֤
			{
				if (x->child[i]->cnt == BF - 1)
				{
					Node *y = x->child[i];
					if (i >= 1 && x->child[i - 1]->cnt >= BF) //���ֵܣ�ȡ�����������
					{
						//�����ڵ��ֵܽ�ڵ㣬������ת����,��x��һ��������Ҫɾ��key���ں��ӣ��������ֵܵ�һ��key�ͺ�������x
						//���Ǵ������ҽ貢����ȫһ�������Բ���һ�Ŵ���
						Node *z = x->child[i - 1];
						shiftBy(y->values, y->cnt, 1);
						//�Ƿ���Ҫ�������������⣿z��cnt�Ѿ����ˣ���������ȥ����z->values[z->cnt - 1]��
						//���ԣ�value�Ĺ������Ҫ��new������=���Ӷ��ܿ�=����Դ�ͷ�
						//����value���ƶ��ƺ�Ӧ����bitwise�ģ�����std::move
						new(y->values) value_type(std::move(x->values[i - 1]));
						new(x->values + i - 1) value_type(std::move(z->values[z->cnt - 1]));
						shiftBy(y->child, y->cnt + 1, 1);
						y->child[0] = z->child[z->cnt];
						--z->cnt, ++y->cnt;
						erase(y, key);
					}
					else if (i < x->cnt&&x->child[i + 1]->cnt >= BF) //���ֵ�,ȡ��������С����
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
					else //�����ֵܶ�û�нڵ��,��ô��������������ĸ��ֵܺϲ���Ȼ������Ҫ����һ��
					{
						if (i != 0) --i;//i==0ʱ��y��y+1�ϲ��Է���y������y��y-1�ϲ�����y-1
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
			//��·���·������ڵ�,ÿ�η��ѳ�����һ�룬���ӵ��м�key���ڸ��׽ڵ������ڷָ������º���
			//insertEmptyֻ��֤�˵�ǰ�ڵ��пռ�(���������ĺ��ӵķ���)������֤key��Ҫȥ�ĺ��ӽڵ�Ҳ�пռ�
			if (root->cnt == 2 * BF - 1)
			{
				Node *x = new Node, *tmp = root;
				root = x;
				x->isLeaf = false, x->child[0] = tmp;
				split(x, 0); //split���ܲ�����node�����ӽڵ��±�
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
					x = x->child[x->cnt];//child�������[0,cnt]�ģ���key��һ��
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
		BTree() :root(new Node) {}
		BTree(const BTree &rhs) = delete; //ʵ��������д��...
		BTree &operator=(const BTree &rhs) = delete;
		~BTree() { clear(root); }
	};

}
