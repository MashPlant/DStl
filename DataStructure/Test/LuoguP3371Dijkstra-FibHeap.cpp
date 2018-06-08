#include <cstring>
#include <iostream>
#include <list>
#include <vector>
#include "../FibHeap.h"

template<typename K, template <typename T, typename = std::allocator<T>> typename Container>
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
	const K &top() const { return min->getKey(); }
	void pop()
	{
		for (auto it : min->childs)
			it->p = nullptr;
		roots.splice(roots.end(), min->childs);
		roots.erase(min->self);
		popFix();
	}
	bool empty()const { return roots.empty(); }
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
				iterator it = roots.insert(roots.end(), aux[i]);
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

typedef std::vector<int> vi;
typedef std::pair<int, int> pii;
const int inf = 0x3f3f3f3f;
const int maxn = 1e4 + 10;

int d[maxn];
int v_cnt, e_cnt;
struct Edge
{
	int to, w;
	Edge(int _to, double _w) : to(_to), w(_w) {}
};
typedef std::vector<Edge> ve;
ve edges[maxn];

typedef typename FibHeap<pii, std::list>::Node Node;
Node *nodes[maxn];
void dijkstra(int s)
{
	memset(d, 0x3f, sizeof(d));
	FibHeap<pii, std::list> fib;
	for (int i = 1; i <= v_cnt; ++i)
		if (i != s)
			nodes[i] = fib.push({ inf ,i });
		else
			nodes[i] = fib.push({ 0,i });
	d[s] = 0;
	while (!fib.empty())
	{
		pii cur = fib.top();
		fib.pop();
		int v = cur.second, dis = cur.first;
		d[v] = dis;
		ve &e = edges[v];
		for (int i = 0; i < e.size(); ++i)
		{
			int to = e[i].to;
			if (d[v] + e[i].w < d[to])
			{
				d[to] = d[v] + e[i].w;
				fib.decreaseKey(nodes[to], { d[to],to });
			}
		}
	}
}

inline int read()
{
	static char ch;
	bool sgn = false;
	while ((ch = getchar()) < '0' || ch > '9')
		if (ch == '-')
			sgn = true;
	int res = ch - 48;
	while ((ch = getchar()) >= '0' && ch <= '9')
		res = res * 10 + ch - 48;
	return sgn ? -res : res;
}

int main()
{
	v_cnt = read(), e_cnt = read();
	int s = read();
	while (e_cnt--)
	{
		int from = read(), to = read(), w = read();
		edges[from].push_back(Edge(to, w));
	}
	dijkstra(s);
	for (int i = 1; i <= v_cnt; ++i)
		printf("%d ", d[i] == inf ? 2147483647 : d[i]);
	return 0;
}
