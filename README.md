# DataStructure
蒟蒻实现的一些数据结构，大体上模仿stl

变量名/函数名/etc 非常不统一，一方面是因为部分函数为了方便测试而与stl一致，一方面也是自己比较懒

目录

1.线性数据结构

1.1 [Vector](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Vector.h)

对应stl中的vector，基本上完全一致，除了使用malloc/realloc/free管理内存之外

1.2 [Deuqe](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Deque.h)

对应stl中的deque，原理上完全不一样。stl的deque使用了一片连续的"中央控制器"来管理零散的大内存块，实现整块连续的假象；这里的Deque是最简单的实现：循环队列。二者相比，stl的虽然迭代较慢，但是在push/pop时，最坏情况下也只是O(n/blocksize)，相当优秀，更加适合用来实现stack和queue。我限于能力有限，没有像stl一样实现。

1.3 [List](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/List.h)

对应stl中的list，基本上完全一致，带一个哨兵节点的双向队列。值得一提的是它的排序算法。不像某些语言(如Java)直接把链表转换成数组来排序(**注1**)(可以说是很流氓了)。这里的List::sort是模仿stl的实现，是一个非递归的归并排序，非常巧妙。

2.树状数据结构

2.0 [Map](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Map.h)

对应stl中的map。Map只是一个壳子，实际上使用的数据结构在它的模板参数中确定，默认使用RBTree。

迭代器支持+-，利用的是**平衡树本来就支持但是stl偏偏没有的**kth和rankof。当然了迭代器的类型仍然为双向迭代器，因为++--比+1-1效率更高。

为什么没有Set呢?懒得写。

2.1 - 2.4

[RBTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/RBTree.h)

[AVL](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/AVL.h)

[Treap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Treap.h)

[Splay](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Splay.h)

注意并不是所有树都支持所有操作，没有什么原理上的困难，主要是有些懒得写了。只保证RBTree和AVL支持所有操作。

2.5

[vEBTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/vEBTree.h)

只能存放一定范围内的整数。非常高效的数据结构，尤其在是利用了一些模板的技巧摆脱了指针之后。不与上面的那些兼容。

2.6
[BTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BTree.h)

谁能想到B树跑的这么快呢?只有insert，find，erase的情况下比红黑树快一倍(包括我的红黑树(稍快一点点)和stl的红黑树)。不与上面的那些兼容。

3.哈希表

[HashMap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/HashMap.h)

没什么好说的，就是个普普通通的哈希表。取模策略是模仿Java，移位+异或+按位与，反正就是不取模。

4.优先队列

4.1

[PriorityQueue](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/PriorityQueue.h)

对应stl的priority_queue。非常遗憾，比它略慢一点。"抱歉，O2优化的stl真的是可以为所欲为的"。

4.2

[LeftistTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/LeftistTree.h)

左偏树，支持O(lgn)合并操作的优先队列。**没有做拷贝控制!!!**

4.3

[FibHeap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/FibHeap.h)

斐波那契堆，支持均摊O(1)合并，O(1)节点减值的优先队列(就是常数大了点)。**没有做拷贝控制!!!**

5.区间查询

5.1

[SegmentTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/SegmentTree.h)

模板化线段树。非常可惜不能用来做题目，因为需要生成的类型和函数太多，OJ的编译器很可能CTLE。

5.2

[BIT](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BIT.h)

普通的树状数组。



注1：

The default implementation obtains an array containing all elements in this list, sorts the array, and iterates over this list resetting each element from the corresponding position in the array. (This avoids the n2 log(n) performance that would result from attempting to sort a linked list in place.)   -JDK

stl:"我就静静看着你装逼"。
