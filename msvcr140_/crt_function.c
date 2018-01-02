#include "stdafx.h"

#pragma function(_abs64)

__int64 __cdecl _abs64(__int64 num)
{
	return (num >= 0 ? num : -num);
}

