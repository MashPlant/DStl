// DataStructure.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <deque>
#include "Util.h"
#include "Deque.h"
#include "List.h"
#include "Vector.h"
#include <algorithm>
#include <ctime>
#include <list>
#include <vector>
#include <deque>
#include "Algorithm.h"
using namespace std;
using namespace ds;

template <typename Container>
void sort(Container &x)
{
	sort(x.begin(), x.end());
}
template <typename K>
void sort(List<K> &lst)
{
	lst.sort();
}
template <typename K>
void sort(list<K> &lst)
{
	lst.sort();
}

template<typename Container>
void test()
{
	Container x;
	clock_t beg = clock();
	for (int i = 0; i < 1e7; ++i)
		x.push_back(rani(1, 1e7));
	cout << clock() - beg << endl;
	beg = clock();
	sort(x);
	cout << clock() - beg << endl;
	cout << is_sorted(x.begin(), x.end()) << endl;
}
/*int main()
{
	test<Deque<int>>();
	cout << endl;
	test<List<int>>();
	cout << endl;
	test<Vector<int>>();
	cout << endl;
	test<deque<int>>();
	cout << endl;
	test<list<int>>();
	cout << endl;
	test<vector<int>>();
	cout << endl;
	getchar();
}*/

