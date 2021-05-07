/***
*risctrnsctrl.cpp -
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*   Common control transfer helpers required for RISC and AMD64 architecture
*   EH.
****/

#include <vcruntime_internal.h>
#include <eh.h>
#include <ehassert.h>
#include <ehdata.h>
#include <ehdata4.h>
#include <ehhooks.h>
#include <trnsctrl.h>
#include "ehhelpers.h"

#if !defined(RENAME_EH_EXTERN_HYBRID)
#define RENAME_EH_EXTERN_HYBRID(x) x
#endif

#if _EH_RELATIVE_FUNCINFO

#define _ImageBase        (RENAME_BASE_PTD(__vcrt_getptd)()->_ImageBase)

extern "C" uintptr_t __cdecl _GetImageBase()
{
    return _ImageBase;
}

extern "C" void __cdecl _SetImageBase(uintptr_t ImageBaseToRestore)
{
    _ImageBase = ImageBaseToRestore;
}

#endif

#if _EH_RELATIVE_TYPEINFO

#define _ThrowImageBase   (RENAME_BASE_PTD(__vcrt_getptd)()->_ThrowImageBase)

extern "C" uintptr_t __cdecl _GetThrowImageBase()
{
    return _ThrowImageBase;
}

extern "C" void __cdecl _SetThrowImageBase(uintptr_t NewThrowImageBase)
{
    _ThrowImageBase = NewThrowImageBase;
}

#endif

#if _EH_RELATIVE_FUNCINFO
#if _VCRT_BUILD_FH4
//
// Returns the establisher frame pointers. For catch handlers it is the parent's frame pointer.
//
EHRegistrationNode * RENAME_EH_EXTERN(__FrameHandler4)::GetEstablisherFrame
(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    EHRegistrationNode  *pEstablisher
    )
{

    *pEstablisher = *pRN;

    if (RENAME_EH_EXTERN(__FrameHandler4)::ExecutionInCatch(pDC, pFuncInfo))
    {

#if defined(_M_ARM64EC)

        if (RtlIsEcCode(pDC->ControlPc)) {
            *pEstablisher = *(EHRegistrationNode *)*pEstablisher;
        } else {
            *pEstablisher = *(EHRegistrationNode *)OffsetToAddress(pFuncInfo->dispFrame, *pRN);
        }

#elif defined(_M_X64)

        *pEstablisher = *(EHRegistrationNode *)OffsetToAddress(pFuncInfo->dispFrame, *pRN);

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

        *pEstablisher = *(EHRegistrationNode *)*pEstablisher;

#else // architecture

#error Unknown processor architecture.

#endif // architecture
    }

    return pEstablisher;
}
#endif // _VCRT_BUILD_FH4

//
// Returns the establisher frame pointers. For catch handlers it is the parent's frame pointer.
//
EHRegistrationNode * RENAME_EH_EXTERN(__FrameHandler3)::GetEstablisherFrame(
    EHRegistrationNode  *pRN,
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo,
    EHRegistrationNode  *pEstablisher
    )
{
    TryBlockMapEntry *pEntry;
    HandlerType *pHandler;
    ULONG_PTR HandlerAdd, ImageBase;
    unsigned num_of_try_blocks = FUNC_NTRYBLOCKS(*pFuncInfo);
    unsigned index, i;
    __ehstate_t curState;

    curState = StateFromControlPc(pFuncInfo, pDC);
    *pEstablisher = *pRN;
    for (index = num_of_try_blocks; index > 0; index--) {
        pEntry = FUNC_PTRYBLOCK(*pFuncInfo, index -1, pDC->ImageBase);
        if (curState > TBME_HIGH(*pEntry) && curState <= TBME_CATCHHIGH(*pEntry)) {
            // Get catch handler address.
            HandlerAdd = (*RENAME_EH_EXTERN(RtlLookupFunctionEntry)(pDC->ControlPc,
                                                                    &ImageBase,
                                                                    nullptr)).BeginAddress;
            pHandler = TBME_PLIST(*pEntry, ImageBase);
            for ( i = 0;
                  i < (unsigned)TBME_NCATCHES(*pEntry) &&
                  static_cast<ULONG_PTR>(pHandler[i].dispOfHandler) != HandlerAdd
                  ; i++);
            if ( i < (unsigned)TBME_NCATCHES(*pEntry)) {

#if defined(_M_ARM64EC)

                if (RtlIsEcCode(pDC->ControlPc)) {
                    *pEstablisher = *(EHRegistrationNode *)*pEstablisher;
                } else {
                    *pEstablisher = *(EHRegistrationNode *)OffsetToAddress(pHandler[i].dispFrame, *pRN);
                }

#elif defined(_M_X64)

                *pEstablisher = *(EHRegistrationNode *)OffsetToAddress(pHandler[i].dispFrame, *pRN);

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

                *pEstablisher = *(EHRegistrationNode *)*pEstablisher;

#else

#error Unknown processor architecture.

#endif

                break;
            }
        }
    }
    return pEstablisher;
}


