#include "stdafx.h"
#include <iostream>
#include "Util.h"
#include "Complexity.h"
#include <algorithm>
#include "Algorithm.h"
#include "BITTree.h"
using namespace std;
using namespace ds;
const int maxn = 2e6;
int a[maxn];
int main()
{
	for (int i = 0; i < maxn; ++i)
		a[i] = i;
	auto f1 = [](int n) {std::sort(a, a + n); };
	auto f2 = [](int n) {ds::radixSort(a, a + n); };
	auto p = [](int n) {random_shuffle(a, a + n); };
	const int start = 1e5, step = 1e5;
	array<int, (maxn - start) / step> arr;
	arr[0] = start;
	for (int i = 1; i < arr.size(); ++i)
		arr[i] = arr[i - 1] + step;
	auto res = Complexity::autoCalc(f1, p, arr, Complexity::nlgn);
	cout << res.k << ' ' << res.b << ' ' << res.R2 << endl;
	res = Complexity::autoCalc(f2, p, arr, Complexity::n);
	cout << res.k << ' ' << res.b << ' ' << res.R2 << endl;
	//运行，得到结果发现b<0，这可能是动态内存申请导致的
	getchar();
}

