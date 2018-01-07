#include <windows.h>
#include "..\msvcp140_\corecrt_internal.h"

#include <stdio.h>
#include <locale.h>
#include <float.h>
#define _MSVCR_VER 141

static inline const char *debugstr_a(const char *s) { return s; }
static inline const char *debugstr_w(const WCHAR *s) { return (wchar_t*)s; }
static const char *wine_dbgstr_longlong(ULONGLONG ll)
{
	return _ui64toa(ll,NULL,10);
}

#define INVALID_PMT(x,err)   (*_errno() = (err), _invalid_parameter(NULL, NULL, NULL, 0, 0))
#define CHECK_PMT_ERR(x,err) ((x) || (INVALID_PMT( 0, (err) ), FALSE))
#define CHECK_PMT(x)         CHECK_PMT_ERR((x), EINVAL)

#define TRACE
#define FIXME TRACE
#define ERR TRACE

typedef __int64        longlong;
typedef unsigned int   ulong;

typedef __acrt_ptd thread_data_t;
typedef __crt_locale_data threadlocinfo;
typedef threadlocinfo* pthreadlocinfo;

#define UI64_MAX   _UI64_MAX
#define I64_MAX    _I64_MAX
#define I64_MIN    _I64_MIN

#define UCRTBASE_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION (0x0001)
#define UCRTBASE_PRINTF_STANDARD_SNPRINTF_BEHAVIOUR      (0x0002)
#define UCRTBASE_PRINTF_LEGACY_WIDE_SPECIFIERS           (0x0004)
#define UCRTBASE_PRINTF_LEGACY_MSVCRT_COMPATIBILITY      (0x0008)
#define UCRTBASE_PRINTF_LEGACY_THREE_DIGIT_EXPONENTS     (0x0010)

#define UCRTBASE_PRINTF_MASK                             (0x001F)

#define PRINTF_POSITIONAL_PARAMS                  (0x0100)
#define PRINTF_INVOKE_INVALID_PARAM_HANDLER       (0x0200)

#define UCRTBASE_SCANF_SECURECRT                         (0x0001)
#define UCRTBASE_SCANF_LEGACY_WIDE_SPECIFIERS            (0x0002)
#define UCRTBASE_SCANF_LEGACY_MSVCRT_COMPATIBILITY       (0x0004)

#define UCRTBASE_SCANF_MASK                              (0x0007)

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__) || defined(__sun)
# define DECLSPEC_HIDDEN
#elif defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3)))
# define DECLSPEC_HIDDEN __attribute__((visibility ("hidden")))
#else
# define DECLSPEC_HIDDEN
#endif

thread_data_t *msvcrt_get_thread_data(void) DECLSPEC_HIDDEN
{
	__acrt_ptd* const ptd = __acrt_getptd();
	return ptd;
}
pthreadlocinfo get_locinfo(void) DECLSPEC_HIDDEN
{
	__acrt_ptd* const ptd = __acrt_getptd();
	return ptd->_locale_info;
}

#define strlenW lstrlenW
#define strcmpiW lstrcmpiW
#define strncmpiW _wcsnicmp
#define toupperW towupper
#define tolowerW towlower
#define strncmpW wcsncmp
#define strcmpW lstrcmpW
#define strstrW wcsstr
#define isspaceW iswspace
#define strchrW wcschr


#define _ARGMAX 100
typedef int(*puts_clbk_a)(void*, int, const char*);
typedef int(*puts_clbk_w)(void*, int, const wchar_t*);
typedef union _printf_arg
{
	void *get_ptr;
	int get_int;
	LONGLONG get_longlong;
	double get_double;
} printf_arg;
typedef printf_arg(*args_clbk)(void*, int, int, va_list*);
int pf_printf_a(puts_clbk_a, void*, const char*, _locale_t,
	DWORD, args_clbk, void*, va_list*) DECLSPEC_HIDDEN;
int pf_printf_w(puts_clbk_w, void*, const wchar_t*, _locale_t,
	DWORD, args_clbk, void*, va_list*) DECLSPEC_HIDDEN;
int create_positional_ctx_a(void*, const char*, va_list) DECLSPEC_HIDDEN;
int create_positional_ctx_w(void*, const wchar_t*, va_list) DECLSPEC_HIDDEN;
printf_arg arg_clbk_valist(void*, int, int, va_list*) DECLSPEC_HIDDEN;
printf_arg arg_clbk_positional(void*, int, int, va_list*) DECLSPEC_HIDDEN;

#define FLT_MIN 1.175494351e-38F
#define DBL_MIN 2.2250738585072014e-308
#define _OVERFLOW  3
#define _UNDERFLOW 4