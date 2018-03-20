#pragma once
#include <corecrt.h>
#include <corecrt_internal.h>
#include <Windows.h>

EXTERN_C_START

#define MakeMiniVersion(v1,v2) (DWORD)(v2|(v1<<16))
#define MakeVersion(v1,v2,v3,v4) (UINT64)(((UINT64)(v4))|((UINT64)v3<<16)|((UINT64)v2<<32)|((UINT64)v1<<48))

DWORD __cdecl __LTL_GetOsMinVersion();

UINT64 __cdecl __LTL_GetOsVersion();


#ifdef _ATL_XP_TARGETING
BOOL WINAPI __crtInitOnceExecuteOnce(
	_Inout_     PINIT_ONCE    InitOnce,
	_In_        PINIT_ONCE_FN InitFn,
	_Inout_opt_ PVOID         Parameter,
	_Out_opt_   LPVOID        *Context
);
#else
#define __crtInitOnceExecuteOnce InitOnceExecuteOnce
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtInitializeConditionVariable(
	_Out_ PCONDITION_VARIABLE ConditionVariable
);
#else
#define __crtInitializeConditionVariable InitializeConditionVariable
#endif

#ifdef _ATL_XP_TARGETING
BOOL WINAPI __crtSleepConditionVariableCS(
	_Inout_ PCONDITION_VARIABLE ConditionVariable,
	_Inout_ PCRITICAL_SECTION   CriticalSection,
	_In_    DWORD               dwMilliseconds
);
#else
#define __crtSleepConditionVariableCS SleepConditionVariableCS
#endif

#ifdef _ATL_XP_TARGETING
void WINAPI __crtWakeConditionVariable(
	_Inout_ PCONDITION_VARIABLE ConditionVariable
);
#else
#define __crtWakeConditionVariable WakeConditionVariable
#endif

#ifdef _ATL_XP_TARGETING
VOID __crtWakeAllConditionVariable(
	_Inout_ PCONDITION_VARIABLE ConditionVariable
);
#else
#define __crtWakeAllConditionVariable WakeAllConditionVariable
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtInitializeSRWLock(
	_Out_ PSRWLOCK SRWLock
);
#else
#define __crtInitializeSRWLock InitializeSRWLock
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtAcquireSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);
#else
#define __crtAcquireSRWLockExclusive AcquireSRWLockExclusive
#endif

//Win7以上版本可用
BOOLEAN WINAPI __crtTryAcquireSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtReleaseSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);
#else
#define __crtReleaseSRWLockExclusive ReleaseSRWLockExclusive
#endif

#ifdef _ATL_XP_TARGETING
BOOL WINAPI __crtSleepConditionVariableSRW(
	_Inout_ PCONDITION_VARIABLE ConditionVariable,
	_Inout_ PSRWLOCK            SRWLock,
	_In_    DWORD               dwMilliseconds,
	_In_    ULONG               Flags
);
#else
#define __crtSleepConditionVariableSRW SleepConditionVariableSRW
#endif

BOOLEAN __cdecl __crt_are_win7_sync_apis_available();

#ifdef _ATL_XP_TARGETING
BOOLEAN __cdecl __crt_are_vista_sync_apis_available();
#else
#define __crt_are_vista_sync_apis_available() true
#endif

#ifdef _ATL_XP_TARGETING
BOOLEAN __cdecl __crt__Is_vista_threadpool_supported();
#else
#define __crt__Is_vista_threadpool_supported() true
#endif


#ifdef _ATL_XP_TARGETING
VOID
WINAPI
__crtFreeLibraryWhenCallbackReturns(
	_Inout_ PTP_CALLBACK_INSTANCE pci,
	_In_ HMODULE mod
	);
#else
#define __crtFreeLibraryWhenCallbackReturns FreeLibraryWhenCallbackReturns
#endif

#ifdef _ATL_XP_TARGETING
VOID
WINAPI
__crtCloseThreadpoolWork(
	_Inout_ PTP_WORK pwk
	);
#else
#define __crtCloseThreadpoolWork CloseThreadpoolWork
#endif

#ifdef _ATL_XP_TARGETING
VOID
WINAPI
__crtSubmitThreadpoolWork(
	_Inout_ PTP_WORK pwk
	);
#else
#define __crtSubmitThreadpoolWork SubmitThreadpoolWork
#endif

#ifdef _ATL_XP_TARGETING
PTP_WORK
WINAPI
__crtCreateThreadpoolWork(
	_In_ PTP_WORK_CALLBACK pfnwk,
	_Inout_opt_ PVOID pv,
	_In_opt_ PTP_CALLBACK_ENVIRON pcbe
	);
