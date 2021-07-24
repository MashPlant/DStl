#pragma once
#include "HashMapUtil.h"
#include <utility>
#include <cstdlib>
#include "Algorithm.h"

namespace ds
{
	//不与stl的unordered_map兼容,纯粹自己发挥
	template <typename K, typename V, typename OpenAddressT>
	class HashMap<K, V, OpenAddressT, true>
	{
		typedef typename OpenAddressT::CodeType CodeType;
		typedef std::pair<const K, V> ValueType;
		typedef std::pair<K, V> InputType;
		struct Node
		{
			enum State { empty, used, deleted };
			ValueType value;
			int state;
			bool terminal() const { return state == empty; } //一个empty的Node意味着序列的终止，deleted的则只是一个洞
			ValueType *operator->() { return &value; }
			Node(const ValueType &value) :value(value), state(used) {}
			//记一个移动构造的坑,原来是这样写的:
			//Node(ValueType &&value) noexcept :value(std::move(value)), state(used) {}
			//每次调用Node的移动构造的时候，虽然写了value(std::move(value))，但是我选的K(比如string)并没有移动
			//这是因为ValueType是pair<const K,V>,所以得到了一个const K &&，它无法匹配K &&，而是会匹配const K &,所以发生了复制
			//const_cast似乎是UB，不用了；rehash的时候，我memcpy谁还能拦得住吗?
			Node(InputType &&value) noexcept : value(std::move(value.first),std::move(value.second)), state(used) {}
		};
		Node *nextNode(Node *x) const
		{
			Node *last = table + capacity;
			if (x++ == last) //end()再++仍是end()
				return last;
			while (x != last && x->state != Node::used)
				++x;
			return x;
		}
		Node *prevNode(Node *x) const
		{
			Node *tmp = x, *first = table - 1;
			--x; //正常情况下x不可能越过first,但是可以越过last,所以这样特判
			while (x != first && x->state != Node::used)
				--x;
			if (x->state != Node::used) //begin()再--仍是begin()
				return tmp;
			return x;
		}
	public:
		class iterator
		{
			HashMap *container;
			Node *self;
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef std::pair<const K, V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
			pointer operator->() { return &self->value; }
			reference operator*() { return self->value; }
			iterator& operator++() { self = container->nextNode(self); return *this; }
			iterator operator++(int) { iterator tmp = *this; operator++(); return tmp; }
			iterator& operator--() { self = container->prevNode(self); return *this; }
			iterator operator--(int) { iterator tmp = *this; operator--(); return tmp; }
			bool operator==(const iterator &rhs) const { return self == rhs.self; }
			bool operator!=(const iterator &rhs) const { return self != rhs.self; }
			iterator(Node *self, HashMap *container) :self(self), container(container) {}
		};
	private:
		Node *table;
		int size_ = 0, capacity = 0;
		float maxFactor = 0.75f;
		CodeType hashCode(CodeType raw) const
		{
			//为了保证探查序列取遍[0,capacity)，这里不能做移位+异或之类的干扰操作(这个操作可以在哈希函数里做)
			//而且对散列函数的取值也有要求
			return raw & (capacity - 1);
		}
		void rehash()
		{
			if (capacity * maxFactor < size_)
			{
				Node *otbl = table;
				int ocap = capacity;
				table = reinterpret_cast<Node*>(malloc((capacity <<= 1) * sizeof(Node)));
				memset(table, 0, capacity * sizeof(Node));
				for (int i = 0; i < ocap; ++i)
					if (otbl[i].state == Node::used)
					{
						OpenAddressT hasher(otbl[i]->first);
						auto h = hashCode(hasher.probe(0));
						for (int j = 0; !table[h].terminal() && j < capacity; h = hashCode(hasher.probe(++j)))
							; //这里没有必要再检查deleted和相等，都不可能发生
						memcpy(table + h, otbl + i, sizeof(Node));
					}
				free(otbl);
			}
		}
		template<typename P>
		std::pair<ValueType *,bool> insertUnchecked(P &&kv)
		{
			OpenAddressT hasher(kv.first);
			auto h = hashCode(hasher.probe(0));
			int free = -1;
			for (int i = 0; !table[h].terminal() && i < capacity; h = hashCode(hasher.probe(++i)))
			{
				if (table[h].state == Node::deleted && free == -1)
					free = h;
				else if (table[h]->first == kv.first)
					return { &table[h].value,false };
			}
			free = (free != -1) ? free : h;
			new(table + free) Node(std::forward<P>(kv));
			return { &table[free].value,true };
		}
	public:
		void insert(const InputType &value)
		{
			if (insertUnchecked(value).second)
				++size_;
			rehash();
		}
		void insert(InputType &&value)
		{
			if (insertUnchecked(std::move(value)).second)
				++size_;
			rehash();
		}
		template <typename ...Args,typename = typename std::enable_if<std::is_constructible<InputType,Args...>::value>::type>
		void insert(Args &&...args)
		{
			insert(InputType(std::forward<Args>(args)...));
		}
		V &operator[](const K &key)
		{
			auto pr = insertUnchecked(std::make_pair(key,V()));
			if (pr.second)
				++size_;
			return pr.first->second;
		}
		ValueType *find(const K&key) const
		{
			OpenAddressT hasher(key);
			auto h = hashCode(hasher.probe(0));
			for (int i = 0; !table[h].terminal() && i < capacity; h = hashCode(hasher.probe(++i)))
				if (table[h].state != Node::deleted && table[h]->first == key)
					return &(table[h].value);
			return nullptr;
		}
		void erase(const K &key)
		{
			OpenAddressT hasher(key);
			auto h = hashCode(hasher.probe(0));
			for (int i = 0; !table[h].terminal() && i < capacity; h = hashCode(hasher.probe(++i)))
				if (table[h].state != Node::deleted && table[h]->first == key)
				{
					--size_;
					table[h].~Node();
					table[h].state = Node::deleted;
					return;
				}
		}
		int size() const { return size_; }
		iterator begin() { return iterator(nextNode(table - 1) ,this); }
		iterator end() { return iterator(table + capacity, this); }
		void swap(HashMap &rhs) noexcept
		{
			ds::bitwiseSwap(this, &rhs);
		}
		HashMap(int capacity = 16) : capacity(capacity)
		{
			table = reinterpret_cast<Node*>(malloc(capacity * sizeof(Node)));
			memset(table, 0, capacity * sizeof(Node));
		}
		HashMap(const HashMap &rhs):size_(rhs.size_),capacity(rhs.capacity), maxFactor(rhs.maxFactor)
		{
			table = reinterpret_cast<Node*>(malloc(capacity * sizeof(Node)));
			memset(table, 0, capacity * sizeof(Node));
			for (int i = 0; i < capacity; ++i)
				if (rhs.table[i].state == Node::used)
					insertUnchecked(rhs.table[i].value);
		}
		HashMap(HashMap &&rhs) noexcept
		{
			if (this!=&rhs)
			{
				memcpy(this, &rhs, sizeof(HashMap));
				memset(&rhs, 0, sizeof(HashMap));
			}
		}
		HashMap& operator=(const HashMap &rhs)
		{
			if (this != &rhs)
			{
				HashMap tmp(rhs);
				swap(tmp);
			}
			return *this;
		}
		HashMap& operator=(HashMap &&rhs) noexcept
		{
			if (this != &rhs)
			{
				HashMap tmp(std::move(rhs));
				swap(tmp);
			}
			return *this;
		}
		~HashMap()
		{
			if (!std::is_trivially_destructible<ValueType>::value)
				for (int i = 0; i < capacity; ++i)
					if (table[i].state == Node::used)
						table[i].~Node();
			free(table);
		}
	};
}
