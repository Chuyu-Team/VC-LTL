;***
;handlers.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

include ksamd64.inc


EXTRN   _NLG_Notify:PROC
EXTRN   __NLG_Return2:PROC

;;++
;;
;;extern "C" void* _CallSettingFrame(
;;    void*               handler,
;;    EHRegistrationNode  *pEstablisher,
;;    ULONG               NLG_CODE)
;;
;;--

CsFrame struct
    P1Home          dq      ?               ; Parameter save area
    P2Home          dq      ?               ;
    P3Home          dq      ?               ;
    P4Home          dq      ?               ;
    Alignment       dq      ?               ; Establisher context
    Return          dq      ?               ; Caller's return address
    Handler         dq      ?               ; Caller parameter save area
    Establisher     dq      ?               ;
    NlgCode         dd      ?               ;
CsFrame ends


    NESTED_ENTRY _CallSettingFrame, _TEXT

    alloc_stack (CsFrame.Return)            ; Allocate private portion of
                                            ; the frame.
    END_PROLOGUE

    mov     CsFrame.Handler[rsp], rcx       ; move parameters to home slots
    mov     CsFrame.Establisher[rsp], rdx   ;
    mov     CsFrame.NlgCode[rsp], r8d       ;

    mov     rdx, [rdx]                      ; dereference establisher pointer
    mov     rax, rcx                        ; prepare return value for dummy notify
    call    _NLG_Notify                     ; notify debugger
    call    rax                             ; call the handler
    call    __NLG_Return2                   ; notify debugger : eh return

    mov     rcx, rax                        ; handler
    mov     rdx, CsFrame.Establisher[rsp]
    mov     rdx, [rdx]                      ; dereference establisher pointer
    mov     r8d, 02h                        ; NLG_CATCH_LEAVE
    call    _NLG_Notify                     ; notify debugger clean the stack,
    add     rsp, CsFrame.Return             ; and return
    ret

    NESTED_END _CallSettingFrame, _TEXT

;;++
;;
;;extern "C" void* _CallSettingFrame_LookupContinuationIndex(
;;    void*               handler,
;;    EHRegistrationNode  *pEstablisher,
;;    ULONG               NLG_CODE)
;;
;;--

CsFrame struct
    P1Home          dq      ?               ; Parameter save area
    P2Home          dq      ?               ;
    P3Home          dq      ?               ;
    P4Home          dq      ?               ;
    Alignment       dq      ?               ; Establisher context
    Return          dq      ?               ; Caller's return address
    Handler         dq      ?               ; Caller parameter save area
    Establisher     dq      ?               ;
    NlgCode         dd      ?               ;
CsFrame ends


    NESTED_ENTRY _CallSettingFrame_LookupContinuationIndex, _TEXT

    alloc_stack (CsFrame.Return)            ; Allocate private portion of
                                            ; the frame.
    END_PROLOGUE

    mov     CsFrame.Handler[rsp], rcx       ; move parameters to home slots
    mov     CsFrame.Establisher[rsp], rdx   ;
    mov     CsFrame.NlgCode[rsp], r8d       ;

    mov     rdx, [rdx]                      ; dereference establisher pointer
    mov     rax, rcx                        ; prepare return value for dummy notify
    call    _NLG_Notify                     ; notify debugger
    call    rax                             ; call the handler
    call    __NLG_Return2                   ; notify debugger : eh return
    add     rsp, CsFrame.Return             ; and return
    ret

    NESTED_END _CallSettingFrame_LookupContinuationIndex, _TEXT

;;++
;;
;;extern "C" void* _CallSettingFrame_NotifyContinuationAddr(
;;    void*               handler,
;;    EHRegistrationNode  *pEstablisher)
;;
;;--

CsFrame struct
    P1Home          dq      ?               ; Parameter save area
    P2Home          dq      ?               ;
    P3Home          dq      ?               ;
    P4Home          dq      ?               ;
    Alignment       dq      ?               ; Establisher context
    Return          dq      ?               ; Caller's return address
    Handler         dq      ?               ; Caller parameter save area
    Establisher     dq      ?               ;
    NlgCode         dd      ?               ;
CsFrame ends


    NESTED_ENTRY _CallSettingFrame_NotifyContinuationAddr, _TEXT

    alloc_stack (CsFrame.Return)            ; Allocate private portion of
                                            ; the frame.
    END_PROLOGUE

    mov     CsFrame.Handler[rsp], rcx       ; move parameters to home slots
    mov     CsFrame.Establisher[rsp], rdx   ;

    mov     rdx, CsFrame.Establisher[rsp]
    mov     rdx, [rdx]                      ; dereference establisher pointer
    mov     r8d, 02h                        ; NLG_CATCH_LEAVE
    call    _NLG_Notify                     ; notify debugger clean the stack,
    add     rsp, CsFrame.Return             ; and return
    ret

    NESTED_END _CallSettingFrame_NotifyContinuationAddr, _TEXT

;;++
;;
;;extern "C" void* _CallSettingFrameEncoded(
;;    void*               handler,
;;    EHRegistrationNode  pEstablisher,
;;    void*               object,
;;    ULONG               NLG_CODE)
;;
;;--

CsFrameEncoded struct
    P1Home          dq      ?               ; Parameter save area
    P2Home          dq      ?               ;
    P3Home          dq      ?               ;
    P4Home          dq      ?               ;
    Alignment       dq      ?               ; Establisher context
    Return          dq      ?               ; Caller's return address
    Handler         dq      ?               ; Caller parameter save area
    Establisher     dq      ?               ;
    Object          dq      ?               ;
    NlgCode         dd      ?               ;
CsFrameEncoded ends


    NESTED_ENTRY _CallSettingFrameEncoded, _TEXT

    alloc_stack (CsFrameEncoded.Return)            ; Allocate private portion of
                                                   ; the frame.
    END_PROLOGUE

    mov     CsFrameEncoded.Handler[rsp], rcx       ; move parameters to home slots
    mov     CsFrameEncoded.Establisher[rsp], rdx   ;
    mov     CsFrameEncoded.Object[rsp], r8         ;
    mov     CsFrameEncoded.NlgCode[rsp], r9d       ;
    mov     r8d, r9d                               ; move NlgCode to correct register for _NLG_Notify

    mov     rax, rcx                               ; prepare return value for dummy notify
    call    _NLG_Notify                            ; notify debugger
    mov     rcx, CsFrameEncoded.Object[rsp]        ; pass address of object to destruct to destructor
    call    rax                                    ; call the handler
    call    __NLG_Return2                          ; notify debugger : eh return

    mov     rcx, rax                               ; handler
    mov     rdx, CsFrameEncoded.Establisher[rsp]   ;
    mov     r8d, 02h                               ; NLG_CATCH_LEAVE
    call    _NLG_Notify                            ; notify debugger clean the stack,
    add     rsp, CsFrameEncoded.Return             ; and return
    ret

    NESTED_END _CallSettingFrameEncoded, _TEXT

END
