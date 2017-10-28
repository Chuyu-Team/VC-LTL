;
; Floating-point to 64-bit integer helper functions
;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
; Contact aarongi for any questions/concerns
;
; INT64 __dtoi64(double fpval)
; UINT64 __dtou64(double fpval)
; INT64 __stoi64(float fpval)
; UINT64 __stou64(float fpval)
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

_dtoi64_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatFp64:SHL:16) :OR: (_FpFormatI64:SHL:24))
_dtou64_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatFp64:SHL:16) :OR: (_FpFormatU64:SHL:24))
_stoi64_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatFp32:SHL:16) :OR: (_FpFormatI64:SHL:24))
_stou64_info    equ         ((_FpCodeConvert:SHL:8) :OR: (_FpFormatFp32:SHL:16) :OR: (_FpFormatU64:SHL:24))


        IMPORT  __fphelper_raise_exception
        
        ;
        ; Macro-ize the implementation to handle signed/unsigned outputs,
        ; and for different floating point sizes.
        ;

        MACRO
        CONVERT_FP_TO_I64 $Name, $ExceptFunc, $FloatSize, $Signed

        EXPORT  $Name

        NESTED_ENTRY $Name
        PROLOG_PUSH     {r4-r5,r11,lr}                  ; save return address and r4/r5 for scratch
        
        ;
        ; Set the FPSCR flags for a comparison of the original against zero, and copy
        ; the raw data into r1:r0 (or just r0 for single-precision).
        ;
        
        IF $FloatSize == 64

        vcmp.f64 d0, #0.0                               ; check input against zero
        vmrs    APSR_nzcv, fpscr                        ; move FP flags into regular flags
        vmov    r0, r1, d0                              ; copy from double to integer registers

        ELSE

        vcmp.f32 s0, #0.0                               ; check input against zero
        vmrs    APSR_nzcv, fpscr                        ; move FP flags into regular flags
        vmov    r0, s0                                  ; copy from float to integer registers

        ENDIF

        beq     $Name._zero_case                        ; early exit if 0
        
        ;
        ; Extract raw FP value into an exponent (in r2) and a 64-bit, left-
        ; justified mantissa (in r1:r0). By left-justifying the mantissa, we are
        ; effectively shifting the binary point right 63 bits (multiply by 2**63).
        ;

        IF $FloatSize == 64

        ubfx    r2, r1, #20, #11                        ; extract exponent in r2
        lsls    r1, r1, #11                             ; left justify mantissa in r1:r0
        orr     r1, r1, r0, lsr #21                     ;
        lsls    r0, r0, #11                             ;

        ELSE

        ubfx    r2, r0, #23, #8                         ; extract exponent in r2
        lsls    r1, r0, #8                              ; left justify mantissa in r1:r0
        movs    r0, #0                                  ;

        ENDIF

        orr     r1, r1, #1 << 31                        ; insert implicit one (even for denorms)

        ;
        ; Now unbias the exponent and account for this 63-bit shift. Actual
        ; exponent is (exraw - Bias). We multiplied by 2**63, so the final exponent
        ; should reflect a division by 2**63, or a -63 subtraction, giving a final
        ; exponent of (exraw - Bias - 63).
        ;
        
        IF $FloatSize == 64
        ldr     r3, =(63 + 1023)                        ; double bias is 1023, plus 63 bit shift
        ELSE
        ldr     r3, =(63 + 127)                         ; float bias is 127, plus 63 bit shift
        ENDIF

        subs    r2, r2, r3                              ; subtract from the exponent
        bgt     $Name._compute_overflow                 ; if we end up positive, we will overflow
        
        ;
        ; If the resulting exponent is less than or equal to 0, shift off that many
        ; bits from our 64-bit value.
        ;

        rsbs    r2, r2, #0                              ; r2 = -r2
        cmp     r2, #64                                 ; are we going to shift everything away? 
        bge     $Name._handle_underflow                 ; if so, handle it specially
        
        movs    r5, #0                                  ; use r5 to accumulate bits we lose
        cmp     r2, #32                                 ; do we have less than 32 to bits to shift?
        blt     $Name._small_shift_right                ; if so, skip ahead
        
        subs    r2, r2, #32                             ; reduce our shift count by 32
        mov     r5, r0                                  ; remember low bits in r5
        mov     r0, r1                                  ; copy high word to low word
        movs    r1, #0                                  ; clear high word

$Name._small_shift_right
        rsbs    r3, r2, #32                             ; r3 = 32 - r2
        lsls    r4, r0, r3                              ; get the bits that are going away in r4
        orrs    r5, r5, r4                              ; merge with any other shifted bits
        lsrs    r0, r0, r2                              ; shift off lower bits
        lsls    r4, r1, r3                              ; get bits from the upper half
        orrs    r0, r0, r4                              ; merge into lower half
        lsrs    r1, r1, r2                              ; perform final shift

        IF "$Signed" == "Signed"
        
        ;
        ; Check for overflow (unsigned result must be <= 0x7FFFFFFF).
        ; Note that 0x80000000 is a special case if negative.
        ;

        cmp     r1, #0x80000000                         ; did we overflow?
        bhi     $Name._handle_overflow                  ; if so, handle it
        beq     $Name._handle_possible_overflow         ; if maybe, see if we were positive or negative

        ENDIF