// This function returns the try block for the given state if the state is in a
// catch; otherwise, nullptr is returned.

TryBlockMapEntry * RENAME_EH_EXTERN(__FrameHandler3)::CatchTryBlock(
    FuncInfo            *pFuncInfo,
    __ehstate_t         curState
) {
    TryBlockMapEntry *pEntry;
    unsigned num_of_try_blocks = FUNC_NTRYBLOCKS(*pFuncInfo);
    unsigned index;

    for (index = num_of_try_blocks; index > 0; index--) {
        pEntry = FUNC_PTRYBLOCK(*pFuncInfo, index -1, _ImageBase);
        if (curState > TBME_HIGH(*pEntry) && curState <= TBME_CATCHHIGH(*pEntry)) {
            return pEntry;
        }
    }

    return nullptr;
}

//
// This routine returns TRUE if we are executing from within a catch.  Otherwise, FALSE is returned.
//
#if _VCRT_BUILD_FH4
bool RENAME_EH_EXTERN(__FrameHandler4)::ExecutionInCatch(
    DispatcherContext*  /*pDC*/,
    FuncInfo            *pFuncInfo
    )
{
    return pFuncInfo->header.isCatch;
}
#endif // _VCRT_BUILD_FH4

bool RENAME_EH_EXTERN(__FrameHandler3)::ExecutionInCatch(
    DispatcherContext   *pDC,
    FuncInfo            *pFuncInfo
    )
{
    __ehstate_t curState = StateFromControlPc(pFuncInfo, pDC);
    return CatchTryBlock(pFuncInfo, curState)? TRUE : FALSE;
}

// The name of this function is rather misleading. This function won't really unwind
// to empty state. This function will unwind to lowest possible state for current block.
// Here is an example
//
// try {
//   // State = 1;
// } catch (...) {
//   // State when we enter catch is 2
//   // State inside catch is 3-5;
// }
// if __FrameUnwindToEmptyState is called for the main function, the target state will
// be -1 but if __FrameUnwindToEmptyState is called for catch block, the target state for
// __FrameUnwindToState will be 2 not -1. This way we are able to unwind the stack block
// by block not the whole function in single call.
#if _VCRT_BUILD_FH4
void RENAME_EH_EXTERN(__FrameHandler4)::FrameUnwindToEmptyState(
    EHRegistrationNode *pRN,
    DispatcherContext  *pDC,
    FuncInfo           *pFuncInfo
    )
{
    EHRegistrationNode EstablisherFramePointers, *pEstablisher;

    pEstablisher = GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);

    FrameUnwindToState(pEstablisher, pDC, pFuncInfo, EH_EMPTY_STATE);
}
#endif // _VCRT_BUILD_FH4

void RENAME_EH_EXTERN(__FrameHandler3)::FrameUnwindToEmptyState(
    EHRegistrationNode *pRN,
    DispatcherContext  *pDC,
    FuncInfo           *pFuncInfo
    )
{
    __ehstate_t         stateFromControlPC;
    TryBlockMapEntry    *pEntry;
    EHRegistrationNode  EstablisherFramePointers, *pEstablisher;

    pEstablisher = GetEstablisherFrame(pRN,
                                       pDC,
                                       pFuncInfo,
                                       &EstablisherFramePointers);

    stateFromControlPC = StateFromControlPc(pFuncInfo, pDC);
    pEntry = CatchTryBlock(pFuncInfo, stateFromControlPC);

    FrameUnwindToState(pEstablisher, pDC, pFuncInfo,
                       pEntry == nullptr ? EH_EMPTY_STATE : TBME_HIGH(*pEntry));
}

