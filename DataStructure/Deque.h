#pragma once
#include "Algorithm.h"

namespace ds
{
	template <typename T,int BlockSize = 4096>
	class Deque
	{
		using Block = T *;
		const static int InitialCap = 16;
		Block *blocks = nullptr; //不采用循环队列，而是简单地从中间开始向两边拓展，毕竟扩容的代价是很小的
		int firstBlock = 0, lastBlock = 0;
		int blockCap = 0;
		int head = 0, tail = 0; 
		//lastBlock指向past end元素所在的块，而非最后一个块之后的块
		//规定：tail==0当且仅当没有任何元素，这也将是empty函数的依据
		//只要有元素，tail的值必须在[1,BlockSize]之间
		template <typename P>
		static P *alloc(int size) { return reinterpret_cast<P *>(malloc(size * sizeof(P))); }
		template <typename P>
		static void dealloc(P *ptr) { free(ptr); }
		template <typename P>
		static void destroy(P *ptr,int first,int last)
		{
			if (!std::is_trivially_destructible<P>::value)
				for (P *cur = ptr + first, *end = ptr + last; cur != end; ++cur)
					cur->~P();
		}
		template <typename P>
		static void destroy(P *ptr) { ptr->~P(); }
		template <typename P,typename V>
		static void construct(P *ptr, V &&value) { new(ptr) typename std::decay<V>::type(std::forward<V>(value)); }
		void expand()
		{
			Block *nBlocks = alloc<Block>(blockCap <<= 1);
			//int mid = (firstBlock + lastBlock) / 2; //把指针调整到新申请的内存的中间，这不是必要的操作，只是直觉上性能会好一些
			memcpy(nBlocks + firstBlock + blockCap / 4 , blocks + firstBlock,
				sizeof(Block) * (lastBlock - firstBlock + 1));
			dealloc(blocks);
			blocks = nBlocks;
			firstBlock += blockCap / 4 , lastBlock += blockCap / 4;
		}

	public:
		Deque()
		{
			blocks = alloc<Block>(blockCap = InitialCap);
			firstBlock = lastBlock = blockCap / 2;
			blocks[firstBlock] = alloc<T>(BlockSize);
		}
		void swap(Deque &other) noexcept { ds::bitwiseSwap(this, &other); }
		Deque(const Deque &src)
		{
			blocks = alloc<Block>(blockCap = src.blockCap);
			firstBlock = src.firstBlock, lastBlock = src.lastBlock;
			head = src.head, tail = src.tail;
			for (int i = firstBlock; i <= lastBlock; ++i)
			{
				blocks[i] = alloc<T>(BlockSize);
				for (int j = (i == firstBlock ? head : 0); j < (i == lastBlock ? tail : BlockSize); ++j)
					construct(&blocks[i][j], src.blocks[i][j]);
			}
		}
		Deque(Deque &&src) noexcept
		{
			memcpy(this, &src, sizeof(Deque));
			memset(&src, 0, sizeof(Deque));
		}
		Deque &operator=(const Deque &src)
		{
			if (this != &src)
			{
				Deque tmp(src);
				swap(tmp);
			}
			return *this;
		}
		Deque &operator=(Deque &&src) noexcept
		{
			if (this != &src)
			{
				Deque tmp(std::move(src));
				swap(tmp);
			}
			return *this;
		}
		~Deque()
		{
			if (!blocks)
				return;
			for (int i = firstBlock; i <= lastBlock; ++i)
			{
				destroy(blocks[i], i == firstBlock ? head : 0, i == lastBlock ? tail : BlockSize);
				dealloc(blocks[i]);
			}
			dealloc(blocks);
		}
		int size() const { return tail - head + BlockSize * (lastBlock - firstBlock); }
		bool empty() const { return tail == 0; }
		T &operator[](int pos)
		{
			return blocks[firstBlock + (head + pos) / BlockSize][(head + pos) % BlockSize];
		}
		void push_back(const T &value)
		{
			if (lastBlock == blockCap - 1 && tail == BlockSize)
				expand();
			if (tail == BlockSize)
			{
				blocks[++lastBlock] = alloc<T>(BlockSize);
				blocks[lastBlock][0] = value;
				tail = 1;
			}
			else
				blocks[lastBlock][tail++] = value;
		}
		void pop_back() //assert(tail!=0)
		{
			if (tail == 1)
			{
				destroy(&blocks[lastBlock][0]);
				dealloc(blocks[lastBlock--]);
				tail = BlockSize;
			}
			else
				destroy(&blocks[lastBlock][--tail]);
		}
		void push_front(const T &value)
		{
			if (firstBlock == 0 && head == 0)
				expand();
			if (head == 0)
			{
				blocks[--firstBlock] = alloc<T>(BlockSize);
				blocks[firstBlock][head = BlockSize - 1] = value;
			}
			else
				blocks[firstBlock][--head] = value;
		}
		void pop_front()
		{
			if (head == BlockSize - 1)
			{
				destroy(&blocks[firstBlock][BlockSize - 1]);
				dealloc(blocks[firstBlock++]);
				head = 0;
			}
			else
				destroy(&blocks[firstBlock][head++]);
		}
		class iterator
		{
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef T value_type;
			typedef int difference_type;
			typedef T *pointer;
			typedef T &reference;
			typedef const T &const_reference;
			typedef const T *const_pointer;
		private:
			Block *which;
			int where;
		public:
			iterator(Block *which, int where) :which(which), where(where) {}
			T &operator*() { return (*which)[where]; }
			T *operator->() { return (*which) + where; }
			iterator &operator+=(int shift)
			{
				which += (where + shift) / BlockSize;
				where = (where + shift) % BlockSize;
				return *this;
			}
			iterator operator+(int shift) const { iterator ret(*this); return ret += shift; }
			iterator &operator-=(int shift)
			{
				which += (where - shift) / BlockSize;
				where = (where - shift) % BlockSize;
				if (where < 0) where += BlockSize, --which;
				return *this;
			}
			iterator operator-(int shift) const { iterator ret(*this); return ret -= shift; }
			int operator-(iterator rhs) const { return where - rhs.where + BlockSize * (which - rhs.which); }
			iterator &operator++()
			{
				if (where == BlockSize - 1)
					++which, where = 0;
				else
					++where;
				return *this;
			}
			iterator &operator--()
			{
				if (where == 0)
					--which, where = BlockSize - 1;
				else
					--where;
				return *this;
			}
			iterator operator++(int) { iterator ret(*this); ++*this; return ret; }
			iterator operator--(int) { iterator ret(*this); --*this; return ret; }
			bool operator<(iterator rhs) const { return which != rhs.which ? which < rhs.which : where < rhs.where; }
			bool operator>(iterator rhs) const { return which != rhs.which ? which > rhs.which : where > rhs.where; }
			bool operator<=(iterator rhs) const { return *this - rhs <= 0; }
			bool operator>=(iterator rhs) const { return *this - rhs >= 0; }
			bool operator==(iterator rhs) const { return where == rhs.where && which == rhs.which; }
			bool operator!=(iterator rhs) const { return where != rhs.where || which != rhs.which; }
		};
		iterator begin() { return iterator(blocks + firstBlock, head); }
		//直接传tail，tail可能为BlockSize，但是正常的迭代器不可能是BlockSize，从而永远不等
		iterator end() { return ++iterator(blocks + lastBlock, tail - 1); }
	};
}
