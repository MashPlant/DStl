#pragma once
#include <utility>
namespace ds
{
	template <typename Traits>
	class Splay
	{
	private:
		typedef typename Traits::KeyType KeyType;
		typedef typename Traits::ValueType ValueType;
		typedef typename Traits::KeyCompare KeyCompare;
		KeyCompare comp;
	public:
		struct Node;
		typedef Node* LinkType;
		typedef std::pair<LinkType, LinkType> NodePair;
		struct Node
		{
			LinkType p = nullptr, ch[2] = { nullptr };
			ValueType value;
			int sz = 0;
			const KeyType &key() const { return Traits::KeyOf(value); }
			ValueType &operator*() { return value; }
			ValueType *operator->() { return &(operator*()); }
			bool isRight()const { return this == p->ch[1]; }
			Node() = default;
			Node(LinkType p, LinkType l, LinkType r, const ValueType &value) :p(p), value(value), sz(1) { ch[0] = l, ch[1] = r; }
		};
	private:
		LinkType nil_, root_;
		static void updSz(LinkType x) { x->sz = x->ch[0]->sz + x->ch[1]->sz + 1; }
		void rotate(LinkType x) //把x向上旋转
		{
			LinkType p = x->p, g = p->p;
			const bool rx = x == p->ch[1], rp = p == g->ch[1];
			x->p = g; //把x接到g上
			g->ch[rp] = x;
			p->ch[rx] = x->ch[!rx]; //把p的与x异侧的孙节点接到p上
			x->ch[!rx]->p = p;
			p->p = x; //把p接到x上
			x->ch[!rx] = p;
			if (root_ == p)
				root_ = x;
			updSz(p), updSz(x);
		}
		void splay(const LinkType dest, LinkType sour)
		{
			while (sour->p != dest)
			{
				LinkType p = sour->p;
				if (p->p != dest) //避免错过
					(p->isRight() == sour->isRight()) ? rotate(p) : rotate(sour);
				rotate(sour);
			}
			if (dest == nil_)
				sour = root_;
		}
		void transplant(LinkType dest, LinkType sour)
		{
			LinkType dp = dest->p;
			sour->p = dp;
			if (dp == nil_)
				root_ = sour;
			else
				dp->ch[dest == dp->ch[1]] = sour;
		}
		LinkType extremeChild(LinkType x, const int what) const
		{
			while (x->ch[what] != nil_)
				x = x->ch[what];
			return x;
		}
	public:
		LinkType nextNode(LinkType x) const
		{
			if (x->ch[1] != nil_)
				return extremeChild(x->ch[1], 0);
			while (x->p->ch[1] == x)
				x = x->p;
			return x->p;//x是最大节点时p为nil_
		}
		LinkType prevNode(LinkType x) const
		{
			if (x == nil_)//end()的前一个
				return maxChild();
			if (x->ch[0] != nil_)
				return extremeChild(x->ch[0], 1);
			while (x->p->ch[0] == x)
				x = x->p;
			return x->p;//x是最小节点时p为nil_
		}
		NodePair findNode(const KeyType &key,const bool doSplay = true) //从x开始搜
		{
			LinkType x = root_, p = nil_;
			while (x != nil_)
			{
				if (comp(x->key(), key))
					p = x, x = x->ch[1];
				else if (!comp(key, x->key()))
				{
					if (doSplay)
						splay(nil_, x);
					return { x,p };
				}
				else
					p = x, x = x->ch[0];
			}
			if (doSplay)
				splay(nil_, p);
			return { nullptr,p };
		}
		LinkType insert(const ValueType &value)
		{
			const KeyType &key = Traits::KeyOf(value);
			NodePair x = findNode(key, false);
			if (x.first)
			{
				splay(nil_, x.first);
				return x.first;
			}
			LinkType px = x.second;
			LinkType z = new Node(px, nil_, nil_, value);
			if (px == nil_) root_ = z;
			else px->ch[comp(px->key(), key)] = z;
			LinkType tmpx = px;
			while (tmpx != nil_)
				++tmpx->sz, tmpx = tmpx->p;
			splay(nil_, z);
			return z;
		}
		void erase(LinkType z) //Map里使用erase是配合find的，所以z一定是根
		{
			LinkType tmpz = z;
			while (tmpz != nil_)
				--tmpz->sz, tmpz = tmpz->p;
			LinkType y = z, x;
			if (z->ch[0] == nil_)
				x = z->ch[1], transplant(z, x);
			else if (z->ch[1] == nil_)
				x = z->ch[0], transplant(z, x);
			else
			{
				y = extremeChild(z->ch[1], 0); 
				x = y->ch[1];
				if (y->p != z)  
				{
					LinkType tmpy = y;
					while (tmpy != z)
					{
						--tmpy->sz;
						tmpy = tmpy->p;
					}
					transplant(y, x);
					y->ch[1] = z->ch[1]; //把y剥离出来，安到z的位置
					y->ch[1]->p = y;
				}
				transplant(z, y);
				y->ch[0] = z->ch[0], y->ch[0]->p = y;
				updSz(y);
			}
		}
		LinkType minChild() const { return extremeChild(root_, 0); }
		LinkType maxChild() const { return extremeChild(root_, 0); }
		int size() const { return root_->sz; }
		LinkType nil()const { return nil_; }
		Splay(KeyCompare comp) :comp(comp), nil_(new Node)
		{
			root_ = nil_;
			root_->p = root_->ch[0] = root_->ch[1] = nil_;
		}
		Splay(const Splay&rhs) = delete;//为了保证Splay的简洁(滑稽)，不做拷贝控制
		Splay& operator=(const Splay&rhs) = delete;
	};
}