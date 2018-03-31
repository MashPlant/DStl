#pragma once
#include <array>
#include <iostream>
#include <random>

namespace ds
{
	template <typename Exp>
	class MatExp;

	template <typename Mat>
	class MatSum
	{
		const Mat &lhs, &rhs;
	public:
		MatSum(const Mat &lhs, const Mat &rhs) : lhs(lhs), rhs(rhs) {}

	};

	template <typename Decimal, int R, int C>
	class Matrix
	{
		typedef std::array<Decimal, C> row;
		std::array<row, R> M;
	public:
		Matrix() //array居然不会自动初始化，这很不stl
		{
			if (!std::is_class<Decimal>::value)
				memset(&M, 0, sizeof(M));
		}
		Matrix(const std::initializer_list<std::array<Decimal, C>> &lst)
		{
			for (int i = 0; i < R; ++i)
				M[i] = lst.begin()[i];
		}
		constexpr static int r() { return R; }
		constexpr static int c() { return C; }
		row &operator[](int n) { return M[n]; } //获取某行
		const row &operator[](int n) const { return M[n]; }
		Matrix& operator+=(const Matrix &rhs) //模板天然保证了形状一致
		{
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					M[i][j] += rhs[i][j];
			return *this;
		}
		Matrix operator+(const Matrix &rhs) const
		{
			Matrix ret(rhs);
			return ret += *this;
		}
		Matrix operator-() const
		{
			Matrix ret(*this);
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					ret[i][j] = -ret[i][j];
			return ret;
		}
		Matrix& operator-=(const Matrix &rhs) { return *this += (-rhs); }
		Matrix operator-(const Matrix &rhs) const { return *this + (-rhs); }
		template <int RC>
		Matrix<Decimal, R, RC> operator*(const Matrix<Decimal, C, RC> &rhs) const
		{
			Matrix<Decimal, R, RC> ret;
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < RC; ++j)
					for (int k = 0; k < C; ++k)
						ret[i][j] += M[i][k] * rhs[k][j];
			return ret;
		}
		Matrix operator*(Decimal x) const
		{
			Matrix ret(*this);
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					ret[i][j] *= x;
			return ret;
		}
		Matrix<Decimal, C, R> T() const
		{
			Matrix<Decimal, C, R> ret;
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					ret[j][i] = M[i][j];
			return ret;
		}
		template <typename Pred>
		Decimal reduce(Pred pred, Decimal origin = 0) const
		{
			Decimal ret = origin;
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					ret = pred(ret, M[i][j]);
			return ret;
		}
		template <typename Pred>
		Matrix map(Pred pred) const
		{
			Matrix ret(*this);
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					ret[i][j] = pred(ret[i][j]);
			return ret;
		}
		void fill(Decimal x)
		{
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					M[i][j] = x;
		}
		friend std::ostream& operator<<(std::ostream &os, const Matrix &rhs)
		{
			for (int i = 0; i < R; ++i)
				for (int j = 0; j < C; ++j)
					os << rhs[i][j] << " \n"[j == C - 1];
			return os;
		}
	};

	template <typename Decimal, int R, int C>
	Matrix<Decimal, R, C> operator*(Decimal x, const Matrix<Decimal, R, C> &m) { return m * x; }

	template <typename Decimal, int R, int C>
	Matrix<Decimal, R, C> dot(const Matrix<Decimal, R, C> &lhs, const Matrix<Decimal, R, C> &rhs)
	{
		auto ret(lhs);
		for (int i = 0; i < R; ++i)
			for (int j = 0; j < C; ++j)
				ret[i][j] *= rhs[i][j];
		return ret;
	}
	template <typename Decimal, int R>
	int argmax(const Matrix<Decimal, R, 1> &m)
	{
		int ret = 0;
		for (int i = 1; i < R; ++i)
			if (m[i][0] > m[ret][0])
				ret = i;
		return ret;
	}
	template <typename Decimal, int N>
	Matrix<Decimal, N, 1> solve(Matrix<Decimal, N, N> A, Matrix<Decimal, N, 1> B)
	{
		for (int i = 0; i < N; ++i)
		{
			if (A[i][i] == 0)
			{
				bool ok = false;
				for (int j = i + 1; j < N && !ok; ++j)
					if (A[j][i] != 0)
						std::swap(A[i], A[j]), ok = true;
				if (!ok)
					throw std::runtime_error("A is not invertible");
			}
			B[i][0] /= A[i][i];
			for (int j = N - 1; j >= i; --j)
				A[i][j] /= A[i][i];
			for (int j = 0; j < N; ++j)
				if (i != j)
				{
					for (int k = i + 1; k < N; ++k)
						A[j][k] -= A[j][i] * A[i][k];
					B[j][0] -= A[j][i] * B[i][0];
				}
		}
		return B;
	}
}

