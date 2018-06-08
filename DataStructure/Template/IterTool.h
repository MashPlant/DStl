#pragma once
#include <type_traits>
#include "TypeName.h"
#include "RefWrapper.h"

/**
* ԭ�ȵ��뷨��Ϊconst�ͷ�const���������غ���������ʹ��ͳһ��ģ����
* ����begin��end����Ҳ��������const�ͷ�const�����汾��̫�鷳�ˣ���������
* ����ȫ����const��Ҳ����˵����ⲻ���޸�����Ԫ��
*
* Ϊ��ʵ����ϵ��﷨������slice(range(10),1,5)������Ҫ�ӳ���ʱ����range(10)������
* ��Ȼ���ܴ�����������slice������ʱ��ͻᷢ�������ĸ���
* ��Ȼ��python֮��֮���Կ�������д������Ϊ��ʱ������������Ƿ������������й�
* ���Կ���ʹ������ָ��
*
*
* һ��ʼ� :
* ��ͨ�� <T> f(T &t) ���԰�const�ͷ�const�����ǲ��ܰ���ֵ
* <T> f(const T &t) ���԰����е�
* initializer_list���ܱ�ģ�庯���ƶ�(���ǿ��Ա�auto�ƶ�)�����������ʽд��initializer_list�İ汾
*/

namespace ds
{
#define NewInstance(Class) \
template<typename... Args> \
static const Class &newInstance(Args... args) { return self = Class(args...); }

	namespace detail
	{
		template <typename T, typename = decltype(std::declval<T>()[0])>
		std::true_type tryIndex(int);
		template <typename T>
		std::false_type tryIndex(float);

		template <typename T, typename = decltype(std::begin(std::declval<T>()))>
		std::true_type tryIter(int);
		template <typename T>
		std::false_type tryIter(float);
	}
	template <typename T>
	using IndexAble = decltype(detail::tryIndex<T>(0));
	template <typename T>
	using IterAble = decltype(detail::tryIter<T>(0));
	template <typename V>
	struct BaseIt
	{
		typedef std::input_iterator_tag iterator_category;
		typedef V value_type;
		typedef ptrdiff_t difference_type;
		typedef V *pointer;
		typedef V &reference;
	};

	class range
	{
		int first, last, step;
		struct RangeIt : BaseIt<const int>
		{
			int cur, step;
			RangeIt(int cur, int step) : cur(cur), step(step) {}
			RangeIt &operator++()
			{
				cur += step;
				return *this;
			}
			int operator*() const { return cur; }
			bool operator==(const RangeIt &rhs) const { return !operator!=(rhs); }
			bool operator!=(const RangeIt &rhs) const { return step > 0 ? cur < rhs.cur : cur > rhs.cur; }
		};
	public:
		range(int first, int last, int step = 1) : first(first), last(last), step(step) {}
		range(int last) : first(0), last(last), step(1) {}
		RangeIt begin() const { return RangeIt(first, step); }
		RangeIt end() const { return RangeIt(last, step); }
	};


