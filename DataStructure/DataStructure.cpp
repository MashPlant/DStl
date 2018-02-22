#include "stdafx.h"
#include <iostream>
#include "HashMap-OpenAddress.h"
#include "HashMap.h"
using namespace std;
using namespace ds;


int main()
{
	HashMap<int, int> m1;
	HashMap<int, int, OpenAddress<int, Linear, hash<int>>> m2;
	getchar();
}