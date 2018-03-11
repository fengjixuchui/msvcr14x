/***
*crt0dat.c - 32-bit C run-time initialization/termination routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This module contains the routines _cinit, exit, and _exit
*       for C run-time startup and termination.  _cinit and exit
*       are called from the _astart code in crt0.asm.
*       This module also defines several data variables used by the
*       runtime.
*
*       [NOTE: Lock segment definitions are at end of module.]
*
*******************************************************************************/

#define BUILD_WINDOWS
#define _CRTIMP

#include <internal_shared.h>
#include <stdlib.h>

/* define errno */




/* define umask */
int _umaskval = 0;


/* define version info variables */

_CRTIMP unsigned int _osplatform = 0;
_CRTIMP unsigned int _osver = 0;
_CRTIMP unsigned int _winver = 0;
_CRTIMP unsigned int _winmajor = 0;
_CRTIMP unsigned int _winminor = 0;

/***
*errno_t _get_osplatform() - get _osplatform
*
*Purpose:
*       Get the value of _osplatform
*
*Entry:
*       unsigned int *pValue - pointer where to store the value
*
*Exit:
*       The error code
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/
errno_t _get_osplatform(unsigned int *pValue)
{
    /* validation section */
    _VALIDATE_RETURN_ERRCODE(pValue != NULL, EINVAL);

_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    _VALIDATE_RETURN_ERRCODE(_osplatform!=0, EINVAL);

    *pValue = _osplatform;
_END_SECURE_CRT_DEPRECATION_DISABLE
    return 0;
}

/***
*errno_t _get_osver() - get _osver
*
*Purpose:
*       Get the value of _osver
*
*Entry:
*       unsigned int *pValue - pointer where to store the value
*
*Exit:
*       The error code
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/
errno_t _get_osver(unsigned int *pValue)
{
    /* validation section */
    _VALIDATE_RETURN_ERRCODE(pValue != NULL, EINVAL);

_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    _VALIDATE_RETURN_ERRCODE(_osplatform!=0, EINVAL);

    *pValue = _osver;
_END_SECURE_CRT_DEPRECATION_DISABLE
    return 0;
}

/***
*errno_t _get_winver() - get _winver
*
*Purpose:
*       Get the value of _winver
*
*Entry:
*       unsigned int *pValue - pointer where to store the value
*
*Exit:
*       The error code
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/
errno_t _get_winver(unsigned int *pValue)
{
    /* validation section */
    _VALIDATE_RETURN_ERRCODE(pValue != NULL, EINVAL);

_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    _VALIDATE_RETURN_ERRCODE(_osplatform!=0, EINVAL);

    *pValue = _winver;
_END_SECURE_CRT_DEPRECATION_DISABLE
    return 0;
}

/***
*errno_t _get_winmajor() - get _winver
*
*Purpose:
*       Get the value of _winmajor
*
*Entry:
*       unsigned int *pValue - pointer where to store the value
*
*Exit:
*       The error code
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/
errno_t _get_winmajor(unsigned int *pValue)
{
    /* validation section */
    _VALIDATE_RETURN_ERRCODE(pValue != NULL, EINVAL);

_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    _VALIDATE_RETURN_ERRCODE(_osplatform!=0, EINVAL);

    *pValue = _winmajor;
_END_SECURE_CRT_DEPRECATION_DISABLE
    return 0;
}

/***
*errno_t _get_winminor() - get _winminor
*
*Purpose:
*       Get the value of _winminor
*
*Entry:
*       unsigned int *pValue - pointer where to store the value
*
*Exit:
*       The error code
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/
errno_t _get_winminor(unsigned int *pValue)
{
    /* validation section */
    _VALIDATE_RETURN_ERRCODE(pValue != NULL, EINVAL);

_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    _VALIDATE_RETURN_ERRCODE(_osplatform!=0, EINVAL);

    *pValue = _winminor;
_END_SECURE_CRT_DEPRECATION_DISABLE
    return 0;
}

//////////////////////////////////////////////////////////////////////////
#ifndef _INTERNAL_IFSTRIP_
/* Retained for compatibility with VC++ 5.0 and earlier versions */
_CRTIMP unsigned int * __cdecl __p__osver(void)
{
	return &_osver;
}
_CRTIMP unsigned int * __cdecl __p__winver(void)
{
	return &_winver;
}
_CRTIMP unsigned int * __cdecl __p__winmajor(void)
{
	return &_winmajor;
}
_CRTIMP unsigned int * __cdecl __p__winminor(void)
{
	return &_winminor;
}

#endif  /* _INTERNAL_IFSTRIP_ */

BOOL os_crt0dat__init()
{
	unsigned int osplatform = 0;
	unsigned int winver = 0;
	unsigned int winmajor = 0;
	unsigned int winminor = 0;
	unsigned int osver = 0;

	OSVERSIONINFOA *posvi;
	/*
	* Dynamically allocate the OSVERSIONINFOA buffer, so we avoid
	* triggering the /GS buffer overrun detection.  That can't be
	* used here, since the guard cookie isn't available until we
	* initialize it from here!
	*/
	posvi = (OSVERSIONINFOA *)HeapAlloc(GetProcessHeap(), 0, sizeof(OSVERSIONINFOA));
	if (!posvi)
		return FALSE;

	/*
	* Get the full Win32 version.
	*/
	posvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if (!GetVersionExA(posvi)) {
		HeapFree(GetProcessHeap(), 0, posvi);
		return FALSE;
	}

	osplatform = posvi->dwPlatformId;
	winmajor = posvi->dwMajorVersion;
	winminor = posvi->dwMinorVersion;

	/*
	* The somewhat bizarre calculations of _osver and _winver are
	* required for backward compatibility (used to use GetVersion)
	*/
	osver = (posvi->dwBuildNumber) & 0x07fff;
	HeapFree(GetProcessHeap(), 0, posvi);
	posvi = NULL;
	if (osplatform != VER_PLATFORM_WIN32_NT)
		osver |= 0x08000;
	winver = (winmajor << 8) + winminor;

	_osplatform=osplatform;
	_winver=winver;
	_winmajor=winmajor;
	_winminor=winminor;
	_osver=osver;

	return TRUE;
}