	template <typename T,typename Pred>
	class Filter
	{
		using Iter = decltype(std::begin(std::declval<T>()));
		using ValueRef = decltype(*std::declval<Iter>());
		SmartRef<T> seq;
		Pred pred;
		struct FilterIt : BaseIt<std::remove_reference<ValueRef>>
		{
			Iter pos;
			const Filter *parent;
			//fuck������const���Ե�һ��
			FilterIt(Iter pos, const Filter *parent) : pos(pos), parent(parent) {}
			FilterIt &operator++()
			{
				++pos;
				while (pos != std::end(parent->seq.value) && !parent->pred(**this))
					++pos;
				return *this;
			}
			ValueRef operator*() { return *pos; }
			bool operator==(const FilterIt &rhs) const { return pos == rhs.pos; }
			bool operator!=(const FilterIt &rhs) const { return pos != rhs.pos; }
		};
	public:
		//��֪��Ϊʲô�����ﴫ��������const T &�����Ǽ�ʹseq������ò���const������Ҳ���Ը�ֵ
		Filter(const T &seq, Pred pred) :seq{ seq }, pred(pred){}
		FilterIt begin() const
		{
			FilterIt it(std::begin(seq.value), this);
			if (!pred(*it))
				++it;
			return it;
		}
		FilterIt end() const { return FilterIt(std::end(seq.value), this); }
	};
	template <typename T,typename Pred>
	auto filter(T &&seq, Pred pred) { return Filter<T, Pred>(seq, pred); }
	template <typename T1,typename T2>
	class Zip
	{
		using It1 = decltype(std::begin(std::declval<T1>()));
		using It2 = decltype(std::begin(std::declval<T2>()));
		using V1 = decltype(*std::declval<It1>());
		using V2 = decltype(*std::declval<It2>());
		struct ZipIt : BaseIt<RefPair<V1, V2>>
		{
			using ItPair = std::pair<It1, It2>;
			ItPair pos;
			RefPair<V1, V2> val;
			ZipIt(It1 it1, It2 it2) :pos(it1, it2), val{ *it1, *it2 } {}
			ZipIt& operator++()
			{
				pos = ItPair(++pos.first, ++pos.second);
				val.assign(makeRefPair(*pos.first, *pos.second));
				return *this;
			}
			bool operator==(const ZipIt &rhs) { return pos.first == rhs.pos.first; }
			bool operator!=(const ZipIt &rhs) { return !operator==(rhs); }
			RefPair<V1, V2> operator*() { return val; }
		};
		SmartRef<T1> t1;
		SmartRef<T2> t2;
	public:
		Zip(const T1 &t1, const T2 &t2) :t1{ t1 }, t2{ t2 } {}
		ZipIt begin() const
		{
			auto beg = std::begin(t1.value);
			auto x = *std::begin(t1.value);
			return ZipIt(std::begin(t1.value), std::begin(t2.value));
		}
		ZipIt end() const
		{
			auto dis1 = std::distance(std::begin(t1.value), std::end(t1.value));
			auto dis2 = std::distance(std::begin(t2.value), std::end(t2.value));
			if (dis1 < dis2)
				return ZipIt(std::end(t1.value), std::next(std::begin(t2.value), dis1));
			return ZipIt(std::next(std::begin(t1.value), dis2), std::end(t2.value));
		}
	};
	template <typename T1,typename T2>
	auto zip(T1 &&t1, T2 &&t2) { return Zip<T1, T2>(t1, t2); }
	
	template <typename T>
	class Slice
	{
		using Iter = decltype(std::begin(std::declval<T>()));
		using ValueRef = decltype(*std::declval<Iter>());
		SmartRef<T> seq;
		int first, last;
		struct SliceIt : BaseIt<std::remove_reference_t<ValueRef>>
		{
			Iter pos;
			SliceIt(Iter pos) :pos(pos) { }
			SliceIt &operator++() { ++pos; return *this; }
			ValueRef operator*() { return *pos; }
			bool operator==(const SliceIt &rhs) { return pos == rhs.pos; }
			bool operator!=(const SliceIt &rhs) { return pos != rhs.pos; }
		};
	public:
		Slice(const T &seq, int first, int last) :seq{ seq }, first(first), last(last) {}
		SliceIt begin() const { return std::next(std::begin(seq.value), first); }
		SliceIt end() const { return std::next(std::begin(seq.value), last); }
	};
	template <typename T>
	auto slice(T &&seq, int first, int last) { return Slice<T>(seq, first, last); }

	template <typename T>
	class Repeat
	{
		using Iter = decltype(std::begin(std::declval<T>()));
		using ValueRef = decltype(*std::declval<Iter>());
		SmartRef<T> seq;
		unsigned times = 1;
		struct RepeatIt : BaseIt<std::remove_reference<ValueRef>>
		{
			Iter pos;
			const Repeat *parent;
			int time;
			RepeatIt(Iter pos, const Repeat *parent, int time) :pos(pos), parent(parent), time(time) {}
			RepeatIt &operator++()
			{
				if (++pos == std::end(parent->seq.value))
					pos = std::begin(parent->seq.value), ++time;
				return *this;
			}
			ValueRef operator*() { return *pos; }
			bool operator==(const RepeatIt &rhs) const { return pos == rhs.pos && time == rhs.time; }
			bool operator!=(const RepeatIt &rhs) const { return pos != rhs.pos || time != rhs.time; }
		};
	public:
		Repeat(const T &seq, int times) :seq{ seq }, times(times){}
		RepeatIt begin() const { return RepeatIt(std::begin(seq.value), this, 0); }
		RepeatIt end() const { return RepeatIt(std::begin(seq.value), this, times); }
	};
	template <typename T>
	auto repeat(T &&seq, unsigned times = 1u) { return Repeat<T>(seq, times); }
	template <typename T>
	auto repeat(const std::initializer_list<T> &seq, unsigned times = 1u) { return Repeat<std::initializer_list<T>>(seq, times); }
}
