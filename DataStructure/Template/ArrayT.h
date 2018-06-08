#pragma once
#include <ostream>

namespace ds
{
	template <typename T, T ...Elems>
	struct ArrayT
	{
	private:
		constexpr static T elems[sizeof...(Elems)] = { Elems... };
	public:
		constexpr T operator[](int index) { return elems[index]; }
		constexpr static int size() { return sizeof...(Elems); }
		template <T ...RhsElems>
		constexpr auto operator+(ArrayT<T, RhsElems...> rhs) const
		{
			return ArrayT<T, Elems..., RhsElems...>{};
		}
	};
	template <char ...Chs>
	using CArrayT = ArrayT<char, Chs...>;

//	template <const char * const Str, typename = void>
//	struct ParseStr
//	{
//		constexpr static auto name =/* ParseStr<Str + 1>::name + */ CArrayT<Str[0]>{};
//	};
//	template <const char * const Str>
//	struct ParseStr<Str, typename std::enable_if<!Str[1]>::type>
//	{
//		constexpr static auto name = CArrayT<*Str>{};
//	};

	template <unsigned N, typename = void>
	struct ParseUInt
	{
		constexpr static auto name = ParseUInt<N / 10>::name + ArrayT<char, N % 10 + '0'>{};
	};
	template <unsigned N>
	struct ParseUInt<N, typename std::enable_if<(N<10)>::type>
	{
		constexpr static auto name = ArrayT<char, N + '0'>{};
	};

	template <typename T, T ...Elems>
	std::ostream& operator<<(std::ostream &os, ArrayT<T, Elems...> arr)
	{
		for (int i = 0; i < arr.size(); ++i)
			os << arr[i] << ' ';
		return os;
	}
	template <char ...Chs>
	std::ostream& operator<<(std::ostream &os, ArrayT<char, Chs...> arr)
	{
		for (int i = 0; i < arr.size(); ++i)
			os << arr[i];
		return os;
	}
}