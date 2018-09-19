/***
*frame.cxx - The frame handler and everything associated with it.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       The frame handler and everything associated with it.
*
*       Entry points:
*       _CxxFrameHandler   - the frame handler.
*
*       Open issues:
*         Handling re-throw from dynamicly nested scope.
*         Fault-tolerance (checking for data structure validity).
****/

#include <eh.h>         // User-visible routines for eh
#include <ehassert.h>   // This project's versions of standard assert macros
#include <ehdata.h>     // Declarations of all types used for EH
#include <ehhooks.h>    // Declarations of hook variables and callbacks
#include <ehstate.h>    // Declarations of state management stuff
#include <trnsctrl.h>   // Routines to handle transfer of control (trnsctrl.asm)
#include <vcruntime_exception.h>
#include <vcruntime_typeinfo.h>

#include <unknwn.h>
#include <windows.h>

// The CallUnexpected function is only called in some build configurations.
#pragma warning(disable: 4505) // unreferenced local function has been removed
#pragma warning(disable: 4702) // unreachable code

#define cxxReThrow   (__vcrt_getptd()->_cxxReThrow)

extern "C" void
__except_validate_context_record(
    _In_ PCONTEXT ContextRecord
    );

// Pre-V4 managed exception code
#define MANAGED_EXCEPTION_CODE  0XE0434F4D

// V4 and later managed exception code
#define MANAGED_EXCEPTION_CODE_V4  0XE0434352

////////////////////////////////////////////////////////////////////////////////
//
// Intel x86-specific definitions
//
#if defined(_M_IX86)
#define __GetRangeOfTrysToCheck(a, b, c, d, e, f, g) \
                                _GetRangeOfTrysToCheck(b, c, d, e, f)
#define __FrameUnwindToEmptyState(a, b, c) \
                                __FrameUnwindToState(a, b, c, EH_EMPTY_STATE);
#define __CallSETranslator(a, b, c, d, e, f, g, h) \
                                _CallSETranslator(a, b, c, d, e, f, g)
#define __GetUnwindState(a, b, c) \
                                GetCurrentState(a, b, c)
#define __OffsetToAddress(a, b, c) \
                                OffsetToAddress(a, b)
#define __GetAddress(a, b) \
                                (a)
#define REAL_FP(a, b) \
                                (a)
#define __ResetException(a)

////////////////////////////////////////////////////////////////////////////////
//
// ARM-specific definitions
//
#elif (defined(_M_ARM) && !defined(_M_ARM_NT)) 
#define __GetRangeOfTrysToCheck(a, b, c, d, e, f, g) \
                                _GetRangeOfTrysToCheck(a, b, c, d, e, f, g)
#define __CallSETranslator(a, b, c, d, e, f, g, h) \
                                _CallSETranslator(a, b, c, d, e, f, g)
#define __GetUnwindState(a, b, c) \
                                GetCurrentState(a, b, c)
#define __OffsetToAddress(a, b, c) \
                                OffsetToAddress(a, b)
#define __GetAddress(a, b) \
                                (void*)(a)
#define REAL_FP(a, b) \
                                (a)
#define __ResetException(a)
#ifdef _MT
#define pExitContext            (*((CONTEXT **)&(__vcrt_getptd()->_pExitContext)))
#else
static CONTEXT                  *pExitContext = nullptr;   // context to assist the return to the continuation point
EHExceptionRecord               *_pForeignExcept = nullptr;
#endif  // _MT

// The throw site
#undef abnormal_termination
#define abnormal_termination()  FALSE

#elif defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
#define __GetRangeOfTrysToCheck(a, b, c, d, e, f, g) \
                                _GetRangeOfTrysToCheck(a, b, c, d, e, f, g)
#define __CallSETranslator(a, b, c, d, e, f, g, h) \
                                _CallSETranslator(a, b, c, d, e, f, g)

#ifdef _M_ARM64
#define __GetUnwindState(a, b, c) \
                                __GetCurrentState(a, b, c)
#else // _M_ARM64
#define __GetUnwindState(a, b, c) \
                                GetCurrentState(a, b, c)
#endif // _M_ARM64
#define __OffsetToAddress(a, b, c) \
                                OffsetToAddress(a, b)
#define __GetAddress(a, b) \
                                (void*)(a)
#define REAL_FP(a, b) \
                                (a)
#define __ResetException(a)
#define pExitContext            (*((CONTEXT **)&(__vcrt_getptd()->_pExitContext)))
#define _pForeignExcept     (*((EHExceptionRecord **)&(__vcrt_getptd()->_pForeignException)))

#define EST_ARRAY(x,n)          EST_ARRAY_IB(x, _GetImageBase(), n)
#define FUNC_ESTYPES(fi)        ((fi).dispESTypeList ? FUNC_ESTYPES_IB(fi, _GetImageBase()) : nullptr )
#define FUNC_PESTYPES(fi)       ((*(fi)).dispESTypeList ? FUNC_PESTYPES_IB((fi), _GetImageBase()) : nullptr )

// The throw site
#undef CT_PTD
#define CT_PTD(ct)              (CT_PTD_IB(ct, _GetThrowImageBase()))
#undef CT_COPYFUNC
#define CT_COPYFUNC(ct)         ((ct).copyFunction? CT_COPYFUNC_IB(ct, _GetThrowImageBase()):nullptr)

#define CT_NAME_IB(ct,ib)       (TD_NAME(*CT_PTD_IB(ct, ib)))

#undef THROW_FORWARDCOMPAT
#define THROW_FORWARDCOMPAT(ti) ((ti).pForwardCompat? THROW_FORWARDCOMPAT_IB(ti, _GetThrowImageBase()):nullptr)
#undef THROW_COUNT
#define THROW_COUNT(ti)         THROW_COUNT_IB(ti, _GetThrowImageBase())
#undef THROW_CTLIST
#define THROW_CTLIST(ti)        THROW_CTLIST_IB(ti, _GetThrowImageBase())

// The catch site
#undef HT_HANDLER
#define HT_HANDLER(ht)          (HT_HANDLER_IB(ht, _GetImageBase()))
#undef UWE_ACTION
#define UWE_ACTION(uwe)         ((uwe).action? UWE_ACTION_IB(uwe, _GetImageBase()):nullptr)

#undef FUNC_UNWIND
#define FUNC_UNWIND(fi,st)      (FUNC_PUNWINDMAP(fi,_GetImageBase())[st])
#undef TBME_CATCH
#define TBME_CATCH(hm,n)        (TBME_PLIST(hm,_GetImageBase())[n])
#undef TBME_PCATCH
#define TBME_PCATCH(hm,n)       (&(TBME_PLIST(hm,_GetImageBase())[n]))
#undef HT_PTD
#define HT_PTD(ht)              ((TypeDescriptor*)((ht).dispType? HT_PTD_IB(ht,_GetImageBase()):nullptr))

#undef abnormal_termination
#define abnormal_termination()  FALSE

#else
#error Unrecognized platform
#endif

extern "C" {
typedef struct {
    unsigned long dwSig;
    unsigned long uoffDestination;
    unsigned long dwCode;
    unsigned long uoffFramePointer;
} _NLG_INFO;

extern _NLG_INFO _NLG_Destination;
}

////////////////////////////////////////////////////////////////////////////////
//
// Forward declaration of local functions:
//

// The local unwinder must be external (see __CxxLongjmpUnwind in trnsctrl.cpp)

extern "C" void __FrameUnwindToState(
    EHRegistrationNode *,
    DispatcherContext *,
    FuncInfo *,
    __ehstate_t
);

static void FindHandler(
    EHExceptionRecord *,
    EHRegistrationNode *,
    CONTEXT *,
    DispatcherContext *,
    FuncInfo *,
    BOOLEAN,
    int,
    EHRegistrationNode*
);

static void CatchIt(
    EHExceptionRecord *,
    EHRegistrationNode *,
    CONTEXT *,
    DispatcherContext *,
    FuncInfo *,
    HandlerType *,
    CatchableType *,
    TryBlockMapEntry *,
    int,
    EHRegistrationNode *,
    BOOLEAN
#if defined (_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
    , BOOLEAN
#endif
);

static void * CallCatchBlock(
    EHExceptionRecord *,
    EHRegistrationNode *,
    CONTEXT *,
    FuncInfo *,
    void *,
    int,
    unsigned long
);

extern "C" _VCRTIMP void __cdecl __BuildCatchObject(
    EHExceptionRecord *,
    void *,
    HandlerType *,
    CatchableType *
);

extern "C" _VCRTIMP int __cdecl __TypeMatch(
    HandlerType *,
    CatchableType *,
    ThrowInfo *
);

