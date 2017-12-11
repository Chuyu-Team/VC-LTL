;
; Division help functions
;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
; Contact aarongi for any questions/concerns
;
; UINT32 __rt_udiv(UINT32 divisor, UINT32 dividend) -- also returns remainder in r1
; UINT32 __rt_sdiv(UINT32 divisor, UINT32 dividend) -- also returns remainder in r1
;
; UINT64 __rt_udiv64(UINT32 divisor, UINT32 dividend) -- also returns remainder in r3:r2
; UINT64 __rt_sdiv64(UINT32 divisor, UINT32 dividend) -- also returns remainder in r3:r2
;

#include "kxarm.h"

        IMPORT __helper_divide_by_0
        
        ;
        ; Macro-ize the implementation of an unsigned 32/32 divide.
        ; Assumes free use of r0-r3
        ;

        MACRO
        DIVIDE_CORE_32_STEP $Shift
        subs    r3, r1, r2, lsl #$Shift                 ; r3 = r1 - r2 << Shift
        movcs   r1, r3                                  ; keep the result if carry set
        adcs    r0, r0, r0                              ; double the result and add the carry
        MEND

        MACRO
        DIVIDE_CORE_32

        cbz     r0, %F1                                 ; skip if divide by 0
        mov     r2, r0                                  ; save the original divisor in r2
        clz     r0, r0                                  ; get zero count of divisor
        clz     r3, r1                                  ; get zero count of dividend
        subs    r0, r0, r3                              ; get difference between bit counts
        adr     r3, %F8 + 1                             ; get address of entry 0 plus thumb bit
        sub     r3, r3, r0, lsl #3                      ; subtract count * 8
        sub     r3, r3, r0, lsl #1                      ; subtract count * 2 = count * 10
        mov     r0, #0                                  ; initialize final result
        blt.w   %F9                                     ; if the number of bits is < 0, result is 0
        bx      r3                                      ; go there
1
        __brkdiv0                                       ; exception if zero

        DIVIDE_CORE_32_STEP 31
        DIVIDE_CORE_32_STEP 30
        DIVIDE_CORE_32_STEP 29
        DIVIDE_CORE_32_STEP 28
        DIVIDE_CORE_32_STEP 27
        DIVIDE_CORE_32_STEP 26
        DIVIDE_CORE_32_STEP 25
        DIVIDE_CORE_32_STEP 24
        DIVIDE_CORE_32_STEP 23
        DIVIDE_CORE_32_STEP 22
        DIVIDE_CORE_32_STEP 21
        DIVIDE_CORE_32_STEP 20
        DIVIDE_CORE_32_STEP 19
        DIVIDE_CORE_32_STEP 18
        DIVIDE_CORE_32_STEP 17
        DIVIDE_CORE_32_STEP 16
        DIVIDE_CORE_32_STEP 15
        DIVIDE_CORE_32_STEP 14
        DIVIDE_CORE_32_STEP 13
        DIVIDE_CORE_32_STEP 12
        DIVIDE_CORE_32_STEP 11
        DIVIDE_CORE_32_STEP 10
        DIVIDE_CORE_32_STEP 9
        DIVIDE_CORE_32_STEP 8
        DIVIDE_CORE_32_STEP 7
        DIVIDE_CORE_32_STEP 6
        DIVIDE_CORE_32_STEP 5
        DIVIDE_CORE_32_STEP 4
        DIVIDE_CORE_32_STEP 3
        DIVIDE_CORE_32_STEP 2
        DIVIDE_CORE_32_STEP 1
8
        DIVIDE_CORE_32_STEP 0
9
        MEND

        
        ;
        ; Macro-ize the implementation of an unsigned 64/64 divide.
        ; Assumes free use of r0-r6, r12
        ;

        MACRO
        DIVIDE_CORE_64

        orrs    r4, r0, r1                              ; check divisor against 0
        beq     %F6                                     ; exception if zero
        
        orrs    r4, r1, r3                              ; check for 32/32 fast case
        beq     %F7                                     ; if we can do it, skip ahead

        mov     r6, r0                                  ; save the original divisor in r12:r6 
        mov     r12, r1                                 ;

        clz     r1, r3                                  ; get zero count of high word
        cbnz    r3, %F1                                 ; if upper word is non-zero, that's it
        clz     r1, r2                                  ; else get zero count of low word
        adds    r1, r1, #32                             ; plus 32
1
        cmp     r12, #0                                 ; is upper word of divisor 0?
        clz     r0, r12                                 ; get zero count of high word
        bne     %F2                                     ; if upper word is non-zero, that's it
        clz     r0, r6                                  ; else get zero count of low word
        adds    r0, r0, #32                             ; plus 32
2
        subs    r0, r0, r1                              ; get difference between bit counts
        blt     %F8                                     ; if divisor is larger than dividend, return 0 
                                                        ; with original dividend

        cmp     r0, #32                                 ; shifting more than 32 bits?
        mov     r5, r12                                 ; copy original values
        mov     r4, r6                                  ;
        blo     %F3                                     ; if less than 32 bits, that's all
        mov     r5, r6                                  ; else preshift by shifting words
        movs    r4, #0                                  ; 
