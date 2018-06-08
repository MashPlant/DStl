#include <iostream>
#include "../DynamicSegTree.h"
#include "../BITTree.h"
#include "../fc.h"
#include "../Util.h"
using namespace std;
using namespace ds;
const int N = 10000;
DynamicSegTree<100000> tr(1, N);
BITTree<N, true> bit;
int main()
{
	for (int i = 0; i < N * 2; ++i)
	{
		int x = rani(1, N);
		tr.insert(x);
		bit.insert(x);
	}
	for (int i = 0; i < N; ++i)
	{
		int x = rani(1, N);
		tr.erase(x);
		bit.erase(x);
	}
	freopen("bit.txt", "w", stdout);
	for (int i = bit.findMin(); ~i; i = bit.findNext(i))
		cout << i;
	for (int i = bit.findMin(); ~i; i = bit.findPrev(i))
		cout << i;
	for (int i = 1, sz = bit.size(); i <= sz; ++i)
		cout << bit.kth(i);
	for (int i = bit.findMin(), mx = bit.findMax(); i <= mx; ++i)
		cout << bit.rank(i);


	freopen("segtree.txt", "w", stdout);
	for (int i = tr.findMin(); ~i; i = tr.findNext(i))
		cout << i;
	for (int i = tr.findMin(); ~i; i = tr.findPrev(i))
		cout << i;
	for (int i = 1, sz = tr.size(); i <= sz; ++i)
		cout << tr.kth(i);
	for (int i = tr.findMin(), mx = tr.findMax(); i <= mx; ++i)
		cout << tr.rank(i);

	freopen("con", "w", stdout);
	
	cout <<"result(0 for correct):" <<fc("bit.txt", "segtree.txt");
	return 0;
}