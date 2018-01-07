#define _X86_
#define _NO_CRT_STDIO_INLINE

#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <tchar.h>
#include <windows.h>
#include "..\msvcp140_\corecrt_internal.h"
#include "msvcrt.h"
#include "ucrt_patch.h"

extern BOOL n_format_enabled;
#include "printf.h"

typedef msvcrt_FILE MSVCRT_FILE;
#define STDOUT_FILENO stdout
#define STDERR_FILENO stderr

#define FTEXT           0x80    /* file handle is in text mode */
#define WX_TEXT FTEXT


/*
* Control structure for lowio file handles
*/
typedef struct {
	intptr_t osfhnd;    /* underlying OS file HANDLE */
	char osfile;        /* attributes of file (e.g., open in text mode?) */
	char pipech;        /* one char buffer for handles opened on pipes */
	int lockinitflag;
	CRITICAL_SECTION lock;
#ifndef _SAFECRT_IMPL
	/* Not used in the safecrt downlevel. We do not define them, so we cannot use them accidentally */
	char textmode : 7;     /* __IOINFO_TM_ANSI or __IOINFO_TM_UTF8 or __IOINFO_TM_UTF16LE */
	char unicode : 1;      /* Was the file opened as unicode? */
	char pipech2[2];       /* 2 more peak ahead chars for UNICODE mode */
	__int64 startpos;      /* File position that matches buffer start */
	BOOL utf8translations; /* Buffer contains translations other than CRLF*/
	char dbcsBuffer;       /* Buffer for the lead byte of dbcs when converting from dbcs to unicode */
	BOOL dbcsBufferUsed;   /* Bool for the lead byte buffer is used or not */
#endif  /* _SAFECRT_IMPL */
}   ioinfo;


// The log-base-2 of the number of elements in each array of lowio file objects
#define IOINFO_L2E          6

// The number of elements in each array of lowio file objects
#define IOINFO_ARRAY_ELTS   (1 << IOINFO_L2E)

// The hard maximum number of arrays of lowio file objects that may be allocated
#define IOINFO_ARRAYS       128

/*
* Array of pointers to arrays of ioinfo structs.
*/
_CRTIMP ioinfo * __pioinfo[IOINFO_ARRAYS];
_CRTIMP ioinfo __badioinfo;


/* INTERNAL: Allocate temporary buffer for stdout and stderr */
static BOOL add_std_buffer(MSVCRT_FILE *file)
{
	return(1);
}

/* INTERNAL: Removes temporary buffer from stdout or stderr */
/* Only call this function when add_std_buffer returned TRUE */
static void remove_std_buffer(MSVCRT_FILE *file)
{
	//_flush(file);
	//file->_ptr = file->_base = NULL;
	//file->_bufsiz = file->_cnt = 0;
	//file->_flag &= ~_USERBUF;
}

#define _pioinfo(i) ( __pioinfo[(i) >> IOINFO_L2E] + ((i) & (IOINFO_ARRAY_ELTS - \
                              1)) )
/*
* Safer versions of the above macros. Currently, only _osfile_safe is
* used.
*/
#define _pioinfo_safe(i)    ( (((i) != -1) && ((i) != -2)) ? _pioinfo(i) : &__badioinfo )

static inline ioinfo* get_ioinfo_nolock(int fd)
{
	return _pioinfo_safe(fd);
}

static int puts_clbk_file_a(void *file, int len, const char *str)
{
	return fwrite(str, sizeof(char), len, file);
}

static int puts_clbk_file_w(void *file, int len, const wchar_t *str)
{
	int i, ret;

	_lock_file(file);

	if (!(get_ioinfo_nolock(((MSVCRT_FILE*)file)->_file)->osfile & WX_TEXT)) {
		ret = _fwrite_nolock(str, sizeof(wchar_t), len, file);
		_unlock_file(file);
		return ret;
	}

	for (i = 0; i < len; i++) {
		if (_fputwc_nolock(str[i], file) == WEOF) {
			_unlock_file(file);
			return -1;
		}
	}

	_unlock_file(file);
	return len;
}

static int vfprintf_helper(DWORD options, FILE* file, const char *format,
	_locale_t locale, va_list valist)
{
	printf_arg args_ctx[_ARGMAX + 1];
	BOOL tmp_buf;
	int ret;

	if (!CHECK_PMT(file != NULL)) return -1;
	if (!CHECK_PMT(format != NULL)) return -1;

	if (options & PRINTF_POSITIONAL_PARAMS) {
		memset(args_ctx, 0, sizeof(args_ctx));
		ret = create_positional_ctx_a(args_ctx, format, valist);
		if (ret < 0) {
			_invalid_parameter(NULL, NULL, NULL, 0, 0);
			*_errno() = EINVAL;
			return ret;
		}
		else if (!ret)
			options &= ~PRINTF_POSITIONAL_PARAMS;
	}

	_lock_file(file);
	tmp_buf = add_std_buffer(file);
	ret = pf_printf_a(puts_clbk_file_a, file, format, locale, options,
		options & PRINTF_POSITIONAL_PARAMS ? arg_clbk_positional : arg_clbk_valist,
		options & PRINTF_POSITIONAL_PARAMS ? args_ctx : NULL, &valist);
	if (tmp_buf) remove_std_buffer(file);
	_unlock_file(file);

	return ret;
}

