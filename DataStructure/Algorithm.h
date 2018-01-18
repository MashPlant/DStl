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
		int curMax = where, i = curMax;
		while (true)
		{
			if ((i << 1) + 1 < heapSize && comp(first[curMax], first[(i << 1) + 1]))
				curMax = (i << 1)+1;
			if ((i << 1) + 2 < heapSize && comp(first[curMax], first[(i << 1) + 2]))
				curMax = (i << 1) + 2;
			if (curMax == i)
				break;
			std::swap(first[i], first[curMax]);
			i = curMax;
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
				++first; //�ҳ���һ��>=pivot��first
			--last; 
			while (comp(pivot, *last))
				--last;  //�ҳ���һ��(������)<=pivot��last
			if (first >= last)
				return first;
			std::swap(*first, *last);
			++first; //swap����֮��firstָ���λ���Ѿ���֤<=pivot��lastͬ��
			//���Ƕ�last���޸�����һ��ѭ���н��У����Ա�������last�ǲ���β�������
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

	//���ص�һ��>=val��,Ҳ���ǵ�һ������!comp(*where,val)��where
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

	//���ص�һ��>val��,Ҳ���ǵ�һ������comp(val,*where)��where
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

	//���ص�һ������check�ģ��ٶ�ǰ��β��������������
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
			if (first==last)
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
			if (comp(*first, *maxIter))
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
		Iter maxIter = first;
		while (first != last)
		{
			if (comp(*maxIter, *first))
				maxIter = first;
			++first;
		}
		return maxIter;
	}
	template <typename Iter>
	Iter rangeMin(Iter first, Iter last)
	{
		return rangeMin(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}
	/*template <typename Iter,typename Comp>
	bool next_permutation(Iter first,Iter last,Comp comp)
	{
		//�Ӻ���ǰ�ң��ҵ���һ���½�Ԫ��(�����ҵ������ϵ��½�)�����Ҳ���֤���������н������У�����һ����
		//����Ԫ����(��Ԫ��,last)�����СԪ�ؽ������ٽ�(��������Ԫ��,last)����
		while (last!=first)
		{
			if (comp(prev(last),last))
			{
				
			}
			//if (comp(*(last-1)))
		}
	}*/
}