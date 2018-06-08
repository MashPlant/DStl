#include "Algorithm.h"
#include <algorithm>
#include <cstring>
#include <random>
#include <iostream>
#include <ctime>
#include "Polynomial.h"

using namespace ds;
using namespace std;

struct A
{
	int pre[10];
	int nxt[10];
} a;

int main()
{
	Polynomial a{4, -27, 66, -65, 24};
	Polynomial b{1, -2};
	b = pow(b, 3);
	Polynomial c{1, -1};
	c *= c;
	cout << a << endl << b << endl << c << endl;
	cout << a % b << endl << a % c;
	vector<int> v = vector<int>({1, 2, 3});
}
