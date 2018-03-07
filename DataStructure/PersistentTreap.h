#pragma once
#include "Util.h"
#include <cassert>

namespace ds
{
	//����ʽ����Treap
	//ֻ�ṩmulti_set�Ĺ��ܣ�����ɾ��Ԫ��ʱֻɾһ��(����multi_set��һ��)
	class PersintentTreap
	{
		const static int maxn = 5e5 + 10;
		struct Node
		{
			int key, sz, ch[2];
			unsigned pri;
		} tr[maxn * 50] = {};
		int roots[maxn] = {}, tsize = 0;

		void update(int x)
		{
			tr[x].sz = tr[tr[x].ch[0]].sz + 1 + tr[tr[x].ch[1]].sz;
		}
		//ʵ����split������ͨ������Ҳ������
		//����merge����������pri
		//����tr[now]���ѳ���������x��y
		//����x��keyȫ��<=k,y��keyȫ��>=k
		void split(int now, int k, int &x, int &y)
		{
			if (!now)
				x = y = 0;
			else
			{
				if (tr[now].key <= k)
				{
					x = ++tsize;
					tr[x] = tr[now];
					split(tr[x].ch[1], k, tr[x].ch[1], y);
					update(x);
				}
				else
				{
					y = ++tsize;
					tr[y] = tr[now];
					split(tr[y].ch[0], k, x, tr[y].ch[0]);
					update(y);
				}
			}
		}

		//x�е�����key��С��y�е�����key
		int merge(int x, int y)
		{
			if (!x || !y)
				return x + y;
			if (tr[x].pri > tr[y].pri)
			{
				int p = ++tsize;
				tr[p] = tr[x];
				tr[p].ch[1] = merge(tr[p].ch[1], y);
				update(p);
				return p;
			}
			else
			{
				int p = ++tsize;
				tr[p] = tr[y];
				tr[p].ch[0] = merge(x, tr[p].ch[0]);
				update(p);
				return p;
			}
		}
		int newNode(int k)
		{
			tr[++tsize].key = k;
			tr[tsize].sz = 1;
			tr[tsize].pri = rawRani();
			return tsize;
		}
	public:
		void insert(int nver,int ver, int k)
		{
			int x, y;
			split(roots[ver], k, x, y);
			roots[nver] = merge(merge(x, newNode(k)), y);
		}

		void erase(int nver, int ver, int k)
		{
			int x, y, z;
			split(roots[ver], k, x, z);          //x��ȫ��<=k
			split(x, k - 1, x, y);               //y��ȫ��=k
			y = merge(tr[y].ch[0], tr[y].ch[1]); //ȷ��ֻɾ����һ��Ԫ��
			roots[nver] = merge(merge(x, y), z);
		}
		int kth(int nver, int ver, int k)
		{
			//Ψһһ������ͨƽ����һ����
			roots[nver] = roots[ver];
			int x = roots[ver];
			while (x)
			{
				int tmp = tr[tr[x].ch[0]].sz + 1;
				if (tmp < k)
					k -= tmp, x = tr[x].ch[1];
				else if (k == tmp)
					return tr[x].key;
				else
					x = tr[x].ch[0];
			}
		}
		int rank(int nver, int ver, int k)
		{
			roots[nver] = roots[ver];
			int x, y;
			split(roots[ver], k - 1, x, y);
			int ret = tr[x].sz + 1;
			roots[ver] = merge(x, y);
			return ret;
		}
		int pre(int nver, int ver, int k)
		{
			roots[nver] = roots[ver];
			int x, y;
			split(roots[ver], k - 1, x, y);
			int tmp = x;
			while (tr[x].ch[1]) //<=k-1�����ֵ
				x = tr[x].ch[1];
			int ret = tr[x].key;
			roots[ver] = merge(tmp, y);
			return ret;
		}
		int nxt(int nver, int ver, int k)
		{
			roots[nver] = roots[ver];
			int x, y;
			split(roots[ver], k, x, y);
			int tmp = y;
			while (tr[y].ch[0]) //>k����Сֵ
				y = tr[y].ch[0];
			int ret = tr[y].key;
			roots[ver] = merge(x, tmp);
			return ret;
		}
	};
}
