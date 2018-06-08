#pragma once
#include <iterator>
namespace ds
{
	//注意，非常依赖于编译器和具体的机器
	//这里假定64位的指针只有后32位非0，很多情况下(如微软的编译器下)这都是不成立的
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
		//注意，const版本的重载实际上是const pointer,不是pointer to const，所以直接返回K的指针或者引用
		//所以实际上不需要提供非const版本的重载
	private:
		uint32_t ptr = 0;
	};
}