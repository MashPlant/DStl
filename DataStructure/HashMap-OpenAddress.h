#pragma once
#include "HashMapUtil.h"
#include <utility>
#include <stdexcept>
#include <cstdlib>

namespace ds
{
	//����stl��unordered_map����,�����Լ�����
	template <typename K, typename V, typename OpenAddressT>
	class HashMap<K, V, OpenAddressT, true>
	{
		typedef typename OpenAddressT::CodeType CodeType;
		typedef std::pair< K, V> ValueType;
		struct Node
		{
			enum State { empty, used, deleted };
			ValueType value;
			int state;
			bool terminal() const { return state == empty; } //һ��empty��Node��ζ�����е���ֹ��deleted����ֻ��һ����
			ValueType *operator->() { return &value; }
			Node(const ValueType &value) :value(value), state(used) {}
			//��һ���ƶ�����Ŀ�,ԭ��������д��:
			//Node(ValueType &&value) noexcept :value(std::move(value)), state(used) {}
			//ÿ�ε���Node���ƶ������ʱ����Ȼд��value(std::move(value))��������ѡ��K(����string)��û���ƶ�
			//������ΪValueType��pair<const K,V>,���Եõ���һ��const K &&�����޷�ƥ��K &&�����ǻ�ƥ��const K &,���Է����˸���
			//��ô����һ��const_cast����
			Node(ValueType &&value) noexcept : value(std::move(const_cast<K &>(value.first)),std::move(value.second)), state(used) {}
		};
		Node *nextNode(Node *x) const
		{
			Node *last = table + capacity;
			if (x++ == last) //end()��++����end()
				return last;
			while (x != last && x->state != Node::used)
				++x;
			return x;
		}
		Node *prevNode(Node *x) const
		{
			Node *tmp = x, *first = table - 1;
			--x; //���������x������Խ��first,���ǿ���Խ��last,������������
			while (x != first && x->state != Node::used)
				--x;
			if (x->state != Node::used) //begin()��--����begin()
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
			//Ϊ�˱�֤̽������ȡ��[0,capacity)�����ﲻ������λ+���֮��ĸ��Ų���(������������ڹ�ϣ��������)
			//���Ҷ�ɢ�к�����ȡֵҲ��Ҫ��
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
						insertUnchecked(std::move(otbl[i].value));
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
		void insert(const ValueType &value)
		{
			if (insertUnchecked(value).second)
				++size_;
			rehash();
		}
		void insert(ValueType &&value)
		{
			if (insertUnchecked(std::move(value)).second)
				++size_;
			rehash();
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
