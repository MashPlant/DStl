#include "stdafx.h"
#include "FibHeap.h"
#include "Algorithm.h"
#include <iostream>
#include "Util.h"
#include "TimSort.h"
#include <algorithm>
#include "BTree.h"
#include "Map.h"
#include <map>
#include <set>
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
/*int main()
{
	
	getchar();
}*/

