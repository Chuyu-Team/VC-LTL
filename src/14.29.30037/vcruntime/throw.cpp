//
// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include <ehassert.h>
#include <ehdata.h>
#include <ehdata4.h>
#include <trnsctrl.h>

#include <Windows.h>

/////////////////////////////////////////////////////////////////////////////
//
// _CxxThrowException - implementation of 'throw'
//
// Description:
//      Builds the NT Exception record, and calls the NT runtime to initiate
//      exception processing.
//
//      Why is pThrowInfo defined as _ThrowInfo?  Because _ThrowInfo is secretly
//      snuck into the compiler, as is the prototype for _CxxThrowException, so
//      we have to use the same type to keep the compiler happy.
//
//      Another result of this is that _CRTIMP can't be used here.  Instead, we
//      synthesize the -export directive below.
//
extern "C" __declspec(noreturn) void __stdcall _CxxThrowException(
    void *pExceptionObject, // The object thrown
    _ThrowInfo *pThrowInfo  // Everything we need to know about it
) {
    EHTRACE_FMT1("Throwing object @ 0x%p", pExceptionObject);
    auto pTI = reinterpret_cast<ThrowInfo *>(pThrowInfo);
    ULONG_PTR magicNumber = EH_MAGIC_NUMBER1;
    if (pTI && (pTI->attributes & TI_IsWinRT)) {
        // The pointer to the ExceptionInfo structure is stored sizeof(void*) in front of each WinRT Exception Info.
        WINRTEXCEPTIONINFO **ppWei = *static_cast<WINRTEXCEPTIONINFO ***>(pExceptionObject);
        --ppWei;
        const auto pWei = *ppWei;
        pTI = pWei->throwInfo;
        pWei->PrepareThrow(ppWei);
    }

#if _EH_RELATIVE_TYPEINFO
    void *throwImageBase = RtlPcToFileHeader(const_cast<void *>(static_cast<const void *>(pTI)), &throwImageBase);
#endif // _EH_RELATIVE_TYPEINFO

    // If the throw info indicates this throw is from a pure region,
    // set the magic number to the Pure one, so only a pure-region
    // catch will see it.
    //
    // Also use the Pure magic number on Win64 if we were unable to
    // determine an image base, since that was the old way to determine
    // a pure throw, before the TI_IsPure bit was added to the FuncInfo
    // attributes field.
    if (pTI && ((pTI->attributes & TI_IsPure)
#if _EH_RELATIVE_TYPEINFO
                || !throwImageBase
#endif // _EH_RELATIVE_TYPEINFO
                )) {
        magicNumber = EH_PURE_MAGIC_NUMBER1;
    }

    // Build the parameters for the EHExceptionRecord:
    const ULONG_PTR parameters[] = {
        magicNumber,
        reinterpret_cast<ULONG_PTR>(pExceptionObject),
        reinterpret_cast<ULONG_PTR>(pTI),
#if _EH_RELATIVE_TYPEINFO
        reinterpret_cast<ULONG_PTR>(throwImageBase),
#endif // _EH_RELATIVE_TYPEINFO
    };

    // Hand it off to the OS:
    RaiseException(EH_EXCEPTION_NUMBER, EXCEPTION_NONCONTINUABLE, _countof(parameters), parameters);
}
