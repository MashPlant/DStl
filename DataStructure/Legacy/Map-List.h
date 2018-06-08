#pragma once
#include <iostream>
#include "List.h"
#include "Map.h"
namespace ds
{
	//链表辅助红黑树，最坏O(1)前驱/后继
	template<typename K, typename V>
	class Map<K, V, true>
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
			Node *self;
		public:
			reference operator*() const { return self->value; }
			pointer operator->()const { return &(operator*()); }
			iterator& operator++()
			{
				self = self->next(); //前驱后继与Map::Node的left和right无任何关系，完全由链表决定
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
				self = self->prev();
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
			iterator prev()const { return iterator(self->prev()); }
			iterator next()const { return iterator(self->next()); }
			bool operator==(iterator rhs) const { return self == rhs.self; }
			bool operator!=(iterator rhs) const { return !(*this == rhs); }
			explicit iterator(Node *n) :self(n) {}
		};
	private:
		const static bool B = false;
		const static bool R = true;
		typedef typename List<Node*>::iterator ListIter;
		struct Node
		{
			value_type value;
			int sz = 0;
			Node *left = nullptr;
			Node *right = nullptr;
			Node *p = nullptr;
			bool color = B;
			ListIter self;

			Node *prev()const { return *(self.prev()); }
			Node *next()const { return *(self.next()); }
			const K &key()const { return value.first; }
			V &val() { return value.second; }
			Node() = default;
			Node(Node* p, Node* l, Node* r, bool c, value_type && vp) :value(vp), p(p), left(l), right(r), color(c), sz(1) {}
		};
		typedef std::pair<Node*, Node*> NodePair; // self,parent
		Node* nil = new Node;
		Node* root;
		List<Node *> nodeList;
		DefaultValue defaultValue;
		//这几个树上的旋转操作的函数不影响前驱后继关系
		void rotataRight(Node *x)
		{
			Node* y = x->left;
			x->left = y->right;
			if (y->right != nil)
				y->right->p = x;
			y->p = x->p;
			if (x->p == nil)
				root = y;
			else
			{
				if (x == x->p->left)
					x->p->left = y;
				else x->p->right = y;
			}
			y->right = x;
			x->p = y;
			y->sz = x->sz;
			x->sz = x->left->sz + x->right->sz + 1;
		}
		void rotataLeft(Node *x)
		{
			Node* y = x->right;
			x->right = y->left;
			if (y->left != nil)
				y->left->p = x;
			y->p = x->p;
			if (x->p == nil)
				root = y;
			else
			{
				if (x == x->p->right)
					x->p->right = y;
				else x->p->left = y;
			}
			y->left = x;
			x->p = y;
			y->sz = x->sz;
			x->sz = x->left->sz + x->right->sz + 1;
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
			root->color = B;
		}
		void delFixUp(Node *x)
		{
			while (x != root&&x->color == B)
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
						x = root;
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
						x = root;
					}
				}
			}
			x->color = B;

		}
		void transplant(Node *to, Node *from)
		{
			from->p = to->p;
			if (to->p == nil)
				root = from;
			else
			{
				if (to == to->p->left)
					to->p->left = from;
				else to->p->right = from;
			}
		}
		Node* minChild(Node *x)
		{
			while (x->left != nil)
				x = x->left;
			return x;
		}
		Node* maxChild(Node *x)
		{
			while (x->right != nil)
				x = x->right;
			return x;
		}
		NodePair findNode(const K& key)
		{
			Node *x = root, *p = nil;
			while (x != nil)
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
			Node *z = new Node(p, nil, nil, R, std::make_pair(key, val));
			if (p == nil)
			{
				//nil并不是list的一部分，所以特判一下
				z->self = nodeList.insert(nodeList.begin(), z);
				root = z;
			}
			else
			{
				if (p->key() < key)
				{
					p->right = z;
					z->self = nodeList.insert(p->self.next(), z);//插在p的next和p的中间
				}
				else
				{
					p->left = z;
					z->self = nodeList.insert(p->self, z);//插在p的prev和p的中间
				}
			}
			while (p != nil)
				++p->sz, p = p->p;
			putFixUp(z);
			return z;
		}
		void erase(Node *z)
		{
			nodeList.erase(z->self);
			Node *tmp = z;
			while (tmp != nil)
				--tmp->sz, tmp = tmp->p;
			Node *y = z, *x;
			bool oldColor = y->color;
			if (z->left == nil)
			{
				x = z->right;
				transplant(z, z->right);
			}
			else if (z->right == nil)
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
						--tempy->sz;
						tempy = tempy->p;
					}
					transplant(y, y->right);
					y->right = z->right;
					y->right->p = y;
				}
				transplant(z, y);
				y->left = z->left, y->left->p = y;
				y->color = z->color;
				y->sz = y->left->sz + y->right->sz + 1;
			}
			delete z;
			if (oldColor == B)
				delFixUp(x);
		}
	public:

		Map(DefaultValue defaultValue) :defaultValue(defaultValue)
		{
			root = nil;
			root->p = root->left = root->right = nil;
		}
		Map()
		{
			defaultValue = []()->mapped_type {return mapped_type(); };
			root = nil;
			root->p = root->left = root->right = nil;
		}
		iterator find(const K& key)
		{
			if (Node *x = findNode(key).first)
				return iterator(x);
			return iterator(nodeList.end());
		}
		V& operator[](const K& key)
		{
			NodePair pn = findNode(key);
			if (pn.first)
				return pn.first->val();
			return insert(pn.second, key, defaultValue())->val();
		}
		void erase(const K& key)
		{
			NodePair pn = findNode(key);
			if (pn.first)
				erase(pn.first);
		}
		iterator insert(const key_type& key, const mapped_type& val)
		{
			//与stl统一，重复insert并不会更新节点的值
			NodePair np = findNode(key);
			if (np.first)
				return iterator(np.first);
			return iterator(insert(np.second, key, val));
		}
		iterator insert(const value_type &kvpair)
		{
			return insert(kvpair.first, kvpair.second);
		}
		int size()const { return root->sz; }
		int countLower(const K &rend, bool canEqual) const
		{
			Node *x = root;
			int cnt = 0;
			while (x != nil)
			{
				if (x->key < rend || (canEqual && !(rend < x->key) && !(x->key < rend)))
				{
					cnt += x->left->sz + 1;
					x = x->right;
				}
				else
					x = x->left;
			}
			return cnt;
		}

		int countBetween(int lend, int rend) { return countLower(rend, true) - countLower(lend, false); }

		iterator kth(int k)
		{
			Node *x = root;
			while (x != nil)
			{
				if (x->left->sz + 1 == k)
					return iterator(x);
				if (x->left->sz + 1 < k)
				{
					k -= x->left->sz + 1;
					x = x->right;
				}
				else
					x = x->left;
			}
			return iterator(nodeList.end());
		}

		iterator begin() { return iterator(*nodeList.begin()); }
		iterator end() { return iterator(*nodeList.end()); }
	};
}

