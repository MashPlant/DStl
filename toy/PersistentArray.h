#pragma once
namespace ds
{
	//下标是[1,N]indexed
	//版本是[0,MaxVersion)indexed
	template <typename K,int N,int MaxVersion>
	class PersistentArray
	{
		int roots[MaxVersion];
		struct Node
		{
			int l, r;
			K key;
		};
		Node tree[N * 4 + MaxVersion * 32]; //树的大小是O(N+MaxVer*lg(N)),O(N)是为了一开始建树的需要,lgN就用32来代替
		int size = 0;
		int build(int l, int r)
		{
			int root = ++size;
			int mid = (l + r) >> 1;
			if (l < r)
			{
				tree[root].l = build(l, mid);
				tree[root].r = build(mid + 1, r);
			}
			return root;
		}
		template <typename Pred>
		int update(int pre, int l, int r, int where, Pred pred)
		{
			int root = ++size;
			tree[root].l = tree[pre].l;
			tree[root].r = tree[pre].r;
			int mid = (l + r) >> 1;
			if (l < r)
			{
				if (where <= mid)
					tree[root].l = update(tree[pre].l, l, mid, where, pred);
				else
					tree[root].r = update(tree[pre].r, mid + 1, r, where, pred);
			}
			else
				pred(tree[root].key);
			return root;
		}
	public:
		PersistentArray() { roots[0] = build(1, N); }
		K &query(int ver, int where)
		{
			int l = 1, r = N, cur = roots[ver];
			while (l != r)
			{
				int mid = (l + r) >> 1;
				if (where <= mid)
					cur = tree[cur].l, r = mid;
				else
					cur = tree[cur].r, l = mid + 1;
			}
			return tree[cur].key;
		}
		template <typename Pred>
		void update(int nver,int ver,int where,Pred pred)
		{
			roots[nver] = update(roots[ver], 1, N, where, pred);
		}
		void update(int nver, int ver, int where,const K &key)
		{
			roots[nver] = update(roots[ver], 1, N, where, 
				[&](K &old) {old = key; });
		}
		void update(int nver, int ver) { roots[nver] = roots[ver]; }
	};
}