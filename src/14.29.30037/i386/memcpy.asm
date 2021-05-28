       page    ,132
        title   memcpy - Copy source memory bytes to destination
;***
;memcpy.asm - contains memcpy and memmove routines
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       memcpy() copies a source memory buffer to a destination buffer.
;       Overlapping buffers are not treated specially, so propagation may occur.
;       memmove() copies a source memory buffer to a destination buffer.
;       Overlapping buffers are treated specially, to avoid propagation.
;
;*******************************************************************************

        .xlist
        include vcruntime.inc
        .list
        .xmm

M_EXIT  macro
        ret                     ; _cdecl return
        endm    ; M_EXIT

PALIGN_memcpy macro d
MovPalign&d&:
        movdqa  xmm1,xmmword ptr [esi-d]
        lea     esi, byte ptr [esi-d]

        align   @WordSize

PalignLoop&d&:
        movdqa  xmm3,xmmword ptr [esi+10h]
        sub     ecx,30h
        movdqa  xmm0,xmmword ptr [esi+20h]
        movdqa  xmm5,xmmword ptr [esi+30h]
        lea     esi, xmmword ptr [esi+30h]
        cmp     ecx,30h
        movdqa  xmm2,xmm3

        palignr xmm3,xmm1,d

        movdqa  xmmword ptr [edi],xmm3
        movdqa  xmm4,xmm0

        palignr xmm0,xmm2,d

        movdqa  xmmword ptr [edi+10h],xmm0
        movdqa  xmm1,xmm5

        palignr xmm5,xmm4,d

        movdqa  xmmword ptr [edi+20h],xmm5
        lea     edi, xmmword ptr [edi+30h]
        jae     PalignLoop&d&
        lea     esi, xmmword ptr [esi+d]

        endm    ; PALIGN_memcpy

        CODESEG

    extrn   __isa_available:dword
    extrn   __isa_enabled:dword
    extrn   __favor:dword

page
;***
;memcpy - Copy source buffer to destination buffer
;
;Purpose:
;       memcpy() copies a source memory buffer to a destination memory buffer.
;       This routine does NOT recognize overlapping buffers, and thus can lead
;       to propagation.
;       For cases where propagation must be avoided, memmove() must be used.
;
;       Algorithm:
;
;           Same as memmove. See Below
;
;
;memmove - Copy source buffer to destination buffer
;
;Purpose:
;       memmove() copies a source memory buffer to a destination memory buffer.
;       This routine recognize overlapping buffers to avoid propagation.
;       For cases where propagation is not a problem, memcpy() can be used.
;
;   Algorithm:
;
;       void * memmove(void * dst, void * src, size_t count)
;       {
;               void * ret = dst;
;
;               if (dst <= src || dst >= (src + count)) {
;                       /*
;                        * Non-Overlapping Buffers
;                        * copy from lower addresses to higher addresses
;                        */
;                       while (count--)
;                               *dst++ = *src++;
;                       }
;               else {
;                       /*
;                        * Overlapping Buffers
;                        * copy from higher addresses to lower addresses
;                        */
;                       dst += count - 1;
;                       src += count - 1;
;
;                       while (count--)
;                               *dst-- = *src--;
;                       }
;
;               return(ret);
;       }
;
;
;Entry:
;       void *dst = pointer to destination buffer
;       const void *src = pointer to source buffer
;       size_t count = number of bytes to copy
;
;Exit:
;       Returns a pointer to the destination buffer in AX/DX:AX
;
;Uses:
;       CX, DX
;
;Exceptions:
;*******************************************************************************

ifdef MEM_MOVE
        _MEM_     equ <memmove>
else  ; MEM_MOVE
        _MEM_     equ <memcpy>
endif  ; MEM_MOVE

%       public  _MEM_
_MEM_   proc \
        dst:ptr byte, \
        src:ptr byte, \
        count:IWORD

        ; destination pointer
        ; source pointer
        ; number of bytes to copy

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        push    edi                             ; save edi
        push    esi                             ; save esi

; size param/4   prolog byte  #reg saved
        .FPO ( 0, 3           , $-_MEM_     , 2, 0, 0 )

        mov     esi,[esp + 010h]                ; esi = source
        mov     ecx,[esp + 014h]                ; ecx = number of bytes to move
        mov     edi,[esp + 0Ch]                 ; edi = dest

