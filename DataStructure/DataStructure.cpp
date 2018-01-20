#include "stdafx.h"
#include "Util.h"
#include "Deque.h"
#include "Vector.h"
#include <algorithm>
#include <ctime>
#include "PriorityQueue.h"
#include "FibHeap.h"
#include "LeftistTree.h"
#include <list>
using namespace std;
using namespace ds;



namespace ds
{
	template<>
	struct NumericTraits<float>
	{
		const static float NegaInf;
	};
	const float NumericTraits<float>::NegaInf = -1e8;
}

int main()
{
	FibHeap<float> fib;
	auto x = fib.push(1.0f);
	fib.push(2.0f);
	fib.push(3.0f);
	fib.delNode(x);
	cout << fib.top();
	getchar();
}

