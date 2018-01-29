#pragma once
#include <functional>
#include <queue>
#include <utility>
#include <iostream>
namespace ds
{
	template <typename Traits>
	class AVL
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
			int sz = 0, h = 0;
			const KeyType &key() const { return Traits::KeyOf(value); }
			ValueType &operator*() { return value; }
			ValueType *operator->() { return &(operator*()); }
			Node() = default;
			Node(LinkType p, LinkType l, LinkType r, const ValueType &value) :p(p), value(value), sz(1), h(1)
			{
				ch[0] = l, ch[1] = r;   //默认初始化时size和h置为0
			}
		};
	private:
		LinkType nil_, root_;
		static void updH(LinkType x) { x->h = max(x->ch[0]->h, x->ch[1]->h) + 1; }
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
			updSz(p), updH(p), updSz(x), updH(x);
		}
		void transplant(LinkType dest, LinkType sour)//并不处理dest的子节点
		{
			LinkType dp = dest->p;
			sour->p = dp;
			if (dp == nil_)
				root_ = sour;
			else
				dp->ch[dest == dp->ch[1]] = sour;
		}
		static bool isBalance(LinkType x)
		{
			const int delta = x->ch[1]->h - x->ch[0]->h;
			return delta <= 1 && delta >= -1;
		}
		static bool checkAndUpd(LinkType x)
		{
			const int h = max(x->ch[0]->h, x->ch[1]->h) + 1;
			if (x->h == h)
				return true;
			x->h = h;
			return false;
		}
		LinkType extremeChild(LinkType x, const int what) const
		{
			while (x->ch[what] != nil_)
				x = x->ch[what];
			return x;
		}
		void clear(LinkType x)
		{
			if (!x || x == nil_)
				return;
			clear(x->ch[0]);
			clear(x->ch[1]);
			delete x;
		}
		void cpy(LinkType dest, const LinkType sour, const LinkType sournil_)
		{
			if (sour->ch[0] != sournil_)
			{
				dest->ch[0] = new Node(dest, nil_, nil_, ValueType(sour->ch[0]->value));
				dest->ch[0]->size = sour->ch[0]->size;
				cpy(dest->ch[0], sour->ch[0], sournil_);
			}
			if (sour->ch[1] != sournil_)
			{
				dest->ch[1] = new Node(dest, nil_, nil_, ValueType(sour->ch[1]->value));
				dest->ch[1]->size = sour->ch[1]->size;
				cpy(dest->ch[1], sour->ch[1], sournil_);
			}
		}
		LinkType insert(LinkType x, const ValueType &value)
		{
			LinkType z = new Node(x, nil_, nil_, value);
			if (x == nil_) x = root_ = z;
			else x->ch[comp(x->key(), Traits::KeyOf(value))] = z;
			LinkType tmpx = x;
			while (tmpx != nil_)
				++tmpx->sz, tmpx = tmpx->p;
			while (x != root_)
			{
				if (checkAndUpd(x))
					break;
				if (!isBalance(x->p))
				{
					const bool rz = z == x->ch[1], rx = x == x->p->ch[1];
					if (rz^rx)
						rotate(z), rotate(z);
					else
						rotate(x);
					break;//插入至多修复一次,但是不知道在那里修复
				}
				z = x, x = x->p;
			}
			return z;
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

		NodePair findNode(const KeyType &key, LinkType x = nullptr) //从x开始搜
		{
			if (!x) x = root_;
			LinkType p = nil_;
			while (x != nil_)
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
		int lower(const KeyType &rend, bool afterEqual) const
		{
			LinkType x = root_;
			int cnt = 0;
			while (x != nil_)
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
		LinkType kth(int k) //k>size() or k<1时自动返回end()
		{
			LinkType x = root_;
			while (x != nil_)
			{
				if (x->ch[0]->sz + 1 == k)
					return x;
				if (x->ch[0]->sz + 1 < k)
				{
					k -= x->ch[0]->sz + 1;
					x = x->ch[1];
				}
				else
					x = x->ch[0];
			}
			return nil_;
		}
		LinkType insert(const ValueType &value)
		{
			NodePair np = findNode(Traits::KeyOf(value));
			if (np.first)
				return np.first;
			return insert(np.second, value);
		}
		void erase(LinkType z)
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
				const int argmax = z->ch[0]->h > z->ch[1]->h ? 0 : 1;
				y = extremeChild(z->ch[argmax], !argmax);  //前驱 or 后继
				x = y->ch[argmax];
				if (y->p != z)  //y->p==z时，x是一起被带过来的，!=时，y被剥离出来，x留在原地
				{
					LinkType tmpy = y;
					while (tmpy != z)
					{
						--tmpy->sz;
						tmpy = tmpy->p;
					}
					transplant(y, x);
					y->ch[argmax] = z->ch[argmax]; //把y剥离出来，安到z的位置
					y->ch[argmax]->p = y;
				}
				transplant(z, y);
				y->ch[!argmax] = z->ch[!argmax], y->ch[!argmax]->p = y;
				updH(y), updSz(y);
			}
			if (z == root_)
				root_ = nil_;
			delete z;
			//现在，x->p是唯一可能破坏高度平衡的节点
			while (x != root_)
			{
				if (x != nil_)
					updH(x);
				if (!isBalance(x->p))
				{
					bool xSide = x == x->p->ch[1], sameSide = true;
					x = x->p->ch[xSide ^= 1];//x的另一边
											 //这是经过缜密分析得到的结果
											 //记x的h=H，可以证明，当且仅当与x同侧的孩子的h=H-2，异侧的孩子的h=H-1时，不能选同侧的孩子
											 //而且可以选异侧的孩子
					if (x->ch[xSide]->h == x->h - 2 && x->ch[!xSide]->h == x->h - 1)
						z = x->ch[!xSide], sameSide = false;
					else
						z = x->ch[xSide];
					if (!sameSide)
						rotate(z), rotate(z), x = z; //此时z==x->p
					else
						rotate(x);//此时不用更新
				}
				else
					x = x->p;
			}
			updH(root_);
		}
		int height() const { return root_->h; }
		LinkType minChild() const { return extremeChild(root_, 0); }
		LinkType maxChild() const { return extremeChild(root_, 0); }
		int size() const { return root_->sz; }
		LinkType nil() const { return nil_; }
		void clear() { clear(root_); }
		void swap(AVL &rhs) noexcept
		{
			std::swap(root_, rhs.root_);
			std::swap(nil_, rhs.nil_);
		}
		AVL(KeyCompare comp) :comp(comp), nil_(new Node)
		{
			root_ = nil_;
			root_->p = root_->ch[0] = root_->ch[1] = nil_;
		}
		AVL(const AVL&rhs)
		{
			nil_ = new Node;
			if (rhs.root_ == rhs.nil_)
			{
				root_ = nil_;
				root_->p = root_->ch[0] = root_->ch[1] = nil_;
			}
			else
			{
				root_ = new Node(nil_, nil_, nil_, ValueType(rhs.root_->value));
				root_->sz = rhs.root_->sz;
				cpy(root_, rhs.root_, rhs.nil_);
			}
		}
		AVL& operator=(const AVL &rhs)
		{
			if (this != &rhs)
			{
				AVL tmp(rhs);
				swap(tmp);
			}
			return *this;
		}
		AVL(AVL &&rhs) noexcept
		{
			if (this != &rhs)
			{
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				rhs.nil_ = rhs.root_ = nullptr;
			}
		}
		AVL& operator=(AVL &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~AVL();
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				rhs.nil_ = rhs.root_ = nullptr;
			}
			return *this;
		}
		~AVL()
		{
			clear(root_);
			delete nil_;
		}
	};
}