//
// __CxxFrameHandler3 - Real entry point to the runtime
//                                              __CxxFrameHandler2 is an alias for __CxxFrameHandler3
//                                              since they are compatible in VC version of CRT
//                      These functions should be separated out if a change makes
//                                              __CxxFrameHandler3 incompatible with __CxxFrameHandler2
//
extern "C" DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl RENAME_EH_EXTERN_HYBRID(__CxxFrameHandler3)(
    EHExceptionRecord  *pExcept,         // Information for this exception
    EHRegistrationNode RN,               // Dynamic information for this frame
    CONTEXT            *pContext,        // Context info
    DispatcherContext  *pDC              // More dynamic info for this frame
) {
    FuncInfo                *pFuncInfo;
    EXCEPTION_DISPOSITION   result;
    EHRegistrationNode      EstablisherFrame = RN;

    _ImageBase = pDC->ImageBase;
#ifdef _ThrowImageBase
    _ThrowImageBase = (uintptr_t)pExcept->params.pThrowImageBase;
#endif
    pFuncInfo = (FuncInfo*)(_ImageBase +*(PULONG)pDC->HandlerData);
    result = __InternalCxxFrameHandler<RENAME_EH_EXTERN(__FrameHandler3)>(pExcept, &EstablisherFrame, pContext, pDC, pFuncInfo, 0, nullptr, FALSE);
    return result;
}

#if _VCRT_BUILD_FH4
extern "C" DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl RENAME_EH_EXTERN_HYBRID(__CxxFrameHandler4)(
    EHExceptionRecord  *pExcept,         // Information for this exception
    EHRegistrationNode RN,               // Dynamic information for this frame
    CONTEXT            *pContext,        // Context info
    DispatcherContext  *pDC              // More dynamic info for this frame
    ) {
    FH4::FuncInfo4          FuncInfo;
    EXCEPTION_DISPOSITION   result;
    EHRegistrationNode      EstablisherFrame = RN;

    _ImageBase = pDC->ImageBase;
#ifdef _ThrowImageBase
    _ThrowImageBase = (uintptr_t)pExcept->params.pThrowImageBase;
#endif
    PBYTE buffer = (PBYTE)(_ImageBase + *(PULONG)pDC->HandlerData);

    FH4::DecompFuncInfo(buffer, FuncInfo, pDC->ImageBase, pDC->FunctionEntry->BeginAddress);

    result = __InternalCxxFrameHandler<RENAME_EH_EXTERN(__FrameHandler4)>(pExcept, &EstablisherFrame, pContext, pDC, &FuncInfo, 0, nullptr, FALSE);
    return result;
}
#endif // _VCRT_BUILD_FH4

#if !defined(_CHPE_X86_ARM64_EH_)

//
// __CxxFrameHandler2 - Remove after compiler is updated
//
extern "C" DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler2(
    EHExceptionRecord  *pExcept,         // Information for this exception
    EHRegistrationNode RN,               // Dynamic information for this frame
    CONTEXT            *pContext,        // Context info
    DispatcherContext  *pDC              // More dynamic info for this frame
)
{
    return __CxxFrameHandler3(pExcept, RN, pContext, pDC);
}

extern "C" DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler(
    EHExceptionRecord  *pExcept,         // Information for this exception
    EHRegistrationNode RN,               // Dynamic information for this frame
    CONTEXT            *pContext,        // Context info
    DispatcherContext  *pDC              // More dynamic info for this frame
)
{
    return __CxxFrameHandler3(pExcept, RN, pContext, pDC);
}

#endif