extern "C" _VCRTIMP void * __AdjustPointer(
    void *,
    const PMD&
);

extern "C" _VCRTIMP void * __GetPlatformExceptionInfo(
    int *
);

static void FindHandlerForForeignException(
    EHExceptionRecord *,
    EHRegistrationNode *, CONTEXT *,
    DispatcherContext *,
    FuncInfo *,
    __ehstate_t,
    int,
    EHRegistrationNode *
);

extern "C" _VCRTIMP int __cdecl __FrameUnwindFilter(
    EXCEPTION_POINTERS *
);

static int ExFilterRethrow(
    EXCEPTION_POINTERS *
#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
    ,EHExceptionRecord *,
    int *
#endif
);

extern "C" _VCRTIMP void __cdecl __DestructExceptionObject(
    EHExceptionRecord *,
    BOOLEAN
);

// If we call DestructExceptionObject directly from C_Specific_Handler/
// _except_handler3, then frame.obj will be pulled in by the linker
// even in programs which do not have C++ exceptions. So we call it using a
// function pointer _pDestructExceptionObject which gets initialized to 0 by
// default, but, if frame.obj is pulled in naturally, it then points to
// __DestructExceptionObject.

extern "C" void (__cdecl * const _pDestructExceptionObject)
                    (EHExceptionRecord *,BOOLEAN) = &__DestructExceptionObject;

static BOOLEAN IsInExceptionSpec(
    EHExceptionRecord *pExcept,         // Information for this (logical)
                                        //   exception
    ESTypeList *pFuncInfo                // Static information for subject frame
);

static void CallUnexpected(ESTypeList* pESTypeList);

static BOOLEAN Is_bad_exception_allowed(ESTypeList *pExceptionSpec);

//
// This describes the most recently handled exception, in case of a rethrow:
//
#define _pCurrentFuncInfo       (*((ESTypeList **)&(__vcrt_getptd()->_curexcspec)))

////////////////////////////////////////////////////////////////////////////////
//
// __InternalCxxFrameHandler - the frame handler for all functions with C++ EH
// information.
//
// If exception is handled, this doesn't return; otherwise, it returns
// ExceptionContinueSearch.
//
// Note that this is called three ways:
//     From __CxxFrameHandler: primary usage, called to inspect whole function.
//         CatchDepth == 0, pMarkerRN == nullptr
//     From CatchGuardHandler: If an exception occurred within a catch, this is
//         called to check for try blocks within that catch only, and does not
//         handle unwinds.
//     From TranslatorGuardHandler: Called to handle the translation of a
//         non-C++ EH exception.  Context considered is that of parent.
#if 0
extern "C" EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler(
    EHExceptionRecord  *pExcept,        // Information for this exception
    EHRegistrationNode *pRN,            // Dynamic information for this frame
    CONTEXT *pContext,                  // Context info
    DispatcherContext *pDC,             // Context within subject frame
    FuncInfo *pFuncInfo,                // Static information for this frame
    int CatchDepth,                     // How deeply nested are we?
    EHRegistrationNode *pMarkerRN,      // Marker node for when checking inside
                                        //  catch block
    BOOLEAN recursive                   // Are we handling a translation?
) {
    EHTRACE_ENTER_FMT2("%s, pRN = 0x%p",
                       IS_UNWINDING(PER_FLAGS(pExcept)) ? "Unwinding" : "Searching",
                       pRN);

    __except_validate_context_record(pContext);

    if ((cxxReThrow == false) && (PER_CODE(pExcept) != EH_EXCEPTION_NUMBER) &&
#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        /* On the 64 bit/ARM platforms, ExceptionCode maybe set to STATUS_UNWIND_CONSOLIDATE
            when called from _UnwindNestedFrames during Logical Unwind. _UnwindNestedFrames
            will also set EH_MAGIC_NUMBER1 in the 8 element */
        (!((PER_CODE(pExcept) == STATUS_UNWIND_CONSOLIDATE) && (PER_NPARAMS(pExcept) == 15) && (PER_EXCEPTINFO(pExcept)[8] == EH_MAGIC_NUMBER1))) &&
#endif
        (PER_CODE(pExcept) != STATUS_LONGJUMP) &&
        (FUNC_MAGICNUM(*pFuncInfo) >= EH_MAGIC_NUMBER3) &&
        ((FUNC_FLAGS(*pFuncInfo) & FI_EHS_FLAG) != 0))
    {
        /*
         * This function was compiled /EHs so we don't need to do anything in
         * this handler.
         */
        return ExceptionContinueSearch;
    }

    if (IS_UNWINDING(PER_FLAGS(pExcept)))
    {
        // We're at the unwinding stage of things.  Don't care about the
        // exception itself.  (Check this first because it's easier)

        if (FUNC_MAXSTATE(*pFuncInfo) != 0 && CatchDepth == 0)
        {
            // Only unwind if there's something to unwind
            // AND we're being called through the primary RN.

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)

            if (IS_TARGET_UNWIND(PER_FLAGS(pExcept)) && PER_CODE(pExcept) == STATUS_LONGJUMP) {
                    __ehstate_t target_state = __StateFromIp(pFuncInfo,
                                                             pDC,
#if defined(_M_X64)
                                                             pContext->Rip
#elif defined(_M_ARM) || defined(_M_ARM64)
                                                             pContext->Pc
#endif
                                                             );

                    DASSERT(target_state >= EH_EMPTY_STATE
                            && target_state < FUNC_MAXSTATE(*pFuncInfo));

                    __FrameUnwindToState(pRN, pDC, pFuncInfo, target_state);
                    EHTRACE_HANDLER_EXIT(ExceptionContinueSearch);
                    return ExceptionContinueSearch;
            } else if(IS_TARGET_UNWIND(PER_FLAGS(pExcept)) &&
                      PER_CODE(pExcept) == STATUS_UNWIND_CONSOLIDATE)
            {
                PEXCEPTION_RECORD pSehExcept = (PEXCEPTION_RECORD)pExcept;
                __ehstate_t target_state = (__ehstate_t)pSehExcept->ExceptionInformation[3];

                DASSERT(target_state >= EH_EMPTY_STATE
                        && target_state < FUNC_MAXSTATE(*pFuncInfo));
                __FrameUnwindToState((EHRegistrationNode *)pSehExcept->ExceptionInformation[1],
                                     pDC,
                                     pFuncInfo,
                                     target_state);
                EHTRACE_HANDLER_EXIT(ExceptionContinueSearch);
                return ExceptionContinueSearch;
            }
#endif // defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
            __FrameUnwindToEmptyState(pRN, pDC, pFuncInfo);
        }
        EHTRACE_HANDLER_EXIT(ExceptionContinueSearch);
        return ExceptionContinueSearch;     // I don't think this value matters

    } else if (FUNC_NTRYBLOCKS(*pFuncInfo) != 0
        //
        // If the function has no try block, we still want to call the
        // frame handler if there is an exception specification
        //
        || (FUNC_MAGICNUM(*pFuncInfo) >= EH_MAGIC_NUMBER2 && FUNC_PESTYPES(pFuncInfo) != nullptr)
        || (FUNC_MAGICNUM(*pFuncInfo) >= EH_MAGIC_NUMBER3 && ((FUNC_FLAGS(*pFuncInfo) & FI_EHNOEXCEPT_FLAG) != 0))) {


        // NT is looking for handlers.  We've got handlers.
        // Let's check this puppy out.  Do we recognize it?

        int (__cdecl *pfn)(...);

        if (PER_CODE(pExcept) == EH_EXCEPTION_NUMBER
          && PER_NPARAMS(pExcept) >= 3
          && PER_MAGICNUM(pExcept) > EH_MAGIC_NUMBER3
          && (pfn = THROW_FORWARDCOMPAT(*PER_PTHROW(pExcept))) != nullptr) {

            // Forward compatibility:  The thrown object appears to have been
            // created by a newer version of our compiler.  Let that version's
            // frame handler do the work (if one was specified).

#if defined(_DEBUG)
            if (_ValidateExecute((FARPROC)pfn)) {
#endif
                EXCEPTION_DISPOSITION result =
                    (EXCEPTION_DISPOSITION)pfn(pExcept, pRN, pContext, pDC,
                                               pFuncInfo, CatchDepth,
                                               pMarkerRN, recursive);
                EHTRACE_HANDLER_EXIT(result);
                return result;
#if defined(_DEBUG)
            } else {
                terminate(); // Does not return; TKB
            }
#endif

        } else {

            // Anything else: we'll handle it here.
            FindHandler(pExcept, pRN, pContext, pDC, pFuncInfo, recursive,
              CatchDepth, pMarkerRN);
        }

        // If it returned, we didn't have any matches.

        } // NT was looking for a handler

    // We had nothing to do with it or it was rethrown.  Keep searching.
    EHTRACE_HANDLER_EXIT(ExceptionContinueSearch);
    return ExceptionContinueSearch;

} // InternalCxxFrameHandler


