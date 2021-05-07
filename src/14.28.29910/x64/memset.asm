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

include ksamd64.inc
        subttl  "memset"
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
;       size_t count;
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
;   size_t count - number of bytes of dst to fill
;
;Exit:
;   returns dst, with filled bytes
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************
    extrn   __favor:byte
    extrn   __ImageBase:byte
    extrn   __memset_nt_iters:qword            ; defined in cpu_disp.c

__FAVOR_ENFSTRG equ 1

; Code for setting block using enhanced fast strings.
; This code needs to be in a separate routine because
; it uses non-volatile registers which must be saved
; and restored for exception handling.

        NESTED_ENTRY memset_repmovs, _TEXT

        push_reg rdi
        .endprolog
        mov     eax, edx                        ; set byte to move
        mov     rdi, rcx                        ; set destination
        mov     rcx, r8                         ; set count
        rep     stosb                           ; store the bytes
        mov     rax, r11                        ; set return value
        .beginepilog
        pop     rdi
        ret                                     ; return

        NESTED_END memset_repmovs, _TEXT


; Main memset routine


        LEAF_ENTRY_ARG3 memset, _TEXT, buf:ptr byte, value:byte, count:dword

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        mov     r11, rcx                        ; save destination address
        movzx   edx, dl                         ; set fill pattern
        mov     r9, 0101010101010101h           ; replicate fill over 8 bytes
        imul    r9, rdx
        cmp     r8, 16
        jbe     SetBytes16                      ; if count <= 15 use GPR stores to set
        movd    xmm0, r9                        ; bytes to store in bits [0:63]
        punpcklbw xmm0, xmm0                    ; dup bytes to [127:64]

;
; Check which algorithm should be used
;
        cmp     r8, 128                         ; is this a small set, size <= 128?
        ja      XmmSet                          ; if large set, use XMM set
        jmp     XmmSetSmall                     ; otherwise, use a 16-byte block set


;
; Fill using SSE instructions - size must be 16 or more.
;
        ; xmm0 has the byte to store replicated to all byte positions
        ; rcx has the destination, can be overwritten
        ; r11 has the destination, must be preserved for return value
        ; r8  has the count
        align   16
XmmSet:
        test    __favor, 1 SHL __FAVOR_ENFSTRG  ; check if string set should be used
        jnz     memset_repmovs

        ; Aligned stores are much faster on AMD hardware. We need to do an unaligned
        ; store of (16 - (dest mod 16)) bytes, but it's faster to just store 16 bytes
        ; and then start the aligned loop as usual at ((dest + 16) - (dest mod 16)).
        ; This results in (dest mod 16) bytes being stored twice. This is a lot faster
        ; than a bunch of code to store maybe 8 then maybe 4 then maybe 2 then maybe 1
        ; byte to achieve alignment. It could cause data breakpoints to trigger twice,
        ; but they will hit here first and hopefully you will read this comment.
        ; The fastest way to subtract (16 - (dest mod 16)) from the length is to add
        ; (original dest - aligned dest). This avoids having to calculate the value.

        movups  [rcx], xmm0                     ; store 16 unaligned from start
        add     r8, rcx                         ; r8 = dest + length
        add     rcx, 16                         ; dest = (dest + 16)
        and     rcx, -16                        ; dest = (dest + 16) - (dest mod 16)
        sub     r8, rcx                         ; r8 = remaining length

; Attempt to set 128-byte blocks.
;
XmmSetLarge:
        mov     r9, r8                          ; copy count of bytes remaining
        shr     r9, 7                           ; compute number of 128-byte blocks
        jz      XmmSetSmall                     ; if z, no 128-byte blocks to fill
        cmp     r9, __memset_nt_iters           ; threshold defined by cpu_disp.c
        ja      XmmSetLargeLoopNT
;
; Set 128-byte blocks
        align   16
XmmSetLargeLoop:
        movaps  0[rcx], xmm0
        movaps  16[rcx], xmm0
        add     rcx, 128                        ; advance destination address early
        movaps  (32 - 128)[rcx], xmm0
        movaps  (48 - 128)[rcx], xmm0
        dec     r9                              ; dec block counter (set cc for jnz)
        movaps  (64 - 128)[rcx], xmm0
        movaps  (80 - 128)[rcx], xmm0
        movaps  (96 - 128)[rcx], xmm0
        movapd  (112 - 128)[rcx], xmm0
        ; to avoid generating a one-byte NOP for the 'align 16' below the previous
        ; instruction is movapd instead of movaps which is one byte longer but
        ; performs exactly the same operation.
        jnz     XmmSetLargeLoop                 ; loop if more blocks

        and     r8, 127                         ; compute remaining byte count
;
; Attempt to set 16-byte blocks
        align   16
XmmSetSmall:
        mov     r9, r8                          ; copy count of bytes remaining
        shr     r9, 4                           ; compute number of 16-byte blocks
        jz      short XmmSetTrailing

        ; This generates an 8-byte nop, which we execute once. This will change only if
        ; any of the code from msetxmm30 down is modified. The following loop thus is
        ; completely contained within one instruction decode buffer on AMD hardware.
        align   16