// Call the SEH to EH translator.
template <class T>
static int SehTransFilter(
    EXCEPTION_POINTERS    *ExPtrs,
    EHExceptionRecord     *pExcept,
    EHRegistrationNode    *pRN,
    CONTEXT               *pContext,
    DispatcherContext     *pDC,
    typename T::FuncInfo  *pFuncInfo,
    __ehstate_t           curState,
    BOOL                  *pResult
) {

        UNREFERENCED_PARAMETER(curState);
        _pForeignExcept = pExcept;
#ifdef _ThrowImageBase
        _ThrowImageBase = (uintptr_t)((EHExceptionRecord *)ExPtrs->ExceptionRecord)->params.pThrowImageBase;
#endif

#if _VCRT_BUILD_FH4
        if constexpr (std::is_same_v<T, RENAME_EH_EXTERN(__FrameHandler4)>)
        {
            // For FH4, the catch state from rethrow is transient and only readable one time before being reset. 
            // This path reprocesses a throw which means the transient state needs to be set again so the correct state is used.
            CatchStateInParent = curState;
        }
#endif

        __InternalCxxFrameHandler<T>((EHExceptionRecord *)ExPtrs->ExceptionRecord,
                                   pRN,
                                   pContext,
                                   pDC,
                                   pFuncInfo,
                                   0,
                                   nullptr,
                                   TRUE );
        _pForeignExcept = nullptr;
        *pResult = TRUE;
        return EXCEPTION_EXECUTE_HANDLER;
}

template <class T>
BOOL _CallSETranslator(
    EHExceptionRecord    *pExcept,    // The exception to be translated
    EHRegistrationNode   *pRN,        // Dynamic info of function with catch
    CONTEXT              *pContext,   // Context info
    DispatcherContext    *pDC,        // More dynamic info of function with catch (ignored)
    typename T::FuncInfo *pFuncInfo,  // Static info of function with catch
    ULONG                CatchDepth,  // How deeply nested in catch blocks are we?
    EHRegistrationNode   *pMarkerRN,  // Marker for parent context
    __ehstate_t          curState     // Current state
    )
{
    UNREFERENCED_PARAMETER(pMarkerRN);

    BOOL result = FALSE;
    pRN;
    pDC;
    pFuncInfo;
    CatchDepth;

    // Call the translator.

    _EXCEPTION_POINTERS excptr = { (PEXCEPTION_RECORD)pExcept, pContext };

    __try {
        _se_translator_function pSETranslator;
        pSETranslator = __pSETranslator;
        pSETranslator(PER_CODE(pExcept), &excptr);
        result = FALSE;
    } __except(SehTransFilter<T>(exception_info(),
                                 pExcept,
                                 pRN,
                                 pContext,
                                 pDC,
                                 pFuncInfo,
                                 curState,
                                 &result
                                )) {}

    // If we got back, then we were unable to translate it.

    return result;
}

#if _VCRT_BUILD_FH4
template
BOOL _CallSETranslator<RENAME_EH_EXTERN(__FrameHandler4)>(
    EHExceptionRecord                           *pExcept,    // The exception to be translated
    EHRegistrationNode                          *pRN,        // Dynamic info of function with catch
    CONTEXT                                     *pContext,   // Context info
    DispatcherContext                           *pDC,        // More dynamic info of function with catch (ignored)
    RENAME_EH_EXTERN(__FrameHandler4)::FuncInfo *pFuncInfo,  // Static info of function with catch
    ULONG                                       CatchDepth,  // How deeply nested in catch blocks are we?
    EHRegistrationNode                          *pMarkerRN,  // Marker for parent context
    __ehstate_t                                 curState     // Current state
    );
#endif // _VCRT_BUILD_FH4

template
BOOL _CallSETranslator<RENAME_EH_EXTERN(__FrameHandler3)>(
    EHExceptionRecord                           *pExcept,    // The exception to be translated
    EHRegistrationNode                          *pRN,        // Dynamic info of function with catch
    CONTEXT                                     *pContext,   // Context info
    DispatcherContext                           *pDC,        // More dynamic info of function with catch (ignored)
    RENAME_EH_EXTERN(__FrameHandler3)::FuncInfo *pFuncInfo,  // Static info of function with catch
    ULONG                                       CatchDepth,  // How deeply nested in catch blocks are we?
    EHRegistrationNode                          *pMarkerRN,  // Marker for parent context
    __ehstate_t                                 curState     // Current state
);

