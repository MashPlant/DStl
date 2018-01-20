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
#include "Map.h"
#include "Map-List.h"
#include "Algorithm.h"
#include <map>
using namespace std;
using namespace ds;

const int maxn = 1e6;
template <typename M>
void test()
{
	M m;
	clock_t beg = clock();
	for (int i = 0; i<maxn; ++i)
		m.insert({ i, 2 * i + 1 });
	for (int i = 0; i < maxn / 2; ++i)
		m[i + 1] = m[i];
	for (int i = 0; i < maxn / 2; ++i)
		m.erase(i);
	cout << clock() - beg << endl;
	cout << m.size() << endl;
}
/*int main()
{
	{
		Map<int, int> m;
		for (int i = 0; i < 1e5; ++i)
			m.insert(i, 2 * i + 1);
		{
			cout << "hit to construct m1" << endl;
			getchar();
			auto m1 = m;
			cout << "hit to construct m2" << endl;
			getchar();
			auto m2 = std::move(m1);
			cout << "hit to construct m3" << endl;
			getchar();
			auto m3(m2);
			cout << "hit to destruct m123" << endl;
			getchar();
		}


		getchar();
	}

}*/