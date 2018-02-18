#pragma once
#include "Ptr32.h"
#include  <type_traits>
namespace ds
{
	//WARNING!!! 这个内存池能且仅能用于处理单个对象!!!vector什么的别想了
	//在VS里debug下跑的时候所有迭代器操作都会崩溃，我实在不知道为什么
	template <typename K, int BlockSize, bool EnablePtr32 = false>
	class MemPool
	{
		union Slot;
		typedef typename std::conditional<EnablePtr32 && sizeof(K) < sizeof(void*), Slot *, Slot *>::type SlotPointer;
		template <typename U>
		static SlotPointer getSlotPtr(U *p)
		{
			if (std::is_same<SlotPointer, Slot*>::value)
				return reinterpret_cast<Slot *>(p);
			return Ptr32<Slot>(reinterpret_cast<Slot*>(p));
		}
		union Slot
		{
			K data;
			SlotPointer	next;
		};
		SlotPointer currBlock, currSlot, lastSlot, freeSlot;
		//currSlot和lastSlot都是currBlock内的指针，所以用下标代替亦可
		//freeSlot储存的是被deallocate释放的内存链表
		template <typename K_, int BlockSize_, bool EnablePtr32_>
		static void init(MemPool<K_, BlockSize_, EnablePtr32_> &m)
		{
			m.currBlock = m.currSlot = m.lastSlot = m.freeSlot = nullptr;
		}
	public:
		typedef K value_type;
		typedef value_type *pointer;
		typedef value_type &reference;
		typedef const value_type *const_pointer;
		typedef const value_type &const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		template <typename U>
		struct rebind { typedef MemPool<U, BlockSize, EnablePtr32> other; };

		pointer allocate(size_type = 1, const_pointer = nullptr)
		{
			if (freeSlot != nullptr) 
			{
				//这里表面上看起来同时使用了union的两个成员(既把freeSlot按照pointer解读，又读取了它的next)
				//但是实际上"按照pointer解读"并没有使用data，只是给了别人使用data的权力而已，data还需要别人来构造
				//这也就是allocate相当于malloc的原因，直接申请的内存是不可用的
				SlotPointer ret = freeSlot;
				freeSlot = freeSlot->next;
				return reinterpret_cast<pointer>(ret);
			}
			if (currSlot == lastSlot)
			{
				SlotPointer tmp = currBlock; //之所以不直接写currBlock->next = currBlock是为了避免currBlock为nullptr的情况
				currBlock = getSlotPtr(malloc(BlockSize * sizeof(Slot)));
				currBlock->next = tmp; //虽曰next，其实指向的是之前申请的内存
				currSlot = currBlock + 1, lastSlot = currBlock + BlockSize;
				//注意+1非常重要，不能破坏currBlock内存放的next(这就是所谓的overhead)
			}
			return reinterpret_cast<pointer>(currSlot++); //一个block中内存是连续的
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
				SlotPointer tmp = getSlotPtr(p);
				tmp->next = freeSlot; //虽曰next，其实指向的是之前返回的内存
				freeSlot = tmp; 
			}
		}
		static void destroy(pointer p)
		{
			p->~value_type();
		}
		template<typename ...Args>
		value_type *newElem(Args&&... args)
		{
			value_type *ret = allocate();
			construct(ret, std::forward<Args>(args)...);
			return ret;
		}
		void delElem(pointer p)
		{
			deallocate(p);
			destroy(p);
		}
		MemPool() noexcept { init(*this); }
		//表面上看起来有各种构造，其实并不做任何事
		template <typename K_, int BlockSize_, bool EnablePtr32_>
		MemPool(const MemPool<K_, BlockSize_, EnablePtr32_>&) noexcept { init(*this); }
		template <typename K_, int BlockSize_, bool EnablePtr32_>
		MemPool& operator=(const MemPool<K_, BlockSize_, EnablePtr32_>&) noexcept { init(*this); return *this; }

		~MemPool() noexcept
		{
			while (currBlock)
			{
				SlotPointer tmp = currBlock->next;
				free(currBlock);
				currBlock = tmp;
			}
		}
		MemPool(MemPool &&rhs) noexcept
		{
			currBlock = rhs.currBlock;
			currSlot = rhs.currSlot;
			lastSlot = rhs.lastSlot;
			freeSlot = rhs.freeSlot;
			init(rhs);
		}
		MemPool& operator=(MemPool &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~MemPool();
				new(this) MemPool(std::move(rhs));
			}
			return *this;
		}
	};
}