;
; Check for overlapping buffers:
;       If (dst <= src) Or (dst >= src + Count) Then
;               Do normal (Upwards) Copy
;       Else
;               Do Downwards Copy to avoid propagation
;

        mov     eax,ecx                         ; eax = byte count

        mov     edx,ecx                         ; edx = byte count
        add     eax,esi                         ; eax = point past source end

        cmp     edi,esi                         ; dst <= src ?
        jbe     short CopyUp                    ; no overlap: copy toward higher addresses

        cmp     edi,eax                         ; dst < (src + count) ?
        jb      CopyDown                        ; overlap: copy toward lower addresses

;
; Buffers do not overlap, copy toward higher addresses.
;
CopyUp:
        cmp     ecx, 020h
        jb      CopyUpDwordMov                  ; size smaller than 32 bytes, use dwords
        cmp     ecx, 080h
        jae     CopyUpLargeMov                  ; if greater than or equal to 128 bytes, use Enhanced fast Strings
        bt      __isa_enabled, __ISA_AVAILABLE_SSE2
        jc      XmmCopySmallTest
        jmp     Dword_align

CopyUpLargeMov:
        bt      __favor, __FAVOR_ENFSTRG        ; check if Enhanced Fast Strings is supported
        jnc     CopyUpSSE2Check                 ; if not, check for SSE2 support
        rep     movsb
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi
        pop     edi
        M_EXIT

;
;      Check if source and destination are equally aligned.
;
CopyUpSSE2Check:
        mov     eax,edi
        xor     eax,esi
        test    eax,15
        jne     AtomChk   ; Not aligned go check Atom
        bt      __isa_enabled, __ISA_AVAILABLE_SSE2
        jc      XmmCopy ; yes, go SSE2 copy (params already set)
AtomChk:
        ; Is Atom supported?
        bt      __favor, __FAVOR_ATOM
        jnc     Dword_align ; no,jump

        ; check if dst is 4 byte aligned
        test    edi, 3
        jne     Dword_align

        ; check if src is 4 byte aligned
        test    esi, 3
        jne     Dword_align_Ok

; A software pipelining vectorized memcpy loop using PALIGN instructions

; (1) copy the first bytes to align dst up to the nearest 16-byte boundary
; 4 byte align -> 12 byte copy, 8 byte align -> 8 byte copy, 12 byte align -> 4 byte copy
PalignHead4:
        bt      edi, 2
        jae     PalignHead8
        mov     eax, dword ptr [esi]
        sub     ecx, 4
        lea     esi, byte ptr [esi+4]
        mov     dword ptr [edi], eax
        lea     edi, byte ptr [edi+4]

PalignHead8:
        bt      edi, 3
        jae     PalignLoop
        movq    xmm1, qword ptr [esi]
        sub     ecx, 8
        lea     esi, byte ptr [esi+8]
        movq    qword ptr [edi], xmm1
        lea     edi, byte ptr [edi+8]

;(2) Use SSE palign loop
PalignLoop:
        test    esi, 7
        je      MovPalign8
        bt      esi, 3
        jae     MovPalign4

PALIGN_memcpy 12
        jmp     PalignTail

PALIGN_memcpy 8
        jmp     PalignTail

PALIGN_memcpy 4

;(3) Copy the tailing bytes.
PalignTail:
        cmp    ecx,10h
        jb     PalignTail4
        movdqu xmm1,xmmword ptr [esi]
        sub    ecx, 10h
        lea    esi, xmmword ptr [esi+10h]
        movdqa xmmword ptr [edi],xmm1
        lea    edi, xmmword ptr [edi+10h]
        jmp    PalignTail

PalignTail4:
        bt      ecx, 2
        jae     PalignTail8
        mov     eax, dword ptr [esi]
        sub     ecx,4
        lea     esi, byte ptr [esi+4]
        mov     dword ptr [edi], eax
        lea     edi, byte ptr [edi+4]

PalignTail8:
        bt      ecx, 3
        jae     PalignTailLE3
        movq    xmm1, qword ptr [esi]
        sub     ecx,8
        lea     esi, byte ptr [esi+8]
        movq    qword ptr [edi], xmm1
        lea     edi, byte ptr [edi+8]

PalignTailLE3:
        mov     eax, dword ptr TrailingUpVec[ecx*4]
        jmp     eax

; The algorithm for forward moves is to align the destination to a dword
; boundary and so we can move dwords with an aligned destination.  This
; occurs in 3 steps.
;
;   - move x = ((4 - Dest & 3) & 3) bytes
;   - move y = ((L-x) >> 2) dwords
;   - move (L - x - y*4) bytes
;

Dword_align:
        test   edi,11b                          ; check if destination is dword aligned
        jz     short Dword_align_Ok             ; if destination not dword aligned already, it should be aligned

