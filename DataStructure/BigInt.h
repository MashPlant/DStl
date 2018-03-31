#pragma once
#include <vector>
#include <ostream>
#include <iomanip>
#include <algorithm>

namespace ds
{
	class BigInt
	{
		//C风格的比较函数,其正负等于abs(lhs)-abs(rhs)的正负
		static int absComp(const BigInt &lhs, const BigInt &rhs)
		{
			if (lhs.size() != rhs.size())
				return lhs.size() < rhs.size() ? -1 : 1;
			for (int i = lhs.size() - 1; i >= 0; --i)
				if (lhs[i] != rhs[i])
					return lhs[i] < rhs[i] ? -1 : 1;
			return 0;
		}
		using Long = long long;
		const static int Exp = 9;
		const static Long Mod = 1000000000;
		mutable std::vector<Long> val;
		mutable bool nega = false;
		//规定:0的nega必须是false,0的size必须是0
		void trim() const
		{
			while (!val.empty() && val.back() == 0)
				val.pop_back();
			if (val.empty())
				nega = false;
		}
		int size() const { return val.size(); }
		Long &operator[](int index) const { return val[index]; }
		Long &back() const { return val.back(); }
		BigInt(int size, bool nega) : val(size), nega(nega) {}
		BigInt(const std::vector<Long> &val, bool nega) : val(val), nega(nega) {}

