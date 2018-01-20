#pragma once
namespace ds
{
	//stl的容器都有一个Allocator参数，为了参数数量与之一致这里加一个PlaceHolder
	//其他容器都还暂时没有加，因为没有需求
	template<typename K,typename PlaceHolder = bool>
	class List
	{
	private:
		struct Node;
	public:
		typedef K value_type;
		typedef int difference_type;
		typedef K *pointer;
		typedef K &reference;
		typedef const K &const_reference;
		typedef const K *const_pointer;
		class const_iterator;
		class iterator
		{
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef K *pointer;
			typedef K &reference;
			typedef const K &const_reference;
			typedef const K *const_pointer;
		private:
			Node *self_;
			//"给出去"的iterator其实是共用同一个Node，任何修改Node的行为都会影响iterator，这样才能保证删除一个节点不影响它的前后节点
		public:
			friend List;
			friend const_iterator;
			reference operator*() { return self_->key_; }
			pointer operator->() { return &(operator*()); }
			iterator& operator++()
			{
				self_ = self_->next_;
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;//复制指针self
				++*this;
				return tmp;
			}
			iterator& operator--()
			{
				self_ = self_->prev_;
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
			iterator prev()const { return iterator(self_->prev_); }
			iterator next()const { return iterator(self_->next_); }
			bool operator==(const_iterator rhs) const
			{
				return self_ == rhs.self_;
			}//iterator可以自动转const_iterator
			bool operator!=(const_iterator rhs) const
			{
				return !(*this == rhs);
			}
			explicit iterator(Node *self = nullptr) :self_(self) {}
			iterator(const_reference key, const_iterator prev, const_iterator next) : self_(new Node(key, prev.self_, next.self_)) {}
		};
		class const_iterator
		{
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef const K &reference;
			typedef const K *pointer;
		private:
			Node *self_;
		public:
			friend List;
			friend iterator;
			reference operator*() { return self_->key_; }
			pointer operator->() { return &(operator*()); }
			const_iterator& operator++() { self_ = self_->next_; return *this; }
			const_iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }
			const_iterator& operator--() { self_ = self_->prev_; return *this; }
			const_iterator operator--(int) { iterator tmp = *this; --*this; return tmp; }
			const_iterator prev()const { return const_iterator(self_->prev_); }
			const_iterator next()const { return const_iterator(self_->next_); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			explicit const_iterator(Node *self = nullptr) :self_(self) {}
			const_iterator(const_reference key, const_iterator prev, const_iterator next) : self_(new Node(key, prev.self_, next.self_)) {}
			const_iterator(iterator it) :self_(it.self_) {}
		};
		typedef std::pair<iterator, iterator> IterPair;//[)
	private:
		struct Node
		{
			K key_;
			Node *prev_;
			Node *next_;
			//iterator析构之时self_并不一定被delete
			//self_被delete之时，一定可以拼接前后
			Node() { prev_ = next_ = this; }//默认构造函数把自己连成环
			static void extract(Node *x)
			{
				x->next_->prev_ = x->prev_;
				x->prev_->next_ = x->next_;
			}
			static void insert(Node *prev, Node *next, Node *x)
			{
				x->next_ = next;
				x->prev_ = prev;
				next->prev_ = x;
				prev->next_ = x;
			}
			Node(const_reference key, Node *prev, Node *next) :key_(key) { insert(prev, next, this); }
			~Node() { extract(this); }
		};
		void exceptionHandle(const std::string &msg)
		{
			std::cerr << msg;
			exit(1);
		}
		static void remove(iterator where) { delete where.self_; }
		int size_ = 0;
		iterator nil_; // head = nil->next , tail = nil -> prev
		//断开sour与源的联系并且把sour插入到dest前，没有复制操作
		//注意没有更新size_(因为insert根本不关心iterator的来源)，调用者要自己更新
		static void insert(iterator dest, iterator sour)
		{
			Node::extract(sour.self_);
			Node::insert(dest.self_->prev_, dest.self_, sour.self_);
		}
	public:
		List()
		{
			nil_.self_ = new Node;//默认构造函数会把自己和自己连起来
		}
		iterator push_back(const_reference key)//为了清晰起见不写成一行，其实是可以的
		{
			iterator ntail(key, nil_.prev(), nil_);
			++size_;
			return ntail;
		}
		void pop_back()
		{
			if (nil_.prev() == nil_) //等价于size_==0
				exceptionHandle("pop_back at lenth of 0");
			--size_;
			remove(nil_.prev());//Node析构函数被调用，自动拼接
		}

		iterator push_front(const_reference key)
		{
			iterator nhead(key, nil_, nil_.next());
			++size_;
			return nhead;
		}
		void pop_front()
		{
			if (nil_.next() == nil_) //等价于size_==0
				exceptionHandle("pop_front at lenth of 0");
			--size_;
			remove(nil_.next());
		}
		iterator erase(iterator where)
		{
			//与标准库统一，返回被移除节点之后的一个节点
			//如果希望在循环中遍历所有节点并删除一些节点，需要 where = erase(where); --where;
			if (where == nil_)
				exceptionHandle("erase end node");
			iterator ret = where.next();
			remove(where);
			--size_;
			return ret;
		}

