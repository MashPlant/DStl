#pragma once
#include "BIT.h"
#include "Algorithm.h"
namespace ds
{
	//很显然，因为树状数组支持[1,N](业界习惯)
	//所以BITTree也只支持[1,N]，要是想用[0,N)的话自觉在客户端-1吧
	//它支持所有平衡树操作(不像垃圾vEB)，而且常数非常小
	//正常情况下可以吊打其他所有树(vEB除外)，要是N实在太大还可以坐标离散化
	template <int N, bool Multi = false>
	struct BITTree : BIT<N>
	{
		//所有函数的参数需自觉保证合法
		//应该不会有人故意传个负数进来吧(滑稽)
		BITTree(int n = N) :BIT<N>(n) {}
		int count(int x) const { return BIT<N>::query(x, x); }
		bool exist(int x) const { return count(x); }
		void insert(int x)
		{
			if (Multi || !count(x))
				BIT<N>::add(x, 1);
		}
		void erase(int x)
		{
			//Multi情况的erase是不明确的
			//比如说stl会整个抹掉，但是很多题目都要求只去掉一个
			if (count(x))
				BIT<N>::add(x, -1);
		}
		int findNext(int x) const //lg^2
		{
			int base = BIT<N>::sum(x);
			if (BIT<N>::sum(BIT<N>::n) == base)
				return -1;
			return ds::bisearch(x + 1, BIT<N>::n + 1,
				[=](int pos) {return BIT<N>::sum(pos) > base; });
			//这里会找到第一个[x+1,pos]间的和不为0的pos，于是的到后继
		}
		int findPrev(int x) const
		{
			int base = BIT<N>::sum(x - 1);
			if (base == 0)
				return -1;
			return ds::bisearch(1, x,
				[=](int pos) {return BIT<N>::sum(pos) == base; });
			//返回第一个[1,pos]的和=[1,x-1]的和的pos
			//这样pos就是"引起最后一次改变"的位置，即前驱
		}
		int findMin() const { return findNext(0); }
		int findMax() const
		{
			if (exist(BIT<N>::n))
				return BIT<N>::n;
			return findPrev(BIT<N>::n);
		}
		int kth(int k) const
		{
			if (k > size() || k < 1)
				return -1;
			return ds::bisearch(1, BIT<N>::n + 1
				, [=](int pos) {return BIT<N>::sum(pos) >= k; });
		}
		//不要求x存在
		int rank(int x) const { return BIT<N>::sum(x - 1) + 1; }
		//求size需要lgN时间，最好保存下来
		int size() const { return BIT<N>::sum(BIT<N>::n); }
	};
}