/////////////////////////////////////////////////////////////////////////////
//
// GetRangeOfTrysToCheck - determine which try blocks are of interest.
//
// The try blocks of interest are the ones in current catch block or function block.
// We will not try to call the catch that are outside the scope of this block. Consider
// the example.
//
// foo() {
//   try {  // Try block 1
//     try { // Try block 2
//       throw 1;  // Throw1
//     } catch (int) { // Catch block 1
//       try { // Try block 3
//         throw 1; // Throw2
//       } catch (int) { // Catch block 2
//         throw;   // Throw3
//       }
//     }
//   } catch (int) { // Catch block 3
//   }
// }
//
// When we have exception from Throw1, our block of concern is function foo. Here the
// try blocks of interest are Try block 2 and Try block 1.
//
// When we have exception for Throw2, we are in Catch block 1 and thus try block of
// concern is Try block 3. We don't really need to care about Try block 1 here as
// we have the main function block still on the stack and exception handler will also
// be called for that block.
//
// Returns:
//      Address of first try block of interest is returned
//      pStart and pEnd get the indices of the range in question
//
#if _VCRT_BUILD_FH4
RENAME_EH_EXTERN(__FrameHandler4)::TryBlockMap::IteratorPair RENAME_EH_EXTERN(__FrameHandler4)::GetRangeOfTrysToCheck(
    TryBlockMap    &tryBlockMap,
    __ehstate_t    curState,
    DispatcherContext * /*pDC*/,
    FuncInfo          * /*pFuncInfo*/,
    int            /*CatchDepth*/
)
{
    auto iterStart = tryBlockMap.begin();
    auto iterEnd = tryBlockMap.begin();
    tryBlockMap.setBuffer(iterStart);

    for (auto iter = tryBlockMap.begin(); iter != tryBlockMap.end(); ++iter)
    {
        auto tryBlock = *iter;
        if (curState >= tryBlock.tryLow && curState <= tryBlock.tryHigh) {
            if (iterStart != tryBlockMap.begin()) {
                iterStart = iter;
            }
            iterEnd = iter;
        }
    }
    // change to be (start, end]
    iterEnd.incrementToSentinel();
    // Reset so when we start reading it starts at the correct location
    tryBlockMap.setBuffer(iterStart);
    return TryBlockMap::IteratorPair(iterStart, iterEnd);
}
#endif // _VCRT_BUILD_FH4

RENAME_EH_EXTERN(__FrameHandler3)::TryBlockMap::IteratorPair RENAME_EH_EXTERN(__FrameHandler3)::GetRangeOfTrysToCheck(
    TryBlockMap       &tryBlockMap,
    __ehstate_t       curState,
    DispatcherContext *pDC,
    FuncInfo          *pFuncInfo,
    int               /*CatchDepth*/
    )
{
    TryBlockMapEntry *pEntry, *pCurCatchEntry = nullptr;
    unsigned num_of_try_blocks = FUNC_NTRYBLOCKS(*pFuncInfo);
    unsigned int index;
    __ehstate_t ipState = StateFromControlPc(pFuncInfo, pDC);

    _VCRT_VERIFY(num_of_try_blocks > 0);

    unsigned start = static_cast<unsigned>(-1);
    unsigned end = start;
    for (index = num_of_try_blocks; index > 0; index--) {
        pEntry = FUNC_PTRYBLOCK(*pFuncInfo, index -1, pDC->ImageBase);
        if (ipState > TBME_HIGH(*pEntry) && ipState <= TBME_CATCHHIGH(*pEntry)) {
            break;
        }
    }
    if (index) {
        pCurCatchEntry = FUNC_PTRYBLOCK(*pFuncInfo, index -1, pDC->ImageBase);
    }
    for(index = 0; index < num_of_try_blocks; index++ ) {
        pEntry = FUNC_PTRYBLOCK(*pFuncInfo, index, pDC->ImageBase);
        // if in catch block, check for try-catch only in current block
        if (pCurCatchEntry) {
            if (TBME_LOW(*pEntry) <= TBME_HIGH(*pCurCatchEntry) ||
                TBME_HIGH(*pEntry) > TBME_CATCHHIGH(*pCurCatchEntry))
                continue;
        }
        if (curState >= TBME_LOW(*pEntry) && curState <= TBME_HIGH(*pEntry)) {
           if (start == -1) {
               start = index;
           }
           end = index;
        }
    }

    // change to be (start, end]
    ++end;
    if (start == -1){
        start = 0;
        end = 0;
    }

    auto iterStart = TryBlockMap::iterator(tryBlockMap, start);
    auto iterEnd = TryBlockMap::iterator(tryBlockMap, end);

    return TryBlockMap::IteratorPair(iterStart, iterEnd);
}

