#pragma once
#include "SegmentTreeUtil.h"

namespace ds
{
	//这是一颗抽象的线段树，任何满足区间加性质的类型&函数均可以用它实现
	//但是区间修改只支持区间加数
	//希望有一些更优雅的写法来支持区间乘/区间=/...
	//不过主要目的还是娱乐，谁会用这种东西做题呢？
	template <typename K,typename Func,int N>
	class SegmentTree
	{
	private:
		Seg::Node<K, Func, 1, N> root;
		typedef Seg::FuncTraits<K, Func> Function;
		static Function func;

		//节点的Left/Right是节点管辖的范围
		//查询的l/r是所需的范围
		template <int Left,int Right>
		static K query(Seg::Node<K,Func,Left,Right> &x,int l,int r) 
		{
			if (l <= Left && r >= Right)
				return x.key;
			pushDown(x);
			constexpr int mid = (Left + Right) / 2;

			K lres = Function::zero, rres = Function::zero;
			if (l <= mid)
				lres = query(x.left,l, r);
			if (r >= mid + 1)
				rres = query(x.right, l, r);
			return func(lres, rres);
		}
		template <int Left>
		static K query(Seg::Node<K, Func, Left, Left> &x, int l, int r) { return x.key; }

		template <int Left, int Right>
		constexpr static int lenOf(const Seg::Node<K, Func, Left, Right> &x) { return Right - Left + 1; }
		
		template <int Left, int Right>
		static void addLabel(Seg::Node<K, Func, Left, Right> &x, const K &lb) { x.label += lb; }
		template <int Left>
		static void addLabel(Seg::Node<K, Func, Left, Left> &x, const K &lb) { }
		template <int Left, int Right>

		static void pushDown(Seg::Node<K, Func, Left, Right> &x)
		{
			if (x.label)
			{
				K lb = x.label;
				x.label = K();
				x.left.key += Function::rangeSum(lb, lenOf(x.left)); 
				x.right.key += Function::rangeSum(lb, lenOf(x.right));
				addLabel(x.left, lb);
				addLabel(x.right, lb);
			}
		}
		template <int Left>
		static void pushDown(Seg::Node<K, Func, Left, Left> &x) { }

		template <int Left, int Right>
		static void rangeAdd(Seg::Node<K, Func, Left, Right> &x, int l, int r,const K &k)
		{
			if (l <= Left && r >= Right)
			{
				x.key += Function::rangeSum(k, lenOf(x));
				x.label += k;
				return;
			}
			pushDown(x);
			constexpr int mid = (Left + Right) / 2;
			if (l <= mid)
				rangeAdd(x.left, l, r, k);
			if (r >= mid + 1)
				rangeAdd(x.right, l, r, k);
			x.key = func(x.left.key, x.right.key);
		}
		template <int Left>
		static void rangeAdd(Seg::Node<K, Func, Left, Left> &x, int l, int r, const K &k) { x.key += k; }
	public:
		template<typename Iter>
		SegmentTree(Iter it):root(it){}
		K query(int l, int r) { return query(root, l, r); }
		void rangeAdd(int l, int r, const K &k) { rangeAdd(root, l, r, k); }

		template <typename ModifyT>
		void modify(int l,int r,const K &k)
		{
			
		}
		
	};
	template <typename K, typename Func, int N>
	typename SegmentTree<K, Func, N>::Function SegmentTree<K, Func, N>::func;
}
