//
// cfguard.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Declarations of Control Flow Guard instrumentation and support functions.
//
#pragma once
#ifndef _CFGUARD_H
#define _CFGUARD_H

#include <sal.h>
#include <vadefs.h>

#pragma warning(push)
#pragma warning(disable: _VCRUNTIME_DISABLED_WARNINGS)

#pragma pack(push, _CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__midl) && !defined(MIDL_PASS)

    #if defined(_M_ARM64EC)
        #define _guard_check_icall __os_arm64x_check_icall_thunk
    #elif defined(_M_ARM64)
        #define _guard_check_icall __guard_check_icall_thunk
    #endif

    #ifdef _M_CEE
        // This is a placeholder. Control Flow Guard is not implemented for /clr.
        void __cdecl _guard_check_icall(_In_ uintptr_t _Target);
    #else
        void __fastcall _guard_check_icall(_In_ uintptr_t _Target);
    #endif

    int __cdecl _guard_icall_checks_enforced(void);

#endif // !defined(__midl) && !defined(MIDL_PASS)

#ifdef __cplusplus
} // extern "C"
#endif

#pragma pack(pop)

#pragma warning(pop) // _VCRUNTIME_DISABLED_WARNINGS

#endif // _CFGUARD_H
