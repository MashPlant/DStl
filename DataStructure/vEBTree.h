/*#pragma once
#include <utility>
#include <algorithm>

namespace ds
{
	template <int Size> //����vEB����������2^Size
	class vEBTree
	{
	public:
		static int countZero(int i) //����һ������0...010..0�����Ҳ��0�ĸ���
		{
			
#if __GNUC__
			//gcc����
			return __builtin_ctz(i);
#else
			int cnt = 0;
			while (i >>= 1)
				++cnt;
			return cnt;
#endif

			
		}
		
		Ҫʵ��vEB Map�е�����(Ӧ�ò���C++�Ĺ�)
		ÿ���ڵ��min������һ��value��summary������value��size=2�Ľڵ���Ҫ����value
		Ҫôͳһ������(�˷ѿռ�)��Ҫô��ָ��(�˷ѿռ�&ʱ��)��Ҫô��ģ��
		ģ��Ĵ���˼·�ǣ����ȴ���Node*�ĺ������ĳ�ģ��
		Ȼ��size��ͬ��Node��summary��Node�����ɲ�ͬ����
		template<int N,bool IsSummary>
		struct Node {   Node<hi(N),true>*summary ,Node<hi(N),false> cluster[lo(N)]    } //hi��lo���Ǵ�N�ĳ������ʽ
		template<bool IsSummary>
		struct Node<1,IsSummary> {...}
		template<int N>
		struct Node<N,true> {...}
		��Ҳ��֪������д�в��У���������
		
		struct Node
		{
			static int hiSqrt[32], loSqrt[32];
			int zeros = 1;
			int min = -1;
			int max = -1;
			Node *summary = nullptr;
			Node **clusters = nullptr;
			//cluster��hi��vEB��(��СΪlo)��summary��1��vEB��(��СΪhi)
			//cluster[i]��Ϊ�� <=> cluster[i]->min!=-1 <=> exist(summary,i)==true 
			//Ԫ��x��lo(x)����ʽ����clustre[hi(x)]�У���minԪ�ز���������cluster��
			Node() = default;
			Node(int zeros, Node *summary, Node **clusters) :zeros(zeros), summary(summary), clusters(clusters) {}
			//��������ȷ�ܴ����������һ��ǲ�̫����������/ģ�������º����Ż�
			int highPart(int x) const { return x >> (zeros >> 1); }
			int lowPart(int x) const { return x & (loSqrt[zeros] - 1); }
			int numOf(int hi, int lo) const { return (hi << (zeros >> 1)) + lo; }
		};
		static Node* buildTree(int u)
		{
			if (u == 1)
				return new Node();
			const int hi = Node::hiSqrt[u], lo = Node::loSqrt[u];
			Node *summary = buildTree(countZero(hi));
			Node **clusters = new Node*[hi];
			for (int i = 0; i < hi; ++i)
				clusters[i] = buildTree(countZero(lo));
			return new Node(u, summary, clusters);
		}
		Node *root;
		static int findMin(Node *z)
		{
			return z->min;
		}
		static int findMax(Node *z)
		{
			return z->max;
		}
		static void putOnEmpty(Node *z, int x)
		{
			z->min = z->max = x;
		}
		void insert(Node *z, int x)
		{
			if (z->min == -1) //minΪ-1��ζ��maxҲΪ-1,�սڵ�ֱ�Ӳ�
				putOnEmpty(z, x);
			else
			{
				if (x < z->min)
					std::swap(x, z->min);
				if (z->zeros > 1)
				{
					const int hi = z->highPart(x), lo = z->lowPart(x);
					if (findMin(z->clusters[hi]) == -1)
					{
						insert(z->summary, z->highPart(x));//������û��Ԫ�أ���Ҫ�޸�summary
						putOnEmpty(z->clusters[hi], lo);
					}
					else //��������Ԫ��
						insert(z->clusters[hi], lo);
				}
				if (z->max < x) //insert����֮����޸ģ����Ҫ��ѭ������Ҫ������ȥ
					z->max = x; 
			}
		}
		void erase(Node *z, int x)
		{
			if (z->max == z->min) //����ֻ��һ��Ԫ��
				z->max = z->min = -1;
			else if (z->zeros == 1) //����Ԫ�أ�ִ�е�����xֻ��Ϊ0/1
				z->max = z->min = !x;
			else
			{
				if (x == z->min) //�ڴ�����һ��Ԫ��������min
				{
					const int firstCluster = findMin(z->summary); //��һ���ǿյĴ�
					x = z->numOf(firstCluster, findMin(z->clusters[firstCluster])); //�Ǹ����еĵ�һ��Ԫ��
					z->min = x;
				}
				erase(z->clusters[z->highPart(x)], z->lowPart(x));
				//��������˶Դص��޸ģ����½����Ϊ��֮���summary��ά��max
				if (findMin(z->clusters[z->highPart(x)]) == -1) //��ɾ��Ĵر����
				{
					erase(z->summary, z->highPart(x));
					if (x == z->max) //ɾ������max�������֮������min�Ĵ��������棻
					{
						const int maxSummary = findMax(z->summary); //�����õ���summary����max��ά�����ڶ�summary��ά��֮��
						if (maxSummary == -1)
							z->max = z->min;
						else z->max = z->numOf(maxSummary, findMax(z->clusters[maxSummary]));
					}
				}
				else if (x == z->max)
					z->max = z->numOf(z->highPart(x), findMax(z->clusters[z->highPart(x)]));
			}
		}
		int findNext(Node *z, int x)
		{
			if (z->zeros == 1)
				return (x == 0 && z->max == 1) ? 1 : -1;
			if (x < z->min) //�����ж�z->min�ǲ���-1
				return z->min;
			const int inClusterMax = findMax(z->clusters[z->highPart(x)]);
			if (inClusterMax != -1 && z->lowPart(x)<inClusterMax) //next�ڵ�ǰ��
				return z->numOf(z->highPart(x), findNext(z->clusters[z->highPart(x)], z->lowPart(x)));
			const int nextCluster = findNext(z->summary, z->highPart(x)); //next���ڵ�ǰ��
			if (nextCluster == -1)
				return -1;
			return z->numOf(nextCluster, findMin(z->clusters[nextCluster]));
		}
		int findPrev(Node *z, int x)
		{
			if (z->zeros == 1)
				return (x == 1 && z->min == 0) ? 0 : -1;
			if (z->max < x) //�����ж�z->max�ǲ���-1
				return z->max;
			const int clusterMin = findMin(z->clusters[z->highPart(x)]);
			if (clusterMin != -1 && clusterMin < z->lowPart(x))
				return z->numOf(z->highPart(x), findPrev(z->clusters[z->highPart(x)], z->lowPart(x)));
			const int prevCluster = findPrev(z->summary, z->highPart(x));
			if (prevCluster == -1) //z->min����������cluster�У�Ҫ����
				return z->min < x ? z->min : -1; //�����ж�z->min�ǲ���-1
			return z->numOf(prevCluster, findMax(z->clusters[prevCluster]));
		}

	public:
		vEBTree()
		{
			for (int i = 0; i < 32; ++i)
			{
				Node::loSqrt[i] = 1 << (i >> 1);
				Node::hiSqrt[i] = 1 << ((i & 1) ? ((i + 1) >> 1) : (i >> 1));
			}
			root = buildTree(Size);
		}
		int findMin() const
		{
			return root->min;
		}
		int findMax() const
		{
			return root->max;
		}
		bool exist(int x) const
		{
			Node *z = root;
			while (true)
			{
				if (z->min == x || z->max == x)
					return true;
				if (z->zeros == 1)
					return false;
				const int tmp = x;
				x = z->lowPart(x), z = z->clusters[z->highPart(tmp)];
			}
		}
		void insert(int x, bool existed = false) //��������ṩ�������Ϣ(ĳ����һ������/������)�����ʡȥ�ж�
		{
			if (!existed || !exist(x))
				insert(root, x);
		}
		void erase(int x, bool existed = true)
		{
			if (existed || exist(x))
				erase(root, x);
		}
		int findNext(int x) { return findNext(root, x); }
		int findPrev(int x) { return findPrev(root, x); }
	};
	template <int Size>
	int vEBTree<Size>::Node::hiSqrt[32];
	template <int Size>
	int vEBTree<Size>::Node::loSqrt[32];
}*/
