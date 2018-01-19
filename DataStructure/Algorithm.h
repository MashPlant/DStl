#pragma once
#include <functional>
namespace ds
{
	template<typename Iter>
	Iter next(Iter it) { return ++it; }
	template<typename Iter>
	Iter prev(Iter it) { return --it; }
	template<typename Iter, typename Comp>
	void fixHeap_(Iter first,  Comp comp, int where, int heapSize)
	{
		int curMax = where, i = curMax, nxt = (i << 1) + 1;
		while (true)
		{
			if (nxt < heapSize && comp(first[curMax], first[nxt]))
				curMax = nxt;
			if (nxt + 1 < heapSize && comp(first[curMax], first[nxt + 1]))
				curMax = nxt + 1;
			if (curMax == i)
				break;
			std::swap(first[i], first[curMax]);
			i = curMax, nxt = (i << 1) + 1;
		}
	}
	template<typename Iter, typename Comp>
	void heapSort(Iter first, Iter last, Comp comp)
	{
		int heapSize = last - first;
		for (int i = heapSize >> 1; i >= 0; --i)
			fixHeap_(first, comp, i, heapSize);
		while (heapSize)
		{
			std::swap(first[0], first[--heapSize]);
			fixHeap_(first, comp, 0, heapSize);
		}
	}
	template<typename Iter>
	void heapSort(Iter first, Iter last)
	{
		ds::heapSort(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}

	template <typename Iter,typename T ,typename Comp>
	Iter partion(Iter first, Iter last, const T &pivot,Comp comp)
	{
		while (true) 
		{
			while (comp(*first, pivot))
				++first; //找出第一个>=pivot的first
			--last; 
			while (comp(pivot, *last))
				--last;  //找出第一个(倒着数)<=pivot的last
			if (first >= last)
				return first;
			std::swap(*first, *last);
			++first; //swap完了之后first指向的位置已经保证<=pivot，last同理
			//但是对last的修改在下一轮循环中进行，可以避免特判last是不是尾后迭代器
		}
	}
	template <typename T, typename Comp>
	T median_(const T& a, const T& b, const T& c,Comp comp)
	{
		if (comp(a,b))// a<b
		{
			if (comp(b, c)) //a<b<c
				return b;
			if (comp(a, c))
				return c;
			return a;
		}
		//a>=b
		if (comp(a, c)) //b<=a<c
			return a;
		if (comp(b, c)) 
			return c;
		return c;
	}
	inline int lg_(int x)
	{
		int ret = 0;
		for (; x; x >>= 1, ++ret);
		return ret;
	}
	template <typename Iter,typename Comp>
	void introSort_(Iter first,Iter last,Comp comp,int depth)
	{
		if (last - first <= 16)
			return;
		if (!depth) 
		{
			heapSort(first, last, comp);
			return;
		}
		const auto &pivot = median_(*first, *(first + (last - first) / 2), *(last - 1), comp);
		Iter div = partion(first, last, pivot, comp);
		introSort_(first, div, comp, depth - 1);
		introSort_(div, last, comp, depth - 1);
	}
	template <typename Iter, typename Comp>
	void insertSort(Iter first,Iter last,Comp comp)
	{
		Iter cur = first;
		while (cur!=last)
		{
			Iter tmp = cur;
			while (tmp != first&&comp(*tmp, *(tmp - 1)))
				swap(*tmp, *(tmp - 1)), --tmp;
			++cur;
		}
	}
	template<typename Iter,typename Comp>
	void sort(Iter first,Iter last,Comp comp)
	{
		introSort_(first, last, comp, 2 * lg_(last - first));
		insertSort(first, last, comp);
	}
	template<typename Iter>
	void sort(Iter first, Iter last)
	{
		ds::sort(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}
	inline void radixSort(int *first,int *last)
	{
		//只排正数，不管负数
		const static int U = 65536;
		static int cnt[U];
		auto mask = [](int x, int d) {return (x >> (d * 16))&(U - 1); };
		const int len = last - first;
		int *tmp = new int[len];
		for (int d=0;d<2;++d)
		{
			memset(cnt, 0, sizeof(cnt));
			for (int i = 0; i < len; ++i)
				++cnt[mask(first[i], d)];
			for (int i = 1; i < U; ++i)
				cnt[i] += cnt[i - 1];
			for (int i = len - 1; i >= 0; --i)//逆序，保证稳定
				tmp[--cnt[mask(first[i], d)]] = first[i];//cnt[mask(first[i], d)-1即是first[i]"应该放置"的位置
			memcpy(first, tmp, len * sizeof(int));
		}
		delete []tmp;
	}
	//返回第一个>=val的,也即是第一个满足!comp(*where,val)的where
	template<typename Iter, typename T, typename Comp>
	Iter lower_bound(Iter first, Iter last, const T &val, Comp comp)
	{
		while (first <= last) 
		{
			Iter mid = first + (last - first) / 2;
			if (comp(*mid, val))
				first = mid + 1;
			else
				last = mid - 1;
		}
		return first;
	}

	template<typename Iter,typename T>
	Iter lower_bound(Iter first, Iter last,const T &val)
	{
		return ds::lower_bound(first, last, val, std::less<T>());
	}

	//返回第一个>val的,也即是第一个满足comp(val,*where)的where
	template<typename Iter, typename T, typename Comp>
	Iter upper_bound(Iter first, Iter last, const T &val, Comp comp)
	{
		while (first <= last)
		{
			Iter mid = first + (last - first) / 2;
			if (comp(val, *mid))
				last = mid - 1;
			else
				first = mid + 1;
		}
		return first;
	}

	template<typename Iter, typename T>
	Iter upper_bound(Iter first, Iter last, const T &val)
	{
		return ds::upper_bound(first, last, val, std::less<T>());
	}

	//返回第一个满足check的，假定前半段不满足而后半段满足
	template<typename Iter, typename Check>
	Iter bisearch(Iter first, Iter last, Check check)
	{
		while (first <= last)
		{
			Iter mid = first + (last - first) / 2;
			if (check(*mid))
				last = mid - 1;
			else
				first = mid + 1;
		}
		return first;
	}

	template<typename Check>
	int bisearch(int first, int last, Check check)
	{
		while (first <= last)
		{
			int mid = first + (last - first) / 2;
			if (check(mid))
				last = mid - 1;
			else
				first = mid + 1;
		}
		return first;
	}

	template <typename Iter,typename Equal>
	Iter unique(Iter first,Iter last,Equal equal)
	{
		Iter uni = first;
		while (first!=last)
		{
			if (equal(*first, *uni))
				++first;
			else
				*(++uni) = *(first++);
		}
		return ++uni;
	}
	template <typename Iter>
	Iter unique(Iter first, Iter last)
	{
		return ds::unique(first, last, std::equal_to<typename std::iterator_traits<Iter>::value_type>());
	}
	template <typename Iter>
	void reverse(Iter first,Iter last)
	{
		--last;
		while (true)
		{
			if (first == last)
				return;
			std::swap(*(first++), *(last--));
			if (prev(first) == last)
				return;
		}
	}
	template <typename Iter, typename Comp>
	Iter rangeMax(Iter first,Iter last,Comp comp)
	{
		Iter maxIter = first;
		while (first!=last)
		{
			if (comp(*maxIter, *first))
				maxIter = first;
			++first;
		}
		return maxIter;
	}
	template <typename Iter>
	Iter rangeMax(Iter first, Iter last)
	{
		return rangeMax(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}
	template <typename Iter, typename Comp>
	Iter rangeMin(Iter first, Iter last, Comp comp)
	{
		Iter minIter = first;
		while (first != last)
		{
			if (comp(*first, *minIter))
				minIter = first;
			++first;
		}
		return minIter;
	}
	template <typename Iter>
	Iter rangeMin(Iter first, Iter last)
	{
		return rangeMin(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}
	template <typename Iter,typename Comp>
	bool next_permutation(Iter first,Iter last,Comp comp)
	{
		//从后向前找，找到第一个下降元素(倒着找的意义上的下降)，如找不到证明整个数列降序排列，无下一排列
		//将该元素与(该元素,last)间的最后一个(正序意义)小于该元素的元素交换，再将(用来换的元素,last)反向
		Iter bound = last;
		while (--last != first)
			if (comp(*prev(last),*last))
			{
				Iter firstDescend = prev(last);
				Iter lastGreater = bound;
				while (!(comp(*firstDescend, *--lastGreater)));
				std::swap(*firstDescend, *lastGreater);
				ds::reverse(last, bound);
				return true;
			}
		ds::reverse(first, bound);
		return false;
	}
	template <typename Iter>
	bool next_permutation(Iter first, Iter last)
	{
		return ds::next_permutation(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}
}