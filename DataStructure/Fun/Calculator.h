#pragma once
#include <string>
#include <array>
#include <cmath>
#include <algorithm>

namespace ds
{
	class Calculator
	{
	private:
		typedef std::pair<double, double> ResPair;
		class Node
		{
			Node *left = nullptr;
			Node *right = nullptr;
			char opt = 0;
			friend Calculator;
		public:
			double val = 0.0;
			double dval = 0.0;
			Node &operator=(const ResPair &rhs)
			{
				val = rhs.first;
				dval = rhs.second;
				return *this;
			}
			static ResPair plus(const Node &lhs ,const Node &rhs)  { return { lhs.val + rhs.val,lhs.dval + rhs.dval }; }
			static ResPair minus(const Node &lhs, const Node &rhs)  { return { lhs.val - rhs.val,lhs.dval - rhs.dval }; }
			static ResPair multiplies(const Node &lhs, const Node &rhs)  { return { lhs.val * rhs.val,lhs.dval * rhs.val + lhs.val * rhs.dval }; }
			static ResPair divides(const Node &lhs, const Node &rhs)  { return { lhs.val / rhs.val,(lhs.dval * rhs.val - lhs.val * rhs.dval) / (rhs.val*rhs.val) }; }
			static ResPair pow(const Node &lhs, const Node &rhs)
			{
				//(u^v)' = (e^(v*lnu))' = (v*1/u*u' + v'*lnu)*(u^v)
				double ans = std::pow(lhs.val, rhs.val);
				double d = (rhs.val / lhs.val * lhs.dval + rhs.dval * std::log(lhs.val)) * ans;
				return { ans,d };
			}
			//为了配合二元函数表，这里sin之类的也做成二元函数，sin(x) := 0 s x
			static ResPair sin(const Node &, const Node &rhs)  { return { std::sin(rhs.val),std::cos(rhs.val) * rhs.dval }; }
			static ResPair cos(const Node &, const Node &rhs)  { return { std::cos(rhs.val),-std::sin(rhs.val) * rhs.dval }; }
			static ResPair ln(const Node &, const Node &rhs)  { return { std::log(rhs.val),1 / rhs.val * rhs.dval }; }
		};
	public:
		//The type of the lambda-expression (which is also the type of the closure object) 
		//is a unique, unnamed non-union class type
		//每一个lambda表达式都有独一无二的类型，妄想这样利用容器保存lambda是不可能的
		//错误示例:typedef decltype([](double, double) {return 0.0; }) Calc;
		//但是(没有捕获变量的)lambda可以隐式转化为函数指针，这样还是可以避开sdt::function
		using Calc = double(*)(double, double);
		typedef std::array<Calc, 127> Table;
		using Der = decltype(&Node::plus);
		typedef std::array<Der, 127> DerTable;
		typedef std::array<int, 127> PriTable;
	private:
		Node *root = nullptr;
		static Node *dummy; //可以减少一些边界检查
		static Table table;
		static DerTable dtable;
		static PriTable ptable;
		static Table buildTable()
		{
			Table tmp;
			tmp['+'] = [](double a, double b) {return a + b; };
			tmp['-'] = [](double a, double b) {return a - b; };
			tmp['*'] = [](double a, double b) {return a * b; };
			tmp['/'] = [](double a, double b) {return a / b; };
			tmp['^'] = std::pow; //强迫症福利
			tmp['s'] = [](double, double x) {return std::sin(x); };
			tmp['c'] = [](double, double x) {return std::cos(x); };
			tmp['l'] = [](double, double x) {return std::log(x); };
			return tmp;
		}

