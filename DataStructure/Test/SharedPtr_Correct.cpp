#include <iostream>
#include <list>
#include "../SharedPtr.h"
#include <ctime>
using namespace std;
using namespace ds;
// const int maxn = 2000;
// template <typename L>
// void test()
// {
// 	L *l2;
// 	{
// 		clock_t beg = clock();
// 		L lst;
// 		for (int i = 0; i < maxn; ++i)
// 			lst.push_back(i);
// 		cout << clock() - beg << endl;
// 	}
// 	getchar();
// }

struct A
{
	static int cnt;
	int _cnt_;
	double x;
	virtual void foo() const
	{
		cout << "A\n";
	}
	virtual ~A()
	{
		++cnt;
		cout << "A destroy\n";
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
	~B() override
	{
		cout << "B destroy\n";
	}
};

int main()
{
	{
		SharedPtr<A> pa(new B);
		pa->foo();

		auto pa1 = pa;
		auto pa2 = move(pa);
		SharedPtr<A> pa3 = nullptr;
		if (!pa3)
			pa3 = pa2;
		swap(pa1, pa2);
		pa1 = makeShared<A>();
		pa2 = pa3 = pa1; //此时之前的B应该析构
		pa3->foo();
	}
	cout << A::cnt;
}