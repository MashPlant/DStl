#include <deque>
#include <algorithm>
#include <ctime>
#include <list>
#include <vector>
#include <deque>
#include <forward_list>
#include <type_traits>
#include "../Util.h"
#include "../Deque.h"
#include "../List.h"
#include "../Vector.h"
#include "../Algorithm.h"
#include "../CircularQueue.h"
#include "../ForwardList.h"

using namespace std;
using namespace ds;

template <typename T>
constexpr int isListV = is_same_v<T, list<int>> || is_same_v<T, List<int>> || is_same_v<T, forward_list<int>> || is_same_v<T, ForwardList<int>>;

template <typename Container>
void push_back_and_sort(const char *name)
{
    Container seq;
    cout << name << endl;
    clock_t beg = clock();
    for (int i = 0; i < 1e7; ++i)
        seq.push_back(rani(1, 1e9));
    cout << "push_back: " << clock() - beg << endl;
    beg = clock();
    if constexpr (isListV<Container>)
        seq.sort();
    else
        std::sort(seq.begin(), seq.end());
    cout << "sort: " << clock() - beg << endl;
    cout << "correct: " << boolalpha << is_sorted(seq.begin(), seq.end()) << endl;
}

template <typename Container>
void push_front_and_sort(const char *name)
{
    Container seq;
    cout << name << endl;
    clock_t beg = clock();
    for (int i = 0; i < 1e7; ++i)
        seq.push_front(rani(1, 1e9));
    cout << "push_front: " << clock() - beg << endl;
    beg = clock();
    if constexpr (isListV<Container>)
        seq.sort();
    else
        std::sort(seq.begin(), seq.end());
    cout << "sort: " << clock() - beg << endl;
    cout << "correct: " << boolalpha << is_sorted(seq.begin(), seq.end()) << endl;
}
int main()
{
    cout << "press to begin\n";
    getchar();
    push_back_and_sort<vector<int>>("stl vector");
    push_back_and_sort<deque<int>>("stl deque");
    push_back_and_sort<list<int>>("stl list");
    push_back_and_sort<Vector<int>>("Vector");
    push_back_and_sort<Deque<int>>("Deque");
    push_back_and_sort<CircularQueue<int>>("CircularQueue");
    push_back_and_sort<List<int>>("List");

    push_front_and_sort<deque<int>>("stl deque");
    push_front_and_sort<list<int>>("stl list");
    push_front_and_sort<forward_list<int>>("stl forward_list");
    push_front_and_sort<Deque<int>>("Deque");
    push_front_and_sort<CircularQueue<int>>("CircularQueue");
    push_front_and_sort<List<int>>("List");
    push_front_and_sort<ForwardList<int>>("ForwardList");
}
