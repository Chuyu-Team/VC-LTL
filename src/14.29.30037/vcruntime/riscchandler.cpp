/*++

Copyright (c) Microsoft Corporation

Module Name:

    riscchandler.cpp

Abstract:

    This module implements the C specific exception handler that provides
    structured condition handling for the C language on RISC and AMD64
    platforms.

--*/

#include <vcruntime_internal.h>
#include <winternl.h>
#include <vcwininternls.h>

#ifndef DISABLE_NLG

extern "C" VOID
_NLG_Notify (
    IN PVOID uoffDestination,
    IN PVOID uoffFramePointer,
    IN ULONG dwCode
    );

extern "C" VOID
__NLG_Return2 (
    VOID
    );

#else

#define _NLG_Notify(...)
#define __NLG_Return2(...)

#endif

#define EH_EXCEPTION_NUMBER ('msc' | 0xE0000000)

//
// If frame.obj is pulled in then _pDestructExceptionObject points to
// __DestructExceptionObject, otherwise, it's NULL.
//

extern "C" void
(__cdecl *_pDestructExceptionObject) (
    void *,
    int
    );

#if defined(_M_ARM64EC)

extern "C" void
__except_validate_context_record (
    _In_ PCONTEXT ContextRecord
    );

//
// The following two wrapper functions are used to call the exception filter
// and termination handlers. They preserve all non-volatile registers, and set
// up the frame pointer as appropriate when invoking the filters and termination
// handlers.
//

extern "C" LONG
__C_ExecuteExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ PVOID EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PUCHAR NonVolatileRegisters
    );

extern "C" VOID
__C_ExecuteTerminationHandler(
    _In_ BOOLEAN AbnormalTermination,
    _In_ PVOID EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _In_ PUCHAR NonVolatileRegisters
    );

#define DC_TARGETPC(DispatcherContext) ((DispatcherContext)->TargetIp)

__forceinline
LONG 
EXECUTE_EXCEPTION_FILTER(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ PVOID EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PDISPATCHER_CONTEXT DispatcherContext
    )
{
    LONG Disposition;
    PDISPATCHER_CONTEXT_ARM64EC DispatcherContextEc;

    if (RtlIsEcCode((DispatcherContext->ControlPc))) {
        DispatcherContextEc = (PDISPATCHER_CONTEXT_ARM64EC)DispatcherContext;
        Disposition =
            __C_ExecuteExceptionFilter(ExceptionPointers,
                                    EstablisherFrame,
                                    ExceptionFilter,
                                    DispatcherContextEc->NonVolatileRegisters);

    } else {
        Disposition = (ExceptionFilter)(ExceptionPointers, EstablisherFrame);
    }

    return Disposition;
}

__forceinline
VOID
EXECUTE_TERMINATION_HANDLER(
    _In_ BOOLEAN AbnormalTermination,
    _In_ PVOID EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _Inout_ PDISPATCHER_CONTEXT DispatcherContext
    )
{
    PDISPATCHER_CONTEXT_ARM64EC DispatcherContextEc;

    if (RtlIsEcCode((DispatcherContext->ControlPc))) {
        DispatcherContextEc = (PDISPATCHER_CONTEXT_ARM64EC)DispatcherContext;
        __C_ExecuteTerminationHandler(AbnormalTermination,
                                    EstablisherFrame,
                                    TerminationHandler,
                                    DispatcherContextEc->NonVolatileRegisters);

    } else {
        (TerminationHandler)(AbnormalTermination, EstablisherFrame);
    }

    return;
}

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

//
// The following two wrapper functions are used to call the exception filter
// and termination handlers. They preserve all non-volatile registers, and set
// up the frame pointer as appropriate when invoking the filters and termination
// handlers.
//

extern "C" LONG
__C_ExecuteExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ PVOID EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PUCHAR NonVolatileRegisters
    );

