//#define sprintf_s my_sprintf_s

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif


#if _MSC_VER>=1900
#define _NO_CRT_STDIO_INLINE
#else
#define __crt_va_start _crt_va_start
#define __crt_va_end _crt_va_end
#endif

/* _countof helper */
#if !defined(_countof)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#include <windows.h>
#include <crtdefs.h>
#undef __STDC_WANT_SECURE_LIB__
#define _vscwprintf _vscwprintf_cur_vc_version
#include <stdio.h>
#undef _vscwprintf
#include <stdarg.h>
#include <vadefs.h>
#include <locale.h>
#include <malloc.h>
#include <errno.h>
#include <crtdbg.h>
#include <time.h>
#include <mbstring.h>
#include <float.h>
#include <tchar.h>

#ifndef _ASSERT_EXPR
#ifndef _DEBUG
#define _ASSERT_EXPR(expr, expr_str) ((void)0)
#else
#define _ASSERT_EXPR(expr, msg) \
	(void) ((!!(expr)) || \
	(1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, msg)) || \
	(_CrtDbgBreak(), 0))
#endif//_DEBUG
#endif//_ASSERT_EXPR

#define __MIN_CRT__

#ifdef __MIN_CRT__

#ifndef va_copy
#define va_copy(destination, source) ((destination) = (source))
#endif

/*
* Assert in debug builds.
* set errno and return value
*/

