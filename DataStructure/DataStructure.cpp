#include "stdafx.h"

#include "SetT.h"
using namespace ds;
using T0 = MakeTreeT<int, 1, 2, 3, 4, 5, 6, 7>;
using T1 = InsertT<T0, -1>;
using T2 = EraseT<T1, 4>;
OutputTree<T2> o;