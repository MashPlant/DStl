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
		std::vector<int> split; //必须把每一次分割的维度保存下来才能查询
		std::vector<char> used; //这里并不在乎空间占用，用vector<char>强制避开vector<bool>的空间优化
		void build(int l, int r) //[0,n) indexed
		{
			if (l >= r)  return;
			int mid = (l + r) >> 1;
			//求出每一维的方差 
			split[mid] = 0;
			double var = 0.0;
			for (int d = 0; d < Dimen; ++d)//找到方差最大的那一维,split[mid] = argmax(var,var+Dimen)
			{
				double ave = 0.0, tmpvar = 0.0;
				for (int i = l; i < r; i++)
					ave += dimen[d](pts[i]);
				ave /= r - l;
				for (int i = l; i < r; ++i)
					tmpvar += pow(dimen[d](pts[i]) - ave, 2); //方差没有除元素个数，因为没有必要
				if (tmpvar > var)
					var = tmpvar, split[mid] = d;
			} 
			//找到中间点  
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
		//ans和index只由query使用；放在函数参数里面传太累了
		void query(const K &pt,int l, int r)
		{
			if (l >= r) return;
			int mid = (l + r) >> 1;
			//求出目标点pt到现在的根节点mid的欧几里得距离，没有开根号，因为没有必要
			double dis = 0.0;
			for (int d = 0; d < Dimen; d++)
				dis += pow(dimen[d](pts[mid]) - dimen[d](pt), 2);
			if (!used[mid] && dis < ans) //更新最近距离 
				used[mid] = 1, ans = dis, index = mid;
			//计算pt到[分裂平面]的距离
			//注意到分裂平面是垂直与被分割的维度的坐标轴的，所以点到平面的距离只需要一个坐标差
			double currOfPt = dimen[split[mid]](pt), currOfMid = dimen[split[mid]](pts[mid]);
			double r2 = pow(currOfMid - currOfPt, 2);
			if (currOfPt < currOfMid) //对子区间进行查询,类似二叉树
			{
				query(pt, l, mid);
				//在某个子树查询完之后，以该点为圆心，目前找到的最小距离为半径画"球"
				//看是否和分裂区间的"平面"相交(r2 <= ans)，要是相交的话，最近点可能还在另一个子树上，所以还要再查询另一个子树
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