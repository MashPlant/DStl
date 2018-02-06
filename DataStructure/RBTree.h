#pragma once
#include <functional>
#include <utility>
namespace ds
{
	//可能有点过度设计吧
	//不需要与stl交互，所以就全部驼峰式命名
	//不是给人继承的(MS实现)，而是给人用的(SGI STL实现)
	template <typename Traits>
	class RBTree
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
			int size = 0;
			Node *left = nullptr;
			Node *right = nullptr;
			Node *p = nullptr;
			bool color = B;
			ValueType &operator*() { return value; }
			ValueType *operator->() { return &(operator*()); }
			const KeyType& key() { return Traits::KeyOf(value); }
			Node() = default;
			Node(Node* p, Node* l, Node* r, bool c, const ValueType &v) :value(v), p(p), left(l), right(r), color(c), size(1) {}
		};
	public:
		typedef Node* LinkType;
	private:
		typedef std::pair<Node*, Node*> NodePair;
		KeyCompare comp;
		Node* nil_;
		Node* root_;
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
			y->size = x->size;
			x->size = x->left->size + x->right->size + 1;
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
			y->size = x->size;
			x->size = x->left->size + x->right->size + 1;
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
				dest->left->size = sour->left->size;
				cpy(dest->left, sour->left, sournil_);
			}
			if (sour->right != sournil_)
			{
				dest->right = new Node(dest, nil_, nil_, sour->right->color, ValueType(sour->right->value));
				dest->right->size = sour->right->size;
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
			while (p != nil_)
				++p->size, p = p->p;
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
		int lower(const KeyType &rend, bool afterEqual) const
		{
			Node *x = root_;
			int cnt = 0;
			while (x != nil_)
			{
				if (comp(x->key() , rend) || (afterEqual && !comp(rend , x->key())))
				{
					cnt += x->left->size + 1;
					x = x->right;
				}
				else
					x = x->left;
			}
			return cnt;
		}
		Node * kth(int k) //k>size() or k<1时自动返回end()
		{
			Node *x = root_;
			while (x != nil_)
			{
				if (x->left->size + 1 == k)
					return x;
				if (x->left->size + 1 < k)
				{
					k -= x->left->size + 1;
					x = x->right;
				}
				else
					x = x->left;
			}
			return nil_;
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
			Node *tmp = z;
			while (tmp != nil_)
				--tmp->size, tmp = tmp->p;
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
					Node *tempy = y;
					while (tempy != z)
					{
						--tempy->size;
						tempy = tempy->p;
					}
					transplant(y, y->right);
					y->right = z->right;
					y->right->p = y;
				}
				transplant(z, y);
				y->left = z->left, y->left->p = y;
				y->color = z->color;
				y->size = y->left->size + y->right->size + 1;
			}
			delete z;
			if (oldColor == B)
				delFixUp(x);
		}
		Node * nil() { return nil_; }
		Node * minChild() const { return minChild(root_); }
		Node * maxChild() const { return maxChild(root_); }
		int size()const { return root_->size; }
		void swap(RBTree &rhs) noexcept
		{
			std::swap(root_, rhs.root_);
			std::swap(nil_, rhs.nil_);
		}
		RBTree(KeyCompare comp = KeyCompare()):comp(comp),nil_(new Node)
		{
			root_ = nil_;
			root_->p = root_->left = root_->right = nil_;
		}
		RBTree(const RBTree&rhs) 
		{
			nil_ = new Node;
			if (rhs.root_ == rhs.nil_)
			{
				root_ = nil_;
				root_->p = root_->left = root_->right = nil_;
			}
			else
			{
				root_ = new Node(nil_, nil_, nil_, rhs.root_->color, ValueType(rhs.root_->value));
				root_->size = rhs.root_->size;
				cpy(root_, rhs.root_, rhs.nil_);
			}
		}
		RBTree& operator=(const RBTree &rhs)
		{
			if (this != &rhs)
			{
				RBTree tmp(rhs);
				swap(tmp);
			}
			return *this;
		}
		RBTree(RBTree &&rhs) noexcept
		{
			if (this != &rhs)
			{
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				rhs.nil_ = rhs.root_ = nullptr;
			}
		}
		RBTree& operator=(RBTree &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~RBTree();
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				rhs.nil_ = rhs.root_ = nullptr;
			}
			return *this;
		}
		~RBTree() 
		{
			clear(root_);
			delete nil_;
		}
	};
}