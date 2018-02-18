#include "stdafx.h"

#include "MemPool.h"
#include <iostream>
#include <list>
#include "SharedPtr.h"
#include "ctime"
using namespace std;
using namespace ds;
const int maxn = 2000;
template <typename L>
void test()
{
	L *l2;
	{
		clock_t beg = clock();
		L lst;
		for (int i = 0; i < maxn; ++i)
			lst.push_back(i);
		cout << clock() - beg << endl;
	}
	getchar();
}

struct A : EnableShared
{
	static int cnt;
	double x;
	virtual void foo() const 
	{
		cout << "A\n";
	}
	~A()
	{
		++cnt;
		cout << "destroy\n";
	}
};
int A::cnt = 0;

struct B : A
{
	double xx;
	void foo() const override
	{
		cout << "B\n";
	}
};

//shared_ptr<int> p[maxn]; //529MB/1e7
//unique_ptr<int> up[maxn];	//274MB/1e7 
//SharedPtr<Int> mp[maxn]; //274MB/1e7 少一个删除器，多一个计数器
template <typename K,typename = typename enable_if<is_floating_point<K>::value>::type>
void foo(K k)
{
	cout << k << endl;
}
int main()
{
	
}