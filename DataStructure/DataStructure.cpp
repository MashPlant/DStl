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
using namespace std;
using namespace ds;
const int maxn = 5e6;
int input[maxn];
int main()
{
	for (int i = 0; i < maxn; ++i)
		input[i] = i;
	random_shuffle(input, input + maxn);
	clock_t beg[4] = { clock() };
	vEBTree<23> veb;
	cout << "create :" << -beg[0] + (beg[1] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
		veb.insert(input[i]);
	cout << "insert :" << -beg[1] + (beg[2] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
		bool bl = veb.exist(input[i]);
	cout << "find :" << -beg[2] + (beg[3] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
		veb.erase(input[i]);
	cout << "erase :" << -beg[3] + clock() << "\n";
	cout << "all :" << clock() - beg[0] << "\n";
	getchar();
	return 0;
}

