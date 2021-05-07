//
// trnsctrl.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Internal VCRuntime header that defines the functions that perform special
// transfer of control operations and related things.
//
#pragma once

#include <vcruntime_internal.h>
#include <ehdata.h>

#if !defined(RENAME_EH_EXTERN)
#define RENAME_EH_EXTERN(x) x
#endif

#pragma pack(push, _CRT_PACKING)

// Link together all existing catch objects to determine when they should
// be destroyed
typedef struct FrameInfo
{
    PVOID              pExceptionObject;

#if defined _M_ARM

    EHRegistrationNode* pRN;
    struct _s_FuncInfo* pFuncInfo;
    __ehstate_t         state;

#endif

    struct FrameInfo*  pNext;
} FRAMEINFO;

#if defined(_M_ARM64EC)
    extern "C" PVOID RENAME_EH_EXTERN(_CallSettingFrameArm64Ec)(
        void*,
        EHRegistrationNode*,
        PULONG,
        ULONG
        );
#endif

#if defined _M_X64 || defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
    extern "C" PVOID RENAME_EH_EXTERN(_CallSettingFrame)(
        void*,
        EHRegistrationNode*,
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
        PULONG,
#endif
        ULONG
        );

    extern "C" PVOID RENAME_EH_EXTERN(_CallSettingFrame_LookupContinuationIndex)(
        void*,
        EHRegistrationNode*,
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
        PULONG,
#endif
        ULONG
        );

    extern "C" PVOID RENAME_EH_EXTERN(_CallSettingFrame_NotifyContinuationAddr)(
        void*,
        EHRegistrationNode*
        );

    extern "C" PVOID RENAME_EH_EXTERN(_CallSettingFrameEncoded)(
        void*,
        EHRegistrationNode,
        void*,
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
        PULONG,
#endif
        ULONG
        );

    #define OffsetToAddress(offset, fp)  (void*)(((char*)(fp)) + (offset))

    #define UNWINDSTATE(base, offset)    *((int*)((char*)base + offset))
    #define UNWINDTRYBLOCK(base, offset) *((int*)((char*)(OffsetToAddress(offset,base)) + 4))
    #define UNWINDHELP(base, offset)     *((__int64*)((char*)base + offset))

    extern "C" uintptr_t __cdecl _GetImageBase();

    extern "C" void __cdecl _SetImageBase(
        uintptr_t ImageBaseToRestore
        );

#if !defined(_CHPE_X86_ARM64_EH_)

    extern "C" uintptr_t __cdecl _GetThrowImageBase();

    extern "C" void __cdecl _SetThrowImageBase(
        uintptr_t NewThrowImageBase
        );

#endif

    template <class T>
    BOOL _CallSETranslator(
        EHExceptionRecord    *pExcept,
        EHRegistrationNode   *pRN,
        CONTEXT              *pContext,
        DispatcherContext    *pDC,
        typename T::FuncInfo *pFuncInfo,
        ULONG                CatchDepth,
        EHRegistrationNode   *pMarkerRN,
        __ehstate_t          curState
    );

#elif defined _M_IX86

    extern "C" void* __stdcall _CallSettingFrame(
        void*,
        EHRegistrationNode*,
        unsigned long
        );

    void  __stdcall _JumpToContinuation(
        void*,
        EHRegistrationNode*
        );

    // Translate an ebp-relative offset to a hard address based on address of
    // registration node:
    #define OffsetToAddress(off, RN) (void*)((char*)(RN) + FRAME_OFFSET + (off))

    // Call RtlUnwind in a returning fashion
    void __stdcall _UnwindNestedFrames(
        EHRegistrationNode*,
        EHExceptionRecord*
        );

    void* _CallCatchBlock2(
        EHRegistrationNode*,
        FuncInfo*,
        void*,
        int,
        unsigned long
        );

    BOOL _CallSETranslator(
        EHExceptionRecord*,
        EHRegistrationNode*,
        void*,
        DispatcherContext*,
        FuncInfo*,
        int,
        EHRegistrationNode*
        );

#else

    #error Special transfer of control routines not defined for this platform

#endif

#if !defined(_M_CEE_PURE)

