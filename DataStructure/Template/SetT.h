#pragma once
#include <type_traits>

namespace ds
{
	namespace stmp //stateful template metaprogramming
	{
		//大概原理是这样的:利用N个bool来维护N个状态(为什么不是lgN个bool?因为每个bool的状态只能变化一次)
		//每次计数器+1,都是一个constexpr函数被定义的过程
		//被定义的constexpr函数具有noexcept性质，不会被SFINAE( char[noexcept(adl_flag(flag<N>())) ? +1 : -1] )判定失败
		//从而可以提供更好的匹配
		template <int N>
		struct flag
		{
			friend constexpr int adl_flag(flag<N>);
		};
		template <int N>
		struct writer
		{
			friend constexpr int adl_flag(flag<N>)
			{
				return N;
			}
			static constexpr int value = N;
		};
		template <int N, class = char[noexcept(adl_flag(flag<N>())) ? +1 : -1]>
		int constexpr reader(int, flag<N>)
		{
			return N;
		}
		template <int N>
		int constexpr reader(float, flag<N>, int R = reader(0, flag<N - 1>()))
		{
			return R;
		}
		int constexpr reader(float, flag<0>)
		{
			return 0;
		}
		template <int N = 1, int C = reader(0, flag<64>())> //只支持64个状态
		int constexpr next(int R = writer<C + N>::value)
		{
			return R;
		}
	}
	constexpr unsigned staticRand(unsigned x)
	{
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return x;
	}
	template <int Version>
	struct StaticRand
	{
		const static unsigned value = staticRand(StaticRand<Version - 1>::value);
	};
	template <>
	struct StaticRand<1>
	{
		const static unsigned value = 19260817u;
	};
	
	template <typename T>
	struct NullT
	{
		using Left = NullT<T>;
		using Right = NullT<T>;
		using ValueType = T;
	};

	template <typename T>
	struct IsNull
	{
		const static bool value = std::is_base_of<NullT<typename T::ValueType>, T>::value;
	};
	template <typename T, T Val, unsigned Pri, typename L, typename R>
	struct NodeT
	{
		const static unsigned pri = Pri;
		const static T value = Val;
		using Left = L;
		using Right = R;
		using ValueType = T;
	};

	template <typename Tree, bool Null = IsNull<Tree>::value>
	struct OutputTree
	{
		using T = typename Tree::ValueType;
		template <T Value>
		struct Outputer;
		OutputTree<typename Tree::Left> left{};
		Outputer<Tree::value> impl{};
		OutputTree<typename Tree::Right> right{};
	};
	template <typename Tree>
	struct OutputTree<Tree, true>
	{	};

	template <typename Node, typename Node::ValueType Pivot, bool Null = IsNull<Node>::value>
	struct Split
	{
		using T = typename Node::ValueType;
		//如果Node的value<=Pivot,那么Node的左子树完全归入左半边,右子树继续划分出来的左子树作为Left的左子树
		//如果Node的value>Pivot,那么Node的右子树完全归入右半边,左子树继续划分出来的右子树作为Right的左子树
		using Left = typename std::conditional<Node::value <= Pivot,
			NodeT<T, Node::value, Node::pri, typename Node::Left, typename Split<typename Node::Right, Pivot>::Left>,
			typename Split<typename Node::Left, Pivot>::Left>::type;
		using Right = typename std::conditional<Node::value <= Pivot,
			typename Split<typename Node::Right, Pivot>::Right,
			NodeT<T, Node::value, Node::pri, typename Split<typename Node::Left, Pivot>::Right, typename Node::Right> >::type;
	};
	template <typename Node, typename Node::ValueType Pivot>
	struct Split<Node, Pivot, true>
	{
		using Left = NullT<typename Node::ValueType>;
		using Right = NullT<typename Node::ValueType>;
	};
	template <typename Left, typename Right, bool Null = IsNull<Left>::value || IsNull<Right>::value>
	struct Merge
	{
		using T = typename Left::ValueType;
		//Left中的元素全部小于Right中的元素
		//如果Left的根的优先级更高,那么把Left和Left的左孩子保持不变,Left的右孩子与Right的归并结果作为Left的右孩子
		//如果Right的根的优先级更高,那么把Right和Right的右孩子保持不变,Left与Right的左孩子的归并结果作为Right的左孩子
		using Root = typename std::conditional < (Left::pri > Right::pri),
			NodeT<T, Left::value, Left::pri, typename Left::Left, typename Merge<typename Left::Right, Right>::Root>,
			NodeT<T, Right::value, Right::pri, typename Merge<Left, typename Right::Left>::Root, typename  Right::Right>
		>::type;
	};
	template <typename Left, typename Right>
	struct Merge<Left, Right, true>
	{
		using Root = typename std::conditional<IsNull<Left>::value, Right, Left>::type;
	};
	template <typename Left, typename Right>
	using MergeT = typename Merge<Left, Right>::Root; //为了使用起来更方便

	template <typename Tree, typename Tree::ValueType Elem>
	struct Insert
	{
		const static int cur = stmp::next(); //这应该是VC++的bug，必须写出这个数来，不能直接在模板参数里用next()
		using T = typename Tree::ValueType;
		using SplitResult = Split<Tree, Elem>;
		using NewNode = NodeT<T, Elem, StaticRand<cur>::value, NullT<T>, NullT<T>>;
		using Root = MergeT<MergeT<typename SplitResult::Left, NewNode>, typename SplitResult::Right>;
	};
	template <typename Tree, typename Tree::ValueType Elem>
	using InsertT = typename Insert<Tree, Elem>::Root;

	template <typename T, T ...>
	struct MakeTree;
	template <typename T, T First, T ...Args>
	struct MakeTree<T, First, Args...>
	{
		using Root = InsertT<typename MakeTree<T, Args...>::Root, First>;
	};
	template <typename T>
	struct MakeTree<T> { using Root = NullT<T>; };
	template <typename T, T...Args>
	using MakeTreeT = typename MakeTree<T, Args...>::Root;

	template <typename Tree, typename Tree::ValueType Elem>
	struct Erase
	{
		using Result1 = Split<Tree, Elem>;
		using Result2 = Split<typename Result1::Left, Elem - 1>;
		using X = typename Result2::Left; //全部<elem
		using Y = MergeT<typename Result2::Right::Left, typename Result2::Right::Right>; //去掉一个==elem的
		using Z = typename Result1::Right; //全部>elem
		using Root = MergeT<MergeT<X, Y>, Z>;
	};
	template <typename Tree, typename Tree::ValueType Elem>
	using EraseT = typename Erase<Tree, Elem>::Root;
}