$Name._no_overflow
        vmrs    APSR_nzcv, fpscr                        ; move FP flags into regular flags
        bge     $Name._non_negative                     ; if not negative, we're done
        rsbs    r0, r0, #0                              ; negate the result
        sbc     r1, r1, r1, lsl #1                      ; trick equivalent to rsc r1, r1, #0

        IF "$Signed" != "Signed"
        b       $Name._handle_overflow_non_negative     ; unsigned negative case is always an overflow
        ENDIF

        ;
        ; Check for inexact exceptions (if we shifted out any non-zero bits).
        ;
        
$Name._non_negative
        cbnz    r5, $Name._handle_inexact               ; did we lose any bits? if so, handle the inexact case

$Name._epilogue
        EPILOG_POP      {r4-r5,r11,pc}                  ; restore and exit

$Name._zero_case
        movs    r0, #0                                  ; our result is 0
        movs    r1, #0                                  ;
        b       $Name._epilogue                         ; standard exit

        ;
        ; Handle overflow condition. Here we still do the shifts which will lose
        ; data, but then we hand the result to the floating point exception
        ; processor which will do the appropriate thing.
        ;
        
$Name._compute_overflow
        cmp     r2, #32                                 ; do we have less than 32 to bits to shift?
        blt     $Name._small_shift_left                 ; if so, skip ahead
        
        subs    r2, r2, #32                             ; reduce our shift count by 32
        mov     r1, r0                                  ; copy low word to high word
        movs    r0, #0                                  ; clear low word

$Name._small_shift_left
        rsbs    r3, r2, #32                             ; r3 = 32 - r2
        lsls    r1, r1, r2                              ; shift off upper bits
        lsrs    r4, r0, r3                              ; get lower half bits
        orrs    r1, r1, r4                              ; merge into upper half
        lsls    r0, r0, r2                              ; perform final shift

$Name._handle_overflow
        vmrs    APSR_nzcv, fpscr                        ; move FP flags into regular flags
        bge     $Name._handle_overflow_non_negative     ; if not negative, we're done
        rsbs    r0, r0, #0                              ; negate the result
        sbc     r1, r1, r1, lsl #1                      ; trick equivalent to rsc r1, r1, #0

$Name._handle_overflow_non_negative
        vmrs    r3, fpscr                               ; get current FPSCR
        orr     r3, r3, #_OFBit                         ; set the overflow
        and     r2, r3, r3, lsr #8                      ; apply the enable mask
        lsls    r2, r2, #27                             ; did we reveal any exceptions?
        beq     $Name._epilogue                         ; if not, just exit
        ldr     r2, =_OFBit :OR: $ExceptFunc            ; set the exception type
        bl      __fptoi64_exception_helper              ; call our helper (return value in r0:r1)
        b       $Name._epilogue                         ; standard exit

        IF "$Signed" == "Signed"

$Name._handle_possible_overflow
        vmrs    APSR_nzcv, fpscr                        ; move FP flags into regular flags
        bge     $Name._handle_overflow_non_negative     ; if not negative, then we did overflow
        b       $Name._no_overflow                      ; otherwise, we didn't

        ENDIF

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
        bl      __fptoi64_exception_helper              ; call our helper (return value in r0:r1)
        b       $Name._epilogue                         ; standard exit

        ;
        ; Handle underflow condition. The result is effectively 0, which is
        ; hard-coded and passed off to the exception processor.
        ;
        
$Name._handle_underflow
        movs    r0, #0                                  ; our result is 0
        movs    r1, #0                                  ;
        vmrs    r3, fpscr                               ; get current FPSCR
        orr     r3, r3, #_UFBit                         ; set the underflow
        and     r2, r3, r3, lsr #8                      ; apply the enable mask
        lsls    r2, r2, #27                             ; did we reveal any exceptions?
        beq     $Name._epilogue                         ; if not, just exit
        ldr     r2, =_UFBit :OR: $ExceptFunc            ; set the exception type
        bl      __fptoi64_exception_helper              ; call our helper (return value in r0:r1)
        b       $Name._epilogue                         ; standard exit

        NESTED_END $Name


        MEND

        ;
        ; Instantiate the macro for all the cases we need.
        ;

        TEXTAREA

        CONVERT_FP_TO_I64 __dtoi64, _dtoi64_info, 64, Signed
        CONVERT_FP_TO_I64 __dtou64, _dtou64_info, 64, Unsigned

        CONVERT_FP_TO_I64 __stoi64, _stoi64_info, 32, Signed
        CONVERT_FP_TO_I64 __stou64, _stou64_info, 32, Unsigned


        NESTED_ENTRY __fptoi64_exception_helper
        ;
        ; On input:
        ;   s0/d0 = original input value
        ;   r1:r0 = 64-bit result
        ;   r2 = FpscrReason | (Operation << 8) | (Param1Format << 16) | (ResultForamt << 24)
        ;   r3 = Updated Fpscr
        ;
        ; On exit:
        ;   r1:r0 = updated result
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

        vstr    d0, [sp, #0]                            ; save parameter to sp+0
        strd    r0, r1, [sp, #8]                        ; save initial result to sp+8
        uxtb    r0, r2                                  ; unpack fpscr reason
        ubfx    r1, r2, #8, #8                          ; unpack operation
        ubfx    r3, r2, #24, #8                         ; unpack result format
        ubfx    r2, r2, #16, #8                         ; unpack param1 foramt
        bl      __fphelper_raise_exception
                                                        ; updated result already in r0:r1
        
        EPILOG_STACK_FREE   16                          ; deallocate stack
        EPILOG_POP          {r11,pc}                    ; return

        NESTED_END __fptoi64_exception_helper
        
        END