        title   llshl - long shift left
;***
;llshl.asm - long shift left
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       define long shift left routine (signed and unsigned are same)
;           __allshl
;
;*******************************************************************************


.xlist
include vcruntime.inc
include mm.inc
.list

;***
;llshl - long shift left
;
;Purpose:
;       Does a Long Shift Left (signed and unsigned are identical)
;       Shifts a long left any number of bits.
;
;Entry:
;       EDX:EAX - long value to be shifted
;       CL    - number of bits to shift by
;
;Exit:
;       EDX:EAX - shifted value
;
;Uses:
;       CL is destroyed.
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

_allshl PROC NEAR
.FPO (0, 0, 0, 0, 0 ,0)

;
; Handle shifts of 64 or more bits (all get 0)
;
        cmp     cl, 64
        jae     short RETZERO

;
; Handle shifts of between 0 and 31 bits
;
        cmp     cl, 32
        jae     short MORE32
        shld    edx,eax,cl
        shl     eax,cl
        ret

;
; Handle shifts of between 32 and 63 bits
;
MORE32:
        mov     edx,eax
        xor     eax,eax
        and     cl,31
        shl     edx,cl
        ret

;
; return 0 in edx:eax
;
RETZERO:
        xor     eax,eax
        xor     edx,edx
        ret

_allshl ENDP

        end
