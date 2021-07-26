Various algorithms and data structures implementation in STL style.

**Use with caution!**
Most of the code in this repository was written in 2018 and has no longer been maintained since then.
It is more like a collection of code that I use to learn and experiment with algorithms and data structures rather than a library intending to be correct and stable.
For example, only part of the classes implement proper C++-style copy control, and many implementations can not comply with the STL specification or exception safety.

[Algorithm](Algorithm.h): Generic algorithms, including binary searches (corresponding to `std::lower/upper_bound`), heap algorithms (corresponding to `std::push/pop/make_heap`), sorting (corresponding to `std::sort`, which is a combination of quick sort, insertion sort and heap sort), etc.

# [sequential](sequential)

- [Vector](sequential/Vector.h): Dynamic-size array (corresponding to `std::vector`). Internally uses `realloc` for space growth, which is generally faster and can handle most cases. However, **it can break** classes containing self-referencing pointers (e.g., some versions of libstdc++ `std::string` use self-referencing pointers for SSO).
- [List](sequential/List.h): Doubly linked list
 (corresponding to `std::list`). The sorting algorithms of `List` and `ForwardList` are quite tricky.
- [ForwardList](sequential/ForwardList.h): Singly linked list
 (corresponding to `std::forward_list`).
- [CircularQueue](sequential/CircularQueue.h)/[Deque](sequential/Deque.h): Both are doubly ended queue. `Deque` corresponds to `std::deque`, and guarantees that "`push_front`, `push_back`, `emplace_front` and `emplace_back` do not invalidate any references to elements of the deque.", while `Vector`/`CircularQueue` cannot guarantee it.
- [String](sequential/String.h): Provides class `String` (essentially `Vector<char>`), `StringView` (corresponding to `std::string_view`), and several substring-searching algorithms such as `KMP` and `KarpRabin`.

# [associative](associative)

- [Map](associative/Map.h): Sorted associative container (corresponding to `std::map`). The map iterator supports offsettings integer and from each other in logarithmic time by storing sub-tree sizes in tree nodes. It is the driver for various balanced trees, and uses template parameter to select one from:
  - [RBTree](associative/RBTree.h)/[AVL](associative/AVL.h)/[Splay](associative/Splay.h)/[Treap](associative/Treap.h)
