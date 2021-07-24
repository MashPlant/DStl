#pragma once
#include <string>
#include <random>
#include <functional>
#include <iostream>
#include <ctime>
#include <ostream>
using std::string;
namespace ds
{
	template <typename First,typename Second>
	std::ostream& operator<<(std::ostream &os,const std::pair<First,Second> &pr)
	{
		os << pr.first << ' ' << pr.second << ' ';
		return os;
	}
	template<typename K>
	void print(K &&lst)
	{
		for (const auto &i : lst)
			std::cout << i << ' ';
		std::cout << std::endl;
	}

	struct GenIter
	{
		typedef std::random_access_iterator_tag iterator_category;
		typedef int value_type;
		typedef int difference_type;
		typedef int *pointer;
		typedef int &reference;
		int pos;
		GenIter(int pos = 0) : pos(pos) {}
		int operator*() const { return pos; }
		GenIter &operator+=(int offset) { return pos += offset, *this; }
		GenIter &operator-=(int offset) { return pos -= offset, *this; }
		GenIter &operator++() { return ++pos, *this; }
		GenIter operator++(int) { return pos++, *this; }
		GenIter &operator--() { return --pos, *this; }
		GenIter operator--(int) { return pos--, *this; }
		GenIter operator+(int offset) const { return pos + offset; }
		GenIter operator-(int offset) const { return pos - offset; }
		int operator-(GenIter rhs) const { return pos - rhs.pos; }
		bool operator!=(GenIter rhs) const { return pos != rhs.pos; }
		bool operator==(GenIter rhs) const { return pos == rhs.pos; }
	};
	inline GenIter operator+(int offset, GenIter rhs) { return offset + rhs.pos; }

	
	inline uint32_t rawRani()
	{
		static unsigned seed = 19260817;// static_cast<unsigned>(time(nullptr));
		auto xorshift32 = [&]()->uint32_t 
		{ 	
			/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */ 	
			uint32_t x = seed; 	
			x ^= x << 13; 	
			x ^= x >> 17; 	
			x ^= x << 5; 	
			return seed = x;
		};
		return xorshift32();
	}
	inline int rani(int l, int r) { return rawRani() % (r - l + 1) + l; }
	template <typename K>
	const K &max(const K &lhs, const K &rhs) { return lhs < rhs ? rhs : lhs; }
	template <typename K>
	const K &min(const K &lhs, const K &rhs) { return lhs < rhs ? lhs : rhs; }

	template <typename K>
	struct Max
	{
		typedef K result_type;
		typedef K first_argument_type;
		typedef K second_argument_type;
		const K & operator()(const K &lhs, const K &rhs) const { return max<K>(lhs, rhs); }
	};

	template <typename K>
	struct Min
	{
		typedef K result_type;
		typedef K first_argument_type;
		typedef K second_argument_type;
		const K & operator()(const K &lhs, const K &rhs) const { return min<K>(lhs, rhs); }
	};
}