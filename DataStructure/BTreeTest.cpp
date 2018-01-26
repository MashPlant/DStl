#include "stdafx.h"
/*
#include "Algorithm.h"
#include <iostream>
#include "Util.h"
#include <algorithm>
#include "BTree.h"
using namespace std;
using namespace ds;*/

/* 正确性检验
BTree<int, int, 300> bt;
Map<int, int> mp;
map<int, int> stlmp;
const int maxn = 1e5;
for (int i = 1; i < maxn; ++i)
{
int x = rani(1, maxn), y = rani(1, maxn);
bt.insert(x, y);
mp.insert(x, y);
stlmp.insert({ x,y });
}

for (int i = 1; i < maxn; ++i)
{
int x = rani(1, maxn);
bt.erase(x);
mp.erase(x);
stlmp.erase(x);
}
auto it1 = mp.begin();
auto it2 = stlmp.begin();
freopen("stlmp.txt", "w", stdout);
for (; it2 != stlmp.end(); ++it2)
cout << it2->first << ' ';
freopen("mp.txt", "w", stdout);
for (; it1 != mp.end(); ++it1)
cout << it1->first << ' ';
freopen("bt.txt", "w", stdout);
bt.inorderWalk([](int f, int s) {cout << f << ' '; });*/
/*内存泄漏检验
int main()
{
	{
		BTree<string, int, 30> bt;
		const int maxn = 2e6;
		for (int i = 1; i < maxn; ++i)
			bt.insert(to_string(i), i);
		cout << "press to destruct\n";
		getchar();
	}
	getchar();
}*/