extern "C" FRAMEINFO * __cdecl RENAME_EH_EXTERN(_CreateFrameInfo)(
    FRAMEINFO * pFrameInfo,
    PVOID       pExceptionObject
) {
    pFrameInfo->pExceptionObject = pExceptionObject;
    pFrameInfo->pNext            = (pFrameInfo < pFrameInfoChain)? pFrameInfoChain : nullptr;
    pFrameInfoChain              = pFrameInfo;
    return pFrameInfo;
}

/////////////////////////////////////////////////////////////////////////////
//
// _FindAndUnlinkFrame - Pop the frame information for this scope that was
//  pushed by _CreateFrameInfo.  This should be the first frame in the list,
//  but the code will look for a nested frame and pop all frames, just in
//  case.
//
extern "C" void __cdecl RENAME_EH_EXTERN(_FindAndUnlinkFrame)(
    FRAMEINFO * pFrameInfo
) {
    _VCRT_VERIFY(pFrameInfo == pFrameInfoChain);

    for (FRAMEINFO *pCurFrameInfo = pFrameInfoChain;
         pCurFrameInfo;
         pCurFrameInfo = pCurFrameInfo->pNext)
    {
        if (pFrameInfo == pCurFrameInfo) {
            pFrameInfoChain = pCurFrameInfo->pNext;
            return;
        }
    }

    // Should never be reached.
    abort();
}

#if _VCRT_BUILD_FH4
void RENAME_EH_EXTERN(__FrameHandler4)::UnwindNestedFrames(
    EHRegistrationNode  *pFrame,            // Unwind up to (but not including) this frame
    EHExceptionRecord   *pExcept,           // The exception that initiated this unwind
    CONTEXT             *pContext,          // Context info for current exception
    EHRegistrationNode  *pEstablisher,
    void                *Handler,
    FuncInfo*           /*pFuncInfo*/,
    __ehstate_t         TargetUnwindState,
    __ehstate_t         CatchState,        // State outside of current try but inside of any enclosing trys
    HandlerType         *pCatch,
    DispatcherContext   *pDC,
    BOOLEAN             recursive
    )
{
    static const EXCEPTION_RECORD ExceptionTemplate = // A generic exception record
    {
        STATUS_UNWIND_CONSOLIDATE,         // STATUS_UNWIND_CONSOLIDATE
        EXCEPTION_NONCONTINUABLE,          // Exception flags (we don't do resume)
        nullptr,                           // Additional record (none)
        nullptr,                           // Address of exception (OS fills in)
        15,                                // Number of parameters
        { EH_MAGIC_NUMBER1,                // Our version control magic number
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
        }                                  // pThrowInfo
    };

    EXCEPTION_RECORD ExceptionRecord = ExceptionTemplate;
    ExceptionRecord.ExceptionInformation[0] = (ULONG_PTR)CxxCallCatchBlock;
    // Address of call back function
    ExceptionRecord.ExceptionInformation[1] = (ULONG_PTR)pEstablisher;
    // Used by callback function
    ExceptionRecord.ExceptionInformation[2] = (ULONG_PTR)Handler;
    // Used by callback function to call catch block
    ExceptionRecord.ExceptionInformation[3] = (ULONG_PTR)TargetUnwindState;
    // Used by CxxFrameHandler to unwind to target_state
    ExceptionRecord.ExceptionInformation[4] = (ULONG_PTR)pContext;
    // used to set pCurrentExContext in callback function
    ExceptionRecord.ExceptionInformation[5] = pCatch->continuationAddress[0] + pDC->ImageBase;
    // Used in callback function for continuation address lookup
    ExceptionRecord.ExceptionInformation[6] = (ULONG_PTR)pExcept;
    // Used for passing current Exception
    ExceptionRecord.ExceptionInformation[7] = (ULONG_PTR)recursive;
    // Used for translated Exceptions
    ExceptionRecord.ExceptionInformation[8] = EH_MAGIC_NUMBER1;
    // Used in __InternalCxxFrameHandler to detected if it's being
    // called from _UnwindNestedFrames.

    // TODO: make these contiguous
    ExceptionRecord.ExceptionInformation[9] = pCatch->continuationAddress[1] + pDC->ImageBase;
    // Used in callback function for continuation address lookup

#if defined(_M_ARM64EC)

    if (RtlIsEcCode(pDC->ControlPc)) {
        ExceptionRecord.ExceptionInformation[10] = static_cast<ULONG_PTR>(-1);
    }

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

    ExceptionRecord.ExceptionInformation[10] = static_cast<ULONG_PTR>(-1);
    // ARM/ARM64-specific: used to hold a pointer to the non-volatile
    // registers

#elif !defined(_M_X64)

#error Unknown processor architecture.

#endif

    // Used to associate Catch Handler state to parent state
    ExceptionRecord.ExceptionInformation[11] = CatchState;

    RtlUnwindEx((void *)*pFrame,
        (void *)pDC->ControlPc,    // Address where control left function
        &ExceptionRecord,
        nullptr,
        pDC->ContextRecord,
        (PUNWIND_HISTORY_TABLE)pDC->HistoryTable);
}
#endif // _VCRT_BUILD_FH4

