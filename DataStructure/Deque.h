#pragma once
#include <iostream>
#include <cassert>
#include "Util.h"
namespace ds
{
	//取模实现循环队列
	//&实现取模
	template <typename K>
	class Deque
	{
	public:
		typedef K value_type;
		typedef int difference_type;
		typedef K *pointer;
		typedef K &reference;
		typedef const K &const_reference;
		typedef const K *const_pointer;
		const static int INITIAL_CAP = 16;

		class const_iterator;
		class iterator
		{
		private:
			pointer self_;
			pointer arr_; //Deque中的arr_，一旦设置不能再更改，Deque中的arr_改变之时，该iterator即失效
			int cur_;     //cur_是相对左端点的偏移量，不是相对arr_的偏移量
			int capacity_;
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef K *pointer;
			typedef K &reference;
			typedef const K &const_reference;
			typedef const K *const_pointer;

			friend Deque;
			friend const_iterator;
			operator pointer() const { return self_; }
			iterator() : self_(nullptr), arr_(nullptr), cur_(0), capacity_(0) {}
			iterator(pointer self, int capacity) : self_(self), arr_(self_), cur_(0), capacity_(capacity) {}
			reference operator*() const { return *self_; }
			pointer operator->() const { return self_; }
			iterator &operator+=(int offset)
			{
				self_ -= cur_; //得到起点
				cur_ = (cur_ + offset) & (capacity_ - 1);
				self_ += cur_;
				return *this;
			}
			iterator &operator-=(int offset)
			{
				operator+=(-offset); //&能够正确处理负数
				return *this;
			}
			iterator &operator++() { return operator+=(1), *this; }
			iterator operator++(int)
			{
				iterator tmp = *this;
				operator+=(1);
				return tmp;
			}
			iterator &operator--() { return operator+=(-1), *this; }
			iterator operator--(int)
			{
				iterator tmp = *this;
				operator+=(-1);
				return tmp;
			}
			iterator operator+(int offset) const
			{
				iterator tmp = *this;
				tmp += offset;
				return tmp;
			}
			iterator operator-(int offset) const
			{
				iterator tmp = *this;
				return tmp += -offset;
			}
			//两个迭代器间的运算为const版本和非const版本都重载一份
			//之所以不选择让运算接受const_iterator为参数，主要是考虑到迭代器大小为4+4+2+2，参数传递时用引用更好
			int operator-(const iterator &rhs) const
			{
				return ((cur_ - rhs.cur_) & (capacity_ - 1)) * (*this < rhs ? -1 : 1);
			}
			bool operator!=(const iterator &rhs) const { return self_ != rhs.self_; }
			bool operator==(const iterator &rhs) const { return self_ == rhs.self_; }
			bool operator<(const iterator &rhs) const
			{
				bool l1 = self_ < arr_, l2 = rhs.self_ < rhs.arr_;
				if (l1 && !l2)
					return false; //this 在arr_左,rhs在右
				if (!l1 && l2)
					return true;        //this 在arr_右,rhs在左
				return cur_ < rhs.cur_; //在同侧，越右越大
			}
			bool operator>(const iterator &rhs) const { return !*this < rhs && *this == rhs; }
			bool operator<=(const iterator &rhs) const { return *this < rhs || *this == rhs; }
			bool operator>=(const iterator &rhs) const { return !*this < rhs; }
			
			int operator-(const const_iterator &rhs) const
			{
				return ((cur_ - rhs.cur_) & (capacity_ - 1)) * (*this < rhs ? -1 : 1);
			}
			bool operator!=(const const_iterator &rhs) const { return self_ != rhs.self_; }
			bool operator==(const const_iterator &rhs) const { return self_ == rhs.self_; }
			bool operator<(const const_iterator &rhs) const
			{
				bool l1 = self_ < arr_, l2 = rhs.self_ < rhs.arr_;
				if (l1 && !l2)
					return false; //this 在arr_左,rhs在右
				if (!l1 && l2)
					return true;        //this 在arr_右,rhs在左
				return cur_ < rhs.cur_; //在同侧，越右越大
			}
			bool operator>(const const_iterator &rhs) const { return !*this < rhs && *this == rhs; }
			bool operator<=(const const_iterator &rhs) const { return *this < rhs || *this == rhs; }
			bool operator>=(const const_iterator &rhs) const { return !*this < rhs; }

