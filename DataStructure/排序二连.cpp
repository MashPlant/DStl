// DataStructure.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Util.h"
#include <algorithm>
#include <ctime>
#include "Algorithm.h"

using namespace std;
using namespace ds;

const int maxn = 2e7;

/*int main()
{
	static int a[maxn];
	static int input[maxn];
	for (int i = 0; i < maxn; ++i)
		input[i] = rani(1, 1e9);

	for (int i = 0; i < maxn; ++i)
		a[i] = input[i];
	clock_t beg = clock();
	std::sort(a, a + maxn);
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
	getchar();
}*/

