        page    ,132
        title   memcmp - compare two blocks of memory
;***
;memcmp.asm - compare two blocks of memory
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines memcmp() - compare two memory blocks lexically and
;       find their order.
;
;*******************************************************************************

include ksamd64.inc

        subttl  "memcmp"
;***
;int memcmp(buf1, buf2, count) - compare memory for lexical order
;
;Purpose:
;       Compares count bytes of memory starting at buf1 and buf2
;       and find if equal or which one is first in lexical order.
;
;       Algorithm:
;       int
;       memcmp (buf1, buf2, count)
;               char *buf1, *buf2;
;               unsigned count;
;       {
;               if (!count)
;                       return(0);
;               while (--count && *buf1 == *buf2)
;                       {
;                       buf1++;
;                       buf2++;
;                       }
;               return(*buf1 - *buf2);
;       }
;
;Entry:
;       char *buf1, *buf2 - pointers to memory sections to compare
;       unsigned count - length of sections to compare
;
;Exit:
;       returns -1 if buf1 < buf2
;       returns  0 if buf1 == buf2
;       returns +1 if buf1 > buf2
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************


        LEAF_ENTRY_ARG3 memcmp, _TEXT, buf1:ptr byte, buf2:ptr byte, count:dword

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        sub     rdx, rcx                ; compute source address difference
        cmp     r8, 8                   ; check if 8 bytes to compare
        jb      short mcmp30            ; if b, less than 8 bytes to compare
        test    cl, 7                   ; test if source1 quadword aligned
        jz      short mcmp20            ; if z, source1 quadword aligned

;
; Align source1 to an 8-byte boundary.
;

        align   16

mcmp10: mov     al, [rcx]               ; compare single byte
        cmp     al, [rcx + rdx]         ;
        jne     short mcmp_not_equal    ; if ne, byte mismatch
        inc     rcx                     ; increment source1 address
        dec     r8                      ; decrement number of bytes
        test    cl, 7                   ; test is source1 quadword aligned
        jne     short mcmp10            ; if ne, source1 not quadword aligned

;
; Check for 8-byte blocks.
;

mcmp20: mov     r9, r8                  ; copy count of bytes remaining
        shr     r9, 3                   ; compute number of 8-byte blocks
        jnz     short mcmp50            ; if nz, 8-byte blocks remaining

;
; Compare remaining bytes.
;

mcmp30: test    r8, r8                  ; test if any bytes remaining
        jz      short mcmp_equal        ; if z, no byte remaining
mcmp40: mov     al, [rcx]               ; compare single byte
        cmp     al, [rcx + rdx]         ;
        jne     short mcmp_not_equal    ; if ne, bytes mismatch
        inc     rcx                     ; advance to next byte
        dec     r8                      ; decrement remaining byte count
        jnz     short mcmp40            ; if nz, more bytes remaining

;
; Memory compared equal.
;

mcmp_equal:                             ;
        xor     rax, rax                ; memory compared equal
        ret                             ; return

;
; Memory compared not equal.
;

mcmp_not_equal:                         ;
        sbb     eax, eax                ; AX=-1, CY=1 AX=0, CY=0
        sbb     eax, -1                 ;
        ret                             ; return

;
; Compare 32-byte blocks.
;

        db      090h                    ; fill

mcmp50: shr     r9, 2                   ; compute 32-byte block remaining
        jz      short mcmp70            ; if z, no 32-byte blocks
mcmp60: mov     rax, [rcx]              ; compare 32-byte block
        cmp     rax, [rcx + rdx]        ;
        jne     mcmp_adjust0            ;
        mov     rax, 8[rcx]             ;
        cmp     rax, 8[rcx + rdx]       ;
        jne     mcmp_adjust8            ;
        mov     rax, 16[rcx]            ;
        cmp     rax, 16[rcx + rdx]      ;
        jne     mcmp_adjust16           ;
        mov     rax, 24[rcx]            ;
        cmp     rax, 24[rcx + rdx]      ;
        jne     mcmp_adjust24           ;
        add     rcx, 32                 ; advance source1 32-byte block address
        dec     r9                      ; decrement loop count
        jnz     short mcmp60            ; if nz, more 32-byte blocks
        and     r8, 31                  ; compute remaining byte count

;
; Compare 8-byte blocks.
;

mcmp70: mov     r9, r8                  ; copy count of bytes remaining
        shr     r9, 3                   ; compute number of 8-byte blocks
        jz      short mcmp30            ; if z, no 8-byte blocks remaining
mcmp80: mov     rax, [rcx]              ; compare single quadword
        cmp     rax, [rcx + rdx]        ;
        jne     short mcmp_adjust0      ;
        add     rcx, 8                  ; advance source1 8-byte block address
        dec     r9                      ; decrement loop count
        jnz     short mcmp80            ; if nz, more 8-byte blocks
        and     r8, 7                   ; compute remaining byte count
        jmp     mcmp30                  ;

;
; Adjust source1 address to account for equal bytes.
;

mcmp_adjust24:                          ;
        add     rcx, 8                  ;

mcmp_adjust16:                          ;
        add     rcx, 8                  ;

mcmp_adjust8:                           ;
        add     rcx, 8                  ;

mcmp_adjust0:                           ;
        mov     rcx, [rdx + rcx]        ; get second source value
        bswap   rax                     ; swap to big endian format
        bswap   rcx                     ;
        cmp     rax, rcx                ; set condition codes
        sbb     eax, eax                ; AX=-1, CY=1 AX=0, CY=0
        sbb     eax, -1                 ;
        ret                             ; return

        LEAF_END memcmp, _TEXT

        end
