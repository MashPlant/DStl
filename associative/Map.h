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
		typedef typename Tree::LinkType LinkType;
		typedef std::pair<LinkType, LinkType> NodePair;// self_,parent
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
		DefaultValue defaultValue_ = []() {return V(); };
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
			LinkType self_;
			Tree *ptree_;
		public:
			friend Map;
			friend const_iterator;
			reference operator*() const { return self_->value; }
			pointer operator->()const { return &(operator*()); }
			iterator& operator+=(int k)
			{
				int rk = -1;
				if (self_ != ptree_->nil())
					rk = ptree_->lower((*this)->first, false) + 1 + k;
				return *this = iterator(ptree_->kth(rk), ptree_);
			}
			iterator& operator-=(int k) { return operator+=(-k); }
			iterator operator+(int k) { iterator tmp = *this; return tmp += k; }
			iterator operator-(int k) { iterator tmp = *this; return tmp += -k; }
			int operator-(const_iterator rhs)//assert ptree_ == rhs.ptree_
			{
				int rkl = ptree_->size(), rkr = rkl; //统一把+1去掉了
				if (self_ != ptree_->nil_)
					rkl = ptree_->lower((*this)->first, false);
				if (rhs.self_ != ptree_->nil_)
					rkr = ptree_->lower(rhs->first, false);
				return rkl - rkr;
			}
			iterator& operator++()
			{
				self_ = ptree_->nextNode(self_);
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
				self_ = ptree_->prevNode(self_);
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
			iterator prev()const { return iterator(ptree_->prevNode(self_), ptree_); }
			iterator next()const { return iterator(ptree_->nextNode(self_), ptree_); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			iterator(LinkType n, Tree *t) :self_(n), ptree_(t) {}
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
			LinkType self_;
			const Tree *ptree_;
		public:
			friend Map;
			const_iterator& operator+=(int k)
			{
				int rk = -1;
				if (self_ != ptree_->nil())
					rk = ptree_->lower((*this)->first, false) + 1 + k;
				return *this = const_iterator(ptree_->kth(rk), ptree_);
			}
			const_iterator& operator-=(int k) { return operator+=(-k); }
			const_iterator operator+(int k) { const_iterator tmp = *this; return tmp += k; }
			const_iterator operator-(int k) { const_iterator tmp = *this; return tmp += -k; }
			reference operator*() const { return self_->value; }
			pointer operator->()const { return &(operator*()); }
			const_iterator& operator++() { self_ = ptree_->nextNode(self_); return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++*this; return tmp; }
			const_iterator& operator--() { self_ = ptree_->prevNode(self_); return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; -*this; return tmp; }
			const_iterator prev()const { return const_iterator(ptree_->prevNode(self_), ptree_); }
			const_iterator next()const { return const_iterator(ptree_->nextNode(self_), ptree_); }
			bool operator==(const_iterator rhs) const { return self_ == rhs.self_; }
			bool operator!=(const_iterator rhs) const { return !(*this == rhs); }
			const_iterator(LinkType n, const Tree *t) :self_(n), ptree_(t) {}
			const_iterator(iterator it) :self_(it.self_), ptree_(it.ptree_) {}
		};

		template<typename ...Args>
		explicit Map(Args... args)
			:tree(std::forward<Args>(args)...)  //注意std::forward本身就是接受参数包，返回参数包的，所以调用它(括号内)不需解包,使用它的结果(括号外)需要解包
			 {	}

		void setDefaultValue(DefaultValue defaultValue) { defaultValue_ = defaultValue_; }
		iterator find(const K& key) 
		{
			if (LinkType x = tree.findNode(key).first)
				return iterator(x, &tree);
			return end();
		}
		const_iterator find(const K&key) const
		{
			if (LinkType x = tree.findNode(key).first)
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
			if (LinkType x = tree.findNode(key).first)
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
		//提供const和非const版本，因为splay的查询不是const的
		int lower(const K &rend, bool afterEqual)  { return tree.lower(rend, afterEqual); }
		int between(int lend, int rend) { return lower(rend, true) - lower(lend, false); }
		int lower(const K &rend, bool afterEqual)const { return tree.lower(rend, afterEqual); }
		int between(int lend, int rend)const { return lower(rend, true) - lower(lend, false); }
		//k>size() or k<1时自动返回end()
		iterator kth(int k) { return iterator(tree.kth(k), &tree); }
		void clear() { tree.clear(); }
		iterator begin() { return iterator(tree.minChild(), &tree); }//root_为nil_时minChild返回nil_
		iterator end() { return iterator(tree.nil(), &tree); }
		const_iterator begin() const { return const_iterator(tree.minChild(), &tree); }//root_为nil_时minChild返回nil_
		const_iterator end() const { return const_iterator(tree.nil(), &tree); }
		int size()const { return tree.size(); }
		bool empty()const { return size() == 0; }
		Map(const Map &rhs) = default;
		Map &operator=(const Map &rhs) = default;
		Map(Map &&rhs) noexcept :tree(std::move(rhs.tree)), defaultValue_(rhs.defaultValue_) {}
		Map& operator=(Map &&rhs) noexcept
		{
			tree = std::move(rhs.tree);//自赋值判断在tree的移动构造函数里
			defaultValue_ = rhs.defaultValue_;
			return *this;
		}
	};
}

