#include <iostream>
#include <algorithm>
#include "../Util.h"
#include "../SkipList.h"
#include "../Treap.h"
#include "../vEBTree.h"
#include "../BITTree.h"
#include "../DynamicSegTree.h"
using namespace std;
using namespace ds;
const int maxn = 2e6;
int input[maxn];
bool exist[maxn];
int ordered[maxn];
template <typename T, typename... Args>
void test(const char *name, Args &&... args)
{
    cout << name << endl;
    clock_t beg[5] = {clock()};
    T *pt = new T(args...);
    cout << "build :" << -beg[0] + (beg[1] = clock()) << endl;
    for (int i = 0; i < maxn; ++i)
        pt->insert(input[i]);
    cout << "insert :" << -beg[1] + (beg[2] = clock()) << endl;
    for (int i = 0; i < maxn; ++i)
        exist[i] = pt->exist(input[i]);
    cout << "find :" << -beg[2] + (beg[3] = clock()) << endl;
    for (int i = pt->findMin(), cnt = 0; ~i; i = pt->findNext(i))
        ordered[cnt++] = i;
    for (int i = pt->findMax(), cnt = 0; ~i; i = pt->findPrev(i))
        ordered[cnt++] = i;
    cout << "prev/next :" << -beg[3] + (beg[4] = clock()) << endl;
    for (int i = 0; i < maxn; ++i)
        pt->erase(input[i]);
    cout << "erase :" << -beg[4] + clock() << endl;
    cout << "all :" << clock() - beg[0] << endl;

    delete pt;
}
int main()
{
    for (int i = 0; i < maxn; ++i)
        input[i] = i + 1;
    random_shuffle(input, input + maxn);

    test<BITTree<maxn>>("BITTree<2000000>");
    test<vEBTree<21>>("vEBTree<21>");
}
