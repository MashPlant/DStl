#pragma once
#include "Vector.h"
namespace ds
{
	template <typename T>
	class MinMaxHeap
	{
		std::vector<T> vec;
		//isMaxLevel指的是它比它的所有孩子大
		static bool isMaxLevel(int pos)
		{
			int lg = 0;
			for (int sum = 0, i = 1; sum < pos; sum += (i <<= 1))
				++lg;
			return lg & 1;
		}
		int checkSon(int pos, bool maxLevel, int size)
		{
			int son = (pos << 1) + 1;
			if (maxLevel) //pos是max level，它的孩子应该比他大，选出较大的孩子
			{
				son += (son + 1 < size && vec[son] < vec[son + 1]);
				return (son < size && vec[pos] < vec[son]) ? son : 0;
			}
			son += (son + 1 < size && vec[son + 1] < vec[son]);
			return (son < size && vec[son] < vec[pos]) ? son : 0;
		}
		int checkGSon(int pos, bool maxLevel, int size)
		{
			const int gson[4] = { (pos << 2) + 3,(pos << 2) + 4,(pos << 2) + 5,(pos << 2) + 6 };
			int ret = pos;
			if (maxLevel) //pos是max level，它的孙子应该比他大，选出最大的孙子
			{
				for (int i = 0; i < 4 && gson[i] < size; ++i)
					if (vec[ret] < vec[gson[i]])
						ret = gson[i];
				return ret == pos ? 0 : ret;
			}
			for (int i = 0; i < 4 && gson[i] < size; ++i)
				if (vec[gson[i]] < vec[ret])
					ret = gson[i];
			return ret == pos ? 0 : ret;
		}
		void popFix(int pos)
		{
			const bool maxLevel = isMaxLevel(pos);
			const int size = vec.size();
			int nxt;
			while (true)
			{
				if ((nxt = checkSon(pos, maxLevel, size)) != 0) //优先满足孩子，但是满足了孩子之后不能更新nxt
					std::swap(vec[nxt], vec[pos]);
				if ((nxt = checkGSon(pos, maxLevel, size)) != 0) //检查孙子的前提是已经满足了孩子，这时可以更新nxt
					std::swap(vec[nxt], vec[pos]), pos = nxt;
				else break;
			}
		}
	public:
		void push(const T &val)
		{
			vec.push_back(val);
			int x = vec.size() - 1, p = (x - 1) >> 1, g = (p - 1) >> 1, nxt;
			bool maxLevel = isMaxLevel(x);
			while (x)
			{
				if (maxLevel) 
				{
					if (vec[x] < vec[p]) //它是max，意味着它的父亲是min，需要比他小
						std::swap(vec[p], vec[x]), nxt = p, maxLevel ^= 1;
					else if (p && vec[g] < vec[x]) //它的祖父也是max，若果它比祖父大则需要更新祖父
						std::swap(vec[g], vec[x]), nxt = g;
					else break;
				}
				else
				{
					if (vec[p] < vec[x]) 
						std::swap(vec[p], vec[x]), nxt = p, maxLevel ^= 1;
					else if (p && vec[x] < vec[g]) 
						std::swap(vec[g], vec[x]), nxt = g;
					else break;
				}
				x = nxt, p = (x - 1) >> 1, g = (p - 1) >> 1;
			}
		}
		void popMin() 
		{
			std::swap(vec.front(), vec.back());
			vec.pop_back();
			popFix(0);
		}
		void popMax() 
		{
			int pos = &max() - &vec[0];
			std::swap(vec[pos], vec.back());
			vec.pop_back();
			popFix(pos);
		}
		const T &min() const noexcept { return vec[0]; }
		const T &max() const noexcept
		{
			if (vec.size() == 1)
				return vec[0];
			if (vec.size() == 2 || vec[2] < vec[1])
				return vec[1];
			return vec[2];
		}
		int size() const { return vec.size(); }
		bool empty() const { return vec.empty(); }
	};
}