#pragma once
#include "Algorithm.h"
#include "Vector.h"
namespace ds
{
	template <typename K, typename Comp = std::less<K>,typename Container=Vector<K> >
	class PriorityQueue
	{
	public:
		typedef K value_type;
		typedef K *iterator;
		typedef K *pointer;
		typedef K &reference;
		typedef const K &const_reference;
		typedef const K *const_iterator;
		typedef const K *const_pointer;
	private:
		Comp comp;
		Container container;
	public:
		explicit PriorityQueue(Comp comp = Comp()) : comp(comp) {}
		reference operator[](int x) { return container[x]; }
		const_reference top() const { return container[0]; }
		void pop()
		{
			container[0] = container[container.size() - 1];
			container.pop_back();
			ds::fixHeap_(container.begin(), comp, 0, container.size());
		}
		void push(const K &key)
		{
			container.push_back(key);
			ds::push_heap(container.begin(), container.end(), comp);
		}
		void replaceFirst(const K &key)
		{
			container[0] = key;
			ds::fixHeap_(container.begin(), comp, 0, container.size());
		}
		bool empty()const { return container.empty(); }
		int size()const { return container.size(); }
	};
}