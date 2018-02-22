#pragma once
#include "Algorithm.h"
#include "HashMapUtil.h"

namespace ds
{
	template <typename K, typename V, typename Hash>
	class HashMap<K, V, Hash, false>
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
		//为了效率起见，使用单向链表，省略拷贝控制和迭代器
		//毕竟stl的迭代器也是ForwardIterator...
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
				else //删除root
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
		Node *nextNode(Node *x, int &where) const
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
		class const_iterator;
		class iterator
		{
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef std::pair<const K, V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
			friend const_iterator;
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
			bool operator==(iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(iterator rhs) const { return !(*this == rhs); }
			iterator(Node *n, HashMap *m, int w) :self_(n), container_(m), where_(w) {}
		};
		class const_iterator
		{
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef std::pair<const K, const V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
		private:
			Node *self_;
			const HashMap *container_;
			int where_;
		public:
			const_reference operator*() const { return self_->data; }
			const_pointer operator->() const { return &(operator*()); }
			const_iterator& operator++()
			{
				self_ = container_->nextNode(self_, where_); //where_传引用
				return *this;
			}
			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++*this;
				return tmp;
			}
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			const_iterator(Node *n, const HashMap *m, int w) :self_(n), container_(m), where_(w) {}
			const_iterator(iterator it) :self_(it.n), container_(it.m), where_(it.w) {}
		};
	private:
		int slotNum_;
		int size_ = 0;
		float maxLoadFactor_ = 1.0f;
		List<value_type> *slots_;
		Hash hasher;
		typedef typename std::conditional<sizeof(decltype(hasher(std::declval<K>()))) <= 4,
			unsigned, unsigned long long>::type CodeType;
		const static int shiftBit = sizeof(CodeType) * 4;

		NodePair findNode(CodeType h, const K &key)
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
		CodeType hashCode(const K &key)
		{
			CodeType code = hasher(key);
			return (code ^ (code >> shiftBit)) & (slotNum_ - 1);
		}
		static int calc(int x)
		{
			int ret = 16;
			for (; ret < x; ret <<= 1);
			return ret;
		}
		void rehash()
		{
			if (slotNum_ * maxLoadFactor_ < size_)
				rehash(size_ / maxLoadFactor_);
		}
	public:
		V &operator[](const K &key)
		{
			auto h = hashCode(key);
			NodePair np = findNode(h, key);
			if (np.first)
				return np.first->val();
			return insert(key, mapped_type())->second;
		}
		iterator find(const K &key)
		{
			auto h = hashCode(key);
			if (Node *n = findNode(h, key).first)
				return iterator(n, this, h);
			return end();
		}
		iterator insert(const K &key, const V &val)
		{
			rehash(); //rehash不用那么精确，size究竟有没有++并不是很重要，虽然看上去会有点奇怪
			auto h = hashCode(key);
			Node *n = findNode(h, key).first;
			if (!n)
				n = slots_[h].addFront({ key,val }), ++size_;
			return iterator(n, this, h);
		}
		iterator insert(const value_type &value) { return insert(value.first, value.second); }
		void erase(const K &key)
		{
			auto h = hashCode(key);
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
			if (nSlotNum > slotNum_)
			{
				//不发生复制，只是链表调整
				List<value_type> *optr = slots_, *nptr = new List<value_type>[nSlotNum];
				int oSlotNum = slotNum_;
				slotNum_ = nSlotNum; //在算hashCode的时候要用新的
				for (int i = 0; i < oSlotNum; ++i)
					while (optr[i].root)
					{
						Node *extracted = optr[i].root;
						optr[i].root = optr[i].root->next;
						nptr[hashCode(extracted->key())].addFront(extracted); //此时hashCode已经以nSlotNum为模数计算了
					}
				delete[]optr;
				slots_ = nptr;
			}
		}
		int size() const { return size_; }
		float load_factor() const { return 1.0f * size_ / slotNum_; }
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
			return ++iterator(nullptr, this, 0); //++并没有引用限定符，可以用在右值上；++返回的引用用于拷贝构造返回值
		}
		const_iterator begin() const
		{
			if (slots_[0].root)
				return const_iterator(slots_[0].root, this, 0);
			return ++const_iterator(nullptr, this, 0);
		}
		iterator end() { return iterator(nullptr, this, slotNum_); }
		const_iterator end() const { return const_iterator(nullptr, this, slotNum_); }
		explicit HashMap(int slotNum_ = 16, Hash hasher = Hash())
			: slotNum_(slotNum_), slots_(new List<value_type>[slotNum_]), hasher(hasher) {}
		void swap(HashMap &rhs) noexcept
		{
			ds::bitwiseSwap(this, &rhs);
		}
		HashMap(const HashMap &rhs) :HashMap(rhs.slotNum_, rhs.hasher)
		{
			maxLoadFactor_ = rhs.maxLoadFactor_;
			for (const auto &elem : rhs) //反正O(n)，为所欲为
				insert(elem);
		}
		HashMap& operator=(const HashMap &rhs)
		{
			if (this != &rhs)
			{
				HashMap tmp(rhs);
				swap(tmp);
			}
			return *this;
		}
		HashMap(HashMap &&rhs) noexcept
		{
			slotNum_ = rhs.slotNum_;
			size_ = rhs.size_;
			maxLoadFactor_ = rhs.maxLoadFactor_;
			slots_ = rhs.slots_, rhs.slots_ = nullptr;
			hasher = rhs.hasher;
		}
		HashMap& operator=(HashMap &&rhs) noexcept
		{
			if (this != &rhs)
			{
				HashMap tmp(std::move(rhs));
				swap(tmp);
			}
			return *this;
		}
		~HashMap() { delete[] slots_; }
	};
}

