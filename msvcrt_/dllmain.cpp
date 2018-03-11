// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

HMODULE hmod_MSVCR140__dll;

BOOL os_crt0dat__init();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		os_crt0dat__init();
	}
	break;
    case DLL_THREAD_ATTACH:
	{

	}
	break;
    case DLL_THREAD_DETACH:
	{

	}
	break;
    case DLL_PROCESS_DETACH:
	{

	}
	break;
    }
    return TRUE;
}

