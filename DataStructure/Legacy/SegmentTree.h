#pragma once
#include "SegmentTreeUtil.h"

namespace ds
{
	//����һ�ų�����߶������κ�������������ʵ�����&��������������ʵ��
	//���������޸�ֻ֧���������
	//ϣ����һЩ�����ŵ�д����֧�������/����=/...
	//������ҪĿ�Ļ������֣�˭�������ֶ��������أ�
	template <typename K,typename Func,int N>
	class SegmentTree
	{
	private:
		Seg::Node<K, Func, 1, N> root;
		typedef Seg::FuncTraits<K, Func> Function;
		static Function func;

		//�ڵ��Left/Right�ǽڵ��Ͻ�ķ�Χ
		//��ѯ��l/r������ķ�Χ
		template <int Left,int Right>
		static K query(Seg::Node<K,Func,Left,Right> &x,int l,int r) 
		{
			if (l <= Left && r >= Right)
				return x.key;
			pushDown(x);
			constexpr int mid = (Left + Right) / 2;

			K lres = Function::zero, rres = Function::zero;
			if (l <= mid)
				lres = query(x.left,l, r);
			if (r >= mid + 1)
				rres = query(x.right, l, r);
			return func(lres, rres);
		}
		template <int Left>
		static K query(Seg::Node<K, Func, Left, Left> &x, int l, int r) { return x.key; }

		template <int Left, int Right>
		constexpr static int lenOf(const Seg::Node<K, Func, Left, Right> &x) { return Right - Left + 1; }
		
		template <int Left, int Right>
		static void addLabel(Seg::Node<K, Func, Left, Right> &x, const K &lb) { x.label += lb; }
		template <int Left>
		static void addLabel(Seg::Node<K, Func, Left, Left> &x, const K &lb) { }
		template <int Left, int Right>

		static void pushDown(Seg::Node<K, Func, Left, Right> &x)
		{
			if (x.label)
			{
				K lb = x.label;
				x.label = K();
				x.left.key += Function::rangeSum(lb, lenOf(x.left)); 
				x.right.key += Function::rangeSum(lb, lenOf(x.right));
				addLabel(x.left, lb);
				addLabel(x.right, lb);
			}
		}
		template <int Left>
		static void pushDown(Seg::Node<K, Func, Left, Left> &x) { }

		template <int Left, int Right>
		static void rangeAdd(Seg::Node<K, Func, Left, Right> &x, int l, int r,const K &k)
		{
			if (l <= Left && r >= Right)
			{
				x.key += Function::rangeSum(k, lenOf(x));
				x.label += k;
				return;
			}
			pushDown(x);
			constexpr int mid = (Left + Right) / 2;
			if (l <= mid)
				rangeAdd(x.left, l, r, k);
			if (r >= mid + 1)
				rangeAdd(x.right, l, r, k);
			x.key = func(x.left.key, x.right.key);
		}
		template <int Left>
		static void rangeAdd(Seg::Node<K, Func, Left, Left> &x, int l, int r, const K &k) { x.key += k; }
	public:
		template<typename Iter>
		SegmentTree(Iter it):root(it){}
		K query(int l, int r) { return query(root, l, r); }
		void rangeAdd(int l, int r, const K &k) { rangeAdd(root, l, r, k); }

		template <typename ModifyT>
		void modify(int l,int r,const K &k)
		{
			
		}
		
	};
	template <typename K, typename Func, int N>
	typename SegmentTree<K, Func, N>::Function SegmentTree<K, Func, N>::func;
}
