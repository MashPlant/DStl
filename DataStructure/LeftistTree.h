#pragma once
#include <utility>
namespace ds
{
	//保证内存泄漏，没有拷贝控制，谁爱用谁用
	template <typename K>
	class LeftistTree
	{
	private:
		struct Node
		{
			K key;
			int dist = 0;
			Node *left = nullptr, *right = nullptr;
			Node (const K &key):key(key){}
		};
		Node *root = nullptr;
		LeftistTree(Node *root) :root(root) {}
		static int dist(Node *x) { return x ? x->dist : -1; }
		static Node *merge(Node *x,Node *y)
		{
			if (!x) return y;
			if (!y) return x;
			if (x->key > y->key) //小根
				std::swap(x, y);
			x->right = merge(x->right, y);
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
		void push(const K&key)
		{
			root = merge(root, new Node(key));
		}
		bool empty()const { return !root; }
	};
}