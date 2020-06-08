#define _CRTIMP

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>


#include <time.h>

HMODULE hmod_MSVCR14X__dll;

size_t(__cdecl* __Wcsftime)(
	wchar_t*       const buffer,
	size_t         const max_size,
	wchar_t const* const format,
	tm const*      const timeptr,
	void*          const lc_time_arg
	);
extern "C" size_t __cdecl _Wcsftime(
	wchar_t*       const buffer,
	size_t         const max_size,
	wchar_t const* const format,
	tm const*      const timeptr,
	void*          const lc_time_arg
)
{
	if (hmod_MSVCR14X__dll==NULL)
	{
		hmod_MSVCR14X__dll = LoadLibrary(TEXT("MSVCR14X.dll"));
	}
	if (__Wcsftime==NULL)
	{
		__Wcsftime = (size_t(__cdecl *)(wchar_t *const, const ::size_t, const wchar_t *const, const tm *const, void *const))GetProcAddress(hmod_MSVCR14X__dll, "_Wcsftime");
	}
	return __Wcsftime(buffer, max_size, format, timeptr, lc_time_arg);
}

void (__cdecl* __libm_sse2_sqrt_precise)(void);
extern "C" void __cdecl _libm_sse2_sqrt_precise(void)
{
	if (hmod_MSVCR14X__dll == NULL)
	{
		hmod_MSVCR14X__dll = LoadLibrary(TEXT("MSVCR14X.dll"));
	}

	if (__libm_sse2_sqrt_precise==NULL)
	{
		__libm_sse2_sqrt_precise = (void(__cdecl *)(void))GetProcAddress(hmod_MSVCR14X__dll, "_libm_sse2_sqrt_precise");
	}

	return __libm_sse2_sqrt_precise();
}

extern "C" char ** _environ;
errno_t __cdecl _get_environ(_Out_ char ***p__environ)
{
	if (!p__environ)
	{
		*_errno() = EINVAL;
		_invoke_watson(0, 0, 0, 0, 0);
		return EINVAL;
	}

	if (!_environ)
	{
		*_errno() = EINTR;
		_invoke_watson(0, 0, 0, 0, 0);
		return EINTR;
	}

	*p__environ = _environ;
	return 0;
}

extern "C" wchar_t ** _wenviron;
errno_t __cdecl _get_wenviron(_Out_ wchar_t ***p__wenviron)
{
	if (!p__wenviron)
	{
		*_errno() = EINVAL;
		_invoke_watson(0, 0, 0, 0, 0);
		return EINVAL;
	}

	if (!_wenviron)
	{
		*_errno() = EINTR;
		_invoke_watson(0, 0, 0, 0, 0);
		return EINTR;
	}

	*p__wenviron = _wenviron;
	return 0;
}