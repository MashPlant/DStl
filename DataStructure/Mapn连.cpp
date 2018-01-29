// DataStructure.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/*#include <algorithm>
#include <ctime>
#include "Map.h"
#include "Algorithm.h"
#include <map>
#include "BTree.h"
#include "AVL.h"
#include "Splay.h"
#include "Util.h"
using namespace std;
using namespace ds;

const int maxn = 1e6;
int input[maxn * 2];
template <typename M>
void test()
{
	M m;
	clock_t beg = clock();
	for (int i = 0; i<maxn; ++i)
		m.insert({ input[i], 0 });
	for (int j = 0; j < 2; ++j)
		for (int i = 0; i < maxn; ++i)
			m.find(input[i]);
	for (int i = 0; i < maxn / 2; ++i)
		m.erase(input[i]);
	for (int i = 0; i < maxn; ++i)
		m.insert({ 2 * input[i], 0 });
	for (int i = 0; i < maxn * 2; ++i)
		m.find(input[i]);
	for (int i = 0; i < maxn * 2; ++i)
		m.erase(input[i]);
	cout << clock() - beg << endl;
}

int main()
{
	for (int i = 0; i < maxn * 2; ++i)
		input[i] = rani(1, 1e9);
	cout << "press to begin\n";
	getchar();
	test<map<int, int>>();
	test<Map<int, int>>();
	test<BTree<int, int, 40>>();
	test<Map<int, int, less<int>, AVL>>();
	test<Map<int, int, less<int>, Splay>>();
	getchar();
}*/
/* kth
const int maxn = 20000;
Map<int, int> m;
Map<int, int, less<int>, AVL> avlm;
for (int i = 0; i < maxn; ++i)
{
int ran = rani(1, maxn);
m.insert(ran, 0);
avlm.insert(ran, 0);
}

for (int i = 0; i < maxn; ++i)
{
int ran = rani(1, maxn);
m.erase(ran);
avlm.erase(ran);
}
freopen("mp.txt", "w", stdout);
for (int i = 1; i <= m.size(); ++i)
cout << m.kth(i)->first << ' ';
freopen("avlmp.txt", "w", stdout);
for (int i = 1; i <= avlm.size(); ++i)
cout << avlm.kth(i)->first << ' ';
 */
/*内存泄漏检验
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
*/