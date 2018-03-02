#include "stdafx.h"
#include <iostream>
#include "ForwardList.h"
#include "Util.h"
#include <forward_list>
#include "algorithm"
#include "Profiler.h"
#include "SegmentTreeUtil.h"
#include "Algorithm.h"
#include "Ptr32.h"
#include <list>
#include "MemPool.h"
#include "Vector.h"
#include "Map.h"
#include <map>
using namespace std;
using namespace ds;

template <typename K>
class MyAlloc
{
private:
	union Data
	{
		K k;
		Data *next;
	};
	Data *pool = reinterpret_cast<Data *>(malloc(sizeof(Data) * 1024));
	int pos = 1;
public:
	typedef K value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	template <typename U>
	struct rebind { typedef MyAlloc<U> other; };
	pointer allocate(size_type = 1, const_pointer = nullptr)
	{
		pool->next = nullptr;
		return reinterpret_cast<K *>(pool + pos++);
	}
	void deallocate(pointer p,size_type)
	{
		p->~value_type();
	}
	template <typename ...Args>
	void construct(pointer p, Args&& ...args)
	{
		new(p) value_type(std::forward<Args>(args)...);
	}
	void destroy(pointer p)
	{
		p->~value_type();
	}
	MyAlloc() {}
	template <typename U>
	MyAlloc(const MyAlloc<U> &)
	{
		int a = 0;
	}

	template <typename U>
	MyAlloc& operator=(const MyAlloc<U> &)
	{
		return *this;
	}

	template <typename U>
	MyAlloc(MyAlloc<U> &&) noexcept {}

	template <typename U>
	MyAlloc& operator=(MyAlloc<U> &&) noexcept { return *this; }

	~MyAlloc() noexcept {}
};

int main()
{
	
	getchar();
}