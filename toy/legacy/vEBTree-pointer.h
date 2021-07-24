//Deprecated,用模板的才是真正的vEB
/*#pragma once
#include <utility>
#include <algorithm>

namespace ds
{
	template <int Size> //最终vEB树的容量是2^Size
	class vEBTree
	{
	public:
		static int countZero(int i) //返回一个形如0...010..0的数右侧的0的个数
		{
			
#if __GNUC__
			//gcc内置
			return __builtin_ctz(i);
#else
			int cnt = 0;
			while (i >>= 1)
				++cnt;
			return cnt;
#endif

			
		}
		
		要实现vEB Map有点困难(应该不是C++的锅)
		每个节点的min都会配一个value，summary不用配value，size=2的节点需要两个value
		要么统一配两个(浪费空间)，要么用指针(浪费空间&时间)，要么用模板
		模板的大致思路是，首先处理Node*的函数都改成模板
		然后size不同的Node，summary的Node都做成不同类型
		template<int N,bool IsSummary>
		struct Node {   Node<hi(N),true>*summary ,Node<hi(N),false> cluster[lo(N)]    } //hi和lo都是带N的常量表达式
		template<bool IsSummary>
		struct Node<1,IsSummary> {...}
		template<int N>
		struct Node<N,true> {...}
		我也不知道这样写行不行，懒得试了
		
		struct Node
		{
			static int hiSqrt[32], loSqrt[32];
			int zeros = 1;
			int min = -1;
			int max = -1;
			Node *summary = nullptr;
			Node **clusters = nullptr;
			//cluster是hi棵vEB树(大小为lo)，summary是1棵vEB树(大小为hi)
			//cluster[i]不为空 <=> cluster[i]->min!=-1 <=> exist(summary,i)==true 
			//元素x以lo(x)的形式存在clustre[hi(x)]中，但min元素并不储存在cluster中
			Node() = default;
			Node(int zeros, Node *summary, Node **clusters) :zeros(zeros), summary(summary), clusters(clusters) {}
			//编译器的确很聪明，但是我还是不太信任它，除/模变量恐怕很难优化
			int highPart(int x) const { return x >> (zeros >> 1); }
			int lowPart(int x) const { return x & (loSqrt[zeros] - 1); }
			int numOf(int hi, int lo) const { return (hi << (zeros >> 1)) + lo; }
		};
		static Node* buildTree(int u)
		{
			if (u == 1)
				return new Node();
			const int hi = Node::hiSqrt[u], lo = Node::loSqrt[u];
			Node *summary = buildTree(countZero(hi));
			Node **clusters = new Node*[hi];
			for (int i = 0; i < hi; ++i)
				clusters[i] = buildTree(countZero(lo));
			return new Node(u, summary, clusters);
		}
		Node *root;
		static int findMin(Node *z)
		{
			return z->min;
		}
		static int findMax(Node *z)
		{
			return z->max;
		}
		static void putOnEmpty(Node *z, int x)
		{
			z->min = z->max = x;
		}
		void insert(Node *z, int x)
		{
			if (z->min == -1) //min为-1意味着max也为-1,空节点直接插
				putOnEmpty(z, x);
			else
			{
				if (x < z->min)
					std::swap(x, z->min);
				if (z->zeros > 1)
				{
					const int hi = z->highPart(x), lo = z->lowPart(x);
					if (findMin(z->clusters[hi]) == -1)
					{
						insert(z->summary, z->highPart(x));//本簇中没有元素，需要修改summary
						putOnEmpty(z->clusters[hi], lo);
					}
					else //本簇中有元素
						insert(z->clusters[hi], lo);
				}
				if (z->max < x) //insert完了之后才修改，如果要改循环，需要独立出去
					z->max = x; 
			}
		}
		void erase(Node *z, int x)
		{
			if (z->max == z->min) //有且只有一个元素
				z->max = z->min = -1;
			else if (z->zeros == 1) //两个元素，执行到这里x只能为0/1
				z->max = z->min = !x;
			else
			{
				if (x == z->min) //在簇中找一个元素来当新min
				{
					const int firstCluster = findMin(z->summary); //第一个非空的簇
					x = z->numOf(firstCluster, findMin(z->clusters[firstCluster])); //那个簇中的第一个元素
					z->min = x;
				}
				erase(z->clusters[z->highPart(x)], z->lowPart(x));
				//以上完成了对簇的修改，以下解决簇为空之后的summary和维护max
				if (findMin(z->clusters[z->highPart(x)]) == -1) //刚删完的簇变空了
				{
					erase(z->summary, z->highPart(x));
					if (x == z->max) //删掉的是max，需更新之；对于min的处理在上面；
					{
						const int maxSummary = findMax(z->summary); //由于用到了summary，对max的维护放在对summary的维护之后
						if (maxSummary == -1)
							z->max = z->min;
						else z->max = z->numOf(maxSummary, findMax(z->clusters[maxSummary]));
					}
				}
				else if (x == z->max)
					z->max = z->numOf(z->highPart(x), findMax(z->clusters[z->highPart(x)]));
			}
		}
		int findNext(Node *z, int x)
		{
			if (z->zeros == 1)
				return (x == 0 && z->max == 1) ? 1 : -1;
			if (x < z->min) //无需判断z->min是不是-1
				return z->min;
			const int inClusterMax = findMax(z->clusters[z->highPart(x)]);
			if (inClusterMax != -1 && z->lowPart(x)<inClusterMax) //next在当前簇
				return z->numOf(z->highPart(x), findNext(z->clusters[z->highPart(x)], z->lowPart(x)));
			const int nextCluster = findNext(z->summary, z->highPart(x)); //next不在当前簇
			if (nextCluster == -1)
				return -1;
			return z->numOf(nextCluster, findMin(z->clusters[nextCluster]));
		}
		int findPrev(Node *z, int x)
		{
			if (z->zeros == 1)
				return (x == 1 && z->min == 0) ? 0 : -1;
			if (z->max < x) //无需判断z->max是不是-1
				return z->max;
			const int clusterMin = findMin(z->clusters[z->highPart(x)]);
			if (clusterMin != -1 && clusterMin < z->lowPart(x))
				return z->numOf(z->highPart(x), findPrev(z->clusters[z->highPart(x)], z->lowPart(x)));
			const int prevCluster = findPrev(z->summary, z->highPart(x));
			if (prevCluster == -1) //z->min并不存在于cluster中，要特判
				return z->min < x ? z->min : -1; //无需判断z->min是不是-1
			return z->numOf(prevCluster, findMax(z->clusters[prevCluster]));
		}

	public:
		vEBTree()
		{
			for (int i = 0; i < 32; ++i)
			{
				Node::loSqrt[i] = 1 << (i >> 1);
				Node::hiSqrt[i] = 1 << ((i & 1) ? ((i + 1) >> 1) : (i >> 1));
			}
			root = buildTree(Size);
		}
		int findMin() const
		{
			return root->min;
		}
		int findMax() const
		{
			return root->max;
		}
		bool exist(int x) const
		{
			Node *z = root;
			while (true)
			{
				if (z->min == x || z->max == x)
					return true;
				if (z->zeros == 1)
					return false;
				const int tmp = x;
				x = z->lowPart(x), z = z->clusters[z->highPart(tmp)];
			}
		}
		void insert(int x, bool existed = false) //如果可以提供额外的信息(某个键一定存在/不存在)，则可省去判断
		{
			if (!existed || !exist(x))
				insert(root, x);
		}
		void erase(int x, bool existed = true)
		{
			if (existed || exist(x))
				erase(root, x);
		}
		int findNext(int x) { return findNext(root, x); }
		int findPrev(int x) { return findPrev(root, x); }
	};
	template <int Size>
	int vEBTree<Size>::Node::hiSqrt[32];
	template <int Size>
	int vEBTree<Size>::Node::loSqrt[32];
}*/