////////////////////////////////////////////////////////////////////////////////
//
// FindHandler - find a matching handler on this frame, using all means
// available.
//
// Description:
//     If the exception thrown was an MSC++ EH, search handlers for match.
//     Otherwise, if we haven't already recursed, try to translate.
//     If we have recursed (ie we're handling the translator's exception), and
//         it isn't a typed exception, call terminate.
//
// Returns:
//      Returns iff exception was not handled.
//
// Assumptions:
//      Only called if there are handlers in this function.

#pragma warning(push)
#pragma warning(disable: 4189) // local variable is initialized but not referenced
                
static void FindHandler(
    EHExceptionRecord *pExcept,         // Information for this (logical)
                                        //   exception
    EHRegistrationNode *pRN,            // Dynamic information for subject frame
    CONTEXT *pContext,                  // Context info
    DispatcherContext *pDC,             // Context within subject frame
    FuncInfo *pFuncInfo,                // Static information for subject frame
    BOOLEAN recursive,                  // TRUE if we're handling the
                                        //   translation
    int CatchDepth,                     // Level of nested catch that is being
                                        //   checked
    EHRegistrationNode *pMarkerRN       // Extra marker RN for nested catch
                                        //   handling
)
{
    EHTRACE_ENTER;

    BOOLEAN IsRethrow = FALSE;
    BOOLEAN gotMatch = FALSE;

    // Get the current state (machine-dependent)
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
    __ehstate_t curState = __StateFromControlPc(pFuncInfo, pDC);
    EHRegistrationNode EstablisherFrame;
    /*
     * Here We find what is the actual State of current function. The way we
     * do this is first get State from ControlPc.
     *
     * Remember we have __GetUnwindTryBlock to remember the last State for which
     * Exception was handled and __GetCurrentState for retriving the current
     * state of the function. Please Note that __GetCurrentState is used
     * primarily for unwinding purpose.
     *
     * Also remember that all the catch blocks act as funclets. This means that
     * ControlPc for all the catch blocks are different from ControlPc of parent
     * catch block or function.
     *
     * take a look at this example
     * try {
     *   // STATE1 = 1
     *   try {
     *     // STATE2
     *     // THROW
     *   } catch (...) { // CatchB1
     *     // STATE3
     *     // RETHROW OR NEW THROW
     *   }
     * } catch (...) { // CatchB2
     * }
     *
     * If we have an exception comming from STATE3, the FindHandler will be
     * called for CatchB1, at this point we do the test which State is our
     * real state, curState from ControlPc or state from __GetUnwindTryBlock.
     * Since curState from ControlPc is greater, we know that real State is
     * curState from ControlPc and thus we update the UnwindTryBlockState.
     *
     * On further examination, we found out that there is no handler within
     * this catch block, we return without handling the exception. For more
     * info on how we determine if we have handler, have a look at
     * __GetRangeOfTrysToCheck.
     *
     * Now FindHandler will again be called for parent function. Here again
     * we test which is real State, state from ControlPc or State from
     * __GetUnwindTryBlock. This time state from __GetUnwindTryBlock is correct.
     *
     * Also look at code in __CxxCallCatchBlock, you will se that as soon as we get
     * out of last catch block, we reset __GetUnwindTryBlock state to -1.
     */

    _GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFrame);
    if (curState > __GetUnwindTryBlock(pRN, pDC, pFuncInfo)) {
        __SetState(&EstablisherFrame, pDC, pFuncInfo, curState);
        __SetUnwindTryBlock(pRN, pDC, pFuncInfo, /*curTry*/ curState);
    } else {
        curState = __GetUnwindTryBlock(pRN, pDC, pFuncInfo);
    }
#else
    __ehstate_t curState = GetCurrentState(pRN, pDC, pFuncInfo);