			reference operator[](int i) const { return *(*this + i); }
		};
		class const_iterator
		{
		private:
			pointer self_, arr_;
			int cur_, capacity_;
			operator pointer() const { return self_; } //不允许在外部获取到self_，以免对其做加减操作没有取模约束
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef const K *pointer;
			typedef const K &reference;
			friend Deque;
			friend iterator;
			const_iterator() : self_(nullptr), arr_(nullptr), cur_(0), capacity_(0) {}
			const_iterator(pointer self, int capacity) : self_(self), arr_(self_), cur_(0), capacity_(capacity) {}
			const_iterator(const iterator& it) :self_(it.self_), arr_(it.arr_), cur_(it.cur_), capacity_(it.capacity_) {}
			reference operator*() const { return *self_; }
			pointer operator->() const { return self_; }
			const_iterator &operator+=(int offset) { self_ -= cur_; cur_ = (cur_ + offset) & (capacity_ - 1); self_ += cur_; return *this; }
			const_iterator &operator-=(int offset) { operator+=(-offset); return *this; }
			const_iterator &operator++() { return operator+=(1), *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; operator+=(1); return tmp; }
			const_iterator &operator--() { return operator+=(-1), *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; operator+=(-1); return tmp; }
			const_iterator operator+(int offset) const { const_iterator tmp = *this; tmp += offset; return tmp; }
			const_iterator operator-(int offset) const { const_iterator tmp = *this; return tmp += -offset; }
			int operator-(const const_iterator &rhs) const { return ((cur_ - rhs.cur_) & (capacity_ - 1)) * (*this < rhs ? -1 : 1); }
			bool operator!=(const const_iterator &rhs) const { return self_ != rhs.self_; }
			bool operator==(const const_iterator &rhs) const { return self_ == rhs.self_; }
			bool operator<(const const_iterator &rhs) const
			{
				bool l1 = self_ < arr_, l2 = rhs.self_ < rhs.arr_;
				if (l1 && !l2) return false; if (!l1 && l2) return true;
				return cur_ < rhs.cur_;
			}
			bool operator>(const const_iterator &rhs) const { return !*this < rhs && *this == rhs; }
			bool operator<=(const const_iterator &rhs) const { return *this < rhs || *this == rhs; }
			bool operator>=(const const_iterator &rhs) const { return !*this < rhs; }

			int operator-(const iterator &rhs) const { return ((cur_ - rhs.cur_) & (capacity_ - 1)) * (*this < rhs ? -1 : 1); }
			bool operator!=(const iterator &rhs) const { return self_ != rhs.self_; }
			bool operator==(const iterator &rhs) const { return self_ == rhs.self_; }
			bool operator<(const iterator &rhs) const
			{
				bool l1 = self_ < arr_, l2 = rhs.self_ < rhs.arr_;
				if (l1 && !l2) return false; if (!l1 && l2) return true;
				return cur_ < rhs.cur_;
			}
			bool operator>(const iterator &rhs) const { return !*this < rhs && *this == rhs; }
			bool operator<=(const iterator &rhs) const { return *this < rhs || *this == rhs; }
			bool operator>=(const iterator &rhs) const { return !*this < rhs; }
			reference operator[](int i) const { return *(*this + i); }
		};
	private:
		iterator arr_;
		int size_;
		int capacity_;
		void exceptionHandle(const std::string &msg)
		{
			std::cerr << msg;
			exit(1);
		}
		void expand()
		{
			if (size_ == capacity_)
			{
				capacity_ <<= 1;
				pointer optr = arr_, obeg = optr - arr_.cur_; //obeg 是malloc的起点,必须是free的参数
				pointer nptr = (pointer)malloc(sizeof(value_type) * capacity_);
				if (!nptr)
					exceptionHandle("alloc fail");
				memmove(nptr, optr, sizeof(value_type) * (size_ - arr_.cur_)); //size_==capacity_
				memmove(nptr + size_ - arr_.cur_, obeg, sizeof(value_type) * arr_.cur_);
				arr_ = iterator(nptr, capacity_);
				free(obeg);
			}
		}
		void mov(iterator dest, iterator first, iterator last) //注意重叠的情况
		{
			int count = last - first;
			if (dest <= first)
				while (count--)
					memcpy(dest++, first++, sizeof(value_type));
			else
			{
				dest += count, first += count;
				while (count--)
					memcpy(--dest, --first, sizeof(value_type));
			}
		}
		void destruct(iterator first, iterator last)
		{
			if (std::is_class<value_type>::value)
				for (; first != last; ++first)
					first.self_->~K();
		}
		void destruct(iterator where)
		{
			if (std::is_class<value_type>::value)
			where.self_->~K();
		}
		static int calc(int len)
		{
			int cap = INITIAL_CAP;
			for (; cap < len; cap <<= 1);
			return cap;
		}

