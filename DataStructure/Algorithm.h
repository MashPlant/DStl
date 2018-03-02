#pragma once
#include <functional>
#include <ratio>
#include <string>
namespace ds
{
	template<typename Iter>
	Iter next(Iter it) { return ++it; }
	template<typename Iter>
	Iter prev(Iter it) { return --it; }
	//返回第一个>=val的,也即是第一个满足!comp(*where,val)的where
	template<typename Iter, typename T, typename Comp>
	Iter lower_bound(Iter first, Iter last, const T &val, Comp comp)
	{
		--last;
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

	template<typename Iter, typename T>
	Iter lower_bound(Iter first, Iter last, const T &val)
	{
		return ds::lower_bound(first, last, val, std::less<T>());
	}

	//返回第一个>val的,也即是第一个满足comp(val,*where)的where
	template<typename Iter, typename T, typename Comp>
	Iter upper_bound(Iter first, Iter last, const T &val, Comp comp)
	{
		--last;
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

	//返回第一个满足check的,假定前半段不满足而后半段满足
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
	//注:stl的push_heap假定[first,last-1)为合法堆,push的新值是last-1
	//pop_heap假定[first,last)为合法堆，pop出来的值放在last-1
	template<typename Iter, typename Comp>
	void fixHeap_(Iter first, Comp comp, int where, int heapSize)//有且仅有where及其孩子违反堆性质，修复之
	{
		int nxt = (where << 1) + 1;
		auto value = std::move(first[where]);
		while (nxt < heapSize)
		{
			if (nxt + 1 < heapSize && comp(first[nxt], first[nxt + 1]))
				++nxt;
			if (comp(value, first[nxt]))
			{
				first[where] = first[nxt];
				where = nxt;
				nxt = (where << 1) + 1;;
			}
			else
				break;
		}
		first[where] = value;
	}
	template<typename Iter, typename Comp>
	void push_heap(Iter first, Iter last, Comp comp) //[first,last-1)为合法堆,push的新值是last-1
	{
		//stl实现,不用swap而是用赋值,稍微快一些
		auto value = std::move(*(last - 1));
		int hole = last - first - 1;//刚刚被move的下标
		int parent = (hole - 1) >> 1;
		while (hole && comp(*(first + parent), value))//value一直在向上浮动
		{
			*(first + hole) = std::move(*(first + parent));
			hole = parent;
			parent = (hole - 1) >> 1;
		}
		*(first + hole) = std::move(value);
	}
	template<typename Iter>
	void push_heap(Iter first, Iter last)
	{
		push_heap(first, last, std::less<typename std::iterator_traits<Iter>::value_type >());
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

	template <typename Iter, typename T, typename Comp>
	Iter partion(Iter first, Iter last, const T &pivot, Comp comp)
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
			++first; //swap完了之后first指向的位置已经保证<=pivot,last同理
					 //但是对last的修改在下一轮循环中进行,可以避免特判last是不是尾后迭代器
		}
	}
	template <typename T, typename Comp>
	T median_(const T& a, const T& b, const T& c, Comp comp)
	{
		if (comp(a, b))// a<b
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
		return b;
	}
	inline int lg_(int x)
	{
		int ret = 0;
		for (; x; x >>= 1, ++ret);
		return ret;
	}
	template <typename Iter, typename Comp>
	void introSort_(Iter first, Iter last, Comp comp, int depth)
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
	void insertSort(Iter first, Iter last, Comp comp)
	{
		Iter cur = first;
		while (cur != last)
		{
			Iter tmp = cur++;
			auto value = std::move(*tmp);
			if (comp(value,*first)) //首先与第一个比较，如果比第一个还小，那么直接移动
			{
				while (tmp != first) //这里就不用判断大小了
					*tmp = std::move(*(tmp - 1)), --tmp;
				*tmp = std::move(value);
				continue;
			}
			//这里就不用判断越界了
			while (comp(value, *(tmp - 1)))
				*tmp = std::move(*(tmp - 1)), --tmp;
			*tmp = std::move(value);
		}
	}
	template<typename Iter, typename Comp>
	void sort(Iter first, Iter last, Comp comp)
	{
		if (last - first <= 1)
			return;
		introSort_(first, last, comp, 2 * lg_(last - first));
		insertSort(first, last, comp);
	}
	template<typename Iter>
	void sort(Iter first, Iter last)
	{
		ds::sort(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
	}
	template<typename Iter, typename Comp>
	void merge(Iter first, Iter mid, Iter last, Comp comp, typename std::iterator_traits<Iter>::value_type* aux = nullptr) //归并[firts,mid)和[mid,last)并置于[)
	{
		typedef typename std::iterator_traits<Iter>::value_type value_type;
		first = ds::upper_bound(first, mid, *mid, comp); //所有<=*mid的都不用移动
		const bool naux = !aux;
		if (naux)
			aux = new value_type[last - first];
		Iter tmpmid = mid, tmpfirst = first;
		int pos = 0;
		while (first != tmpmid && mid != last)
		{
			if (comp(*mid, *first)) //稳定
				aux[pos++] = std::move(*(mid++));
			else
				aux[pos++] = std::move(*(first++));
		}
		while (first != tmpmid) aux[pos++] = std::move(*(first++));
		pos = 0;
		for (; tmpfirst != mid; ++tmpfirst, ++pos)
			*tmpfirst = std::move(aux[pos]);
		if (naux)
			delete[]aux;
	}
	template<typename Iter>
	void merge(Iter first, Iter mid, Iter last)
	{
		ds::merge(first, mid, last, std::less<>());
	}
	template<typename Iter, typename Comp>
	void stable_sort_impl1_(Iter first, Iter last, Comp comp)
	{
		const int len = last - first;
		if (len <= 1)
			return;
		typedef std::pair<typename std::iterator_traits<Iter>::value_type, int> SortPair;
		//核心思想：假装我没有动过对象
		SortPair *aux = (SortPair*)malloc((last - first) * sizeof(SortPair));
		for (int i = 0; i < len; ++i, ++first)
			new(aux + i) SortPair(std::move(*first), i + 0);//确保调用pair的两个参数都是右值引用的构造函数
		auto nComp = [comp](const SortPair &lhs, const SortPair &rhs) ->bool
		{
			if (comp(lhs.first, rhs.first))
				return true;
			if (comp(rhs.first, lhs.first))
				return false;
			return lhs.second < rhs.second;
		};
		ds::sort(aux, aux + len, nComp);
		--last;
		for (int i = len - 1; i >= 0; --i, --last) //仅仅是因为懒得存first，所以用一下last
			*last = std::move(aux[i].first);
		free(aux);//pair不需要析构
	}
	template<typename Iter, typename Comp>
	void merge_sort_impl_(Iter first, Iter last, Comp comp, typename std::iterator_traits<Iter>::pointer aux)
	{
		if (last - first <= 16)
		{
			ds::insertSort(first, last, comp);
			return;
		}
		const Iter m = first + (last - first) / 2;
		merge_sort_impl_(first, m, comp, aux);
		merge_sort_impl_(m, last, comp, aux);
		ds::merge(first, m, last, comp, aux);
	}
	template<typename Iter, typename Comp>
	void stable_sort_impl2_(Iter first, Iter last, Comp comp)
	{
		typedef typename std::iterator_traits<Iter>::value_type value_type;
		const Iter mid = first + (last - first) / 2;
		value_type *aux = new value_type[last - first];
		ds::merge_sort_impl_<Iter, Comp>(first, mid, comp, aux);
		ds::merge_sort_impl_<Iter, Comp>(first, mid, last, comp, aux);
		ds::merge(first, mid, last, comp, aux);
		delete[]aux;
	}
	template<typename Iter>
	void stable_sort(Iter first, Iter last)
	{
		ds::stable_sort_impl2_(first, last, std::less<>());
	}
	inline void radixSort(int *first, int *last)
	{
		//只排正数,不管负数
		const static int U = 65536;
		static int cnt[U];
		auto mask = [](int x, int d) {return (x >> (d * 16))&(U - 1); };
		const int len = last - first;
		int *tmp = new int[len];
		for (int d = 0; d<2; ++d)
		{
			memset(cnt, 0, sizeof(cnt));
			for (int i = 0; i < len; ++i)
				++cnt[mask(first[i], d)];
			for (int i = 1; i < U; ++i)
				cnt[i] += cnt[i - 1];
			for (int i = len - 1; i >= 0; --i)//逆序,保证稳定
				tmp[--cnt[mask(first[i], d)]] = first[i];//cnt[mask(first[i], d)-1即是first[i]"应该放置"的位置
			memcpy(first, tmp, len * sizeof(int));
		}
		delete[]tmp;
	}

	template <typename Iter, typename Equal>
	Iter unique(Iter first, Iter last, Equal equal)
	{
		Iter uni = first;
		while (first != last)
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
	void reverse(Iter first, Iter last)
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
	Iter rangeMax(Iter first, Iter last, Comp comp)
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
	template <typename Iter, typename Comp>
	bool next_permutation(Iter first, Iter last, Comp comp)
	{
		//从后向前找,找到第一个下降元素(倒着找的意义上的下降),如找不到证明整个数列降序排列,无下一排列
		//将该元素与(该元素,last)间的最后一个(正序意义)小于该元素的元素交换,再将(用来换的元素,last)反向
		Iter bound = last;
		while (--last != first)
			if (comp(*prev(last), *last))
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

	inline int kmp(const std::string &str, const std::string &pat)
	{
		//build next
		int *next = new int[pat.size()];	//next[i]:=pat[0...i]的最长公共前/后缀(既是前缀也是后缀)
		next[0] = 0;
		int pos = 1, k = 0;
		for (; pos<pat.size(); ++pos)
		{
			while (k && pat[k] != pat[pos]) //如pat[k]==pat[pos]，即证明[0...k]与[pos-k+1...pos]完全相同
				k = next[k - 1];
			if (pat[pos] == pat[k]) ++k; //可能是因为k==0跳出循环，所以还要判一下
			next[pos] = k;
		}
		for (pos = k = 0; pos < str.size(); ++pos)
		{
			while (k && str[pos] != pat[k])
				k = next[k - 1];//匹配就很简单，k和pos各+1；不匹配？k需要向前移动才可能匹配；
								//因为最后一位已经失配，必然要用到前k-1个，假设存在一个(next[k-1],k)间的位置x能够使pat[0,x]和str[pos-k,pos-k+x]完全匹配，那么next[k-1]就应该=x+1，矛盾
			if (str[pos] == pat[k]) ++k;
			if (k == pat.size())
				break;
		}
		delete[]next;
		return k == pat.size() ? pos - k + 1 : -1;
	}

	template<typename K>
	void bitwiseSwap(K *lhs,K *rhs) noexcept
	{
		//这并不是什么奇技淫巧，也完全没有UB(只要不拿来swap带虚函数表的)
		//主要是为了避免忘记swap某些成员
		char *cl = reinterpret_cast<char *>(lhs), *cr = reinterpret_cast<char *>(rhs);
		int bit = sizeof(K);
		while (bit--)
		{
			char tmp = *cl;
			*cl++ = *cr;
			*cr++ = tmp;
		}
	}
}
