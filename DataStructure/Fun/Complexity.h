#pragma once
#include "Matrix.h"
#include <functional>
#include <cmath>
#include "Profiler.h"

namespace ds
{
	struct Complexity 
	{
		enum O { lgn, sqrtn, n, nlgn, n2, n3 };
		struct Result { double k, b, R2; };
		static std::array<std::function<double(double)>, 6> table;
		template <typename Decimal, int N,typename Func>
		static Result calc(const Matrix<Decimal, N, 2> &input , Func f)
		{
			//不管什么输入，反正用double处理
			Matrix<double, N, 2> A;
			Matrix<double, N, 1> B;
			for (int i = 0; i<N; ++i)
			{
				A[i][0] = f(input[i][0]);
				A[i][1] = 1;
				B[i][0] = input[i][1];
			}
			//以下求Ax=B的最小二乘解，也就是求ATAx=ATB的解，没有考虑无解的情况
			//考虑一下std::array是在栈上分配的，所以...输入规模不能太大
			auto kb = solve(A.T()*A, A.T()*B);
			double mean = 0.0, R2, k = kb[0][0], b = kb[1][0];
			for (int i = 0; i < N; ++i)
				mean += B[i][0];
			mean /= N;
			double ssr = 0.0, var = 0.0;
			for (int i = 0; i < N; ++i)
				ssr += pow(B[i][0] - k*A[i][0] - b, 2), var += pow(B[i][0] - mean, 2);
			R2 = 1 - ssr / var;
			return { k,b,R2 };
		}
		template <typename Decimal,int N>
		static Result calc(const Matrix<Decimal,N,2> &input,decltype(O::n) type)
		{
			//注意参数不是int!尤其是在上面还有个模板函数的情况下，给int那enum就会被模板匹配到
			return calc(input, table[type]);
		}
		template <typename Func,int N>
		static Result autoCalc(Func f, const std::array<int,N> &n, int type)
		{
			Matrix<double, N, 2> input;
			for (int i = 0; i<N; ++i)
			{
				Profiler prof;
				f(n[i]);
				input[i][1] = prof.reset();
				input[i][0] = n[i];
			}
			return calc(input, table[type]);
		}
		template <typename Func, typename Prep,int N>
		static Result autoCalc(Func f, Prep p,const std::array<int, N> &n, int type)
		{
			Matrix<double, N, 2> input;
			for (int i = 0; i < N; ++i)
			{
				p(n[i]);
				Profiler prof;
				f(n[i]);
				input[i][1] = prof.reset();
				input[i][0] = n[i];
			}
			return calc(input, table[type]);
		}
	};
	std::array<std::function<double(double)>, 6> Complexity::table = 
	{
		[](double d) {return log2(d); },
		[](double d) {return sqrt(d); },
		[](double d) {return d; },
		[](double d) {return d * log2(d); },
		[](double d) {return d * d; },
		[](double d) {return d * d * d; }
	};
}
/*demo
 const int maxn = 5e6;
int a[maxn];
int main()
{
	for (int i = 0; i < maxn; ++i)
		a[i] = i;
	auto f1 = [](int n) {std::sort(a, a + n); };
	auto f2 = [](int n) {ds::radixSort(a, a + n); };
	auto p = [](int n) {random_shuffle(a, a + n); };
	const int start = 1e5, step = 1e5;
	array<int, (maxn - start) / step> arr;
	arr[0] = start;
	for (int i = 1; i < arr.size(); ++i)
		arr[i] = arr[i - 1] + step;
	auto res = Complexity::autoCalc(f1, p, arr, Complexity::nlgn);
	cout << res.k << ' ' << res.b << ' ' << res.R2 << endl;
	res = Complexity::autoCalc(f2, p, arr, Complexity::n);
	cout << res.k << ' ' << res.b << ' ' << res.R2 << endl;
	//运行，得到结果发现b<0，这可能是动态内存申请导致的
	getchar();
}

 */