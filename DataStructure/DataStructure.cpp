// DataStructure.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <deque>
#include "Util.h"
#include "Deque.h"
#include "List.h"
#include "Vector.h"
#include <algorithm>
#include <ctime>
#include "Map.h"
#include "Algorithm.h"
#include <map>
using namespace std;
using namespace ds;

int main()
{	
	Map<int, int> m;
	for (int i = 0; i < 100; ++i)
		m.insert(i, 2 * i);
	for (auto it : m)
		cout << it.first << ' ' << it.second<<"   ";
	getchar();
}