#endif
    DASSERT(curState >= EH_EMPTY_STATE && curState < FUNC_MAXSTATE(*pFuncInfo));

    // Check if it's a re-throw.  Use the exception we stashed away if it is.
    if (PER_IS_MSVC_EH(pExcept) && PER_PTHROW(pExcept) == nullptr) {

        if (_pCurrentException == nullptr) {
            // Oops!  User re-threw a non-existant exception!  Let it propogate.
            EHTRACE_EXIT;
            return;
        }

        pExcept = _pCurrentException;
        pContext = _pCurrentExContext;
        IsRethrow = TRUE;
#if _EH_RELATIVE_OFFSETS 
        _SetThrowImageBase((ptrdiff_t)pExcept->params.pThrowImageBase);
#endif

        DASSERT(_ValidateRead(pExcept));
        DASSERT(!PER_IS_MSVC_EH(pExcept) || PER_PTHROW(pExcept) != nullptr);

        //
        // We know it is a rethrow -- did we come here as a result of an
        // exception re-thrown from CallUnexpected() ?
        //
        if( _pCurrentFuncInfo != nullptr )
        {
            ESTypeList* pCurrentFuncInfo = _pCurrentFuncInfo;   // remember it in a local variable
            _pCurrentFuncInfo = nullptr;   // and reset it immediately -- so we don't forget to do it later

            // Does the exception thrown by CallUnexpected belong to the exception specification?

            if( IsInExceptionSpec(pExcept, pCurrentFuncInfo) )
            {
                // Yes it does -- so "continue the search for another handler at the call of the function
                // whose exception-specification was violated"
                ;
            }
            else
            {
                // Nope, it does not. Is std::bad_exception allowed by the spec?

                if( Is_bad_exception_allowed(pCurrentFuncInfo) )
                {
                    // yup -- so according to the standard, we need to replace the thrown
                    // exception by an implementation-defined object of the type std::bad_exception
                    // and continue the search for another handler at the call of the function
                    // whose exception-specification was violated.

                    // Just throw bad_exception -- we will then come into FindHandler for the third time --
                    // but make sure we will not get here again

                    __DestructExceptionObject(pExcept, TRUE);   // destroy the original object

                    throw std::bad_exception();
                }
                else
                {
                    terminate();
                }
            }
        }
    }

    if (PER_IS_MSVC_EH(pExcept)) {
        // Looks like it's ours.  Let's see if we have a match:
        //
        // First, determine range of try blocks to consider:
        // Only try blocks which are at the current catch depth are of interest.

        unsigned curTry;
        unsigned end;

        if( FUNC_NTRYBLOCKS(*pFuncInfo) > 0 )
        {
            TryBlockMapEntry *pEntry = __GetRangeOfTrysToCheck(pRN,
                                                            pFuncInfo,
                                                            CatchDepth,
                                                            curState,
                                                            &curTry,
                                                            &end,
                                                            pDC);

            // Scan the try blocks in the function:
            for (; curTry < end; curTry++, pEntry++) {
                HandlerType *pCatch;
#if _EH_RELATIVE_OFFSETS
                __int32 const *ppCatchable;
#else
                CatchableType * const *ppCatchable;
#endif
                CatchableType *pCatchable;
                int catches;
                int catchables;

                if (TBME_LOW(*pEntry) > curState || curState > TBME_HIGH(*pEntry)) {
                    continue;
                }

                // Try block was in scope for current state.  Scan catches for this
                // try:
                pCatch  = TBME_PCATCH(*pEntry, 0);
                for (catches = TBME_NCATCHES(*pEntry); catches > 0; catches--,
                pCatch++) {

                    // Scan all types that thrown object can be converted to:
                    ppCatchable = THROW_CTLIST(*PER_PTHROW(pExcept));
                    for (catchables = THROW_COUNT(*PER_PTHROW(pExcept));
                    catchables > 0; catchables--, ppCatchable++) {

#if _EH_RELATIVE_OFFSETS
                        pCatchable = (CatchableType *)(_GetThrowImageBase() + *ppCatchable);
#else
                        pCatchable = *ppCatchable;
#endif

                        if (!__TypeMatch(pCatch, pCatchable, PER_PTHROW(pExcept))) {
                            continue;
                        }

                        // OK.  We finally found a match.  Activate the catch.  If
                        // control gets back here, the catch did a re-throw, so
                        // keep searching.

                        gotMatch = TRUE;

                        CatchIt(pExcept,
                                pRN,
                                pContext,
                                pDC,
                                pFuncInfo,
                                pCatch,
                                pCatchable,
                                pEntry,
                                CatchDepth,
                                pMarkerRN,
                                IsRethrow
#if defined (_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
                                , recursive
#endif
                                );
                        goto NextTryBlock;

                    } // Scan posible conversions
                } // Scan catch clauses
    NextTryBlock: ;
            } // Scan try blocks
        } // if FUNC_NTRYBLOCKS( pFuncInfo ) > 0
#if defined(_DEBUG)
        else
        {
            //
            // This can only happen if the function has an exception specification
            // but no try/catch blocks
            //
            DASSERT( FUNC_MAGICNUM(*pFuncInfo) >= EH_MAGIC_NUMBER2 );
            DASSERT( FUNC_PESTYPES(pFuncInfo) != nullptr );
        }
#endif

#if defined(_M_IX86)
        if (recursive) {
            //
            // A translation was provided, but this frame didn't catch it.
            // Destruct the translated object before returning; if destruction
            // raises an exception, terminate.
            //
            // This is not done for Win64 platforms.  On those, the translated
            // object is destructed in __CxxCallCatchBlock.
            //
            __DestructExceptionObject(pExcept, TRUE);
        }
#endif

#if !defined(_M_ARM) || defined(_M_ARM_NT)
        //
        // We haven't found the match -- let's look at the exception spec and see if our try
        // matches one of the listed types.
        //
        // This block also handles noexcept termination if the current function is noexcept and
        // we are not handling a nested throw or re-throw. e.g.:
        //
        // try { 
        //   try { throw 1; } (1)
        //   catch(int) { throw; /* or throw <something new>; */ } (2)
        // } catch(int) {} (3)
        //
        // During the initial search for a handler when throwing the original exception (1)
        // we find and execute catch handler (2). This throws, which re-enters this function
        // while in the context of a catch, but which cannot find a handler. We cannot terminate here,
        // we're in a recursive context. Just let it go, and we'll return back to the original search
        // from (1) and find (3). If we fail to find a handler, e.g. (3) didn't match, we would
        // then terminate at this point as we are not in a catch block.
        //
        // Catch block detection uses CatchDepth on X86, or _ExecutionInCatch on other platforms. On
        // these platforms CatchDepth is always 0 - we don't maintain a stack of entered try/catch
        // states.
        if( !gotMatch && FUNC_MAGICNUM(*pFuncInfo) >= EH_MAGIC_HAS_ES && 
            (FUNC_PESTYPES(pFuncInfo) != nullptr || (((FUNC_FLAGS(*pFuncInfo) & FI_EHNOEXCEPT_FLAG) != 0) && 
#if _M_IX86
                CatchDepth == 0
#else
                !_ExecutionInCatch(pDC, pFuncInfo)
#endif
            )))
        {
            // Are we noexcept?
            if( (FUNC_FLAGS(*pFuncInfo) & FI_EHNOEXCEPT_FLAG) != 0 )
            {
                terminate();
            }

            if( !IsInExceptionSpec(pExcept, FUNC_PESTYPES(pFuncInfo)) )
            {
                // Nope, it does not. Call unexpected

                //
                // We must unwind the stack before calling unexpected -- this makes it work
                // as if it were inside catch(...) clause
                //
#if defined (_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
                EHRegistrationNode *pEstablisher = pRN;
                EHRegistrationNode EstablisherFramePointers;
                pEstablisher = _GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
                PVOID pExceptionObjectDestroyed = nullptr;
                _UnwindNestedFrames(pRN,
                                    pExcept,
                                    pContext,
                                    pEstablisher,
                                    nullptr,
                                    -1,
                                    pFuncInfo,
                                    pDC,
                                    recursive
                                    );
#else

                EHExceptionRecord *pSaveException = _pCurrentException;
                CONTEXT *pSaveExContext = _pCurrentExContext;

                _pCurrentException = pExcept;
                _pCurrentExContext = pContext;

                if (pMarkerRN == nullptr) {
                    _UnwindNestedFrames(pRN, pExcept);
                } else {
                    _UnwindNestedFrames(pMarkerRN, pExcept);
                }
                __FrameUnwindToEmptyState(pRN, pDC, pFuncInfo);

                CallUnexpected(FUNC_PESTYPES(pFuncInfo));
                _pCurrentException = pExcept;
                _pCurrentExContext = pContext;
#endif
            }
        }
#endif // !defined(_M_ARM)

    } // It was a C++ EH exception
    else {
        // Not ours.  But maybe someone told us how to make it ours.
        if( FUNC_NTRYBLOCKS(*pFuncInfo) > 0 ) {
            if (!recursive) {
                FindHandlerForForeignException(pExcept, pRN, pContext, pDC,
                pFuncInfo, curState, CatchDepth, pMarkerRN);
            } else {
                // We're recursive, and the exception wasn't a C++ EH!
                // Translator threw something uninteligable.

                // Two choices here: we could let the new exception take over, or we could abort. We abort.
                terminate();
            }
        }
    } // It wasn't our exception

    DASSERT( _pCurrentFuncInfo == nullptr );   // never leave it initialized with something

    EHTRACE_EXIT;
}

#pragma warning(pop)

////////////////////////////////////////////////////////////////////////////////
//
// FindHandlerForForeignException - We've got an exception which wasn't ours.
//     Try to translate it into C++ EH, and also check for match with ellipsis.
//
// Description:
//     If an SE-to-EH translator has been installed, call it.  The translator
//     must throw the appropriate typed exception or return.  If the translator
//     throws, we invoke FindHandler again as the exception filter.
//
// Returns:
//     Returns if exception was not fully handled.
//     No return value.
//
// Assumptions:
//     Only called if there are handlers in this function.

static void FindHandlerForForeignException(
    EHExceptionRecord *pExcept,         // Information for this (logical)
                                        //   exception
    EHRegistrationNode *pRN,            // Dynamic information for subject frame
    CONTEXT *pContext,                  // Context info
    DispatcherContext *pDC,             // Context within subject frame
    FuncInfo *pFuncInfo,                // Static information for subject frame
    __ehstate_t curState,               // Current state
    int CatchDepth,                     // Level of nested catch that is being
                                        //   checked
    EHRegistrationNode *pMarkerRN       // Extra marker RN for nested catch
                                        //   handling
)
{
    EHTRACE_ENTER;

    unsigned curTry;
    unsigned end;
    TryBlockMapEntry *pEntry;
    // We don't want to touch BreakPoint generated Exception.
    if (PER_CODE(pExcept) == STATUS_BREAKPOINT) {
        EHTRACE_EXIT;
        return;
    }

    if (__pSETranslator != nullptr && __pSETranslator != EncodePointer(nullptr) &&
      pExcept->ExceptionCode != MANAGED_EXCEPTION_CODE &&
      pExcept->ExceptionCode != MANAGED_EXCEPTION_CODE_V4) {

        // Call the translator.  If the translator knows what to
        // make of it, it will throw an appropriate C++ exception.
        // We intercept it and use it (recursively) for this
        // frame.  Don't recurse more than once.

        if (__CallSETranslator(pExcept, pRN, pContext, pDC, pFuncInfo,
          CatchDepth, pMarkerRN, TDTransOffset)) {
            EHTRACE_EXIT;
            return;
        }
    }

    DASSERT( FUNC_NTRYBLOCKS(*pFuncInfo) != 0 );

    // Didn't have a translator, or the translator returned normally (i.e.
    // didn't translate it).  Still need to check for match with ellipsis:
    pEntry = __GetRangeOfTrysToCheck(pRN, pFuncInfo, CatchDepth, curState,
      &curTry, &end, pDC);

    // Scan the try blocks in the function:
    for (; curTry < end; curTry++, pEntry++) {

        // If the try-block was in scope *and* the last catch in that try is an
        // ellipsis (no other can be)
        if (curState < TBME_LOW(*pEntry) || curState > TBME_HIGH(*pEntry)
          || !((HT_IS_TYPE_ELLIPSIS(TBME_CATCH(*pEntry, TBME_NCATCHES(*pEntry) - 1)))
               && !(HT_IS_STD_DOTDOT(TBME_CATCH(*pEntry, TBME_NCATCHES(*pEntry) - 1))))) {
            continue;
        }

        // Found an ellipsis.  Handle exception.

        CatchIt(pExcept,
                pRN,
                pContext,
                pDC,
                pFuncInfo,
                TBME_PCATCH(*pEntry, TBME_NCATCHES(*pEntry) - 1),
                nullptr,
                pEntry,
                CatchDepth,
                pMarkerRN,
                TRUE
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
                ,FALSE
#endif
                );

        // If it returns, handler re-threw.  Keep searching.

    } // Search for try

    EHTRACE_EXIT;

    // If we got here, that means we didn't have anything to do with the
    // exception.  Continue search.
}
#endif

PGETWINRT_OOM_EXCEPTION g_OutOfMemoryExceptionCallback = nullptr;

