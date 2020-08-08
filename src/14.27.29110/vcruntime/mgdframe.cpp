/***
* mgdframe.cpp - Managed exception frame support.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       These routines are called by managed code during exception propagation.
*
****/

#include <eh.h>         // User-visible routines for eh
#include <ehassert.h>   // This project's versions of standard assert macros
#include <ehdata.h>     // Declarations of all types used for EH
#include <ehdata4.h>
#include <ehhooks.h>    // Declarations of hook variables and callbacks
#include <trnsctrl.h>   // Routines to handle transfer of control (trnsctrl.asm)
#include <vcruntime_exception.h>
#include <vcruntime_typeinfo.h>
#include "ehhelpers.h"


////////////////////////////////////////////////////////////////////////////////
// Model of C++ eh in COM+
//
// void func()
// {
//     try {
//         TryBody();
//     } catch (cpp_object o)
//     {
//         CatchOBody();
//     } catch (...)
//     {
//         CatchAllBody();
//     }
// }
//
// Turns into this:
//
//
// void func()
// {
//     int rethrow;
//     // One per try block
//     int isCxxException;
//     // One per catch(...)
//     __try {
//         TryBody();
//     }
//     __except(__CxxExceptionFilter(exception,
//                                   typeinfo(cpp_object),
//                                   flags,
//                                   &o))
//     // This is how it's done already
//     {
//     // Begin catch(object) prefix
//     char *storage = _alloca(__CxxQueryExceptionSize());
//     rethrow = false;
//     __CxxRegisterExceptionObject(exception,
//                                  storage);
//     __try {
//         __try {
//             // End catch(object) prefix
//             CatchOBody();
//             // Begin catch(object) suffix
//         } __except(rethrow = __CxxDetectRethrow(exception),
//                    EXCEPTION_CONTINUE_SEARCH)
//         {}
//     }
//     __finally
//     {
//         __CxxUnregisterExceptionObject(storage,
//                                        rethrow);
//     }
//     // End catch(object) suffix
//     }
//     __except(1)
//     {
//         // Begin catch(...) prefix
//         char *storage = _alloca(__CxxQueryExceptionSize());
//         rethrow = false;
//         isCxxException = __CxxRegisterExceptionObject(exception,
//                                                       storage);
//         __try
//         {
//             __try
//             {
//             // End catch(...) prefix
//             CatchAllBody();
//             // Begin catch(...) suffix
//         } __except(rethrow = __CxxDetectRethrow(exception),
//                    EXCEPTION_CONTINUE_SEARCH)
//         {}
//     } __finally
//     {
//         if (isCxxException)
//         __CxxUnregisterExceptionObject(storage, rethrow);
//     }
//     // End catch(...) suffix
//     }
// }
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// __CxxExceptionFilter() - Returns EXCEPTION_EXECUTE_HANDLER when the pType
//                          matches with the objects we can catch. Returns
//                          EXCEPTION_CONTINUE_SEARCH when pType is not one of
//                          the catchable type for the thrown object. This
//                          function is made for use with COM+ EH, where they
//                          attempt to do C++ EH as well.
//


