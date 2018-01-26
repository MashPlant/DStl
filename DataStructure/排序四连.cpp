// DataStructure.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Util.h"
#include <algorithm>
#include <ctime>
#include "Algorithm.h"
#include "TimSort.h"
using namespace std;
using namespace ds;

const long long maxn = 1e8;
/*void stable()
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
}*/
int main()
{
	//stable();
	static int a[maxn];
	static int input[maxn];
	for (int i = 0; i < maxn; ++i)
		input[i] = rani(1, 1e9);
	clock_t beg = clock();
	cout << "press to begin" << endl;
	getchar();

	for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	beg = clock();
	std::sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	beg = clock();
	std::stable_sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	beg = clock();
	ds::sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	beg = clock();
	ds::radixSort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	beg = clock();
	ds::stable_sort(a, a + maxn);
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;

	/*for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	beg = clock();
	TimSort<int *>(a, a + maxn).sort();
	cout << clock() - beg << endl;
	cout << is_sorted(a, a + maxn) << endl;*/
	getchar();
}

