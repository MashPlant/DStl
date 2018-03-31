#pragma once
#include <cstdint>
#include <string>
#include "BigInt.h"

namespace ds
{
	template <int Bit>
	class UInt
	{
		static_assert(Bit >= 32 && Bit % 32 == 0, "Bit illegal");
		constexpr static int ArrLen = Bit / 32;
		uint32_t val[ArrLen];
		static int comp(const UInt &lhs,const UInt &rhs)
		{
			for (int i = ArrLen - 1; i >= 0; --i)
				if (lhs[i] != rhs[i])
					return lhs[i] < rhs[i] ? -1 : 1;
			return 0;
		}
		int hiBit() const
		{
			for (int i = ArrLen - 1; i >= 0; --i)
				if (val[i])
				{
					for (int bit = 31; bit > 0; --bit)
						if (val[i] & 1 << bit)
							return 32 * i + bit;
					return 32 * i;
				}
			return -1;
		}
	public:
		uint32_t &operator[](int index) { return val[index]; }
		uint32_t operator[](int index) const { return val[index]; }
		BigInt toBigInt() const
		{
			BigInt ret, cur = 1;
			for (int i = 0; i < ArrLen; ++i)
				ret += cur * BigInt(val[i]), cur *= 1ull << 32;
			return ret;
		}
		UInt(uint32_t i = 0) : val{} { val[0] = i; }
		UInt(uint64_t i): val{}
		{
			static_assert(Bit >= 64, "Bit too short");
			val[0] = i & 0xffffffff, val[1] = i >> 32;
		}
		static UInt fromHex(const char *src)
		{
			UInt ret;
			for (int i = 0, sz = strlen(src); i < ArrLen; ++i)
				for (int j = 0; j < 8 && i * 8 + j < sz; ++j)
				{
					char ch = src[sz - i * 8 - j - 1];
					ret.val[i] += (isdigit(ch) ? ch - '0' : ch - 'a' + 10) << (j * 4);
				}
			return ret;
		}
		const char *toHex()
		{
			static char hex[ArrLen * 8];
			auto mask = [](uint32_t x, int shift) {return (x >> shift * 4) & 15; };
			auto toChar = [](int x) {return x < 10 ? x + '0' : x - 10 + 'a'; };
			for (int i = 0; i < ArrLen; ++i)
				for (int j = 0; j < 8; ++j)
					hex[i * 8 + j] = toChar(mask(val[i], j));
			std::reverse(hex, hex + sizeof hex);
			return hex;
		}
		friend UInt operator+(UInt lhs, const UInt &rhs) { return lhs += rhs; }
		friend UInt operator-(UInt lhs, const UInt &rhs) { return lhs -= rhs; }
		friend UInt operator<<(UInt lhs, int rhs) { return lhs <<= rhs; }
		friend UInt operator>>(UInt lhs, int rhs) { return lhs >>= rhs; }
		friend UInt operator&(UInt lhs, int rhs) { return lhs &= rhs; }
		friend UInt operator|(UInt lhs, int rhs) { return lhs |= rhs; }
		friend UInt operator^(UInt lhs, int rhs) { return lhs ^= rhs; }
		UInt &operator*=(const UInt &rhs) { return *this = *this * rhs; }
		UInt &operator/=(const UInt &rhs) { return *this = *this / rhs; }
		UInt &operator+=(const UInt &rhs)
		{
			uint64_t carry = 0ull;
			for (int i = 0; i < ArrLen; ++i)
			{
				carry += (uint64_t)val[i] + rhs.val[i];
				val[i] = carry & 0xffffffff;
				carry >>= 32;
			}
			return *this;
		}
		UInt &operator-=(const UInt &rhs) { return *this += -rhs; }
		UInt operator-() const
		{
			UInt ret(*this);
			for (int i = 0; i < ArrLen; ++i)
				ret.val[i] = ~ret.val[i];
			return ++ret;
		}
		UInt &operator++()
		{
			for (int i = 0; i < ArrLen && !++val[i++];);
			return *this;
		}
		UInt operator++(int) { UInt ret(this); return ++ret; }
		UInt &operator--()
		{
			for (int i = 0; i < ArrLen && !~--val[i++];);
			return *this;
		}
		UInt operator--(int) { UInt ret(this); return --ret; }
		UInt operator~() const
		{
			UInt ret(*this);
			for (int i = 0; i < ArrLen; ++i)
				ret[i] = ~ret[i];
			return ret;
		}
		UInt &operator&=(const UInt &rhs)
		{
			for (int i = 0; i < ArrLen; ++i)
				val[i] &= rhs[i];
			return *this;
		}
		UInt &operator|=(const UInt &rhs)
		{
			for (int i = 0; i < ArrLen; ++i)
				val[i] |= rhs[i];
			return *this;
		}
		UInt &operator^=(const UInt &rhs)
		{
			for (int i = 0; i < ArrLen; ++i)
				val[i] ^= rhs[i];
			return *this;
		}
		UInt &operator<<=(int shift)
		{
			const int wordShift = shift / 32;
			if (wordShift != 0)
				for (int pos = ArrLen - 1; pos >= 0; --pos)
					val[pos] = wordShift <= pos ? val[pos - wordShift] : 0;
			if ((shift %= 32) != 0)
			{	
				for (int pos = ArrLen - 1; pos > 0; --pos)
					val[pos] = (val[pos] << shift) | (val[pos - 1] >> (32 - shift));
				val[0] <<= shift;
			}
			return *this;
		}
		UInt &operator>>=(int shift)
		{
			const int wordShift = shift / 32;
			if (wordShift != 0)
				for (int pos = 0; pos < ArrLen; ++pos)
					val[pos] = wordShift < ArrLen - pos ? val[pos + wordShift] : 0;
			if ((shift %= 32) != 0)
			{	
				for (int pos = 0; pos < ArrLen - 1; ++pos)
					val[pos] = (val[pos] >> shift) | (val[pos + 1] << (32 - shift));
				val[ArrLen - 1] >>= shift;
			}
			return *this;
		}
		friend UInt operator*(const UInt &lhs,const UInt &rhs)
		{
			UInt ret;
			for (int i = 0; i < ArrLen; ++i)
			{
				uint64_t carry = 0;
				for (int j = 0; j + i < ArrLen; ++j)
				{
					uint64_t n = carry + ret[j + i] + (uint64_t)lhs[i] * rhs[j];
					ret[j + i] = n & 0xffffffff;
					carry = n >> 32;
				}
			}
			return ret;
		}
		friend UInt operator/(UInt lhs, UInt rhs) //复制是一定要进行的，不如直接在参数中复制
		{
			UInt ret;
			int lhsBit = lhs.hiBit(), rhsBit = rhs.hiBit();
			if (rhsBit > lhsBit)
				return 0u;
			int shift = lhsBit - rhsBit;
			rhs <<= shift; 
			while (shift >= 0) //思路：分别考虑每个二进制位填1还是0
			{
				if (lhs >= rhs) 
				{
					lhs -= rhs; //填1意味着需要一个rhs，那么lhs就减掉一个rhs
					ret[shift / 32] |= 1 << (shift % 32); //填上一个1
				}
				rhs >>= 1; //rhs除2后继续考虑
				--shift;
			}
			return ret;
		}
		bool operator<(const UInt &rhs)const { return comp(*this, rhs) < 0; }
		bool operator<=(const UInt &rhs)const { return comp(*this, rhs) <= 0; }
		bool operator>(const UInt &rhs)const { return comp(*this, rhs) > 0; }
		bool operator>=(const UInt &rhs)const { return comp(*this, rhs) >= 0; }
		bool operator==(const UInt &rhs)const { return comp(*this, rhs) == 0; }
		bool operator!=(const UInt &rhs)const { return comp(*this, rhs) != 0; }
		explicit operator bool() const 
		{
			uint32_t ret = 0;
			for (int i = 0; i < ArrLen; ++i)
				ret |= val[i];
			return ret != 0;
		}
	};
}