#else
#define __crtCreateThreadpoolWork CreateThreadpoolWork
#endif

#define __crtQueueUserWorkItem QueueUserWorkItem

#define __crtInitializeCriticalSectionEx __acrt_InitializeCriticalSectionEx
#define __crtGetSystemTimePreciseAsFileTime __acrt_GetSystemTimePreciseAsFileTime

#ifdef _ATL_XP_TARGETING
BOOLEAN
WINAPI
__crtCreateSymbolicLinkW(
	_In_ LPCWSTR lpSymlinkFileName,
	_In_ LPCWSTR lpTargetFileName,
	_In_ DWORD dwFlags
);
#else
#define __crtCreateSymbolicLinkW CreateSymbolicLinkW
#endif

#ifdef _ATL_XP_TARGETING
PVOID __fastcall __CRT_DecodePointer(
	PVOID Ptr
);

PVOID __fastcall __CRT_EncodePointer(PVOID const Ptr);

#define DecodePointerDownlevel(p) __CRT_DecodePointer
#define EncodePointerDownlevel(p) __CRT_EncodePointer
#else
#define DecodePointerDownlevel DecodePointer
#define EncodePointerDownlevel EncodePointer
#endif


#ifdef _ATL_XP_TARGETING
#define __crtCreateEventExW(lpEventAttributes,lpName,dwFlags,dwDesiredAccess) CreateEvent(lpEventAttributes,(dwFlags&CREATE_EVENT_MANUAL_RESET),(dwFlags&CREATE_EVENT_INITIAL_SET),lpName)
#else
#define __crtCreateEventExW CreateEventExW
#endif

#ifdef _ATL_XP_TARGETING
ULONGLONG WINAPI __crtGetTickCount64(VOID);
#else
#define __crtGetTickCount64 GetTickCount64
#endif

#if defined _ATL_XP_TARGETING && defined _X86_
DWORD WINAPI __crtGetCurrentProcessorNumber(void);
#else
#define __crtGetCurrentProcessorNumber GetCurrentProcessorNumber
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtFlushProcessWriteBuffers(void);
#else
#define __crtFlushProcessWriteBuffers FlushProcessWriteBuffers
#endif

#ifdef _ATL_XP_TARGETING
#define __crtCreateSemaphoreExW(lpSemaphoreAttributes,lInitialCount,lMaximumCount,lpName,dwFlags,dwDesiredAccess) CreateSemaphoreW(lpSemaphoreAttributes,lInitialCount,lMaximumCount,lpName)
#else
#define __crtCreateSemaphoreExW CreateSemaphoreExW
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtSetThreadpoolTimer(
	_Inout_ PTP_TIMER pti,
	_In_opt_ PFILETIME pftDueTime,
	_In_ DWORD msPeriod,
	_In_opt_ DWORD msWindowLength
	);
#else
#define __crtSetThreadpoolTimer SetThreadpoolTimer
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtSetThreadpoolWait(
	_Inout_ PTP_WAIT pwa,
	_In_opt_ HANDLE h,
	_In_opt_ PFILETIME pftTimeout
	);
#else
#define __crtSetThreadpoolWait SetThreadpoolWait
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtCloseThreadpoolWait(
	_Inout_ PTP_WAIT pwa
	);
#else
#define __crtCloseThreadpoolWait CloseThreadpoolWait
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtWaitForThreadpoolTimerCallbacks(
	_Inout_ PTP_TIMER pti,
	_In_ BOOL fCancelPendingCallbacks
	);
#else
#define __crtWaitForThreadpoolTimerCallbacks WaitForThreadpoolTimerCallbacks
#endif

#ifdef _ATL_XP_TARGETING
PTP_WAIT WINAPI __crtCreateThreadpoolWait(
	_In_ PTP_WAIT_CALLBACK pfnwa,
	_Inout_opt_ PVOID pv,
	_In_opt_ PTP_CALLBACK_ENVIRON pcbe
	);
#else
#define __crtCreateThreadpoolWait CreateThreadpoolWait
#endif

#ifdef _ATL_XP_TARGETING
VOID WINAPI __crtCloseThreadpoolTimer(
	_Inout_ PTP_TIMER pti
	);
#else
#define __crtCloseThreadpoolTimer CloseThreadpoolTimer 
#endif

#ifdef _ATL_XP_TARGETING
PTP_TIMER WINAPI __crtCreateThreadpoolTimer(
	_In_ PTP_TIMER_CALLBACK pfnti,
	_Inout_opt_ PVOID pv,
	_In_opt_ PTP_CALLBACK_ENVIRON pcbe
	);
