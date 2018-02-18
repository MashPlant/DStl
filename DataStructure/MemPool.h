#pragma once
#include "Ptr32.h"
#include  <type_traits>
namespace ds
{
	//WARNING!!! ����ڴ�����ҽ������ڴ���������!!!vectorʲô�ı�����
	//��VS��debug���ܵ�ʱ�����е��������������������ʵ�ڲ�֪��Ϊʲô
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
		//currSlot��lastSlot����currBlock�ڵ�ָ�룬�������±�������
		//freeSlot������Ǳ�deallocate�ͷŵ��ڴ�����
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
				//��������Ͽ�����ͬʱʹ����union��������Ա(�Ȱ�freeSlot����pointer������ֶ�ȡ������next)
				//����ʵ����"����pointer���"��û��ʹ��data��ֻ�Ǹ��˱���ʹ��data��Ȩ�����ѣ�data����Ҫ����������
				//��Ҳ����allocate�൱��malloc��ԭ��ֱ��������ڴ��ǲ����õ�
				SlotPointer ret = freeSlot;
				freeSlot = freeSlot->next;
				return reinterpret_cast<pointer>(ret);
			}
			if (currSlot == lastSlot)
			{
				SlotPointer tmp = currBlock; //֮���Բ�ֱ��дcurrBlock->next = currBlock��Ϊ�˱���currBlockΪnullptr�����
				currBlock = getSlotPtr(malloc(BlockSize * sizeof(Slot)));
				currBlock->next = tmp; //��Իnext����ʵָ�����֮ǰ������ڴ�
				currSlot = currBlock + 1, lastSlot = currBlock + BlockSize;
				//ע��+1�ǳ���Ҫ�������ƻ�currBlock�ڴ�ŵ�next(�������ν��overhead)
			}
			return reinterpret_cast<pointer>(currSlot++); //һ��block���ڴ���������
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
				tmp->next = freeSlot; //��Իnext����ʵָ�����֮ǰ���ص��ڴ�
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
		//�����Ͽ������и��ֹ��죬��ʵ�������κ���
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