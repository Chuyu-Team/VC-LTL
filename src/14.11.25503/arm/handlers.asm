;***
;handlers.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

#include "ksarm.h"

; CallSettingFrame
;
; this version restores and preserves the permanent register
; state of the funclet
;
; as the funclet returns to this function, it
; would be possible to insert calls to NLG
; for the debugger (are these needed for wince?)
; _ARM_WORKITEM_ NLG support?
;
;;extern "C" void* _CallSettingFrame(
;;    void*               handler,
;;    EHRegistrationNode* pRN,
;;    PULONG  *           pNonVolatiles,
;;    ULONG               NLG_CODE)
;
; funclet is expected to return continuation address in r0.
;

        IMPORT _NLG_Notify
        IMPORT __NLG_Return2

        NESTED_ENTRY _CallSettingFrame
        PROLOG_NOP      ldr r1,[r1]                     ; get establisher frame
        PROLOG_PUSH     {r0-r3}                         ; save arguments
        PROLOG_PUSH     {r1,r4-r11, lr}                 ; save registers, plus establisher frame

        mov     r2, r3                                  ; r0, r1 already have the values we want
        blx     _NLG_Notify                             ; call debugger hook
        
        add     r0, sp, #40
        ldm     r0, {r0-r3}                             ; load the arguments
        ldm     r2, {r4-r11}                            ; load non-volatiles from r1
        blx     r0                                      ; branch to handler
        mov     r4, r0                                  ; save the return from the handler
        
        blx     __NLG_Return2                           ; call debugger hook (no params)

        mov     r0, r4
        ldr     r1, [sp]
        mov     r2, #2                                  ; CATCH_LEAVE parameter for _NLG_Notify
        blx     _NLG_Notify                             ; call debugger hook
        
        mov     r0, r4                                  ; return value from the handler

        EPILOG_POP      {r3-r11, lr}                    ; restore everything
        EPILOG_STACK_FREE 16                            ; remove the arguments from the stack
        EPILOG_RETURN

        NESTED_END _CallSettingFrame
        END