	public:
		void push_back(const_reference key)
		{
			expand();
			new (arr_ + size_++) value_type(key);
			arr_.arr_ = arr_; //原有迭代器可能不失效，但不保证
		}
		void push_front(const_reference key)
		{
			expand();
			new (--arr_) value_type(key);
			++size_;
			arr_.arr_ = arr_;
			//arr_改变,必须更新之,原有迭代器一定失效
			//而且注意不能直接调指针的--，否则没有取模
		}
		void pop_back()
		{
			if (size_ == 0)
				exceptionHandle("pop_back at size of 0");
			destruct(arr_ + size_ - 1);
			--size_;
			arr_.arr_ = arr_;
		}
		void pop_front()
		{
			if (size_ == 0)
				exceptionHandle("pop_front at size of 0");
			destruct(arr_++);
			--size_;
			arr_.arr_ = arr_;
		}
		void erase(int pos, int cnt = 1)
		{
			if (pos < 0 || pos >= size_)
				exceptionHandle("erase subscript out of range");
			cnt = min(size_ - pos, cnt);
			if (!cnt)
				return;
			destruct(arr_ + pos, arr_ + pos + cnt);
			if (pos*2+cnt<size_)//尽量移动较少元素
			{
				mov(arr_ + cnt, arr_, arr_ + pos);
				arr_ += cnt;
			}
			else
				mov(arr_ + pos, arr_ + pos + cnt, arr_ + size_);
			size_ -= cnt;
			arr_.arr_ = arr_;
		}
		void insert(int pos, const_reference key) //insert and be there
		{
			if (pos < 0 || pos > size_)
				exceptionHandle("insert subscript out of range");
			expand();
			if (pos * 2 < size_)
			{
				--arr_; 
				arr_.arr_ = arr_; //及时更新arr_内保存的arr_
				mov(arr_, arr_ + 1, arr_ + pos + 1);
				new (arr_ + pos) value_type(key);
			}
			else
			{
				mov(arr_ + pos + 1, arr_ + pos, arr_ + size_);
				new (arr_ + pos) value_type(key);
			}
			++size_;
			arr_.arr_ = arr_;
		}
		reference operator[](int pos){return arr_[pos];}
		const_reference operator[](int pos) const { return arr_[pos]; }
		bool empty() const { return size_ == 0; }
		int size() const { return size_; }
		int capacity() const { return capacity_; }
		iterator begin() { return arr_; }
		iterator end() { return arr_ + size_; }
		const_iterator begin() const { return arr_; }
		const_iterator end() const { return arr_ + size_; }
		Deque(int size = 0, int capacity = INITIAL_CAP) : size_(size), capacity_(calc(capacity))
		{
			pointer ptr = (pointer)malloc(capacity_ * sizeof(value_type));
			arr_ = iterator(ptr, capacity_);
		}
		void swap(Deque &rhs) noexcept
		{
			std::swap(arr_, rhs.arr_);
			std::swap(size_, rhs.size_);
			std::swap(capacity_, rhs.capacity_);
		}
		Deque &operator=(Deque rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		Deque(const Deque &rhs) : arr_(nullptr)
		{
			capacity_ = rhs.capacity_, size_ = rhs.size_;
			arr_ = iterator((pointer)malloc(sizeof(value_type)*capacity_), capacity_);
			iterator it = arr_, first = rhs.begin(), last = rhs.end();
			if (std::is_class<value_type>::value) //直接复制到起点，可以简化一点，也不另外定义函数了
				for (; first != last; ++it, ++first)
					new (it) value_type(*first);
			else
				memcpy(arr_, first, (last - first) * sizeof(value_type));
		}
		Deque &operator=(Deque &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~Deque();
				arr_ = rhs.arr_;
				rhs.arr_ = nullptr;
				capacity_ = rhs.capacity_, size_ = rhs.size_;
			}
			return *this;
		}
		Deque(Deque &&rhs) noexcept
		{
			arr_ = rhs.arr_;
			rhs.arr_ = nullptr;
			capacity_ = rhs.capacity_, size_ = rhs.size_;
		}
		~Deque()
		{
			destruct(arr_, arr_ + size_);
			free(arr_.self_ - arr_.cur_); //必须free malloc得到的内存
		}
	};
}

