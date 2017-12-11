;
; 64-bit integer to floating-point helper functions
;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
; Contact aarongi for any questions/concerns
;
; double __u64tod(UINT64 intval)
; double __i64tod(INT64 intval)
; float __u64tos(UINT64 intval)
; float __i64tos(INT64 intval)
;

#include "kxarm.h"

        ;
        ; FPSCR bit constants
        ;
        
_IXBit          equ         1
_UFBit          equ         2
_OFBit          equ         4
_DZBit          equ         8
_IOBit          equ         16

        ;
        ; Floating point exception constants from fpieee.h
        ;
        
_FpFormatFp32   equ         0
_FpFormatFp64   equ         1
_FpFormatI64    equ         6
_FpFormatU64    equ         9

_FpCodeConvert  equ         8

        ;
        ; Derived combinations for our specific needs
        ;

_i64tod_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatI64:SHL:16) :OR: (_FpFormatFp64:SHL:24))
_u64tod_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatU64:SHL:16) :OR: (_FpFormatFp64:SHL:24))
_i64tos_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatI64:SHL:16) :OR: (_FpFormatFp32:SHL:24))
_u64tos_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatU64:SHL:16) :OR: (_FpFormatFp32:SHL:24))


        IMPORT  __fphelper_raise_exception

        ;
        ; Macro-ize the implementation to handle signed/unsigned inputs,
        ; and for different floating point sizes.
        ;

        MACRO
        CONVERT_I64_TO_FP $Name, $ExceptFunc, $FloatSize, $Signed

        EXPORT  $Name

        NESTED_ENTRY $Name
        PROLOG_PUSH     {r0-r1,r11,lr}                  ; save return address and upper half
        
        ;
        ; Compute the absolute value of the input, and compare the upper word
        ; against 0.
        ;

        cmp     r1, #0                                  ; are we zero or negative?

        IF "$Signed" == "Signed"

        bge     $Name._non_negative                     ; if positive, skip
        rsbs    r0, r0, #0                              ; negate the input
        sbcs    r1, r1, r1, lsl #1                      ; trick equivalent to rsc r1, r1, #0

        ENDIF
        
        ;
        ; Left-justify the significant bits in r1:r0. This will become our mantissa.
        ; Compute the shift amount in r2, which becomes our exponent.
        ;

$Name._non_negative
        bne     $Name._shift_small                      ; if r1 is non-zero, we shift less than 32 bits
        clz     r2, r0                                  ; count leading zeros in r2
        lsls    r1, r0, r2                              ; shift low word by that amount and move to high word
        beq     $Name._zero                             ; special case for zero
        movs    r0, #0                                  ; low word is 0 now
        add     r2, r2, #32                             ; we effectively added 32 to the exponent
        b       $Name._post_shift
        
$Name._shift_small
        clz     r2, r1                                  ; get number of leading zeros in r2
        rsbs    r3, r2, #32                             ; use r3 to hold 32-r2
        lsls    r1, r1, r2                              ; left-justify the value
        lsrs    r3, r0, r3                              ; get upper bits from r0
        orrs    r1, r1, r3                              ; merge into r1
        lsls    r0, r0, r2                              ; shift lower bits up
        
        ;
        ; Apply the exponent bias to the value in r2 and position everything
        ; within r1:r0 (or within r1 for single-precision). The bias is further
        ; biased by 63 to account for the fact that our left-justified mantissa
        ; is effectively a 1.63 fixed point value (the top bit is the implicit
        ; 1 from the mantissa).
        ;
        
