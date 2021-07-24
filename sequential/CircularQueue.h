#pragma once
#include <iostream>
#include "Util.h"
namespace ds
{
	//取模实现循环队列
	//&实现取模
	template <typename K>
	class CircularQueue
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
			CircularQueue *container_;
			operator pointer() const { return self_; }
			int cur() const { return (self_ - container_->arr_)&(container_->capacity_ - 1); }
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef K *pointer;
			typedef K &reference;
			typedef const K &const_reference;
			typedef const K *const_pointer;
			friend CircularQueue;
			friend const_iterator;

			iterator() : self_(nullptr), container_(nullptr) {}
			iterator(pointer self, CircularQueue *container) :self_(self), container_(container) {}
			reference operator*() const { return *self_; }
			pointer operator->() const { return self_; }
			iterator &operator+=(int offset)
			{
				const int oldoffset = self_ - container_->beg_;
				self_ = container_->beg_ + ((oldoffset + offset)&(container_->capacity_ - 1));
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
			int operator-(const const_iterator &rhs) const
			{
				return ((self_ - rhs.self_) & (container_->capacity_ - 1)) * (*this < rhs ? -1 : 1);
			}
			bool operator!=(const const_iterator &rhs) const { return self_ != rhs.self_; }
			bool operator==(const const_iterator &rhs) const { return self_ == rhs.self_; }
			bool operator<(const const_iterator &rhs) const
			{
				const pointer arr = container_->arr_;
				const bool l1 = self_ < arr, l2 = rhs.self_ < arr;
				if (l1 && !l2) return false;
				if (!l1 && l2) return true;
				return self_ < rhs.self_;
			}
			bool operator>(const const_iterator &rhs) const { return !*this < rhs && *this == rhs; }
			bool operator<=(const const_iterator &rhs) const { return *this < rhs || *this == rhs; }
			bool operator>=(const const_iterator &rhs) const { return !(*this < rhs); }
			reference operator[](int i) const { return *(*this + i); }
		};
		class const_iterator
		{
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef const K *pointer;
			typedef const K &reference;
			friend CircularQueue;
			friend iterator;
		private:
			pointer self_;
			CircularQueue *container_;
			operator pointer() const { return self_; } //不允许在外部获取到self_，以免对其做加减操作没有取模约束
			int cur() const { return (self_ - container_->arr_)&(container_->capacity_ - 1); }
		public:
			const_iterator() : self_(nullptr), container_(nullptr) {}
			const_iterator(pointer self, CircularQueue *container) :self_(self), container_(container) {}
			const_iterator(const iterator& it) :self_(it.self_), container_(it.container_) {}
			reference operator*() const { return *self_; }
			pointer operator->() const { return self_; }
			const_iterator &operator+=(int offset)
			{
				const int oldoffset = self_ - container_->beg_;
				self_ = container_->beg_ + ((oldoffset + offset)&(container_->capacity_ - 1));
				return *this;
			}
			const_iterator &operator-=(int offset) { operator+=(-offset); return *this; }
			const_iterator &operator++() { return operator+=(1), *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; operator+=(1); return tmp; }
			const_iterator &operator--() { return operator+=(-1), *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; operator+=(-1); return tmp; }
			const_iterator operator+(int offset) const { const_iterator tmp = *this; tmp += offset; return tmp; }
			const_iterator operator-(int offset) const { const_iterator tmp = *this; return tmp += -offset; }
			int operator-(const const_iterator &rhs) const
			{
				return ((self_ - rhs.self_) & (container_->capacity_ - 1)) * (*this < rhs ? -1 : 1);
			}
			bool operator!=(const const_iterator &rhs) const { return self_ != rhs.self_; }
			bool operator==(const const_iterator &rhs) const { return self_ == rhs.self_; }
			bool operator<(const const_iterator &rhs) const
			{
				const pointer arr = container_->arr_;
				const bool l1 = self_ < arr, l2 = rhs.self_ < arr;
				if (l1 && !l2) return false; if (!l1 && l2) return true;
				return self_ < rhs.self_;
			}
			bool operator>(const const_iterator &rhs) const { return !*this < rhs && *this == rhs; }
			bool operator<=(const const_iterator &rhs) const { return *this < rhs || *this == rhs; }
			bool operator>=(const const_iterator &rhs) const { return !(*this < rhs); }
			const_reference operator[](int i) const { return *(*this + i); }
		};
	private:
		iterator arr_;
		pointer beg_; //申请到的内存的真正起点
		int size_;
		int capacity_;
		void expand()
		{
			if (size_ == capacity_)
			{
				capacity_ <<= 1;
				const pointer optr = arr_, obeg = beg_; //obeg 是malloc的起点,必须是free的参数
				beg_ = (pointer)malloc(sizeof(value_type) * capacity_);
				memmove(beg_, optr, sizeof(value_type) * (obeg + size_ - arr_.self_)); //size_ == capacity_
				memmove(beg_ + (obeg + size_ - arr_.self_), obeg, sizeof(value_type) * (arr_.self_ - obeg));
				arr_ = iterator(beg_, this);
				free(obeg);
			}
		}
		static void mov(iterator dest, iterator first, iterator last) //注意重叠的情况
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
		}
		void push_front(const_reference key)
		{
			expand();
			new (--arr_) value_type(key);
			++size_;
		}
		void pop_back()
		{
			destruct(arr_ + --size_);
		}
		void pop_front()
		{
			destruct(arr_++);
			--size_;
		}
		void erase(int pos, int cnt = 1)
		{
			cnt = min(size_ - pos, cnt);
			if (!cnt)
				return;
			destruct(arr_ + pos, arr_ + pos + cnt);
			if (pos * 2 + cnt<size_)//尽量移动较少元素
			{
				mov(arr_ + cnt, arr_, arr_ + pos);
				arr_ += cnt;
			}
			else
				mov(arr_ + pos, arr_ + pos + cnt, arr_ + size_);
			size_ -= cnt;
		}
		void insert(int pos, const_reference key) //insert and be there
		{
			expand();
			if (pos * 2 < size_)
			{
				--arr_;
				mov(arr_, arr_ + 1, arr_ + pos + 1);
				new (arr_ + pos) value_type(key);
			}
			else
			{
				mov(arr_ + pos + 1, arr_ + pos, arr_ + size_);
				new (arr_ + pos) value_type(key);
			}
			++size_;
		}
		reference operator[](int pos) { return arr_[pos]; }
		const_reference operator[](int pos) const { return arr_[pos]; }
		bool empty() const { return size_ == 0; }
		int size() const { return size_; }
		int capacity() const { return capacity_; }
		iterator begin() { return arr_; }
		iterator end() { return arr_ + size_; }
		const_iterator begin() const { return arr_; }
		const_iterator end() const { return arr_ + size_; }
		reference front() { return *arr_; }
		reference back() { return *(arr_ + size_ - 1); }
		const_reference front() const { return *arr_; }
		const_reference back() const { return *(arr_ + size_ - 1); }
		CircularQueue(int size = 0, int capacity = INITIAL_CAP) : size_(size), capacity_(calc(capacity))
		{
			beg_ = (pointer)malloc(capacity_ * sizeof(value_type));
			arr_ = iterator(beg_, this);
		}
		void swap(CircularQueue &rhs) noexcept
		{
			std::swap(arr_, rhs.arr_);
			std::swap(beg_, rhs.beg_);
			std::swap(size_, rhs.size_);
			std::swap(capacity_, rhs.capacity_);
		}
		CircularQueue &operator=(CircularQueue rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		CircularQueue(const CircularQueue &rhs) : arr_(nullptr)
		{
			capacity_ = rhs.capacity_, size_ = rhs.size_;
			beg_ = (pointer)malloc(sizeof(value_type)*capacity_);
			arr_ = iterator(beg_, this);
			iterator it = arr_, first = rhs.begin(), last = rhs.end();
			if (std::is_class<value_type>::value) //直接复制到起点，可以简化一点，也不另外定义函数了
				for (; first != last; ++it, ++first)
					new (it) value_type(*first);
			else
				memcpy(arr_, first, (last - first) * sizeof(value_type));
		}
		CircularQueue &operator=(CircularQueue &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~CircularQueue();
				arr_ = rhs.arr_, beg_ = rhs.beg_;
				rhs.arr_ = rhs.beg_ = nullptr;
				capacity_ = rhs.capacity_, size_ = rhs.size_;
			}
			return *this;
		}
		CircularQueue(CircularQueue &&rhs) noexcept
		{
			arr_ = rhs.arr_, beg_ = rhs.beg_;
			rhs.arr_ = rhs.beg_ = nullptr;
			capacity_ = rhs.capacity_, size_ = rhs.size_;
		}
		~CircularQueue()
		{
			destruct(arr_, arr_ + size_);
			free(beg_); //必须free malloc得到的内存
		}
	};
}