extern "C" _VCRTIMP void __cdecl _SetWinRTOutOfMemoryExceptionCallback(PGETWINRT_OOM_EXCEPTION pCallback)
{
	g_OutOfMemoryExceptionCallback = pCallback;
}

////////////////////////////////////////////////////////////////////////////////
//
// __TypeMatch - Check if the catch type matches the given throw conversion.
//
// Returns:
//     TRUE if the catch can catch using this throw conversion, FALSE otherwise.

extern "C" _VCRTIMP int __cdecl __TypeMatch(
    HandlerType *pCatch,                // Type of the 'catch' clause
    CatchableType *pCatchable,          // Type conversion under consideration
    ThrowInfo *pThrow                   // General information about the thrown
                                        //   type.
) {
    // First, check for match with ellipsis:
    if (HT_IS_TYPE_ELLIPSIS(*pCatch)) {
        return TRUE;
    }

	if (HT_ISBADALLOCCOMPAT(*pCatch) && CT_ISSTDBADALLOC(*pCatchable))
	{
		return true;
	}

    // Not ellipsis; the basic types match if it's the same record *or* the
    // names are identical.
    if (HT_PTD(*pCatch) != CT_PTD(*pCatchable)
      && strcmp(HT_NAME(*pCatch), CT_NAME(*pCatchable)) != 0) {
        return FALSE;
    }

    // Basic types match.  The actual conversion is valid if:
    //   caught by ref if ref required *and*
    //   the qualifiers are compatible *and*
    //   the alignments match *and*
    //   the volatility matches

    return (!CT_BYREFONLY(*pCatchable) || HT_ISREFERENCE(*pCatch))
      && (!THROW_ISCONST(*pThrow) || HT_ISCONST(*pCatch))
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
      && (!THROW_ISUNALIGNED(*pThrow) || HT_ISUNALIGNED(*pCatch))
#endif
      && (!THROW_ISVOLATILE(*pThrow) || HT_ISVOLATILE(*pCatch));
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


////////////////////////////////////////////////////////////////////////////////
//
// __FrameUnwindToState - Unwind this frame until specified state is reached.
//
// Returns:
//     No return value.
//
// Side Effects:
//     All objects on frame which go out of scope as a result of the unwind are
//       destructed.
//     Registration node is updated to reflect new state.
//
// Usage:
//      This function is called both to do full-frame unwind during the unwind
//      phase (targetState = -1), and to do partial unwinding when the current
//      frame has an appropriate catch.

#if 0
extern "C" void __FrameUnwindToState (
    EHRegistrationNode *pRN,            // Registration node for subject
                                        //   function
    DispatcherContext *pDC,             // Context within subject frame
    FuncInfo *pFuncInfo,                // Static information for subject
                                        //   function
    __ehstate_t targetState             // State to unwind to
) {
    UNREFERENCED_PARAMETER(pDC); // This parameter is not used in some compilations

    EHTRACE_ENTER;

#if _EH_RELATIVE_OFFSETS
    ptrdiff_t unwindImageBase = _GetImageBase();
#endif
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
    __ehstate_t curState = __GetCurrentState(pRN, pDC, pFuncInfo);
#else
    __ehstate_t curState = __GetUnwindState(pRN, pDC, pFuncInfo);
#endif
    __ProcessingThrow++;
    __try {
#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
    while (curState != EH_EMPTY_STATE && curState > targetState)
#else
    while (curState != targetState)
#endif
    {
        DASSERT((curState > EH_EMPTY_STATE)
          && (curState < FUNC_MAXSTATE(*pFuncInfo)));

        // Get state after next unwind action
        __ehstate_t nxtState = UWE_TOSTATE(FUNC_UNWIND(*pFuncInfo, curState));

        __try {
            // Call the unwind action (if one exists):

            if (UWE_ACTION(FUNC_UNWIND(*pFuncInfo, curState)) != nullptr) {

                // Before calling unwind action, adjust state as if it were
                // already completed:
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
                __SetState(pRN, pDC, pFuncInfo, nxtState);
#else
                SetState(pRN, pDC, pFuncInfo, nxtState);
#endif

                EHTRACE_FMT2("Unwind from state %d to state %d", curState, nxtState);
                _CallSettingFrame(__GetAddress(UWE_ACTION(FUNC_UNWIND(*pFuncInfo, curState)),
                                               pDC),
                                  REAL_FP(pRN, pFuncInfo),
#if defined(_M_ARM_NT) || defined(_M_ARM64)
                                  (PULONG)pDC->NonVolatileRegisters,
#endif
                                  0x103);
#if _EH_RELATIVE_OFFSETS
                _SetImageBase(unwindImageBase);
#endif
            }

        } __except(EHTRACE_EXCEPT(__FrameUnwindFilter(exception_info()))) {
            ; // Deliberately do nothing
        }

        curState = nxtState;
    }
    } __finally {
        if (__ProcessingThrow > 0) {
            __ProcessingThrow--;
        }
    }


    // Now that we're done, set the frame to reflect the final state.

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
    DASSERT(curState == EH_EMPTY_STATE || curState <= targetState);
#else
    DASSERT(curState == targetState);
#endif

    EHTRACE_FMT2("Move from state %d to state %d", __GetUnwindState(pRN, pDC, pFuncInfo), curState);
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
    __SetState(pRN, pDC, pFuncInfo, curState);
#else
    SetState(pRN, pDC, pFuncInfo, curState);
#endif

    EHTRACE_EXIT;
}

////////////////////////////////////////////////////////////////////////////////
//
// CatchIt - A handler has been found for the thrown type.  Do the work to
//   transfer control.
//
// Description:
//     Builds the catch object
//     Unwinds the stack to the point of the try
//     Calls the address of the handler (funclet) with the frame set up for that
//       function but without resetting the stack.
//     Handler funclet returns address to continue execution, or nullptr if the
//       handler re-threw ("throw;" lexically in handler)
//     If the handler throws an EH exception whose exception info is nullptr, then
//       it's a re-throw from a dynamicly enclosed scope.
//
// It is an open question whether the catch object is built before or after the local unwind.
//
// Returns:
//     No return value.  Returns iff handler re-throws.
static void CatchIt(
    EHExceptionRecord *pExcept,         // The exception thrown
    EHRegistrationNode *pRN,            // Dynamic info of function with catch
    CONTEXT *pContext,                  // Context info
    DispatcherContext *pDC,             // Context within subject frame
    FuncInfo *pFuncInfo,                // Static info of function with catch
    HandlerType *pCatch,                // The catch clause selected
    CatchableType *pConv,               // The rules for making the conversion
    TryBlockMapEntry *pEntry,           // Description of the try block
    int CatchDepth,                     // How many catches are we nested in?
    EHRegistrationNode *pMarkerRN,      // Special node if nested in catch
    BOOLEAN IsRethrow                   // Is this a rethrow ?
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
    , BOOLEAN recursive
#endif // defined(_POWERPC)
) {
    // These parameters are not used in some compilations
    UNREFERENCED_PARAMETER(CatchDepth);
    UNREFERENCED_PARAMETER(pMarkerRN);
    UNREFERENCED_PARAMETER(IsRethrow);

    EHTRACE_ENTER_FMT1("Catching object @ 0x%p", PER_PEXCEPTOBJ(pExcept));

    EHRegistrationNode *pEstablisher = pRN;

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
    EHRegistrationNode EstablisherFramePointers;
    pEstablisher = _GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
#else
    void *continuationAddress;
#endif // defined(_POWERPC)

    // Copy the thrown object into a buffer in the handler's stack frame,
    // unless the catch was by elipsis (no conversion) OR the catch was by
    // type without an actual 'catch object'.

    if (pConv != nullptr) {
        __BuildCatchObject(pExcept, pEstablisher, pCatch, pConv);
    }

    // Unwind stack objects to the entry of the try that caught this exception.

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
    // This call will never return. This call will end up calling CxxCallCatchBlock
    // through RtlUnwind (STATUS_CONSULIDATE_FRAMES) mechanism.
    _UnwindNestedFrames(pRN,
                        pExcept,
                        pContext,
                        pEstablisher,
                        __GetAddress(HT_HANDLER(*pCatch), pDC),
                        TBME_LOW(*pEntry),
                        pFuncInfo,
                        pDC,
#if defined(_M_ARM) && !defined(_M_ARM_NT)
                        FALSE
#else
                        recursive
#endif
                        );
#else

    if (pMarkerRN == nullptr) {
        _UnwindNestedFrames(pRN, pExcept);
    } else {
        _UnwindNestedFrames(pMarkerRN, pExcept);
    }


    __FrameUnwindToState(pEstablisher, pDC, pFuncInfo, TBME_LOW(*pEntry));

    // Call the catch.  Separated out because it introduces a new registration
    // node.

    EHTRACE_FMT2("Move from state %d to state %d", __GetUnwindState(pRN, pDC, pFuncInfo), TBME_HIGH(*pEntry) + 1);
    SetState(pRN, pDC, pFuncInfo, TBME_HIGH(*pEntry) + 1);

    continuationAddress = CallCatchBlock(pExcept,
                                         pEstablisher,
                                         pContext,
                                         pFuncInfo,
                                         __GetAddress(HT_HANDLER(*pCatch), pDC),
                                         CatchDepth,
                                         0x100);

    // Transfer control to the continuation address.  If no continuation then
    // it's a re-throw, so return.

    if (continuationAddress != nullptr) {

        _JumpToContinuation(continuationAddress, pRN);
        // No return.

    }
    EHTRACE_EXIT;
#endif
}

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)  /* { { */

