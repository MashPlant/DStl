#pragma once
#include  <utility>
namespace ds
{
	namespace TreeUtil
	{
		template <typename LinkType>
		LinkType extremeChild(LinkType  x, LinkType nil, const int what)
		{
			while (x->ch[what] != nil)
				x = x->ch[what];
			return x;
		}

		template <typename LinkType>
		LinkType nextNode(LinkType x, LinkType nil)
		{
			if (x->ch[1] != nil)
				return extremeChild(x->ch[1], nil, 0);
			while (x->p->ch[1] == x)
				x = x->p;
			return x->p;//x是最大节点时p为nil_
		}

		template<typename LinkType>
		LinkType prevNode(LinkType  x, LinkType nil, LinkType root)
		{
			if (x == nil) //end()的前一个
				return extremeChild(root, nil, 1);
			if (x->ch[0] != nil)
				return extremeChild(x->ch[0], nil, 1);
			while (x->p->ch[0] == x)
				x = x->p;
			return x->p;//x是最小节点时p为nil_
		}

		template<typename LinkType, typename KeyType, typename Comp>
		std::pair<LinkType, LinkType > findNode(const KeyType &key, LinkType  x, LinkType nil, Comp comp) //从x开始搜
		{
			LinkType  p = nil;
			while (x != nil)
			{
				if (comp(x->key(), key))
					p = x, x = x->ch[1];
				else if (!comp(key, x->key()))
					return { x,p };
				else
					p = x, x = x->ch[0];
			}
			return { nullptr,p };
		}
		template<typename LinkType, typename KeyType, typename Comp>
		int lower(const KeyType &rend, LinkType root, LinkType nil, const bool afterEqual, Comp comp)
		{
			LinkType  x = root;
			int cnt = 0;
			while (x != nil)
			{
				if (comp(x->key(), rend) || (afterEqual && !comp(rend, x->key())))
				{
					cnt += x->ch[0]->sz + 1;
					x = x->ch[1];
				}
				else
					x = x->ch[0];
			}
			return cnt;
		}
		template<typename LinkType>
		LinkType kth(LinkType root, LinkType nil, int k) //k>size() or k<1时自动返回end()
		{
			LinkType  x = root;
			while (x != nil)
			{
				LinkType l = x;
				l = l->ch[0];
				if (l->sz + 1 == k)
					return x;
				if (l->sz + 1 < k)
				{
					k -= l->sz + 1;
					x = x->ch[1];
				}
				else
					x = l;
			}
			return nil;
		}

		template<typename LinkType>
		void clear(LinkType x, LinkType nil)
		{
			if (!x || x == nil)
				return;
			clear(x->ch[0], nil);
			clear(x->ch[1], nil);
			delete x;
		}

		template<typename LinkType>
		void cpy(LinkType dest, LinkType  destnil, const LinkType sour, const LinkType sournil)
		{
			for (int son = 0; son <= 1; ++son)
				if (sour->ch[son] != sournil)
				{
					dest->ch[son] = sour->ch[son]->clone(dest, destnil, destnil); //clone返回信息一样的节点
					cpy(dest->ch[son], destnil, sour->ch[son], sournil);
				}
		}
	}
}