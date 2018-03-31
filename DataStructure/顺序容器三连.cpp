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
#include "CircularQueue.h"
#include "ForwardList.h"
#include <forward_list>

using namespace std;
using namespace ds;

template <typename Container>
void sort(Container &x)
{
	std::sort(x.begin(), x.end());
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
void test(const char *name)
{
	Container x;
	cout << name << ' ';
	clock_t beg = clock();
	for (int i = 0; i < 1e7; ++i)
		x.push_back(rani(1, 1e9));
	cout << clock() - beg << ' ';
	beg = clock();
	sort(x);
	cout << clock() - beg << ' ';
	cout << is_sorted(x.begin(), x.end()) << endl;
}
//int main()
//{
//	cout << "press to begin\n";
//	getchar();
//	test<Vector<int>>("Vector");
//	test<Deque<int>>("Deque");
//	test<CircularQueue<int>>("CircularQueue");
//	test<List<int>>("List");
//	test<vector<int>>("stl vector");
//	test<deque<int>>("stl deque");
//	test<list<int>>("stl list");
//	cout << "round2\n";
//	test<vector<int>>("stl vector");
//	test<deque<int>>("stl deque");
//	test<list<int>>("stl list");
//	test<Vector<int>>("Vector");
//	test<Deque<int>>("Deque");
//	test<CircularQueue<int>>("CircularQueue");
//	test<List<int>>("List");
//	getchar();
//}