Dword_up_align_loop:
        mov     al, byte ptr [esi]
        mov     byte ptr [edi], al
        dec     ecx
        add     esi, 1
        add     edi, 1
        test    edi, 11b
        jnz     Dword_up_align_loop
Dword_align_Ok:
        mov     edx, ecx
        cmp     ecx, 32
        jb      CopyUpDwordMov
        shr     ecx,2
        rep     movsd                           ; move all of our dwords
        and     edx,11b                         ; trailing byte count
        jmp     dword ptr TrailingUpVec[edx*4]  ; process trailing bytes

;
; Code to do optimal memory copies for non-dword-aligned destinations.
;

; The following length check is done for two reasons:
;
;    1. to ensure that the actual move length is greater than any possible
;       alignment move, and
;
;    2. to skip the multiple move logic for small moves where it would
;       be faster to move the bytes with one instruction.
;


        align   @WordSize
ByteCopyUp:
        jmp     dword ptr TrailingUpVec[ecx*4+16]  ; process just bytes


;-----------------------------------------------------------------------------

        align   @WordSize
TrailingUpVec      dd      TrailingUp0, TrailingUp1, TrailingUp2, TrailingUp3

        align   @WordSize
TrailingUp0:
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
                                                ; spare
        M_EXIT

        align   @WordSize
TrailingUp1:
        mov     al,[esi]                        ; get byte from source
                                                ; spare
        mov     [edi],al                        ; put byte in destination
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

        align   @WordSize
TrailingUp2:
        mov     al,[esi]                        ; get first byte from source
                                                ; spare
        mov     [edi],al                        ; put first byte into destination
        mov     al,[esi+1]                      ; get second byte from source
        mov     [edi+1],al                      ; put second byte into destination
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

        align   @WordSize
TrailingUp3:
        mov     al,[esi]                        ; get first byte from source
                                                ; spare
        mov     [edi],al                        ; put first byte into destination
        mov     al,[esi+1]                      ; get second byte from source
        mov     [edi+1],al                      ; put second byte into destination
        mov     al,[esi+2]                      ; get third byte from source
        mov     [edi+2],al                      ; put third byte into destination
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Copy down to avoid propagation in overlapping buffers.
        align   @WordSize
CopyDown:
; inserting check for size. For < 16 bytes, use dwords without checking for alignment

        lea     esi, [esi+ecx] ; esi, edi pointing to the end of the buffer
        lea     edi,  [edi+ecx]
        cmp     ecx, 32
        jb      CopyDownSmall
        bt      __isa_enabled, __ISA_AVAILABLE_SSE2
        jc      XmmMovLargeAlignTest
; See if the destination start is dword aligned

        test    edi,11b                         ; Test if dword aligned
        jz      CopyDownAligned                 ; If not, jump

CopyDownNotAligned:
        mov     edx,edi                         ; get destination offset
        and     edx, 11b
        sub     ecx, edx
CopyDownAlignLoop:
        mov     al, byte ptr [esi-1]
        mov     byte ptr[edi-1], al
        dec     esi
        dec     edi
        sub     edx, 1
        jnz     CopyDownAlignLoop

CopyDownAligned:
        cmp     ecx,32                          ; test if small enough for unwind copy
        jb      CopyDownSmall                   ; if so, then jump
        mov     edx, ecx
        shr     ecx,2                           ; shift down to dword count
        and     edx,11b                         ; trailing byte count
        sub     esi, 4
        sub     edi, 4                          ; setting up src, dest registers
        std                                     ; set direction flag
        rep     movsd                           ; move all of dwords at once
        cld                                     ; clear direction flag back

        jmp     dword ptr TrailingDownVec[edx*4]; process trailing bytes


;-----------------------------------------------------------------------------

        align   @WordSize
TrailingDownVec    dd      TrailingDown0, TrailingDown1, TrailingDown2, TrailingDown3

        align   @WordSize
TrailingDown0:
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
                                                ; spare
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

        align   @WordSize
TrailingDown1:
        mov     al,[esi+3]                      ; get byte from source
                                                ; spare
        mov     [edi+3],al                      ; put byte in destination
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

        align   @WordSize
TrailingDown2:
        mov     al,[esi+3]                      ; get first byte from source
                                                ; spare
        mov     [edi+3],al                      ; put first byte into destination
        mov     al,[esi+2]                      ; get second byte from source
        mov     [edi+2],al                      ; put second byte into destination
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

        align   @WordSize
