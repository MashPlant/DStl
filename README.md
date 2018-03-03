# DataStructure
蒟蒻实现的一些数据结构，大体上模仿stl

变量名/函数名/etc 非常不统一，一方面是因为部分函数为了方便测试而与stl一致，一方面也是自己比较懒

目录

1.线性数据结构

1.1 [Vector](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Vector.h)

对应stl中的vector，基本上完全一致，除了使用malloc/realloc/free管理内存之外

1.2 [Deque](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Deque.h)

对应stl中的deque，原理上完全不一样。stl的deque使用了一片连续的"中央控制器"来管理零散的大内存块，实现整块连续的假象；这里的Deque是最简单的实现：循环队列。二者相比，stl的虽然迭代较慢，但是在push/pop时，最坏情况下也只是O(n/blocksize)，相当优秀，更加适合用来实现stack和queue。我限于能力有限，没有像stl一样实现。

1.3 

[List](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/List.h)

对应stl中的list，基本上完全一致，带一个哨兵节点的双向队列。值得一提的是它的排序算法。不像某些语言(如Java)直接把链表转换成数组来排序(**注1**)(可以说是很流氓了)。这里的List::sort是模仿stl的实现，是一个非递归的归并排序，非常巧妙。

[ForwardList](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/ForwardList.h)

不带哨兵节点，不保存size的单向链表。这是由它的使用目的决定的:省内存。(然而并没有用在HashMap里)

1.4 [SkipList](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/SkipList.h)

总有一些数据结构，跑的又慢，占空间又多，关键是也不好写。跳表可以说是典型代表了。听说在多线程下有很大用处，我不会，溜了溜了。

2.平衡树

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

2.5 [vEBTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/vEBTree.h)

只能存放一定范围内的整数。非常高效的数据结构，尤其在是利用了一些模板的技巧摆脱了指针之后。不与上面的那些兼容。

2.6 [BTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BTree.h)

谁能想到B树跑的这么快呢?只有insert，find，erase的情况下比红黑树快一倍(包括我的红黑树(稍快一点点)和stl的红黑树)。不与上面的那些兼容。

3.哈希表

默认的HashMap使用开链法处理冲突。

3.1 [HashMap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/HashMap.h)

开链法处理冲突的哈希表。取模策略是模仿Java，移位+异或+按位与，反正就是不取模。

3.2 [HashMap-OpenAddress](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/HashMap-OpenAddress.h)

开放寻址法处理冲突的哈希表。探查方式有三种:Linear，Quadratic，Double，可以在模板参数里面选择。貌似跑的比开链法快一些?

4.优先队列

4.1 [PriorityQueue](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/PriorityQueue.h)

对应stl的priority_queue。非常遗憾，比它略慢一点。"抱歉，O2优化的stl真的是可以为所欲为的"。

4.2 [LeftistTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/LeftistTree.h)

左偏树，支持O(lgn)合并操作的优先队列。**没有做拷贝控制!!!**

4.3 [FibHeap](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/FibHeap.h)

斐波那契堆，支持均摊O(1)合并，O(1)节点减值的优先队列(就是常数大了点)。**没有做拷贝控制!!!**

5.各种查询

5.1 [SegmentTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/SegmentTree.h)

模板化线段树。非常可惜不能用来做题目，因为需要生成的类型和函数太多，OJ的编译器很可能CTLE。

5.2 

[BIT](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BIT.h)

普通的树状数组。

[BITTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/BITTree.h)

树状数组模拟的平衡树，前驱后继kth是(lgn)^2，其他是lgn。虽然复杂度大但是常数小。

5.3 [Trie](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/Trie.h)

emmm用来存字符串貌似跑的比红黑树还慢一点.....的确是我写的太差了。

5.4 [KDTree](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/KDTree.h)

本来是klgn的k近邻查询，但是因为必须维护一个标记数组，所以还是O(n)，或者说O(klgn+n)。貌似可以直接把时间戳打在节点上，这样就不用每次memset了，不过懒得写了。

5.5 [PersistentArray](https://github.com/MashPlant/DataStructure/blob/master/DataStructure/PersistentArray.h)

可持久化数组。没有做operator[]，因为实在意义不大，只能用来读数据，没办法用来创建新版本，所以就用query和update吧。



注1：

The default implementation obtains an array containing all elements in this list, sorts the array, and iterates over this list resetting each element from the corresponding position in the array. (This avoids the n2 log(n) performance that would result from attempting to sort a linked list in place.)   -JDK

stl:"我就静静看着你装逼"。
