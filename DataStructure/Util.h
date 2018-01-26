#pragma once
#include <string>
#include <random>
#include <functional>
#include <iostream>
#include <ctime>
using std::string;
namespace ds
{
	template<typename K>
	void print(const K &lst)
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

	inline int read()
	{
		static char ch;
		bool sgn = false;
		while ((ch = getchar()) < '0' || ch > '9')
			if (ch == '-')
				sgn = true;
		int res = ch - 48;
		while ((ch = getchar()) >= '0' && ch <= '9')
			res = res * 10 + ch - 48;
		return sgn ? -res : res;
	}
	inline string reads()
	{
		static char buf[100];
		scanf("%s", buf);
		return buf;
	}
	inline int rani(int l, int r)
	{
		static int seed = time(0);
		auto xorshift32 = [&]()->uint32_t 
		{ 	
			/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */ 	
			uint32_t x = seed; 	
			x ^= x << 13; 	
			x ^= x >> 17; 	
			x ^= x << 5; 	
			return seed = x;
		};
		return xorshift32() % (r - l + 1) + l;
	}
	template <typename K>
	K max(const K &lhs, const K &rhs) { return lhs < rhs ? rhs : lhs; }
	template <typename K>
	K min(const K &lhs, const K &rhs) { return lhs < rhs ? lhs : rhs; }
	/*template <typename V1,typename V2>
	bool operator==( V1& v1, V2&v2)
	{
	if (v1.size() != v2.size())
	return false;
	//freopen("out.txt", "w", stdout);
	for (int i = 0; i < v1.size(); ++i)
	{
	//if (i%100==0)
	//std::cerr << i<<' ';
	auto tmp1 = v1[i], tmp2 = v2[i];
	//printf("%d %d\n", tmp1, tmp2);
	if (tmp1 != tmp2)
	return false;
	}
	return true;
	}*/
}