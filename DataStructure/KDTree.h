#pragma once
#include <functional>
#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
namespace ds
{
	template <typename K, int Dimen>
	class KDTree
	{
		const std::array<std::function<double(const K&)>, Dimen> dimen;
		std::vector<K> pts;
		std::vector<int> split; //�����ÿһ�ηָ��ά�ȱ����������ܲ�ѯ
		std::vector<char> used; //���ﲢ���ں��ռ�ռ�ã���vector<char>ǿ�Ʊܿ�vector<bool>�Ŀռ��Ż�
		void build(int l, int r) //[0,n) indexed
		{
			if (l >= r)  return;
			int mid = (l + r) >> 1;
			//���ÿһά�ķ��� 
			split[mid] = 0;
			double var = 0.0;
			for (int d = 0; d < Dimen; ++d)//�ҵ�����������һά,split[mid] = argmax(var,var+Dimen)
			{
				double ave = 0.0, tmpvar = 0.0;
				for (int i = l; i < r; i++)
					ave += dimen[d](pts[i]);
				ave /= r - l;
				for (int i = l; i < r; ++i)
					tmpvar += pow(dimen[d](pts[i]) - ave, 2); //����û�г�Ԫ�ظ�������Ϊû�б�Ҫ
				if (tmpvar > var)
					var = tmpvar, split[mid] = d;
			} 
			//�ҵ��м��  
			std::nth_element(pts.begin() + l, pts.begin() + mid, pts.begin() + r,
				[=](const K &lhs, const K &rhs)
			{return dimen[split[mid]](lhs) < dimen[split[mid]](rhs); });

			build(l, mid);
			build(mid + 1, r);
		}

		struct Result
		{
			const K &point;
			double distance2;
		};
		double ans;
		int index;
		//ans��indexֻ��queryʹ�ã����ں����������洫̫����
		void query(const K &pt,int l, int r)
		{
			if (l >= r) return;
			int mid = (l + r) >> 1;
			//���Ŀ���pt�����ڵĸ��ڵ�mid��ŷ����þ��룬û�п����ţ���Ϊû�б�Ҫ
			double dis = 0.0;
			for (int d = 0; d < Dimen; d++)
				dis += pow(dimen[d](pts[mid]) - dimen[d](pt), 2);
			if (!used[mid] && dis < ans) //����������� 
				used[mid] = 1, ans = dis, index = mid;
			//����pt��[����ƽ��]�ľ���
			//ע�⵽����ƽ���Ǵ�ֱ�뱻�ָ��ά�ȵ�������ģ����Ե㵽ƽ��ľ���ֻ��Ҫһ�������
			double currOfPt = dimen[split[mid]](pt), currOfMid = dimen[split[mid]](pts[mid]);
			double r2 = pow(currOfMid - currOfPt, 2);
			if (currOfPt < currOfMid) //����������в�ѯ,���ƶ�����
			{
				query(pt, l, mid);
				//��ĳ��������ѯ��֮���Ըõ�ΪԲ�ģ�Ŀǰ�ҵ�����С����Ϊ�뾶��"��"
				//���Ƿ�ͷ��������"ƽ��"�ཻ(r2 <= ans)��Ҫ���ཻ�Ļ����������ܻ�����һ�������ϣ����Ի�Ҫ�ٲ�ѯ��һ������
				if (r2 <= ans)
					query(pt, mid + 1, r);
			}
			else
			{
				query(pt, mid + 1, r);
				if (r2 <= ans) 
					query(pt, l, mid);
			}
		}
	public:
		KDTree(decltype(dimen) dimen, const std::vector<K> &pts) 
		:dimen(dimen), pts(pts),split(pts.size()),used(split.size())
		{
			build(0, pts.size());
		}
		std::vector<Result> knn(const K &pt,int k) 
		{
			std::vector<Result> ret;
			memset(&used[0], 0, used.size());
			while (k--)
			{
				ans = DBL_MAX, index = -1;
				query(pt, 0, pts.size());
				used[index] = 1;
				ret.push_back({ pts[index] ,ans });
			}
			return ret;
		}
	};
}
/* demo
array<function<double(const P&p)>, 2> arr =
{
[](const P&p) {return p.x; },
[](const P&p) {return p.y; },
};
const int maxn = 200000;
vector<P> input(maxn);
for (int i = 0; i < input.size(); ++i)
input[i] = { rani(1,maxn),rani(1,maxn) };
KDTree<P, 2> kd(arr, input);
auto knn = kd.knn({ maxn / 2,maxn / 2 }, 20);
for (auto &p : knn)
cout << p.point.x << ' ' << p.point.y << ' ' << sqrt(p.distance2) << endl;
getchar();
 */