extern "C" int __cdecl __CxxExceptionFilter(
    void *ppExcept,                     // Information for this (logical)
                                        // exception
    void *pType,                        // Info about the datatype.
    int adjectives,                     // Extra Info about the datatype.
    void *pBuildObj                     // Pointer to datatype.
    )
{
#if _EH_RELATIVE_TYPEINFO
    __int32 const *ppCatchable;
#elif defined(_WIN64)
    CatchableType * UNALIGNED const *ppCatchable;
#else
    CatchableType * const *ppCatchable;
#endif
    CatchableType *pCatchable;
    int catchables;
    EHExceptionRecord *pExcept;

    if (!ppExcept) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    pExcept = *(EHExceptionRecord **)ppExcept;
    // If catch all, always return EXCEPTION_EXECUTE_HANDLER
    // Note that if the adjective has HT_IsStdDotDot flag on, we comply Std
    // C++ eh behaviour, i.e. only catching C++ objects.
    if ( TD_IS_TYPE_ELLIPSIS((TypeDescriptor *)pType) &&
         (pExcept->ExceptionCode == MANAGED_EXCEPTION_CODE ||
          pExcept->ExceptionCode == MANAGED_EXCEPTION_CODE_V4 ||
         !(adjectives & HT_IsStdDotDot)))
    {
        if (PER_IS_MSVC_EH(pExcept))
        {
            if ( PER_PTHROW(pExcept) == nullptr)
            {
                if ( _pCurrentException == nullptr) {
                    return EXCEPTION_CONTINUE_SEARCH;
                }
            }
        }
        __ProcessingThrow++;
        return EXCEPTION_EXECUTE_HANDLER;
    }

    if (PER_IS_MSVC_EH(pExcept))
    {
        if ( PER_PTHROW(pExcept) == nullptr) {
            if (_pCurrentException == nullptr)
                return EXCEPTION_CONTINUE_SEARCH;
            pExcept =  _pCurrentException;
        }

        /*
         * Note that we will only be using pType or dispType
         * and adjective field of pCatch.
         */
        struct _s_HandlerType pCatch;
#if _EH_RELATIVE_TYPEINFO
        _SetThrowImageBase((ptrdiff_t)pExcept->params.pThrowImageBase);
#endif
#if _EH_RELATIVE_FUNCINFO
        ptrdiff_t ImageBaseBak = _GetImageBase();
        __try       //Try finally for restoring _ImageBase;
#endif
        {

#if _EH_RELATIVE_FUNCINFO
            void *CatchImageBase;
            // Set the image base using Rtl API.
            CatchImageBase = RtlPcToFileHeader((PVOID)pType, &CatchImageBase);
            _SetImageBase((ptrdiff_t)CatchImageBase);

            pCatch.dispType = (__int32)(
                    (ptrdiff_t)pType - (ptrdiff_t)CatchImageBase);
#else
            pCatch.pType = (TypeDescriptor *)pType;
#endif
            pCatch.adjectives = adjectives | HT_IsComplusEh;

            // Scan all types that thrown object can be converted to:
            ppCatchable = THROW_CTLIST(*PER_PTHROW(pExcept));
            for (catchables = THROW_COUNT(*PER_PTHROW(pExcept));
              catchables > 0; catchables--, ppCatchable++) {

#if _EH_RELATIVE_TYPEINFO
                pCatchable = (CatchableType *)(_GetThrowImageBase() + *ppCatchable);
#else
                pCatchable = *ppCatchable;
#endif

                if (__TypeMatch(&pCatch, pCatchable, PER_PTHROW(pExcept))) {
                    // Successful. Now build the object.
                    __ProcessingThrow++;
                    if (pBuildObj)
                        __BuildCatchObject(pExcept, pBuildObj, &pCatch, pCatchable);
                    return EXCEPTION_EXECUTE_HANDLER;
                }
            } // Scan possible conversions
        }
#if _EH_RELATIVE_FUNCINFO
        __finally {
            _SetImageBase(ImageBaseBak);
        }
#endif
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

////////////////////////////////////////////////////////////////////////////////
//
// __CxxRegisterExceptionObject() - Registers an Exception object for managed
//                                  C++ exception handling.
//
extern "C" int __cdecl __CxxRegisterExceptionObject(
    void *ppExcept,
    void *pStorage
)
{
    // This function is only called for C++ EH.
    EHExceptionRecord *pExcept = nullptr;
    FRAMEINFO *pFrameInfo = (FRAMEINFO *)pStorage;
    EHExceptionRecord **ppSaveException;
    CONTEXT **ppSaveExContext;
    ppSaveException = (EHExceptionRecord **)(&pFrameInfo[1]);
    ppSaveExContext = (CONTEXT **)(&ppSaveException[1]);
    if (ppExcept  && (*(void **)ppExcept)) {
        pExcept = *(EHExceptionRecord **)ppExcept;
        if (PER_IS_MSVC_EH(pExcept)) {
            if ( PER_PTHROW(pExcept) == nullptr) {
                // was a rethrow
                pExcept = _pCurrentException;
            }
        }
        pFrameInfo = _CreateFrameInfo(pFrameInfo, PER_PEXCEPTOBJ(pExcept));
        *ppSaveException = _pCurrentException;
        *ppSaveExContext = _pCurrentExContext;
        _pCurrentException = pExcept;
    } else {
        *ppSaveException = (EHExceptionRecord *)-1;
        *ppSaveExContext = (CONTEXT *)-1;
    }
    __ProcessingThrow--;
    if ( __ProcessingThrow < 0)
        __ProcessingThrow = 0;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// __CxxDetectRethrow() - Looks at the Exception and returns true if rethrow,
//                        false if not a rethrow. This is then used for
//                        destructing the exception object in
//                        __CxxUnregisterExceptionObject().
//
extern "C" int __cdecl __CxxDetectRethrow(
    void *ppExcept
)
{
    EHExceptionRecord *pExcept;
    if (!ppExcept)
        return EXCEPTION_CONTINUE_SEARCH;
    pExcept = *(EHExceptionRecord **)ppExcept;
    if (PER_IS_MSVC_EH(pExcept) && PER_PTHROW(pExcept) == nullptr) {
        __ProcessingThrow++;
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

////////////////////////////////////////////////////////////////////////////////
//
// __CxxUnregisterExceptionObject - Destructs Exception Objects if rethrow ==
//                          true. Also set __pCurrentException and
//                          __pCurrentExContext() to current value.
//
extern "C" void __cdecl __CxxUnregisterExceptionObject(
    void *pStorage,
    int rethrow
)
{
    FRAMEINFO *pFrameInfo = (FRAMEINFO *)pStorage;
    EHExceptionRecord **ppSaveException;
    CONTEXT **ppSaveExContext;
    ppSaveException = (EHExceptionRecord **)(&pFrameInfo[1]);
    ppSaveExContext = (CONTEXT **)(&ppSaveException[1]);
    if (*ppSaveException != (void *)-1) {
        _FindAndUnlinkFrame(pFrameInfo);
        if ( !rethrow && PER_IS_MSVC_EH(_pCurrentException) &&
                _IsExceptionObjectToBeDestroyed(PER_PEXCEPTOBJ(_pCurrentException))
                ) {
            __DestructExceptionObject(_pCurrentException, TRUE);
        }
        if (PER_IS_MSVC_EH(_pCurrentException) && rethrow)
            __ProcessingThrow--;
        _pCurrentException = *ppSaveException;
        _pCurrentExContext = *ppSaveExContext;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// __CxxQueryExceptionSize - returns the value of Storage needed to save
//                          FrameInfo + two pointers.
//
extern "C" int __cdecl __CxxQueryExceptionSize(
    void
)
{
    return sizeof(FRAMEINFO) + sizeof(void *) + sizeof(void *);
}
