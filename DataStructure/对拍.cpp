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
#include "vEBTree.h"
#include "vEBTreee-template.h"
using namespace std;
using namespace ds;

//正确性检验
vEBTree<20> veb; //2^20
int main()
{
	Map<int, int> mp;
	
	const int maxn = 4e5;
	for (int i = 1; i < maxn; ++i)
	{
		int x = rani(1, maxn), y = rani(1, maxn);
		mp.insert({ x, y });
		veb.insert(x, true);
	}
	
	for (int i = 1; i < maxn; ++i)
	{
		int x = rani(1, maxn);
		mp.erase(x);
		veb.erase(x, false);
	}
	freopen("mp.txt", "w", stdout);
	for (int k = 1; k <= mp.size(); ++k)
		cout << mp.kth(k)->first << ' ';
	freopen("veb.txt", "w", stdout);
	for (int x = veb.findMin(); ~x; x = veb.findNext(x))
	{
		cout << x << ' ';
	}
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