static void __RethrowException(EHExceptionRecord *pThisException)
{
    RaiseException( pThisException->ExceptionCode,
                    pThisException->ExceptionFlags,
                    pThisException->NumberParameters,
                    (PULONG_PTR)&(pThisException->params) );
}

extern "C" void *__CxxCallCatchBlock(
    EXCEPTION_RECORD *pExcept
    )
{
    int rethrow = 0, TranslatedCatch = 0;
    void *handlerAddress;
    void *continuationAddress = nullptr;
    FuncInfo *pFuncInfo;
    FRAMEINFO FrameInfo;
    CONTEXT *pContext, *pSaveContext;
    EHRegistrationNode *pEstablisherFrame;
    EHExceptionRecord *pThisException, *pSaveException, *pForeignException = nullptr;

    pSaveContext = _pCurrentExContext;
    pSaveException = _pCurrentException;

    // Copy Necessary Information which is passed from UnwindNestedFrames.
    pThisException      = (EHExceptionRecord *) pExcept->ExceptionInformation[6];
    pFuncInfo           = (FuncInfo *)          pExcept->ExceptionInformation[5];
    pContext            = (CONTEXT *)           pExcept->ExceptionInformation[4];
    handlerAddress      = (void *)              pExcept->ExceptionInformation[2];
    pEstablisherFrame   = (EHRegistrationNode *)pExcept->ExceptionInformation[1];

    __except_validate_context_record(pContext);

    _pCurrentException  = pThisException;
    _pCurrentExContext = pContext;
    FRAMEINFO *pFrameInfo = _CreateFrameInfo(&FrameInfo,
                                             (void *)PER_PEXCEPTOBJ(_pCurrentException));

    if (pExcept->ExceptionInformation[7]) {
    // Stuff for SehTralation book keeping
        // This Exception Object is Translation of Froigen Exception And should
        // be destroyed in any case. If there is rethrow, throw ForeignException.
        // IMPRORTANT:     One one else can rethrow this Exception Object.
        TranslatedCatch = 1;
        pForeignException = _pForeignExcept;
    // End Translation Stuff
    }

    __try {
        __try{
            continuationAddress = _CallSettingFrame(handlerAddress,
                                                    pEstablisherFrame,
#if defined(_M_ARM_NT) || defined(_M_ARM64)
                                                    (PULONG)pExcept->ExceptionInformation[10],
#endif
                                                    0x100);
        } __except(ExFilterRethrow(exception_info(),
                                   pThisException,
                                   &rethrow)) {
            rethrow = 1;
            cxxReThrow = false;
            if (TranslatedCatch) {
                // Note in case of rethrow, no one else can convert rethrow to
                // pThisException. This means only this except will deal with conversion
                // of rethrow to pThisException. Instead of pThisException, we
                // will throw original Foreign Exception. Also we will need to
                // destroy Exception Object of before Raising Foreign Exception.
                __DestructExceptionObject(pThisException, TRUE);
                __RethrowException(pForeignException);
            } else {
                __RethrowException(pThisException);
            }
        }
    } __finally {
        _FindAndUnlinkFrame(pFrameInfo);
        if ( !rethrow &&
             PER_IS_MSVC_EH(pThisException) &&
             _IsExceptionObjectToBeDestroyed(PER_PEXCEPTOBJ(pThisException)))
        {
            __DestructExceptionObject(pThisException, TRUE);
        }
        _pCurrentException = pSaveException;
        _pCurrentExContext = pSaveContext;
    }
#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64)
    UNWINDHELP(*pEstablisherFrame, FUNC_DISPUNWINDHELP(*pFuncInfo)) = -2;
#endif
    return continuationAddress;
}

////////////////////////////////////////////////////////////////////////////////
//
// ExFilterRethrow - Exception filter for re-throw exceptions.
//
// Returns:
//     EXCEPTION_EXECUTE_HANDLER - exception was a re-throw
//     EXCEPTION_CONTINUE_SEARCH - anything else
//
// Side-effects: sets rethrow = TRUE if exception objects of the two Exception matches

