#pragma once
#include "List.h"
#include <memory>
#include <limits>

namespace ds
{
	//我实在懒得定义类型了....反正斐波那契堆也不需要和标准库交互
	//注意template template parameter的语法,默认模板参数被忽略，必须手动指出
	//保证内存泄漏，没有拷贝控制，谁爱用谁用
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
			x->self = it; //不写在一行，因为x既被修改也被取用，未定义行为
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
		void cut(Node *x) //x被切掉并放到根
		{
			roots.splice(roots.end(), x->p->childs, x->self);
			x->p = nullptr;
			x->marked = false;
		}
		void cascadingCut(Node *y)
		{
			while (y->p)
			{
				if (!y->marked) //节点减值在cut(x)之后紧接一步cascadingCut(x->p)，如果x->没有标记那么这里标记上
				{
					y->marked = true;
					break;
				}
				Node *tmp = y->p;
				cut(y); //cur把y->p置为nullptr
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
		void link(Node *x, Node *y) //把y从根链表中移除并且加入x的孩子
		{
			y->marked = false;
			y->p = x;
			x->childs.splice(x->childs.end(), roots, y->self);
		}
	};
}
