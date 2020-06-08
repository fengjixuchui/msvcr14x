/***
*fileinfo.c - sets C file info flag
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Sets the C file info flag.  By default, the openfile information
*       is NOT passed along to children on spawn/exec calls.  If the flag
*       is set, openfile information WILL get passed on to children on
*       spawn/exec calls.
*
*******************************************************************************/

#define BUILD_WINDOWS
#define _CRTIMP

#include <internal_shared.h>
#include <stdlib.h>

#include "os_crtlib.h"


int _fileinfo = -1;

//_CRTIMP int *          __cdecl __p__fileinfo(void);
_CRTIMP int * __cdecl
AFNAME(_fileinfo) (void)
{
	return AFRET(_fileinfo);
}

signed int __cdecl _get_fileinfo(int *p__fileinfo)
{
	signed int result;

	if (p__fileinfo)
	{
		*p__fileinfo = _fileinfo;
		result = 0;
	}
	else
	{
		*_errno() = EINVAL;
		_invoke_watson(0, 0, 0, 0, 0);
		result = EINVAL;
	}
	return result;
}

int __cdecl _set_fileinfo(int fileinfo)
{
	_fileinfo = fileinfo;
	return 0;
}