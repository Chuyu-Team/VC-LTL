/*++

Copyright (c) Microsoft Corporation

Module Name:

    chandler.c

Abstract:

    This module implements the C specific exception handler that provides
    structured condition handling for the C language.

--*/
#include <vcruntime_internal.h>
#include <winternl.h>
#include <vcwininternls.h>

VOID
_NLG_Notify (
    IN PVOID uoffDestination,
    IN PVOID uoffFramePointer,
    IN ULONG dwCode
    );

VOID
__NLG_Return2 (
    VOID
    );

#define EH_EXCEPTION_NUMBER ('msc' | 0xE0000000)

//
// If frame.obj is pulled in then _pDestructExceptionObject points to
// __DestructExceptionObject, otherwise, it's NULL.
//

void
(__cdecl *_pDestructExceptionObject) (
    void *,
    int
    );

//
// The following two wrapper functions are used to call the exception filter
// and termination handlers on ARM. They preserve all non-volatile registers,
// and ensure that the establisher frame is located in r7 before calling the
// provided function. This allows the compiler to generate less setup code.
//

LONG
__C_ExecuteExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ ULONG EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PUCHAR NonVolatileRegisters
    );

VOID
__C_ExecuteTerminationHandler(
    _In_ BOOLEAN AbnormalTermination,
    _In_ ULONG EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _In_ PUCHAR NonVolatileRegisters
    );

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

    ULONG ControlPc;
    PEXCEPTION_FILTER ExceptionFilter;
    EXCEPTION_POINTERS ExceptionPointers;
    ULONG ImageBase;
    ULONG Handler;
    ULONG Index;
    ULONG TargetIndex;
    PSCOPE_TABLE ScopeTable;
    ULONG TargetPc;
    PTERMINATION_HANDLER TerminationHandler;
    LONG Value;

    DISABLE_SHRINK_WRAPPING();

    //
    // Get the image base address. compute the relative address of where
    // control left the establisher, and get the address of the scope table.
    //

    ImageBase =  DispatcherContext->ImageBase;
    ControlPc = DispatcherContext->ControlPc - ImageBase;

    ScopeTable = (PSCOPE_TABLE)(DispatcherContext->HandlerData);

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
    // size to ensure we are in the same scope as the original BL/BLX opcode.
    //

    if (DispatcherContext->ControlPcIsUnwound) {
        ControlPc -= 2;
    }

    //
    // If an unwind is not in progress, then scan the scope table and call
    // the appropriate exception filter routines. Otherwise, scan the scope
    // table and call the appropriate termination handlers using the target
    // PC obtained from the dispatcher context.
    // are called.
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

                    Value = __C_ExecuteExceptionFilter(&ExceptionPointers,
                                                       (ULONG)EstablisherFrame,
                                                       ExceptionFilter,
                                                       DispatcherContext->NonVolatileRegisters);
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
                                (PVOID)((ULONG)ExceptionRecord->ExceptionCode),
                                DispatcherContext->ContextRecord,
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

        TargetPc = DispatcherContext->TargetPc - ImageBase;

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
                    if (TargetPc == ScopeTable->ScopeRecord[Index].JumpTarget
                        // Terminate only when we are at the Target frame;
                        // otherwise, continue search for outer finally:
                        && IS_TARGET_UNWIND(ExceptionRecord->ExceptionFlags)
                        ) {
                        break;
                    }
                } else {
                    DispatcherContext->ScopeIndex = Index + 1;
                    TerminationHandler =
                        (PTERMINATION_HANDLER)(ScopeTable->ScopeRecord[Index].HandlerAddress + ImageBase);

                    __C_ExecuteTerminationHandler(TRUE,
                                                  (ULONG)EstablisherFrame,
                                                  TerminationHandler,
                                                  DispatcherContext->NonVolatileRegisters);
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
