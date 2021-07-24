#pragma once
#include <utility>
namespace ds
{
	//保证内存泄漏，没有拷贝控制，谁爱用谁用
	template <typename K>
	class LeftistTree
	{
	public:
		struct Node
		{
		private:
			K key;
			int dist = 0;
			Node *p = nullptr;
			Node *left = nullptr, *right = nullptr;
			Node(const K &key) :key(key) {}
		public:
			const K &getKey()const { return key; }
			friend LeftistTree;
		};
	private:
		Node *root = nullptr;
		LeftistTree(Node *root) :root(root) {}
		static int dist(Node *x) { return x ? x->dist : -1; }
		static Node *merge(Node *x,Node *y)
		{
			if (!x) return y;
			if (!y) return x;
			if (x->key > y->key) //小根
				std::swap(x, y);
			Node *tmp = merge(x->right, y);
			x->right = tmp, tmp->p = x;
			if (dist(x->right) > dist(x->left))
				std::swap(x->left, x->right);
			x->dist = dist(x->right) + 1;
			return x;
		}
	public:
		LeftistTree() :root(nullptr) {}
		LeftistTree(const K &key) :root(new Node(key)) {}
		void merge(LeftistTree &rhs)
		{
			if (this != &rhs)
			{
				root = merge(root, rhs.root);
				rhs.root = nullptr;
			}
		}
		//请自觉保证树非空
		const K &top()const { return root->key; }
		void pop()
		{
			root = merge(root->left, root->right);
		}
		Node *push(const K&key)
		{
			Node *x = new Node(key);
			root = merge(root, x);
			return x;
		}
		void erase(Node *x)
		{
			Node *tmp = merge(x->left, x->right); 
			if (!x->p) { root = tmp; return; }
			if (tmp) tmp->p = x->p;//本身x的两个孩子就是nullptr，所以tmp可能是nullptr
			if (x->p->right == x) x->p->right = tmp;
			else x->p->left = tmp;
			x = tmp;
			while (x && x->p)
			{
				if (dist(x->p->right) > dist(x->p->left))
					std::swap(x->p->left, x->p->right);
				if (dist(x->p)==dist(x->p->right)+1)
					break;
				x->p->dist = dist(x->p->right) + 1;
				x = x->p;
			}
		}
		bool empty()const { return !root; }
	};
}