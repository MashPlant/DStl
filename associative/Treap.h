#pragma once
#include <utility>
#include "Util.h"
#include <queue>
#include <iostream>
#include "TreeUtil.h"
namespace ds
{
	template <typename Traits>
	class Treap
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
			uint32_t priority = 0u;
			const KeyType &key() const { return Traits::KeyOf(value); }
			ValueType &operator*() { return value; }
			ValueType *operator->() { return &(operator*()); }
			Node() = default;
			Node(LinkType p, LinkType l, LinkType r, const ValueType &value) :p(p), value(value), sz(1)
			{
				ch[0] = l, ch[1] = r; 
				priority = rawRani();
			}
		};
	private:
		LinkType nil_, root_; //大根堆
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
		void transplant(LinkType dest, LinkType sour)//并不处理dest的子节点
		{
			LinkType dp = dest->p;
			sour->p = dp;
			if (dp == nil_)
				root_ = sour;
			else
				dp->ch[dest == dp->ch[1]] = sour;
		}
	public:
		LinkType nextNode(Node *x)const { return TreeUtil::nextNode(x, nil_); }
		LinkType prevNode(Node *x)const { return TreeUtil::prevNode(x, nil_, root_); }
		LinkType kth(const int k)const { return TreeUtil::kth(root_, nil_, k); }
		int lower(const KeyType &key, const bool afterEqual) const { return TreeUtil::lower(key, root_, nil_, afterEqual, comp); }
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
		LinkType insert(LinkType x, const ValueType &value)
		{
			LinkType z = new Node(x, nil_, nil_, value);
			if (x == nil_) root_ = z;
			else x->ch[comp(x->key(), Traits::KeyOf(value))] = z;
			LinkType tmpx = x;
			while (tmpx != nil_)
				++tmpx->sz, tmpx = tmpx->p;
			while (z != root_ && z->priority > z->p->priority)//一旦z的priority没有z->p高，可以立即停止更新
				rotate(z);
			return z;
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
			//把z转到最底层，从而直接删除
			z->priority = 0u;
			while (z->ch[0] != nil_&&z->ch[1] != nil_)
			{
				const int argmax = z->ch[0]->priority < z->ch[1]->priority;
				rotate(z->ch[argmax]);
			}
			LinkType tmpz = z;
			while (tmpz != nil_)
				--tmpz->sz, tmpz = tmpz->p;
			const int argnil = z->ch[1] == nil_;
			transplant(z, z->ch[!argnil]);
			delete z;
		}
		void check(Node *x)
		{
			if (x == nil_)
				return;
			//if (x->ch[0]->priority > x->priority || x->ch[1]->priority>x->priority)
			if (x->ch[0]->sz+x->ch[1]->sz+1!=x->sz)
				std::cout << "fuck it\n";
			check(x->ch[0]);
			check(x->ch[1]);
		}
		void bfs()
		{
			std::queue<LinkType> q;
			q.push(root_);
			while (!q.empty())
			{
				std::queue<LinkType> layer;
				while (!q.empty())
				{
					LinkType x = q.front();
					q.pop();
					printf("(%d,%u) ", (*x)->first, x->priority);
					for (int i = 0; i<2; ++i)
						if (x->ch[i] != nil_)
							layer.push(x->ch[i]);
				}
				std::cout << std::endl;
				q = layer;
			}
			std::cout << std::endl;
		}
		LinkType minChild() const { return TreeUtil::extremeChild(root_, nil_, 0); }
		LinkType maxChild() const { return TreeUtil::extremeChild(root_, nil_, 1); }
		int size() const { return root_->sz; }
		LinkType nil() const { return nil_; }
		Treap(KeyCompare comp = KeyCompare()) :comp(comp), nil_(new Node)
		{
			root_ = nil_;
			root_->p = root_->ch[0] = root_->ch[1] = nil_;
		}
		Treap(const Treap &) = delete;
		Treap& operator=(const Treap &) = delete;
	};
}