#include "stdafx.h"

/*#include <iostream>
#include "Util.h"
#include "Map.h"
#include <map>
#include "AVL.h"
#include "vEBTree.h"
#include "BITTree.h"
using namespace std;
using namespace ds;

//正确性检验
const int maxn = 4e5;
BITTree<maxn> bit;
int main()
{
	Map<int, int> mp;
	
	
	for (int i = 1; i < 2 * maxn; ++i)
	{
		int x = rani(1, maxn), y = rani(1, maxn);
		mp.insert({ x, y });
		bit.insert(x);
	}
	
	for (int i = 1; i < 2 * maxn; ++i)
	{
		int x = rani(1, maxn);
		mp.erase(x);
		bit.erase(x);
	}
	freopen("mp.txt", "w", stdout);
	for (int k = 1; k <= mp.size(); ++k)
		cout << mp.kth(k)->first << ' ';
	freopen("bit.txt", "w", stdout);
	for (int i = 1, sz = bit.size(); i <= sz; ++i)
		cout << bit.kth(i) << ' ';
	//for (int x = bit.findMin(); ~x; x = bit.findNext(x))
		//cout << x << ' ';
	
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

