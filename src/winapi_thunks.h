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


EXTERN_C_END