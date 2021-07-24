#include <algorithm>
#include <ctime>
#include <iomanip>
#include "../Algorithm.h"
#include "../TimSort.h"
#include "../BTree.h"
#include "../Util.h"
using namespace std;
using namespace ds;

const int maxn = 5e7;
int a[maxn];
int input[maxn];
template <typename Sorter>
void test(Sorter s, const char *name)
{
    cout << name << endl;
    memcpy(a, input, sizeof input);
    clock_t beg = clock();
    s(a, a + maxn);
    cout << clock() - beg << endl;
    cout << "correct? " << boolalpha << is_sorted(a, a + maxn) << endl;
}
int main()
{
    for (int i = 0; i < maxn; ++i)
        input[i] = rani(1, 1e9);
    cout << "press to begin" << endl;
    getchar();

    test(std::sort<int *>, "std::sort");
    test(std::stable_sort<int *>, "std::stable_sort");
    test(ds::sort<int *>, "ds::sort");
    test(ds::stable_sort<int *>, "ds::stable_sort");
    test(ds::radixSort, "ds::radixSort");
}
