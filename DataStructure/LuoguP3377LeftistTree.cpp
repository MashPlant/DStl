#include "stdafx.h"
#include <cstdio>
#include <utility>

/*template <typename K>
class LeftistTree
{
private:
	struct Node
	{
		K key;
		int dist = 0;
		Node *left = nullptr, *right = nullptr;
		Node(const K &key) :key(key) {}
	};
	Node *root = nullptr;
	LeftistTree(Node *root) :root(root) {}
	static int dist(Node *x) { return x ? x->dist : -1; }
	static Node *merge(Node *x, Node *y)
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
		if (this!=&rhs)
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
int n, m;
int nums[100010];
LeftistTree<std::pair<int, int>> heaps[100010];
//---------------------------------------------------------------
//简化的并查集
int p[100010], rank[100010];
bool exist[100010];
void init(int n)
{
	for (int i = 1; i <= n; ++i)
		p[i] = i;
}
int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
//---------------------------------------------------------------

int main()
{
	scanf("%d%d", &n, &m);
	init(n);
	for (int i = 1; i <= n; ++i)
	{
		scanf("%d", nums + i);
		heaps[i].push({ nums[i] ,i }); //当堆里有多个最小值时，优先删除原序列的靠前的
		exist[i] = true;
	}	
	while (m--)
	{
		int opt, x, y;
		scanf("%d%d", &opt, &x);
		if (opt == 1)
		{
			scanf("%d", &y);
			if (exist[x]&&exist[y])
			{
				int px = find(x), py = find(y);
				if (px==py)
					continue;
				p[py] = px;
				heaps[px].merge(heaps[py]);
			}
		}
		else
		{
			if (exist[x])
			{
				int p = find(x);
				std::pair<int, int> pr = heaps[p].top();
				printf("%d\n", pr.first);
				exist[pr.second] = false;
				heaps[p].pop();
			}
			else
				puts("-1");
		}
	}
	return 0;
}*/