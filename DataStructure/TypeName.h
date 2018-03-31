#pragma once
#include "ArrayT.h"
namespace ds
{
	template <typename T,typename = void> //for SFINAE
	struct TypeName { constexpr static CArrayT< 'u', 'n', 'd', 'e', 'f', 'i', 'n', 'e', 'd'> name{}; };

	template <> struct TypeName<int> { constexpr static CArrayT< 'i', 'n', 't'> name{}; };
	template <> struct TypeName<char> { constexpr static CArrayT< 'c', 'h', 'a', 'r'> name{}; };
	template <> struct TypeName<double> { constexpr static CArrayT< 'd', 'o', 'u', 'b', 'l', 'e'> name{}; };
	template <> struct TypeName<void> { constexpr static CArrayT< 'v', 'o', 'i', 'd'> name{}; };
	template <> struct TypeName<float> { constexpr static CArrayT< 'f', 'l', 'o','a', 't'> name{}; };

	//const数组在匹配时发生了冲突，这里用SFINAE强制规定顺序
	template <typename T>
	struct TypeName<const T, typename std::enable_if<!std::is_array<T>::value>::type>
	{
		constexpr static auto name = CArrayT< 'c', 'o', 'n', 's', 't', ' '>{}+TypeName<T>::name;
	};

	template <typename T>
	struct TypeName<T *>
	{
		constexpr static auto name = CArrayT< 'p', 'o', 'i', 'n', 't', 'e', 'r', ' ', 't', 'o', ' ' >{}+TypeName<T>::name;
	};
	template <typename T>
	struct TypeName<T &>
	{
		constexpr static auto name = CArrayT< 'r', 'e', 'f', 'e', 'r', 'e', 'n', 'c', 'e', ' ', 'o', 'f', ' ' >{} +TypeName<T>::name;
	};
	template <typename T>
	struct TypeName<T &&>
	{
		constexpr static auto name = CArrayT<'r', '-', 'r', 'e', 'f', 'e', 'r', 'e', 'n', 'c', 'e', ' ', 'o', 'f', ' ' >{} +TypeName<T>::name;
	};

	template <typename T, int N>
	struct TypeName<T[N]>
	{
		constexpr static auto name = CArrayT<'a', 'r', 'r', 'a', 'y', '['>{} +ParseUInt<N>::name +
			CArrayT<']', ' ', 'o', 'f', ' ' >{}+TypeName<T>::name;
	};

	template <typename ...>
	struct TypeNames;
	template <typename First, typename ...Args>
	struct TypeNames<First, Args...>
	{
		constexpr static auto name = TypeName<First>::name + CArrayT<','>{} +TypeNames<Args...>::name;
	};
	template <typename First>
	struct TypeNames<First> { constexpr static auto name = TypeName<First>::name; };
	template <>
	struct TypeNames<> { constexpr static ArrayT<char> name{}; };

	template <typename Ret, typename ...Args>
	struct TypeName<Ret(Args...)>
	{
		constexpr static auto name = CArrayT< 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', ' ', '('>{} +TypeNames<Args...>::name +
			CArrayT< ')', ' ', 'r', 'e', 't', 'u', 'r', 'n', ' '>{}+TypeName<Ret>::name;
	};
}