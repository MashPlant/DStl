#include "stdafx.h"
/*#include <cstdio>
#include <utility>
#include <queue>
#include <functional>
typedef std::pair<int, int> pii;
const int maxn = 1e4 + 10;

int d[maxn];
int v_cnt, e_cnt;
struct Edge
{
	int to, w, pre;
} edges[500010];
int head[maxn];//某点出发的最后条边
void dijkstra(int s)
{
	memset(d, 0x3f, sizeof(d));
	std::priority_queue<pii, std::vector<pii>, std::greater<pii>> q; //最小堆
	q.push({ 0, s });
	d[s] = 0;
	while (!q.empty())
	{
		pii cur = q.top();
		q.pop();
		int v = cur.second, dis = cur.first;
		if (d[v] < dis)
			continue;
		for (int e = head[v]; e; e = edges[e].pre)
		{
			Edge &eg = edges[e];
			if (d[v] + eg.w < d[eg.to])
			{
				d[eg.to] = d[v] + eg.w;
				q.push({ d[eg.to], eg.to });
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
	int cur_e = 0;
	while (e_cnt--)
	{
		int from = read(), to = read(), w = read();
		edges[++cur_e] = { to, w, head[from] };
		head[from] = cur_e;
	}
	dijkstra(s);
	for (int i = 1; i <= v_cnt; ++i)
		printf("%d ", d[i] == 0x3f3f3f3f ? 2147483647 : d[i]);
	return 0;
}*/
