#include "stdafx.h"
/*#include "IntrusiveList.h"
#include <iostream>
#include "Util.h"
#include "Map.h"
#include "RBTreeArray.h"
#include "Stack.h"
#include "String.h"
#include "Treap.h"
#include "SkipList.h"
#include "vEBTree.h"
#include "vEBTree-template.h"
#include "SegmentTree.h"
using namespace std;
using namespace ds;
const int maxn = 5e6;
int input[maxn];
clock_t beg[4] = { clock() };
vEBTree<23> veb;
int main()
{
	
	cout << "create :" << -beg[0] + clock() << "\n";

	for (int i = 0; i < maxn; ++i)
		input[i] = i;
	random_shuffle(input, input + maxn);

	beg[1] = clock();
	for (int i = 0; i < maxn; ++i)
		veb.insert(input[i]);
	cout << "insert :" << -beg[1] + (beg[2] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
	{
		bool b = veb.exist(input[i]);
		if (rawRani() == 0) //避免编译器把这整个循环给优化掉了
			cout << b;
	}
	cout << "find :" << -beg[2] + (beg[3] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
		veb.erase(input[i]);
	cout << "erase :" << -beg[3] + clock() << "\n";
	cout << "all :" << clock() - beg[0] << "\n"; 
	Vector<int> vec = { 1,2,3,4,5 };
	SegmentTree<int, plus<int>, 5> seg = Seg::InputIter(4);
	seg.rangeAdd(1, 5, 1);
	for (int i = 1; i <= 5; ++i)
		for (int j = i; j <= 5; ++j)
			cout << i << ' ' << j << ' ' << seg.query(i, j) << endl;
	seg.rangeAdd(1, 5, -1);
	for (int i = 1; i <= 5; ++i)
		for (int j = i; j <= 5; ++j)
			cout << i << ' ' << j << ' ' << seg.query(i, j) << endl;
	getchar();
	return 0;
}*/

