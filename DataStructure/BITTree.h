#pragma once
#include "BIT.h"
#include "Algorithm.h"
namespace ds
{
	//����Ȼ����Ϊ��״����֧��[1,N](ҵ��ϰ��)
	//����BITTreeҲֻ֧��[1,N]��Ҫ������[0,N)�Ļ��Ծ��ڿͻ���-1��
	//��֧������ƽ��������(��������vEB)�����ҳ����ǳ�С
	//��������¿��Ե�������������(vEB����)��Ҫ��Nʵ��̫�󻹿���������ɢ��
	template <int N, bool Multi = false>
	struct BITTree : BIT<N>
	{
		//���к����Ĳ������Ծ���֤�Ϸ�
		//Ӧ�ò������˹��⴫������������(����)
		int count(int x) const { return query(x, x); }
		bool exist(int x) const { return count(x); }
		void insert(int x)
		{
			if (Multi || !count(x))
				add(x, 1);
		}
		void erase(int x)
		{
			//Multi�����erase�ǲ���ȷ��
			//����˵stl������Ĩ�������Ǻܶ���Ŀ��Ҫ��ֻȥ��һ��
			if (count(x))
				add(x, -1);
		}
		int findNext(int x) const //lg^2
		{
			int base = sum(x);
			if (sum(N) == base)
				return -1;
			return ds::bisearch(x + 1, N + 1,
				[=](int pos) {return sum(pos) > base; });
			//������ҵ���һ��[x+1,pos]��ĺͲ�Ϊ0��pos�����ǵĵ����
		}
		int findPrev(int x) const
		{
			int base = sum(x - 1);
			if (base == 0)
				return -1;
			return ds::bisearch(1, x,
				[=](int pos) {return sum(pos) == base; });
			//���ص�һ��[1,pos]�ĺ�=[1,x-1]�ĺ͵�pos
			//����pos����"�������һ�θı�"��λ�ã���ǰ��
		}
		int findMin() const { return findNext(0); }
		int findMax() const
		{
			if (exist(N))
				return N;
			return findPrev(N);
		}
		int kth(int k) const
		{
			if (k > size() || k < 1)
				return -1;
			return ds::bisearch(1, N + 1
				, [=](int pos) {return sum(pos) >= k; });
		}
		//��Ҫ��x����
		int rank(int x) const { return sum(x - 1) + 1; }
		//��size��ҪlgNʱ�䣬��ñ�������
		int size() const { return sum(N); }
	};
}