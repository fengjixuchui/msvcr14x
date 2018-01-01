// msvcr140_.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "msvcr140_.h"

FILE* __cdecl __acrt_iob_func(unsigned const id)
{
	FILE* std_files = __iob_func();
	return std_files + id;
}