#pragma once
#include "Algorithm.h"
namespace ds
{
	//动态顺序统计量
	//插入，删除，rank : O(lgU)
	//前驱，后继，kth : O(lg^2(U))
	//空间 : O(n*lgU) (对比普通权值线段树 : O(U))
	template <int MaxSize, bool Dynamic = MaxSize == -1>
	class DynamicSegTree;

	template <int MaxSize>
	class DynamicSegTree<MaxSize, false>
	{
		struct Node
		{
			int l, r;
			int size;
		};
		static Node tree[MaxSize];
		static int treeSize;
		int root = 0;
		const int minKey, maxKey;
		static void erase(int rt,int l,int r,int key)
		{
			if (l > r || !rt) return;
			int mid = (l + r) >> 1;
			if (l < r)
			{
				key <= mid ? erase(tree[rt].l, l, mid, key) : erase(tree[rt].r, mid + 1, r, key);
				tree[rt].size = tree[tree[rt].l].size + tree[tree[rt].r].size;
			}
			else if (tree[rt].size)
				--tree[rt].size;
		}
		//求出<key的键的数量,+1得到rank
		static int lower(int rt,int l,int r,int key)
		{
			int rk = 0;
			if (key > r) return tree[rt].size;
			while (rt)
			{
				int mid = (l + r) >> 1;
				if (key <= mid)
					rt = tree[rt].l, r = mid;
				else
					rk += tree[tree[rt].l].size, rt = tree[rt].r, l = mid + 1;
			}
			return rk;
		}
	public:
		DynamicSegTree(int minKey, int maxKey) :minKey(minKey), maxKey(maxKey) { }
		int size() const { return tree[root].size; }
		int count(int key) const
		{
			int l = minKey, r = maxKey, rt = root;
			while (tree[rt].size)
			{
				int mid = (l + r) >> 1;
				if (l == r && l == key)
					return tree[rt].size;
				if (key <= mid)
					rt = tree[rt].l, r = mid;
				else
					rt = tree[rt].r, l = mid + 1;
			}
			return 0;
		}
		bool exist(int key) const { return count(key); }
		void insert(int key)
		{
			int l = minKey, r = maxKey, *rt = &root;
			while (l <= r)
			{
				if (!*rt) 
					*rt = ++treeSize;
				++tree[*rt].size;
				if (l == r)
					break;
				int mid = (l + r) >> 1;
				if (key <= mid)
					rt = &tree[*rt].l, r = mid;
				else
					rt = &tree[*rt].r, l = mid + 1;
			}
		}
		void erase(int key) const
		{
			//erase不存在的节点可能需要回溯，但是没有记录p，所以不能回溯
			//所以erase还是采用递归的方法
			erase(root, minKey, maxKey, key);
		}
		int rank(int key) const 
		{
			return lower(root, minKey, maxKey, key) + 1;
		}
		int kth(int k) const 
		{
			//当k<1时返回最小值，k>size()时返回最大值
			int l = minKey, r = maxKey, rt = root;
			while (tree[rt].size)
			{
				if (l == r)
					return l;
				int mid = (l + r) >> 1;
				if (tree[tree[rt].l].size < k)
					k -= tree[tree[rt].l].size, rt = tree[rt].r, l = mid + 1;
				else
					rt = tree[rt].l, r = mid;
			}
			return -1;
		}
		int findPrev(int key) const
		{
			int rk = rank(key) - 1;
			return rk < 1 ? -1 : kth(rk);
		}
		int findNext(int key) const
		{
			int rk = rank(key) + count(key);
			return rk > size() ? -1 : kth(rk);
		}
		int findMin() const { return findNext(minKey - 1); }
		int findMax() const { return findPrev(maxKey + 1); }
	};
	template <int MaxSize>
	typename DynamicSegTree<MaxSize, false>::Node DynamicSegTree<MaxSize, false>::tree[MaxSize];
	template <int MaxSize>
	int DynamicSegTree<MaxSize, false>::treeSize = 0;

//	template <int Min,int Max,int MaxSize = -1>
//	class DynamicSegTree
//	{
//		static struct Node
//		{
//			int l, r;
//			int size;
//		} tree[MaxSize];
//	};
//
//	template <int Min,int Max>
//	class DynamicSegTree<Min,Max,-1>
//	{
//		
//	};
}