#pragma once
#include <type_traits>
namespace ds
{
	//RefPair的目的是:
	//把引用像对象一样存下来，并且允许改变它指向的对象
	template <typename F, typename S, int = !std::is_rvalue_reference<F &&>::value * 2 + !std::is_rvalue_reference<S &&>::value>
	struct RefPair
	{
		F &first;
		S &second;
		//替换引用的对象
		void assign(const RefPair &rhs) { memcpy(this, &rhs, sizeof(RefPair)); }
	};

	template <typename F, typename S>
	struct RefPair<F, S, 0>
	{
		F first;
		S second;
		void assign(const RefPair &rhs) { *this = rhs; }
	};

	template <typename F, typename S>
	struct RefPair<F, S, 1>
	{
		F first;
		S &second;
		void assign(const RefPair &rhs)
		{
			first = rhs.first;
			//尝试获取引用的地址会得到引用指向的对象的地址，那样是错误的
			//引用的大小等同于指针的大小
			memcpy(&first + 1, &rhs.first + 1, sizeof(void *));
		}
	};

	template <typename F, typename S>
	struct RefPair<F, S, 2>
	{
		F &first;
		S second;
		void assign(const RefPair &rhs)
		{
			memcpy(this, &rhs, sizeof(void *));
			second = rhs.second;
		}
	};

	template <typename F,typename S>
	auto makeRefPair(F &&first,S &&second)
	{
		return RefPair<F, S>{std::forward<F>(first), std::forward<S>(second)};
	}

	template <typename T, typename = void>
	struct SmartRef
	{
		T &value;
	};
	template <typename T>
	struct SmartRef<T, std::enable_if_t<std::is_rvalue_reference<T &&>::value>>
	{
		T value;
	};

	template <typename T>
	auto makeRef(T &&value)
	{
		//如果value是右值，则T推断为 无任何修饰的类型，T&&为右值引用
		//否则T推断为左值引用，T&&经过引用折叠，等价于T&
		return SmartRef<T>{std::forward<T>(value)};
	}
}