TrailingDown3:
        mov     al,[esi+3]                      ; get first byte from source
                                                ; spare
        mov     [edi+3],al                      ; put first byte into destination
        mov     al,[esi+2]                      ; get second byte from source
        mov     [edi+2],al                      ; put second byte into destination
        mov     al,[esi+1]                      ; get third byte from source
        mov     [edi+1],al                      ; put third byte into destination
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT

; Copy overlapping buffers using XMM registers
XmmMovLargeAlignTest:
        test    edi, 0Fh                        ; check if it's 16-byte aligned
        jz     XmmMovLargeLoop
XmmMovAlignLoop:
        dec     ecx
        dec     esi
        dec     edi
        mov     al, [esi]
        mov     [edi], al
        test    edi, 0Fh
        jnz     XmmMovAlignLoop

XmmMovLargeLoop:
        cmp     ecx, 128
        jb      XmmMovSmallTest
        sub         esi, 128
        sub         edi, 128
        movdqu     xmm0, xmmword ptr[esi]
        movdqu     xmm1, xmmword ptr[esi+16]
        movdqu     xmm2, xmmword ptr[esi+32]
        movdqu     xmm3, xmmword ptr[esi+48]
        movdqu     xmm4, xmmword ptr[esi+64]
        movdqu     xmm5, xmmword ptr[esi+80]
        movdqu     xmm6, xmmword ptr[esi+96]
        movdqu     xmm7, xmmword ptr[esi+112]
        movdqu     xmmword ptr[edi], xmm0
        movdqu     xmmword ptr[edi+16], xmm1
        movdqu     xmmword ptr[edi+32], xmm2
        movdqu     xmmword ptr[edi+48], xmm3
        movdqu     xmmword ptr[edi+64], xmm4
        movdqu     xmmword ptr[edi+80], xmm5
        movdqu     xmmword ptr[edi+96], xmm6
        movdqu     xmmword ptr[edi+112], xmm7
        sub     ecx, 128
        test    ecx, 0FFFFFF80h
        jnz     XmmMovLargeLoop


XmmMovSmallTest:
        cmp     ecx, 32                         ; if lesser than 32, use dwords
        jb     CopyDownSmall

XmmMovSmallLoop:
        sub        esi, 32
        sub        edi, 32
        movdqu     xmm0, xmmword ptr[esi]
        movdqu     xmm1, xmmword ptr[esi+16]
        movdqu     xmmword ptr[edi], xmm0
        movdqu     xmmword ptr[edi+16], xmm1
        sub        ecx, 32
        test       ecx, 0FFFFFFE0h
        jnz        XmmMovSmallLoop

CopyDownSmall:
        test    ecx, 0FFFFFFFCh                 ; mask the bytes
        jz      CopyDownByteTest
CopyDownDwordLoop:
        sub     edi, 4
        sub     esi, 4
        mov     eax, [esi]
        mov     [edi], eax
        sub     ecx, 4
        test    ecx, 0FFFFFFFCh
        jnz     CopyDownDwordLoop
CopyDownByteTest:
        test    ecx, ecx
        jz      CopyDownReturn
CopyDownByteLoop:
        sub     edi, 1
        sub     esi, 1
        mov     al, [esi]
        mov     [edi], al
        sub     ecx, 1
        jnz     CopyDownByteLoop
CopyDownReturn:
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
                                                ; spare
        pop     esi                             ; restore esi
        pop     edi                             ; restore edi
        M_EXIT


; Using XMM registers for non-overlapping buffers

align       16
XmmCopy:
        mov         eax, esi
        and         eax, 0Fh
        ; eax = src and dst alignment (src mod 16)
        test        eax, eax
        jne         XmmCopyUnaligned

        ; in:
        ; edi = dst (16 byte aligned)
        ; esi = src (16 byte aligned)
        ; ecx = len is >= (128 - head alignment bytes)
        ; do block copy using SSE2 stores
XmmCopyAligned:
        mov         edx, ecx
        and         ecx, 7Fh
        shr         edx, 7
        je          XmmCopySmallTest
        ; ecx = loop count
        ; edx = remaining copy length

