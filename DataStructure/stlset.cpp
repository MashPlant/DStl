#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <set>
#include "Util.h"
using namespace std;


/*int main()
{
	freopen("in.txt", "r", stdin);
	freopen("brutle.txt", "w", stdout);
	clock_t beg = clock();
	multiset<string> s;
	int n = ds::read();
	while (n--)
	{
		int o = ds::read();
		string str = ds::reads();
		if (o == 1)
			s.insert(str);
		else if (o == 2)
		{
			auto it = s.find(str);
			if (it != s.end())
				s.erase(it);
		}
		else
			printf("%d\n", s.count(str));
	}
	printf("%d\n", s.size());
	cout << clock() - beg << endl;
	return 0;
}*/
