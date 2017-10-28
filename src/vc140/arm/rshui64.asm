;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
;
; Use of this source code is subject to the terms of your Microsoft Windows CE
; Source Alliance Program license form.  If you did not accept the terms of
; such a license, you are not authorized to use this source code.
;
;rshui64.asm - perform unsigned __int64 right shift operation
;

; r0 - loword of __int64 value
; r1 - hiword of __int64 value
; r2 - amount to right shift value

#include "kxarm.h"

    TEXTAREA

    LEAF_ENTRY __rt_ursh

    CMP     r2, #0
    BXEQ    lr
    
    CMP     r2, #32
    BLT     shift31

    CMP     r2, #64
    BLT     shift63

    MOV     r1, r1, LSR #32             ; shift >= 64, value = 0
    MOV     r0, r1
    BX      lr

shift63                                 ; 32 <= shift < 64
    AND     r2, r2, #31
    MOV     r0, r1, LSR r2
    MOV     r1, #0
    BX      lr

shift31                                 ; shift < 32
    MOV     r0, r0, LSR r2
    RSB     r3, r2, #32
    MOV     r3, r1, LSL r3
    ORR     r0, r0, r3
    MOV     r1, r1, LSR r2
    BX      lr

    LEAF_END __rt_ursh

    END
