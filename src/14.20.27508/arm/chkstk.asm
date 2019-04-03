        TTL   "Runtime Stack Checking"
;++
;
; Copyright (c) Microsoft Corporation
;
; Module Name:
;
;   chkstk.asm
;
; Abstract:
;
;   This module implements runtime stack checking.
;
;--

#include "ksarm.h"

        TEXTAREA

        SUBT  "Check Stack"
;++
;
; ULONG
; __chkstk (
;     VOID
;     )
;
; Routine Description:
;
;   This function provides runtime stack checking for local allocations
;   that are more than a page and for storage dynamically allocated with
;   the alloca function. Stack checking consists of probing downward in
;   the stack a page at a time. If the current stack commitment is exceeded,
;   then the system will automatically attempts to expand the stack. If the
;   attempt succeeds, then another page is committed. Otherwise, a stack
;   overflow exception is raised. It is the responsibility of the caller to
;   handle this exception.
;
;   N.B. This routine is called using a non-standard calling sequence since
;        it is typically called from within the prologue. The allocation size
;        argument is in register r4, and is divided by 4. Upon return, r4 is
;        expected to contain the full allocation amount.
;
;        The typical calling sequence from the prologue is:
;
;        ldr    r4, [pc, #xxx]            ; set requested stack frame size
;        bl     _chkstk                   ; r4 is subtracted from sp in __chkstk
;
; Arguments:
;
;   None.
;
; Implicit Arguments:
;
;   Allocation (r4) - Supplies the size of the allocation on the stack, divided by 4.
;
; Return Value:
;
;   None.
;
;--

#ifdef NTOS_KERNEL_RUNTIME

        LEAF_ENTRY __chkstk //, _TEXT$00

        lsls    r4, r4, #2                              ; multiply adjustment by 4
        bx      lr

        LEAF_END __chkstk //, _TEXT$00

#else

        NESTED_ENTRY __chkstk //, _TEXT$00
        ALTERNATE_ENTRY _alloca_probe

;
; Compute the new stack address.
;
; N.B. If the computation causes a borrow, i.e., the requested size is very
;      large, then the computed address is forced to zero
;

        subs    r12, sp, r4, lsl #2                     ; calculate new stack address
        TEB_READ r4                                     ; get a pointer to the TEB
        ldr     r4, [r4, #TeStackLimit]                 ; get current stack limit
        blo     %F2                                     ; if wrapped through zero, fix it up out-of-band

;
; If the new stack address is greater than the current stack limit, then the
; pages have already been allocated and nothing further needs to be done.
;
1
        cmp     r12, r4                                 ; check if stack within limits
        blo     __chkstk_probe                          ; if stack outside of limits, probe it
        sub     r4, sp, r12                             ; re-derive stack adjustment
        EPILOG_RETURN                                   ; otherwise, just return
2
        movs    r12, #0                                 ; set r12 to 0
        b       %B1                                     ; resume

        NESTED_END __chkstk


        NESTED_ENTRY __chkstk_probe
        PROLOG_PUSH {r5, r6}                            ; save some scratch registers

;
; The new stack address is not within the currently allocated stack. Probe
; pages downward from the current stack limit until all pages have been
; allocated or a stack overflow occurs in which case an exception
; will be raised.
;

        mov     r6, r12                                 ; get target stack value
        bfc     r6, #0, #12                             ; clear out the low bits
1
        sub     r4, r4, #PAGE_SIZE                      ; get next lower page address
        ldr     r5, [r4]                                ; probe stack address
        cmp     r4, r6                                  ; check if end of probe range
        bne     %B1                                     ; if ne, not end of probe range

        EPILOG_POP  {r5, r6}                            ; restore scratch registers
        EPILOG_NOP  sub r4, sp, r12                     ; re-derive stack adjustment
        EPILOG_RETURN                                   ; return

        NESTED_END __chkstk_probe //, _TEXT$00

#endif

        END