		static DerTable buildDTable()
		{
			DerTable tmp;
			tmp['+'] = &Node::plus, tmp['-'] = &Node::minus, tmp['*'] = &Node::multiplies, tmp['/'] = &Node::divides, tmp['^'] = &Node::pow;
			tmp['s'] = &Node::sin, tmp['c'] = &Node::cos, tmp['l'] = &Node::ln;
			return tmp;
		}
		static PriTable buildPTable()
		{
			PriTable tmp;
			memset(&tmp[0], 0, sizeof tmp); //再次提醒:内置类型的std::array不会初始化
			tmp['+'] = tmp['-'] = 1;
			tmp['*'] = tmp['/'] = 2;
			tmp['s'] = tmp['c'] = tmp['l'] = 3; //认为sinx^2表示sin(x^2)
			tmp['^'] = 4;
			return tmp;
		}
		static Node* build(const std::string& s, int beg, int end)
		{
			if (beg >= end)
				return dummy;
			bool isVal = true; //是否只含数字或者变量
			int pos[127]; //不在任何括号内的 不同优先级的 运算符的 最后一次出现的位置
			memset(pos, -1, sizeof pos);
			int cnt = 0;
			for (int i = beg; i < end; ++i)
			{
				if (s[i] == '(') ++cnt, isVal = false;
				if (s[i] == ')') --cnt, isVal = false;
				if (!cnt && ptable[s[i]]) pos[ptable[s[i]]] = i, isVal = false;
			}
			if (isVal)
			{
				Node *n = new Node;
				if (s[beg] == 'x') n->opt = 'x';
				else n->val = atof(s.c_str() + beg);
				return n;
			}
			//优先级最低的运算符最后一次出现的位置，便是整个表达式最后一次进行的运算
			int *lastOpt = std::find_if(std::begin(pos), std::end(pos), [](int x) {return x != -1; });
			if (lastOpt == std::end(pos)) //表达式形如(exp)
				return build(s, beg + 1, end - 1);
			Node *n = new Node;
			n->opt = s[*lastOpt];
			n->left = build(s, beg, *lastOpt);
			n->right = build(s, *lastOpt + 1, end);
			return n;
		}
		static double result(const Node *n, double x)
		{
			if (!n) return 0.0;
			if (!n->left && !n->right) return n->opt == 'x' ? x : n->val;
			return table[n->opt](result(n->left, x), result(n->right, x));
		}
		static void derivative(Node *n, double x)
		{
			if (!n) return;
			if (!n->left && !n->right) //叶子结点只有存了变量导数才是1
			{
				if (n->opt == 'x') n->val = x, n->dval = 1.0;
				return;
			}
			derivative(n->left, x);
			derivative(n->right, x);
			*n = dtable[n->opt](*n->left, *n->right);
		}
		static void clear(Node *x)
		{
			if (!x)
				return;
			clear(x->left);
			clear(x->right);
			if (x != dummy)
				delete x;
		}
	public:
		Calculator(std::string s)
		{
			auto replace = [](std::string &raw, const std::string &o, const std::string &n) -> void
			{
				int p = 0;
				while ((p = raw.find(o, p)) != std::string::npos)
				{
					raw.replace(p, o.size(), n);
					p += static_cast<int>(n.size()) - static_cast<int>(o.size()) + 1;
				}
			};
			replace(s, "sin", "s");
			replace(s, "cos", "c");
			replace(s, "ln", "l");
			replace(s, " ", "");
			root = build(s, 0, s.size());
		}
		Calculator(const Calculator &) = delete;
		Calculator& operator=(const Calculator &) = delete;
		~Calculator() { clear(root); }

		double result(double x = 0.0) const
		{
			return result(root, x);
		}
		double derivative(double x) const
		{
			derivative(root, x);
			return root->dval;
		}
		double solve(double goal, double hint, double eps = 1e-5) const
		{
			double x = hint;
			while (derivative(root, x), std::fabs(root->val - goal) > eps)
				x -= (root->val - goal) / root->dval;
			return x;
		}
	};
	Calculator::Table Calculator::table = buildTable();
	Calculator::DerTable Calculator::dtable = buildDTable();
	Calculator::PriTable Calculator::ptable = buildPTable();
	Calculator::Node *Calculator::dummy = new Node;
}
