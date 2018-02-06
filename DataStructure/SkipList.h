#pragma once
#include <utility>
#include "Util.h"
#include <functional>
namespace ds
{
	//����Map����
	template <typename Traits>
	class SkipList
	{
	private:
		const static int MaxLevel = 16;
		const static int P = 4 - 1; //���������ȡģ���Ӷ��������level��Ϊ��ȡģЧ�ʣ��̶�P=2^k-1
		typedef typename Traits::KeyType KeyType;
		typedef typename Traits::ValueType ValueType;
		typedef typename Traits::KeyCompare KeyCompare;
		KeyCompare comp;
	public:
		struct Node
		{
			ValueType value;
			Node *next[MaxLevel] = { nullptr };
			const KeyType &key()const { return Traits::KeyOf(value); }
			explicit Node(const ValueType &value) :value(value) {}
			Node() = default;
		};
	private:
		Node *nil = nullptr; //nilװ�ܶ�ָ�룬���ǲ�װ����;��С������nil->next[0]��
		int level = 1;
		static int randomLevel()
		{
			int nlevel = 1;
			while (nlevel < MaxLevel && (rawRani() & P) == 0)
				++nlevel;
			return nlevel;
		}
		Node *prev[MaxLevel];
		Node* findNode(const KeyType &key)
		{
			Node *x = nil;
			for (int i = level - 1; i >= 0; --i)
			{
				while (x->next[i] && comp(x->next[i]->key(), key))
					x = x->next[i];
				prev[i] = x;
			}
			return x->next[0];
		}
	public:
		SkipList(KeyCompare comp) :comp(comp), nil(new Node){} //ע��˳��˳������nil��head�Ķ���˳�������
		Node *insert(const ValueType &value)
		{
			Node *x = findNode(Traits::KeyOf(value));
			if (x && !comp(Traits::KeyOf(value), x->key()))
				return x;
			const int nlevel = randomLevel();
			if (nlevel > level)
			{
				for (int i = level; i < nlevel; ++i) //�������level��nil������һ��
					prev[i] = nil;
				level = nlevel;
			}
			x = new Node(value);
			for (int i = 0; i < nlevel; ++i)
			{
				x->next[i] = prev[i]->next[i]; //�����������
				prev[i]->next[i] = x;
			}
			return x;
		}
		void erase(const KeyType &key)
		{
			Node *x = findNode(key);
			if (x && !comp(key, x->key()))
			{
				for (int i = 0; i < level && prev[i]; ++i)
					if (prev[i]->next[i]!=x) break;
					else prev[i]->next[i] = x->next[i];
				delete x;
				while (level > 1 && !(nil->next[level - 1]))
					--level;
			}
		}
		ValueType *find(const KeyType &key) const
		{
			Node *x = nil;
			for (int i = level - 1; i >= 0; --i)
				while (x->next[i] && comp(x->next[i]->key(), key))
					x = x->next[i];
			x = x->next[0];
			if (x && !comp(key, x->key()))
				return &(x->value);
			return nullptr;
		}
		template <typename Pred>
		void walk(Pred pred)
		{
			for (Node *x = nil->next[0]; x; x = x->next[0])
				pred(x->value);
		}
		template <typename Pred, typename Reduce>
		auto walk(Pred pred, Reduce reduce) ->decltype(reduce(pred(nil->value), pred(nil->value)))
		{
			typedef decltype(reduce(pred(nil->value), pred(nil->value))) ReturnType;
			ReturnType ret = ReturnType(); //�����Լ���֤��Ԫ������˷���walk������Ĭ�Ϲ��������0����
			for (Node *x = nil; x; x = x->next[0])
				ret = reduce(ret, pred(x->value));
			return ret;
		}
	};
	template<typename K, typename V, typename Comp>
	struct SkipListMapTraits
	{
		typedef K KeyType;
		typedef std::pair<const K, V> ValueType;
		typedef Comp KeyCompare;
		static const KeyType &KeyOf(const ValueType &value) { return value.first; }
	};
	template<typename K, typename V, typename Comp = std::less<K>>
	class SkipListMap : public SkipList<SkipListMapTraits<K, V, Comp>>
	{
	public:
		SkipListMap(Comp comp = Comp()) :SkipList(comp) {}
	};
	template<typename K, typename Comp>
	struct SkipListSetTraits
	{
		typedef K KeyType;
		typedef K ValueType;
		typedef Comp KeyCompare;
		static const KeyType &KeyOf(const ValueType &value) { return value; }
	};
	template<typename K, typename Comp = std::less<K>>
	class SkipListSet : public SkipList<SkipListSetTraits<K, Comp>>
	{
	public:
		SkipListSet(Comp comp = Comp()) :SkipList(comp) {}
	};
}