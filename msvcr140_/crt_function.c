#include "stdafx.h"

#pragma function(_abs64)

__int64 __cdecl _abs64(__int64 num)
{
	return (num >= 0 ? num : -num);
}

char *(__cdecl* Real__unDName)(char *outputString, const char *name, int maxStringLength, void *(__cdecl *pAlloc)(unsigned int), void(__cdecl *pFree)(void *), unsigned __int16 disableFlags);
char *__cdecl __unDName(char *outputString, const char *name, int maxStringLength, void *(__cdecl *pAlloc)(unsigned int), void(__cdecl *pFree)(void *), unsigned __int16 disableFlags)
{
	if (Real__unDName==NULL)
	{
		Real__unDName= (char *(__cdecl *)(char *, const char *, int, void *(__cdecl *)(unsigned int), void(__cdecl *)(void *), unsigned short))GetProcAddress(GetModuleHandle(_T("msvcrt.dll")), "__unDName");
	}
	return Real__unDName(outputString,name,maxStringLength,pAlloc,pFree,disableFlags);
}

