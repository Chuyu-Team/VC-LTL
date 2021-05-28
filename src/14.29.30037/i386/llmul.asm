        title   llmul - long multiply routine
;***
;llmul.asm - long multiply routine
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       Defines long multiply routine
;       Both signed and unsigned routines are the same, since multiply's
;       work out the same in 2's complement
;       creates the following routine:
;           __allmul
;
;*******************************************************************************


.xlist
include vcruntime.inc
include mm.inc
.list

;***
;llmul - long multiply routine
;
;Purpose:
;       Does a long multiply (same for signed/unsigned)
;       Parameters are not changed.
;
;Entry:
;       Parameters are passed on the stack:
;               1st pushed: multiplier (QWORD)
;               2nd pushed: multiplicand (QWORD)
;
;Exit:
;       EDX:EAX - product of multiplier and multiplicand
;       NOTE: parameters are removed from the stack
;
;Uses:
;       ECX
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

_allmul PROC NEAR
.FPO (0, 4, 0, 0, 0, 0)

A       EQU     [esp + 4]       ; stack address of a
B       EQU     [esp + 12]      ; stack address of b

;
;       AHI, BHI : upper 32 bits of A and B
;       ALO, BLO : lower 32 bits of A and B
;
;             ALO * BLO
;       ALO * BHI
; +     BLO * AHI
; ---------------------
;

        mov     eax,HIWORD(A)
        mov     ecx,HIWORD(B)
        or      ecx,eax         ;test for both hiwords zero.
        mov     ecx,LOWORD(B)
        jnz     short hard      ;both are zero, just mult ALO and BLO

        mov     eax,LOWORD(A)
        mul     ecx

        ret     16              ; callee restores the stack

hard:
        push    ebx
.FPO (1, 4, 0, 0, 0, 0)

; must redefine A and B since esp has been altered

A2      EQU     [esp + 8]       ; stack address of a
B2      EQU     [esp + 16]      ; stack address of b

        mul     ecx             ;eax has AHI, ecx has BLO, so AHI * BLO
        mov     ebx,eax         ;save result

        mov     eax,LOWORD(A2)
        mul     dword ptr HIWORD(B2) ;ALO * BHI
        add     ebx,eax         ;ebx = ((ALO * BHI) + (AHI * BLO))

        mov     eax,LOWORD(A2)  ;ecx = BLO
        mul     ecx             ;so edx:eax = ALO*BLO
        add     edx,ebx         ;now edx has all the LO*HI stuff

        pop     ebx

        ret     16              ; callee restores the stack

_allmul ENDP

        end
