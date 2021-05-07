;***
;except.inc - definitions for exception handling
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       Structure and constant definitions used by exception handling code.
;
;*******************************************************************************

; structure used by SEH support function and intrinsics. the information
; passed by the OS exception dispatcher is repackaged in this form by the
; runtime (_except_handler()).

__EXCEPTION_INFO_PTRS struc
        preport                 dd      ?
        pcontext                dd      ?
__EXCEPTION_INFO_PTRS ends


; exception registration record structure.

__EXCEPTIONREGISTRATIONRECORD struc
        prev_structure          dd      ?
        ExceptionHandler        dd      ?
        ExceptionFilter         dd      ?
        FilterFrame             dd      ?
        PExceptionInfoPtrs      dd      ?
__EXCEPTIONREGISTRATIONRECORD ends

; size of exception registration record in double words

DWORDS_IN_XREGREC       equ     5


; exception report record

__EXCEPTIONREPORTRECORD struc
        ExceptionNum            dd      ?
        fHandlerFlags           dd      ?
        ExceptionReportRecord   dd      ?
        ExceptionAddress        dd      ?
        cParameters             dd      ?
        ExceptionInfo           dd      4 dup (?)
__EXCEPTIONREPORTRECORD ends


; setjmp/longjmp buffer (i.e., structure underlying jmp_buf array)

_JMP_BUF        struc
        ebpsave                 dd      ?
        ebxsave                 dd      ?
        edisave                 dd      ?
        esisave                 dd      ?
        espsave                 dd      ?
        retsave                 dd      ?
        xcptsave                dd      ?
_JMP_BUF        ends


; exceptions corresponding to C runtime errors (these are explicitly
; referenced in the startup code)

_XCPT_UNABLE_TO_GROW_STACK      equ             080010001h
_XCPT_INTEGER_DIVIDE_BY_ZERO    equ             0C000009Bh
_XCPT_NONCONTINUABLE_EXCEPTION  equ             0C0000024h
_XCPT_INVALID_DISPOSITION       equ             0C0000025h
_XCPT_SIGABRT                   equ             020000001h


; unwind settings in fHandlerFlags

_EH_UNWINDING   equ     2
_EH_EXIT_UNWIND equ     4
UNWIND          equ     _EH_UNWINDING OR _EH_EXIT_UNWIND


; return values (to the exception dispatcher)

IFDEF   _WIN32

_XCPT_CONTINUE_SEARCH           equ     000000001h
_XCPT_CONTINUE_EXECUTION        equ     000000000h

ENDIF