#else
#define __crtCreateThreadpoolTimer CreateThreadpoolTimer
#endif

_CRTIMP int __cdecl __crtCompareStringA(LCID, ...);

_CRTIMP int __cdecl __crtCompareStringA_WinXP
(
    _In_ LCID     _Locale,
    _In_ DWORD    _DwCmpFlags,
    _In_count_(_CchCount1) LPCSTR   _LpString1,
    _In_ int      _CchCount1,
    _In_count_(_CchCount2) LPCSTR   _LpString2,
    _In_ int      _CchCount2,
    _In_ int      _Code_page
);

_CRTIMP int __cdecl __crtCompareStringA_Vista
(
	_In_opt_ _locale_t _Plocinfo,
	_In_ LCID     _Locale,
	_In_ DWORD    _DwCmpFlags,
	_In_count_(_CchCount1) LPCSTR   _LpString1,
	_In_ int      _CchCount1,
	_In_count_(_CchCount2) LPCSTR   _LpString2,
	_In_ int      _CchCount2,
	_In_ int      _Code_page
);

__forceinline int __cdecl __crtCompareStringA_Current
(
	_In_ LCID     _Locale,
	_In_ DWORD    _DwCmpFlags,
	_In_count_(_CchCount1) LPCSTR   _LpString1,
	_In_ int      _CchCount1,
	_In_count_(_CchCount2) LPCSTR   _LpString2,
	_In_ int      _CchCount2,
	_In_ int      _Code_page
)
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < MakeMiniVersion(6, 0))
	{
		return __crtCompareStringA(_Locale, _DwCmpFlags, _LpString1, _CchCount1, _LpString2, _CchCount2, _Code_page);
	}
	else
#endif
	{
		return __crtCompareStringA(0,_Locale, _DwCmpFlags, _LpString1, _CchCount1, _LpString2, _CchCount2, _Code_page);
	}
}

#define __crtCompareStringA __crtCompareStringA_Current

_CRTIMP int __cdecl __crtCompareStringW(LCID, ...);

_CRTIMP int __cdecl __crtCompareStringW_WinXP
(
    _In_ LCID     _Locale,
    _In_ DWORD    _DwCmpFlags,
    _In_count_(_CchCount1) LPCWSTR  _LpString1,
    _In_ int      _CchCount1,
    _In_count_(_CchCount2) LPCWSTR  _LpString2,
    _In_ int      _CchCount2
);

_CRTIMP int __cdecl __crtCompareStringW_Vista
(
	_In_opt_ _locale_t _Plocinfo,
	_In_ LCID     _Locale,
	_In_ DWORD    _DwCmpFlags,
	_In_count_(_CchCount1) LPCWSTR  _LpString1,
	_In_ int      _CchCount1,
	_In_count_(_CchCount2) LPCWSTR  _LpString2,
	_In_ int      _CchCount2
);

__forceinline int __cdecl __crtCompareStringW_Current
(
	_In_ LCID     _Locale,
	_In_ DWORD    _DwCmpFlags,
	_In_count_(_CchCount1) LPCWSTR  _LpString1,
	_In_ int      _CchCount1,
	_In_count_(_CchCount2) LPCWSTR  _LpString2,
	_In_ int      _CchCount2
)
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < MakeMiniVersion(6, 0))
	{
		return __crtCompareStringW(_Locale, _DwCmpFlags, _LpString1, _CchCount1, _LpString2, _CchCount2);
	}
	else
#endif
	{
		return __crtCompareStringW(0,_Locale, _DwCmpFlags, _LpString1, _CchCount1, _LpString2, _CchCount2);
	}
}

#define __crtCompareStringW __crtCompareStringW_Current

_CRTIMP int __cdecl __crtLCMapStringW(LCID, ...);

_CRTIMP int __cdecl __crtLCMapStringW_WinXP
(
    _In_ LCID _Locale,
    _In_ DWORD _DWMapFlag,
    _In_count_(_CchSrc) LPCWSTR _LpSrcStr,
    _In_ int _CchSrc,
    _Out_opt_cap_(_CchDest) LPWSTR _LpDestStr,
    _In_ int _CchDest
);

_CRTIMP int __cdecl __crtLCMapStringW_Vista
(
	_In_opt_ _locale_t _Plocinfo,
	_In_ LCID _Locale,
	_In_ DWORD _DWMapFlag,
	_In_count_(_CchSrc) LPCWSTR _LpSrcStr,
	_In_ int _CchSrc,
	_Out_opt_cap_(_CchDest) LPWSTR _LpDestStr,
	_In_ int _CchDest
);

