// xmtx.h internal header
#pragma once
#ifndef _XMTX
#define _XMTX
#ifndef RC_INVOKED
#include <yvals.h>

#include <stdlib.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, 3)
#pragma push_macro("new")
#undef new

#ifndef MRTDLL
#ifndef _M_CEE_PURE
_EXTERN_C
#endif // _M_CEE_PURE
#endif // MRTDLL

#ifdef MRTDLL
#ifndef _M_CEE
// We want to declare Enter/LeaveCriticalSection as p/invoke
#define EnterCriticalSection _undefined_EnterCriticalSection
#define LeaveCriticalSection _undefined_LeaveCriticalSection
#endif // _M_CEE
#endif // MRTDLL

#include <Windows.h>

#ifdef MRTDLL
#ifndef _M_CEE
#undef EnterCriticalSection
#undef LeaveCriticalSection

_RELIABILITY_CONTRACT[System::Security::SuppressUnmanagedCodeSecurity]
                     [System::Runtime::InteropServices::DllImport("kernel32.dll")] void
    EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

_RELIABILITY_CONTRACT[System::Security::SuppressUnmanagedCodeSecurity]
                     [System::Runtime::InteropServices::DllImport("kernel32.dll")] void
    LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
#endif // _M_CEE
#endif // MRTDLL

typedef CRITICAL_SECTION _Rmtx;

#ifdef _M_CEE_PURE
void __clrcall _Mtxinit(_Rmtx*);
void __clrcall _Mtxdst(_Rmtx*);
void __clrcall _Mtxlock(_Rmtx*);
void __clrcall _Mtxunlock(_Rmtx*);

#else // _M_CEE_PURE
_MRTIMP2 void __cdecl _Mtxinit(_Rmtx*);
_MRTIMP2 void __cdecl _Mtxdst(_Rmtx*);
_MRTIMP2 void __cdecl _Mtxlock(_Rmtx*);
_MRTIMP2 void __cdecl _Mtxunlock(_Rmtx*);
#endif // _M_CEE_PURE

#ifndef MRTDLL
#ifndef _M_CEE_PURE
_END_EXTERN_C
#endif // _M_CEE_PURE
#endif // MRTDLL
#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
#endif // RC_INVOKED
#endif // _XMTX

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
