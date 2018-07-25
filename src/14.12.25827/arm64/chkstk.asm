        TTL   "Runtime Stack Checking"
;++
;
; Copyright (c) Microsoft Corporation
;
; Module Name:
;
;   chkstk.s
;
; Abstract:
;
;   This module implements runtime stack checking.
;
; Author:
;
;   David N. Cutler (davec) 20-Oct-2000
;
; Environment:
;
;   Any mode.
;
;--

#include "ksarm64.h"

#if !defined(TEBREG)

#define TEBREG x18

#endif

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
;        argument is in register x15, and is divided by 16.
;
;        The typical calling sequence from the prologue is:
;
;        mov    x15, #immed                ; set requested stack frame size
;        bl     __chkstk                  ; verify stack
;        sub    sp, sp, x15, lsl #4        ; apply stack adjust
;
; Arguments:
;
;   None.
;
; Implicit Arguments:
;
;   Allocation (x15) - Supplies the size of the allocation on the stack, divided by 16.
;
; Return Value:
;
;   None.
;
;--

#ifdef NTOS_KERNEL_RUNTIME

        LEAF_ENTRY __chkstk //, _TEXT$00

        ret                                             ; do nothing

        LEAF_END __chkstk //, _TEXT$00

#else

        LEAF_ENTRY __chkstk //, _TEXT$00
        ALTERNATE_ENTRY _alloca_probe

;
; Compute the new stack address.
;
; N.B. If the computation causes a borrow, i.e., the requested size is very
;      large, then the computed address is forced to zero
;

        ldr     x17, [TEBREG, #TeStackLimit]            ; get current stack limit
        subs    x16, sp, x15, uxtx #4                   ; calculate new stack address
        csello  x16, xzr, x16                           ; clamp to 0

;
; If the new stack address is greater than the current stack limit, then the
; pages have already been allocated and nothing further needs to be done.
;

        cmp     x16, x17                                ; check if stack within limits
        blo     %F3                                     ; if stack outside of limits, probe it
        ret                                             ; otherwise, just return
3
;
; The new stack address is not within the currently allocated stack. Probe
; pages downward from the current stack limit until all pages have been
; allocated or a stack overflow occurs in which case an exception
; will be raised.
;

        and     x16, x16, #0xfffffffffffff000           ; get target stack value, page-aligned
1
        sub     x17, x17, #PAGE_SIZE/4096, lsl #12      ; get next lower page address
        ldr     xzr, [x17]                              ; probe stack address
        cmp     x17, x16                                ; check if end of probe range
        bne     %B1                                     ; if ne, not end of probe range
        ret                                             ; return

        LEAF_END

#endif

        END
