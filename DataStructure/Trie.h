#pragma once
#include <string>
namespace ds
{
	struct NullType
	{
	};
	//可以当multiset用,但是不能当multimap用,懒得搞了
	template <size_t AlphabetSize = 26, typename MappedType = NullType, typename ElementType = char, typename Container = std::basic_string<ElementType>>
	class Trie
	{
	public:
		typedef Container Seq;
		typedef MappedType V;
		typedef ElementType K;
		typedef int ToIndex(K);

	private:
		struct Node
		{
			int sz = 0;  //counting the node AND its childs
			int cnt = 0; //only self
			V val;
			Node *kids[AlphabetSize] = { nullptr };
			Node *p = nullptr;
			Node(Node *p = nullptr) : p(p) {}
		};
		Node *root_;
		ToIndex *toIndex_;
		void clear(Node *x)
		{
			if (!x)
				return;
			for (int i = 0; i < AlphabetSize; ++i)
				clear(x->kids[i]);
			if (x != root_)
				delete x;
			else
			{
				x->sz = x->cnt = 0;
				for (int i = 0; i < AlphabetSize; ++i)
					x->kids[i] = nullptr;
			}
		}
		void cpy(Node *dest, const Node *sour)
		{
			dest->sz = sour->sz, dest->cnt = sour->cnt, dest->val = sour->val;
			for (int i = 0; i < AlphabetSize; ++i)
				if (sour->kids[i])
				{
					dest->kids[i] = new Node(dest);
					cpy(dest->kids[i], sour->kids[i]);
				}
		}
		Node *findNode(const Seq &key) const
		{
			Node *x = root_;
			int d = 0, len = key.size();
			while (d < len)
			{
				int index = toIndex_(key[d]);
				if (!x->kids[index])
					return nullptr;
				x = x->kids[index];
				++d;
			}
			return x;
		}

	public:
		Trie(ToIndex toIndex) : root_(new Node), toIndex_(toIndex) {}
		void put(const Seq &key, const V &val = V())
		{
			Node *x = root_, *y = x;
			int d = 0, len = key.size();
			while (d < len)
			{
				int index = toIndex_(key[d]);
				if (!(x->kids[index]))
					x->kids[index] = new Node(x);
				y = x;
				++y->sz;
				x = x->kids[index];
				++d;
			}
			x->val = val, ++x->cnt, ++x->sz;
			x->p = y;
		}
		V *find(const Seq &key) const
		{
			Node *x = findNode(key);
			if (!x || !x->cnt) //找到了链上的某个节点,但是该节点没有值
				return nullptr;
			return &(x->val);
		}

		void del(const Seq &key)
		{
			Node *x = root_;
			int d = 0, len = key.size();
			while (d < len)
			{
				int index = toIndex_(key[d]);
				--x->sz;
				if (!x->kids[index])
				{
					while (x)
						++x->sz, x = x->p;
					return;
				}
				x = x->kids[index];
				++d;
			}
			Node *y = x;
			if (!y->cnt) //key只是已经存在的某个串的一部分,并没有真的找到
			{
				while (x)
					++x->sz, x = x->p;
				return;
			}
			--y->cnt, --y->sz;
			d = len - 1;
			while (y != root_ && !y->sz)
			{
				x = x->p;
				delete y;
				x->kids[toIndex_(key[d--])] = nullptr;
				y = x;
			}
		}
		int size() const { return root_->sz; }
		int size(const Seq &key) const //以key为前缀的串的数量
		{
			Node *x = findNode(key);
			if (!x)
				return 0;
			return x->sz;
		}

		int count(const Seq &key) const
		{
			Node *x = findNode(key);
			if (!x)
				return 0;
			return x->cnt;
		}
		void clear() { clear(root_); }
		void swap(Trie &rhs) noexcept
		{
			std::swap(root_, rhs.root_);
			std::swap(toIndex_, rhs.toIndex_);
		}
		Trie &operator=(Trie rhs)
		{
			swap(*this, rhs); //释放自身的资源由rhs的析构函数来做
			return *this;
		}
		Trie(const Trie &rhs) : root_(new Node), toIndex_(rhs.toIndex_)
		{
			cpy(root_, rhs.root_);
		}
		Trie &operator=(Trie &&rhs) noexcept
		{
			if (this != &rhs)
			{
				~Trie(); //释放自身的资源
				root_ = rhs.root_;
				toIndex_ = rhs.toIndex_;
				rhs.root_ = nullptr;
			}
			return *this;
		}
		Trie(Trie &&rhs) noexcept
		{
			root_ = rhs.root_;
			toIndex_ = rhs.toIndex_;
			rhs.root_ = nullptr;
		}
		~Trie()
		{
			clear(root_);
			delete root_;
		}
	};
}
