#pragma once
#include <cassert>

namespace ds
{
	/*template<typename K,K *Array>  失败的尝试
	struct ArrayTag //node 本身就是int ？ or ？
	{

	};*/
	struct PointerTag {};

	template<typename Tag>
	struct IntrusiveListNode;

	template<>
	struct IntrusiveListNode<PointerTag>
	{
		typedef IntrusiveListNode* pointer;
		pointer prev;
		pointer next;
		pointer operator->() { return this; } //按道理来说应该是node.left/right，然而由于.操作符不可重载，会给数组下标版的带来麻烦，所以同意用->
		operator pointer() { return this; }
		bool operator==(const IntrusiveListNode &rhs) const { return prev == rhs.prev&&next == rhs.next; }
		bool operator!=(const IntrusiveListNode &rhs) const { return !(operator==(rhs)); }
	};

	template<typename K>
	class IntrusiveList
	{
#define listEntry(ptr) (K*)((char *)ptr - offsetOf(K,node))
#define offsetOf(TYPE, member) ((size_t)& ((TYPE *)0)->member) 
		//其实这东西在C语言的库里面就有...还要改一下名字免得重了
		typedef decltype(K().node) Node;
		typedef typename Node::pointer pointer;
		K nil;
		int size = 0;
		//auto deref(pointer ptr)->decltype(Node::deref(0)) { return Node::deref(ptr); } //仅仅是为了不用每次都写Node::
	public:
		IntrusiveList() { nil.node->next = nil.node->prev = nil.node; }
		void push_back(K &key) //显然右值之类的东西不能用于侵入式数据结构
		{
			key.node->next = nil.node, key.node->prev = nil.node->prev;
			nil.node->prev->next = key.node, nil.node->prev = key.node;
		}
		void push_front(K &key)
		{
			key.node->prev = nil.node, key.node->next = nil.node->next;
			nil.node->next->prev = key.node, nil.node->next = key.node;
		}
		void erase(K &key)
		{
			assert(key.node != nil);
			key.node->next->prev = key.node->prev;
			key.node->prev->next = key.node->next;
		}
		static void insert(K &where, K &key) { insert(where.node, key); }
		static void insert(Node node, K &key) //insert before
		{
			key.node->next = node, key.node->prev = node->prev;
			node->prev->next = key.node, node->prev = key.node;
		}
		static K& entryOf(Node node) { return *listEntry(pointer(node)); }
		static K& entryOf(pointer ptr) { return *listEntry(ptr); }
		static K& inc(K &key) { return key = entryOf(key.node->next); }
		static K& dec(K &key) { return key = entryOf(key.node->prev); }
		static K& next(const K &key) { return entryOf(key.node->next); }
		static K& prev(const K &key) { return entryOf(key.node->prev); }
		K &front() { return *listEntry(nil.node->next); }
		K &back() { return *listEntry(nil.node->prev); }
		const K &front() const { return *listEntry(nil.node->next); }
		const K &back() const { return *listEntry(nil.node->prev); }
		K &begin() { return front(); }
		K &end() { return nil; }
		const K &begin() const { return front(); }
		const K &end() const { return nil; }
	};

}

//demo
/*struct ListedInt
{
int val;
bool operator!=(const ListedInt &rhs) const { return node != rhs.node; }
int& operator*() { return val; }
ListedInt& operator++() { return IntrusiveList<ListedInt>::inc(*this); }
IntrusiveListNode<PointerTag> node;
};
ostream& operator<<(ostream &os,const ListedInt &x)
{
os << x.val;
return os;
}
ListedInt arr[1000];
IntrusiveList<ListedInt> list; */