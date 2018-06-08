#pragma once
#include "Vector.h"
namespace ds
{
	template <typename T>
	class MinMaxHeap
	{
		std::vector<T> vec;
		//isMaxLevelָ���������������к��Ӵ�
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
			if (maxLevel) //pos��max level�����ĺ���Ӧ�ñ�����ѡ���ϴ�ĺ���
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
			if (maxLevel) //pos��max level����������Ӧ�ñ�����ѡ����������
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
				if ((nxt = checkSon(pos, maxLevel, size)) != 0) //�������㺢�ӣ����������˺���֮���ܸ���nxt
					std::swap(vec[nxt], vec[pos]);
				if ((nxt = checkGSon(pos, maxLevel, size)) != 0) //������ӵ�ǰ�����Ѿ������˺��ӣ���ʱ���Ը���nxt
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
					if (vec[x] < vec[p]) //����max����ζ�����ĸ�����min����Ҫ����С
						std::swap(vec[p], vec[x]), nxt = p, maxLevel ^= 1;
					else if (p && vec[g] < vec[x]) //�����游Ҳ��max�����������游������Ҫ�����游
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