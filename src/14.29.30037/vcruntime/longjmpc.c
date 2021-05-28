/*++

Copyright (c) Microsoft Corporation

Module Name:

    longjmpc.c

Abstract:

    This module implements C support code for long jump.

Environment:

    Kernel mode only.

--*/

#ifndef _M_IX86
#define longjmp __longjmp_crt
#endif

#include <vcruntime_internal.h>
#include <setjmp.h>
#include <stdlib.h>

#undef longjmp

int
__longjmp_internal (
    _In_reads_(_JBLEN) jmp_buf JumpBuffer,
    _In_ int ReturnValue
    );

#if defined(NTOS_KERNEL_RUNTIME) && defined(_AMD64_)

VOID
KeCheckStackAndTargetAddress (
    _In_ LONG64 Rip,
    _In_ ULONG64 Rsp
    );

#else

void
__except_validate_jump_buffer (
    _In_reads_(_JBLEN) jmp_buf JumpBuffer
    );

#endif

DECLSPEC_GUARD_SUPPRESS
void
longjmp (
    _In_reads_(_JBLEN) jmp_buf JumpBuffer,
    _In_ int ReturnValue
    )

/*++

Routine Description:

    This function validates a jump buffer and performs a long jump to the
    context specified by the jump buffer.

Arguments:

    JumpBuffer - Supplies the address of a jump buffer.

    ReturnValue - Supplies the value that is to be returned to the caller of
        set jump.

Return Value:

    None (however the return value is loaded).

--*/

{

    //
    // Validate the contents of the jump buffer and execute long jump.
    //

#if defined(NTOS_KERNEL_RUNTIME) && defined(_AMD64_)

    _JUMP_BUFFER *jmpBuf;

    jmpBuf = (_JUMP_BUFFER *)JumpBuffer;
    KeCheckStackAndTargetAddress(jmpBuf->Rip, jmpBuf->Rsp);

#else

    __except_validate_jump_buffer(JumpBuffer);

#endif

    __longjmp_internal(JumpBuffer, ReturnValue);
}
