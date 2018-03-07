#pragma once
namespace ds
{
	//�㷨�����gnu pbds��ʵ����O(1)�ռ�pop (�ݹ�mergeҲ��O(n)�ģ�������ջ�ռ�O(n))
	//����һ���ܼ򵥵�ʵ�֣�O(n)�ռ�pop��������һЩ���ں��汻ע�͵���(��ʵ�������ⲻ����vector������Ҳ�����ܵĺ���)
	//���ߵ�˼������ȫ��ͬ�ģ���Ȼ�������ܲ�һ��

	template <typename K>
	class PairingHeap	//��С��
	{
	public:
		class Node 
		{
			K key;
			Node *porp = nullptr; //prev or parent
			Node *ch = nullptr; 
			Node *next = nullptr;
			Node(const K &key) :key(key) {}
			friend PairingHeap;
		public:
			const K &getKey()const { return key; }
		};
	private:
		Node *root = nullptr;
		static Node* merge(Node *dest, Node *sour)
		{
			if (!dest) return sour;
			if (!sour) return dest;
			if (sour->key < dest->key)
				std::swap(dest, sour);
			makeChildOf(sour, dest);
			return dest;
		}
		static void makeChildOf(Node *ch, Node *p) 
		{
			ch->next = p->ch;
			if (p->ch)
				p->ch->porp = ch; //�����porp��prev
			ch->porp = p;
			p->ch = ch;
		}
		static Node *joinChild(Node *p)
		{
			/*
			�����Ĵ���Ķ�Ӧ:
			fwdJoin�ȼ���
			for (int i = 0; i < chs.size(); i += 2)
				aux.push_back(merge(chs[i], chs[i + 1])); //һ��һ�Ժϲ�
			backJoin�ȼ���
			for (int i = 0; i < aux.size(); ++i)
				root = merge(root, aux[i]); //����Щ�Ժϲ�����
			*/
			Node *ret = p->ch;
			if (!ret)
				return nullptr;
			while (ret->next)
				ret = fwdJoin(ret, ret->next); //ÿ��mergeһ�ԣ�Paring��������Դ
			while (ret->porp != p)
				ret = backJoin(ret->porp, ret);
			return ret;
		}
		static Node *fwdJoin(Node *cur, Node *nxt)
		{
			if (nxt->key < cur->key) //nxt��Ϊ����cur��Ϊ����
			{
				nxt->porp = cur->porp; //����ɾ��
				makeChildOf(cur, nxt); //û���޸�cur->next����ΪmakeChildOf�����޸���
				return nxt->next ? nxt->next : nxt;
			}
			if (nxt->next) //cur��Ϊ����nxt��Ϊ���ӣ���һ��nxt->next����ִ������ɾ��
			{
				nxt->next->porp = cur;
				cur->next = nxt->next; //����ɾ��
				makeChildOf(nxt, cur);
				return cur->next;
			}
			//����nxt->nextΪ��
			cur->next = nullptr;
			makeChildOf(nxt, cur);
			return cur;
		}
		static Node *backJoin(Node *pre, Node *cur)
		{
			if (cur->key < pre->key) //cur��Ϊ��
			{
				cur->porp = pre->porp;
				makeChildOf(pre, cur);
				return cur;
			}
			pre->next = nullptr; //pre��Ϊ����ֱ�Ӱ�pre֮��� [���нڵ�] �������ĺ���
			makeChildOf(cur, pre);
			return pre;
		}
	public:
		PairingHeap() = default;
		Node *push(const K &key)
		{	
			Node *ret = new Node(key);
			root = merge(root, ret);
			return ret;
		}
		void merge(PairingHeap &rhs) { root = merge(root, rhs.root); }
		const K &top() const { return root->key; }
		bool empty() const { return !root; }
		void pop() { root = joinChild(root); }
		void decreaseKey(Node *x,const K &key)
		{
			x->key = key;
			if (x == root)
				return;
			//����ɾ��
			if (x->next)
				x->next->porp = x->porp;
			if (x->porp->next == x) //porp �� prev
				x->porp->next = x->next;
			else //porp �� parent
				x->porp->ch = x->next;
			x->porp = nullptr;
			root = merge(root, x);
		}
	};
	/*template <typename K>
	class PairingHeap
	{
	public:
		struct Node //��С��
		{
			K key;
			Node *p = nullptr;
			Node *ch = nullptr; //����-���ֵ�ʽ��
			Node *next = nullptr;
			Node(const K &key) :key(key) {}
		};
	private:
		Node *root = nullptr;
		PairingHeap(const K &key) : root(new Node(key)) {}
		static Node* merge(Node *dest,Node *sour)
		{
			if (!dest) return sour;
			if (!sour) return dest;
			if (dest->key > sour->key)
				std::swap(dest, sour);
			sour->p = dest;
			Node *tmp = dest->ch;
			dest->ch = sour;
			sour->next = tmp; //��������ǰ��
			return dest;
		}

		//@Deprecated
		static Node *mergePair(Node *ch) //һ��һ�Ե�merge��������Դ
		{
			if (!ch || !ch->next)
				return ch;
			return merge(merge(ch, ch->next), mergePair(ch->next->next));
			//�뷨�����ã���̯ʱ�临�Ӷ�Ҳ�б�֤
			//���Ǻܿ�ϧ��һ���push֮��ĵ�һ��pop�ᵼ��ջ���
		}
	public:
		PairingHeap() = default;
		void merge(PairingHeap &rhs) { merge(root, rhs.root); }
		Node *push(const K &key)
		{
			Node *ret = new Node(key);
			root = merge(root, ret);
			return ret;
		}
		const K &top() const { return root->key; }
		bool empty() const { return !root; }
		void pop()
		{
			//root = mergePair(root->ch); //ջ���
			static std::vector<Node *> chs; //һ�����ɣ�vector��clear/pop_back�����ͷ��ڴ棬�����������Ա��ⷴ������/�ͷ��ڴ�
			static std::vector<Node *> aux;
			chs.clear(), aux.clear();
			for (Node *ch = root->ch; ch; ch = ch->next)
				chs.push_back(ch);
			if (chs.size() & 1)
				chs.push_back(nullptr);
			for (int i = 0; i < chs.size(); i += 2)
				aux.push_back(merge(chs[i], chs[i + 1]));
			root = nullptr;
			for (int i = 0; i < aux.size(); ++i)
				root = merge(root, aux[i]);
		}
	};*/

}