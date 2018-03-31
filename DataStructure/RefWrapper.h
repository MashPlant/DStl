#pragma once
#include <type_traits>
namespace ds
{
	//RefPair��Ŀ����:
	//�����������һ������������������ı���ָ��Ķ���
	template <typename F, typename S, int = !std::is_rvalue_reference<F &&>::value * 2 + !std::is_rvalue_reference<S &&>::value>
	struct RefPair
	{
		F &first;
		S &second;
		//�滻���õĶ���
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
			//���Ի�ȡ���õĵ�ַ��õ�����ָ��Ķ���ĵ�ַ�������Ǵ����
			//���õĴ�С��ͬ��ָ��Ĵ�С
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
		//���value����ֵ����T�ƶ�Ϊ ���κ����ε����ͣ�T&&Ϊ��ֵ����
		//����T�ƶ�Ϊ��ֵ���ã�T&&���������۵����ȼ���T&
		return SmartRef<T>{std::forward<T>(value)};
	}
}
