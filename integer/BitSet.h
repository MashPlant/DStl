#pragma once
#include <string>

namespace ds
{
	template <int Bit>
	class BitSet
	{
		using Int = unsigned long long;
		const static int IntSize = sizeof(Int) * 8;
		const static int Size = Bit / IntSize + (Bit % IntSize != 0);
		Int bits[Size] = { 0 };
		void trim() { bits[Size - 1] &= (1ull << Bit % IntSize) - 1; }
	public:
		BitSet() = default;
		BitSet(Int init) :BitSet() { bits[0] = init; }
		explicit operator std::string() const
		{
			std::string ret(Bit, '\0');
			for (int i = 0; i < Bit; ++i)
				ret[i] = at(i) + '0';
			return ret;
		}
		int count() const 
		{
			const char table[] =
				"\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
				"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
				"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
				"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
				"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
				"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
				"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
				"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
				"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
				"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
				"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
				"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
				"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
				"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
				"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
				"\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";
			int sum = 0;
			for (auto cur = reinterpret_cast<const unsigned char *>(bits), end = cur + sizeof(bits)
				; cur != end; ++cur)
				sum += table[*cur];
			return sum;
		}
		bool at(int pos) const
		{
			const int byte = pos / IntSize, bit = pos % IntSize;
			return bits[byte] & 1ull << bit;
		}
		void set(int pos, bool val = true)
		{
			const int byte = pos / IntSize, bit = pos % IntSize;
			val ? bits[byte] |= 1ull << bit : bits[byte] &= ~1ull << bit;
		}
		friend BitSet operator<<(BitSet lhs, int rhs) { return lhs <<= rhs; }
		friend BitSet operator>>(BitSet lhs, int rhs) { return lhs >>= rhs; }
		friend BitSet operator&(BitSet lhs, int rhs) { return lhs &= rhs; }
		friend BitSet operator|(BitSet lhs, int rhs) { return lhs |= rhs; }
		friend BitSet operator^(BitSet lhs, int rhs) { return lhs ^= rhs; }
		void flip()
		{
			for (int i = 0; i < Size; ++i)
				bits[i] = ~bits[i];
			trim();
		}
		BitSet operator~() const
		{
			BitSet ret(*this);
			for (int i = 0; i < Size; ++i)
				ret.bits[i] = ~ret.bits[i];
			ret.trim();
			return ret;
		}
		BitSet &operator<<=(int shift)
		{
			const int wordShift = shift / IntSize;
			if (wordShift != 0)
				for (int pos = Size - 1; pos >= 0; --pos)
					bits[pos] = wordShift <= pos ? bits[pos - wordShift] : 0;
			if ((shift %= IntSize) != 0)
			{
				for (int pos = Size - 1; pos > 0; --pos)
					bits[pos] = (bits[pos] << shift) | (bits[pos - 1] >> (IntSize - shift));
				bits[0] <<= shift;
			}
			return *this;
		}
		BitSet &operator>>=(int shift)
		{
			const int wordShift = shift / IntSize;
			if (wordShift != 0)
				for (int pos = 0; pos < Size; ++pos)
					bits[pos] = wordShift < Size - pos ? bits[pos + wordShift] : 0;
			if ((shift %= IntSize) != 0)
			{
				for (int pos = 0; pos < Size - 1; ++pos)
					bits[pos] = (bits[pos] >> shift) | (bits[pos + 1] << (IntSize - shift));
				bits[Size - 1] >>= shift;
			}
			return *this;
		}
		BitSet &operator&=(const BitSet &rhs)
		{
			for (int i = 0; i < Size; ++i)
				bits[i] &= rhs.bits[i];
			trim();
			return *this;
		}
		BitSet &operator|=(const BitSet &rhs)
		{
			for (int i = 0; i < Size; ++i)
				bits[i] |= rhs.bits[i];
			trim();
			return *this;
		}
		BitSet &operator^=(const BitSet &rhs)
		{
			for (int i = 0; i < Size; ++i)
				bits[i] ^= rhs.bits[i];
			trim();
			return *this;
		}
		struct BitRef
		{
			BitSet *self;
			int pos;
			operator bool() const { return self->at(pos); }
			BitRef &operator=(bool b) { return self->set(pos, b), *this; }
		};
		BitRef operator[](int pos) { return { this,pos }; }
		bool operator[](int pos) const { return at(pos); }
	};
}