__forceinline int __cdecl __crtLCMapStringW_Current
(
	_In_ LCID _Locale,
	_In_ DWORD _DWMapFlag,
	_In_count_(_CchSrc) LPCWSTR _LpSrcStr,
	_In_ int _CchSrc,
	_Out_opt_cap_(_CchDest) LPWSTR _LpDestStr,
	_In_ int _CchDest
)
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < MakeMiniVersion(6, 0))
	{
		return __crtLCMapStringW(_Locale, _DWMapFlag, _LpSrcStr, _CchSrc, _LpDestStr, _CchDest);
	}
	else
#endif
	{
		return __crtLCMapStringW(0, _Locale, _DWMapFlag, _LpSrcStr, _CchSrc, _LpDestStr, _CchDest);
	}
}

#define __crtLCMapStringW __crtLCMapStringW_Current

_CRTIMP int __cdecl __crtLCMapStringA(LCID, ...);

_CRTIMP int __cdecl __crtLCMapStringA_WinXP
(
	_In_ LCID _Locale,
	_In_ DWORD _DwMapFlag,
	_In_count_(_CchSrc) LPCSTR _LpSrcStr,
	_In_ int _CchSrc,
	_Out_opt_cap_(_CchDest) LPSTR _LpDestStr,
	_In_ int _CchDest,
	_In_ int _Code_page,
	_In_ BOOL _BError
);

_CRTIMP int __cdecl __crtLCMapStringA_Vista
(
	_In_opt_ _locale_t _Plocinfo,
	_In_ LCID _Locale,
	_In_ DWORD _DwMapFlag,
	_In_count_(_CchSrc) LPCSTR _LpSrcStr,
	_In_ int _CchSrc,
	_Out_opt_cap_(_CchDest) LPSTR _LpDestStr,
	_In_ int _CchDest,
	_In_ int _Code_page,
	_In_ BOOL _BError
);

__forceinline int __cdecl __crtLCMapStringA_Current
(
	_In_ LCID _Locale,
	_In_ DWORD _DwMapFlag,
	_In_count_(_CchSrc) LPCSTR _LpSrcStr,
	_In_ int _CchSrc,
	_Out_opt_cap_(_CchDest) LPSTR _LpDestStr,
	_In_ int _CchDest,
	_In_ int _Code_page,
	_In_ BOOL _BError
)
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < MakeMiniVersion(6, 0))
	{
		return __crtLCMapStringA(_Locale, _DwMapFlag, _LpSrcStr, _CchSrc, _LpDestStr, _CchDest, _Code_page, _BError);
	}
	else
#endif
	{
		return __crtLCMapStringA(0, _Locale, _DwMapFlag, _LpSrcStr, _CchSrc, _LpDestStr, _CchDest, _Code_page, _BError);
	}
}

#define __crtLCMapStringA __crtLCMapStringA_Current

__declspec(dllimport) LCID* __cdecl ___lc_handle_func(void);

/* Lock symbols */

#define _SIGNAL_LOCK    0       /* lock for signal()                */
#define _IOB_SCAN_LOCK  1       /* _iob[] table lock                */
#define _TMPNAM_LOCK    2       /* lock global tempnam variables    */
#define _CONIO_LOCK     3       /* lock for conio routines          */
#define _HEAP_LOCK      4       /* lock for heap allocator routines */
#define _UNDNAME_LOCK   5       /* lock for unDName() routine       */
#define _TIME_LOCK      6       /* lock for time functions          */
#define _ENV_LOCK       7       /* lock for environment variables   */
#define _EXIT_LOCK1     8       /* lock #1 for exit code            */
#define _POPEN_LOCK     9       /* lock for _popen/_pclose database */
#define _LOCKTAB_LOCK   10      /* lock to protect semaphore lock table */
#define _OSFHND_LOCK    11      /* lock to protect _osfhnd array    */
#define _SETLOCALE_LOCK 12      /* lock for locale handles, etc.    */
#define _MB_CP_LOCK     13      /* lock for multibyte code page     */
#define _TYPEINFO_LOCK  14      /* lock for type_info access        */
#define _DEBUG_LOCK     15      /* lock for debug global structs    */

#define _STREAM_LOCKS   16      /* Table of stream locks            */

__declspec(dllimport) void __cdecl _lock(
	int locknum
);
__declspec(dllimport) void __cdecl _unlock(
	int locknum
);

__declspec(dllimport) void __cdecl _amsg_exit(
	int rterrnum
);


EXTERN_C_END