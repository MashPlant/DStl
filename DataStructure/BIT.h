#pragma once
namespace ds
{
	/*
	 *��״������չ
	 *1.�����޸ĵ������:ά��������飬b[i]:=a[i]-a[i-1]
	 *�޸�:ֻ���޸�����b����ֵ:��b[1] to b[x]�ĺ�
	 *2.�����޸��������:ͬ�����������ԭ��
	 *��ֵ:a[1]+...+a[x] = b[1]*x + b[2]*(x-1) +...+ b[x] 
	 *=(b[1]+...+b[x])*x-(b[1]*0+b[2]*1+...+b[x]*(x-1))
	 *��ά��b[i]*(i-1)���飬ά����ʱ��Ҳֻ���޸�������
	 *3.ģ��ƽ����:��BITTree.h
	 *����+1/ɾ��-1
	 *ǰ��/���/kth => ���� O(lg^2(n))�����ǳ���С
	 */
	//[1,N] indexed
	template <int N>
	class BIT
	{
		int arr[N + 1] = { 0 };
		struct Reference
		{
			BIT *self;
			int pos;
			void operator+=(int d) { self->add(pos, d); }
			void operator-=(int d) { self->add(pos, -d); }
			int operator-(const Reference &rhs) const { return self->query(rhs.pos, pos); }
		};
	protected:
		int n;
	public:
		BIT(int n = N) :n(n) {}
		int sum(int pos) const
		{
			int ret = 0;
			while (pos)
			{
				ret += arr[pos];
				pos &= pos - 1; //�ȼ���pos -= pos & -pos
			}
			return ret;
		}
		int query(int l,int r) const
		{
			//��ѯ[l,r]��ĺͣ���Ҫsum��l-1
			return sum(r) - sum(l - 1);
		}
		void add(int pos,int d)
		{
			while (pos <= n)
			{
				arr[pos] += d;
				pos += pos & -pos;
			}
		}
		Reference operator[](int pos) { return { this,pos }; }
	};
}