#pragma once

#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>

namespace ds
{
	class Polynomial : public std::vector<double>
	{
		constexpr static double eps = 1e-5;

		void trim()
		{
			while (!empty() && fabs(back()) < eps)
				pop_back();
		}

	public:
		template <typename OS>
		friend OS &&operator<<(OS &&os, const Polynomial &p)
		{
			if (p.empty())
				return os << "0";
			for (auto it = p.rbegin(); it != p.rend(); ++it)
				os << *it << ' ';
			return os;
		}

		explicit Polynomial(int size, double value = 0.0) : vector(size, value)
		{}

		Polynomial(std::initializer_list<double> lst) : vector(lst)
		{ std::reverse(begin(), end()); }

		Polynomial &operator+=(const Polynomial &rhs)
		{
			const int cap = std::max(size(), rhs.size());
			resize(cap);
			for (int i = 0; i < cap; ++i)
				(*this)[i] += i > rhs.size() ? 0 : rhs[i];
			trim();
			return *this;
		}

		friend Polynomial operator+(Polynomial lhs, const Polynomial &rhs)
		{ return lhs += rhs; }

		Polynomial &operator-=(const Polynomial &rhs)
		{
			const int cap = std::max(size(), rhs.size());
			resize(cap);
			for (int i = 0; i < cap; ++i)
				(*this)[i] -= i > rhs.size() ? 0 : rhs[i];
			trim();
			return *this;
		}

		friend Polynomial operator-(Polynomial lhs, const Polynomial &rhs)
		{ return lhs -= rhs; }

		Polynomial &operator*=(double rhs)
		{
			for (auto &d:*this)
				d *= rhs;
			return *this;
		}

		friend Polynomial operator*(Polynomial lhs, double rhs)
		{ return lhs *= rhs; }

		friend Polynomial operator*(double lhs, Polynomial rhs)
		{ return rhs *= lhs; }

		friend Polynomial operator*(const Polynomial &lhs, const Polynomial &rhs)
		{
			const int lSize = lhs.size(), rSize = rhs.size();
			Polynomial ret(lSize + rSize - 1);
			for (int i = 0; i < lSize; ++i)
				for (int j = 0; j < rSize; ++j)
					ret[i + j] += lhs[i] * rhs[j];
			ret.trim();
			return ret;
		}

		Polynomial &operator*=(const Polynomial &rhs)
		{ return *this = *this * rhs; }

		//除法无法原地操作,所以用/来实现/=
		Polynomial &operator/=(const Polynomial &rhs)
		{ return *this = *this / rhs; }

		friend Polynomial operator/(Polynomial lhs, const Polynomial &rhs)
		{
			if (lhs.size() < rhs.size())
				return {};
			Polynomial ret(lhs.size());
			while (lhs.size() >= rhs.size())
			{
				const double fac = lhs.back() / rhs.back();
				auto it1 = lhs.rbegin();
				auto it2 = rhs.rbegin();
				for (; it1 != lhs.rend(); ++it1, ++it2)
					*it1 -= fac * *it2;
				ret[lhs.size() - rhs.size()] = fac;
				lhs.trim();
			}
			ret.trim();
			return ret;
		}

		Polynomial &operator%=(const Polynomial &rhs)
		{
			if (size() < rhs.size())
				return *this;
			while (size() >= rhs.size())
			{
				const double fac = back() / rhs.back();
				auto it1 = rbegin();
				auto it2 = rhs.rbegin();
				for (; it1 != rend(); ++it1, ++it2)
					*it1 -= fac * *it2;
				trim();
			}
			return *this;
		}

		friend Polynomial operator%(Polynomial lhs, const Polynomial &rhs)
		{ return lhs %= rhs; }

		explicit operator bool() const
		{ return !empty(); }

	};

	inline Polynomial gcd(const Polynomial &lhs, const Polynomial &rhs)
	{ return rhs ? gcd(rhs, lhs % rhs) : lhs; }

	inline Polynomial pow(Polynomial lhs, int rhs)
	{
		--rhs;
		Polynomial ret = lhs;
		while (rhs)
		{
			if (rhs & 1)
				ret *= lhs;
			lhs *= lhs;
			rhs >>= 1;
		}
		return ret;
	}


//	template <typename Number>
//	class Polynomial : public std::vector<Number>
//	{
//	public:
//		Polynomial &operator+=(const Polynomial &rhs)
//		{
//			if (rhs.size() > size())
//				resize(rhs.size());
//			for (int i = 0; i < size(); ++i)
//				operator[](i) += i >= rhs.size() ? Number() : rhs[i];
//			return *this;
//		}
//		Polynomial operator-() const
//		{
//			Polynomial ret(*this);
//			for (int i = 0; i < ret.size(); ++i)
//				ret[i] = -ret[i];
//			return ret;
//		}
//		Polynomial &operator-=(const Polynomial &rhs)
//		{
//			return operator+=(-rhs);
//		}
//		Polynomial &operator*=(const Polynomial &rhs)
//		{
//			int nSize = size() + rhs.size();
//			int lg = 0;
//			for (; 1 << lg < nSize; lg <<= 1);
//			nSize = 1 << lg;
//			Polynomial<Complex> a(nSize), b(nSize);
//			for (int i = 0; i < size(); ++i)
//				a[i].x = operator[](i);
//			for (int i = 0; i < rhs.size(); ++i)
//				b[i].x = rhs[i];
//			fft(a, 1);
//			fft(b, 1);
//			for (int i = 0; i < a.size(); ++i)
//				a[i] *= b[i];
//			fft(a, -1);
//			resize(a.size());
//			for (int i = 0; i < size(); ++i)
//				operator[](i) = a[i].x;
//			return *this;
//		}
//	private:
//		struct Complex
//		{
//			double x, y;
//			Complex operator*(const Complex&rhs) const { return { x*rhs.x - y*rhs.y, x*rhs.y + y*rhs.x }; }
//			Complex operator+(const Complex&rhs) const { return { x + rhs.x, y + rhs.y }; }
//			Complex operator-(const Complex&rhs) const { return { x - rhs.x, y - rhs.y }; }
//		};
//		static void bitReverse(Polynomial<Complex> &in)
//		{
//			//把每个下标的二进制表示反转(只反转0-lgn位)之后得到新的下标
//			//这样可以把递归树构建出来
//			for (int i = 0, j = 0; i < in.size(); ++i)
//			{
//				if (i > j)
//					std::swap(in[i], in[j]);
//				for (int l = in.size() >> 1; (j ^= l) < l; l >>= 1);
//			}
//		}
//		//f==1把系数表示转换成点值表示,f==-1把点值表示转换成系数表示
//		static void fft(Polynomial<Complex> &in,int f)
//		{
//			const static double pi = acos(-1);
//			bitReverse(in);
//			for (int i = 1; i < in.size(); i <<= 1) {
//				Complex wn{ cos(pi / i), f * sin(pi / i) };
//				for (int j = 0; j < in.size(); j += (i << 1))
//				{
//					Complex w{ 1.0, 0.0 };
//					for (int k = 0; k < i; ++k, w *= wn)
//					{
//						Complex x = in[j + k], y = w * in[j + k + i];
//						in[j + k] = x + y;
//						in[j + k + i] = x - y;
//					}
//				}
//			}
//			if (f == -1)
//				for (int i = 0; i < in.size(); ++i)
//					in[i].x /= in.size();
//		}
//
//	};
}