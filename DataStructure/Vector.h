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
		pointer arr;
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
			arr = (pointer)std::realloc(arr, sizeof(value_type) * newCapacity);
			capacity_ = newCapacity;
			if (!arr)
				exceptionHandle("alloc fail");
		}
		void shrink()
		{
			if ((size_ >> 1) + INITIAL_CAP < capacity_)
				realloc(capacity_ >> 1);
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
		void mov(iterator dest, const_iterator first, const_iterator last) //����ԭ����
		{
			memmove(dest, first, (last - first) * sizeof(value_type));
		}
		void cpy(iterator dest, const_iterator first, const_iterator last) //����ԭ����
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
			new (arr + size_++) value_type(key);
		}
		void pop_back()
		{
			if (size_ == 0)
				exceptionHandle("pop_back at size_th of 0");
			destruct(arr + size_ - 1);
			--size_;
			shrink();
		}

		void erase(int pos, int cnt = 1)
		{
			if (pos < 0 || pos >= size_)
				exceptionHandle("erase subscript out of range");
			cnt = min(size_ - pos, cnt);
			if (!cnt)
				return;
			destruct(arr + pos, arr + pos + cnt);
			mov(arr + pos, arr + pos + cnt, arr + size_);
			//ע�⵽�����������ͻ����size_��size_-1λ������bit��ȫһ�µİ汾
			//�����Ⲣ������ʲô���⣬��Ϊ�������ٶ�size_λ�õĶ��������������
			//���û�����������װ����shared_ptr�أ�
			size_ -= cnt;
			shrink();
		}
		void insert(int pos, const_reference key) //insert and be there
		{
			if (pos < 0 || pos > size_)
				exceptionHandle("insert subscript out of range");
			expand();
			mov(arr + pos + 1, arr + pos, arr + size_);
			new (arr + pos) value_type(key);
		}
		iterator begin() { return arr; }
		iterator end() { return arr + size_; }
		iterator begin() const { return arr; }
		iterator end() const { return arr + size_; }
		bool empty() const { return size_ == 0; }
		int size() const { return size_; }
		int capacity() const { return capacity_; }
		reference operator[](int pos)
		{
			if (pos < 0 || pos >= size_)
				exceptionHandle("operator[] subscript out of range");
			return arr[pos];
		}
		Vector(int size = 0, int newCapacity = INITIAL_CAP) : size_(size), capacity_(newCapacity)
		{
			arr = (pointer)malloc(newCapacity * sizeof(value_type));
		}
		void swap(Vector &rhs) noexcept
		{
			std::swap(arr, rhs.arr);
			std::swap(size_, rhs.size_);
			std::swap(capacity_, rhs.capacity_);
		}
		Vector &operator=(Vector rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		Vector(const Vector &rhs) : arr(nullptr)
		{
			std::cout << "const Vector & ";
			realloc(rhs.capacity_);
			size_ = rhs.size_;
			cpy(arr, rhs.begin(), rhs.end());
		}
		Vector &operator=(Vector &&rhs) noexcept
		{
			std::cout << "= Vector && ";
			if (this != &rhs)
			{
				~Vector();
				arr = rhs.arr;
				rhs.arr = nullptr;
				capacity_ = rhs.capacity_, size_ = rhs.size_;
			}
			return *this;
		}
		Vector(Vector &&rhs) noexcept
		{
			std::cout << " Vector && ";
			arr = rhs.arr;
			rhs.arr = nullptr;
			capacity_ = rhs.capacity_, size_ = rhs.size_;
		}
		Vector(const std::initializer_list<value_type> &rhs) : arr(nullptr)
		{
			realloc(rhs.size() + INITIAL_CAP);
			size_ = rhs.size();
			cpy(arr, rhs.begin(), rhs.end());
		}
		template <size_t N>
		Vector(const K(&keys)[N]) : arr(nullptr)
		{
			realloc(N + INITIAL_CAP);
			size_ = N;
			cpy(arr, keys, keys + N);
		}
		~Vector()
		{
			destruct(arr, arr + size_);
			free(arr);
		}
	};

}