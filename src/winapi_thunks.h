#pragma once
#include <corecrt.h>
#include <corecrt_internal.h>
#include <Windows.h>

EXTERN_C_START


#ifdef _ATL_XP_TARGETING
extern BOOL WINAPI __crtInitOnceExecuteOnce(
	_Inout_     PINIT_ONCE    InitOnce,
	_In_        PINIT_ONCE_FN InitFn,
	_Inout_opt_ PVOID         Parameter,
	_Out_opt_   LPVOID        *Context
);
#else
#define __crtInitOnceExecuteOnce InitOnceExecuteOnce
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C VOID WINAPI __crtInitializeConditionVariable(
	_Out_ PCONDITION_VARIABLE ConditionVariable
);
#else
#define __crtInitializeConditionVariable InitializeConditionVariable
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C BOOL WINAPI __crtSleepConditionVariableCS(
	_Inout_ PCONDITION_VARIABLE ConditionVariable,
	_Inout_ PCRITICAL_SECTION   CriticalSection,
	_In_    DWORD               dwMilliseconds
);
#else
#define __crtSleepConditionVariableCS SleepConditionVariableCS
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C void WINAPI __crtWakeConditionVariable(
	_Inout_ PCONDITION_VARIABLE ConditionVariable
);
#else
#define __crtWakeConditionVariable WakeConditionVariable
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C VOID __crtWakeAllConditionVariable(
	_Inout_ PCONDITION_VARIABLE ConditionVariable
);
#else
#define __crtWakeAllConditionVariable WakeAllConditionVariable
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C VOID WINAPI __crtInitializeSRWLock(
	_Out_ PSRWLOCK SRWLock
);
#else
#define __crtInitializeSRWLock InitializeSRWLock
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C VOID WINAPI __crtAcquireSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);
#else
#define __crtAcquireSRWLockExclusive AcquireSRWLockExclusive
#endif


EXTERN_C BOOLEAN WINAPI __crtTryAcquireSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);

#ifdef _ATL_XP_TARGETING
EXTERN_C VOID WINAPI __crtReleaseSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);
#else
#define __crtReleaseSRWLockExclusive ReleaseSRWLockExclusive
#endif

#ifdef _ATL_XP_TARGETING
EXTERN_C BOOL WINAPI __crtSleepConditionVariableSRW(
	_Inout_ PCONDITION_VARIABLE ConditionVariable,
	_Inout_ PSRWLOCK            SRWLock,
	_In_    DWORD               dwMilliseconds,
	_In_    ULONG               Flags
);
#else
#define __crtSleepConditionVariableSRW SleepConditionVariableSRW
#endif

EXTERN_C bool __cdecl __crt_are_win7_sync_apis_available();

#ifdef _ATL_XP_TARGETING
bool __cdecl __crt_are_vista_sync_apis_available();
#else
#define __crt_are_vista_sync_apis_available() true
#endif

__declspec(dllimport) int __cdecl __crtCompareStringA
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

__declspec(dllimport) int __cdecl __crtCompareStringW
(
    _In_ LCID     _Locale,
    _In_ DWORD    _DwCmpFlags,
    _In_count_(_CchCount1) LPCWSTR  _LpString1,
    _In_ int      _CchCount1,
    _In_count_(_CchCount2) LPCWSTR  _LpString2,
    _In_ int      _CchCount2
);


__declspec(dllimport) int __cdecl __crtLCMapStringW
(
    _In_ LCID _Locale,
    _In_ DWORD _DWMapFlag,
    _In_count_(_CchSrc) LPCWSTR _LpSrcStr,
    _In_ int _CchSrc,
    _Out_opt_cap_(_CchDest) LPWSTR _LpDestStr,
    _In_ int _CchDest
);


__declspec(dllimport) LCID* ___lc_handle_func(void);


__declspec(dllimport) int __cdecl __crtLCMapStringA
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

__declspec(dllimport) void _amsg_exit(
	int rterrnum
);

#define MakeMiniVersion(v1,v2) (DWORD)(v2|(v1<<16))
#define MakeVersion(v1,v2,v3,v4) (UINT64)(((UINT64)(v4))|((UINT64)v3<<16)|((UINT64)v2<<32)|((UINT64)v1<<48))

DWORD __cdecl __LTL_GetOsMinVersion();

UINT64 __cdecl __LTL_GetOsVersion();

EXTERN_C_END