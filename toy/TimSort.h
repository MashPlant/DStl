#pragma once
#include "Algorithm.h"
#include <functional>
namespace
{
	template<typename Iter, typename Comp = std::less<>>
	class TimSort
	{
	private:
		typedef typename std::iterator_traits<Iter>::value_type value_type;
		const Iter first, last;
		Comp comp;
		const static int MIN_MERGE = 32;
		int stackSize = 0;
		int *runFirst, *runLen;
		value_type *aux;
		//一个run指一段连续不降的或者连续严格降的子串
		//countRunAndMakeAscending找出以_first开头，不超过last-1的最长run；并且如果是连续降，顺便反转之
		int countRunAndMakeAscending(Iter _first) //似乎可以去掉第二个参数?
		{
			int lo = 0, runHi = 1, hi = last - _first;
			if (_first + 1 == last)
				return 1;
			if (comp(_first[runHi++], _first[lo]))//下降
			{
				while (runHi < hi && comp(_first[runHi], _first[runHi - 1]))
					runHi++;
				ds::reverse(_first, _first + runHi);
			}
			else                              //不降
				while (runHi < hi && !comp(_first[runHi], _first[runHi - 1]))
					runHi++;
			return runHi - lo;
		}
		static int minRunLength(int n)
		{
			int r = 0;
			while (n >= MIN_MERGE)
			{
				r |= n & 1;
				n >>= 1;
			}
			return n + r;
		}
		void insertSort(Iter _first, Iter _last, int div = 0) //[_first,_first + _div)已经排好序了
		{
			Iter cur = _first + div;
			while (cur != _last)
			{
				Iter tmp = cur++;
				auto value = std::move(*tmp);
				while (tmp != _first&&comp(value, *(tmp - 1)))
					*tmp = std::move(*(tmp - 1)), --tmp;
				*tmp = std::move(value);
			}
		}
		void pushRun(int _first, int len)
		{
			runFirst[stackSize] = _first;
			runLen[stackSize] = len;
			++stackSize;
		}
		void mergeAt(int n) //把栈中的下标n和n+1归并,n <= stackSize-2
		{
			//assert(runFirst[n + 1] == runFirst[n] + runLen[n])
			runLen[n] += runLen[n + 1];
			if (n == stackSize - 3) //挪动一位
				runFirst[n + 1] = runFirst[n + 2], runLen[n + 1] = runLen[n + 2];
			--stackSize;
			ds::merge(first + runFirst[n], first + runFirst[n + 1], first + runFirst[n + 1] + runLen[n + 1], comp, aux);
		}
		void mergeCollapse()
		{
			while (stackSize > 1)
			{
				int n = stackSize - 2;
				if (n > 0 && runLen[n - 1] <= runLen[n] + runLen[n + 1])
				{
					if (runLen[n - 1] < runLen[n + 1])
						--n;
					mergeAt(n);
				}
				else if (runLen[n] <= runLen[n + 1])
					mergeAt(n);
				else break;
			}
		}
	public:
		TimSort(Iter first, Iter last, Comp comp = Comp()) :first(first), last(last), comp(comp)
		{
			const int len = last - first;
			const int stackLen = len <    120 ? 5 :
				len <   1542 ? 10 :
				len < 119151 ? 24 : 49;
			runFirst = new int[stackLen], runLen = new int[stackLen];
			aux = new value_type[len];
		}
		void sort()
		{
			int len = last - first, lo = 0, hi = len;
			if (len <= 1) return;
			if (len < MIN_MERGE)
			{
				insertSort(first, last, 0);
				return;
			}
			const int minRun = minRunLength(len);
			do
			{
				int curLen = countRunAndMakeAscending(first + lo);
				if (curLen < minRun)
				{
					const int force = len <= minRun ? len : minRun;//避免越界 
					insertSort(first + lo, first + lo + force, curLen);
					curLen = force;
				}
				pushRun(lo, curLen);
				mergeCollapse();
				lo += curLen;
				len -= curLen;
			} while (len != 0);
			while (stackSize > 1)
				mergeAt(stackSize - 2);
		}
		~TimSort()
		{
			delete runFirst;
			delete runLen;
			delete aux;
		}
	};
}