extern "C" VOID
__C_ExecuteTerminationHandler(
    _In_ BOOLEAN AbnormalTermination,
    _In_ PVOID EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _In_ PUCHAR NonVolatileRegisters
    );

#define DC_TARGETPC(DispatcherContext) ((DispatcherContext)->TargetPc)

#define EXECUTE_EXCEPTION_FILTER(ExceptionPointers, \
                                 EstablisherFrame, \
                                 ExceptionFilter, \
                                 DispatcherContext) \
    __C_ExecuteExceptionFilter(ExceptionPointers, \
                               EstablisherFrame, \
                               ExceptionFilter, \
                               DispatcherContext->NonVolatileRegisters)

#define EXECUTE_TERMINATION_HANDLER(AbnormalTermination, \
                                    EstablisherFrame, \
                                    TerminationHandler, \
                                    DispatcherContext) \
    __C_ExecuteTerminationHandler(AbnormalTermination, \
                                  EstablisherFrame, \
                                  TerminationHandler, \
                                  DispatcherContext->NonVolatileRegisters)

#elif defined(_M_X64) || defined(_M_ARM64EC)

extern "C" void
__except_validate_context_record (
    _In_ PCONTEXT ContextRecord
    );

#define EXECUTE_EXCEPTION_FILTER(ExceptionPointers, \
                                 EstablisherFrame, \
                                 ExceptionFilter, \
                                 DispatcherContext) \
    (ExceptionFilter)(ExceptionPointers, EstablisherFrame)

#define EXECUTE_TERMINATION_HANDLER(AbnormalTermination, \
                                    EstablisherFrame, \
                                    TerminationHandler, \
                                    DispatcherContext) \
    (TerminationHandler)(AbnormalTermination, EstablisherFrame)

#define DC_TARGETPC(DispatcherContext) ((DispatcherContext)->TargetIp)

#else

#error Unknown processor architecture.

#endif

extern "C"
#if defined(_M_X64)

__declspec(guard(ignore))

#endif

DECLSPEC_GUARD_SUPPRESS
EXCEPTION_DISPOSITION
__C_specific_handler (
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PVOID EstablisherFrame,
    _Inout_ PCONTEXT ContextRecord,
    _Inout_ PDISPATCHER_CONTEXT DispatcherContext
    )

/*++

Routine Description:

    This function scans the scope tables associated with the specified
    procedure and calls exception and termination handlers as necessary.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    EstablisherFrame - Supplies a pointer to frame of the establisher function.

    ContextRecord - Supplies a pointer to a context record.

    DispatcherContext - Supplies a pointer to the exception dispatcher or
        unwind dispatcher context.

    N.B. SHRINK WRAPPING MUST BE DISABLED FOR THIS FUNCTION.

Return Value:

    If an exception is being dispatched and the exception is handled by one
    of the exception filter routines, then there is no return from this
    routine and RtlUnwind is called. Otherwise, an exception disposition
    value of continue execution or continue search is returned.

    If an unwind is being dispatched, then each termination handler is called
    and a value of continue search is returned.

--*/