		iterator erase(iterator first, iterator last)//[)
		{
			//不检查，自裁吧
			iterator ret = last.next();
			while (first != last)
				remove(first++), --size_;
			return ret;
		}
		iterator insert(iterator where, const_reference key)//插在where之前，返回插入后节点的迭代器
		{
			iterator ret(key, where.prev(), where);
			++size_;
			return ret;
		}
		template <typename Iter>
		iterator insert(iterator where, Iter first, Iter last)//与标准库一样，当first或last是本list的迭代器时，结果未定义
		{
			//把[first,last)插入到where之前
			//返回指向被插入的第一个元素的迭代器
			iterator beforeinsert = where.prev();
			while (first != last)
				insert(where, *first++), ++size_;
			return beforeinsert.next();
		}

		//标准库splice没有返回值。这个返回值完全是为了斐波那契堆加的(后来又发现用不着，就删掉了
		void splice(iterator where,List &rhs,iterator sour) //返回值:first->类似插入的返回值，second->类似删除的返回值
		{
			iterator afterdel = sour.next();
			insert(where, sour);
			++size_, --rhs.size_;
			//return { sour, afterdel };
		}

		void splice(iterator where,List &rhs) //清空rhs
		{
			if (this == &rhs || rhs.empty())
				return;
			//我也想写的优雅一点的，不过就这样也没毛病
			iterator beforeinsert = where.prev();
			Node *prev = where.prev().self_, *pos=where.self_,*sourfirst = rhs.begin().self_, *sourlast = rhs.end().prev().self_;
			//连
			sourfirst->prev_ = prev, prev->next_ = sourfirst;
			sourlast->next_ = pos, pos->prev_ = sourlast;
			//断，也就是把rhs的nil自己连成环
			sourlast = rhs.end().self_;
			sourlast->next_ = sourlast->prev_ = sourlast;
			size_ += rhs.size_;
			rhs.size_ = 0;
			//return { beforeinsert.next(),rhs.end() };
		}

		void merge(List &rhs) //归并，清空rhs的内容
		{
			iterator it1 = begin(), it2 = rhs.begin();
			auto cmp = [](iterator it1, iterator it2) {return it1.self_->key_ < it2.self_->key_; };
			while (it1 != end() && it2 != rhs.end())
			{
				if (cmp(it2, it1))  //it1大，需要把it2插入到it1之前
					insert(it1, it2++);
				else //it2大，继续找插入点
					++it1;
			}
			while (it2 != rhs.end()) //还剩一些rhs中的元素必须插在尾部
				insert(end(), it2++);
			size_ += rhs.size_;
			rhs.size_ = 0;
		}

		void sort()
		{
			if (size_ >= 1)
			{
				List carry;
				List counter[64];//定义的counter数组有若干桶用于保存临时的已排序元素链，多余的空桶只占一点内存，可以忽略
				int fill = 0;
				while (size_) //n次操作
				{
					insert(carry.begin(), this->begin());//从this移走第一个节点
					--size_; ++carry.size_;
					int i = 0;
					//carry带着从*this移走的那个节点一路归并上去
					//碰到有元素的桶就和该桶合并，合并后swap到carry中
					while (i < fill && !counter[i].empty()) 
					{
						counter[i].merge(carry);
						carry.swap(counter[i++]);
					}
					//一致进行直到碰到空桶就把当前停止并把carry链表swap到该桶里保存
					carry.swap(counter[i]);
					if (i == fill) 
						++fill;
				}
				for (int i = 1; i < fill; ++i)
					counter[i].merge(counter[i - 1]);
				swap(counter[fill - 1]);
			}
		}

		void clear()
		{
			iterator head = nil_.next();
			while (head != nil_)
				remove(head++);
			size_ = 0;
		}

		bool empty() const { return size_ == 0; }
		int size() const { return size_; }
		iterator begin() { return nil_.next(); }
		iterator end() { return nil_; }
		const_iterator begin() const { return nil_.next(); }
		const_iterator end() const { return nil_; }
		reference front() { return *begin(); }
		reference back() { return *(end().prev()); }
		const_reference front() const { return *begin(); }
		const_reference back() const { return *(end().prev()); }
		void swap(List &rhs) noexcept
		{
			std::swap(nil_, rhs.nil_);
			std::swap(size_, rhs.size_);
		}
		List &operator=(List rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		List(const List &rhs)
		{
			nil_.self_ = new Node;
			insert(nil_, rhs.begin(), rhs.end());
		}
		List &operator=(List &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~List();
				nil_ = rhs.nil_;
				rhs.nil_.self_ = nullptr;
				size_ = rhs.size_;
			}
			return *this;
		}
		List(List &&rhs) noexcept
		{
			nil_ = rhs.nil_;
			rhs.nil_.self_ = nullptr;
			size_ = rhs.size_;
		}

		~List()
		{
			if (nil_.self_)//移动构造函数会使nil_.self_为空
			{
				clear();
				remove(nil_);
			}
		}
	};
}

