#include <windows.h>

#undef NDEBUG
#define NDEBUG
#define _DEBUG

#if defined(NDEBUG)&&defined(_DEBUG)
int (__cdecl* _VCrtDbgReportW)
(int nRptType, const wchar_t* szFile, int nLine, const wchar_t* szModule,
	const wchar_t* szFormat, va_list arglist);

int __cdecl _CrtDbgReportWV(int nRptType, const WCHAR* szFile, int nLine, const WCHAR* szModule, const WCHAR* szFormat, va_list arglist)
{
	if (_VCrtDbgReportW == NULL)
	{
		//貌似这样无法加载
		HMODULE hLib = GetModuleHandle(TEXT("msvcr90d.dll"));
		if (hLib == NULL)
		{
			hLib = LoadLibrary(TEXT("msvcr90d.dll"));
		}
		if (hLib == NULL)
		{
			return 0;
		}

		_VCrtDbgReportW = (int (__cdecl *)(int, const wchar_t *, int, const wchar_t *, const wchar_t *, va_list))GetProcAddress(hLib, "_VCrtDbgReportW");
	}
	return _VCrtDbgReportW(nRptType, szFile, nLine, szModule, szFormat, arglist);
}

int __cdecl _CrtDbgReportW(int nRptType, const unsigned short* szFile, int nLine, const unsigned short* szModule, const unsigned short* szFormat, ...)
{
	int retval;
	va_list arglist;

	va_start(arglist, szFormat);

	retval = _CrtDbgReportWV(nRptType, (const wchar_t*)(szFile), nLine, (const wchar_t*)(szModule), (const wchar_t*)(szFormat), arglist);

	va_end(arglist);

	return retval;
}

int (__cdecl* _VCrtDbgReportA)(int nRptType, const char* szFile, int nLine,
	const char* szModule, const char* szFormat, va_list arglist);

int __cdecl _CrtDbgReportAV(int nRptType, const CHAR* szFile, int nLine, const CHAR* szModule, const CHAR* szFormat, va_list arglist)
{
	if (_VCrtDbgReportA == NULL)
	{
		//貌似这样无法加载
		HMODULE hLib = GetModuleHandle(TEXT("msvcr90d.dll"));
		if (hLib == NULL)
		{
			hLib = LoadLibrary(TEXT("msvcr90d.dll"));
		}
		if (hLib == NULL)
		{
			return 0;
		}

		_VCrtDbgReportA = (int (__cdecl *)(int, const char *, int, const char *, const char *, va_list))GetProcAddress(hLib, "_VCrtDbgReportA");
	}
	return _VCrtDbgReportA(nRptType, szFile, nLine, szModule, szFormat, arglist);
}

int __cdecl _CrtDbgReport(int nRptType, const CHAR* szFile, int nLine, const CHAR* szModule, const CHAR* szFormat, ...)
{
	int retval;
	va_list arglist;

	va_start(arglist, szFormat);

	retval = _CrtDbgReportAV(nRptType, szFile, nLine, szModule, szFormat, arglist);

	va_end(arglist);

	return retval;
}

typedef int (__cdecl * _CRT_ALLOC_HOOK)(int, void*, size_t, int, long,
	const unsigned char*, int);
_CRT_ALLOC_HOOK __cdecl _CrtSetAllocHook(_In_opt_ _CRT_ALLOC_HOOK _PfnNewHook)
{
	return _PfnNewHook;
}

/*
void* _malloc_dbg(size_t size,
				  int blockType,
				  const char *filename,
				  int linenumber )
{
	return malloc(size);
}
void _free_dbg(void *userData,
			   int blockType 
			   )
{
	free(userData);
}
*/

#endif//defined(NDEBUG)&&defined(_DEBUG)