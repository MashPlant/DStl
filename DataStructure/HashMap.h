#pragma once
#include <iostream>
namespace ds
{
	template <typename K, typename V, typename Hash = std::hash<K>>
	class HashMap
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
	private:
		//为了效率起见，使用单向链表，省略拷贝控制和迭代器，因为HashMap保证不使用这些东西
		//貌似stl的迭代器也是ForwardIterator...
		template <typename U>
		struct List
		{
			struct Node
			{
				U data;
				Node *next;
				const key_type &key()const { return data.first; }
				mapped_type &val() { return data.second; }
				Node(const U&data, Node *next = nullptr) :data(data), next(next) {}
			};
			Node *root = nullptr;
			Node *addFront(const U &data)
			{
				if (root)
					root = new Node(data, root);
				else
					root = new Node(data);
				return root;
			}
			void addFront(Node *x)
			{
				x->next = root;
				root = x;
			}
			void delAfter(Node *p)
			{
				if (p)
				{
					Node *tmp = p->next;
					p->next = p->next->next;
					delete tmp;
				}
				else
				{
					Node *tmp = root;
					root = root->next;
					delete tmp;
				}
			}
			~List()
			{
				while (root)
				{
					Node *nxt = root->next;
					delete root;
					root = nxt;
				}
			}
		};
		typedef typename List<value_type>::Node Node;
		typedef std::pair<Node*, Node*> NodePair;//first->自身,second->链表内的前一个
		Node *nextNode(Node *x,int &where)
		{
			if (x && x->next)
				return x->next;
			++where;
			while (where < slotNum_ && !slots_[where].root)
				++where;
			if (where < slotNum_)
				return slots_[where].root;
			return nullptr;
		}
	public:
		class iterator
		{
		public:
			typedef std::forward_iterator_tag iterator_category;
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
			HashMap *container_;
			int where_;
		public:
			reference operator*() const { return self_->data; }
			pointer operator->()const { return &(operator*()); }
			iterator& operator++()
			{
				self_ = container_->nextNode(self_, where_); //where_传引用
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;
				++*this;
				return tmp;
			}
			iterator next()const
			{
				int tmp = where_;
				Node *ret = container_->nextNode(self_, tmp); //如果写在一行，tmp的值既被改变也被取用，是未定义行为
				return iterator(ret, container_, tmp);
			}
			bool operator==(iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(iterator rhs) const { return !(*this == rhs); }
			iterator(Node *n, HashMap *m,int w) :self_(n), container_(m),where_(w) {}
		};
	private:
		int slotNum_;
		int size_ = 0;
		float maxLoadFactor_ = 1.0f;
		List<value_type> *slots_;
		NodePair findNode(int h,const K &key)
		{
			Node *prev = nullptr;
			Node *n = slots_[h].root;
			while (n)
			{
				if (n->key() == key)
					break;
				prev = n;
				n = n->next;
			}
			return { n,prev };//没找到 -> n==nullptr
		}
		Hash hasher;
		int hashCode(const K &key)
		{
			const int code = hasher(key);
			return (code ^ (unsigned(code) >> 16)) & (slotNum_ - 1);
		}
		static int calc(int x)
		{
			int ret = 16;
			for (; ret < x; ret <<= 1);
			return ret;
		}
		void rehash()
		{
			if (slotNum_*maxLoadFactor_ < size_)
				rehash(size_ / maxLoadFactor_);
		}
	public:
		HashMap(int slotNum_= 16, Hash hasher = Hash()) : slotNum_(slotNum_), hasher(hasher)
		{
			slots_ = new List<value_type>[slotNum_];
		}
		~HashMap() { delete[] slots_; }
		V &operator[](const K &key)
		{
			int h = hashCode(key);
			NodePair np = findNode(h, key);
			if (np.first)
				return np.first->val();
			return insert(key, mapped_type())->val();
		}
		iterator find(const K &key)
		{
			const int h = hashCode(key);
			if (Node *n = findNode(h, key).first)
				return iterator(n, this, h);
			return end();
		}
		Node *insert(const K &key, const V &val) //不检查是否已经存在
		{
			int h = hashCode(key);
			if (!findNode(h,key).first)
			{
				Node *ret = slots_[h].addFront({ key,val });
				++size_;
				rehash();
				return ret;
			}
			return findNode(h, key).first;
		}
		void insert(const value_type&value)
		{
			insert(value.first, value.second);
		}
		void erase(const K &key) 
		{
			int h = hashCode(key);
			NodePair pn = findNode(h, key);
			if (pn.first)
			{
				slots_[h].delAfter(pn.second);
				--size_;
			}
		}
		void rehash(int nSlotNum)
		{
			nSlotNum = calc(nSlotNum);
			if (nSlotNum != slotNum_)
			{
				//不发生复制，只是链表调整
				List<value_type> *optr = slots_, *nptr = new List<value_type>[nSlotNum];
				int oSlotNum = slotNum_;
				slotNum_ = nSlotNum;
				for (int i = 0; i < oSlotNum; ++i)
				{
					while (optr[i].root)
					{
						Node *extracted = optr[i].root;
						optr[i].root = optr[i].root->next;
						nptr[hashCode(extracted->key())].addFront(extracted); //此时hashCode已经以nSlotNum为模数计算了
					}
				}
				delete[]optr;
				slots_ = nptr;
			}
		}
		int size() const { return size_; }
		float load_factor()const { return 1.0f*size_ / slotNum_; }
		float max_load_factor() const { return maxLoadFactor_; }
		void max_load_factor(float nLoadFactor)
		{
			maxLoadFactor_ = nLoadFactor;
			rehash();
		}
		iterator begin()
		{
			if (slots_[0].root)
				return iterator(slots_[0].root, this, 0);
			return iterator(nullptr, this, 0).next();
		}
		iterator end() { return iterator(nullptr, this, slotNum_); }
	};
}

