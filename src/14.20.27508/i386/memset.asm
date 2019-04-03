    page    ,132
    title   memset - set sections of memory all to one byte
;***
;memset.asm - set a section of memory to all one byte
;
;   Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;   contains the memset() routine
;
;*******************************************************************************

    .xlist
    include vcruntime.inc
    .list
    .xmm

page
;***
;char *memset(dst, value, count) - sets "count" bytes at "dst" to "value"
;
;Purpose:
;   Sets the first "count" bytes of the memory starting
;   at "dst" to the character value "value".
;
;   Algorithm:
;   char *
;   memset (dst, value, count)
;       char *dst;
;       char value;
;       unsigned int count;
;       {
;       char *start = dst;
;
;       while (count--)
;           *dst++ = value;
;       return(start);
;       }
;
;Entry:
;   char *dst - pointer to memory to fill with value
;   char value - value to put in dst bytes
;   int count - number of bytes of dst to fill
;
;Exit:
;   returns dst, with filled bytes
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

    CODESEG

    extrn   __isa_available:dword
    extrn   __isa_enabled:dword
    extrn   __favor:dword

    public  memset
memset proc \
        dst:ptr byte, \
        value:byte, \
        count:dword

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        .FPO    ( 0, 3, 0, 0, 0, 0 )

        mov     ecx, [esp + 0ch]             ; the number of bytes to be set
        movzx   eax, byte ptr[esp + 08h]     ; the value to be stored
        mov     edx, edi                     ; saving non-volatile edi
        mov     edi, [esp + 04h]; the dest pointer
        test    ecx, ecx; 0 ?
        jz      toend; if so, nothing to do
        imul    eax, 01010101h
        cmp     ecx, 020h; < 32 bytes use SmallMov
        jbe     SmallMov
        cmp     ecx, 080h; For copies 32 < length < 128
        jb      XmmMovSmall
        bt      __favor, __FAVOR_ENFSTRG
        jnc     XMMMov; no jump

; Enhanced Fast Strings

        rep stosb                        ; store the values in the destination buffer
        mov     eax, dword ptr[esp + 04h]    ; return the original destination pointer
        mov     edi, edx                     ; restoring non-volatile edi
        ret
; XMM register usage

XMMMov :
        bt      __isa_enabled, __ISA_AVAILABLE_SSE2
        jnc     SmallMov                    ; if yes, use xmm large block set
        movd    xmm0, eax
        pshufd  xmm0, xmm0, 0

        add     ecx, edi                    ; ecx points to end of buffer
        movups  [edi], xmm0
        add     edi, 16                     ; point to next xmmword
        and     edi, -16                    ; align xmmword ptr
        sub     ecx, edi                    ; ecx is offset to end of buffer

        cmp     ecx, 080h
        jbe     XmmMovSmall

align 16
LargeRangeBytes :
        movdqa  [edi], xmm0
        movdqa  [edi + 010h], xmm0
        movdqa  [edi + 020h], xmm0
        movdqa  [edi + 030h], xmm0
        movdqa  [edi + 040h], xmm0
        movdqa  [edi + 050h], xmm0
        movdqa  [edi + 060h], xmm0
        movdqa  [edi + 070h], xmm0
        lea     edi, [edi + 080h]
        sub     ecx, 080h
        test    ecx, 0FFFFFF00h
        jnz     LargeRangeBytes
        jmp     XmmSmallLoop


; Do not require 16-byte alignment for sizes lesser than 128 bytes when using XMM registers
XmmMovSmall :
        bt      __isa_enabled, __ISA_AVAILABLE_SSE2
        jnc     SmallMov                    ; if yes, use xmm large block set
        movd    xmm0, eax
        pshufd  xmm0, xmm0, 0

align 16

XmmSmallLoop :
        cmp     ecx, 32
        jb      XMMTrailingBytes
MidRangeBytes :
        movdqu  [edi], xmm0
        movdqu  [edi + 010h], xmm0

        add     edi, 020h
        sub     ecx, 020h
        cmp     ecx, 32                     ; checking number of 32 byte blocks left
        jnb     MidRangeBytes

        test    ecx, 01Fh                   ; check to see if there are bytes left
        jz      toend

XMMTrailingBytes:
; Remaining bytes written with two stores that may overlap instead of 1 byte at a time
        lea     edi, [edi + ecx - 020h]
        movdqu  [edi], xmm0
        movdqu  [edi + 010h], xmm0
        mov     eax, dword ptr [esp + 04h]  ; return the original destination pointer
        mov     edi, edx                    ; restoring non-volatile edi
        ret

; Copying less than or equal to 32 bytes

SmallMov:
        test    ecx, 03h                    ; check if there are bytes that can be stored
        jz      DwordTest

ByteLoop:
        mov     [edi], al
        inc     edi
        sub     ecx, 1
        test    ecx, 03h
        jnz     ByteLoop

DwordTest:
        test    ecx, 04h                     ; checking if there are dword blocks that can be set
        jz      QwordTest
        mov     [edi], eax
        add     edi, 4
        sub     ecx, 4
QwordTest:
        test    ecx, 0FFFFFFF8h
        jz      toend
align 16
QwordLoop:
        mov     [edi], eax
        mov     [edi + 04h], eax
        add     edi, 8
        sub     ecx, 8
        test    ecx, 0FFFFFFF8h
        jnz     QwordLoop

toend:
        mov     eax, dword ptr[esp + 04h]    ; return the original destination pointer
        mov     edi, edx                     ; restore non-volatile edi
        ret

memset  endp

    end
