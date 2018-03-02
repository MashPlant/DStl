#pragma once
#include <vector>
#include <utility>
#include <cmath>

namespace ds
{
	template <typename Number>
	class Polynomial : public std::vector<Number>
	{
	public:
		Polynomial &operator+=(const Polynomial &rhs)
		{
			if (rhs.size() > size())
				resize(rhs.size());
			for (int i = 0; i < size(); ++i)
				operator[](i) += i >= rhs.size() ? Number() : rhs[i];
			return *this;
		}
		Polynomial operator-() const
		{
			Polynomial ret(*this);
			for (int i = 0; i < ret.size(); ++i)
				ret[i] = -ret[i];
			return ret;
		}
		Polynomial &operator-=(const Polynomial &rhs)
		{
			return operator+=(-rhs);
		}
		Polynomial &operator*=(const Polynomial &rhs)
		{
			int nSize = size() + rhs.size();
			int lg = 0;
			for (; 1 << lg < nSize; lg <<= 1);
			nSize = 1 << lg;
			Polynomial<Complex> a(nSize), b(nSize);
			for (int i = 0; i < size(); ++i)
				a[i].x = operator[](i);
			for (int i = 0; i < rhs.size(); ++i)
				b[i].x = rhs[i];
			fft(a, 1);
			fft(b, 1);
			for (int i = 0; i < a.size(); ++i)
				a[i] *= b[i];
			fft(a, -1);
			resize(a.size());
			for (int i = 0; i < size(); ++i)
				operator[](i) = a[i].x;
			return *this;
		}
	private:
		struct Complex
		{
			double x, y;
			Complex operator*(const Complex&rhs) const { return { x*rhs.x - y*rhs.y, x*rhs.y + y*rhs.x }; }
			Complex operator+(const Complex&rhs) const { return { x + rhs.x, y + rhs.y }; }
			Complex operator-(const Complex&rhs) const { return { x - rhs.x, y - rhs.y }; }
		};
		static void bitReverse(Polynomial<Complex> &in)
		{
			//把每个下标的二进制表示反转(只反转0-lgn位)之后得到新的下标
			//这样可以把递归树构建出来
			for (int i = 0, j = 0; i < in.size(); ++i) 
			{
				if (i > j) 
					std::swap(in[i], in[j]);
				for (int l = in.size() >> 1; (j ^= l) < l; l >>= 1);
			}
		}
		//f==1把系数表示转换成点值表示,f==-1把点值表示转换成系数表示
		static void fft(Polynomial<Complex> &in,int f)
		{
			const static double pi = acos(-1);
			bitReverse(in);
			for (int i = 1; i < in.size(); i <<= 1) {
				Complex wn{ cos(pi / i), f * sin(pi / i) };
				for (int j = 0; j < in.size(); j += (i << 1)) 
				{
					Complex w{ 1.0, 0.0 };
					for (int k = 0; k < i; ++k, w *= wn) 
					{
						Complex x = in[j + k], y = w * in[j + k + i];
						in[j + k] = x + y; 
						in[j + k + i] = x - y;
					}
				}
			}
			if (f == -1)
				for (int i = 0; i < in.size(); ++i)
					in[i].x /= in.size();
		}
	
	};
}