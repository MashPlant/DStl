#pragma once
#include <utility>
#include "TreeUtil.h"
namespace ds
{
	template<typename Traits>
	class RBTreeArray
	{
	private:
		typedef typename Traits::KeyType KeyType;
		typedef typename Traits::ValueType ValueType;
		typedef typename Traits::KeyCompare KeyCompare;
		const static bool B = false;
		const static bool R = true;
		const static int nil_ = 0;
	public:
		struct Node //不给用户创建Node的权力,只接受void*作为存放Node的数组
		{
			int ch[2] = { nil_,nil_ }, p = nil_, sz = 0;
			ValueType value;
			bool color = B;
			Node() = default;
			Node(int p, const ValueType &value, bool color) :p(p), sz(1), value(value), color(color) {}
			const KeyType &key() const { return Traits::KeyOf(value); }
		};
		struct LinkType //类似Node*
		{
			int where;
			Node *tree = nullptr;
			bool operator==(const LinkType&rhs) const { return where == rhs.where&&tree == rhs.tree; }
			bool operator!=(const LinkType&rhs) const { return where != rhs.where||tree != rhs.tree;}
			bool operator==(int where) const { return this->where == where; }
			bool operator!=(int where) const { return this->where != where;}
			LinkType &operator=(int where)
			{
				this->where = where;
				return *this;
			}
			LinkType(int where, Node *tree) :where(where), tree(tree) {}
			Node *operator->() const { return tree + where; }
			operator int() const { return where; }
		};
		typedef std::pair<LinkType, LinkType> NodePair;
	private:
		Node *tree_;
		KeyCompare comp_;
		int root_ = nil_;
		int treeSize_ = 0;
		void rotate(int x, bool right)  //是以x为基旋转，不是把x转上去
		{
			int y = tree_[x].ch[!right];
			tree_[x].ch[!right] = tree_[y].ch[right];
			if (tree_[y].ch[right] != nil_)
				tree_[tree_[y].ch[right]].p = x;
			tree_[y].p = tree_[x].p;
			if (tree_[x].p == nil_)
				root_ = y;
			else
				tree_[tree_[x].p].ch[x == tree_[tree_[x].p].ch[1]] = y;
			tree_[y].ch[right] = x;
			tree_[x].p = y;
			tree_[y].sz = tree_[x].sz;
			tree_[x].sz = tree_[tree_[x].ch[0]].sz + tree_[tree_[x].ch[1]].sz + 1;
		}
		void transplant(int to, int from)
		{
			tree_[from].p = tree_[to].p;
			if (tree_[to].p == nil_)
				root_ = from;
			else
				tree_[tree_[to].p].ch[to == tree_[tree_[to].p].ch[1]] = from;
		}
		void putFixUp(int z)
		{
			while (tree_[tree_[z].p].color == R)
			{
				int fa = tree_[z].p, grand = tree_[fa].p;
				bool isLeft = fa == tree_[grand].ch[0];
				int y = tree_[grand].ch[isLeft];
				if (tree_[y].color == R)
				{
					tree_[y].color = tree_[fa].color = B;
					tree_[grand].color = R;
					z = grand;
				}
				else
				{
					if (z == tree_[fa].ch[isLeft])
					{
						z = fa;
						rotate(z, !isLeft);
					}
					tree_[tree_[z].p].color = B;
					tree_[tree_[tree_[z].p].p].color = R;
					rotate(grand, isLeft);
				}
			}
			tree_[root_].color = B;
		}
		void delFixUp(int x)
		{
			while (x != root_ && tree_[x].color == B)
			{
				int fa = tree_[x].p;
				bool isLeft = x == tree_[tree_[x].p].ch[0];
				int w = tree_[fa].ch[isLeft];
				if (tree_[w].color == R) //case 1,will enter case 234 later
				{
					tree_[fa].color = R;
					tree_[w].color = B;
					rotate(fa, !isLeft);
					w = tree_[fa].ch[isLeft];
				}
				if (tree_[tree_[w].ch[0]].color == B && tree_[tree_[w].ch[1]].color == B) //case 2,will not enter case 34
				{
					tree_[w].color = R;
					x = tree_[x].p;
				}
				else
				{
					if (tree_[tree_[w].ch[isLeft]].color == B) //case 3,will enter case 4
					{
						tree_[w].color = R;
						tree_[tree_[w].ch[!isLeft]].color = B;
						rotate(w, isLeft);
						w = tree_[fa].ch[isLeft];
					}
					//case 4,after which the "while" will finish
					tree_[w].color = tree_[fa].color; //swap the color of w and x->p(w->p)
					tree_[fa].color = B;
					tree_[tree_[w].ch[isLeft]].color = B;
					rotate(tree_[w].p, !isLeft);
					x = root_;
				}
			}
			tree_[x].color = B;
		}
	public:
		RBTreeArray(void *arr, KeyCompare comp = KeyCompare()) :comp_(comp)  //很丑陋的解决方法
		{
			tree_ = reinterpret_cast<Node*>(arr);
			new(tree_) Node(); //构造nil_节点
		}
		LinkType nextNode(LinkType x)const { return TreeUtil::nextNode(x, nil()); }
		LinkType prevNode(LinkType x)const { return TreeUtil::prevNode(x, nil(), root()); }
		LinkType kth(const int k)const { return TreeUtil::kth(root(), nil(), k); }
		int lower(const KeyType &key, const bool afterEqual) const { return TreeUtil::lower(key, root_, nil_, afterEqual, comp_); }
		NodePair findNode(const KeyType &key) //从x开始搜
		{
			int x = root_, p = nil_;
			while (x != nil_)
			{
				if (comp_(tree_[x].key(), key))
					p = x, x = tree_[x].ch[1];
				else if (!comp_(key, tree_[x].key()))
					return { LinkType(x,tree_) ,LinkType(p,tree_) };
				else
					p = x, x = tree_[x].ch[0];
			}
			return { nil() ,LinkType(p,tree_) };
		}
		LinkType insert(LinkType x, const ValueType &value)
		{
			int z = ++treeSize_;
			new(tree_ + z) Node({ int(x),value,R });
			if (x == nil_)
				root_ = z;
			else
				x->ch[comp_(x->key(), Traits::KeyOf(value))] = z;
			while (x != nil_)
				++x->sz, x = x->p;
			putFixUp(z);
			return LinkType(z, tree_);
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
			LinkType tmp = z;
			while (tmp != nil_)
				--tmp->sz, tmp = tmp->p;
			int y = z, x;
			bool oldColor = tree_[y].color;
			if (tree_[z].ch[0] == nil_)
			{
				x = tree_[z].ch[1];
				transplant(z, tree_[z].ch[1]);
			}
			else if (tree_[z].ch[1] == nil_)
			{
				x = tree_[z].ch[0];
				transplant(z, tree_[z].ch[0]);
			}
			else
			{
				y = TreeUtil::extremeChild(LinkType(tree_[z].ch[1],tree_), nil(), 0);
				oldColor = tree_[y].color;
				x = tree_[y].ch[1];
				if (tree_[y].p == z)
					tree_[x].p = y;
				else
				{
					int tempy = y;
					while (tempy != z)
					{
						--tree_[tempy].sz;
						tempy = tree_[tempy].p;
					}
					transplant(y, tree_[y].ch[1]);
					tree_[y].ch[1] = tree_[z].ch[1];
					tree_[tree_[y].ch[1]].p = y;
				}
				transplant(z, y);
				tree_[y].ch[0] = tree_[z].ch[0];
				tree_[tree_[y].ch[0]].p = y;
				tree_[y].color = tree_[z].color;
				tree_[y].sz = tree_[tree_[y].ch[0]].sz + tree_[tree_[y].ch[1]].sz + 1;
			}
			if (oldColor == B)
				delFixUp(x);
		}
		LinkType minChild() const { return TreeUtil::extremeChild(root(), nil(), 0); }
		LinkType maxChild() const { return TreeUtil::extremeChild(root(), nil(), 1); }
		int size() const { return tree_[root_].sz; }
		LinkType nil() const { return LinkType( nil_,tree_ ); }
		LinkType root() const { return LinkType( root_,tree_ ); }
		RBTreeArray(const RBTreeArray&) = delete;
		RBTreeArray& operator=(const RBTreeArray&) = delete;
	};
}
