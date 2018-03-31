大家应该已经看出来了，现在这个repo已经和数据结构没有什么必然的联系了。我们的目标是~~星辰大海~~stl。

一.容器

1.顺序容器

[Vector](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Vector.h)
[Deque](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Deque.h)
[CircularQueue](https://github.com/MashPlant/DStl/blob/master/DataStructure/CircularQueue.h)
[List](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/List.h)
[ForwardList](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/ForwardList.h)

Deque和CircularQueue都是双向队列的实现，其中Deque符合stl对deque的规范，"push_front, push_back, emplace_front and emplace_back do not invalidate any references to elements of the deque."，这是Vector/vector/CircularQueue都不具备的能力。

2.关联容器

[Map](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Map.h)

[RBTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/RBTree.h)
[AVL](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/AVL.h)
[Treap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Treap.h)
[Splay](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Splay.h)

这四种平衡树是Map的四种实现，默认的模板参数是RBTree

迭代器支持+-，利用的是**平衡树本来就支持但是stl偏偏没有的**kth和rankof

~~然而即使每个节点多维护了一个size信息还是轻松干掉开O2的stl~~

[HashMap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/HashMap.h)
[HashMap-OpenAddress](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/HashMap-OpenAddress.h)

链表/开放寻址的散列表，根据SFINAE来自动选择使用哪一个。提供一个哈希函数的话就会选择链表，从Linear，Quadratic，Double三种探查方式里选一个的话就会用开放寻址。

[SkipList](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/SkipList.h)

如果跳表也算K-V容器的话.....

[BTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BTree.h)

B树跑的是真的快。https://www.luogu.org/record/show?rid=6366862 平衡树模板题，我的B树是所有真的用了平衡树的提交里最快的，干掉红黑树不在话下。

3.堆

[PriorityQueue](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/PriorityQueue.h)
[LeftistTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/LeftistTree.h)
[PairingHeap](https://github.com/MashPlant/DStl/blob/master/DataStructure/PairingHeap.h)
[FibHeap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/FibHeap.h)

后面三个都没有提供拷贝控制，懒得写

二.非容器数据结构
1.[vEBTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/vEBTree.h)

存放一定范围内的整数的数据结构。利用了一些模板的技巧摆脱了指针。

2.[BIT](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BIT.h)
[BITTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BITTree.h)

普通的树状数组和树状数组模拟的平衡树

3.[KDTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/KDTree.h)

普通的KD树

4.[PersistentArray](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/PersistentArray.h)
[PersistentTreap](https://github.com/MashPlant/DStl/blob/master/DataStructure/PersistentTreap.h)

普通的可持久化数组/可持久化Treap

5.[BitSet](https://github.com/MashPlant/DStl/blob/master/DataStructure/BitSet.h)

这个基本上是源码级复制stl的了...

三.算法

[Algorithm]https://github.com/MashPlant/DStl/blob/master/DataStructure/Algorithm.h

大概能看的就只有一个排序算法。原理是stl的魔改快排，快排+插排+堆排，基本可以稳定赢过stl一点。

[https://github.com/MashPlant/DStl/blob/master/DataStructure/TimSort.h]

一个TimSort的实现，又长又慢...

四.工具类

1.高精整数

[BigInt](https://github.com/MashPlant/DStl/blob/master/DataStructure/BigInt.h)
[UInt](https://github.com/MashPlant/DStl/blob/master/DataStructure/UInt.h)

十进制/二进制高精整数，后者的各种运算要快的多。

2.[SharedPtr](https://github.com/MashPlant/DStl/blob/master/DataStructure/SharedPtr.h)

简化的智能指针实现，比shared_ptr小的多(当然能力也弱一些)。

五.C++黑科技

1.模板元编程

[ListT](https://github.com/MashPlant/DStl/blob/master/DataStructure/ListT.h)
编译期链表

[SetT](https://github.com/MashPlant/DStl/blob/master/DataStructure/SetT.h)
编译期可持久化Treap

[TypeName](https://github.com/MashPlant/DStl/blob/master/DataStructure/TypeName.h)

准确输出类的名字，与之相比，typeid有可能丢失引用/const信息。这个用来调一些模板的bug很好用。

2.[IterTool](https://github.com/MashPlant/DStl/blob/master/DataStructure/IterTool.h)

```cpp
for (auto i : slice(filter(vec,[](int x){return x % 2 == 0;}), 0, 10))
  std::cout << i << ' ';
```
大概就是这种画风的吧。

六.失败的作品

[Trie](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Trie.h)

跑的比装string的map还慢...

[SegmentTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/SegmentTree.h)

除了拖慢编译时间之外什么也做不了
