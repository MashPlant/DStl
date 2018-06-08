#pragma once
#include <cstdlib>
#include <type_traits>

namespace ds
{
	template <typename K>
	class Vector
	{
		const static int InitialCap = 8;
		const static bool NonTrivial = !std::is_trivially_copyable<K>::value || !std::is_trivially_destructible<K>::value;
		void realloc(int newCap)
		{
			//realloc:尝试追加内存；失败则新申请内存并bitwise拷贝
			arr = (K*)std::realloc(arr, sizeof(K) * newCap);
			cap = newCap;
		}
		void expand()
		{
			if (siz == cap)
				realloc(cap << 1);
		}
		static void destruct(K *first, K *last)
		{
			if (NonTrivial)
				for (; first != last; ++first)
					first->~K();
		}
		static void destruct(K *pos) { pos->~K(); }
		template <typename ...Args>
		static void construct(K *pos, Args &&...args) { new(pos) K(std::forward<Args>(args)...); }
		static int calc(int len)
		{
			int cap = InitialCap;
			for (; cap < len; cap <<= 1);
			return cap;
		}
	protected:
		int siz = 0;
		int cap = 0;
		K * arr = nullptr;
		static void mov(K *dest, const K *first, const K *last) //放弃原来的
		{
			memmove(dest, first, (last - first) * sizeof(K));
		}
		static void cpy(K *dest, const K *first, const K *last) //保留原来的
		{
			if (NonTrivial)
				for (; first != last; ++dest, ++first)
					new(dest) K(*first);
			else
				memcpy(dest, first, (last - first) * sizeof(K));
		}
	public:
		void push_back(K key)
		{
			expand();
			construct(arr + siz++, std::move(key));
		}
		void add_all(const Vector &rhs)
		{
			reserve(siz + rhs.siz);
			cpy(arr + siz, rhs.begin(), rhs.end());
			siz += rhs.siz;
		}
		void resize(int nSize)
		{
			if (nSize == siz)
				return;
			if (siz < nSize)
			{
				reserve(calc(nSize));
				for (K *cur = arr + siz, *last = arr + nSize; cur != last; ++cur)
					construct(cur, K());
			}
			else
				destruct(arr + nSize, arr + siz);
			siz = nSize;
		}
		void pop_back() { destruct(arr + --siz); }
		void erase(int pos, int cnt = 1)
		{
			cnt = min(siz - pos, cnt);
			if (!cnt)
				return;
			destruct(arr + pos, arr + pos + cnt);
			mov(arr + pos, arr + pos + cnt, arr + siz);
			//注意到，对于类类型会产生siz和siz-1位置两个bit完全一致的版本
			//但是这并不会有什么问题，因为不可能再对siz位置的对象调用析构函数
			siz -= cnt;
		}
		void insert(int pos, K key) //insert and be there
		{
			expand();
			mov(arr + pos + 1, arr + pos, arr + siz);
			construct(arr + pos, std::move(key));
		}
		void reserve(int nCap)
		{
			nCap = calc(nCap);
			if (nCap > cap)
				realloc(nCap);
		}
		K *begin() { return arr; }
		K *end() { return arr + siz; }
		const K *begin() const { return arr; }
		const K *end() const { return arr + siz; }
		K &front() { return *arr; }
		K &back() { return *(arr + siz - 1); }
		const K &front() const { return *arr; }
		const K &back() const { return *(arr + siz - 1); }
		bool empty() const { return siz == 0; }
		int size() const { return siz; }
		int capacity() const { return cap; }
		K &operator[](int pos) { return arr[pos]; }
		const K &operator[](int pos) const { return arr[pos]; }
		Vector(int size = 0)
			: siz(size), cap(max(calc(siz),InitialCap)), arr((K *)malloc(cap * sizeof(K)))
		{
			for (K *cur = arr, *last = arr + siz; cur != last; ++cur)
				construct(cur, K());
		}
		void swap(Vector &rhs) noexcept
		{
			std::swap(arr, rhs.arr);
			std::swap(siz, rhs.siz);
			std::swap(cap, rhs.cap);
		}
		Vector &operator=(Vector rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		Vector(const Vector &rhs) : arr(nullptr)
		{
			reserve(rhs.siz);
			siz = rhs.siz;
			cpy(arr, rhs.begin(), rhs.end());
		}
		Vector &operator=(Vector &&rhs) noexcept
		{
			if (this != &rhs)
			{
				Vector tmp(std::move(rhs));
				swap(tmp);
			}
			return *this;
		}
		Vector(Vector &&rhs) noexcept
		{
			arr = rhs.arr;
			cap = rhs.cap, siz = rhs.siz;
			rhs.arr = nullptr;
		}
		Vector(const std::initializer_list<K> &rhs)
		{
			reserve(rhs.size());
			siz = rhs.size();
			cpy(arr, rhs.begin(), rhs.end());
		}
		~Vector()
		{
			if (arr)
			{
				destruct(arr, arr + siz);
				free(arr);
			}
		}
	};
}