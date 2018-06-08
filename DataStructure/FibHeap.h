#pragma once
#include "List.h"
#include <memory>
#include <limits>

namespace ds
{
	//��ʵ�����ö���������....����쳲�������Ҳ����Ҫ�ͱ�׼�⽻��
	//ע��template template parameter���﷨,Ĭ��ģ����������ԣ������ֶ�ָ��
	//��֤�ڴ�й©��û�п������ƣ�˭����˭��
	template<typename K, template <typename T,typename=std::allocator<T>> typename Container = List>
	class FibHeap
	{
	public:
		struct Node
		{
		private:
			K key;
			Node *p = nullptr;
			typename Container<Node *>::iterator self;
			Container<Node*> childs;
			bool marked = false;
			explicit Node(const K & key) :key(key) {}
		public:
			friend FibHeap;
			int degree()const { return childs.size(); }
			const K& getKey() const { return key; }
		};
		typedef typename Container<Node *>::iterator iterator;
		Node* push(const K &key)
		{
			Node *x = new Node(key);
			iterator it = roots.insert(roots.end(), x);
			x->self = it; //��д��һ�У���Ϊx�ȱ��޸�Ҳ��ȡ�ã�δ������Ϊ
			if (!min || key < min->key)
				min = x;
			return x;
		}
		void decreaseKey(Node *x, const K &key)
		{
			x->key = key;
			Node *y = x->p;
			if (y && x->key < y->key)
			{
				cut(x);
				cascadingCut(y);
			}
			if (x->key < min->key)
				min = x;
		}
		void delNode(Node *x)
		{
			decreaseKey(x, std::numeric_limits<K>::min());
			pop();
		}
		const K &top() const { return min->getKey(); }
		void pop()
		{
			for (auto it : min->childs)
				it->p = nullptr;
			roots.splice(roots.end(), min->childs);
			roots.erase(min->self);
			popFix();
		}
		void merge(FibHeap &src)
		{
			roots.splice(roots.end(),src.roots);
			if (!min || (src.min&&src.min->key < min->key))
				min = src.min;
			src.min = nullptr;
		}
		bool empty()const{return roots.empty();}
	private:
		Node *min = nullptr;
		Container<Node *> roots;
		void cut(Node *x) //x���е����ŵ���
		{
			roots.splice(roots.end(), x->p->childs, x->self);
			x->p = nullptr;
			x->marked = false;
		}
		void cascadingCut(Node *y)
		{
			while (y->p)
			{
				if (!y->marked) //�ڵ��ֵ��cut(x)֮�����һ��cascadingCut(x->p)�����x->û�б����ô��������
				{
					y->marked = true;
					break;
				}
				Node *tmp = y->p;
				cut(y); //cur��y->p��Ϊnullptr
				y = tmp;
			}
		}
		Node *aux[32];
		void popFix()
		{
			memset(aux, 0, sizeof(aux));		
			for (auto it = roots.begin(); it != roots.end();)
			{
				Node *x = *it;
				++it;
				while (Node *y = aux[x->degree()])
				{
					aux[x->degree()] = nullptr;
					if (y->key < x->key)
						link(y, x), x = y;
					else
						link(x, y);
				}
				aux[x->degree()] = x;
			}
			roots.clear();
			min = nullptr;
			for (int i = 0; i < 32; ++i)
				if (aux[i])
				{
					iterator it= roots.insert(roots.end(), aux[i]);
					aux[i]->self = it;
					if (!min || aux[i]->key < min->key)
						min = aux[i];
				}
		}
		void link(Node *x, Node *y) //��y�Ӹ��������Ƴ����Ҽ���x�ĺ���
		{
			y->marked = false;
			y->p = x;
			x->childs.splice(x->childs.end(), roots, y->self);
		}
	};
}
