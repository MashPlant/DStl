#include <iostream>
#include <list>
#include <ctime>
#include <algorithm>
#include "../MemPool.h"
using namespace std;
using namespace ds;
const int maxn = 5e6;
template <typename L>
void test(const char *name)
{
    cout << name << endl;
    clock_t beg = clock();
    {
        L lst;
        for (int i = 0; i < maxn; ++i)
            lst.push_back(i), lst.push_front(i);
    }
    cout << clock() - beg << endl;
}

int main()
{
    test<list<int, MemPool<int>>>("list<int, MemPool<int>>");
    test<list<int>>("list<int>");
}