//#pragma once
//#include <array>
//#include <iostream>
//#include <random>
//#include <type_traits>
//
//namespace ds
//{
//	struct ExpBase //仅用来给SFINAE识别，没有实际用途
//	{	}; 
//
//	template <typename Exp>
//	class MatSum : ExpBase
//	{
//		const Exp &lhs, &rhs;
//	public:
//		typedef typename Exp::ValueType ValueType;
//		const static int Row = Exp::Row;
//		const static int Col = Exp::Col;
//		MatSum(const Exp &lhs, const Exp &rhs) : lhs(lhs), rhs(rhs) {}
//		ValueType at(int x, int y) const { return lhs.at(x, y) + rhs.at(x, y); }
//	};
//	template <typename Exp>
//	class MatSub : ExpBase
//	{
//		const Exp &lhs, &rhs;
//	public:
//		typedef typename Exp::ValueType ValueType;
//		const static int Row = Exp::Row;
//		const static int Col = Exp::Col;
//		MatSub(const Exp &lhs, const Exp &rhs) : lhs(lhs), rhs(rhs) {}
//		ValueType at(int x, int y) const { return lhs.at(x,y) - rhs.at(x, y); }
//	};
//	template <typename LExp,typename RExp>
//	class MatMul : ExpBase
//	{
//		const LExp &lhs;
//		const RExp &rhs;
//	public:
//		static_assert(std::is_same<typename LExp::ValueType, typename RExp::ValueType>::value, "LExp and RExp must have the same value type");
//		static_assert(LExp::Col == RExp::Row, "shape of LExp and RExp are incompatible");
//		const static int Row = LExp::Row;
//		const static int Col = RExp::Col;
//		typedef typename LExp::ValueType ValueType;
//		MatMul(const LExp &lhs, const RExp &rhs) : lhs(lhs), rhs(rhs) {}
//		ValueType at(int x, int y) const
//		{
//			ValueType ret = 0;
//			for (int i = 0; i < LExp::Col; ++i)
//				ret += lhs.at(x, i) * rhs.at(i, y);
//			return ret;
//		}
//	};
//
//	template <typename Exp,typename = typename std::enable_if<std::is_base_of<ExpBase,Exp>::value>::type>
//	MatSum<Exp> operator+(const Exp &lhs, const Exp &rhs) { return MatSum<Exp>(lhs, rhs); }
//	template <typename Exp, typename = typename std::enable_if<std::is_base_of<ExpBase, Exp>::value>::type>
//	MatSub<Exp> operator-(const Exp &lhs, const Exp &rhs) { return MatSub<Exp>(lhs, rhs); }
//	template <typename LExp, typename RExp,typename = typename std::enable_if<std::is_base_of<ExpBase, LExp>::value>::type>
//	MatMul<LExp,RExp> operator*(const LExp &lhs, const RExp &rhs) { return MatMul<LExp,RExp>(lhs, rhs); }
//
//	template <typename Decimal, int R, int C>
//	class Matrix : ExpBase
//	{
//		typedef std::array<Decimal, C> row;
//		std::array<row, R> M;
//	public:
//		typedef Decimal ValueType;
//		const static int Row = R;
//		const static int Col = C;
//		constexpr static int r() { return R; }
//		constexpr static int c() { return C; }
//		row &operator[](int n) { return M[n]; } //获取某行
//		const row &operator[](int n) const { return M[n]; }
//		Decimal &at(int x, int y) { return M[x][y]; }
//		Decimal at(int x, int y) const { return M[x][y]; }
//		Matrix() //array居然不会自动初始化，这很不stl
//		{
//			if (!std::is_class<Decimal>::value)
//				memset(&M, 0, sizeof(M));
//		}
//		template <typename Exp>
//		Matrix(const Exp &e)
//		{
//			for (int i = 0; i < R; ++i)
//				for (int j = 0; j < C; ++j)
//					M[i][j] = e.at(i, j);
//		}
//		template <typename Exp>
//		Matrix &operator=(const Exp &e)
//		{
//			for (int i = 0; i < R; ++i)
//				for (int j = 0; j < C; ++j)
//					M[i][j] = e.at(i, j);
//			return *this;
//		}
//	};
//
//	/*template <typename Decimal, int R, int C>
//	Matrix<Decimal, R, C> operator*(Decimal x, const Matrix<Decimal, R, C> &m) { return m * x; }
//
//	template <typename Decimal, int R, int C>
//	Matrix<Decimal, R, C> dot(const Matrix<Decimal, R, C> &lhs, const Matrix<Decimal, R, C> &rhs)
//	{
//		auto ret(lhs);
//		for (int i = 0; i < R; ++i)
//			for (int j = 0; j < C; ++j)
//				ret[i][j] *= rhs[i][j];
//		return ret;
//	}
//	template <typename Decimal, int R>
//	int argmax(const Matrix<Decimal, R, 1> &m)
//	{
//		int ret = 0;
//		for (int i = 1; i < R; ++i)
//			if (m[i][0] > m[ret][0])
//				ret = i;
//		return ret;
//	}
//	template <typename Decimal, int N>
//	Matrix<Decimal, N, 1> solve(Matrix<Decimal, N, N> A,Matrix<Decimal,N,1> B)
//	{
//		for (int i = 0; i < N; ++i)
//		{
//			if (A[i][i] == 0)
//			{
//				bool ok = false;
//				for (int j = i + 1; j < N && !ok; ++j)
//					if (A[j][i] != 0)
//						std::swap(A[i], A[j]), ok = true;
//				if (!ok)
//					throw std::runtime_error("A is not invertible");
//			}
//			B[i][0] /= A[i][i];
//			for (int j = N - 1; j >= i; --j)
//				A[i][j] /= A[i][i];
//			for (int j = 0; j < N; ++j)
//				if (i != j)
//				{
//					for (int k = i + 1; k < N; ++k)
//						A[j][k] -= A[j][i] * A[i][k];
//					B[j][0] -= A[j][i] * B[i][0];
//				}	
//		}
//		return B;
//	}*/
//}
