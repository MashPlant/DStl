#pragma once
#include <stdexcept>

namespace ds
{
	//没有哨兵节点的单向链表
	//与stl一样，没有保存size，额外提供了一个size()函数，O(n)的，正常情况下都不要用
	template <typename  K>
	class ForwardList
	{
	public:
		class iterator;
		friend iterator;
	private:
		struct Node
		{
			K key;
			Node *next;
			explicit Node(const K &key, Node *next = nullptr) :key(key), next(next) {}
		};
		Node *head = nullptr;
		static void extract_after(iterator pre1,iterator pre2)
		{
			Node *ext = pre2.nextNode();
			pre2.nextNode() = ext->next;
			ext->next = pre1.nextNode();
			pre1.nextNode() = ext;
		}
	public:
		class iterator
		{
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef K value_type;
			typedef int difference_type;
			typedef K *pointer;
			typedef K &reference;
			typedef const K &const_reference;
			typedef const K *const_pointer;
			friend ForwardList;
		private:
			Node *self;
			ForwardList *container;
			Node *&nextNode()  //注意返回值是引用
			{
				if (!self)
					return container->head;
				return self->next;
			}
		public:
			K & operator*() { return self->key; }
			K * operator->() { return &(self->key); }
			iterator& operator++()
			{
				self = self ? self->next : container->head; //这样可以自然构建循环链表，以时间换空间
				return *this;
			}
			iterator operator++(int) { iterator tmp(*this); operator++(); return tmp; }
			bool operator==(iterator rhs) const { return self == rhs.self; }
			bool operator!=(iterator rhs) const { return self != rhs.self; }
			iterator(Node *self,ForwardList *container) :self(self),container(container) {}
		};
		void push_front(const K &key)
		{ 
			head = new Node(key, head);
		}
		void pop_front()
		{
			Node *tmp = head->next;
			delete head;
			head = tmp;
		}
		iterator insert_after(Node *where,const K &key)
		{
			if (!where) //before begin
			{
				push_front(key);
				return { head,this };
			}
			Node *ins = new Node(key, where->next);
			where->next = ins;
			return { ins ,this };
		}
		iterator erase_after(Node *where) //返回where->next->next
		{
			if (!where) //before begin
			{
				pop_front();
				return { head ,this };
			}
			Node *era = where->next, ret = era->next; //显然不能erase_after最后一个元素
			delete era;
			where->next = ret;
			return { ret ,this };
		}
		template <typename Pred>
		void remove_if(Pred pred) 
		{
			if (pred(head))
			{
				pop_front();
				return;
			}
			Node *cur = head;
			while (cur->next)
				if (pred(cur->next->key))
				{
					Node *tmp = cur->next;
					cur->next = cur->next->next;
					delete tmp;
				}
				else
					cur = cur->next;
		}
		int size() const
		{
			Node *cur = head;
			int ret = 0;
			while (cur)
				++ret, cur = cur->next;
			return ret;
		}
		bool empty() const { return !head; }
		iterator begin() { return { head ,this }; }
		iterator end() { return { nullptr,this }; }
		iterator before_begin() { return end(); }
		void reverse()
		{
			if (!head||!head->next)
				return;
			Node *cur = head, *pre = nullptr;
			while (cur)
			{
				Node *tmp = cur->next;
				if (!tmp)
					head = cur; //末尾节点变成头
				cur->next = pre;
				pre = cur;
				cur = tmp;
			}
		}
		void merge(ForwardList &rhs) //归并，清空rhs的内容
		{
			iterator it1 = before_begin(), it2 = rhs.before_begin();
			auto cmp = [](iterator it1, iterator it2) {return it1.nextNode()->key < it2.nextNode()->key; };
			while (it1.nextNode() && it2.nextNode())
			{
				if (cmp(it2, it1))  
					extract_after(it1, it2); 
				else 
					++it1;
			}
			if (!it1.nextNode())
			{
				it1.nextNode() = it2.nextNode();
				it2.nextNode() = nullptr;
			}
		}
		void swap(ForwardList &rhs) noexcept
		{
			std::swap(head, rhs.head);
		}
		void sort()
		{
			if (head && head->next)
			{
				ForwardList carry;
				ForwardList counter[64];
				int fill = 0;
				while (head)
				{
					extract_after(carry.before_begin(), before_begin());
					int i = 0;
					while (i < fill && !counter[i].empty())
					{
						counter[i].merge(carry);
						carry.swap(counter[i++]);
					}
					carry.swap(counter[i]);
					if (i == fill)
						++fill;
				}
				for (int i = 1; i < fill; ++i)
					counter[i].merge(counter[i - 1]);
				swap(counter[fill - 1]);
			}
		}
		ForwardList() = default;
		ForwardList(const ForwardList &rhs)
		{
			if (!rhs.head)
				return;
			Node *pre = head = new Node(rhs.head->key),*cur = rhs.head->next;
			while (cur)
			{
				pre->next = new Node(cur->key);
				cur = cur->next;
				pre = pre->next;
			}
		}
		ForwardList &operator=(const ForwardList &rhs)
		{
			if (this!=&rhs)
			{
				ForwardList tmp(rhs);
				swap(tmp);
			}
			return *this;
		}
		ForwardList(ForwardList &&rhs) noexcept
		{
			head = rhs.head;
			rhs.head = nullptr;
		}
		ForwardList &operator=(ForwardList &&rhs) noexcept
		{
			if (this != &rhs)
			{
				ForwardList tmp(std::move(rhs));
				swap(tmp);
			}
			return *this;
		}
		~ForwardList()
		{
			Node *cur = head;
			while (cur)
			{
				Node *tmp = cur->next;
				delete cur;
				cur = tmp;
			}		
		}
	};
}
