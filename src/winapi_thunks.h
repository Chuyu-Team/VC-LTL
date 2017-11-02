#pragma once
#include <Windows.h>

EXTERN_C_START

extern int WINAPI __acrt_LCIDToLocaleName(
	LCID   const locale,
	LPWSTR const name,
	int    const name_count,
	DWORD  const flags
);

extern LCID WINAPI __acrt_LocaleNameToLCID(
	LPCWSTR const name,
	DWORD   const flags
);

extern VOID WINAPI __acrt_GetSystemTimePreciseAsFileTime(LPFILETIME const system_time);


extern BOOL WINAPI __crtInitOnceExecuteOnce(
	_Inout_     PINIT_ONCE    InitOnce,
	_In_        PINIT_ONCE_FN InitFn,
	_Inout_opt_ PVOID         Parameter,
	_Out_opt_   LPVOID        *Context
);

EXTERN_C VOID WINAPI __crtInitializeConditionVariable(
	_Out_ PCONDITION_VARIABLE ConditionVariable
);

EXTERN_C BOOL WINAPI __crtSleepConditionVariableCS(
	_Inout_ PCONDITION_VARIABLE ConditionVariable,
	_Inout_ PCRITICAL_SECTION   CriticalSection,
	_In_    DWORD               dwMilliseconds
);

EXTERN_C void WINAPI __crtWakeConditionVariable(
	_Inout_ PCONDITION_VARIABLE ConditionVariable
);

EXTERN_C VOID __crtWakeAllConditionVariable(
	_Inout_ PCONDITION_VARIABLE ConditionVariable
);

EXTERN_C VOID WINAPI __crtInitializeSRWLock(
	_Out_ PSRWLOCK SRWLock
);


EXTERN_C VOID WINAPI __crtAcquireSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);


EXTERN_C BOOLEAN WINAPI __crtTryAcquireSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);

EXTERN_C VOID WINAPI __crtReleaseSRWLockExclusive(
	_Inout_ PSRWLOCK SRWLock
);


EXTERN_C BOOL WINAPI __crtSleepConditionVariableSRW(
	_Inout_ PCONDITION_VARIABLE ConditionVariable,
	_Inout_ PSRWLOCK            SRWLock,
	_In_    DWORD               dwMilliseconds,
	_In_    ULONG               Flags
);

//EXTERN_C bool __cdecl __crt_are_win7_sync_apis_available();

//EXTERN_C bool __cdecl __crt_are_vista_sync_apis_available();

int __cdecl __crtCompareStringA
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

int __cdecl __crtCompareStringW
(
    _In_ LCID     _Locale,
    _In_ DWORD    _DwCmpFlags,
    _In_count_(_CchCount1) LPCWSTR  _LpString1,
    _In_ int      _CchCount1,
    _In_count_(_CchCount2) LPCWSTR  _LpString2,
    _In_ int      _CchCount2
);


int __cdecl __crtLCMapStringW
(
    _In_ LCID _Locale,
    _In_ DWORD _DWMapFlag,
    _In_count_(_CchSrc) LPCWSTR _LpSrcStr,
    _In_ int _CchSrc,
    _Out_opt_cap_(_CchDest) LPWSTR _LpDestStr,
    _In_ int _CchDest
);


LCID* ___lc_handle_func(void);


int __cdecl __crtLCMapStringA
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

void _amsg_exit(
	int rterrnum
);

#define MakeMiniVersion(v1,v2) (DWORD)(v2|(v1<<16))
#define MakeVersion(v1,v2,v3,v4) (UINT64)(((UINT64)(v4))|((UINT64)v3<<16)|((UINT64)v2<<32)|((UINT64)v1<<48))

DWORD __LTL_GetOsMinVersion();

UINT64 __LTL_GetOsVersion();

EXTERN_C_END