__declspec(guard(ignore)) inline void __stdcall _CallMemberFunction0(
    void* const pthis,
    void* const pmfn
    ) noexcept(false)
{
    auto const OneArgFn = reinterpret_cast<void (__thiscall*)(void*)>(pmfn);
    OneArgFn(pthis);
}

__declspec(guard(ignore)) inline void __stdcall _CallMemberFunction1(
    void* const pthis,
    void* const pmfn,
    void* const pthat
    ) noexcept(false)
{
    auto const TwoArgFn = reinterpret_cast<void (__thiscall*)(void*, void*)>(pmfn);
    TwoArgFn(pthis, pthat);
}

__declspec(guard(ignore)) inline void __stdcall _CallMemberFunction2(
    void* const pthis,
    void* const pmfn,
    void* const pthat,
    int   const val2
    ) noexcept(false)
{
    auto const ThreeArgFn = reinterpret_cast<void (__thiscall*)(void*, void*, int)>(pmfn);
    ThreeArgFn(pthis, pthat, val2);
}

#endif

// The following functions are implemented in the common transfer-of-control
// implementation shared by the x64, arm, and arm64 EH implementations.

extern "C" _VCRTIMP FRAMEINFO* __cdecl RENAME_EH_EXTERN(_CreateFrameInfo)(
    FRAMEINFO* fi,
    PVOID exception
    );

extern "C" _VCRTIMP BOOL __cdecl _IsExceptionObjectToBeDestroyed(
    PVOID exception
    );

extern "C" _VCRTIMP void __cdecl RENAME_EH_EXTERN(_FindAndUnlinkFrame)(
    FRAMEINFO* fi
    );

typedef void (__stdcall* PFNPREPARE_FOR_THROW)(void* ExceptionInfo);

typedef struct WinRTExceptionInfo
{
    void*                description;
    void*                restrictedErrorString;
    void*                restrictedErrorReference;
    void*                capabilitySid;
    long                 hr;
    void*                restrictedInfo;
    ThrowInfo*           throwInfo;
    unsigned int         size;
    PFNPREPARE_FOR_THROW PrepareThrow;
} WINRTEXCEPTIONINFO;

extern "C" _VCRTIMP void** __cdecl __current_exception();
extern "C" _VCRTIMP void** __cdecl __current_exception_context();
extern "C" _VCRTIMP int*   __cdecl __processing_throw();

// NOTE: __uncaught_exceptions duplicates __ProcessingThrow.

#ifndef _VCRT_DIRECT_PTD
#ifdef _VCRT_BUILD
#define _VCRT_DIRECT_PTD 1
#else // ^^^ _VCRT_BUILD // !_VCRT_BUILD vvv
#define _VCRT_DIRECT_PTD 0
#endif // _VCRT_BUILD
#endif // _VCRT_DIRECT_PTD

#if _VCRT_DIRECT_PTD

    #define _pCurrentException (*reinterpret_cast<EHExceptionRecord**>(&RENAME_BASE_PTD(__vcrt_getptd)()->_curexception))
    #define _pCurrentExContext (*reinterpret_cast<CONTEXT**>(&RENAME_BASE_PTD(__vcrt_getptd)()->_curcontext))
    #define __ProcessingThrow  (RENAME_BASE_PTD(__vcrt_getptd)()->_ProcessingThrow)

#else // ^^^ _VCRT_DIRECT_PTD ^^^ // vvv !_VCRT_DIRECT_PTD vvv //

    #define _pCurrentException (*reinterpret_cast<EHExceptionRecord**>(__current_exception()))
    #define _pCurrentExContext (*reinterpret_cast<CONTEXT**>(__current_exception_context()))
    #define __ProcessingThrow  (*__processing_throw())

#endif // !_VCRT_DIRECT_PTD

#ifndef _VCRT_BUILD_FH4
#ifdef _M_X64
#define _VCRT_BUILD_FH4 1
#else // ^^^ _M_X64 // !_M_X64 vvv
#define _VCRT_BUILD_FH4 0
#endif // _M_X64
#endif // _VCRT_BUILD_FH4

#if _VCRT_BUILD_FH4
// In the satellite build, __vcrt_getptd uses satellite's PTD
// In the non-DLL build, __vcrt_getptd uses the main PTD which was updated to have this field
#define CatchStateInParent      (__vcrt_getptd()->_CatchStateInParent)
#endif

#pragma pack(pop)
