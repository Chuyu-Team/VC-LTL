/***
*ehhelpers.cpp - Some target-independent helper routines used by the EH frame handler.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
****/

#include <eh.h>         // User-visible routines for eh
#include <ehassert.h>   // This project's versions of standard assert macros
#include <ehdata.h>     // Declarations of all types used for EH
#include <ehhooks.h>    // Declarations of hook variables and callbacks
#include <ehstate.h>    // Declarations of state management stuff
#include <trnsctrl.h>   // Routines to handle transfer of control (trnsctrl.asm)
#include <vcruntime_exception.h>
#include <vcruntime_typeinfo.h>

#include <Unknwn.h>
#include <Windows.h>

#include "ehhelpers.h"

// If we call DestructExceptionObject directly from C_Specific_Handler/
// _except_handler3, then this obj file will be pulled in by the linker
// even in programs which do not have C++ exceptions. So we call it using a
// function pointer _pDestructExceptionObject which gets initialized to 0 by
// default. If C++ exceptions are present, this file will be pulled in naturally
// and _pDestructExceptionObject will point to __DestructExceptionObject.

extern "C" void (__cdecl * const _pDestructExceptionObject)(EHExceptionRecord *,BOOLEAN)
    = &__DestructExceptionObject;

////////////////////////////////////////////////////////////////////////////////
//
// __AdjustPointer - Adjust the pointer to the exception object to a pointer to a
//   base instance.
//
// Output:
//     The address point of the base.
//
// Side-effects:
//     NONE.

