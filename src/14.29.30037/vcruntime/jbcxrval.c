/*++

Copyright (c) Microsoft Corporation

Module Name:

    jbcxrval.c

Abstract:

    This module implements C support code for jump buffer and context record
    validation.

Environment:

    Kernel mode only.

--*/

#include <vcruntime_internal.h>
#include <stdlib.h>

#if !(defined(NTOS_KERNEL_RUNTIME) && defined(_AMD64_))

#include "../misc/cfg_support.inc"  // To inline _guard_icall_checks_enforced()

#if defined(_AMD64_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Rsp)
#define JB_FRAME(jmpbuf) (((_JUMP_BUFFER*)jmpbuf)->Frame)

#elif defined(_ARM_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Sp)
#define JB_FRAME(jmpbuf) (((_JUMP_BUFFER*)jmpbuf)->Frame)

#elif defined(_ARM64_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Sp)
#define JB_FRAME(jmpbuf) (((_JUMP_BUFFER*)jmpbuf)->Frame)

#elif defined(_X86_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Esp)
#else
#error Unsupported architecture.
#endif

#define CONTEXT_TO_STACK_POINTER(Context) JUMP_BUFFER_TO_STACK_POINTER(Context)

void
__except_validate_context_record (
    _In_ PCONTEXT ContextRecord
    )

/*++

Routine Description:

    This function validates a context record for exception handling support.

Arguments:

    ContextRecord - Supplies a pointer to the context record to validate.

Return Value:

    None.  If the context record was not valid, a fast fail event is raised if
    CFG was enforced.

--*/

{
    PVOID StackPointer;
    PNT_TIB Tib;

    //
    // If guard ICall checks are enforced, then validate the stack extents of
    // the context record and raise a fast fail exception if the extents are
    // invalid.  If checks are not enforced or the jump buffer was valid, then
    // return.
    //
    if (_guard_icall_checks_enforced()) {
        Tib = (PNT_TIB)NtCurrentTeb();

        //
        // HYB-TODO: Validate both chpe and guest context.
        //

        StackPointer = (PVOID)CONTEXT_TO_STACK_POINTER(ContextRecord);
        if ((StackPointer < Tib->StackLimit) ||
            (StackPointer > Tib->StackBase)) {

            __fastfail(FAST_FAIL_INVALID_SET_OF_CONTEXT);
        }
    }
}

__forceinline
void
__except_validate_jump_buffer_common (
    _In_reads_(_JBLEN) jmp_buf JumpBuffer,
    _In_ PVOID (*ExceptGetJbSpRoutine)(jmp_buf)
    )

/*++

Routine Description:

    This function validates a jump buffer for exception handling support.

Arguments:

    JumpBuffer - Supplies a pointer to the jump buffer to validate.

Return Value:

    None.  If the jump buffer was not valid, a fast fail event is raised if
    CFG was enforced.

--*/

{

    PVOID StackPointer;
    PNT_TIB Tib;

    //
    // If guard ICall checks are enforced, then validate the stack extents of
    // the jump buffer and raise a fast fail exception if the extents are
    // invalid.  If checks are not enforced or the jump buffer was valid, then
    // return.
    //

    if (_guard_icall_checks_enforced()) {
        Tib = (PNT_TIB)NtCurrentTeb();
        StackPointer = ExceptGetJbSpRoutine(JumpBuffer);

        if ((StackPointer < Tib->StackLimit) ||
            (StackPointer > Tib->StackBase)) {

            __fastfail(FAST_FAIL_INVALID_SET_OF_CONTEXT);
        }

#if defined(JB_FRAME)
        if (JB_FRAME(JumpBuffer) == 0) {

            __fastfail(FAST_FAIL_INVALID_SET_OF_CONTEXT);
        }
#endif
    }
}

__forceinline
static PVOID __except_get_jumpbuf_sp(_In_reads_(_JBLEN) jmp_buf JumpBuffer)
{
    return (PVOID)JUMP_BUFFER_TO_STACK_POINTER((_JUMP_BUFFER*)JumpBuffer);
}

void
__except_validate_jump_buffer (
    _In_reads_(_JBLEN) jmp_buf JumpBuffer
    )
{
    __except_validate_jump_buffer_common(JumpBuffer, __except_get_jumpbuf_sp);
}

#endif
