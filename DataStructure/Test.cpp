// DataStructure.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <deque>
#include "Util.h"
#include "Deque.h"
#include <algorithm>
using namespace std;
using namespace ds;

/*void itertest()
{
	Deque<int> dq;
	for (int i = 0; i < 1e3; ++i)
		dq.push_front(rani(1, 1e3));
	sort(dq.begin(), dq.end());
	for_each(dq.begin(), dq.end(), [](int x) {cout << x << ' '; });
	cout << endl;
	reverse(dq.begin(), dq.end());
	for_each(dq.begin(), dq.end(), [](int x) {cout << x << ' '; });
	getchar();
}
int main()
{
	freopen("in.txt", "r", stdin);
	//freopen("deque.txt", "w", stdout);
	int n = ds::read();
	ds::Deque<int> dq;
	deque<int> stldq;
	while (n--)
	{
		if (n % 100 == 0)
			cout << n << ' ';
		int o = ds::read(), x = ds::read(), y;
		if (o != 1 && o != 2)
			y = ds::read();
		//1 x ��ͷ������x
		//2 x ��β������x
		//3 x y ���±�x������y
		//4 x y ɾ��[x,y]���Ԫ��
		//5 x y ���[x,y]���Ԫ��
		switch (o)
		{
		case 1:
			dq.push_front(x);
			stldq.push_front(x);
			break;
		case 2:
			dq.push_back(x);
			stldq.push_back(x);
			break;
		case 3:
			if (x < dq.size())
			{
				dq.insert(x, y);
				stldq.insert(stldq.begin() + x, y);
			}
			break;
		case 4:
			if (x < dq.size() && y<dq.size())
			{
				dq.erase(x, y - x + 1);
				stldq.erase(stldq.begin() + x, stldq.begin() + y + 1);
			}
			break;
		case 5:
			break;
		}
	}
	for (int i = 0; i < dq.size(); ++i)
		if (dq[i] != stldq[i])
			cout << "no\n";
	cout << "yes\n";
	cout << "finished";
	return 0;
}*/

