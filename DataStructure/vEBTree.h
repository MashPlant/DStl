#pragma once
#include <utility>
#include <bitset>

namespace ds
{
	namespace vEB
	{
		constexpr int lodiv2(int i) { return i >> 1; }
		constexpr int hidiv2(int i) { return (i >> 1) + (i & 1); }
		template <int N>
		struct Node
		{
			int min = -1, max = -1;
			//ԭ��֮���Բ��ò���ָ�룬ֻ����Ϊ��ͬ��С��Node����һ������
			//���������ǲ�ͬ���͵ģ�������Ҫָ�룬Ҳ����Ҫ�ݹ齨��
			Node<hidiv2(N)> summary;
			Node<lodiv2(N)> clusters[1 << hidiv2(N)];
			constexpr static int highPart(int x) { return x >> lodiv2(N); }
			constexpr static int lowPart(int x) { return x & ((1 << lodiv2(N)) - 1); }
			constexpr static int numOf(int hi, int lo) { return (hi << lodiv2(N)) + lo; }
		};
		template <>
		struct Node<1> //����C++�������������������ʶ���������������ռ���
		{
			int min = -1, max = -1;
		};
	}
	template <int Size> //����vEB����������2^Size
	class vEBTree
	{
	private:
		vEB::Node<Size> root;
		std::bitset<1 << Size> table; //O(1)exist��ѯ
		template <int N>
		static void putOnEmpty(vEB::Node<N> &z, int x) { z.min = z.max = x; }
		template <int N>
		static void insert(vEB::Node<N> &z,int x)
		{
			if (z.min == -1) //minΪ-1��ζ��maxҲΪ-1,�սڵ�ֱ�Ӳ�
				putOnEmpty(z, x);
			else
			{
				if (x < z.min)
					std::swap(x, z.min);
				//��ʱN > 1
				const int hi = z.highPart(x), lo = z.lowPart(x);
				if (z.clusters[hi].min == -1)
				{
					insert(z.summary, z.highPart(x));//������û��Ԫ�أ���Ҫ�޸�summary
					putOnEmpty(z.clusters[hi], lo);
				}
				else //��������Ԫ��
					insert(z.clusters[hi], lo);
				if (x > z.max)//insert����֮����޸ģ����Ҫ��ѭ������Ҫ������ȥ
					z.max = x;
			}
		}
		static void insert(vEB::Node<1> &z,int x)
		{
			if (z.min == -1) //minΪ-1��ζ��maxҲΪ-1,�սڵ�ֱ�Ӳ�
				putOnEmpty(z, x);
			else
			{
				if (x < z.min)
					std::swap(x, z.min);
				if (z.max < x) 
					z.max = x;
			}
		}
		template <int N>
		static void erase(vEB::Node<N> &z, int x)
		{
			if (z.max == z.min) //����ֻ��һ��Ԫ��
				z.max = z.min = -1;
			else
			{
				if (x == z.min) //�ڴ�����һ��Ԫ��������min
				{
					const int firstCluster = z.summary.min; //��һ���ǿյĴ�
					x = z.numOf(firstCluster, z.clusters[firstCluster].min); //�Ǹ����еĵ�һ��Ԫ��
					z.min = x;
				}
				erase(z.clusters[z.highPart(x)], z.lowPart(x));
				//��������˶Դص��޸ģ����½����Ϊ��֮���summary��ά��max
				if (z.clusters[z.highPart(x)].min == -1) //��ɾ��Ĵر����
				{
					erase(z.summary, z.highPart(x));
					if (x == z.max) //ɾ������max�������֮������min�Ĵ��������棻
					{
						const int maxSummary = z.summary.max; //�����õ���summary����max��ά�����ڶ�summary��ά��֮��
						if (maxSummary == -1)
							z.max = z.min;
						else z.max = z.numOf(maxSummary, z.clusters[maxSummary].max);
					}
				}
				else if (x == z.max)
					z.max = z.numOf(z.highPart(x), z.clusters[z.highPart(x)].max);
			}
		}
		static void erase(vEB::Node<1> &z, int x)
		{
			if (z.max == z.min) //����ֻ��һ��Ԫ��
				z.max = z.min = -1;
			else
				z.max = z.min = !x;
		}

		template <int N>
		static int findNext(const vEB::Node<N> &z, int x)
		{
			if (x < z.min) //�����ж�z.min�ǲ���-1
				return z.min;
			const int inClusterMax = z.clusters[z.highPart(x)].max;
			if (inClusterMax != -1 && z.lowPart(x)<inClusterMax) //next�ڵ�ǰ��
				return z.numOf(z.highPart(x), findNext(z.clusters[z.highPart(x)], z.lowPart(x)));
			const int nextCluster = findNext(z.summary, z.highPart(x)); //next���ڵ�ǰ��
			if (nextCluster == -1)
				return -1;
			return z.numOf(nextCluster, z.clusters[nextCluster].min);
		}
		static int findNext(const vEB::Node<1> &z, int x) { return (x == 0 && z.max == 1) ? 1 : -1; }

		template <int N>
		static int findPrev(const vEB::Node<N> &z, int x)
		{
			if (x > z.max) //�����ж�z.max�ǲ���-1
				return z.max;
			const int clusterMin = z.clusters[z.highPart(x)].min;
			if (clusterMin != -1 && clusterMin < z.lowPart(x))
				return z.numOf(z.highPart(x), findPrev(z.clusters[z.highPart(x)], z.lowPart(x)));
			const int prevCluster = findPrev(z.summary, z.highPart(x));
			if (prevCluster == -1) //z.min����������cluster�У�Ҫ����
				return x > z.min ? z.min : -1; //�����ж�z.min�ǲ���-1
			return z.numOf(prevCluster, z.clusters[prevCluster].max);
		}
		static int findPrev(const vEB::Node<1> &z, int x) { return (x == 1 && z.min == 0) ? 0 : -1; }

	public:
		int findMin() const { return root.min; }
		int findMax() const { return root.max; }
		bool exist(int x) const { return table[x]; }
		void insert(int x)
		{
			if (!table[x])
				table[x] = true, insert(root, x);
		}
		void erase(int x)
		{
			if (table[x])
				table[x] = false, erase(root, x);
		}
		int findNext(int x) { return findNext(root, x); }
		int findPrev(int x) { return findPrev(root, x); }
	};
}
