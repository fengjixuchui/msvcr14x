// msvcr140_.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "msvcr140_.h"
#include <crtdbg.h>

#include "ucrt_patch.h"
//__crt_stdio_stream_data相当于msvcrt的_FILEX
typedef msvcrt__FILEX ucrt__crt_stdio_stream_data;

//msvcrt中stdio的三个元素是FILE对象，需要锁的时候是访问的全局变量_locktable中的对应元素(16+index)
//而ucrt中stdio的三个元素是__crt_stdio_stream_data对象，也即是msvcrt中的_FILEX对象

/*
* Number of entries in _iob[] (declared below). Note that _NSTREAM_ must be
* greater than or equal to _IOB_ENTRIES.
*/
#define msvcrt__IOB_ENTRIES 20
#define msvcrt__STREAM_LOCKS   16      /* Table of stream locks            */

//_IOLOCKED  - Indicate that the stream is locked.
#define _IOLOCKED       0x8000


msvcrt_FILE* msvcrt__iob;
msvcrt_FILE * __cdecl my__iob_func(void)
{
	if (msvcrt__iob == NULL)
	{
		HMODULE hmod = GetModuleHandle(_T("msvcrt.dll"));
		msvcrt__iob = (msvcrt_FILE*)GetProcAddress(hmod, "_iob");
		_ASSERT(msvcrt__iob);
	}
	return msvcrt__iob;
}
//由于msvcrt.dll的_stbuf函数会验证是否是msvcrt的_iob数组中的stdout或者stderr对象，
//不是的话就不允许输出内容到控制台,因为我们截获了fprintf等函数所以我们必须截获__acrt_iob_func函数才行，
//不然将没有输出
FILE* __cdecl __acrt_iob_func(unsigned id)
{
	msvcrt_FILE* fs = my__iob_func();
	return (FILE*)&fs[id];
}

BOOL IsMsvcrtStdFileHandle(FILE* f)
{
	msvcrt_FILE * fs = my__iob_func();
	for (int i=0;i<msvcrt__IOB_ENTRIES;i++)
	{
		if (&fs[i]==(msvcrt_FILE*)f)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//也截获ucrt的_lock_file和_unlock_file函数以防止错误锁到了msvcrt的stdio流对象
void __cdecl _lock(_In_ int _File);

// Locks a stdio stream.
void __cdecl _lock_file(_Inout_ FILE* stream)
{
	if (IsMsvcrtStdFileHandle(stream))
	{
		msvcrt_FILE *pf = (msvcrt_FILE*)stream;
		/*
		* FILE lies in _iob[] so the lock lies in _locktable[].
		*/
		_lock(msvcrt__STREAM_LOCKS + (int)(pf - msvcrt__iob));
		/* We set _IOLOCKED to indicate we locked the stream */
		pf->_flag |= _IOLOCKED;
	}
	else
	{
		EnterCriticalSection(&((ucrt__crt_stdio_stream_data*)stream)->lock);
	}
}

void __cdecl _unlock(_Inout_ int _File);

// Unlocks a stdio stream.
void __cdecl _unlock_file(_Inout_ FILE* stream)
{
	if (IsMsvcrtStdFileHandle(stream))
	{
		msvcrt_FILE *pf = (msvcrt_FILE*)stream;
		/*
		* FILE lies in _iob[] so the lock lies in _locktable[].
		* We reset _IOLOCKED to indicate we unlock the stream.
		*/
		pf->_flag &= ~_IOLOCKED;
		_unlock(msvcrt__STREAM_LOCKS + (int)(pf - msvcrt__iob));
	}
	else
	{
		LeaveCriticalSection(&((ucrt__crt_stdio_stream_data*)stream)->lock);
	}
}