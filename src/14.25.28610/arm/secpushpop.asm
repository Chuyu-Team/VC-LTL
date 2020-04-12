        TTL ARM /GS cookie check compiler helper
;++
;
; Copyright (c) Microsoft Corporation
;
; Module Name:
;
;    secpushpop.asm
;
; Abstract:
;
;    ARM security cookie push & pop helpers
;
; Purpose:
;    ARM prologue & epilogue helpers to save code size in functions
;    with /GS cookies.
;

#include "ksarm.h"

        TEXTAREA

        IMPORT __security_cookie
        IMPORT __security_check_cookie

        TEXTAREA

; void __security_push_cookie()
;
; NOTE: this is a prologue helper and the stack may
;       not be aligned to 8-bytes on entry
;
        NESTED_ENTRY __security_push_cookie
        PROLOG_STACK_ALLOC  4

        mov32   r12, __security_cookie
        ldr     r12, [r12]
        sub     r12, sp, r12
        str     r12, [sp]

        ; no EPILOG_STACK_FREE since this helper allocates stack
        EPILOG_RETURN

        NESTED_END __security_push_cookie

; void __security_pop_cookie()
;
; NOTE: this is an epilogue helper and the stack may
;       not be aligned to 8-bytes on entry
;
        NESTED_ENTRY __security_pop_cookie
        ; no PROLOG_STACK_ALLOC since this helper frees stack

        mov32   r12, __security_cookie
        ldr     r3, [sp]
        ldr     r12, [r12]
        sub     r3, sp, r3
        cmp     r3, r12
        bne     %F10
        EPILOG_STACK_FREE   4
        EPILOG_RETURN

10      mov     r0, r3
        ; __fallthrough to __ppgsfailure

        NESTED_END __security_pop_cookie

; void __ppgsfailure(DWORD_PTR cookie)
;
; NOTE: since this can be called from __security_pop_cookie()
;       we need to dynamically align the stack to the ABI requirements
;
        NESTED_ENTRY __ppgsfailure
        PROLOG_PUSH         {r4, lr}
        PROLOG_STACK_SAVE   r4

        ; dynamically align the stack to the EABI requirement
        mov r12, sp
        bic r12, r12, #7
        mov sp, r12

        ; go through "standard" cookie processing
        bl       __security_check_cookie

        EPILOG_STACK_RESTORE r4
        EPILOG_POP          {r4, lr}
        EPILOG_RETURN

        NESTED_END __ppgsfailure

        END
