#pragma once
#include  <type_traits>
#ifdef _MSC_VER
#include <xutility>
#endif

namespace ds
{
	//这个内存池能且仅能用于处理单个对象
	template <typename K, int BlockSize = 4096>
	class MemPool
	{
#ifdef _MSC_VER
		static_assert(!std::is_same<K, std::_Container_proxy>::value,
			"Fucking MSVC use MemPool on STACK to allocate std::_Container_proxy, which will crash the program"\
			"There migth be something else like this, I can't guarantee");
#pragma warning("Don't use it on MSVC")
#endif

		union Slot
		{
			K data;
			Slot *next;
		};
		Slot *curBlock = nullptr, *freeSlot = nullptr;//freeSlot储存的是被deallocate释放的内存链表
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
		struct rebind { typedef MemPool<U, BlockSize> other; };

		pointer allocate(size_type = 1)
		{
			if (freeSlot != nullptr) 
			{
				//这里表面上看起来同时使用了union的两个成员(既把freeSlot按照pointer解读，又读取了它的next)
				//但是实际上"按照pointer解读"并没有使用data，只是给了别人使用data的权力而已，data还需要别人来构造
				//这也就是allocate相当于malloc的原因，直接申请的内存是不可用的
				Slot *ret = freeSlot;
				freeSlot = freeSlot->next;
				return reinterpret_cast<pointer>(ret);
			}
			if (pos == BlockSize || curBlock == nullptr)
			{
				Slot *tmp = curBlock; 
				curBlock = reinterpret_cast<Slot *>(malloc(BlockSize * sizeof(Slot)));
				curBlock->next = tmp; //虽曰next，其实指向的是之前申请的内存
				pos = 1;
			}
			//从1开始使用内存，不能破坏curBlock内存放的next(这就是所谓的overhead)
			return reinterpret_cast<pointer>(curBlock + pos++); //一个block中内存是连续的
		}
		template <typename ...Args>
		void construct(pointer p,Args&& ...args)
		{
			new(p) value_type(std::forward<Args>(args)...);
		}

		void deallocate(pointer p, size_type)
		{
			if (p)
			{
				Slot *tmp = reinterpret_cast<Slot *>(p);
				tmp->next = freeSlot; //虽曰next，其实指向的是之前返回的内存
				freeSlot = tmp; 
			}
		}
		static void destroy(pointer p)
		{
			p->~value_type();
		}
		MemPool() noexcept {  }
		//表面上看起来有各种构造，其实并不做任何事
		template <typename K_, int BlockSize_>
		MemPool(const MemPool<K_, BlockSize_>&) noexcept { }

		//出于某种我无法理解的原因，微软的编译器会在stl容器的构造函数中利用allocator来构造一个_Container_proxy
		//而且那个allocator是在函数内创建的，很快就析构了。如果执行析构函数那个proxy就会被释放掉，从而挂掉

		~MemPool() 
		{
			while (curBlock)
			{
				Slot *nxt = curBlock->next;
				free(curBlock);
				curBlock = nxt;
			}
		}
	};
	template <typename K1, int BlockSize1,typename K2, int BlockSize2>
	bool operator==(const MemPool<K1, BlockSize1> &, const MemPool<K2, BlockSize2> &) { return true; }
	template <typename K1, int BlockSize1, typename K2, int BlockSize2>
	bool operator!=(const MemPool<K1, BlockSize1> &, const MemPool<K2, BlockSize2> &) { return false; }

}