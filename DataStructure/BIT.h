#pragma once
namespace ds
{
	/*
	 *树状数组拓展
	 *1.区间修改单点求和:维护差分数组，b[i]:=a[i]-a[i-1]
	 *修改:只需修改两个b；求值:求b[1] to b[x]的和
	 *2.区间修改区间求和:同样基于上面的原理
	 *求值:a[1]+...+a[x] = b[1]*x + b[2]*(x-1) +...+ b[x] 
	 *=(b[1]+...+b[x])*x-(b[1]*0+b[2]*1+...+b[x]*(x-1))
	 *需维护b[i]*(i-1)数组，维护的时候也只用修改两个点
	 *3.模拟平衡树:见BITTree.h
	 *插入+1/删除-1
	 *前驱/后继/kth => 二分 O(lg^2(n))，但是常数小
	 */
	//[1,N] indexed
	template <int N>
	class BIT
	{
		int arr[N + 1] = { 0 };
		struct Reference
		{
			BIT *self;
			int pos;
			void operator+=(int d) { self->add(pos, d); }
			void operator-=(int d) { self->add(pos, -d); }
			int operator-(const Reference &rhs) const { return self->query(rhs.pos, pos); }
		};
	protected:
		int n;
	public:
		BIT(int n = N) :n(n) {}
		int sum(int pos) const
		{
			int ret = 0;
			while (pos)
			{
				ret += arr[pos];
				pos &= pos - 1; //等价于pos -= pos & -pos
			}
			return ret;
		}
		int query(int l,int r) const
		{
			//查询[l,r]间的和，需要sum到l-1
			return sum(r) - sum(l - 1);
		}
		void add(int pos,int d)
		{
			while (pos <= n)
			{
				arr[pos] += d;
				pos += pos & -pos;
			}
		}
		Reference operator[](int pos) { return { this,pos }; }
	};
}