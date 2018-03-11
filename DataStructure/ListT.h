#pragma once
#include <type_traits>

namespace ds
{
	template <typename T>
	struct NullT
	{
		//NullT相当于空指针，空指针显然也可以有自己的类型
		typedef T ValueType;
	};
	template <typename T>
	struct IsNull //有些返回值不是NullT,而是继承自NullT,所以这里加一个判断
	{
		const static bool value = std::is_base_of<NullT<typename T::ValueType>, T>::value;
	};

	template <typename T, T...>
	struct ListT;

	template <typename T, T First, T... Items>
	struct ListT<T, First, Items...>
	{
		typedef ListT<T, Items...> Next;
		const static T value = First;
		typedef T ValueType;
	};

	template <typename T, T First>
	struct ListT<T, First>
	{
		typedef NullT<T> Next;
		const static T value = First;
		typedef T ValueType;
	};

	template <typename List, int Where>
	struct Get : Get<typename List::Next, Where - 1>
	{	};
	template <typename List>
	struct Get<List, 0> : List
	{	};

	template <typename List, typename List::ValueType Elem, int Where = 0,
		bool Found = List::value == Elem>
		struct Find : Find<typename List::Next, Elem, Where + 1>
	{	};
	template <typename List, typename List::ValueType Elem, int Where>
	struct Find<List, Elem, Where, true>
	{
		const static int value = Where;
	};

	template <typename List, int Cur = 0, bool Null = IsNull<List>::value>
	struct Output
	{
		using T = typename List::ValueType;
		template <int Index, T Value>
		struct Outputer; //利用编译器的错误信息来输出
		Outputer<Cur, List::value> impl{}; //貌似不加大括号强制初始化的话，输出的顺序就会乱，不知道为什么
		Output<typename List::Next, Cur + 1> next;
	};
	template <typename List, int Cur>
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
			const static T value = Iter::value;
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

	namespace setimpl
	{
		template <typename Return, typename Iter, int Where, typename Iter::ValueType Elem, bool Null = IsNull<Iter>::value>
		struct Impl : Impl<
			typename std::conditional<Where != 0, PushBack<Return, Iter::value>, PushBack<Return, Elem>>::type,
			typename Iter::Next, Where - 1, Elem>
		{	};
		template <typename Return, typename Iter, int Where, typename Iter::ValueType Elem>
		struct Impl<Return, Iter, Where, Elem, true> : Return
		{	};
		//如果where越界，按照上面的逻辑，会自动地不做任何处理
	}
	template <typename List, int Where, typename List::ValueType Elem>
	struct Set : setimpl::Impl<NullT<typename List::ValueType>, List, Where, Elem>
	{	};

	namespace eraseimpl
	{
		template <typename Return, typename Iter, int Where, bool Null = IsNull<Iter>::value>
		struct Impl : Impl<
			typename std::conditional<Where != 0, PushBack<Return, Iter::value>, Return>::type,
			typename Iter::Next, Where - 1>
		{	};
		template <typename Return, typename Iter, int Where>
		struct Impl<Return, Iter, Where, true> : Return
		{	};
	}
	template <typename List, int Where>
	struct Erase : eraseimpl::Impl<NullT<typename List::ValueType>, List, Where>
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

	namespace sortimpl
	{
		template <typename List, bool Null = IsNull<List>::value>
		struct Impl
		{
			using T = typename List::ValueType;
			const static T pivot = List::value;
			template <T I>
			struct Less { const static bool value = I < pivot; };
			template <T I>
			struct Equal { const static bool value = I == pivot; };
			template <T I>
			struct Greater { const static bool value = I > pivot; };
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
	struct Sort : sortimpl::Impl<List>::Type
	{	};
}