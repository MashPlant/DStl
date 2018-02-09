#include "stdafx.h"
#include "IntrusiveList.h"
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

int main()
{
	SegmentTree<int, plus<int>, 5> seg = Seg::InputIter();
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
}

