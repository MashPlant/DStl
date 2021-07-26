#pragma once
namespace ds
{
	//std::is_invocable是C++17标准提供的，MS的编译器没有实现
	//这里提供一个非常简单的实现，可能有很多情形都没有考虑到，但是够用了
	namespace detail
	{
		template <typename F, typename ...Args>
		struct CallableImpl
		{
			template <typename = decltype(std::declval<F>()(std::declval<Args>()...))>
			static std::true_type tryCall(int);
			template <typename ...>
			static std::false_type tryCall(...);
		};
	}
	template <typename F, typename ...Args>
	struct Callable : decltype(detail::CallableImpl<F, Args...>::tryCall(0)) {};

	//这里对Chain还是OpenAddress的区分原则是:只要能够调用，就认为是哈希函数，就认为是使用Chain
	template <typename K, typename V, typename Hash = std::hash<K>,
	bool EnableOpenAddress = !Callable<Hash,K>::value>
	class HashMap;


	template <typename K, typename Mode, typename ...Hashs>
	struct OpenAddress;
	struct Linear {};
	struct Quadratic {};
	struct Double {};

	template<typename K, typename Hash>
	struct OpenAddress<K, Linear, Hash>
	{
		Hash hasher;
		typedef decltype(hasher(std::declval<K>())) CodeType;
		CodeType cache;
		OpenAddress(const K&key) :cache(hasher(key)) {}
		CodeType probe(unsigned index) const { return cache + index; }
	};
	template<typename K, typename Hash>
	struct OpenAddress<K, Quadratic, Hash> : Hash
	{
		Hash hasher;
		typedef decltype(hasher(std::declval<K>())) CodeType;
		CodeType cache;
		OpenAddress(const K&key) :cache(hasher(key)) {}
		CodeType probe(unsigned index) const
		{
			//h(key,i) = (h0(key) + i * c1 + i^2 * c2) % m
			//为了保证探查序列取遍[0,m)，一种可行的c1,c2,m取法是
			//c1=c2=0.5,m为2的幂
			return cache + index * (index + 1) / 2;
		}
	};

	template<typename K, typename Hash1, typename Hash2>
	struct OpenAddress<K, Double, Hash1, Hash2>
	{
		Hash1 hasher1;
		Hash2 hasher2;
		typedef decltype(hasher1(std::declval<K>())) CodeType;
		CodeType cache1, cache2;
		OpenAddress(const K&key) :cache1(hasher1(key)),cache2(hasher2(key))
		{
			//h(key,i) = (h1(key) + i * h2(key)) % m
			//为了保证探查序列取遍[0,m)，必须保证h2与m互素
			//一种简单的实现是，令m为2的幂，h2返回奇数
			if ((cache2 & 1) == 0)
				throw std::runtime_error("Hash2 must return an odd number.");
		}
		CodeType probe(unsigned index) const { return cache1 + index * cache2; }
	};
}