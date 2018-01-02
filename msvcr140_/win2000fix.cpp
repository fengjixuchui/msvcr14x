#define NONAMELESSUNION
#include "StdAfx.h"
#pragma comment(lib,"win2k_KERNEL32.lib")
#pragma comment(lib,"win2k_kernl32p.lib")

/*__declspec(naked) */__int64 interlocked_cmpxchg64(__int64 *dest, __int64 xchg, __int64 compare)
{
	//__asm push ebx;
	//__asm push esi;
	//__asm mov esi, 12[esp];
	//__asm mov ebx, 16[esp];
	//__asm mov ecx, 20[esp];
	//__asm mov eax, 24[esp];
	//__asm mov edx, 28[esp];
	//__asm lock cmpxchg8b[esi];
	//__asm pop esi;
	//__asm pop ebx;
	//__asm ret;
	return _InterlockedCompareExchange64(dest, xchg, compare);
}

extern "C"
{
	PVOID WINAPI EncodePointer(__in_opt PVOID Ptr)
	{
		static PVOID(WINAPI* Real_EncodePointer)(__in_opt PVOID Ptr) = (PVOID(__stdcall *)(PVOID))GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "EncodePointer");

		if (Real_EncodePointer)
		{
			return Real_EncodePointer(Ptr);
		}
		else
		{
			return Ptr;
		}
	}


	__out_opt PVOID WINAPI DecodePointer(__in_opt PVOID Ptr)
	{
		static __out_opt PVOID(WINAPI* Real_DecodePointer)(__in_opt PVOID Ptr) = (PVOID(__stdcall *)(PVOID))GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "DecodePointer");

		if (Real_DecodePointer)
		{
			return Real_DecodePointer(Ptr);
		}
		else
		{
			return Ptr;
		}
	}

	BOOL WINAPI GetModuleHandleExW(__in  DWORD dwFlags, __in_opt LPCWSTR lpModuleName, __out HMODULE* phModule)
	{
		static BOOL(WINAPI* Real_GetModuleHandleExW)(__in  DWORD dwFlags, __in_opt LPCWSTR lpModuleName, __out HMODULE* phModule) =
			(BOOL(__stdcall *)(DWORD, LPCWSTR, HMODULE *))GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "GetModuleHandleExW");

		if (Real_GetModuleHandleExW)
		{
			return Real_GetModuleHandleExW(dwFlags, lpModuleName, phModule);
		}
		else
		{
			HMODULE hModule = GetModuleHandleW(lpModuleName);
			if (phModule)
			{
				*phModule = hModule;
			}
			return hModule != NULL;
		}
	}

	int WINAPI CompareStringA(__in LCID Locale, __in DWORD dwCmpFlags, __in_ecount(cchCount1) PCNZCH lpString1, __in int cchCount1, __in_ecount(cchCount2) PCNZCH lpString2, __in int cchCount2)
	{
		static int (WINAPI* Real_CompareStringA)(__in LCID Locale, __in DWORD dwCmpFlags, __in_ecount(cchCount1) PCNZCH lpString1, __in int cchCount1, __in_ecount(cchCount2) PCNZCH lpString2, __in int cchCount2) =
			(int(__stdcall *)(LCID, DWORD, PCNZCH, int, PCNZCH, int))GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "CompareStringA");
		
		if (Locale == LOCALE_INVARIANT)
		{
			if (GetWindowsVersionEx(FALSE) <= 0x0500)
			{
				Locale = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
			}
		}
		return Real_CompareStringA(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
	}

	int WINAPI CompareStringW(__in LCID Locale, __in DWORD dwCmpFlags, __in_ecount(cchCount1) PCNZWCH lpString1, __in int cchCount1, __in_ecount(cchCount2) PCNZWCH lpString2,__in int cchCount2)
	{
		static int (WINAPI* Real_CompareStringW)(__in LCID Locale, __in DWORD dwCmpFlags, __in_ecount(cchCount1) PCNZWCH lpString1, __in int cchCount1, __in_ecount(cchCount2) PCNZWCH lpString2, __in int cchCount2) =
			(int(__stdcall *)(LCID, DWORD, PCNZWCH, int, PCNZWCH, int))GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "CompareStringW");

		if (Locale == LOCALE_INVARIANT)
		{
			if (GetWindowsVersionEx(FALSE) <= 0x0500)
			{
				Locale = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
			}
		}

		return Real_CompareStringW(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
	}

	/*************************************************************************
	* RtlInterlockedFlushSList   [NTDLL.@]
	*/
	PSLIST_ENTRY WINAPI RtlInterlockedFlushSList(PSLIST_HEADER list)
	{
		SLIST_HEADER old, _new;

#ifdef _WIN64
		if (!list->Header16.NextEntry) return NULL;
		_new.s.Alignment = _new.s.Region = 0;
		_new.Header16.HeaderType = 1;  /* we use the 16-byte header */
		do
		{
			old = *list;
			_new.Header16.Sequence = old.Header16.Sequence + 1;
		} while (!interlocked_cmpxchg128((__int64 *)list, _new.s.Region, _new.s.Alignment, (__int64 *)&old));
		return (SLIST_ENTRY *)((ULONG_PTR)old.Header16.NextEntry << 4);
#else
		if (!list->s.Next.Next) return NULL;
		_new.Alignment = 0;
		do
		{
			old = *list;
#if VER_PRODUCTBUILD > 7600
			_new.s.CpuId = old.s.CpuId + 1;
#else
			_new.s.Sequence = old.s.Sequence + 1;
#endif
		} while (interlocked_cmpxchg64((__int64 *)&list->Alignment, _new.Alignment,
			old.Alignment) != old.Alignment);
		return old.s.Next.Next;
#endif
	}

	PSLIST_ENTRY WINAPI	InterlockedFlushSList(__inout PSLIST_HEADER ListHead)
	{
		return RtlInterlockedFlushSList(ListHead);
	}

	/*************************************************************************
	* RtlInterlockedPushEntrySList   [NTDLL.@]
	*/
	PSLIST_ENTRY WINAPI RtlInterlockedPushEntrySList(PSLIST_HEADER list, PSLIST_ENTRY entry)
	{
		SLIST_HEADER old, _new;

#ifdef _WIN64
		new.Header16.NextEntry = (ULONG_PTR)entry >> 4;
		do
		{
			old = *list;
			entry->Next = (SLIST_ENTRY *)((ULONG_PTR)old.Header16.NextEntry << 4);
			_new.Header16.Depth = old.Header16.Depth + 1;
			_new.Header16.Sequence = old.Header16.Sequence + 1;
		} while (!interlocked_cmpxchg128((__int64 *)list, _new.s.Region, _new.s.Alignment, (__int64 *)&old));
		return (SLIST_ENTRY *)((ULONG_PTR)old.Header16.NextEntry << 4);
#else
		_new.s.Next.Next = entry;
		do
		{
			old = *list;
			entry->Next = old.s.Next.Next;
			_new.s.Depth = old.s.Depth + 1;
#if VER_PRODUCTBUILD > 7600
			_new.s.CpuId = old.s.CpuId + 1;
#else
			_new.s.Sequence = old.s.Sequence + 1;
#endif
		} while (interlocked_cmpxchg64((__int64 *)&list->Alignment, _new.Alignment,
			old.Alignment) != old.Alignment);
		return old.s.Next.Next;
#endif
	}

	PSLIST_ENTRY WINAPI	InterlockedPushEntrySList(_Inout_ PSLIST_HEADER ListHead, _Inout_ __drv_aliasesMem PSLIST_ENTRY ListEntry)
	{
		return RtlInterlockedPushEntrySList(ListHead, ListEntry);
	}
}