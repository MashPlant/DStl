#pragma once
#include <iterator>
namespace ds
{
	//ע�⣬�ǳ������ڱ������;���Ļ���
	//����ٶ�64λ��ָ��ֻ�к�32λ��0���ܶ������(��΢��ı�������)�ⶼ�ǲ�������
	template <typename K>
	class Ptr32
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef K value_type;
		typedef int difference_type;
		typedef K *pointer;
		typedef K &reference;
		Ptr32(uint32_t ptr) : ptr(ptr) {}
		Ptr32(K *ptr64 = nullptr) :ptr(uint32_t(ptr64 - reinterpret_cast<K*>(nullptr))) {}
		Ptr32 &operator+=(int d) { return ptr += d, *this; }
		Ptr32 &operator-=(int d) { return operator+=(-d); }
		int operator-(const Ptr32 &rhs) const { return ptr - rhs.ptr; }
		Ptr32 operator+(int d)
		{
			Ptr32 tmp(ptr);
			return tmp += d;
		}
		Ptr32 operator-(int d)
		{
			Ptr32 tmp(ptr);
			return tmp -= d;
		}
		Ptr32 &operator++() { return operator+=(1); }
		Ptr32 operator++(int)
		{
			Ptr32 tmp(ptr);
			operator+=(1);
			return tmp;
		}
		Ptr32 &operator--() { return operator-=(1); }
		Ptr32 operator--(int)
		{
			Ptr32 tmp(ptr);
			operator-=(1);
			return tmp;
		}
		operator K *() const { return static_cast<K *>(nullptr) + ptr; }
		K &operator*() const { return **this; }
		K *operator->() const { return *this; }
		K &operator[](int index) const { return *(*this + index); }
		//ע�⣬const�汾������ʵ������const pointer,����pointer to const������ֱ�ӷ���K��ָ���������
		//����ʵ���ϲ���Ҫ�ṩ��const�汾������
	private:
		uint32_t ptr = 0;
	};
}