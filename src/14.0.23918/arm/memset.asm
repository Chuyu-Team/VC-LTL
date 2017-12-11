;***
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
;
; Use of this source code is subject to the terms of your Microsoft Windows CE
; Source Alliance Program license form.  If you did not accept the terms of
; such a license, you are not authorized to use this source code.
;
;
; void *
; memset( void *dest, int c, size_t count );
;
;       The memset function sets the first count bytes of
;       dest to the character c (r1).
;**********

#include "ksarm.h"

        TEXTAREA

        LEAF_ENTRY memset

        subs    r2, r2, #4
        mov     r3, r0                                  ; Save r0 for return
        blt     ByteSet

        ands    r1, r1, #&FF
        orr     r1, r1, r1, lsl #8
CheckAlign                                              ; 2-3 cycles
        ands    r12, r3, #3                             ; Check alignment and fix if possible
        bne     Align

BlockSet                                                ; 6-7 cycles
        orr     r1, r1, r1, lsl #16
        subs    r2, r2, #12
        mov     r12, r1
        blt     BlkSet8

BlkSet16                                                ; 7 cycles/16 bytes
        stmia   r3!, {r1, r12}
        subs    r2, r2, #16
        stmia   r3!, {r1, r12}
        bge     BlkSet16

BlkSet8                                                 ; 4 cycles/8 bytes
        adds    r2, r2, #8
        blt     BlkSet4
        stmia   r3!, {r1, r12}
        subs    r2, r2, #8

BlkSet4
        adds    r2, r2, #4                              ; 4 cycles/4 bytes
        blt     ByteSet
        str     r1, [r3], #4

ByteSet
        bge     MaybeExit
        adds    r2, r2, #4
MaybeExit
        beq     Exit

        strb    r1, [r3]                                ; 5 cycles/1-3bytes
        cmp     r2, #2
        blt     Exit
        strb    r1, [r3, #1]
        strbgt  r1, [r3, #2]

Exit

        bx      lr

Align                                                   ; 8 cycles/1-3 bytes
        tst     r3, #1                                  ; Check byte alignment
        beq     AlignHalf
        subs    r2, r2, #1
        strb    r1, [r3], #1
AlignHalf
        tst     r3, #2                                  ; Check Half-word alignment
        beq     BlockSet
        subs    r2, r2, #2
        strh    r1, [r3], #2
        b       BlockSet

        LEAF_END memset
        END
