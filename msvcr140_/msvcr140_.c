// msvcr140_.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "msvcr140_.h"
#include <crtdbg.h>

struct msvcrt__iobuf {
	char *_ptr;
	int   _cnt;
	char *_base;
	int   _flag;
	int   _file;
	int   _charbuf;
	int   _bufsiz;
	char *_tmpfname;
};
typedef struct msvcrt__iobuf msvcrt_FILE;

typedef struct {
	msvcrt_FILE f;
	CRITICAL_SECTION lock;
}   msvcrt__FILEX;
//__crt_stdio_stream_data相当于msvcrt的_FILEX

//msvcrt中stdio的三个元素是FILE对象，需要锁的时候是访问的全局变量_locktable中的对应元素(16+index)
//而ucrt中stdio的三个元素是__crt_stdio_stream_data对象，也即是msvcrt中的_FILEX对象

msvcrt_FILE* _iob;
msvcrt_FILE * __cdecl __iob_func(void)
{
	if (_iob ==NULL)
	{
		HMODULE hmod = GetModuleHandle(_T("msvcrt.dll"));
		_iob = (msvcrt_FILE*)GetProcAddress(hmod, "_iob");
		_ASSERT(_iob);
	}
	return _iob;
}
FILE* __cdecl __acrt_iob_func(unsigned const id)
{
	msvcrt_FILE* fs=__iob_func();
	return (FILE*)&fs[id];
}

int __cdecl __acrt_initialize_stdio()
{
	return 0;
}

void __cdecl __acrt_uninitialize_stdio()
{

}