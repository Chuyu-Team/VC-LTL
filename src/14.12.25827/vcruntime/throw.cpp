/***
*throw.cpp - Implementation of the 'throw' command.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Implementation of the exception handling 'throw' command.
*
*       Entry points:
*       * _CxxThrowException - does the throw.
****/

#include <ehdata.h>
#include <ehassert.h>
#include <trnsctrl.h>

#include <Windows.h>

//
// Make sure PULONG_PTR is available
//

#if !defined(PULONG_PTR)
#if defined(_WIN64)
typedef unsigned __int64 *      PULONG_PTR;
#else
typedef unsigned long *         PULONG_PTR;
#endif
#endif

#if defined(_M_X64)
extern "C" PVOID _ReturnAddress(VOID);
#pragma intrinsic(_ReturnAddress)
#endif

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
//      synthesisze the -export directive below.
//
// Returns:
//      NEVER.  (until we implement resumable exceptions, that is)
//
extern "C" __declspec(noreturn) void __stdcall
_CxxThrowException(
        void*           pExceptionObject,   // The object thrown
        _ThrowInfo*     pThrowInfo          // Everything we need to know about it
) {
        EHTRACE_ENTER_FMT1("Throwing object @ 0x%p", pExceptionObject);

        static const EHExceptionRecord ExceptionTemplate = { // A generic exception record
            EH_EXCEPTION_NUMBER,            // Exception number
            EXCEPTION_NONCONTINUABLE,       // Exception flags (we don't do resume)
            nullptr,                           // Additional record (none)
            nullptr,                           // Address of exception (OS fills in)
            EH_EXCEPTION_PARAMETERS,        // Number of parameters
            {   EH_MAGIC_NUMBER1,           // Our version control magic number
                nullptr,                       // pExceptionObject
                nullptr,
#if _EH_RELATIVE_TYPEINFO
                nullptr                        // Image base of thrown object
#endif
            }                      // pThrowInfo
        };
        EHExceptionRecord ThisException = ExceptionTemplate;    // This exception

        ThrowInfo* pTI = (ThrowInfo*)pThrowInfo;
        if (pTI && (THROW_ISWINRT( (*pTI) ) ) )
        {
            ULONG_PTR *exceptionInfoPointer = *reinterpret_cast<ULONG_PTR**>(pExceptionObject);
            exceptionInfoPointer--; // The pointer to the ExceptionInfo structure is stored sizeof(void*) infront of each WinRT Exception Info.

            WINRTEXCEPTIONINFO** ppWei = reinterpret_cast<WINRTEXCEPTIONINFO**>(exceptionInfoPointer);
			pTI = (*ppWei)->throwInfo;

			(*ppWei)->PrepareThrow( ppWei );
        }

        //
        // Fill in the blanks:
        //
        ThisException.params.pExceptionObject = pExceptionObject;
        ThisException.params.pThrowInfo = pTI;
#if _EH_RELATIVE_TYPEINFO
        PVOID ThrowImageBase = RtlPcToFileHeader((PVOID)pTI, &ThrowImageBase);
        ThisException.params.pThrowImageBase = ThrowImageBase;
#endif

        //
        // If the throw info indicates this throw is from a pure region,
        // set the magic number to the Pure one, so only a pure-region
        // catch will see it.
        //
        // Also use the Pure magic number on Win64 if we were unable to
        // determine an image base, since that was the old way to determine
        // a pure throw, before the TI_IsPure bit was added to the FuncInfo
        // attributes field.
        //
        if (pTI != nullptr)
        {
            if (THROW_ISPURE(*pTI))
            {
                ThisException.params.magicNumber = EH_PURE_MAGIC_NUMBER1;
            }
#if _EH_RELATIVE_TYPEINFO
            else if (ThrowImageBase == nullptr)
            {
                ThisException.params.magicNumber = EH_PURE_MAGIC_NUMBER1;
            }
#endif
        }

        //
        // Hand it off to the OS:
        //

        EHTRACE_EXIT;
#if defined(_M_X64) && defined(_NTSUBSET_)
        RtlRaiseException( (PEXCEPTION_RECORD) &ThisException );
#else
        RaiseException( ThisException.ExceptionCode,
                        ThisException.ExceptionFlags,
                        ThisException.NumberParameters,
                        (PULONG_PTR)&ThisException.params );
#endif
}