{

    ULONG_PTR ControlPc;
    PEXCEPTION_FILTER ExceptionFilter;
    EXCEPTION_POINTERS ExceptionPointers;
    ULONG_PTR ImageBase;
    ULONG_PTR Handler;
    ULONG Index;
    PSCOPE_TABLE ScopeTable;
    ULONG TargetIndex;
    ULONG_PTR TargetPc;
    PTERMINATION_HANDLER TerminationHandler;
    LONG Value;

    DISABLE_SHRINK_WRAPPING();

#if defined(_M_X64)

    //
    // Validate integrity of context record.
    //
    __except_validate_context_record(ContextRecord);

#endif

    //
    // Get the image base address. compute the relative address of where
    // control left the establisher, and get the address of the scope table.
    //

    ImageBase = DispatcherContext->ImageBase;
    ControlPc = DispatcherContext->ControlPc - ImageBase;
    ScopeTable = (PSCOPE_TABLE)(DispatcherContext->HandlerData);

#if defined(_M_ARM64EC)

    if (RtlIsEcCode((DispatcherContext->ControlPc))) {
        PDISPATCHER_CONTEXT_ARM64EC DispatcherContextEc;
        DispatcherContextEc = (PDISPATCHER_CONTEXT_ARM64EC)DispatcherContext;

        if ((ScopeTable->Count & (1 << 31)) != 0) {
            ScopeTable = (PSCOPE_TABLE)(ImageBase + (ScopeTable->Count & ~(1 << 31)));
        }

        if (DispatcherContextEc->ControlPcIsUnwound != FALSE) {
            ControlPc -= 4;
        }
    }

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

    //
    // Do we have an indirect scope table?
    //

    if ((ScopeTable->Count & (1 << 31)) != 0) {
        ScopeTable = (PSCOPE_TABLE)(ImageBase + (ScopeTable->Count & ~(1 << 31)));
    }

    //
    // If this context came from an unwind to a call, then the ControlPc points
    // to a return address, which could put us at the start of a neighboring
    // scope. To correct for this, back the PC up by the minimum instruction
    // size to ensure we are in the same scope as the original branch opcode.
    //

    if (DispatcherContext->ControlPcIsUnwound != FALSE) {

#if defined(_M_ARM_NT)

        ControlPc -= 2;

#else

        ControlPc -= 4;

#endif

    }

#endif // defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

    //
    // If an unwind is not in progress, then scan the scope table and call
    // the appropriate exception filter routines. Otherwise, scan the scope
    // table and call the appropriate termination handlers using the target
    // PC obtained from the dispatcher context.
    //

    if (IS_DISPATCHING(ExceptionRecord->ExceptionFlags)) {

        //
        // Scan the scope table and call the appropriate exception filter
        // routines.
        //

        ExceptionPointers.ExceptionRecord = ExceptionRecord;
        ExceptionPointers.ContextRecord = ContextRecord;
        for (Index = DispatcherContext->ScopeIndex; Index < ScopeTable->Count; Index += 1) {
            if ((ControlPc >= ScopeTable->ScopeRecord[Index].BeginAddress) &&
                (ControlPc < ScopeTable->ScopeRecord[Index].EndAddress) &&
                (ScopeTable->ScopeRecord[Index].JumpTarget != 0)) {

                //
                // If the filter function address is the distinguished value
                // one, then set the disposition value to execute handler.
                // Otherwise, call the exception filter function to get the
                // disposition value.
                //

                if (ScopeTable->ScopeRecord[Index].HandlerAddress == 1) {
                    Value = EXCEPTION_EXECUTE_HANDLER;

                } else {
                    ExceptionFilter =
                        (PEXCEPTION_FILTER)(ScopeTable->ScopeRecord[Index].HandlerAddress + ImageBase);

                    Value = EXECUTE_EXCEPTION_FILTER(&ExceptionPointers,
                                                     EstablisherFrame,
                                                     ExceptionFilter,
                                                     DispatcherContext);
                }

                //
                // If the return value is less than zero, then dismiss the
                // exception. Otherwise, if the value is greater than zero,
                // then unwind to the target exception handler. Otherwise,
                // continue the search for an exception filter.
                //

                if (Value < 0) {
                    return ExceptionContinueExecution;

                } else if (Value > 0) {

                    //
                    // If a thrown C++ exception is being handled, then let
                    // the C++ exception handler destruct the thrown object.
                    //

#ifndef _NTSUBSET_

                    if ((ExceptionRecord->ExceptionCode == EH_EXCEPTION_NUMBER) &&
                        (_pDestructExceptionObject != NULL) &&
                        (_IsNonwritableInCurrentImage(&_pDestructExceptionObject))) {

                        (*_pDestructExceptionObject)(ExceptionRecord, TRUE);
                    }

#endif

                    //
                    // Inform the debugger that control is about to be passed
                    // to an exception handler and pass the handler's address
                    // to NLG_Notify.
                    //

                    Handler = ImageBase + ScopeTable->ScopeRecord[Index].JumpTarget;
                    _NLG_Notify((PVOID)Handler, EstablisherFrame, 0x1);
                    RtlUnwindEx(EstablisherFrame,
                                (PVOID)(ScopeTable->ScopeRecord[Index].JumpTarget + ImageBase),
                                ExceptionRecord,
                                (PVOID)((ULONG_PTR)ExceptionRecord->ExceptionCode),
                                (PCONTEXT)DispatcherContext->ContextRecord,
                                DispatcherContext->HistoryTable);


                    //
                    // Notify debugger : return from exception handler
                    //

                    __NLG_Return2();
                }
            }
        }

    } else {

        //
        // Scan the scope table and call the appropriate termination handler
        // routines.
        //

        TargetPc = DC_TARGETPC(DispatcherContext) - ImageBase;
        for (Index = DispatcherContext->ScopeIndex; Index < ScopeTable->Count; Index += 1) {
            if ((ControlPc >= ScopeTable->ScopeRecord[Index].BeginAddress) &&
                (ControlPc < ScopeTable->ScopeRecord[Index].EndAddress)) {

                if (IS_TARGET_UNWIND(ExceptionRecord->ExceptionFlags)) {

                    //
                    // If the target PC is within the same scope as the control PC,
                    // then this is an uplevel goto out of an inner try scope or a
                    // long jump back into a try scope. Terminate the scan for a
                    // termination handler.
                    //
                    // N.B.  Due to a bug in the AMD64 compiler, try scopes may
                    //       be split into multiple regions, requiring a scan
                    //       of the earlier region of the table to verify that
                    //       the target PC is not within the same try scope.
                    //       The split scopes could reside both before or after
                    //       the currently searched scope in the scope table.
                    //
                    // N.B.  The target PC can be just beyond the end of the
                    //       scope in which case it is a leave from a scope.
                    //       The "leave from a scope" case is subsequently
                    //       handled before control is transferred.
                    //

                    for (TargetIndex = 0; TargetIndex < ScopeTable->Count; TargetIndex += 1) {
                        if ((TargetPc >= ScopeTable->ScopeRecord[TargetIndex].BeginAddress) &&
                            (TargetPc < ScopeTable->ScopeRecord[TargetIndex].EndAddress) &&
                            (ScopeTable->ScopeRecord[TargetIndex].JumpTarget == ScopeTable->ScopeRecord[Index].JumpTarget) &&
                            (ScopeTable->ScopeRecord[TargetIndex].HandlerAddress == ScopeTable->ScopeRecord[Index].HandlerAddress)) {

                            break;
                        }
                    }

                    if (TargetIndex != ScopeTable->Count) {
                        break;
                    }
                }

                //
                // If the scope table entry describes an exception filter
                // and the associated exception handler is the target of
                // the unwind, then terminate the scan for termination
                // handlers. Otherwise, if the scope table entry describes
                // a termination handler, then record the address of the
                // end of the scope as the new control PC address and call
                // the termination handler.
                //

                if (ScopeTable->ScopeRecord[Index].JumpTarget != 0) {
                    if ((TargetPc == ScopeTable->ScopeRecord[Index].JumpTarget) &&
                        (IS_TARGET_UNWIND(ExceptionRecord->ExceptionFlags))) {

                        break;
                    }

                } else {
                    DispatcherContext->ScopeIndex = Index + 1;
                    TerminationHandler =
                        (PTERMINATION_HANDLER)(ScopeTable->ScopeRecord[Index].HandlerAddress + ImageBase);

                    EXECUTE_TERMINATION_HANDLER(TRUE,
                                                EstablisherFrame,
                                                TerminationHandler,
                                                DispatcherContext);
                }
            }
        }
    }

    //
    // Continue search for exception or termination handlers.
    //

    ENABLE_SHRINK_WRAPPING();

    return ExceptionContinueSearch;
}
