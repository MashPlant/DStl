#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;
//��һ������һ����n,��ʾ������
//��2-n+1������n������
//1 x ��ͷ������x
//2 x ��β������x
//3 x y ���±�x������y
//4 x y ɾ��[x,y]���Ԫ��
//5 x y ���[x,y]���Ԫ��
inline int rani(int l, int r)
{
	return (rand()*RAND_MAX + rand()) % (r - l + 1) + l;
}
int getopt()
{
	int ran = rani(1, 18);
	if (ran <= 6)
		return 1;
	if (ran <= 12)
		return 2;
	if (ran <= 14)
		return 3;
	if (ran <= 16)
		return 4;
	return 5;
}
/*int main()
{
	freopen("in.txt", "w", stdout);
	int n = 1e6;
	printf("%d\n", n);
	while (n--)
	{
		int o = getopt(), x = rani(1, 1e4);
		if (o == 1 || o == 2)
			printf("%d %d\n", o, rani(1, 1e3));
		if (o == 3)
			printf("%d %d %d\n", o, x, rani(1, 1e3));
		if (o == 4 || o == 5)
			printf("%d %d %d\n", o, x, x + rani(5, 10));
	}
}*/