- [BTree](associative/BTree.h): Sorted associative container implemented with B-Tree. B-Tree in memory may be faster than your imagination. See [Rust BTreeMap](https://doc.rust-lang.org/stable/std/collections/struct.BTreeMap.html) documentation for some explanations.
- [SkipList](associative/SkipList.h): Sorted associative container implemented with skip list. Generally slower than balanced trees, but said to be easier to extend for concurrency.
- [HashMap](associative/HashMap.h): Hash map with closed hashing (corresponding to `std::unordered_map`). Generally slower and consumes more memory than open hashing. But it guarantees that iterators are only invalidated when rehashing occurs in insertion, and references are not invalidated, while open hashing cannot guarantee it.
- [HashMap-OpenAddress](associative/HashMap-OpenAddress.h): Hash map with open hashing. Uses template parameter to select from `Linear`, `Quadratic`, `Double` or a user-defined function for probing.

# [heap](heap)

- [FibHeap](heap/FibHeap.h): [Fibonacci heap](https://en.wikipedia.org/wiki/Fibonacci_heap). min: constant; insert/decrease-key/merge: amortized constant; delete: amortized logarithmic.
- [PairingHeap](heap/PairingHeap.h): [Pairing heap](https://en.wikipedia.org/wiki/Pairing_heap). min: constant; insert/merge: amortized constant; delete-min: amortized logarithmic; decrease-key: not clear, amortized sub-logarithmic. Although its theoretical time complexity is not as good as `FibHeap`, it is much easier to implement and has a smaller constant factor.
- [LeftistTree](heap/LeftistTree.h): [Leftist tree](https://en.wikipedia.org/wiki/Leftist_tree). min: constant; insert/merge/delete: amortized logarithmic.
- [MinMaxHeap](heap/MinMaxHeap.h): [Min-max heap](https://en.wikipedia.org/wiki/Min-max_heap). min/max: constant; delete-min/delete-max: logarithmic.
- [PriorityQueue](heap/PriorityQueue.h): Container adaptor that implements heap by applying functions defined in `Algorithm` on a underlying container (corresponding to `std::priority_queue`).

# [integer](integer)

- [BitSet](integer/BitSet.h): Fixed-size sequence of bits (corresponding to `std::bitset`).
- [BIT](integer/BIT.h): [Binary indexed tree (Fenwick tree)](https://en.wikipedia.org/wiki/Fenwick_tree) that supports range addition and sum.
- [BITTree](integer/BITTree.h): Extends `BIT` to support dynamic order statistics for integers.
- [UInt](integer/UInt.h): Fixed-size arbitrary-precision unsigned integer.
- [BigInt](integer/BigInt.h): Dynamic-size arbitrary-precision integer.
- [vEBTree](integer/vEBTree.h): Fixed-size [van Emde Boas tree](https://en.wikipedia.org/wiki/Van_Emde_Boas_tree). Use some template tricks to get rid of pointers and store the tree compactly. insert/delete/find/predecessor/successor for M-bit integer: logarithmic to M.

# [toy](toy)

- [Calculator](toy/Calculator.h): A simple calculator that uses a stack to parse operators with precedence.
- [Complexity](toy/Complexity.h): Measures algorithm runtime and uses Least Squares Method to estimate asymptotic time complexity (try to put it into one of the predefined categories).
- [DynamicSegTree](toy/DynamicSegTree.h): Segment tree that allocates nodes dynamically.
- [IntrusiveList](toy/IntrusiveList.h): Similar to [Boost.Intrusive list](https://www.boost.org/doc/libs/1_76_0/doc/html/intrusive/list.html).
- [KDTree](toy/KDTree.h): Static [k-d tree](https://en.wikipedia.org/wiki/K-d_tree). Built on initialization. Supports [k-NN](KNN) query.
- [Matrix](toy/Matrix.h): Naive implementation of fixed-size matrix.
- [MemPool](toy/MemPool.h): Caches allocations of a single kind of object. Organizes the deallocated memory with a linked list. Never deallocates any memory chunks halfway, and deallocates them at one time on destruction.
- [Polynomial](toy/Polynomial.h): Naive implementation of polynomial `+-*/%`.
- [Profiler](toy/Profiler.h): Records duration using the constructor/destructor pair.
- [Ptr32](toy/Ptr32.h): Stores a 64-bit pointer in a 32-bit word by assuming its most significant 32 bits to be zero.
- [SharedPtr](toy/SharedPtr.h): Intrusive shared pointer. Unlike `std::shared_ptr`, it doesn't support polymorphic deleter or atomic reference counting.
- [TimSort](toy/TimSort.h): [Timsort](https://en.wikipedia.org/wiki/Timsort), a hybrid stable sorting algorithm, derived from merge sort and insertion sort.
- [Trie](toy/Trie.h): Quite a failure, sometimes even slower than `Map<std::string, V>`.
- [PersistentArray](toy/PersistentArray.h): [Persistent array](https://en.wikipedia.org/wiki/Persistent_array) implemented with a tree. I only have used `PersistentArray`, `PersistentTreap` and `WBT` to solve online judge problems, and they are completely immature for any practical usage.
- [PersistentTreap](toy/PersistentTreap.h): Functional balanced tree implemented with treap.
- [WBT](toy/WBT.h): [Weight-balanced tree](https://en.wikipedia.org/wiki/Weight-balanced_tree). It is also a functional balanced tree, and is the data structure that [Haskell Data.Map](https://hackage.haskell.org/package/containers-0.4.0.0/docs/Data-Map.html) uses.

# [toy/legacy](toy/legacy)

- [Map-List](toy/legacy/Map-List.h): Maintain an implicit linked list in `RBTree` nodes to find predecessor/successor in worst-case constant time. It is useless in practice because finding predecessor/successor originally only needs average constant time.
- [NaiveDB](toy/legacy/NaiveDB.h): A naive trial to store B-tree on disk.
- [SegmentTree](toy/legacy/SegmentTree.h): Fixed-size abstract segment tree. The heavy use of templates here greatly slows compiling speed with little improvement on runtime speed.
- [vEBTree-pointer](toy/legacy/vEBTree-pointer.h): `vEBTree` implementation using pointers. Size can be specified on runtime initialization, but much slower and consumes much more memory than the template version.

# [toy/template](toy/template)

- [SetT](toy/template/SetT.h): `PersistentTreap` at compile time.
- [ListT](toy/template/ListT.h): `ForwardList` at compile time.
- [ArrayT](toy/template/ArrayT.h): Stores variadic template parameter pack in an array.
- [Function](toy/template/Function.h): Polymorphic function wrapper (corresponding to `std::function`).
- [IterTool](toy/template/IterTool.h): A small part of [C++20 Ranges library](https://en.cppreference.com/w/cpp/ranges).
- [RefWrapper](toy/template/RefWrapper.h): Wraps reference in copyable, assignable object (corresponding to `std::reference_wrapper`).
- [SIUnit](toy/template/SIUnit.h): See [Compile-time numerical unit dimension checking](https://benjaminjurke.com/content/articles/2015/compile-time-numerical-unit-dimension-checking/).
- [TypeName](toy/template/TypeName.h): Compute type name precisely at compile time. On the contrary, `typeid` may lose reference/const information. Useful to debug other template metaprogramming programs.