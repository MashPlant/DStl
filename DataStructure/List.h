#pragma once
namespace ds
{
	//stl����������һ��Allocator������Ϊ�˲���������֮һ�������һ��PlaceHolder
	//��������������ʱû�мӣ���Ϊû������
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
			//"����ȥ"��iterator��ʵ�ǹ���ͬһ��Node���κ��޸�Node����Ϊ����Ӱ��iterator���������ܱ�֤ɾ��һ���ڵ㲻Ӱ������ǰ��ڵ�
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
				iterator tmp = *this;//����ָ��self
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
			}//iterator�����Զ�תconst_iterator
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
			//iterator����֮ʱself_����һ����delete
			//self_��delete֮ʱ��һ������ƴ��ǰ��
			Node() { prev_ = next_ = this; }//Ĭ�Ϲ��캯�����Լ����ɻ�
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
		//�Ͽ�sour��Դ����ϵ���Ұ�sour���뵽destǰ��û�и��Ʋ���
		//ע��û�и���size_(��Ϊinsert����������iterator����Դ)��������Ҫ�Լ�����
		static void insert(iterator dest, iterator sour)
		{
			Node::extract(sour.self_);
			Node::insert(dest.self_->prev_, dest.self_, sour.self_);
		}
	public:
		List()
		{
			nil_.self_ = new Node;//Ĭ�Ϲ��캯������Լ����Լ�������
		}
		iterator push_back(const_reference key)//Ϊ�����������д��һ�У���ʵ�ǿ��Ե�
		{
			iterator ntail(key, nil_.prev(), nil_);
			++size_;
			return ntail;
		}
		void pop_back()
		{
			if (nil_.prev() == nil_) //�ȼ���size_==0
				exceptionHandle("pop_back at lenth of 0");
			--size_;
			remove(nil_.prev());//Node�������������ã��Զ�ƴ��
		}

		iterator push_front(const_reference key)
		{
			iterator nhead(key, nil_, nil_.next());
			++size_;
			return nhead;
		}
		void pop_front()
		{
			if (nil_.next() == nil_) //�ȼ���size_==0
				exceptionHandle("pop_front at lenth of 0");
			--size_;
			remove(nil_.next());
		}
		iterator erase(iterator where)
		{
			//���׼��ͳһ�����ر��Ƴ��ڵ�֮���һ���ڵ�
			//���ϣ����ѭ���б������нڵ㲢ɾ��һЩ�ڵ㣬��Ҫ where = erase(where); --where;
			if (where == nil_)
				exceptionHandle("erase end node");
			iterator ret = where.next();
			remove(where);
			--size_;
			return ret;
		}

		iterator erase(iterator first, iterator last)//[)
		{
			//����飬�Բð�
			iterator ret = last.next();
			while (first != last)
				remove(first++), --size_;
			return ret;
		}
		iterator insert(iterator where, const_reference key)//����where֮ǰ�����ز����ڵ�ĵ�����
		{
			iterator ret(key, where.prev(), where);
			++size_;
			return ret;
		}
		template <typename Iter>
		iterator insert(iterator where, Iter first, Iter last)//���׼��һ������first��last�Ǳ�list�ĵ�����ʱ�����δ����
		{
			//��[first,last)���뵽where֮ǰ
			//����ָ�򱻲���ĵ�һ��Ԫ�صĵ�����
			iterator beforeinsert = where.prev();
			while (first != last)
				insert(where, *first++), ++size_;
			return beforeinsert.next();
		}

		//��׼��spliceû�з���ֵ���������ֵ��ȫ��Ϊ��쳲������Ѽӵ�(�����ַ����ò��ţ���ɾ����
		void splice(iterator where,List &rhs,iterator sour) //����ֵ:first->���Ʋ���ķ���ֵ��second->����ɾ���ķ���ֵ
		{
			iterator afterdel = sour.next();
			insert(where, sour);
			++size_, --rhs.size_;
			//return { sour, afterdel };
		}

		void splice(iterator where,List &rhs) //���rhs
		{
			if (this == &rhs || rhs.empty())
				return;
			//��Ҳ��д������һ��ģ�����������Ҳûë��
			iterator beforeinsert = where.prev();
			Node *prev = where.prev().self_, *pos=where.self_,*sourfirst = rhs.begin().self_, *sourlast = rhs.end().prev().self_;
			//��
			sourfirst->prev_ = prev, prev->next_ = sourfirst;
			sourlast->next_ = pos, pos->prev_ = sourlast;
			//�ϣ�Ҳ���ǰ�rhs��nil�Լ����ɻ�
			sourlast = rhs.end().self_;
			sourlast->next_ = sourlast->prev_ = sourlast;
			size_ += rhs.size_;
			rhs.size_ = 0;
			//return { beforeinsert.next(),rhs.end() };
		}

		void merge(List &rhs) //�鲢�����rhs������
		{
			iterator it1 = begin(), it2 = rhs.begin();
			auto cmp = [](iterator it1, iterator it2) {return it1.self_->key_ < it2.self_->key_; };
			while (it1 != end() && it2 != rhs.end())
			{
				if (cmp(it2, it1))  //it1����Ҫ��it2���뵽it1֮ǰ
					insert(it1, it2++);
				else //it2�󣬼����Ҳ����
					++it1;
			}
			while (it2 != rhs.end()) //��ʣһЩrhs�е�Ԫ�ر������β��
				insert(end(), it2++);
			size_ += rhs.size_;
			rhs.size_ = 0;
		}

		void sort()
		{
			if (size_ >= 1)
			{
				List carry;
				List counter[64];//�����counter����������Ͱ���ڱ�����ʱ��������Ԫ����������Ŀ�Ͱֻռһ���ڴ棬���Ժ���
				int fill = 0;
				while (size_) //n�β���
				{
					insert(carry.begin(), this->begin());//��this���ߵ�һ���ڵ�
					--size_; ++carry.size_;
					int i = 0;
					//carry���Ŵ�*this���ߵ��Ǹ��ڵ�һ·�鲢��ȥ
					//������Ԫ�ص�Ͱ�ͺ͸�Ͱ�ϲ����ϲ���swap��carry��
					while (i < fill && !counter[i].empty()) 
					{
						counter[i].merge(carry);
						carry.swap(counter[i++]);
					}
					//һ�½���ֱ��������Ͱ�Ͱѵ�ǰֹͣ����carry����swap����Ͱ�ﱣ��
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
			if (nil_.self_)//�ƶ����캯����ʹnil_.self_Ϊ��
			{
				clear();
				remove(nil_);
			}
		}
	};
}