	public:
		friend std::ostream &operator<<(std::ostream &os, const BigInt &n)
		{
			if (n.size())
			{
				if (n.nega)
					os << '-';
				os << n.back();
				for (int i = (int)n.size() - 2; i >= 0; --i)
					os << std::setw(n.Exp) << std::setfill('0') << n[i];
			}
			else
				os << 0;
			return os;
		}
		friend BigInt operator+(const BigInt &lhs, const BigInt &rhs)
		{
			BigInt ret(lhs);
			return ret += rhs;
		}
		friend BigInt operator-(const BigInt &lhs, const BigInt &rhs)
		{
			BigInt ret(lhs);
			return ret -= rhs;
		}
		BigInt(Long x = 0)
		{
			if (x < 0)
				x = -x, nega = true;
			while (x >= Mod)
				val.push_back(x % Mod), x /= Mod;
			if (x)
				val.push_back(x);
		}
		BigInt(const char *s)
		{
			if (s[0] == '-')
				nega = true, ++s;
			int cur = 0, pow = 1, pos; //int存就够了
			for (pos = strlen(s) - 1; pos >= Exp - 1; pos -= Exp, val.push_back(cur), cur = 0, pow = 1)
				for (int i = pos; i > pos - Exp; --i)
					cur += (s[i] - '0') * pow, pow *= 10;
			for (cur = 0, pow = 1; pos >= 0; --pos)
				cur += (s[pos] - '0') * pow, pow *= 10;
			if (cur)
				val.push_back(cur);
		}
		BigInt &assign(const BigInt &rhs) //为了充分利用之前已经申请的空间
		{
			val.resize(rhs.size());
			nega = rhs.nega;
			if (!val.empty())
				memcpy(&val[0], &rhs[0], val.size() * sizeof(Long));
			return *this;
		}
		BigInt &operator+=(const BigInt &rhs)
		{
			const int cap = std::max(size(), rhs.size());
			val.resize(cap);
			int carry = 0;
			for (int i = 0; i < cap; ++i)
			{
				val[i] = (nega ? -val[i] : val[i]) + (i < rhs.size() ? (rhs.nega ? -rhs[i] : rhs[i]) : 0) + carry, carry = 0;
				if (val[i] >= Mod)
					val[i] -= Mod, carry = 1; //至多只需要减一次，不需要取模
				else while (val[i] < 0) //有可能需要加两次，例如-9-2
					val[i] += Mod, --carry; 
			}
			nega = carry < 0;
			if (carry)
			{
				val.push_back(carry < 0 ? -carry : carry);
				if (nega)
				{
					carry = 0;
					for (int i = 0; i < cap; ++i)
						if (carry != 0 || val[i] != 0)
							val[i] = Mod + carry - val[i], carry = -1;
					val.back() += carry; 
				}
			}
			trim();
			return *this;
		}
		friend BigInt operator-(const BigInt &rhs)
		{
			BigInt ret(rhs);
			ret.nega ^= 1;
			return ret;
		}
		BigInt &operator-=(const BigInt &rhs)
		{
			rhs.nega ^= 1;
			*this += rhs;
			rhs.nega ^= 1;
			return *this;
		}
		//高精*高精没办法原地操作，所以实现operator*，除法同理
		friend BigInt operator*(const BigInt &lhs, const BigInt &rhs)
		{
			const int cap = lhs.size() + rhs.size() + 1;
			BigInt ret(cap, lhs.nega ^ rhs.nega);
			for (int i = 0; i < cap - 1; ++i)
				for (int j = std::max(i - rhs.size() + 1, 0), up = std::min(i + 1, lhs.size()); j < up; ++j)
				{
					ret[i] += lhs[j] * rhs[i - j];
					ret[i + 1] += ret[i] / Mod, ret[i] %= Mod;
				}
			ret.trim();
			return ret;
		}
		BigInt &operator*=(const BigInt &rhs) { return *this = *this * rhs; }
		friend BigInt operator/(const BigInt &lhs, const BigInt &rhs)
		{
			static std::vector<BigInt> powTwo{ BigInt(1) };
			static std::vector<BigInt> estimate;
			estimate.clear();
			if (absComp(lhs, rhs) < 0)
				return BigInt();
			BigInt cur = rhs;
			int cmp;
			while ((cmp = absComp(cur, lhs)) <= 0)
			{
				estimate.push_back(cur), cur += cur;
				if (estimate.size() > powTwo.size())
					powTwo.push_back(powTwo.back() + powTwo.back());
			}
			if (cmp == 0)
				return BigInt(powTwo.back().val, lhs.nega ^ rhs.nega);
			const int begin = estimate.size() - 1;
			BigInt ret = powTwo[begin], tmp;
			cur = estimate[begin];
			for (int i = begin; i >= 0 && cmp != 0; --i)
				if ((cmp = absComp(tmp.assign(cur) += estimate[i], lhs)) <= 0)
					cur.assign(tmp), ret += powTwo[i]; //运算过程中几乎不会发生内存申请
			ret.nega = lhs.nega ^ rhs.nega;
			return ret;
		}
		BigInt &operator/=(const BigInt &rhs) { return *this = *this / rhs; }
		friend BigInt operator/(const BigInt &lhs, int rhs)
		{
			BigInt ret(lhs.size(), lhs.nega ^ (rhs < 0));
			if (rhs < 0)
				rhs = -rhs;
			Long carry = 0;
			for (int i = ret.size() - 1; i >= 0; --i)
			{
				carry = carry * lhs.Mod + lhs[i];
				ret[i] = carry / rhs;
				carry %= rhs;
			}
			ret.trim();
			return ret;
		}
		BigInt &operator/=(int rhs) { return *this = *this / rhs; }
		operator bool() const { return !val.empty(); }
		bool operator==(const BigInt &rhs) const { return nega == rhs.nega && val == rhs.val; }
		bool operator!=(const BigInt &rhs) const { return nega != rhs.nega || val != rhs.val; }
		bool operator>=(const BigInt &rhs) const { return !(*this < rhs); }
		bool operator>(const BigInt &rhs) const { return !(*this <= rhs); }
		bool operator<=(const BigInt &rhs) const
		{
			if (nega && !rhs.nega)
				return true;
			if (!nega && rhs.nega)
				return false;
			int cmp = absComp(*this, rhs);
			return nega ? cmp >= 0 : cmp <= 0;
		}
		bool operator<(const BigInt &rhs) const
		{
			if (nega && !rhs.nega)
				return true;
			if (!nega && rhs.nega)
				return false;
			return (absComp(*this, rhs) < 0) ^ nega;
		}
		void swap(const BigInt &rhs) const
		{
			std::swap(val, rhs.val);
			std::swap(nega, rhs.nega);
		}
	};
}
