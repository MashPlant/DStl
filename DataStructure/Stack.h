#pragma once
#include "Vector.h"

namespace ds
{
	template<typename K,typename Container = Vector<K>>
	class Stack : public Container
	{
	private:
		using Container::operator[];
		using Container::erase;
		using Container::insert;
		using Container::push_back;
		using Container::pop_back;
		using Container::begin;
		using Container::end;
		using Container::front;
		using Container::back;
	public:
		const K & top()const { return back(); }
		void pop() { pop_back(); }
		void push(const K &key) { push_back(key); }
	};
}