;
; Set 16-byte blocks
;
XmmSetSmallLoop:
        movups  [rcx], xmm0
        add     rcx, 16
        dec     r9
        jnz     short XmmSetSmallLoop

XmmSetTrailing:
        and     r8, 15                          ; compute remaining length
        jz      XmmSetReturn                    ; skip over movups if done, we could just do it anyway

        ; As at the start, we are going to do an unaligned store of 16 bytes which will overwrite
        ; some bytes already stored. The math is easier, rcx+r8 is one byte past the end, just
        ; back up 16 from there and store 16.

        movups  [rcx+r8-16], xmm0               ; write remainder, overwriting 16-r8 bytes we already wrote

XmmSetReturn:
        mov     rax, r11                        ; must return original dest that we saved in r11
        ret

;
; Set 128-byte blocks (non-temporal)
        align   16
XmmSetLargeLoopNT:
        movntps 0[rcx], xmm0
        movntps 16[rcx], xmm0
        add     rcx, 128                        ; advance destination address early
        movntps (32 - 128)[rcx], xmm0
        movntps (48 - 128)[rcx], xmm0
        dec     r9                              ; dec block counter (set cc for jnz)
        movntps (64 - 128)[rcx], xmm0
        movntps (80 - 128)[rcx], xmm0
        movntps (96 - 128)[rcx], xmm0
        movntps (112 - 128)[rcx], xmm0
        jnz     XmmSetLargeLoopNT               ; loop if more blocks

        sfence
        and     r8, 127                         ; compute remaining byte count
        jmp     XmmSetSmall

_TEXT   ends

;
; Jump table for fills of 15 bytes or fewer
;

_RDATA   segment read para 'DATA'

MsetTab dd         IMAGEREL msetTab00
        dd         IMAGEREL msetTab01
        dd         IMAGEREL msetTab02
        dd         IMAGEREL msetTab03
        dd         IMAGEREL msetTab04
        dd         IMAGEREL msetTab05
        dd         IMAGEREL msetTab06
        dd         IMAGEREL msetTab07
        dd         IMAGEREL msetTab08
        dd         IMAGEREL msetTab09
        dd         IMAGEREL msetTab10
        dd         IMAGEREL msetTab11
        dd         IMAGEREL msetTab12
        dd         IMAGEREL msetTab13
        dd         IMAGEREL msetTab14
        dd         IMAGEREL msetTab15
        dd         IMAGEREL msetTab16
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        dd 15 dup (IMAGEREL msetTab00) ; 17 -> 31 padding
ENDIF

_RDATA  ends

_TEXT   segment para 'CODE'

        ; Set blocks that are less than 16 bytes long.
        ; Preconditions:
        ; rdx has the byte to fill and has been zero extended (ready for imul)
        ; rcx has dest
        ; r8 has len, r8 < 16
        ; r11 has the dest
        align   16
SetBytes16:
        mov     rdx, r9
        lea     r9, OFFSET __ImageBase
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and     r8, 1Fh                         ; bound r8 to 0-31 in speculation scenarios
ENDIF
        mov     eax, [(IMAGEREL  MsetTab) + r9 +r8*4]
        add     r9, rax
        add     rcx, r8                         ; rcx is now 1 past last byte to set
        mov     rax, r11                        ; set return value
        jmp     r9


        align   16

        ; Code for setting various sized blocks up to 15 bytes long.
        ; preconditions:
        ; rcx points 1 byte beyond end of bytes to set
        ; rax has the correct return value (the original dest)
        ; each byte of the rdx reg is set to the byte to store
msetTab15:
        mov     (-15)[rcx], rdx
        ; fallthrough to 7
msetTab07:
        mov     (-7)[rcx], edx
        ;; fallthrough to 3
msetTab03:
        mov     (-3)[rcx], dx
        ; fallthrough to 1
msetTab01:
        mov     (-1)[rcx], dl
msetTab00:
        ret

        align   16
msetTab12:
        mov     (-12)[rcx], rdx
        ; fallthrough to 4
msetTab04:
        mov     (-4)[rcx], edx
        ret

msetTab09:
        mov     (-9)[rcx], rdx
        mov     (-1)[rcx], dl
        ret

        align   16
msetTab13:
        mov     (-13)[rcx], rdx
        ; fallthrough to 5
msetTab05:
        mov     (-5)[rcx], edx
        mov     (-1)[rcx], dl
        ret

        align   16
msetTab14:
        mov     (-14)[rcx], rdx
        ; fallthrough to 6
msetTab06:
        mov     (-6)[rcx], edx
        ; fallthrough to 2
msetTab02:
        mov     (-2)[rcx], dx
        ret

msetTab08:
        mov     [rax], rdx
        ret

msetTab11:
        mov     [rax], rdx
        mov     (8)[rax], dx
        mov     (10)[rax], dl
        ret

        align   16
msetTab10:
        mov     [rax], rdx
        mov     (8)[rax], dx
        ret

msetTab16:
        mov     [rax], rdx
        mov     (8)[rax], rdx
        ret

        LEAF_END memset, _TEXT

    end