static __declspec(guard(ignore)) int ExFilterRethrow(
    EXCEPTION_POINTERS *pExPtrs,
    EHExceptionRecord *pOldExcept,
    int *rethrow
) {
    // Get the exception record thrown (don't care about other info)
    EHExceptionRecord *pExcept = (EHExceptionRecord *)pExPtrs->ExceptionRecord;
    *rethrow = 0;
    if (PER_IS_MSVC_EH(pExcept) && PER_PEXCEPTOBJ(pExcept) == PER_PEXCEPTOBJ(pOldExcept))
        *rethrow = 1;
    // Check if it's ours and it's has no exception information.
    if (PER_IS_MSVC_EH(pExcept) && PER_PTHROW(pExcept) == nullptr) {
    // In a rethrow, ExceptionCode isn't set to EH_EXCEPTION_NUMBER even for a
    // C++ Exception. Hence we use cxxReThrow in __InternalCxxFrameHandler
    // to check for a C++ Exception
        cxxReThrow = true;
        *rethrow = 1;
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

#elif defined(_M_IX86)

////////////////////////////////////////////////////////////////////////////////
//
// CallCatchBlock - continuation of CatchIt.
//
// This is seperated from CatchIt because it needs to introduce an SEH/EH frame
//   in case the catch block throws.  This frame cannot be added until unwind of
//   nested frames has been completed (otherwise this frame would be the first
//   to go).

static __declspec(guard(ignore)) void *CallCatchBlock(
    EHExceptionRecord *pExcept,         // The exception thrown
    EHRegistrationNode *pRN,            // Dynamic info of function with catch
    CONTEXT *pContext,                  // Context info
    FuncInfo *pFuncInfo,                // Static info of function with catch
    void *handlerAddress,               // Code address of handler
    int CatchDepth,                     // How deeply nested in catch blocks
                                        //   are we?
    unsigned long NLGCode               // NLG destination code
) {
    EHTRACE_ENTER;

    // Address where execution resumes after exception handling completed.
    // Initialized to non-nullptr (value doesn't matter) to distinguish from
    // re-throw in __finally.
    void *continuationAddress = handlerAddress;

    BOOL ExceptionObjectDestroyed = FALSE;

    // The stack pointer at entry to the try must be saved, in case there is
    // another try inside this catch.  We'll restore it on our way out.
    void *saveESP = PRN_STACK(pRN);

    // Push this catch block's frame info on a linked list
    FRAMEINFO FrameInfo;
    FRAMEINFO *pFrameInfo = _CreateFrameInfo(&FrameInfo, PER_PEXCEPTOBJ(pExcept));

    // Save the current exception in case of a rethrow.  Save the previous value
    // on the stack, to be restored when the catch exits.
    EHExceptionRecord *pSaveException = _pCurrentException;
    CONTEXT *pSaveExContext = _pCurrentExContext;

    _pCurrentException = pExcept;
    _pCurrentExContext = pContext;

    __try {
        __try {
            // Execute the handler as a funclet, whose return value is the
            // address to resume execution.

            continuationAddress = _CallCatchBlock2(pRN, pFuncInfo,
              handlerAddress, CatchDepth, NLGCode);
        } __except(EHTRACE_EXCEPT(ExFilterRethrow(exception_info()))) {
            cxxReThrow=false;
            // Here we are exiting the catch block on rethrow out of this
            // catch block. To keep the order of destruction and construction
            // same when the the rethrow was from function or was inline, here
            // we unwind to the parent state for this catch.
            UnwindMapEntry *pUnwindMap = pFuncInfo->pUnwindMap;
            int cState = GetCurrentState(pRN, handlerAddress, pFuncInfo);
            TryBlockMapEntry *pTryBlockMap = pFuncInfo->pTryBlockMap;
            unsigned int i;
            for (i = 0; i < pFuncInfo->nTryBlocks; i++) {
                if (cState > pTryBlockMap[i].tryHigh &&
                    cState <= pTryBlockMap[i].catchHigh) {
                    cState = pTryBlockMap[i].tryHigh +1;
                    cState = pUnwindMap[cState].toState;
                    break;
                }
            }
            __FrameUnwindToState(pRN, nullptr, pFuncInfo, cState);
            // If the handler threw a typed exception without exception info or
            // exception object, then it's a re-throw, so return.  Otherwise
            // it's a new exception, which takes precedence over this one.
            continuationAddress = nullptr;
        }
    } __finally {
        EHTRACE_SAVE_LEVEL;
        EHTRACE_FMT1("Executing __finally, %snormal termination", _abnormal_termination() ? "ab" : "");

        // Restore the saved stack pointer, so the stack can be reset when
        // we're done.
        PRN_STACK(pRN) = saveESP;

        // Pop this catch block's frame info
        _FindAndUnlinkFrame(pFrameInfo);

        // Restore the 'current exception' for a possibly enclosing catch
        _pCurrentException = pSaveException;
        _pCurrentExContext = pSaveExContext;

        // Destroy the original exception object if we're not exiting on a
        // re-throw and the object isn't also in use by a more deeply nested
        // catch.  Note that the catch handles destruction of its parameter.

        if (PER_IS_MSVC_EH(pExcept) && !ExceptionObjectDestroyed
          && continuationAddress != nullptr
            && _IsExceptionObjectToBeDestroyed(PER_PEXCEPTOBJ(pExcept))
            ) {
            __DestructExceptionObject(pExcept, abnormal_termination() != FALSE);
        }

        EHTRACE_RESTORE_LEVEL(!!_abnormal_termination());
    }
    EHTRACE_EXIT;
    return continuationAddress;
}


////////////////////////////////////////////////////////////////////////////////
//
// ExFilterRethrow - Exception filter for re-throw exceptions.
//
// Returns:
//     EXCEPTION_EXECUTE_HANDLER - exception was a re-throw
//     EXCEPTION_CONTINUE_SEARCH - anything else
//
// Side-effects: NONE.

static int ExFilterRethrow(
    EXCEPTION_POINTERS *pExPtrs
) {
    // Get the exception record thrown (don't care about other info)
    EHExceptionRecord *pExcept = (EHExceptionRecord *)pExPtrs->ExceptionRecord;

    // Check if it's ours and it's has no exception information.
    if (PER_IS_MSVC_EH(pExcept) && PER_PTHROW(pExcept) == nullptr) {
    // In a rethrow, ExceptionCode isn't set to EH_EXCEPTION_NUMBER even for a
    // C++ Exception. Hence we use cxxReThrow in __InternalCxxFrameHandler
    // to check for a C++ Exception
        cxxReThrow = true;
        return EXCEPTION_EXECUTE_HANDLER;
    } else {
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

#endif /* } } */
#endif

////////////////////////////////////////////////////////////////////////////////
//
// __BuildCatchObjectHelper - Copy or construct the catch object from the object thrown.
//
// Returns:
//     0 if nothing to be done for constructing object from caller side
//     1 if single parameter constructor is to be called.
//     2 if two parameter constructor ist to be called.
//
// Side-effects:
//     A buffer in the subject function's frame is initialized.
//
// Open issues:
//     What happens if the constructor throws?  (or faults?)

extern "C" _VCRTIMP int __BuildCatchObjectHelper(
    EHExceptionRecord *pExcept,         // Original exception thrown
    void *pRN,                          // This is a pointer to the object
                                        // that we want to build while doing
                                        // COM+ eh. If we are in our own eh,
                                        // then this is a Registration node of
                                        // catching function
    HandlerType *pCatch,                // The catch clause that got it
    CatchableType *pConv                // The conversion to use
) {
    int retval = 0;
    EHTRACE_ENTER;

    // If the catch is by ellipsis, then there is no object to construct.
    // If the catch is by type(No Catch Object), then leave too!
    if (HT_IS_TYPE_ELLIPSIS(*pCatch) ||
        (!HT_DISPCATCH(*pCatch) && !HT_ISCOMPLUSEH(*pCatch))) {
        EHTRACE_EXIT;
        return 0;
    }

    void **pCatchBuffer;
    if ( HT_ISCOMPLUSEH(*pCatch))
    {
        pCatchBuffer = (void **)pRN;
    }
    else
    {
#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        pCatchBuffer = (void **)__OffsetToAddress(
                                HT_DISPCATCH(*pCatch),
                                *((EHRegistrationNode *)pRN),
                                HT_FRAMENEST(*pCatch)
                                );
#else
        pCatchBuffer = (void **)__OffsetToAddress(
                                HT_DISPCATCH(*pCatch),
                                (EHRegistrationNode *)pRN,
                                HT_FRAMENEST(*pCatch)
                                );
#endif
    }
    __try {
		if (HT_ISBADALLOCCOMPAT(*pCatch) && CT_ISSTDBADALLOC(*pConv) && g_OutOfMemoryExceptionCallback)
		{
			void* pException = g_OutOfMemoryExceptionCallback();

            if (_ValidateRead(pException)
              && _ValidateWrite(pCatchBuffer)) {
                *pCatchBuffer = pException;
                *pCatchBuffer = __AdjustPointer(*pCatchBuffer,
                  CT_THISDISP(*pConv));
            } else {
                terminate(); // Does not return; TKB
            }
		}
        else if (HT_ISREFERENCE(*pCatch)) {

            // The catch is of form 'reference to T'.  At the throw point we
            // treat both 'T' and 'reference to T' the same, i.e.
            // pExceptionObject is a (machine) pointer to T.  Adjust as
            // required.
            if (_ValidateRead(PER_PEXCEPTOBJ(pExcept))
              && _ValidateWrite(pCatchBuffer)) {
                *pCatchBuffer = PER_PEXCEPTOBJ(pExcept);
                *pCatchBuffer = __AdjustPointer(*pCatchBuffer,
                  CT_THISDISP(*pConv));
            } else {
                terminate(); // Does not return; TKB
            }
        } else if (CT_ISSIMPLETYPE(*pConv)) {

            // Object thrown is of simple type (this including pointers) copy
            // specified number of bytes.  Adjust the pointer as required.  If
            // the thing is not a pointer, then this should be safe since all
            // the entries in the THISDISP are 0.
            if (_ValidateRead(PER_PEXCEPTOBJ(pExcept))
              && _ValidateWrite(pCatchBuffer)) {
                memmove(pCatchBuffer, PER_PEXCEPTOBJ(pExcept), CT_SIZE(*pConv));

                if (CT_SIZE(*pConv) == sizeof(void*) && *pCatchBuffer != nullptr) {
                    *pCatchBuffer = __AdjustPointer(*pCatchBuffer,
                      CT_THISDISP(*pConv));
                }
            } else {
                terminate(); // Does not return; TKB
            }
        } else {

            // Object thrown is UDT.
            if (CT_COPYFUNC(*pConv) == nullptr) {

                // The UDT had a simple ctor.  Adjust in the thrown object,
                // then copy n bytes.
                if (_ValidateRead(PER_PEXCEPTOBJ(pExcept))
                  && _ValidateWrite(pCatchBuffer)) {
                    memmove(pCatchBuffer, __AdjustPointer(PER_PEXCEPTOBJ(pExcept),
                      CT_THISDISP(*pConv)), CT_SIZE(*pConv));
                } else {
                    terminate(); // Does not return; TKB
                }
            } else {

                // It's a UDT: make a copy using copy ctor

#pragma warning(disable:4191)

                if (_ValidateRead(PER_PEXCEPTOBJ(pExcept))
                  && _ValidateWrite(pCatchBuffer)
                  && _ValidateExecute((FARPROC)CT_COPYFUNC(*pConv))) {

#pragma warning(default:4191)

                    if (CT_HASVB(*pConv)) {
                        retval = 2;
                    } else {
                        retval = 1;
                    }
                } else {
                    terminate(); // Does not return; TKB
                }
            }
        }
    } __except(EHTRACE_EXCEPT(EXCEPTION_EXECUTE_HANDLER)) {
        // Something went wrong when building the catch object.
        terminate();
    }

    EHTRACE_EXIT;
    return retval;
}
////////////////////////////////////////////////////////////////////////////////
//
// __BuildCatchObject - Copy or construct the catch object from the object thrown.
//
// Returns:
//     nothing.
//
// Side-effects:
//     A buffer in the subject function's frame is initialized.
//
// Open issues:
//     What happens if the constructor throws?  (or faults?)

extern "C" _VCRTIMP void __BuildCatchObject(
    EHExceptionRecord *pExcept,         // Original exception thrown
    void *pRN,                          // This is a pointer to the object
                                        // that we want to build while doing
                                        // COM+ eh. If we are in our own eh,
                                        // then this is a Registration node of
                                        // catching function
    HandlerType *pCatch,                // The catch clause that got it
    CatchableType *pConv                // The conversion to use
) {
    EHTRACE_ENTER;

    void **pCatchBuffer;
    if ( HT_ISCOMPLUSEH(*pCatch))
    {
        pCatchBuffer = (void **)pRN;
    }
    else
    {
#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        pCatchBuffer = (void **)__OffsetToAddress(
                                HT_DISPCATCH(*pCatch),
                                *((EHRegistrationNode *)pRN),
                                HT_FRAMENEST(*pCatch)
                                );
#else
        pCatchBuffer = (void **)__OffsetToAddress(
                                HT_DISPCATCH(*pCatch),
                                (EHRegistrationNode *)pRN,
                                HT_FRAMENEST(*pCatch)
                                );
#endif
    }

    __try {
        switch(__BuildCatchObjectHelper(pExcept, pRN, pCatch, pConv))
        {
            case 1:
                _CallMemberFunction1((char *)pCatchBuffer,
                  CT_COPYFUNC(*pConv),
                  __AdjustPointer(PER_PEXCEPTOBJ(pExcept),
                  CT_THISDISP(*pConv)));
                break;
            case 2:
                _CallMemberFunction2((char *)pCatchBuffer,
                  CT_COPYFUNC(*pConv),
                  __AdjustPointer(PER_PEXCEPTOBJ(pExcept),
                  CT_THISDISP(*pConv)), 1);
                break;
            case 0:
                break;
            default:
                break;
        }
    } __except(EHTRACE_EXCEPT(EXCEPTION_EXECUTE_HANDLER)) {
        // Something went wrong when building the catch object.
        terminate();
    }

    EHTRACE_EXIT;
}


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

#if _EH_RELATIVE_OFFSETS 
                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                    THROW_UNWINDFUNC_IB(*PER_PTHROW(pExcept),(ptrdiff_t)PER_PTHROWIB(pExcept)));
#else
                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                    THROW_UNWINDFUNC(*PER_PTHROW(pExcept)));
