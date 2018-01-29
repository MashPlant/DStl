#include "stdafx.h"
#include "Algorithm.h"
#include <iostream>
#include "Util.h"
#include <algorithm>
#include "BTree.h"
#include "Map.h"
#include "NaiveDB.h"
#include <map>
#include <set>
#include "AVL.h"
#include "Splay.h"
using namespace std;
using namespace ds;

int main()
{
	Map<int, int, less<int>, RBTree> rbtree;
	Map<int, int, less<int>, Splay> splay;
	Map<int, int, less<int>, AVL> avl;
	getchar();
}