#if _CRT_NTDDI_MIN < NTDDI_WINBLUE
extern "C" _VCRTIMP void *__AdjustPointer(
    void *pThis,                        // Address point of exception object
    const PMD& pmd                      // Generalized pointer-to-member
                                        //   descriptor
) {
    char *pRet = (char *)pThis + pmd.mdisp;

    if (pmd.pdisp >= 0) {
        pRet += *(__int32 *)((char *)*(ptrdiff_t *)((char *)pThis + pmd.pdisp)
#if defined(_WIN64)
           + (unsigned __int64)pmd.vdisp
#else
           + pmd.vdisp
#endif
           );
        pRet += pmd.pdisp;
    }

    return pRet;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//
// __GetPlatformExceptionInfo - Get Platform Exception extra information from current exception
//
// Output:
//     The address of WINRTEXCEPTIONINFO structure
//
// Side-effects:
//     NONE.
extern "C" _VCRTIMP void * __GetPlatformExceptionInfo(
    int *pIsBadAlloc
) {
    *pIsBadAlloc = 0;
    EHExceptionRecord *pExcept = _pCurrentException;
    if (pExcept != nullptr)
    {
        ThrowInfo* pTI = PER_PTHROW(pExcept);
        if (pTI != nullptr)
        {
            if (THROW_ISWINRT( (*pTI) ) )
            {
                ULONG_PTR *exceptionInfoPointer = *reinterpret_cast<ULONG_PTR**>(PER_PEXCEPTOBJ(pExcept));
                exceptionInfoPointer--;
                return reinterpret_cast<void*>(*exceptionInfoPointer);
            }
            else
            {
                _EXCEPTION_POINTERS exceptionPointers;
                exceptionPointers.ExceptionRecord = reinterpret_cast<PEXCEPTION_RECORD>(pExcept);
                exceptionPointers.ContextRecord = nullptr;

                *pIsBadAlloc = _is_exception_typeof(typeid(std::bad_alloc), &exceptionPointers);
            }
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
//
// __current_exception(), __current_exception_context(), __processing_throw()
//                        - Accessors for the per-thread exception state, used
//                          by the managed EH implementation, exception_ptr, etc.
//


extern "C" void** __cdecl __current_exception()
{
	auto ptd = __acrt_getptd();
#if _CRT_NTDDI_MIN < NTDDI_WIN6
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._curexception;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._curexception;
	}
}

extern "C" void** __cdecl __current_exception_context()
{
	auto ptd = __acrt_getptd();
#if _CRT_NTDDI_MIN < NTDDI_WIN6
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._curcontext;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._curcontext;
	}
}

extern "C" int* __cdecl __processing_throw()
{
	auto ptd = __acrt_getptd();
#if _CRT_NTDDI_MIN < NTDDI_WIN6
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._ProcessingThrow;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._ProcessingThrow;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// __FrameUnwindFilter - Allows possibility of continuing through SEH during
//   unwind.
//

extern "C" _VCRTIMP int __cdecl __FrameUnwindFilter(
    EXCEPTION_POINTERS *pExPtrs
) {
    EHTRACE_ENTER;

    EHExceptionRecord *pExcept = (EHExceptionRecord *)pExPtrs->ExceptionRecord;

    switch (PER_CODE(pExcept)) {
    case EH_EXCEPTION_NUMBER:
        __ProcessingThrow = 0;
        terminate();

#ifdef ALLOW_UNWIND_ABORT
    case EH_ABORT_FRAME_UNWIND_PART:
        EHTRACE_EXIT;
        return EXCEPTION_EXECUTE_HANDLER;
#endif

    case MANAGED_EXCEPTION_CODE:
    case MANAGED_EXCEPTION_CODE_V4:
        /*
         See VSW#544593 for more details. __ProcessingThrow is used to implement
         std::uncaught_exception(). The interaction between C++, SEH and managed
         exception wrt __ProcessingThrow is unspec'ed. From code inspection, it
         looks like that __ProcessingThrow works ok with all C++ exceptions.

         In this case, when we encounter a managed exception thrown from a destructor
         during unwind, we choose to decrement the count. This means that the previous
         C++ exception which incremented the count won't be considered any longer.
         In fact, the managed exception will be thrown, and the native C++ one will
         not have any possibility to be catched any longer.

         We should revisit std::uncaught_exception() and SEH/managed exception in the
         next version.
         */
        EHTRACE_EXIT;
        if (__ProcessingThrow > 0)
        {
            --__ProcessingThrow;
        }
        return EXCEPTION_CONTINUE_SEARCH;

    default:
        EHTRACE_EXIT;
        return EXCEPTION_CONTINUE_SEARCH;
    }
}


PGETWINRT_OOM_EXCEPTION __WinRTOutOfMemoryExceptionCallback = nullptr;

extern "C" _VCRTIMP void __cdecl _SetWinRTOutOfMemoryExceptionCallback(PGETWINRT_OOM_EXCEPTION pCallback)
{
	__WinRTOutOfMemoryExceptionCallback = pCallback;
}

//////////////////////////////////////////////////////////////////////////////////
// __std_terminate: Helper function referenced by the front-end to assist in
// implementing noexcept. Noexcept termination semantics are generally handled
// by FindHandler() above, when we cross a function with the noexcept bit set
// in the xdata when looking for a handler. When noexcept functions are inlined,
// though, we may no longer cross a noexcept function boundary when searching for
// a handler. In this case the inlined code contains an EH state that will invoke
// this function should an exception occur.
#if 0
extern "C" __declspec(noreturn) void __cdecl __std_terminate()
{
    terminate();
}
#endif

////////////////////////////////////////////////////////////////////////////////
//
// __DestructExceptionObject - Call the destructor (if any) of the original
//   exception object.
//
// Returns: None.
//
// Side-effects:
//     Original exception object is destructed.
//
// Notes:
//     If destruction throws any exception, and we are destructing the exception
//       object as a result of a new exception, we give up.  If the destruction
//       throws otherwise, we let it be.

#if 0
extern "C" _VCRTIMP void __cdecl __DestructExceptionObject(
    EHExceptionRecord *pExcept,         // The original exception record
    BOOLEAN fThrowNotAllowed            // TRUE if destructor not allowed to
                                        //   throw
) {
    // Ignore if not a C++ exception (since this is now called from
    // _except_handler3)
    if (pExcept == nullptr || !(PER_IS_MSVC_EH(pExcept)))
    {
        return;
    }

    EHTRACE_ENTER_FMT1("Destroying object @ 0x%p", PER_PEXCEPTOBJ(pExcept));

    /*UNDONE:Is this _SYSCRT specific */
#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST __WARNING_REDUNDANT_POINTER_TEST_FAR_EVIDENCE, "Current definition of PER_CODE ensures that pExcept cannot be nullptr")
    if (PER_PTHROW(pExcept) != nullptr)  {
        if (THROW_UNWINDFUNC(*PER_PTHROW(pExcept)) != 0) {

            __try {

#if _EH_RELATIVE_TYPEINFO
                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                    THROW_UNWINDFUNC_IB(*PER_PTHROW(pExcept),(ptrdiff_t)PER_PTHROWIB(pExcept)));
#else
                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                    THROW_UNWINDFUNC(*PER_PTHROW(pExcept)));
#endif

            } __except(EHTRACE_EXCEPT(fThrowNotAllowed
                ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)) {

                    // Can't have new exceptions when we're unwinding due to another
                    // exception.
                    terminate();
            }
        }
        else if (THROW_ISWINRT(*PER_PTHROW(pExcept))) {

            // Release if WinRT reference type exception
            IUnknown* pUnknown = *(reinterpret_cast<IUnknown**>(PER_PEXCEPTOBJ(pExcept)));
            if (pUnknown)
                pUnknown->Release();
        }
    }

    EHTRACE_EXIT;
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
// _IsExceptionObjectToBeDestroyed - Determine if an exception object is still
//  in use by a more deeply nested catch frame, or if it unused and should be
//  destroyed on exiting from the current catch block.
//
// Returns:
//      TRUE if exception object not found and should be destroyed.
//
extern "C" BOOL __cdecl _IsExceptionObjectToBeDestroyed(
    PVOID pExceptionObject
) {
    FRAMEINFO * pFrameInfo;

    for (pFrameInfo = pFrameInfoChain; pFrameInfo != nullptr; pFrameInfo = pFrameInfo->pNext ) {
        if( pFrameInfo->pExceptionObject == pExceptionObject ) {
            return FALSE;
        }
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// _is_exception_typeof - checks if the thrown exception is the type, the caller
// has passed in.
//
extern "C" _VCRTIMP int __cdecl _is_exception_typeof(const type_info & type, struct _EXCEPTION_POINTERS * ep)
{
    DASSERT(ep != nullptr);

    EHExceptionRecord *pExcept = (EHExceptionRecord *)ep->ExceptionRecord;

    // Is this our Exception?
    DASSERT(pExcept != nullptr);
    DASSERT(PER_IS_MSVC_EH(pExcept));

#if _EH_RELATIVE_TYPEINFO
    __int32 const *ppCatchable;
    ptrdiff_t imgBase = (ptrdiff_t)pExcept->params.pThrowImageBase;
    ppCatchable = THROW_CTLIST_IB(*PER_PTHROW(pExcept), imgBase );
    int catchables = THROW_COUNT_IB(*PER_PTHROW(pExcept), imgBase );
#else
    CatchableType * const *ppCatchable;
    ppCatchable = THROW_CTLIST(*PER_PTHROW(pExcept));
    int catchables = THROW_COUNT(*PER_PTHROW(pExcept));
#endif

    CatchableType *pCatchable;

    // Scan all types that thrown object can be converted to.
    for (; catchables > 0; catchables--, ppCatchable++)
    {
#if _EH_RELATIVE_TYPEINFO
        pCatchable = (CatchableType *)(imgBase + *ppCatchable);
        if(strcmp(type.raw_name(), CT_NAME_IB(*pCatchable, imgBase)) == 0)
#else
        pCatchable = *ppCatchable;
        if(strcmp(type.raw_name(), CT_NAME(*pCatchable)) == 0)
#endif
        {
            // Found a Match.
            return 1;
        }
    }

    return 0;
}

