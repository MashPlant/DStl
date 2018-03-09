#pragma once
#include <type_traits>

namespace ds
{
	template <typename T>
	struct NullT
	{
		//NullT�൱�ڿ�ָ�룬��ָ����ȻҲ�������Լ�������
		typedef T ValueType;
	};
	template <typename T>
	struct IsNull //��Щ����ֵ����NullT,���Ǽ̳���NullT,���������һ���ж�
	{
		const static bool value = std::is_base_of<NullT<typename T::ValueType>, T>::value;
	};

	template <typename T, T...>
	struct ListT;

	template <typename T, T First, T... Items>
	struct ListT<T, First, Items...>
	{
		typedef ListT<T, Items...> Next;
		const static int value = First;
		typedef T ValueType;
	};

	template <typename T, T First>
	struct ListT<T, First>
	{
		typedef NullT<T> Next;
		const static int value = First;
		typedef T ValueType;
	};

	template <typename List, int Where>
	struct Get : Get<typename List::Next, Where - 1>
	{	};
	template <typename List>
	struct Get<List, 0> : List
	{	};

	template <typename List, int Cur = 0,bool Null = IsNull<List>::value>
	struct Output
	{
		using T = typename List::ValueType;
		template <int Index, T>	
		struct Outputer; //���ñ������Ĵ�����Ϣ�����
		Outputer<Cur, List::value> impl{}; //ò�Ʋ��Ӵ�����ǿ�Ƴ�ʼ���Ļ��������˳��ͻ��ң���֪��Ϊʲô
		Output<typename List::Next, Cur + 1> next;
	};
	template <typename List,int Cur>
	struct Output<List, Cur, true>
	{	};

	template <typename List, bool Null = IsNull<List>::value>
	struct Size
	{
		const static int value = Size<typename List::Next>::value + 1;
	};
	template <typename List>
	struct Size<List, true>
	{
		const static int value = 0;
	};

	template <typename List, typename List::ValueType Elem>
	struct PushFront
	{
		typedef List Next;
		const static typename List::ValueType value = Elem;
		typedef typename List::ValueType ValueType;
	};

	namespace pbimpl
	{
		template <typename T, typename Iter, T Elem, bool Null = IsNull<Iter>::value>
		struct Impl
		{
			typedef Impl<T, typename Iter::Next, Elem> Next;
			const static int value = Iter::value;
			typedef T ValueType;
		};
		template <typename T, typename Iter, T Elem>
		struct Impl<T, Iter, Elem, true>
		{
			const static T value = Elem;
			typedef NullT<T> Next;
			typedef T ValueType;
		};
	}
	template <typename List, typename List::ValueType Elem>
	struct PushBack : pbimpl::Impl<typename List::ValueType, List, Elem>
	{	};

	namespace mapimpl
	{
		template <typename T, typename Return, typename Iter, template <T> class Pred, bool Null = IsNull<Iter>::value>
		struct Impl : Impl<T, PushBack<Return, Pred<Iter::value>::value>, typename Iter::Next, Pred>
		{	};
		template <typename T, typename Return, typename Iter, template <T> class Pred>
		struct Impl<T, Return, Iter, Pred, true> : Return
		{	};
	}
	template <typename List, template <typename List::ValueType> class Pred>
	struct Map : mapimpl::Impl<typename List::ValueType, NullT<typename List::ValueType>, List, Pred>
	{	};

	namespace filimpl
	{
		template <typename T, typename Return, typename Iter, template <T> class Pred, bool Null = IsNull<Iter>::value>
		struct Impl : Impl<T,
			typename std::conditional<Pred<Iter::value>::value, PushBack<Return, Iter::value>, Return>::type,
			typename Iter::Next, Pred>
		{	};
		template <typename T, typename Return, typename Iter, template <T> class Pred>
		struct Impl<T, Return, Iter, Pred, true> : Return
		{	};
	}
	template <typename List, template <typename List::ValueType> class Pred>
	struct Filter : filimpl::Impl<typename List::ValueType, NullT<typename List::ValueType>, List, Pred>
	{	};

	namespace conimpl
	{
		template <typename L1, typename L2, bool Null = IsNull<L2>::value>
		struct Impl : Impl<PushBack<L1, L2::value>, typename L2::Next>
		{	};
		template <typename L1, typename L2>
		struct Impl<L1, L2, true> : L1
		{	};
	}
	template <typename L1, typename L2>
	struct Contact : conimpl::Impl<L1, L2>
	{	};

	namespace qsortimpl
	{
		template <typename List, bool Null = IsNull<List>::value>
		struct Impl
		{
			using T = typename List::ValueType;
			const static T pivot = List::value;
			template <T I>
			struct Less
			{
				const static bool value = I < pivot;
			};
			template <T I>
			struct Equal
			{
				const static bool value = I == pivot;
			};
			template <T I>
			struct Greater
			{
				const static bool value = I > pivot;
			};
			using L1 = Filter<List, Less>;
			using L2 = Filter<List, Equal>;
			using L3 = Filter<List, Greater>;
			using Type = Contact<Contact<typename Impl<L1>::Type, L2>, typename Impl<L3>::Type>;
		};
		template <typename List>
		struct Impl<List, true>
		{
			using Type = List;
		};
	}
	template <typename List>
	struct QSort : qsortimpl::Impl<List>::Type
	{	};
}