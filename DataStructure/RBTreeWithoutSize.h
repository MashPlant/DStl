#pragma once
#include <functional>
#include <utility>
namespace ds
{
	template <typename Traits>
	class RBTreeWithoutSize
	{
	private:
		typedef typename Traits::KeyType KeyType;
		typedef typename Traits::ValueType ValueType;
		typedef typename Traits::KeyCompare KeyCompare;
		const static bool B = false;
		const static bool R = true;
		struct Node
		{
			ValueType value;
			Node *left = nullptr;
			Node *right = nullptr;
			Node *p = nullptr;
			bool color = B;
			ValueType &operator*() { return value; }
			ValueType *operator->() { return &(operator*()); }
			const KeyType& key() { return Traits::KeyOf(value); }
			Node() = default;
			Node(Node* p, Node* l, Node* r, bool c, const ValueType &v) :value(v), left(l), right(r), p(p), color(c) { }
		};
	public:
		typedef Node* LinkType;
	private:
		typedef std::pair<Node*, Node*> NodePair;
		KeyCompare comp;
		Node* nil_;
		Node* root_;
		int size_ = 0;
		void rotataRight(Node *x)
		{
			Node* y = x->left;
			x->left = y->right;
			if (y->right != nil_)
				y->right->p = x;
			y->p = x->p;
			if (x->p == nil_)
				root_ = y;
			else
			{
				if (x == x->p->left)
					x->p->left = y;
				else x->p->right = y;
			}
			y->right = x;
			x->p = y;
		}
		void rotataLeft(Node *x)
		{
			Node* y = x->right;
			x->right = y->left;
			if (y->left != nil_)
				y->left->p = x;
			y->p = x->p;
			if (x->p == nil_)
				root_ = y;
			else
			{
				if (x == x->p->right)
					x->p->right = y;
				else x->p->left = y;
			}
			y->left = x;
			x->p = y;
		}
		void putFixUp(Node *z)
		{
			while (z->p->color == R)
			{
				if (z->p == z->p->p->left)
				{
					Node *y = z->p->p->right;
					if (y->color == R)
					{
						y->color = z->p->color = B;
						z->p->p->color = R;
						z = z->p->p;
					}
					else
					{
						if (z == z->p->right)
						{
							z = z->p;
							rotataLeft(z);
						}
						z->p->color = B;
						z->p->p->color = R;
						rotataRight(z->p->p);
					}
				}
				else
				{
					Node *y = z->p->p->left;
					if (y->color == R)
					{
						y->color = z->p->color = B;
						z->p->p->color = R;
						z = z->p->p;
					}
					else
					{
						if (z == z->p->left)
						{
							z = z->p;
							rotataRight(z);
						}
						z->p->color = B;
						z->p->p->color = R;
						rotataLeft(z->p->p);
					}
				}
			}
			root_->color = B;
		}
		void delFixUp(Node *x)
		{
			while (x != root_&&x->color == B)
			{
				if (x == x->p->left)
				{
					Node *w = x->p->right;
					if (w->color == R)//case 1,will enter case 234 later 
					{
						x->p->color = R;
						w->color = B;
						rotataLeft(x->p);
						w = x->p->right;
					}
					if (w->left->color == B&&w->right->color == B)//case 2,will not enter case 34
					{
						w->color = R;
						x = x->p;
					}
					else
					{
						if (w->right->color == B)//case 3,will enter case 4
						{
							w->color = R;
							w->left->color = B;
							rotataRight(w);
							w = x->p->right;
						}
						//case 4,after which the "while" will finish
						w->color = x->p->color;//swap the color of w and x->p(w->p)
						x->p->color = B;
						w->right->color = B;
						rotataLeft(w->p);
						x = root_;
					}
				}
				else
				{
					Node *w = x->p->left;
					if (w->color == R)//case 1,will enter case 234 later 
					{
						x->p->color = R;
						w->color = B;
						rotataRight(x->p);
						w = x->p->left;
					}
					if (w->right->color == B&&w->left->color == B)//case 2,will not enter case 34
					{
						w->color = R;
						x = x->p;
					}
					else
					{
						if (w->left->color == B)//case 3,will enter case 4
						{
							w->color = R;
							w->right->color = B;
							rotataLeft(w);
							w = x->p->left;
						}
						//case 4,after which the "while" will finish
						w->color = x->p->color;//swap the color of w and x->p(w->p)
						x->p->color = B;
						w->left->color = B;
						rotataRight(w->p);
						x = root_;
					}
				}
			}
			x->color = B;

		}
		void transplant(Node *to, Node *from)
		{
			from->p = to->p;
			if (to->p == nil_)
				root_ = from;
			else
			{
				if (to == to->p->left)
					to->p->left = from;
				else to->p->right = from;
			}
		}
		void clear(Node *x)
		{
			if (!x || x == nil_)
				return;
			clear(x->left);
			clear(x->right);
			delete x;
		}
		void cpy(Node *dest, const Node *sour, const Node *sournil_)
		{
			if (sour->left != sournil_)
			{
				dest->left = new Node(dest, nil_, nil_, sour->left->color, ValueType(sour->left->value));
				cpy(dest->left, sour->left, sournil_);
			}
			if (sour->right != sournil_)
			{
				dest->right = new Node(dest, nil_, nil_, sour->right->color, ValueType(sour->right->value));
				cpy(dest->right, sour->right, sournil_);
			}
		}
		Node* minChild(Node *x) const
		{
			while (x->left != nil_)
				x = x->left;
			return x;
		}
		Node* maxChild(Node *x) const
		{
			while (x->right != nil_)
				x = x->right;
			return x;
		}
		Node* insert(Node *p, const ValueType& value)
		{
			++size_;
			Node *z = new Node(p, nil_, nil_, R, value);
			if (p == nil_)
				root_ = z;
			else
			{
				if (comp(p->key(), Traits::KeyOf(value)))
					p->right = z;
				else
					p->left = z;
			}
			putFixUp(z);
			return z;
		}
	public:
		Node* nextNode(Node *x) const
		{
			if (x->right != nil_)
				return minChild(x->right);
			while (x->p->right == x)
				x = x->p;
			return x->p;//x是最大节点时p为nil_
		}
		Node* prevNode(Node *x) const
		{
			if (x == nil_)//end()的前一个
				return maxChild(root_);
			if (x->left != nil_)
				return maxChild(x->left);
			while (x->p->left == x)
				x = x->p;
			return x->p;//x是最小节点时p为nil_
		}
		NodePair findNode(const KeyType& key)
		{
			Node *x = root_, *p = nil_;
			while (x != nil_)
			{
				if (comp(x->key(), key))
					p = x, x = x->right;
				else if (!comp(key, x->key()))
					return { x,p };
				else p = x, x = x->left;
			}
			return { nullptr,p };
		}
		Node* insert(const ValueType &value)
		{
			NodePair np = findNode(Traits::KeyOf(value));
			if (np.first)
				return np.first;
			return insert(np.second, value);
		}
		void erase(Node *z)
		{
			--size_;
			Node *y = z, *x;
			bool oldColor = y->color;
			if (z->left == nil_)
			{
				x = z->right;
				transplant(z, z->right);
			}
			else if (z->right == nil_)
			{
				x = z->left;
				transplant(z, z->left);
			}
			else
			{
				y = minChild(z->right);
				oldColor = y->color;
				x = y->right;
				if (y->p == z)
					x->p = y;
				else
				{
					transplant(y, y->right);
					y->right = z->right;
					y->right->p = y;
				}
				transplant(z, y);
				y->left = z->left, y->left->p = y;
				y->color = z->color;
			}
			delete z;
			if (oldColor == B)
				delFixUp(x);
		}
		Node * nil() { return nil_; }
		Node * minChild() const { return minChild(root_); }
		Node * maxChild() const { return maxChild(root_); }
		int size()const { return size_; }
		void swap(RBTreeWithoutSize &rhs) noexcept
		{
			std::swap(root_, rhs.root_);
			std::swap(nil_, rhs.nil_);
			std::swap(size_, rhs.size_);
		}
		RBTreeWithoutSize(KeyCompare comp = KeyCompare()) :comp(comp), nil_(new Node)
		{
			root_ = nil_;
			root_->p = root_->left = root_->right = nil_;
		}
		RBTreeWithoutSize(const RBTreeWithoutSize&rhs)
		{
			nil_ = new Node;
			size_ = rhs.size_;
			if (rhs.root_ == rhs.nil_)
			{
				root_ = nil_;
				root_->p = root_->left = root_->right = nil_;
			}
			else
			{
				root_ = new Node(nil_, nil_, nil_, rhs.root_->color, ValueType(rhs.root_->value));
				cpy(root_, rhs.root_, rhs.nil_);
			}
		}
		RBTreeWithoutSize& operator=(const RBTreeWithoutSize &rhs)
		{
			if (this != &rhs)
			{
				RBTreeWithoutSize tmp(rhs);
				swap(tmp);
			}
			return *this;
		}
		RBTreeWithoutSize(RBTreeWithoutSize &&rhs) noexcept
		{
			if (this != &rhs)
			{
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				size_ = rhs.size_;
				rhs.nil_ = rhs.root_ = nullptr;
			}
		}
		RBTreeWithoutSize& operator=(RBTreeWithoutSize &&rhs) noexcept
		{
			if (this != &rhs)
			{
				RBTreeWithoutSize tmp(std::move(rhs));
				swap(tmp);
			}
			return *this;
		}
		~RBTreeWithoutSize()
		{
			clear(root_);
			delete nil_;
		}
	};
}