#pragma once
#include <iostream>
#include <functional>
#include "RBTree.h"
namespace ds
{
	template<typename K, typename V, typename Comp = std::less<K>, template<typename> typename TreeType = RBTree>
	class Map
	{
	private:
		struct Traits
		{
			typedef K KeyType;
			typedef std::pair<const K, V> ValueType;
			typedef Comp KeyCompare;
			static const KeyType& KeyOf(const ValueType &value) { return value.first; }
		};
		typedef TreeType<Traits> Tree;
		typedef typename Tree::Node Node;
		typedef std::pair<Node*, Node*> NodePair; // self_,parent
		Tree tree;
	public:
		typedef K key_type;
		typedef V mapped_type;
		typedef int size_type;
		typedef int difference_type;
		typedef std::pair<const K, V> value_type;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;
		typedef value_type &reference;
		typedef const value_type &const_reference;

		typedef std::function<mapped_type()> DefaultValue;
		DefaultValue defaultValue_;
		class const_iterator;
		class iterator
		{
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef int size_type;
			typedef int difference_type;
			typedef std::pair<const K, V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
		private:
			Node *self_;
			Tree *ptree;
		public:
			friend Map;
			friend const_iterator;
			reference operator*() const { return self_->value; }
			pointer operator->()const { return &(operator*()); }
			iterator& operator+=(int k)
			{
				int rk = -1;
				if (self_ != ptree->nil())
					rk = ptree->lower((*this)->first, false) + 1 + k;
				return *this = iterator(ptree->kth(rk), ptree);
			}
			iterator& operator-=(int k) { return operator+=(-k); }
			iterator operator+(int k) { iterator tmp = *this; return tmp += k; }
			iterator operator-(int k) { iterator tmp = *this; return tmp += -k; }
			int operator-(const_iterator rhs)
			{
				//assert ptree == rhs.ptree
				int rkl = ptree->size(), rkr = rkl; //统一把+1去掉了
				if (self_ != ptree->nil_)
					rkl = ptree->lower((*this)->first, false);
				if (rhs.self_ != ptree->nil_)
					rkr = ptree->lower(rhs->first, false);
				return rkl - rkr;
			}
			iterator& operator++()
			{
				self_ = ptree->nextNode(self_);
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;
				++*this;
				return tmp;
			}
			iterator& operator--()
			{
				self_ = ptree->prevNode(self_);
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
			iterator prev()const { return iterator(ptree->prevNode(self_), ptree); }
			iterator next()const { return iterator(ptree->nextNode(self_), ptree); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			iterator(Node *n, Tree *t) :self_(n), ptree(t) {}
		};
		class const_iterator
		{
		public:
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef int size_type;
			typedef int difference_type;
			typedef const std::pair<const K, V> value_type;
			typedef value_type *pointer;
			typedef const value_type *const_pointer;
			typedef value_type &reference;
			typedef const value_type &const_reference;
		private:
			Node *self_;
			const Tree *ptree;
		public:
			friend Map;
			const_iterator& operator+=(int k)
			{
				int rk = -1;
				if (self_ != ptree->nil())
					rk = ptree->lower((*this)->first, false) + 1 + k;
				return *this = const_iterator(ptree->kth(rk), ptree);
			}
			const_iterator& operator-=(int k) { return operator+=(-k); }
			const_iterator operator+(int k) { const_iterator tmp = *this; return tmp += k; }
			const_iterator operator-(int k) { const_iterator tmp = *this; return tmp += -k; }
			reference operator*() const { return self_->value; }
			pointer operator->()const { return &(operator*()); }
			const_iterator& operator++() { self_ = ptree->nextNode(self_); return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++*this; return tmp; }
			const_iterator& operator--() { self_ = ptree->prevNode(self_); return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; -*this; return tmp; }
			const_iterator prev()const { return const_iterator(ptree->prevNode(self_), ptree); }
			const_iterator next()const { return const_iterator(ptree->nextNode(self_), ptree); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			const_iterator(Node *n, const Tree *t) :self_(n), ptree(t) {}
			const_iterator(iterator it) :self_(it.self_), ptree(it.ptree) {}
		};

		explicit Map(Comp comp = Comp(), DefaultValue defaultValue_ = V())
			:tree(comp),
			defaultValue_(defaultValue_) {	}

		iterator find(const K& key) 
		{
			if (Node *x = tree.findNode(key).first)
				return iterator(x, &tree);
			return end();
		}
		const_iterator find(const K&key) const
		{
			if (Node *x = tree.findNode(key).first)
				return const_iterator(x, &tree);
			return end();
		}
		V& operator[](const K& key)
		{
			NodePair pn = tree.findNode(key);
			if (pn.first)
				return pn.first->value.second;
			return (tree.insert(pn.second, { key, defaultValue_() }))->value.second;
		}
		void erase(const K& key)
		{
			if (Node *x = tree.findNode(key).first)
				tree.erase(x);
		}
		void erase(iterator where)
		{
			if (where.self_ != tree.nil())
				tree.erase(where.self_);
		}
		//与stl统一，重复insert并不会更新节点的值
		iterator insert(const key_type& key, const mapped_type& val) { return insert({ key,val }); }
		iterator insert(const value_type &kvpair) { return iterator(tree.insert(kvpair), &tree); }
		int lower(const K &rend, bool afterEqual) const { return tree.lower(rend, afterEqual); }
		int between(int lend, int rend) const { return lower(rend, true) - lower(lend, false); }
		//k>size() or k<1时自动返回end()
		iterator kth(int k) { return iterator(tree.kth(k), &tree); }
		void clear() { tree.clear(); }
		iterator begin() { return iterator(tree.minChild(), &tree); }//root_为nil_时minChild返回nil_
		iterator end() { return iterator(tree.nil(), &tree); }
		const_iterator begin() const { return const_iterator(tree.minChild(), &tree); }//root_为nil_时minChild返回nil_
		const_iterator end() const { return const_iterator(tree.nil(), &tree); }
		int size()const { return tree.size(); }
		Map(Map &&rhs) noexcept :tree(std::move(rhs.tree)), defaultValue_(rhs.defaultValue_) {}
		Map& operator=(Map &&rhs) noexcept
		{
			//自赋值判断在tree的移动构造函数里
			tree = rhs.tree;//std::move(rhs.tree);
			defaultValue_ = rhs.defaultValue_;
			return *this;
		}
	};
}

