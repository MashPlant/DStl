#pragma once
#include <functional>
#include "Util.h"
namespace ds
{
	namespace Seg
	{
		template <typename K,typename Func,int Left,int Right>
		struct Node
		{
			K key;
			K label;
			Node<K, Func,Left, Left + (Right - Left) / 2> left; //��ʹ�ö�ʽ����
			Node<K, Func,Left + (Right - Left) / 2 + 1, Right> right;
			template <typename Iter>
			Node(Iter &it) : label(K()), left(it), right(it)
			{
				key = Func()(left.key, right.key);
			}
		};
		template <typename K,typename Func,int Left>
		struct Node<K,Func,Left,Left>
		{
			//��ײ��Ҷ�Ӳ�����Ҫ��ǩ
			K key;
			template <typename Iter>
			Node(Iter &it) :key(*it++) {} //����ʼ���ȳ�ʼ������ٳ�ʼ���ұߣ��ʶ���ʼ��˳��������˳����ͬ
		};

		struct InputIter 
		{
			//һ������������Iter����ʽ
			//ע��ģ�����N��ʵ�ʷ�Χn������ǰ��ֻ�����ݹ�ģ����һ������ô������

			//remain������InputIter������Ϊ����Ĺ����Ǵ�����
			int remain;
			InputIter(int n):remain(n){}
			int operator*() const 
			{
				if (remain != -1)
					return ds::read();
				return 0;
			}
			InputIter operator++(int) { return --remain, *this; }
			InputIter &operator++()  { return --remain,*this; }
		};

		template<typename K,typename Func>
		struct FuncTraits{};

		template<>
		struct FuncTraits<int,std::plus<int>> : std::plus<int>
		{
			const static int zero = 0;
			static int rangeSum(int val, int len) { return val*len; }
		};
		
		template<>
		struct FuncTraits<int, ds::Min<int>> : ds::Min<int>
		{
			const static int zero = INT_MAX;
			constexpr int rangeSum(int val, int len) { return val; }
		};

		template<>
		struct FuncTraits<int, ds::Max<int>> : ds::Max<int>
		{
			const static int zero = INT_MIN;
			constexpr int rangeSum(int val, int len) { return val; }
		};
	}
}