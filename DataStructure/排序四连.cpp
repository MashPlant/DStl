// DataStructure.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/*#include "Util.h"
#include <algorithm>
#include <ctime>
#include "Algorithm.h"
#include "TimSort.h"
#include "BTree.h"
using namespace std;
using namespace ds;

const long long maxn = 2e7;
void stable()
{
	struct A
	{
		int a, b;
		bool operator<(A rhs) const { return a < rhs.a; }
	};
	static A a[maxn];
	for (int i = 0; i < maxn; ++i)
		a[i] = { rani(1,1e8),rani(1,1e8) };

	clock_t beg = clock();
	ds::stable_sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	for (int i = 0; i < maxn; ++i)
		a[i] = { rani(1,1e8),rani(1,1e8) };

	beg = clock();
	std::stable_sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;
}
int main()
{
	static int a[maxn];
	static int input[maxn];
	for (int i = 0; i < maxn; ++i)
		input[i] = rani(1, 1e9);
	clock_t beg = clock();
	cout << "press to begin" << endl;
	getchar();

	memcpy(a, input, sizeof input);
	beg = clock();
	std::sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	memcpy(a, input, sizeof input);
	beg = clock();
	std::stable_sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	memcpy(a, input, sizeof input);
	beg = clock();
	ds::sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	memcpy(a, input, sizeof input);
	beg = clock();
	ds::radixSort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;
	
	memcpy(a, input, sizeof input);
	beg = clock();
	BTree<int, int, 40> bt;
	for (int i=0;i<maxn;++i)
	{
		int *v = bt.find(i);
		if (v)++*v;
		else bt.insert({ i,1 });
	}
	int pos = 0;
	bt.inorderWalk([&](int x,int n)
	{
		for (int i = 0; i < n; ++i)
			a[pos++] = x;
	});
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;
	getchar();
}*/

