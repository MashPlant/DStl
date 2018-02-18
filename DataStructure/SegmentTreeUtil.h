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
			Node<K, Func,Left, Left + (Right - Left) / 2> left; //不使用堆式储存
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
			//最底层的叶子并不需要标签
			K key;
			template <typename Iter>
			Node(Iter &it) :key(*it++) {} //由于始终先初始化左边再初始化右边，故而初始化顺序与排列顺序相同
		};

		struct InputIter 
		{
			//一个常见的上述Iter的形式
			//注意模板参数N与实际范围n的区别，前者只是数据规模，不一定有那么多输入

			//remain被所有InputIter共享，因为上面的构造是传引用
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