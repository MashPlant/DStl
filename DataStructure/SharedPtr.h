#pragma once
#include <type_traits>
#include <iostream>

namespace ds
{
	//侵入式的智能指针
	//只要使用它的类包含一个 _cnt_ 成员并且保证不修改它即可
	namespace detail
	{
		template <typename T,typename = decltype(std::declval<T>()._cnt_)>
		constexpr bool hasCnt(int) { return true; }
		template <typename T>
		constexpr bool hasCnt(float) { return true; }
	}

	struct DefaultDelete
	{
		template <typename K>
		void operator()(K *&p) const 
		{
			delete p; 
			p = nullptr;
		}
	};

	struct ArrayDelete 
	{
		template <typename K>
		void operator()(K *&p) const 
		{
			delete[]p; 
			p = nullptr;
		}
	};
	//没有stl那么多顾虑
	//每个SharedPtr配一个Del太浪费了，不支持
	template <typename K,typename Delete = DefaultDelete>
	struct SharedPtr
	{
		static_assert(detail::hasCnt<K>(0), "K must has field _cnt_");
		static_assert(std::is_class<Delete>::value,
			"SharedPtr only support struct with operator() as its deleter \n function and lambda are not accepted");
		K *ptr;
		const static Delete Del;
		void tryDel()
		{
			if (ptr && --(ptr->_cnt_) == 0)
				Del(ptr);
		}	
		//下面用模板写了拷贝构造/拷贝赋值
		//但是这 不会 覆盖掉编译器合成的构造/拷贝赋值
		//从而编译器合成的函数比模板函数的优先级更高，执行了合成的版本，导致错误
		//所以必须显式提供这两个函数(移动构造/赋值则不需要，因为此时编译器不会合成它们)
		SharedPtr(const SharedPtr &rhs) { ++(ptr = rhs.ptr)->_cnt_; }
		SharedPtr &operator=(const SharedPtr &rhs)
		{
			++rhs.ptr->_cnt_; 
			tryDel();
			ptr = rhs.ptr;
			return *this;
		}
		SharedPtr(K *ptr = nullptr) :ptr(ptr) { if (ptr) ptr->_cnt_ = 1; }
		template <typename U>
		void swap(SharedPtr<U, Delete> &rhs) noexcept { std::swap(ptr, rhs.ptr); }
		template <typename U>
		SharedPtr(const SharedPtr<U, Delete> &rhs) { ++(ptr = rhs.ptr)->_cnt_; }
		template <typename U>
		SharedPtr &operator=(const SharedPtr<U, Delete> &rhs)
		{
			++rhs.ptr->_cnt_; //先+再-，无需判断自赋值
			tryDel();
			ptr = rhs.ptr;
			return *this;
		}
		template <typename U>
		SharedPtr(SharedPtr<U, Delete> &&rhs) noexcept { ptr = rhs.ptr, rhs.ptr = nullptr; }
		template <typename U>
		SharedPtr& operator=(SharedPtr<U, Delete> &&rhs) noexcept
		{	
			SharedPtr<U, Delete> tmp(std::move(rhs));
			swap(tmp);
			return *this;
		}

		//注意区分const sharedptr 与 sharedptr<const> 
		//后者才是真正的const type *,所以const版本的重载仍返回无const的*和&
		K &operator*() { return *ptr; }
		K &operator*() const { return *ptr; }
		K *operator->() { return ptr; }
		K *operator->() const { return ptr; }
		K *get() const { return ptr; }
		void reset(K *ptr = nullptr) { tryDel(); this->ptr = ptr; }
		explicit operator bool() const { return ptr != nullptr; }
		K &operator[](int x) { return ptr[x]; } //stl没有提供这个
		~SharedPtr() { tryDel(); }
	};
	template <typename K,typename Delete>
	const Delete SharedPtr<K, Delete>::Del{};
	template<typename K, typename Delete = DefaultDelete, typename ...Args>
	SharedPtr<K, Delete> makeShared(Args&& ...args)
	{
		return new K(std::forward<Args>(args)...);
	}
}
