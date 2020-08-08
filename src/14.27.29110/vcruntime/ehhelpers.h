//
// Copyright (c) Microsoft Corporation. All Rights reserved.
//

#pragma once

#include <ptd_downlevel.h>

#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

#if _CRT_NTDDI_MIN >= NTDDI_WIN6
#define _pForeignExcept   (*(EHExceptionRecord**)&(((_ptd_msvcrt_win6_shared*)__acrt_getptd())->_pForeignException))
#else
#define _pForeignExcept   (*(EHExceptionRecord**)(__LTL_GetOsMinVersion() < 0x00060000 ? &(__LTL_get_ptd_downlevel()->_pForeignException) : &(((_ptd_msvcrt_win6_shared*)__acrt_getptd())->_pForeignException)))
#endif

#endif

#if _CRT_NTDDI_MIN >= NTDDI_WIN6
#define pFrameInfoChain   (*((FRAMEINFO **)    &(((_ptd_msvcrt_win6_shared*)__acrt_getptd())->_pFrameInfoChain)))
#else

static __inline void* __fastcall pFrameInfoChain_fun()
{
    auto ptd = __acrt_getptd();
    const auto OsVersion = __LTL_GetOsMinVersion();

#if defined(_M_IX86)
    if (OsVersion < 0x00050001)
    {
        return &(__LTL_get_ptd_downlevel()->_pFrameInfoChain);
    }
#endif
    if (OsVersion < 0x00060000)
    {
        return &(((_ptd_msvcrt_winxp*)ptd)->_pFrameInfoChain);
    }

    return &(((_ptd_msvcrt_win6_shared*)ptd)->_pFrameInfoChain);
}

#define pFrameInfoChain   (*((FRAMEINFO **)  pFrameInfoChain_fun()))
#endif

// Pre-V4 managed exception code
#define MANAGED_EXCEPTION_CODE  0XE0434F4D

// V4 and later managed exception code
#define MANAGED_EXCEPTION_CODE_V4  0XE0434352

extern "C" void
__except_validate_context_record(
    _In_ PCONTEXT ContextRecord
    );

extern "C" _VCRTIMP void * __AdjustPointer(
    void *,
    const PMD&
);

extern "C" _VCRTIMP void * __GetPlatformExceptionInfo(
    int *
);

extern "C" _VCRTIMP int __cdecl __FrameUnwindFilter(
    EXCEPTION_POINTERS *
);

extern PGETWINRT_OOM_EXCEPTION __WinRTOutOfMemoryExceptionCallback;

extern "C" _VCRTIMP void __cdecl __DestructExceptionObject(
    EHExceptionRecord *,
    BOOLEAN
);

extern "C" _VCRTIMP void __cdecl RENAME_EH_EXTERN(__BuildCatchObject)(
    EHExceptionRecord *,
    void *,
    HandlerType *,
    CatchableType *
);

extern "C" _VCRTIMP int __cdecl RENAME_EH_EXTERN(__TypeMatch4)(
    FH4::HandlerType4 *,
    CatchableType *,
    ThrowInfo *
    );

extern "C" _VCRTIMP int __cdecl RENAME_EH_EXTERN(__TypeMatch)(
    HandlerType *,
    CatchableType *,
    ThrowInfo *
);

//
// Prototype for the internal handler
//

template<class T>
EXCEPTION_DISPOSITION __InternalCxxFrameHandler(
    EHExceptionRecord    *pExcept,
    EHRegistrationNode   *pRN,
    CONTEXT              *pContext,
    DispatcherContext    *pDC,
    typename T::FuncInfo *pFuncInfo,
    int                  CatchDepth,
    EHRegistrationNode   *pMarkerRN,
    BOOLEAN              recursive);

#if _EH_RELATIVE_TYPEINFO

#undef THROW_COUNT
#define THROW_COUNT(ti)         THROW_COUNT_IB(ti, _GetThrowImageBase())
#undef THROW_CTLIST
#define THROW_CTLIST(ti)        THROW_CTLIST_IB(ti, _GetThrowImageBase())

#endif // _EH_RELATIVE_TYPEINFO
