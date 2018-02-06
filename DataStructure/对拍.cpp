#include "stdafx.h"

/*#include "Algorithm.h"
#include <iostream>
#include "Util.h"
#include "Map.h"
#include <map>
#include "AVL.h"
#include "Splay.h"
#include "SkipList.h"
#include "BTree.h"
#include "RBTreeArray.h"
#include "Treap.h"
using namespace std;
using namespace ds;

//正确性检验
int main()
{
	Map<int, int> mp;
	Map<int, int, less<int>, Treap> tp;
	const int maxn = 2e5;
	for (int i = 1; i < maxn; ++i)
	{
		int x = rani(1, maxn), y = rani(1, maxn);
		mp.insert({ x, y });
		tp.insert({ x,y });
	}
	for (int i = 1; i < maxn; ++i)
	{
		int x = rani(1, maxn);
		mp.erase(x);
		tp.erase(x);
	}
	freopen("mp.txt", "w", stdout);
	for (int k = 1; k <= mp.size(); ++k)
		cout << mp.kth(k)->first << ' ';
	freopen("tp.txt", "w", stdout);
	for (int k = 1; k <= tp.size(); ++k)
		cout << tp.kth(k)->first << ' ';
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