#ifndef _VALIDATE_RETURN
#define _VALIDATE_RETURN( expr, errorcode, retexpr )						   \
	{   																	   \
	int _Expr_val=!!(expr); 											   \
	_ASSERT_EXPR( ( _Expr_val ), _CRT_WIDE(#expr) );					   \
	if ( !( _Expr_val ) )   											   \
		{   																   \
		errno = errorcode;  											   \
		/*_INVALID_PARAMETER(_CRT_WIDE(#expr) );*/  					  \
		return ( retexpr ); 											   \
		}   																   \
	}
#endif  /* _VALIDATE_RETURN */

int __CRTDECL vsprintf_s(char* buffer, size_t numberOfElements, const char* format, va_list argptr)
{
	return _vsnprintf(buffer, numberOfElements, format, argptr);
}

int __CRTDECL vswprintf_s(wchar_t* buffer, size_t numberOfElements, const wchar_t* format, va_list argptr)
{
	return _vsnwprintf(buffer, numberOfElements, format, argptr);
}

int __CRTDECL _vscprintf(const char* format, va_list ap)
{
	int retval;
	va_list argcopy;
	va_copy(argcopy, ap);
	retval = _vsnprintf(NULL, 0, format, argcopy);
	if (retval == -1)
	{
		//windows2000会出现这种情况
		char szText[1024 * 2];
		retval = _vsnprintf(szText, _countof(szText), format, argcopy);
	}
	va_end(argcopy);
	return retval;
}

int __CRTDECL _vscwprintf(const wchar_t* format, va_list ap)
{
	int retval;
	va_list argcopy;
	va_copy(argcopy, ap);
	retval = _vsnwprintf(NULL, 0, format, argcopy);
	if (retval == -1)
	{
		//windows2000会出现这种情况
		wchar_t szText[1024 * 2];
		retval = _vsnwprintf(szText, _countof(szText), format, argcopy);
	}
	va_end(argcopy);
	return retval;
}

int __cdecl _scprintf(_In_z_ _Printf_format_string_ const char* _Format, ...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vscprintf(_Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

int __cdecl _scwprintf(_In_z_ _Printf_format_string_ const wchar_t* _Format, ...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vscwprintf(_Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

int __cdecl _vsnwprintf_s(_Out_z_cap_(_SizeInWords) wchar_t* _DstBuf, _In_ size_t _SizeInWords, _In_ size_t _MaxCount, _In_z_ _Printf_format_string_ const wchar_t* _Format, va_list _ArgList)
{
	int _Result;
	_Result = _vsnwprintf(_DstBuf, __min(_SizeInWords, _MaxCount), _Format, _ArgList);
	return _Result;
}

int __cdecl _vsnprintf_s(_Out_z_cap_(_SizeInBytes) char* _DstBuf, _In_ size_t _SizeInBytes, _In_ size_t _MaxCount, _In_z_ _Printf_format_string_ const char* _Format, va_list _ArgList)
{
	int _Result;
	_Result = _vsnprintf(_DstBuf, __min(_SizeInBytes, _MaxCount), _Format, _ArgList);
	return _Result;
}

void __cdecl _ftime64(_Out_ struct __timeb64* _Time);
errno_t __CRTDECL _ftime64_s(struct __timeb64* timeptr)
{
	_ftime64(timeptr);
	return 0;
}

errno_t strncat_s(char* strDest, size_t numberOfElements, const char* strSource, size_t count)
{
	char* pRetStr = strncat(strDest, strSource, __min(numberOfElements, count));
	errno_t e = pRetStr == strDest ? 0 : 1;
	return e;
}

#if _MSC_VER<1800
errno_t __cdecl memmove_s(_Out_opt_bytecap_post_bytecount_(_DstSize, _MaxCount) void* _Dst, _In_ rsize_t _DstSize, _In_opt_bytecount_(_MaxCount) const void* _Src, _In_ rsize_t _MaxCount)
{
	memmove(_Dst, _Src, __min(_DstSize, _MaxCount));
	return 0;
}

errno_t __cdecl memcpy_s(_Out_opt_bytecap_post_bytecount_(_DstSize, _MaxCount) void* _Dst, _In_ rsize_t _DstSize, _In_opt_bytecount_(_MaxCount) const void* _Src, _In_ rsize_t _MaxCount)
{
	memcpy(_Dst, _Src, __min(_DstSize, _MaxCount));
	return 0;
}
#endif

void* __cdecl _recalloc(void* memblock, size_t count, size_t size)
{
	void* retp = NULL;
	size_t size_orig = 0, old_size = 0;

	/* ensure that (size * count) does not overflow */
	if (count > 0)
	{
		_VALIDATE_RETURN((_HEAP_MAXREQ / count) >= size, ENOMEM, NULL);
	}
	size_orig = size * count;
	if (memblock != NULL)
		old_size = _msize(memblock);
	retp = realloc(memblock, size_orig);
	if (retp != NULL && old_size < size_orig)
	{
		memset((char*)retp + old_size, 0, size_orig - old_size);
	}
	return retp;
}


int __CRTDECL _vswprintf(_Pre_notnull_ _Post_z_ wchar_t* _String, _In_z_ _Printf_format_string_ const wchar_t* _Format, va_list _Ap)
{
	static int (__CRTDECL* msvcrt__vswprintf)(
	_Pre_notnull_ _Post_z_ wchar_t* _String,
		_In_z_ _Printf_format_string_ const wchar_t* _Format, va_list _Ap);
	if (!msvcrt__vswprintf)
	{
		msvcrt__vswprintf = (int (__cdecl *)(wchar_t *, const wchar_t *, va_list))GetProcAddress(GetModuleHandle(TEXT("msvcrt.dll")), "vswprintf");
	}
	return msvcrt__vswprintf(_String, _Format, _Ap);
}

int __cdecl _swprintf(_Pre_notnull_ _Post_z_ wchar_t* _String, _In_z_ _Printf_format_string_ const wchar_t* _Format, ...)
{
	int _Ret;
	va_list _Arglist;
	__crt_va_start(_Arglist, _Format);
	_Ret = _vswprintf(_String, _Format, _Arglist);
	__crt_va_end(_Arglist);
	return _Ret;
}

int __cdecl sprintf_s(_Out_z_bytecap_(_SizeInBytes) char* _DstBuf, _In_ size_t _SizeInBytes, _In_z_ _Printf_format_string_ const char* _Format, ...)
{
	int _Ret;
	va_list _Arglist;
	__crt_va_start(_Arglist, _Format);
	_Ret = _vsnprintf(_DstBuf, _SizeInBytes, _Format, _Arglist);
	__crt_va_end(_Arglist);
	return _Ret;
}

int __cdecl _snprintf_s(_Out_z_bytecap_(_SizeInBytes) char* _DstBuf, _In_ size_t _SizeInBytes, _In_ size_t _MaxCount, _In_z_ _Printf_format_string_ const char* _Format, ...)
{
	int _Ret;
	va_list _Arglist;
	__crt_va_start(_Arglist, _Format);
	_Ret = _vsnprintf(_DstBuf, _SizeInBytes, _Format, _Arglist);
	__crt_va_end(_Arglist);
	return _Ret;
}

int __cdecl swprintf_s(_Out_z_cap_(_SizeInWords) wchar_t* _Dst, _In_ size_t _SizeInWords, _In_z_ _Printf_format_string_ const wchar_t* _Format, ...)
{
	int _Ret;
	va_list _Arglist;
	__crt_va_start(_Arglist, _Format);
	_Ret = _vsnwprintf(_Dst, _SizeInWords, _Format, _Arglist);
	__crt_va_end(_Arglist);
	return _Ret;
}

int __cdecl _snwprintf_s(_Out_z_cap_(_SizeInWords) wchar_t* _DstBuf, _In_ size_t _SizeInWords, _In_ size_t _MaxCount, _In_z_ _Printf_format_string_ const wchar_t* _Format, ...)
{
	int _Ret;
	va_list _Arglist;
	__crt_va_start(_Arglist, _Format);
	_Ret = _vsnwprintf(_DstBuf, _SizeInWords, _Format, _Arglist);
	__crt_va_end(_Arglist);
	return _Ret;
}


size_t __cdecl wcsnlen(_In_z_ const wchar_t* _Src, _In_ size_t _MaxCount)
{
	return wcslen(_Src);
}

size_t __cdecl strnlen(_In_z_ const char* _Str, _In_ size_t _MaxCount)
{
	return strlen(_Str);
}

errno_t __cdecl _wcslwr_s(_Inout_z_cap_(_SizeInWords) wchar_t* _Str, _In_ size_t _SizeInWords)
{
	_wcslwr(_Str);
	return 0;
}

errno_t __cdecl _wcsupr_s(_Inout_z_cap_(_Size) wchar_t* _Str, _In_ size_t _Size)
{
	_wcsupr(_Str);
	return 0;
}

errno_t __cdecl strcpy_s(_Out_z_cap_(_SizeInBytes) char* _Dst, _In_ rsize_t _SizeInBytes, _In_z_ const char* _Src)
{
	strncpy(_Dst, _Src, _SizeInBytes);
	return 0;
}

errno_t __cdecl strncpy_s(_Out_z_cap_(_SizeInBytes) char* _Dst, _In_ rsize_t _SizeInBytes, _In_z_ const char* _Src, _In_ rsize_t _MaxCount)
{
	strncpy(_Dst, _Src, _SizeInBytes);
	return 0;
}

errno_t __cdecl wcsncpy_s(_Out_z_cap_(_SizeInWords) wchar_t* _Dst, _In_ rsize_t _SizeInWords, _In_z_ const wchar_t* _Src, _In_ rsize_t _MaxCount)
{
	wcsncpy(_Dst, _Src, _SizeInWords);
	return 0;
}

errno_t __cdecl wcscpy_s(_Out_z_cap_(_SizeInWords) wchar_t* _Dst, _In_ rsize_t _SizeInWords, _In_z_ const wchar_t* _Src)
{
	wcsncpy(_Dst, _Src, _SizeInWords);
	return 0;
}

errno_t __cdecl _mbsnbcpy_s(_Out_z_cap_(_SizeInBytes) unsigned char* _Dst, _In_ size_t _SizeInBytes, _In_z_ const unsigned char* _Src, _In_ size_t _MaxCount)
{
	_mbsnbcpy(_Dst, _Src, _SizeInBytes);
	return 0;
}

errno_t __cdecl strcat_s(_Inout_z_cap_(_SizeInBytes) char* _Dst, _In_ rsize_t _SizeInBytes, _In_z_ const char* _Src)
{
	strncat(_Dst, _Src, _SizeInBytes);
	return 0;
}

errno_t __cdecl wcscat_s(_Inout_z_cap_(_SizeInWords) wchar_t* _Dst, _In_ rsize_t _SizeInWords, _In_z_ const wchar_t* _Src)
{
	wcsncat(_Dst, _Src, _SizeInWords);
	return 0;
}

errno_t __cdecl _mbslwr_s(_Inout_opt_z_bytecap_(_SizeInBytes) unsigned char* _Str, _In_ size_t _SizeInBytes)
{
	_mbslwr(_Str);
	return 0;
}

errno_t __cdecl _localtime64_s(struct tm* _tm, const __time64_t* time)
{
	struct tm* _tm_new = _localtime64(time);
	if (_tm_new && _tm)
	{
		*_tm = *_tm_new;
	}
	return 0;
}

errno_t __cdecl _gmtime64_s(struct tm* _tm, const __time64_t* time)
{
	struct tm* _tm_new = _gmtime64(time);
	if (_tm_new && _tm)
	{
		*_tm = *_tm_new;
	}
	return 0;
}

errno_t __cdecl _controlfp_s(_Out_opt_ unsigned int* _CurrentState, _In_ unsigned int _NewValue, _In_ unsigned int _Mask)
{
	unsigned int ret = _controlfp(_NewValue, _Mask);
	return 0;
}

errno_t __cdecl wctomb_s(_Out_opt_ int* _SizeConverted, _Out_opt_bytecap_post_bytecount_(_SizeInBytes, *_SizeConverted) char* _MbCh, _In_ rsize_t _SizeInBytes, _In_ wchar_t _WCh)
{
	int ret = wctomb(_MbCh, _WCh);
	return 0;
}

#ifndef __NO_WMEMCPY_S__
errno_t __cdecl wmemcpy_s(_Out_opt_cap_post_count_(_N1, _N) wchar_t* _S1, _In_ rsize_t _N1, _In_opt_count_(_N) const wchar_t* _S2, rsize_t _N)
{
#pragma warning( push )
#pragma warning( disable : 6386 )
	return memcpy_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
}
#endif

errno_t __cdecl _itoa_s(_In_ int _Value, _Out_z_cap_(_Size) char* _DstBuf, _In_ size_t _Size, _In_ int _Radix)
{
	_itoa(_Value, _DstBuf, _Radix);
	return 0;
}

errno_t __cdecl _ultoa_s(_In_ unsigned long _Val, _Out_z_cap_(_Size) char* _DstBuf, _In_ size_t _Size, _In_ int _Radix)
{
	_ultoa(_Val, _DstBuf, _Radix);
	return 0;
}

#endif//__MIN_CRT__
