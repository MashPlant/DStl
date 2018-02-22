#pragma once
#include "Ptr32.h"
#include  <type_traits>
namespace ds
{
	//WARNING!!! ����ڴ�����ҽ������ڴ���������!!!vectorʲô�ı�����
	//��VS��debug���ܵ�ʱ�����е��������������������ʵ�ڲ�֪��Ϊʲô
	template <typename K, int BlockSize>
	class MemPool
	{
		union Slot
		{
			K data;
			Slot *	next;
		};
		template <typename U>
		static Slot *getSlotPtr(U *p)
		{
			if (std::is_same<Slot *, Slot*>::value)
				return reinterpret_cast<Slot *>(p);
			return Ptr32<Slot>(reinterpret_cast<Slot*>(p));
		}
		Slot * currBlock, currSlot, lastSlot, freeSlot;
		//currSlot��lastSlot����currBlock�ڵ�ָ�룬�������±�������
		//freeSlot������Ǳ�deallocate�ͷŵ��ڴ�����
		template <typename K_, int BlockSize_>
		static void init(MemPool<K_, BlockSize_> &m)
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
		struct rebind { typedef MemPool<U, BlockSize> other; };

		pointer allocate(size_type = 1, const_pointer = nullptr)
		{
			if (freeSlot != nullptr) 
			{
				//��������Ͽ�����ͬʱʹ����union��������Ա(�Ȱ�freeSlot����pointer������ֶ�ȡ������next)
				//����ʵ����"����pointer���"��û��ʹ��data��ֻ�Ǹ��˱���ʹ��data��Ȩ�����ѣ�data����Ҫ����������
				//��Ҳ����allocate�൱��malloc��ԭ��ֱ��������ڴ��ǲ����õ�
				Slot *ret = freeSlot;
				freeSlot = freeSlot->next;
				return reinterpret_cast<pointer>(ret);
			}
			if (currSlot == lastSlot)
			{
				Slot * tmp = currBlock; //֮���Բ�ֱ��дcurrBlock->next = currBlock��Ϊ�˱���currBlockΪnullptr�����
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
				Slot * tmp = getSlotPtr(p);
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
		template <typename K_, int BlockSize_>
		MemPool(const MemPool<K_, BlockSize_>&) noexcept { init(*this); }
		template <typename K_, int BlockSize_>
		MemPool& operator=(const MemPool<K_, BlockSize_>&) noexcept { init(*this); return *this; }
		//�ƶ����챻��ɾ���ˣ���Ϊ��������������������RE����CE
		~MemPool() noexcept
		{
			while (currBlock)
			{
				Slot *tmp = currBlock->next;
				free(currBlock);
				currBlock = tmp;
			}
		}
	};
}