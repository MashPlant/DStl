#pragma once
namespace ds
{
	//std::is_invocable��C++17��׼�ṩ�ģ�MS�ı�����û��ʵ��
	//�����ṩһ���ǳ��򵥵�ʵ�֣������кܶ����ζ�û�п��ǵ������ǹ�����
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

	//�����Chain����OpenAddress������ԭ����:ֻҪ�ܹ����ã�����Ϊ�ǹ�ϣ����������Ϊ��ʹ��Chain
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
			//Ϊ�˱�֤̽������ȡ��[0,m)��һ�ֿ��е�c1,c2,mȡ����
			//c1=c2=0.5,mΪ2����
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
			//Ϊ�˱�֤̽������ȡ��[0,m)�����뱣֤h2��m����
			//һ�ּ򵥵�ʵ���ǣ���mΪ2���ݣ�h2��������
			if ((cache2 & 1) == 0)
				throw std::runtime_error("Hash2 must return an odd number.");
		}
		CodeType probe(unsigned index) const { return cache1 + index * cache2; }
	};
}