$Name._post_shift
        IF $FloatSize == 64
        
        ldr     r3, =(1023 + 63)                        ; double bias is 1023, plus 63 bit shift
        rsbs    r2, r2, r3                              ; apply exponent bias
        lsls    r12, r0, #32 - 12                       ; save low 12 bits in r12 for inexact & rounding
        lsrs    r0, r0, #11                             ; shift right by 11 to make room for exponent
        lsls    r3, r1, #32 - 11                        ; get lower bits from upper word
        orrs    r0, r0, r3                              ; merge into lower word
        lsrs    r1, r1, #11                             ; shift upper word down
        bfi     r1, r2, #20, #11                        ; insert the exponent
        
        ELSE
        
        ldr     r3, =(127 + 63)                         ; float bias is 127, plus 63 bit shift
        rsbs    r2, r2, r3                              ; apply exponent bias
        lsls    r12, r1, #32 - 9                        ; save low 9 bits in r12 for inexact & rounding
        lsrs    r1, r1, #8                              ; shift right by 8 to make room for exponent
        bfi     r1, r2, #23, #8                         ; insert the exponent

        ENDIF
        
        ;
        ; If the original value was negative, insert the sign into the
        ; top bit of r1.
        ;
        
        IF "$Signed" == "Signed"
        
        ldr     r2, [sp, #4]                            ; fetch top word of original value
        lsrs    r2, r2, #31                             ; isolate the sign bit
        bfi     r1, r2, #31, #1                         ; insert it
        
        ENDIF
        
        ;
        ; Handle rounding. This is taken from the original algorithm used for Windows CE.
        ; The value in r12 contains the low bit of the mantissa plus bits we lost, all
        ; left-justified in the register.
        ;
        ; Round up if:
        ;   - the "guard" bit (MSB of bits we shifted out) is set
        ;   - AND one of the following is set:
        ;      - the low bit of the final mantissa
        ;      - any of the other (non-guard) bits shifted outs
        ;
        
        tst     r12, #0x40000000                        ; guard bit set?
        beq     $Name._dont_round_up                    ; nope, don't round
        bics    r12, r12, #0x40000000                   ; clear guard bit and check everything else
        beq     $Name._dont_round_up                    ; if nothing else is set, skip rounding

        IF $FloatSize == 64

        adds    r0, r0, #1                              ; round up mantissa; overflow will carry
        adc     r1, r1, #0                              ; into exponent

        ELSE

        add     r1, r1, #1                              ; round up mantissa; overflow will carry
                                                        ; into exponent
        ENDIF

        ;
        ; Copy to the return value and handle inexactitude.
        ;

$Name._dont_round_up        
        IF $FloatSize == 64

        vmov    d0, r0, r1                              ; load d0 from r1:r0
        lsls    r12, r12, #1                            ; did we lose any bits?

        ELSE

        vmov    s0, r1                                  ; load s0 from r1
        lsls    r12, r12, #1                            ; did we lose any bits?
        orrs    r12, r12, r0                            ; make sure to include everything in r0

        ENDIF
        
        bne     $Name._handle_inexact                   ; if so, generate an inexact exception

$Name._epilogue        
        EPILOG_POP      {r0-r1,r11,pc}                  ; restore registers and return

        ;
        ; Handle zero as a special case.
        ;
        
$Name._zero
        IF $FloatSize == 64
        vmov    d0, r1, r1                              ; clear d0
        ELSE
        vmov    s0, r1                                  ; clear s0
        ENDIF
        
        b       $Name._epilogue

        ;
        ; Handle inexact condition. The result is already computed, so just
        ; hand it off to the exception processor.
        ;
        
$Name._handle_inexact
        vmrs    r3, fpscr                               ; get current FPSCR
        orr     r3, r3, #_IXBit                         ; set the inexact
        and     r2, r3, r3, lsr #8                      ; apply the enable mask
        lsls    r2, r2, #27                             ; did we reveal any exceptions?
        beq     $Name._epilogue                         ; if not, just exit
        ldr     r2, =_IXBit :OR: $ExceptFunc            ; set the exception type
        bl      __i64tofp_exception_helper              ; call our helper (return value in r0:r1)
        b       $Name._epilogue                         ; standard exit

        NESTED_END $Name
        
        MEND

        ;
        ; Instantiate the macro for all the cases we need.
        ;

        TEXTAREA

        CONVERT_I64_TO_FP __i64tod, _i64tod_info, 64, Signed
        CONVERT_I64_TO_FP __u64tod, _u64tod_info, 64, Unsigned

        CONVERT_I64_TO_FP __i64tos, _i64tos_info, 32, Signed
        CONVERT_I64_TO_FP __u64tos, _u64tos_info, 32, Unsigned

        

        NESTED_ENTRY __i64tofp_exception_helper
        ;
        ; On input:
        ;   [sp] = original input value
        ;   s0/d0 = result
        ;   r2 = FpscrReason | (Operation << 8) | (Param1Format << 16) | (ResultForamt << 24)
        ;   r3 = Updated Fpscr
        ;
        ; On exit:
        ;   s0/d0 = updated result
        ;
        ; Calling convention for __fphelper_raise_exception:
        ;   r0 = FpscrReason
        ;   r1 = Operation
        ;   r2 = Param1Format
        ;   r3 = ResultFormat
        ;   [sp+0] = Param1Value
        ;   [sp+8] = ResultValue
        ;
        
        PROLOG_PUSH         {r11,lr}                    ; save return address
        PROLOG_STACK_ALLOC  16                          ; allocate space for params

        vmsr    fpscr, r3                               ; store updated FPSCR
                                                        ; parameter already at sp+0
        vstr    d0, [sp, #0]                            ; save result to sp+8
        uxtb    r0, r2                                  ; unpack fpscr reason
        ubfx    r1, r2, #8, #8                          ; unpack operation
        ubfx    r3, r2, #24, #8                         ; unpack result format
        ubfx    r2, r2, #16, #8                         ; unpack param1 foramt
        bl      __fphelper_raise_exception
        vmov    d0, r0, r1                              ; update the result
        
        EPILOG_STACK_FREE   16                          ; deallocate stack
        EPILOG_POP          {r11,pc}                    ; return
        NESTED_END __i64tofp_exception_helper

        END
