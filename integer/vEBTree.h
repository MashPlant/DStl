#pragma once
#include <utility>
#include <bitset>

namespace ds
{
	namespace vEB
	{
		constexpr int lodiv2(int i) { return i >> 1; }
		constexpr int hidiv2(int i) { return (i >> 1) + (i & 1); }
		template <int N>
		struct Node
		{
			int min = -1, max = -1;
			//原来之所以不得不用指针，只是因为不同大小的Node都是一个类型
			//现在它们是不同类型的，不再需要指针，也不需要递归建树
			Node<hidiv2(N)> summary;
			Node<lodiv2(N)> clusters[1 << hidiv2(N)];
			constexpr static int highPart(int x) { return x >> lodiv2(N); }
			constexpr static int lowPart(int x) { return x & ((1 << lodiv2(N)) - 1); }
			constexpr static int numOf(int hi, int lo) { return (hi << lodiv2(N)) + lo; }
		};
		template <>
		struct Node<1> //由于C++不允许类内特例化，故而放在外面的命名空间里
		{
			int min = -1, max = -1;
		};
	}
	template <int Size> //最终vEB树的容量是2^Size
	class vEBTree
	{
	private:
		vEB::Node<Size> root;
		std::bitset<1 << Size> table; //O(1)exist查询
		template <int N>
		static void putOnEmpty(vEB::Node<N> &z, int x) { z.min = z.max = x; }
		template <int N>
		static void insert(vEB::Node<N> &z,int x)
		{
			if (z.min == -1) //min为-1意味着max也为-1,空节点直接插
				putOnEmpty(z, x);
			else
			{
				if (x < z.min)
					std::swap(x, z.min);
				//此时N > 1
				const int hi = z.highPart(x), lo = z.lowPart(x);
				if (z.clusters[hi].min == -1)
				{
					insert(z.summary, z.highPart(x));//本簇中没有元素，需要修改summary
					putOnEmpty(z.clusters[hi], lo);
				}
				else //本簇中有元素
					insert(z.clusters[hi], lo);
				if (x > z.max)//insert完了之后才修改，如果要改循环，需要独立出去
					z.max = x;
			}
		}
		static void insert(vEB::Node<1> &z,int x)
		{
			if (z.min == -1) //min为-1意味着max也为-1,空节点直接插
				putOnEmpty(z, x);
			else
			{
				if (x < z.min)
					std::swap(x, z.min);
				if (z.max < x) 
					z.max = x;
			}
		}
		template <int N>
		static void erase(vEB::Node<N> &z, int x)
		{
			if (z.max == z.min) //有且只有一个元素
				z.max = z.min = -1;
			else
			{
				if (x == z.min) //在簇中找一个元素来当新min
				{
					const int firstCluster = z.summary.min; //第一个非空的簇
					x = z.numOf(firstCluster, z.clusters[firstCluster].min); //那个簇中的第一个元素
					z.min = x;
				}
				erase(z.clusters[z.highPart(x)], z.lowPart(x));
				//以上完成了对簇的修改，以下解决簇为空之后的summary和维护max
				if (z.clusters[z.highPart(x)].min == -1) //刚删完的簇变空了
				{
					erase(z.summary, z.highPart(x));
					if (x == z.max) //删掉的是max，需更新之；对于min的处理在上面；
					{
						const int maxSummary = z.summary.max; //由于用到了summary，对max的维护放在对summary的维护之后
						if (maxSummary == -1)
							z.max = z.min;
						else z.max = z.numOf(maxSummary, z.clusters[maxSummary].max);
					}
				}
				else if (x == z.max)
					z.max = z.numOf(z.highPart(x), z.clusters[z.highPart(x)].max);
			}
		}
		static void erase(vEB::Node<1> &z, int x)
		{
			if (z.max == z.min) //有且只有一个元素
				z.max = z.min = -1;
			else
				z.max = z.min = !x;
		}

		template <int N>
		static int findNext(const vEB::Node<N> &z, int x)
		{
			if (x < z.min) //无需判断z.min是不是-1
				return z.min;
			const int inClusterMax = z.clusters[z.highPart(x)].max;
			if (inClusterMax != -1 && z.lowPart(x)<inClusterMax) //next在当前簇
				return z.numOf(z.highPart(x), findNext(z.clusters[z.highPart(x)], z.lowPart(x)));
			const int nextCluster = findNext(z.summary, z.highPart(x)); //next不在当前簇
			if (nextCluster == -1)
				return -1;
			return z.numOf(nextCluster, z.clusters[nextCluster].min);
		}
		static int findNext(const vEB::Node<1> &z, int x) { return (x == 0 && z.max == 1) ? 1 : -1; }

		template <int N>
		static int findPrev(const vEB::Node<N> &z, int x)
		{
			if (x > z.max) //无需判断z.max是不是-1
				return z.max;
			const int clusterMin = z.clusters[z.highPart(x)].min;
			if (clusterMin != -1 && clusterMin < z.lowPart(x))
				return z.numOf(z.highPart(x), findPrev(z.clusters[z.highPart(x)], z.lowPart(x)));
			const int prevCluster = findPrev(z.summary, z.highPart(x));
			if (prevCluster == -1) //z.min并不存在于cluster中，要特判
				return x > z.min ? z.min : -1; //无需判断z.min是不是-1
			return z.numOf(prevCluster, z.clusters[prevCluster].max);
		}
		static int findPrev(const vEB::Node<1> &z, int x) { return (x == 1 && z.min == 0) ? 0 : -1; }

	public:
		int findMin() const { return root.min; }
		int findMax() const { return root.max; }
		bool exist(int x) const { return table[x]; }
		void insert(int x)
		{
			if (!table[x])
				table[x] = true, insert(root, x);
		}
		void erase(int x)
		{
			if (table[x])
				table[x] = false, erase(root, x);
		}
		int findNext(int x) { return findNext(root, x); }
		int findPrev(int x) { return findPrev(root, x); }
	};
}
