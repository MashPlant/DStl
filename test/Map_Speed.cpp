#include <ctime>
#include <unordered_map>
#include <type_traits>
#include <iostream>
#include <map>
#include <algorithm>
#include "../Map.h"
#include "../Algorithm.h"
#include "../BTree.h"
#include "../AVL.h"
#include "../Splay.h"
#include "../Util.h"
#include "../HashMap.h"
#include "../SkipList.h"
#include "../Treap.h"
#include "../vEBTree.h"
#include "../BITTree.h"
#include "../RBTreeWithoutSize.h"
using namespace std;
using namespace ds;

const int maxn = 5e6;
int input[maxn];

template <typename M>
void test(const char *name)
{
	M m;
	cout << name << endl;
	clock_t beg[3] = { clock() };
	for (int i = 0; i < maxn; ++i)
		m.insert({ input[i] ,0 });
	cout << "insert :" << -beg[0] + (beg[1] = clock()) << endl;
	for (int i = 0; i < maxn; ++i)
		m.find(input[i]);
	cout << "find :" << -beg[1] + (beg[2] = clock()) << endl;
	for (int i = 0; i < maxn; ++i)
		m.erase(input[i]);
	cout << "erase :" << -beg[2] + clock() << endl;
	cout << "all :" << clock() - beg[0] << endl;
}

int main()
{
	for (int i = 0; i < maxn; ++i)
		input[i] = i;
	random_shuffle(input, input + maxn);
	cout << "press to begin\n";
	getchar();
	
	test<map<int, int>>("stl map");
	test<BTree<int, int, 40>>("BTree");
	test<Map<int, int>>("RBTree");
	test<Map<int, int,less<int>, RBTreeWithoutSize>>("RBTree without size");
	test<Map<int, int, less<int>, Treap>>("Treap");
	test<Map<int, int, less<int>, Splay>>("Splay");
	test<Map<int, int, less<int>, AVL>>("AVL");
	test<SkipListMap<int,int>>("SkipList");
}
//veb & bit
/*const int maxn = 3e6;
int input[maxn];
clock_t beg[4] = { clock() };
//vEBTree<23> veb;
BITTree<maxn> veb;
int main()
{
	for (int i = 0; i < maxn; ++i)
		input[i] = i + 1;
	random_shuffle(input, input + maxn);

	beg[1] = clock();
	for (int i = 0; i < maxn; ++i)
		veb.insert(input[i]);
	cout << "insert :" << -beg[1] + (beg[2] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
		bool b = veb.exist(input[i]);
	cout << "find :" << -beg[2] + (beg[3] = clock()) << "\n";
	for (int i = 0; i < maxn; ++i)
		veb.erase(input[i]);
	cout << "erase :" << -beg[3] + clock() << "\n";
	cout << "all :" << clock() - beg[1] << "\n";
	getchar();
}*/

// kth
/*int main()
{
const int maxn = 3e5;
Map<int, int> m;
Map<int, int, less<int>, AVL> avlm;
BTree<int, int, 2> bt;
for (int i = 0; i < maxn; ++i)
{
int ran = rani(1, maxn);
m.insert(ran, 0);
avlm.insert(ran, 0);
bt.insert(ran, 0);
}

for (int i = 0; i < maxn; ++i)
{
int ran = rani(1, maxn);
m.erase(ran);
avlm.erase(ran);
bt.erase(ran);
}
freopen("mp.txt", "w", stdout);
for (int i = 1; i <= m.size(); ++i)
cout << m.kth(i)->first << ' ';
freopen("avlmp.txt", "w", stdout);
for (int i = 1; i <= avlm.size(); ++i)
cout << avlm.kth(i)->first << ' ';
freopen("bt.txt", "w", stdout);
for (int i = 1; i <= bt.size(); ++i)
cout << bt.kth(i)->first << ' ';
}*/

/*内存泄漏检验
{
Map<int, int> m;
for (int i = 0; i < 1e5; ++i)
m.insert(i, 2 * i + 1);
{
cout << "hit to construct m1" << endl;
getchar();
auto m1 = m;
cout << "hit to construct m2" << endl;
getchar();
auto m2 = std::move(m1);
cout << "hit to construct m3" << endl;
getchar();
auto m3(m2);
cout << "hit to destruct m123" << endl;
getchar();
}


getchar();
}*/
