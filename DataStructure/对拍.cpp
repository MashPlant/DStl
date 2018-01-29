#include "stdafx.h"

/*#include "Algorithm.h"
#include <iostream>
#include "Util.h"
#include <algorithm>
#include "BTree.h"
#include "Map.h"
#include <map>
#include "NaiveDB.h"
#include "AVL.h"
#include "Splay.h"
using namespace std;
using namespace ds;

//正确性检验
int main()
{
	Map<int, int> mp;
	Map<int, int, less<int>, Splay> splay;
	Map<int, int, less<int>, AVL> avl;
	const int maxn = 3e5;
	for (int i = 1; i < maxn; ++i)
	{
		int x = rani(1, maxn), y = rani(1, maxn);
		mp.insert({ x, y });
		splay.insert({ x,y });
		avl.insert({ x, y });
	}
	
	for (int i = 1; i < maxn; ++i)
	{
		int x = rani(1, maxn);
		mp.erase(x);
		splay.erase(x);
		avl.erase(x);
	}
	freopen("mp.txt", "w", stdout);
	for (const auto &it : mp)
		cout << it.first << ' ';
	freopen("splay.txt", "w", stdout);
	for (const auto &it : splay)
		cout << it.first << ' ';
	freopen("avl.txt", "w", stdout);
	for (const auto &it : avl)
		cout << it.first << ' ';
	//getchar();
}*/

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