static int vfwprintf_helper(DWORD options, FILE* file, const wchar_t *format,
	_locale_t locale, va_list valist)
{
	printf_arg args_ctx[_ARGMAX + 1];
	BOOL tmp_buf;
	int ret;

	if (!CHECK_PMT(file != NULL)) return -1;
	if (!CHECK_PMT(format != NULL)) return -1;

	if (options & PRINTF_POSITIONAL_PARAMS) {
		memset(args_ctx, 0, sizeof(args_ctx));
		ret = create_positional_ctx_w(args_ctx, format, valist);
		if (ret < 0) {
			_invalid_parameter(NULL, NULL, NULL, 0, 0);
			*_errno() = EINVAL;
			return ret;
		}
		else if (!ret)
			options &= ~PRINTF_POSITIONAL_PARAMS;
	}

	_lock_file(file);
	tmp_buf = add_std_buffer(file);
	ret = pf_printf_w(puts_clbk_file_w, file, format, locale, options,
		options & PRINTF_POSITIONAL_PARAMS ? arg_clbk_positional : arg_clbk_valist,
		options & PRINTF_POSITIONAL_PARAMS ? args_ctx : NULL, &valist);
	if (tmp_buf) remove_std_buffer(file);
	_unlock_file(file);

	return ret;
}

/*********************************************************************
*              __stdio_common_vfprintf (UCRTBASE.@)
*/
int CDECL __stdio_common_vfprintf(unsigned __int64 options, FILE *file, const char *format,
	_locale_t locale, va_list valist)
{
	if (options & ~UCRTBASE_PRINTF_MASK)
		FIXME("options %s not handled\n", wine_dbgstr_longlong(options));

	return vfprintf_helper(options & UCRTBASE_PRINTF_MASK, file, format, locale, valist);
}

int CDECL __stdio_common_vfprintf_p(unsigned __int64 options, FILE *file, const char *format,
	_locale_t locale, va_list valist)
{
	if (options & ~UCRTBASE_PRINTF_MASK)
		FIXME("options %s not handled\n", wine_dbgstr_longlong(options));

	return vfprintf_helper(options & UCRTBASE_PRINTF_MASK, file, format, locale, valist);
}

/*********************************************************************
*              __stdio_common_vfprintf_s (UCRTBASE.@)
*/
int CDECL __stdio_common_vfprintf_s(unsigned __int64 options, FILE *file, const char *format,
	_locale_t locale, va_list valist)
{
	if (options & ~UCRTBASE_PRINTF_MASK)
		FIXME("options %s not handled\n", wine_dbgstr_longlong(options));

	return vfprintf_helper((options & UCRTBASE_PRINTF_MASK) | PRINTF_INVOKE_INVALID_PARAM_HANDLER,
		file, format, locale, valist);
}

/*********************************************************************
*              __stdio_common_vfwprintf (UCRTBASE.@)
*/
int CDECL __stdio_common_vfwprintf(unsigned __int64 options, FILE *file, const wchar_t *format,
	_locale_t locale, va_list valist)
{
	if (options & ~UCRTBASE_PRINTF_MASK)
		FIXME("options %s not handled\n", wine_dbgstr_longlong(options));

	return vfwprintf_helper(options & UCRTBASE_PRINTF_MASK, file, format, locale, valist);
}

int CDECL __stdio_common_vfwprintf_p(unsigned __int64 options, FILE *file, const wchar_t *format,
	_locale_t locale, va_list valist)
{
	if (options & ~UCRTBASE_PRINTF_MASK)
		FIXME("options %s not handled\n", wine_dbgstr_longlong(options));

	return vfwprintf_helper(options & UCRTBASE_PRINTF_MASK, file, format, locale, valist);
}

/*********************************************************************
*              __stdio_common_vfwprintf_s (UCRTBASE.@)
*/
int CDECL __stdio_common_vfwprintf_s(unsigned __int64 options, FILE *file, const wchar_t *format,
	_locale_t locale, va_list valist)
{
	if (options & ~UCRTBASE_PRINTF_MASK)
		FIXME("options %s not handled\n", wine_dbgstr_longlong(options));

	return vfwprintf_helper((options & UCRTBASE_PRINTF_MASK) | PRINTF_INVOKE_INVALID_PARAM_HANDLER,
		file, format, locale, valist);
}