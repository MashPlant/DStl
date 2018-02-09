#pragma once
#include "Vector.h"
#include "Util.h"
#include <iostream>
namespace ds
{
	class StringView
	{
	private:
		const char *first;
		const char *last;
		static int *buildKMP(const StringView &rhs)
		{
			int *nxt = new int[rhs.size()];
			memset(nxt, 0, rhs.size() * sizeof(int));
			for (int i = 1, k = 0; i < rhs.size(); ++i)
			{
				while (k && rhs[k] != rhs[i])
					k = nxt[k - 1];
				nxt[i] = k += rhs[k] == rhs[i];
			}
			return nxt;
		}
	public:
		const static int npos = ~(1 << 31);
		typedef const char * iterator;
		char operator[](int k)const { return first[k]; }
		iterator begin() const { return first; }
		iterator end() const { return last; }
		StringView(const char *first, const char *last = nullptr) :first(first)
		{
			this->last = last ? last : (first + strlen(first));
		}
		int size()const { return last - first; }
		void removeSuffix(int cnt = 1) { last -= cnt; }
		void removePrefix(int cnt = 1) { first += cnt; }
		StringView substr(int pos, int cnt = npos) const
		{
			cnt = min(cnt, size() - pos);
			return StringView(first + pos, first + pos + cnt);
		}
		struct KMP {};
		struct Brute {};
		struct KarpRabin {};
		struct BoyerMoore {};
		int find(const StringView &rhs, KMP) const
		{
			int *nxt = buildKMP(rhs);
			int i, k;
			for (i=0,k=0;i<size();++i)
			{
				while (k && operator[](i) != rhs[k])
					k = nxt[k - 1];
				k += operator[](i) == rhs[k];
				if (k == rhs.size())
					break;
			}
			delete[]nxt;
			return k == rhs.size() ? i - k + 1 : npos;
		}
		Vector<int> findAll(const StringView &rhs, KMP) const
		{
			int *nxt = buildKMP(rhs);
			Vector<int> ret;
			int i, k;
			for (i = 0, k = 0; i<size(); ++i)
			{
				while (k && operator[](i) != rhs[k])
					k = nxt[k - 1];
				k += operator[](i) == rhs[k];
				if (k == rhs.size())
					ret.push_back(i - k + 1), k = 0;
			}
			delete[]nxt;
			return ret;
		}
		int find(const StringView &rhs, KarpRabin) const
		{
			const int p = 127;
			typedef unsigned long long ull;
			ull rhsHash = 0, selfHash = 0, hi = 1;
			for (int i = 0; i < rhs.size(); ++i)
				rhsHash = rhsHash * p + rhs[i], (i ? hi *= p : 0); //少乘一次,因为除法并不兼容取模
			for (int i = 0; i < rhs.size(); ++i)
				selfHash = selfHash * p + operator[](i);
			if (selfHash == rhsHash)
				return 0;
			for (int i = rhs.size(); i < size(); ++i)
			{
				selfHash = (selfHash - operator[](i - rhs.size()) * hi) * p + operator[](i);
				if (selfHash == rhsHash)
					return i + 1 - rhs.size();
			}
			return npos;
		}
		Vector<int> findAll(const StringView &rhs,KarpRabin) const
		{
			const int p = 127;
			typedef unsigned long long ull;
			Vector<int> ret;
			ull rhsHash = 0, selfHash = 0, hi = 1;
			for (int i = 0; i < rhs.size(); ++i)
				rhsHash = rhsHash * p + rhs[i], (i ? hi *= p : 0); 
			for (int i = 0; i < rhs.size(); ++i)
				selfHash = selfHash * p + operator[](i);
			if (selfHash == rhsHash)
				ret.push_back(0);
			for (int i = rhs.size(); i < size(); ++i)
			{
				selfHash = (selfHash - operator[](i - rhs.size()) * hi) * p + operator[](i);
				if (selfHash == rhsHash)
					ret.push_back(i + 1 - rhs.size());
			}
			return ret;
		}
	};
	class String : public Vector<char>
	{
		friend std::ostream& operator<<(std::ostream& os, const String &rhs) //不支持中文
		{
			for (auto ch : rhs)
				os << ch;
			return os;
		}
		friend std::istream& operator>>(std::istream& is, String &rhs)
		{
			char ch;
			while (is.get(ch) && isspace(ch));
			rhs.push_back(ch);
			while (is.get(ch) && !isspace(ch))
				rhs.push_back(ch);
			return is;
		}
	public:
		const static int npos = ~(1 << 31);
		String(const char *ptr)
		{
			const int len = strlen(ptr);
			reserve(len);
			cpy(arr_, ptr, ptr + len);
			size_ = len;
		}
		String() = default;
		String& operator+=(char c) { return push_back(c), *this; }
		String& operator+=(const String &rhs) { return addAll(rhs), *this; }
		String& operator+=(const char *ptr)
		{
			const int len = strlen(ptr);
			reserve(len + size_);
			cpy(arr_ + size_, ptr, ptr + len);
			size_ += len;
			return *this;
		}
		template<typename K>
		String operator+(K &&k) const
		{
			String tmp(*this);
			tmp += std::forward<K>(k);
			return tmp;
		}
		operator StringView() const { return StringView(begin(), end()); }
		template<typename Mode = StringView::KMP>
		int find(const StringView &rhs, Mode) { return StringView(*this).find(rhs, Mode()); }
		template<typename Mode = StringView::KMP>
		Vector<int> findAll(const StringView &rhs, Mode) { return StringView(*this).findAll(rhs, Mode()); }
	};
	template<typename K>
	String operator+(K &&k, const String &rhs)
	{
		return rhs + std::forward<K>(k);
	}
}
