#pragma once
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
			Slot *next;
		};
		Slot *curBlock = nullptr, *freeSlot = nullptr;//freeSlot������Ǳ�deallocate�ͷŵ��ڴ�����
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
				//��������Ͽ�����ͬʱʹ����union��������Ա(�Ȱ�freeSlot����pointer������ֶ�ȡ������next)
				//����ʵ����"����pointer���"��û��ʹ��data��ֻ�Ǹ��˱���ʹ��data��Ȩ�����ѣ�data����Ҫ����������
				//��Ҳ����allocate�൱��malloc��ԭ��ֱ��������ڴ��ǲ����õ�
				Slot *ret = freeSlot;
				freeSlot = freeSlot->next;
				return reinterpret_cast<pointer>(ret);
			}
			if (pos == BlockSize || curBlock == nullptr)
			{
				Slot *tmp = curBlock; 
				curBlock = reinterpret_cast<Slot *>(malloc(BlockSize * sizeof(Slot)));
				curBlock->next = tmp; //��Իnext����ʵָ�����֮ǰ������ڴ�
				pos = 1;
			}
			//��1��ʼʹ���ڴ棬�����ƻ�curBlock�ڴ�ŵ�next(�������ν��overhead)
			return reinterpret_cast<pointer>(curBlock + pos++); //һ��block���ڴ���������
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
				tmp->next = freeSlot; //��Իnext����ʵָ�����֮ǰ���ص��ڴ�
				freeSlot = tmp; 
			}
		}
		static void destroy(pointer p)
		{
			p->~value_type();
		}
		MemPool() noexcept {  }
		//�����Ͽ������и��ֹ��죬��ʵ�������κ���
		template <typename K_, int BlockSize_>
		MemPool(const MemPool<K_, BlockSize_>&) noexcept { }

		//����ĳ�����޷�����ԭ��΢��ı���������stl�����Ĺ��캯��������allocator������һ��_Container_proxy
		//�����Ǹ�allocator���ں����ڴ����ģ��ܿ�������ˡ����ִ�����������Ǹ�proxy�ͻᱻ�ͷŵ����Ӷ��ҵ�
//		~MemPool() 
//		{
//			std::cout << typeid(K).name();
//			while (curBlock)
//			{
//				Slot *tmp = curBlock->next;
//				free(curBlock);
//				curBlock = tmp;
//			}
//		}
	};
}