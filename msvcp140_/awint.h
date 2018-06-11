#include "winapi_thunks.h"

#define KERNEL32 L"kernel32.dll"

enum wrapEncodedKERNEL32Functions
{
	eFlsAlloc = 0,
	eFlsFree,
	eFlsGetValue,
	eFlsSetValue,
	eInitializeCriticalSectionEx,
	eInitOnceExecuteOnce,
	eCreateEventExW,
	eCreateSemaphoreW,
	eCreateSemaphoreExW,
	eCreateThreadpoolTimer,
	eSetThreadpoolTimer,
	eWaitForThreadpoolTimerCallbacks,
	eCloseThreadpoolTimer,
	eCreateThreadpoolWait,
	eSetThreadpoolWait,
	eCloseThreadpoolWait,
	eFlushProcessWriteBuffers,
	eFreeLibraryWhenCallbackReturns,
	eGetCurrentProcessorNumber,
	eCreateSymbolicLinkW,
	eGetCurrentPackageId,
	eGetTickCount64,
	eGetFileInformationByHandleEx,
	eSetFileInformationByHandle,
	eGetSystemTimePreciseAsFileTime,
	eInitializeConditionVariable,
	eWakeConditionVariable,
	eWakeAllConditionVariable,
	eSleepConditionVariableCS,
	eInitializeSRWLock,
	eAcquireSRWLockExclusive,
	eTryAcquireSRWLockExclusive,
	eReleaseSRWLockExclusive,
	eSleepConditionVariableSRW,
	eCreateThreadpoolWork,
	eSubmitThreadpoolWork,
	eCloseThreadpoolWork,
	eCompareStringEx,
	eGetLocaleInfoEx,
	eLCMapStringEx,
	eMaxKernel32Function
};

EXTERN_C PVOID __encodedKERNEL32Functions[eMaxKernel32Function];

/* Needed for pointer encoding */
extern UINT_PTR __security_cookie;

typedef DWORD(WINAPI *PFNFLSALLOC)(PFLS_CALLBACK_FUNCTION);
typedef BOOL(WINAPI *PFNFLSFREE)(DWORD);
typedef PVOID(WINAPI *PFNFLSGETVALUE)(DWORD);
typedef BOOL(WINAPI *PFNFLSSETVALUE)(DWORD, PVOID);
typedef BOOL(WINAPI *PFNINITIALIZECRITICALSECTIONEX)(LPCRITICAL_SECTION, DWORD, DWORD);
typedef HANDLE(WINAPI *PFNCREATEEVENTEXW)(LPSECURITY_ATTRIBUTES, LPCWSTR, DWORD, DWORD);
typedef HANDLE(WINAPI *PFNCREATESEMAPHOREEXW)(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCWSTR, DWORD, DWORD);
typedef BOOL(WINAPI *PFNSETTHREADSTACKGUARANTEE)(PULONG);
typedef PTP_TIMER(WINAPI *PFNCREATETHREADPOOLTIMER)(PTP_TIMER_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
typedef VOID(WINAPI *PFNSETTHREADPOOLTIMER)(PTP_TIMER, PFILETIME, DWORD, DWORD);
typedef VOID(WINAPI *PFNWAITFORTHREADPOOLTIMERCALLBACKS)(PTP_TIMER, BOOL);
typedef VOID(WINAPI *PFNCLOSETHREADPOOLTIMER)(PTP_TIMER);
typedef PTP_WAIT(WINAPI *PFNCREATETHREADPOOLWAIT)(PTP_WAIT_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON);
typedef VOID(WINAPI *PFNSETTHREADPOOLWAIT)(PTP_WAIT, HANDLE, PFILETIME);
typedef VOID(WINAPI *PFNCLOSETHREADPOOLWAIT)(PTP_WAIT);
typedef VOID(WINAPI *PFNFLUSHPROCESSWRITEBUFFERS)(void);
typedef VOID(WINAPI *PFNFREELIBRARYWHENCALLBACKRETURNS)(PTP_CALLBACK_INSTANCE, HMODULE);
typedef DWORD(WINAPI *PFNGETCURRENTPROCESSORNUMBER)(void);
typedef BOOL(WINAPI *PFNGETLOGICALPROCESSORINFORMATION)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
typedef BOOLEAN(WINAPI *PFNCREATESYMBOLICLINK)(LPCWSTR, LPCWSTR, DWORD);
typedef BOOL(WINAPI *PFNENUMSYSTEMLOCALESEX)(LOCALE_ENUMPROCEX, DWORD, LPARAM, LPVOID);
typedef int (WINAPI *PFNCOMPARESTRINGEX)(LPCWSTR, DWORD, LPCWSTR, int, LPCWSTR, int, LPNLSVERSIONINFO, LPVOID, LPARAM);
typedef int (WINAPI *PFNGETDATEFORMATEX)(LPCWSTR, DWORD, const SYSTEMTIME *, LPCWSTR, LPWSTR, int, LPCWSTR);
typedef int (WINAPI *PFNGETLOCALEINFOEX)(LPCWSTR, LCTYPE, LPWSTR, int);
typedef int (WINAPI *PFNGETTIMEFORMATEX)(LPCWSTR, DWORD, const SYSTEMTIME *, LPCWSTR, LPWSTR, int);
typedef int (WINAPI *PFNGETUSERDEFAULTLOCALENAME)(LPWSTR, int);
typedef BOOL(WINAPI *PFNISVALIDLOCALENAME)(LPCWSTR);
typedef int (WINAPI *PFNLCMAPSTRINGEX)(LPCWSTR, DWORD, LPCWSTR, int, LPWSTR, int, LPNLSVERSIONINFO, LPVOID, LPARAM);
typedef ULONGLONG(WINAPI *PFNGETTICKCOUNT64)(void);
typedef BOOL(WINAPI *PFNGETFILEINFORMATIONBYHANDLEEX)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);
typedef BOOL(WINAPI *PFNSETFILEINFORMATIONBYHANDLE)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);
typedef BOOLEAN
(WINAPI*
	PFNTRYACQUIRESRWLOCKEXCLUSIVE)(
	_Inout_ PSRWLOCK SRWLock
);
typedef VOID
(WINAPI*
	PFNINITIALIZECONDITIONVARIABLE)(
	_Out_ PCONDITION_VARIABLE ConditionVariable
);
typedef PTP_WORK
(WINAPI*
	PFNCREATETHREADPOOLWORK)(
	_In_ PTP_WORK_CALLBACK pfnwk,
	_Inout_opt_ PVOID pv,
	_In_opt_ PTP_CALLBACK_ENVIRON pcbe
);

// Use this macro for encoding and caching a function pointer from a DLL
#define STOREENCODEDFUNCTIONPOINTER(instance, libraryname, functionname) \
    encoded##libraryname##Functions[e##functionname] = (PVOID)(((UINT_PTR)GetProcAddress(instance, #functionname)) ^ __security_cookie);

// Use this macro for decoding a cached function pointer from a DLL
#define IFDYNAMICGETCACHEDFUNCTION(libraryname, functiontypedef, functionname, functionpointer) \
    functiontypedef functionpointer = (functiontypedef)(((UINT_PTR)__encoded##libraryname##Functions[e##functionname]) ^ __security_cookie); \
    if (functionpointer != NULL)

#define DYNAMICGETCACHEDFUNCTION(libraryname, functiontypedef, functionname, functionpointer) \
    functiontypedef functionpointer = (functiontypedef)(((UINT_PTR)__encoded##libraryname##Functions[e##functionname]) ^ __security_cookie);