3
        ands    r0, r0, #31                             ; keep low 32 bits of shift count
        lsls    r5, r5, r0                              ; compute r5:r4 = r12:r6 << r0
        rsbs    r1, r0, #32                             ; 
        lsrs    r1, r4, r1                              ; 
        lsls    r4, r4, r0                              ; 
        orrs    r5, r5, r1                              ; 

        movs    r0, #0                                  ; start with a 0 result
        movs    r1, #0                                  ;
        b       %F1                                     ; jump into the middle of the loop

6
        __brkdiv0                                       ; exception if zero

0
        lsrs    r4, r4, #1                              ; shift divisor down 1 bit
        adds    r0, r0, r0                              ; double the result
        orr     r4, r5, lsl #31                         ; shift
        adcs    r1, r1, r1                              ; double
        lsrs    r5, r5, #1                              ; shift
1
        cmp     r5, r3                                  ; compare upper word of shifted divisor to dividend
        bhi     %F3                                     ; skip if above
        blo     %F2                                     ; subtract if below
        cmp     r4, r2                                  ; compare lower word of shifted divisor to dividend
        bhi     %F3                                     ; skip if above
2
        subs    r2, r2, r4                              ; subtract: r3:r2 -= r5:r4
        sbcs    r3, r3, r5                              ; 
        adds    r0, r0, #1                              ; add one to the result: r12:r6++
        adcs    r1, r1, #0                              ;
3
        cmp     r4, r6                                  ; compare lower word against original divisor
        bne     %B0                                     ; if not equal, keep looping
        cmp     r5, r12                                 ; compare upper word against original divisor
        bne     %B0                                     ; if not equal, keep looping
        b       %F9                                     ; skip over the special-case code
7
        mov     r1, r2                                  ; copy low word of dividend to r1
        bl      __rt_udiv                               ; do a 32/32 divide
        mov     r2, r1                                  ; move remainder to r2
        movs    r1, #0                                  ; clear high words
        movs    r3, #0                                  ;
        b       %F9                                     ; skip over the special-case code
8
        movs    r1, #0                                  ; set result to 0
        movs    r0, #0                                  ;
9
        MEND


        TEXTAREA
        
        
        ;
        ; 32-bit unsigned divide + remainder
        ;
        
        LEAF_ENTRY __rt_udiv

        DIVIDE_CORE_32                                  ; handle core division of unsigned values
        bx      lr                                      ; return
        
        LEAF_END __rt_udiv
        
        
        ;
        ; 32-bit signed divide + remainder
        ;
        
        LEAF_ENTRY __rt_sdiv

        ands    r3, r0, #0x80000000                     ; get high bit of divisor in r3
        rsbmi   r0, r0, #0                              ; if that was negative, negate divisor
        eors    r12, r3, r1, asr #32                    ; get sign bits XORed in top of r12
                                                        ; and sign bit of dividend in remaining bits
        rsbcs   r1, r1, #0                              ; if dividend was negative, negate it

        DIVIDE_CORE_32                                  ; handle core division of unsigned values
        
        lsls    r12, r12, #1                            ; shift XORed sign bit into carry
                                                        ; and sign bit of dividend into top bit (mi)
        rsbcs   r0, r0, #0                              ; apply negation of result based on XOR
        rsbmi   r1, r1, #0                              ; apply negation of remainder based on dividend
        bx      lr                                      ; return
        
        LEAF_END __rt_sdiv
        
        
        ;
        ; 64-bit unsigned divide + remainder
        ;
        
        NESTED_ENTRY __rt_udiv64
        PROLOG_PUSH     {r4-r7,r11,lr}                  ; save registers and return address
        
        DIVIDE_CORE_64                                  ; handle core division of unsigned values
        
        EPILOG_POP      {r4-r7,r11,pc}                  ; restore registers and return
        NESTED_END __rt_udiv64
        
        
        ;
        ; 64-bit signed divide + remainder
        ;
        
        NESTED_ENTRY __rt_sdiv64
        PROLOG_PUSH     {r4-r7,r11,lr}                  ; save registers and return address
        
        ands    r4, r1, #0x80000000                     ; get high bit of divisor in r4
        bpl     %F1                                     ; if positive, skip
        rsbs    r0, r0, #0                              ; negate divisor
        sbc     r1, r1, r1, lsl #1                      ; trick equivalent to rsc r1, r1, #0
1       eors    r7, r4, r3, asr #32                     ; get sign bits XORed in top of r7
                                                        ; and sign bit of dividend in remaining bits
        bcc     %F2                                     ; if carry clear, skip
        rsbs    r2, r2, #0                              ; if dividend was negative, negate it
        sbc     r3, r3, r3, lsl #1                      ; trick equivalent to rsc r3, r3, #0
2
        DIVIDE_CORE_64
        
        lsls    r7, r7, #1                              ; shift XORed sign bit into carry
                                                        ; and sign bit of dividend into top bit (mi)
        bcc     %F3                                     ; if no carry, skip
        rsbs    r0, r0, #0                              ; negate result
        sbc     r1, r1, r1, lsl #1                      ; trick equivalent to rsc r1, r1, #0
        ands    r7, r7                                  ; re-test r7 for minus
3       bpl     %F4                                     ; if non-negative, skip
        rsbs    r2, r2, #0                              ; negate remainder
        sbc     r3, r3, r3, lsl #1                      ; trick equivalent to rsc r3, r3, #0
4
        EPILOG_POP      {r4-r7,r11,pc}                  ; restore registers and return
        NESTED_END __rt_sdiv64
        
        END