; Copy greater than or equal to 128 bytes using XMM registers
align       16
XmmCopyLargeLoop:
        movdqa      xmm0,xmmword ptr [esi]
        movdqa      xmm1,xmmword ptr [esi + 10h]
        movdqa      xmm2,xmmword ptr [esi + 20h]
        movdqa      xmm3,xmmword ptr [esi + 30h]
        movdqa      xmmword ptr [edi],xmm0
        movdqa      xmmword ptr [edi + 10h],xmm1
        movdqa      xmmword ptr [edi + 20h],xmm2
        movdqa      xmmword ptr [edi + 30h],xmm3
        movdqa      xmm4,xmmword ptr [esi + 40h]
        movdqa      xmm5,xmmword ptr [esi + 50h]
        movdqa      xmm6,xmmword ptr [esi + 60h]
        movdqa      xmm7,xmmword ptr [esi + 70h]
        movdqa      xmmword ptr [edi + 40h],xmm4
        movdqa      xmmword ptr [edi + 50h],xmm5
        movdqa      xmmword ptr [edi + 60h],xmm6
        movdqa      xmmword ptr [edi + 70h],xmm7
        lea         esi,[esi + 80h]
        lea         edi,[edi + 80h]
        dec         edx
        jne         XmmCopyLargeLoop

; Copy lesser than 128 bytes
XmmCopySmallTest:
        test        ecx, ecx
        je          CopyUpReturn

        ; ecx = length (< 128 bytes)
        mov         edx, ecx
        shr         edx, 5                      ; check if there are 32 bytes that can be set
        test        edx, edx
        je          CopyUpDwordMov
        ; if > 16 bytes do a loop (16 bytes at a time)
        ; edx - loop count
        ; edi = dst
        ; esi = src

align 16
XmmCopySmallLoop:
        movdqu      xmm0, xmmword ptr [esi]
        movdqu      xmm1, xmmword ptr [esi + 10h]
        movdqu      xmmword ptr [edi], xmm0
        movdqu      xmmword ptr [edi + 10h], xmm1
        lea         esi, [esi + 20h]
        lea         edi, [edi + 20h]
        dec         edx
        jne         XmmCopySmallLoop

CopyUpDwordMov:

        ; last 1-32 bytes: step back according to dst and src alignment and do a 16-byte copy
        ; esi = src
        ; eax = src alignment  (set at the start of the procedure and preserved up to here)
        ; edi = dst
        ; ecx = remaining len
        and         ecx, 1Fh
        je          CopyUpReturn
CopyUpDwordTest:
        mov     eax, ecx  ; save remaining len and calc number of dwords
        shr     ecx, 2
        je      CopyUpByteTest ; if none try bytes
CopyUpDwordLoop:
        mov     edx, dword ptr [esi]
        mov     dword ptr [edi], edx
        add     edi, 4
        add     esi, 4
        sub     ecx, 1
        jne     CopyUpDwordLoop
CopyUpByteTest:
        mov     ecx, eax
        and     ecx, 03h
        je      CopyUpReturn ; if none return
CopyUpByteLoop:
        mov     al, byte ptr [esi]
        mov     byte ptr [edi], al
        inc     esi
        inc     edi
        dec     ecx
        jne     CopyUpByteLoop
align 16
CopyUpReturn:
        ; return dst
        mov     eax,[esp + 0Ch]                 ; return original destination pointer
        pop     esi
        pop     edi
        M_EXIT


; dst addr is not 16 byte aligned
align 16
XmmCopyUnaligned:

; copy the first the first 1-15 bytes to align both src and dst up to the nearest 16-byte boundary:

; in
; esi = src
; edi = dst
; eax = src and dst alignment
; ecx = length

        mov     edx, 010h
        sub     edx, eax                        ; calculate number of bytes to get it aligned
        sub     ecx, edx                        ; calc new length and save it
        push    ecx
        mov     eax, edx                        ; save alignment byte count for dwords
        mov     ecx, eax                        ; set ecx to rep count
        and     ecx, 03h
        je      XmmAlignDwordTest               ; if no bytes go do dwords
XmmAlignByte:
        mov     dl, byte ptr [esi]              ; move the bytes
        mov     byte ptr [edi], dl
        inc     esi                             ; increment  the addresses
        inc     edi
        dec     ecx                             ; decrement the counter
        jne     XmmAlignByte
XmmAlignDwordTest:
        shr     eax, 2                          ; get dword count
        je      XmmAlignAdjustCnt               ; if none go to main loop
XmmAlignDwordLoop:
        mov     edx, dword ptr [esi]            ; move the dwords
        mov     dword ptr [edi], edx
        lea     esi, [esi+4]                    ; increment the addresses
        lea     edi, [edi+4]
        dec     eax                             ; decrement the counter
        jne     XmmAlignDwordLoop
XmmAlignAdjustCnt:
        pop     ecx                             ; retrieve the adjusted length
        jmp     XmmCopyAligned


_MEM_   endp
        end