#endif
                __ResetException(pExcept);

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
           + (unsigned __int64)pmd.vdisp);
#else
           + pmd.vdisp);
#endif
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
// __uncaught_exception() - Returns true after completing of a throw-expression
//                          untils completing initialization of the
//                          exception-declaration in the matching handler.
//
// __uncaught_exceptions() - Returns the number of exceptions that have been
//                           thrown but not yet caught.
//

#if 0
extern "C" bool __cdecl __uncaught_exception()
{
    return __ProcessingThrow != 0;
}
#endif

extern "C" int __cdecl __uncaught_exceptions()
{
    return __ProcessingThrow;
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
#if _EH_RELATIVE_OFFSETS
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
#if _EH_RELATIVE_OFFSETS
        ptrdiff_t ImageBaseBak = _GetImageBase();
        _SetThrowImageBase((ptrdiff_t)pExcept->params.pThrowImageBase);
        __try       //Try finally for restoring _ImageBase;
        {
            void *CatchImageBase;
            // Set the image base using Rtl API.
            CatchImageBase = RtlPcToFileHeader((PVOID)pType, &CatchImageBase);
            _SetImageBase((ptrdiff_t)CatchImageBase);

            pCatch.dispType = (__int32)(
                    (ptrdiff_t)pType - (ptrdiff_t)CatchImageBase);
#else
            pCatch.pType = (TypeDescriptor *)pType;
#endif
            pCatch.adjectives = adjectives;
            SET_HT_ISCOMPLUSEH(pCatch);

            // Scan all types that thrown object can be converted to:
            ppCatchable = THROW_CTLIST(*PER_PTHROW(pExcept));
            for (catchables = THROW_COUNT(*PER_PTHROW(pExcept));
              catchables > 0; catchables--, ppCatchable++) {

#if _EH_RELATIVE_OFFSETS
                pCatchable = (CatchableType *)(_GetThrowImageBase() + *ppCatchable);
#else
                pCatchable = *ppCatchable;
#endif

                if (__TypeMatch(&pCatch, pCatchable, PER_PTHROW(pExcept))) {
                    // SucessFull. Now build the object.
                    __ProcessingThrow++;
                    if (pBuildObj != nullptr)
                        __BuildCatchObject(pExcept, pBuildObj, &pCatch, pCatchable);
                    return EXCEPTION_EXECUTE_HANDLER;
                }
            } // Scan posible conversions
#if _EH_RELATIVE_OFFSETS
        } __finally {
            _SetImageBase(ImageBaseBak);
        }
#endif
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

////////////////////////////////////////////////////////////////////////////////
//
// __CxxRgisterExceptionObject() - Registers Exception Object and saves it to
//                                 This is same as first part of
//                                 CallCatchBlock.
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
    if (ppExcept  != nullptr && (*(void **)ppExcept) != nullptr) {
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

//////////////////////////////////////////////////////////////////////////////////
// IsInExceptionSpec - Checks if the exception matches the exception specification
//                     list. Returns TRUE if it does, otherwise FALSE
//
BOOLEAN IsInExceptionSpec(
    EHExceptionRecord *pExcept,         // Information for this (logical)
                                        //   exception
    ESTypeList *pESTypeList             // Static information for subject frame
)
{
    DASSERT( pESTypeList != nullptr );
    if(!pESTypeList)
    {
        // should never happen
        terminate();
    }

    BOOLEAN bFoundMatchingTypeInES = FALSE;

#if _EH_RELATIVE_OFFSETS
    __int32 const *ppCatchable;
#elif defined(_WIN64)
    CatchableType * UNALIGNED const *ppCatchable;
#else
    CatchableType * const *ppCatchable;
#endif
    CatchableType *pCatchable;
    int catchables;

    // for every type in the exception spec...
    for (int i=0; i<EST_COUNT(pESTypeList); i++ )
    {
        // for all types that thrown object can be converted to...
        ppCatchable = THROW_CTLIST(*PER_PTHROW(pExcept));
        for (catchables = THROW_COUNT(*PER_PTHROW(pExcept));
            catchables > 0; catchables--, ppCatchable++) {

#if _EH_RELATIVE_OFFSETS
            pCatchable = (CatchableType *)(_GetThrowImageBase() + *ppCatchable);
#else
            pCatchable = *ppCatchable;
#endif
            if (__TypeMatch(EST_ARRAY(pESTypeList,i), pCatchable, PER_PTHROW(pExcept)))
            {
                bFoundMatchingTypeInES = TRUE;
                break;
            }
        }
    }

    return bFoundMatchingTypeInES;
}

//////////////////////////////////////////////////////////////////////////////////
// CallUnexpected - Calls unexpected and handles all exceptions
//                  thrown by it
//
// The unexpected() function shall not return, but it can throw (or re-throw) an
// exception.
//
//  If it throws a new exception which is allowed by the exception
//  specification which previously was violated, then
//  {
//      the search for another handler will continue at the call of the function
//      whose exception specification was violated.
//  }
//  else /* If it throws or rethrows an exception that the exception-specification does not
//      allow */ then the following happens:
//  {
//      If the exception-specification does not include the class std::bad_exception
//      (lib.bad.exception) then
//      {
//          the function terminate() is called,
//      }
//      otherwise
//      {
//          the thrown exception is replaced by an implementation-defined
//          object of the type std::bad_exception and the search for another handler
//          will continue at the call of the function whose exception-specification
//          was violated.
//      }
//  }
//
// Simple, isn't it?
//
#if 0
void CallUnexpected( ESTypeList* pESTypeList )
{
    DASSERT( _pCurrentFuncInfo == nullptr );

    try
    {
        unexpected();
    }
    catch(...)
    {
        _pCurrentFuncInfo = pESTypeList;

        throw; // rethrow -- we will catch it in the FrameHandler
    }
    terminate();
}
#endif

//////////////////////////////////////////////////////////////////////////////////
// Is_bad_exception_allowed - checks if std::bad_exception belongs to the list
//
BOOLEAN Is_bad_exception_allowed(ESTypeList *pESTypeList)
{
    for (int i=0; i<EST_COUNT(pESTypeList); i++ )
    {
        if( *(type_info*)HT_PTD(*EST_ARRAY(pESTypeList,i)) == typeid( std::bad_exception ) )
        {
            return TRUE;
        }
    }

    return FALSE;
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

#if _EH_RELATIVE_OFFSETS
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
#if _EH_RELATIVE_OFFSETS
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

