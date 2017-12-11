;***
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
;
; Derived from CE
;
; int
; memcmp( const void *r0, const void *r1, size_t r2 );
;
;   The memcmp function compares the first r2 bytes of r0
;   and r1 and returns a value indicating their relationship.
;      < 0  if r0 less than r1
;     == 0  if r0 identical to r1
;      > 0  if r0 greater than r1
;**********

#include "ksarm.h"

        TEXTAREA

        LEAF_ENTRY memcmp

        cmp     r2, #4
        blt     DoBytes

        orr     r3, r0, r1                              ; 4-6 cycles
        tst     r3, #1
        bne     ByteCmp
        tst     r3, #2
        bne     HalfCmp

WordCmp                                                 ; 7 cycles/4 bytes
        subs    r2, r2, #4
        blt     Finish
        ldr     r3, [r0], #4
        ldr     r12, [r1], #4
        eors    r12, r12, r3
        beq     WordCmp

        movs    r3, #4                                 ; Some cycles
        tst     r12, #0xFF
        bne     WordByteCmp
        subs    r3, r3, #1
        tst     r12, #0xFF00
        bne     WordByteCmp
        subs    r3, r3, #1
        tst     r12, #0xFF0000
        bne     WordByteCmp
        subs    r3, r3, #1
WordByteCmp
        subs    r0, r0, r3
        subs    r1, r1, r3
        ldrb    r3, [r0]
        ldrb    r12, [r1]
        sub     r0, r3, r12
        bx      lr

HalfCmp                                                 ; 7 cycles/2 bytes
        subs    r2, r2, #2
        blt     Half_Finish
        ldrh    r3, [r0], #2
        ldrh    r12, [r1], #2
        subs    r3, r3, r12
        beq     HalfCmp

        tst     r3, #0xFF
        bne     HalfWordByteCmp
        movs    r0, r3, asr #8

        bx      lr

HalfWordByteCmp
        ldrb    r3, [r0, #-2]
        and     r12, r12, #0xFF
        sub     r0, r3, r12

        bx      lr

Finish
        adds    r2, r2, #2
Half_Finish
        adds    r2, r2, #2
DoBytes
        cbz     r2, ReturnZero

ByteCmp                                                 ; 7 cycles/byte
        ldrb    r3, [r0], #1                            ; Load the characters
        ldrb    r12, [r1], #1
        subs    r2, r2, #1
        beq     ReturnDiff
        teq     r3, r12                                 ; Are they the same?
        beq     ByteCmp                                 ; Loop if r2 > 1

ReturnDiff
        sub     r0, r3, r12
        bx      lr

ReturnZero
        mov     r0, r2
        bx      lr

        LEAF_END memcmp
        END
