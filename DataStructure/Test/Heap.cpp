#include <iostream>
#include <algorithm>
#include <ctime>
#include <queue>
#include <list>
#include "../Util.h"
#include "../Deque.h"
#include "../Vector.h"
#include "../PriorityQueue.h"
#include "../LeftistTree.h"
#include "../FibHeap.h"
#include "../MinMaxHeap.h"
#include "../PairingHeap.h"

using namespace std;
using namespace ds;

const int maxn = 5e6;
int input[maxn], a[maxn];
template <typename PQ>
void test(const char *name)
{
    cout << name << endl;
    PQ pq;
    clock_t beg = clock();
    for (int i = 0; i < maxn; ++i)
        pq.push(input[i]);
    if constexpr (!std::is_same_v<PQ, MinMaxHeap<int>>)
    {
        int pos = 0;
        while (!pq.empty())
        {
            a[pos++] = pq.top();
            pq.pop();
        }
    }
    else
    {
        int head = 0, tail = maxn - 1;
        while (!pq.empty())
        {
            a[head++] = pq.min();
            pq.popMin();
            a[tail--] = pq.max();
            pq.popMax();
        }
    }
    cout << clock() - beg << endl;
    cout << "correct? " << boolalpha << is_sorted(a, a + maxn) << endl;
}
int main()
{
    for (int i = 0; i < maxn; ++i)
        input[i] = rani(1, 1e9);
    test<priority_queue<int, vector<int>, greater<int>>>("std::priority_queue");
    test<PriorityQueue<int, greater<int>>>("PriorityQueue(binary heap)");
    test<MinMaxHeap<int>>("MinMaxHeap");
    test<PairingHeap<int>>("PairingHeap");
    test<LeftistTree<int>>("LeftistTree");
    test<FibHeap<int, std::list>>("FibHeap");
    getchar();
}