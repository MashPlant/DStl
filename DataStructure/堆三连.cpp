#include "stdafx.h"
#include "Util.h"
#include "Deque.h"
#include "Vector.h"
#include <algorithm>
#include <ctime>
#include "PriorityQueue.h"
#include "LeftistTree.h"
#include "FibHeap.h"
#include <list>
#include <queue>
using namespace std;
using namespace ds;

/*namespace testpq
{
	const int maxn = 3e6;
	int input[maxn], a[maxn];

	template <typename PQ>
	void testpq()
	{
		PQ pq;
		clock_t beg = clock();
		for (int i = 0; i < maxn; ++i)
			pq.push(input[i]);
		int pos = 0;
		while (!pq.empty())
		{
			a[pos++] = pq.top();
			pq.pop();
		}
		cout << clock() - beg << endl;
		cout << is_sorted(a, a + maxn) << endl;
	}
	
}
int main()
{
	for (int i = 0; i < testpq::maxn; ++i)
		testpq::input[i] = rani(1, 1e9);
	testpq::testpq<FibHeap<int, list>>();
	testpq::testpq<FibHeap<int, List>>();
	testpq::testpq<LeftistTree<int>>();
	testpq::testpq<PriorityQueue<int, greater<int>>>();
	//然而写了一大堆，都没有stl快......(斐波那契堆才不是给你这样用的呢
	testpq::testpq<priority_queue<int, vector<int>, greater<int>>>();
	getchar();
}*/