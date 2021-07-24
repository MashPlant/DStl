#pragma once
namespace ds
{
	//算法借鉴了gnu pbds，实现了O(1)空间pop (递归merge也是O(n)的，而且是栈空间O(n))
	//另有一个很简单的实现，O(n)空间pop，而且慢一些，在后面被注释掉了(其实真正做题不会用vector，所以也不会跑的很慢)
	//二者的思想是完全相同的，虽然看起来很不一样

	template <typename K>
	class PairingHeap	//最小堆
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
				p->ch->porp = ch; //这里的porp是prev
			ch->porp = p;
			p->ch = ch;
		}
		static Node *joinChild(Node *p)
		{
			/*
			与后面的代码的对应:
			fwdJoin等价于
			for (int i = 0; i < chs.size(); i += 2)
				aux.push_back(merge(chs[i], chs[i + 1])); //一对一对合并
			backJoin等价于
			for (int i = 0; i < aux.size(); ++i)
				root = merge(root, aux[i]); //把这些对合并起来
			*/
			Node *ret = p->ch;
			if (!ret)
				return nullptr;
			while (ret->next)
				ret = fwdJoin(ret, ret->next); //每次merge一对，Paring的名字来源
			while (ret->porp != p)
				ret = backJoin(ret->porp, ret);
			return ret;
		}
		static Node *fwdJoin(Node *cur, Node *nxt)
		{
			if (nxt->key < cur->key) //nxt作为根，cur作为孩子
			{
				nxt->porp = cur->porp; //链表删除
				makeChildOf(cur, nxt); //没有修改cur->next是因为makeChildOf里面修改了
				return nxt->next ? nxt->next : nxt;
			}
			if (nxt->next) //cur作为根，nxt作为孩子；判一下nxt->next才能执行链表删除
			{
				nxt->next->porp = cur;
				cur->next = nxt->next; //链表删除
				makeChildOf(nxt, cur);
				return cur->next;
			}
			//特判nxt->next为空
			cur->next = nullptr;
			makeChildOf(nxt, cur);
			return cur;
		}
		static Node *backJoin(Node *pre, Node *cur)
		{
			if (cur->key < pre->key) //cur作为根
			{
				cur->porp = pre->porp;
				makeChildOf(pre, cur);
				return cur;
			}
			pre->next = nullptr; //pre作为根，直接把pre之后的 [所有节点] 加入它的孩子
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
			//链表删除
			if (x->next)
				x->next->porp = x->porp;
			if (x->porp->next == x) //porp 是 prev
				x->porp->next = x->next;
			else //porp 是 parent
				x->porp->ch = x->next;
			x->porp = nullptr;
			root = merge(root, x);
		}
	};
	/*template <typename K>
	class PairingHeap
	{
	public:
		struct Node //最小堆
		{
			K key;
			Node *p = nullptr;
			Node *ch = nullptr; //左孩子-右兄弟式树
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
			sour->next = tmp; //插入链表前端
			return dest;
		}

		//@Deprecated
		static Node *mergePair(Node *ch) //一对一对的merge，名字来源
		{
			if (!ch || !ch->next)
				return ch;
			return merge(merge(ch, ch->next), mergePair(ch->next->next));
			//想法很美好，均摊时间复杂度也有保证
			//但是很可惜，一大堆push之后的第一次pop会导致栈溢出
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
			//root = mergePair(root->ch); //栈溢出
			static std::vector<Node *> chs; //一个技巧，vector的clear/pop_back不会释放内存，所以这样可以避免反复申请/释放内存
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