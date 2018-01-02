// msvcr140_.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "msvcr140_.h"

FILE* __cdecl __iob_func(void)
{
	return NULL;
}
FILE* __cdecl my__acrt_iob_func(unsigned const id)
{
	FILE* std_files = __iob_func();
	return std_files + id;
}

//VC2013中没有但是低版本VC中有的一些函数
int __cdecl my_fputc_nolock(
	IN    int   _Character,
	IN OUT FILE* _Stream
)
{
	HMODULE hmod = GetModuleHandle(_T("msvcrt.dll"));
	int(__cdecl* msvcrt_fputc_nolock)(IN int _Character, IN OUT FILE* _Stream) = (int(__cdecl *)(int, FILE *))GetProcAddress(hmod, "_fputc_nolock");
	return msvcrt_fputc_nolock(_Character, _Stream);
}

int __cdecl my_putc_nolock(
	IN    int   _Character,
	IN OUT FILE* _Stream
)
{
	return my_fputc_nolock(_Character, _Stream);
}