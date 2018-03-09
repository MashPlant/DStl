#include "stdafx.h"
#include <iostream>
#include "Calculator.h"
#include <functional>
#include "Util.h"
#include "ListT.h"
using namespace std;
using namespace ds;

using L = ListT<int, 1, 7, 14, 0, 9, 4, 18, 18, 2, 4>;
using Ls = QSort<L>;
Output<Ls> o;