void RENAME_EH_EXTERN(__FrameHandler3)::UnwindNestedFrames(
    EHRegistrationNode  *pFrame,            // Unwind up to (but not including) this frame
    EHExceptionRecord   *pExcept,           // The exception that initiated this unwind
    CONTEXT             *pContext,          // Context info for current exception
    EHRegistrationNode  *pEstablisher,
    void                *Handler,
    FuncInfo            *pFuncInfo,
    __ehstate_t         TargetUnwindState,
    __ehstate_t         /*CatchState*/,
    HandlerType*        /*pCatch*/,
    DispatcherContext   *pDC,
    BOOLEAN             recursive
    )
{
    static const EXCEPTION_RECORD ExceptionTemplate = // A generic exception record
    {
        STATUS_UNWIND_CONSOLIDATE,         // STATUS_UNWIND_CONSOLIDATE
        EXCEPTION_NONCONTINUABLE,          // Exception flags (we don't do resume)
        nullptr,                           // Additional record (none)
        nullptr,                           // Address of exception (OS fills in)
        15,                                // Number of parameters
        {   EH_MAGIC_NUMBER1,              // Our version control magic number
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0
        }                                  // pThrowInfo
    };

    EXCEPTION_RECORD ExceptionRecord = ExceptionTemplate;
    ExceptionRecord.ExceptionInformation[0] = (ULONG_PTR)CxxCallCatchBlock;
                // Address of call back function
    ExceptionRecord.ExceptionInformation[1] = (ULONG_PTR)pEstablisher;
                // Used by callback function
    ExceptionRecord.ExceptionInformation[2] = (ULONG_PTR)Handler;
                // Used by callback function to call catch block
    ExceptionRecord.ExceptionInformation[3] = (ULONG_PTR)TargetUnwindState;
                // Used by CxxFrameHandler to unwind to target_state
    ExceptionRecord.ExceptionInformation[4] = (ULONG_PTR)pContext;
                // used to set pCurrentExContext in callback function
    ExceptionRecord.ExceptionInformation[5] = (ULONG_PTR)pFuncInfo;
                // Used in callback function to set state on stack to -2
    ExceptionRecord.ExceptionInformation[6] = (ULONG_PTR)pExcept;
                // Used for passing current Exception
    ExceptionRecord.ExceptionInformation[7] = (ULONG_PTR)recursive;
                // Used for translated Exceptions
    ExceptionRecord.ExceptionInformation[8] = EH_MAGIC_NUMBER1;
                // Used in __InternalCxxFrameHandler to detected if it's being
                // called from _UnwindNestedFrames.

#if defined(_M_ARM64EC)

    if (RtlIsEcCode(pDC->ControlPc)) {
        ExceptionRecord.ExceptionInformation[10] = static_cast<ULONG_PTR>(-1);
    }

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

    ExceptionRecord.ExceptionInformation[10] = static_cast<ULONG_PTR>(-1);
                // ARM/ARM64-specific: used to hold a pointer to the non-volatile
                // registers

#elif !defined(_M_X64)

#error Unknown processor architecture.

#endif

    RtlUnwindEx((void *)*pFrame,
                (void *)pDC->ControlPc,    // Address where control left function
                &ExceptionRecord,
                nullptr,
                pDC->ContextRecord,
                (PUNWIND_HISTORY_TABLE)pDC->HistoryTable);
}

#endif // _EH_RELATIVE_FUNCINFO
