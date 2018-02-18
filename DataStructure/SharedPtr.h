#pragma once
#include <type_traits>
#include <cassert>

namespace ds
{
	//侵入式的智能指针
	//需要多继承请自觉开virtual
	struct EnableShared
	{
		int _cnt_ = 1;
	};

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
	//每个SharedPtr配一个deleter太浪费了，不支持
	template <typename K,typename Delete = DefaultDelete>
	class SharedPtr
	{
		static_assert(std::is_base_of<EnableShared, K>::value,
			"K must derive from EnableShared");
		static_assert(std::is_class<Delete>::value,
			"SharedPtr only support struct with operator() as its deleter\nfunction and lambda are not accepted");
	private:
		K *ptr;
		const static Delete deleter;
		void tryDel()
		{
			if (ptr && --(ptr->_cnt_) == 0)
				deleter(ptr);
		}
	public:
		SharedPtr(K *ptr = nullptr) :ptr(ptr) {}
		template <typename U, typename = typename std::enable_if<std::is_base_of<K, U>::value>::type>
		SharedPtr(const SharedPtr<U, Delete> &rhs)
		{
			ptr = rhs.ptr;
			++ptr->_cnt_;
		}
		template <typename U, typename = typename std::enable_if<std::is_base_of<K, U>::value>::type>
		SharedPtr &operator=(const SharedPtr<U, Delete> &rhs)
		{
			if (this != &rhs)
			{
				tryDel();
				ptr = rhs.get();
				++ptr->_cnt_;
			}
			return *this;
		}
		template <typename U, typename = typename std::enable_if<std::is_base_of<K, U>::value>::type>
		SharedPtr(SharedPtr<U, Delete> &&rhs)
		{
			ptr = rhs.get();
			rhs.ptr = nullptr;
		}

		//注意区分const sharedptr 与 sharedptr<const> 
		//后者才是真正的const type *,所以const版本的重载仍返回无const的*和&
		K &operator*() { return *ptr; }
		K &operator*() const { return *ptr; }
		K *operator->() { return ptr; }
		K *operator->() const { return ptr; }
		K *get() { return ptr; }
		K *get() const { return ptr; }
		K *release() { tryDel(); return ptr; }
		void reset(K *ptr = nullptr) { tryDel(); this->ptr = ptr; }
		explicit operator bool() const { return ptr != nullptr; }
		K &operator[](int x) { return ptr[x]; } //stl没有提供这个
		~SharedPtr() { tryDel(); }
	};
	template <typename K, typename Delete>
	const Delete SharedPtr<K, Delete>::deleter = Delete();

	template<typename K, typename Delete = DefaultDelete, typename ...Args>
	SharedPtr<K, Delete> makeShared(Args&& ...args)
	{
		return new K(std::forward<Args>(args)...);
	}
}
