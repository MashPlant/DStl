#pragma once
#include <string>
#include <cstdlib>
#include <iostream>
#include <type_traits>

namespace ds
{
	template <typename K>
	class Vector
	{
	public:
		typedef K value_type;
		typedef K *iterator;
		typedef K *pointer;
		typedef K &reference;
		typedef const K &const_reference;
		typedef const K *const_iterator;
		typedef const K *const_pointer;
	private:
		const static int INITIAL_CAP = 16;
		pointer arr_;
		int size_;
		int capacity_;
		void realloc(int newCapacity)
		{
			/*
			void *realloc( void *ptr, size_t newsize );
			a) expanding or contracting the existing area pointed to by ptr,
			If the area is expanded, the contents of the new part of the array are undefined.
			b) allocating a new memory block of size newsize bytes,
			copying memory area with size equal the lesser of the new and the old sizes,
			and freeing the old block.

			On failure, returns a null pointer. The original pointer ptr remains valid and may need to be deallocated with std::free()
			*/
			arr_ = (pointer)std::realloc(arr_, sizeof(value_type) * newCapacity);
			capacity_ = newCapacity;
			if (!arr_)
				exceptionHandle("alloc fail");
		}
		void expand()
		{
			if (size_ == capacity_)
				realloc(capacity_ << 1);
		}

		void exceptionHandle(const std::string &msg)
		{
			std::cerr << msg;
			exit(1);
		}

		void destruct(iterator first, iterator last)
		{
			if (std::is_class<value_type>::value)
				for (; first != last; ++first)
					first->~K();
		}
		void destruct(iterator where)
		{
			if (std::is_class<value_type>::value)
			where->~K();
		}
		void mov(iterator dest, const_iterator first, const_iterator last) //放弃原来的
		{
			memmove(dest, first, (last - first) * sizeof(value_type));
		}
		void cpy(iterator dest, const_iterator first, const_iterator last) //保留原来的
		{
			if (std::is_class<value_type>::value)
				for (; first != last; ++dest, ++first)
					new (dest) value_type(*first);
			else
				memcpy(dest, first, (last - first) * sizeof(value_type));
		}

	public:
		void push_back(const_reference key)
		{
			expand();
			new (arr_ + size_++) value_type(key);
		}
		void pop_back()
		{
			if (size_ == 0)
				exceptionHandle("pop_back at size_th of 0");
			destruct(arr_ + size_ - 1);
			--size_;
		}

		void erase(int pos, int cnt = 1)
		{
			if (pos < 0 || pos >= size_)
				exceptionHandle("erase subscript out of range");
			cnt = min(size_ - pos, cnt);
			if (!cnt)
				return;
			destruct(arr_ + pos, arr_ + pos + cnt);
			mov(arr_ + pos, arr_ + pos + cnt, arr_ + size_);
			//注意到，对于类类型会产生size_和size_-1位置两个bit完全一致的版本
			//但是这并不会有什么问题，因为不可能再对size_位置的对象调用析构函数
			size_ -= cnt;
		}
		void insert(int pos, const_reference key) //insert and be there
		{
			if (pos < 0 || pos > size_)
				exceptionHandle("insert subscript out of range");
			expand();
			mov(arr_ + pos + 1, arr_ + pos, arr_ + size_);
			new (arr_ + pos) value_type(key);
		}
		iterator begin() { return arr_; }
		iterator end() { return arr_ + size_; }
		const_iterator begin() const { return arr_; }
		const_iterator end() const { return arr_ + size_; }
		reference front() { return *arr_; }
		reference back() { return *(arr_ + size_ - 1); }
		const_reference front() const { return *arr_; }
		const_reference back() const { return *(arr_ + size_ - 1); }
		bool empty() const { return size_ == 0; }
		int size() const { return size_; }
		int capacity() const { return capacity_; }
		reference operator[](int pos) { return arr_[pos]; }
		const_reference operator[](int pos) const { return arr_[pos]; }
		Vector(int size = 0, int newCapacity = INITIAL_CAP) : size_(size), capacity_(newCapacity)
		{
			arr_ = (pointer)malloc(newCapacity * sizeof(value_type));
		}
		void swap(Vector &rhs) noexcept
		{
			std::swap(arr_, rhs.arr_);
			std::swap(size_, rhs.size_);
			std::swap(capacity_, rhs.capacity_);
		}
		Vector &operator=(Vector rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		Vector(const Vector &rhs) : arr_(nullptr)
		{
			realloc(rhs.capacity_);
			size_ = rhs.size_;
			cpy(arr_, rhs.begin(), rhs.end());
		}
		Vector &operator=(Vector &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~Vector();
				arr_ = rhs.arr_;
				rhs.arr_ = nullptr;
				capacity_ = rhs.capacity_, size_ = rhs.size_;
			}
			return *this;
		}
		Vector(Vector &&rhs) noexcept
		{
			std::cout << " Vector && ";
			arr_ = rhs.arr_;
			rhs.arr_ = nullptr;
			capacity_ = rhs.capacity_, size_ = rhs.size_;
		}
		Vector(const std::initializer_list<value_type> &rhs) : arr_(nullptr)
		{
			realloc(rhs.size() + INITIAL_CAP);
			size_ = rhs.size();
			cpy(arr_, rhs.begin(), rhs.end());
		}
		template <size_t N>
		Vector(const K(&keys)[N]) : arr_(nullptr)
		{
			realloc(N + INITIAL_CAP);
			size_ = N;
			cpy(arr_, keys, keys + N);
		}
		~Vector()
		{
			destruct(arr_, arr_ + size_);
			free(arr_);
		}
	};

}