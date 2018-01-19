#pragma once
#include <iostream>

namespace ds
{
	template<typename K, typename V,bool LinkWithList = false>
	class Map
	{
	public:
		typedef K key_type;
		typedef V mapped_type;
		typedef int size_type;
		typedef int difference_type;
		typedef std::pair<const K, V> value_type;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;
		typedef value_type &reference;
		typedef const value_type &const_reference;

		typedef std::function<mapped_type()> DefaultValue;
		struct Node;
		class const_iterator;
		class iterator
		{
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef K key_type;
			typedef V mapped_type;
			typedef int size_type;
			typedef int difference_type;
			typedef std::pair<const K, V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
		private:
			Node *self_;
			Map *container_;
		public:
			friend Map;
			friend const_iterator;
			reference operator*() const { return self_->value; }
			pointer operator->()const { return &(operator*()); }
			iterator& operator++()
			{
				self_ = container_->nextNode(self_);
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;
				++*this;
				return tmp;
			}
			iterator& operator--()
			{
				self_ = container_->prevNode(self_);
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
			iterator prev()const { return iterator(container_->prevNode(self_),container_); }
			iterator next()const { return iterator(container_->nextNode(self_),container_); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			iterator(Node *n,Map *m) :self_(n),container_(m) {}
		};
		class const_iterator
		{
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef K key_type;
			typedef V mapped_type;
			typedef int size_type;
			typedef int difference_type;
			typedef std::pair<const K, const V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
		private:
			Node *self_;
			Map *container_;
		public:
			friend Map;
			reference operator*() const { return self_->value; }
			pointer operator->()const { return &(operator*()); }
			const_iterator& operator++() { self_ = container_->nextNode(self_); return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++*this; return tmp; }
			const_iterator& operator--() { self_ = container_->prevNode(self_); return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; -*this; return tmp; }
			const_iterator prev()const { return const_iterator(container_->prevNode(self_), container_); }
			const_iterator next()const { return const_iterator(container_->nextNode(self_), container_); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			const_iterator(Node *n, Map *m) :self_(n), container_(m) {}
			const_iterator(iterator it) :self_(it.self_), container_(it.container_) {}
		};
	private:
		const static bool B = false;
		const static bool R = true;
		typedef typename List<Node*>::iterator ListIter;
		struct Node
		{
			value_type value;
			int size = 0;
			Node *left = nullptr;
			Node *right = nullptr;
			Node *p = nullptr;
			bool color = B;
			const K &key()const { return value.first; }
			V &val() { return value.second; }
			Node() = default;
			Node(Node* p, Node* l, Node* r, bool c, value_type && vp) :value(vp), p(p), left(l), right(r), color(c), size(1) {}
		};
		typedef std::pair<Node*, Node*> NodePair; // self_,parent
		Node* nil_;
		Node* root_;
		DefaultValue defaultValue_;
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
		Node* minChild(Node *x)
		{
			while (x->left != nil_)
				x = x->left;
			return x;
		}
		Node* maxChild(Node *x)
		{
			while (x->right != nil_)
				x = x->right;
			return x;
		}
		Node* nextNode(Node *x)
		{
			if (x->right != nil_)
				return minChild(x->right);
			while (x->p->right == x)
				x = x->p;
			return x->p;//x是最大节点时p为nil_
		}
		Node* prevNode(Node *x)
		{
			if (x->left != nil_)
				return maxChild(x->left);
			while (x->p->left == x)
				x = x->p;
			return x->p;//x是最小节点时p为nil_
		}
		NodePair findNode(const K& key)
		{
			Node *x = root_, *p = nil_;
			while (x != nil_)
			{
				if (x->key() < key)
					p = x, x = x->right;
				else if (!(key < x->key()))
					return { x,p };// p == x->p
				else p = x, x = x->left;
			}
			return { nullptr,p };
		}
		Node* insert(Node *p, const K& key, const V& val)
		{
			Node *z = new Node(p, nil_, nil_, R, std::make_pair(key, val));
			if (p == nil_)
				root_ = z;
			else
			{
				if (p->key() < key)
					p->right = z;
				else
					p->left = z;
			}
			while (p != nil_)
				++p->size, p = p->p;
			putFixUp(z);
			return z;
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
		void clear(Node *x)
		{
			if (!x || x == nil_)
				return;
			clear(x->left);
			clear(x->right);
			delete x;
		}
		void cpy(Node *dest, const Node *sour,const Node *sournil_)
		{
			if (sour->left != sournil_)
			{
				dest->left = new Node(dest, nil_, nil_, sour->left->color, value_type(sour->left->value));
				dest->left->size = sour->left->size;
				cpy(dest->left, sour->left, sournil_);
			}
			if (sour->right != sournil_)
			{
				dest->right = new Node(dest, nil_, nil_, sour->right->color, value_type(sour->right->value));
				dest->right->size = sour->right->size;
				cpy(dest->right, sour->right, sournil_);
			}
		}
	public:
		explicit Map(DefaultValue defaultValue_):defaultValue_(defaultValue_), nil_(new Node)
		{
			root_ = nil_;
			root_->p = root_->left = root_->right = nil_;
		}
		Map():nil_(new Node)
		{
			defaultValue_ = []()->mapped_type {return mapped_type(); };
			root_ = nil_;
			root_->p = root_->left = root_->right = nil_;
		}
		iterator find(const K& key)
		{
			if (Node *x = findNode(key).first)
				return iterator(x);
			return end();
		}
		V& operator[](const K& key)
		{
			NodePair pn = findNode(key);
			if (pn.first)
				return pn.first->val();
			return insert(pn.second, key, defaultValue_())->val();
		}
		void erase(const K& key)
		{
			if (Node *x = findNode(key).first)
				erase(x);
		}
		void erase(iterator where)
		{
			if (where.self_!=nil_)
				erase(where.self_);
		}
		iterator insert(const key_type& key, const mapped_type& val)
		{
			//与stl统一，重复insert并不会更新节点的值
			NodePair np = findNode(key);
			if (np.first)
				return iterator(np.first, this);
			return iterator(insert(np.second, key, val),this);
		}
		iterator insert(const value_type &kvpair)
		{
			return insert(kvpair.first, kvpair.second);
		}
		
		int lower(const K &rend, bool afterEqual) const
		{
			Node *x = root_;
			int cnt = 0;
			while (x != nil_)
			{
				if (x->key < rend || (afterEqual && !(rend < x->key) && !(x->key < rend)))
				{
					cnt += x->left->size + 1;
					x = x->right;
				}
				else
					x = x->left;
			}
			return cnt;
		}

		int between(int lend, int rend) const { return lower(rend, true) - lower(lend, false); }

		iterator kth(int k)
		{
			Node *x = root_;
			while (x != nil_)
			{
				if (x->left->size + 1 == k)
					return iterator(x, this);
				if (x->left->size + 1 < k)
				{
					k -= x->left->size + 1;
					x = x->right;
				}
				else
					x = x->left;
			}
			return end();
		}
		void clear() { clear(root_); }
		iterator begin() { return iterator(minChild(root_), this); }//root_为nil_时minChild返回nil_
		iterator end() { return iterator(nil_,this); }
		const_iterator begin() const { return const_iterator(minChild(root_), this); }//root_为nil_时minChild返回nil_
		const_iterator end() const { return const_iterator(nil_, this); }
		int size()const { return root_->size; }
		void swap(Map &rhs) noexcept
		{
			std::swap(root_, rhs.root_);
			std::swap(nil_, rhs.nil_);
			std::swap(defaultValue_, rhs.defaultValue_);
		}
		Map(const Map&rhs):defaultValue_(rhs.defaultValue_)
		{
			nil_ = new Node;
			if (rhs.root_==rhs.nil_)
			{
				root_ = nil_;
				root_->p = root_->left = root_->right = nil_;
			}
			else
			{
				root_ = new Node(nil_, nil_, nil_, rhs.root_->color, value_type(rhs.root_->value));
				root_->size = rhs.root_->size;
				cpy(root_, rhs.root_, rhs.nil_);
			}
		}
		Map& operator=(Map rhs)
		{
			swap(rhs);
			return *this;
		}
		Map(Map &&rhs) noexcept
		{
			if (this!=&rhs)
			{
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				rhs.nil_ = rhs.root_ = nullptr;
				defaultValue_ = rhs.defaultValue_;
			}
		}
		Map& operator=(Map &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~Map();
				nil_ = rhs.nil_;
				root_ = rhs.root_;
				rhs.nil_ = rhs.root_ = nullptr;
				defaultValue_ = rhs.defaultValue_;
			}
			return *this;
		}
		~Map()
		{
			clear(root_);
			delete nil_;
		}
	};
}

