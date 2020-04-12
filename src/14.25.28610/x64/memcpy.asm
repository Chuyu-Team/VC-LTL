        page    ,132
        title   memcpy - Copy source memory bytes to destination
;***
;memcpy.asm - contains memcpy and memmove routines
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       memcpy() copies a source memory buffer to a destination buffer.
;       Overlapping buffers are not treated specially, so propogation may occur.
;       memmove() copies a source memory buffer to a destination buffer.
;       Overlapping buffers are treated specially, to avoid propogation.
;
;*******************************************************************************

include ksamd64.inc
        subttl  "memcpy"

;***
;memcpy - Copy source buffer to destination buffer
;
;Purpose:
;       memcpy() copies a source memory buffer to a destination memory buffer.
;       This routine does NOT recognize overlapping buffers, and thus can lead
;       to propogation.
;       For cases where propogation must be avoided, memmove() must be used.
;
;       Algorithm:
;
;       void * memcpy(void * dst, void * src, size_t count)
;       {
;               void * ret = dst;
;
;               /*
;                * copy from lower addresses to higher addresses
;                */
;               while (count--)
;                       *dst++ = *src++;
;
;               return(ret);
;       }
;
;memmove - Copy source buffer to destination buffer
;
;Purpose:
;       memmove() copies a source memory buffer to a destination memory buffer.
;       This routine recognize overlapping buffers to avoid propogation.
;       For cases where propogation is not a problem, memcpy() can be used.
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
        extrn   __favor:byte
        extrn   __ImageBase:byte
        extrn   __memcpy_nt_iters:qword     ; defined in cpu_disp.c

__FAVOR_ENFSTRG equ 1

; Code for copying block using enhanced fast strings.
; This code needs to be in a separate routine because
; it uses non-volatile registers which must be saved
; and restored for exception handling.

        NESTED_ENTRY memcpy_repmovs, _TEXT

        push_reg rdi
        push_reg rsi
        .endprolog

        mov     rax, r11                ; return original destination pointer
        mov     rdi, rcx                ; move destination pointer to rdi
        mov     rcx, r8                 ; move length to rcx
        mov     rsi, r10                ; move source pointer to rsi
        rep     movsb                   ; copy source to destination buffer

        .beginepilog
        pop     rsi
        pop     rdi
        ret                             ; return

        NESTED_END memcpy_repmovs, _TEXT


; Main memmove/memcpy routine


        public memmove

        LEAF_ENTRY_ARG3 memcpy, _TEXT, dst:ptr byte, src:ptr byte, count:dword

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        memmove = memcpy

        mov     r11, rcx                ; save destination address
        mov     r10, rdx                ; save source address
        cmp     r8, 16                  ; if 16 bytes or less
        jbe     MoveBytes16             ; go move them quick
        cmp     r8, 32                  ; check for length <= 32 (we know its > 16)
        jbe     Move17to32              ; go handle lengths 17-32 as a special case
        sub     rdx, rcx                ; compute offset to source buffer
        jae     CopyUp                  ; if above or equal, go move up
        lea     rax, [r8+r10]           ; else check that src+count < dst
        cmp     rcx, rax                ; (src + count) < dst
        jb      CopyDown                ; no, buffers overlap go move downward

 CopyUp:
        cmp     r8, 128
        jbe     XmmCopySmall

        test    __favor, 1 SHL __FAVOR_ENFSTRG ; check for ENFSTRG (enhanced fast strings)
        jz      XmmCopyUp               ; If Enhanced Fast String not available, use XMM
        jmp     memcpy_repmovs


; Handle lengths 17-32 as a special case using XMM registers.
; This allows the regular code to assume that there will always be enough
; bytes for the "deferred" block of 16. Also any case that can be handled
; with just two stores is handled with just two stores, the regular code
; will always do 3 stores for unaligned moves that have a remainder.
; No assumptions are made here about buffer alignment or overlap.
; We load the entire string to be moved in 2 xmm registers before storing
; anything, so this works for any arrangement of overlapping buffers.
;
; dst is in rcx (can modify) and r11 (must preserve for return value)
; src is in r10 (should preserve for consistency)
; rdx is the offset from the dst to the source, so rcx + rdx is the src
; r8 is the length, and is known to be 17 <= r8 <= 32
;
; When length < 32 the first 16 bytes includes some of the last 16 bytes
; and we will store (length - 32) bytes twice. (E.g. in the worst case
; of len 17 we are storing the middle 15 bytes of the buffer twice).
; This is still much faster than doing logic and branching with 1, 2, 4
; and 8 byte conditional copies.
;
        align   16

Move17to32:
        movups  xmm0, [rdx]             ; load first 16 bytes of src
        movups  xmm1, (-16)[rdx + r8]   ; load last 16 bytes of src
        movups  [rcx], xmm0             ; store first 16 bytes of dst
        movups  (-16)[rcx + r8], xmm1   ; store last 16 bytes of dst
        mov     rax, rcx                ; set destination address
        ret

;
; Move residual bytes.
;

        align   16

MoveBytes16:
         mov    rax, rcx                ; mov destination address to rax
         lea    r9, OFFSET __ImageBase
IFDEF _VCRUNTIME_BUILD_QSPECTRE
         and    r8, 1Fh                  ; bound r8 to 0-31 in speculation scenarios (17-31 is padding)
ENDIF
         mov    ecx, [(IMAGEREL  MoveSmall) + r9 +r8*4]
         add    rcx, r9
         jmp    rcx

_TEXT    ends

_RDATA   segment read para 'DATA'

MoveSmall dd         IMAGEREL MoveSmall0
          dd         IMAGEREL MoveSmall1
          dd         IMAGEREL MoveSmall2
          dd         IMAGEREL MoveSmall3
          dd         IMAGEREL MoveSmall4
          dd         IMAGEREL MoveSmall5
          dd         IMAGEREL MoveSmall6
          dd         IMAGEREL MoveSmall7
          dd         IMAGEREL MoveSmall8
          dd         IMAGEREL MoveSmall9
          dd         IMAGEREL MoveSmall10
          dd         IMAGEREL MoveSmall11
          dd         IMAGEREL MoveSmall12
          dd         IMAGEREL MoveSmall13
          dd         IMAGEREL MoveSmall14
          dd         IMAGEREL MoveSmall15
          dd         IMAGEREL MoveSmall16
IFDEF _VCRUNTIME_BUILD_QSPECTRE
          dd 15 dup (IMAGEREL MoveSmall0) ; 17 -> 31 padding
ENDIF

_RDATA  ends

_TEXT   segment para 'CODE'

        align   16

MoveSmall0::
        ret

MoveSmall2::
        movzx   ecx, word ptr [rdx]     ; get two bytes from source
        mov     [rax], cx               ; write two bytes to destination
        ret

MoveSmall8::
        mov     rcx, qword ptr [rdx]    ; get eight bytes from source
        mov     [rax], rcx              ; write eight bytes to destination
        ret

MoveSmall3::
        movzx   ecx, word ptr [rdx]     ; get two bytes from source
        movzx   r8d, byte ptr 2[rdx]    ; get last byte from source
        mov     [rax], cx               ; write two bytes to destination
        mov     2[rax], r8b             ; write last byte to destination
        ret

MoveSmall1::
        movzx   ecx, byte ptr [rdx]     ; get byte from source
        mov     [rax], cl               ; write byte to destination
        ret

MoveSmall16::
        movdqu  xmm0, xmmword ptr [rdx] ; get sixteen bytes from source
        movdqu  xmmword ptr [rax], xmm0 ; write sixteen bytes to destination
        ret

        align   16
MoveSmall11::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        movzx   ecx, word ptr 8[rdx]    ; get two bytes from source
        movzx   r9d, byte ptr 10[rdx]   ; get last byte from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], cx              ; write two bytes to destination
        mov     10[rax], r9b            ; write last byte to destination
        ret

MoveSmall4::
        mov     ecx, dword ptr [rdx]    ; get four bytes from source
        mov     [rax], ecx              ; write four bytes to destination
        ret

        align   16
MoveSmall5::
        mov     ecx, dword ptr [rdx]    ; get four bytes from source
        movzx   r8d, byte ptr 4[rdx]    ; get last byte from source
        mov     [rax], ecx              ; write four bytes to destination
        mov     4[rax], r8b             ; write last byte to destination
        ret

        align   16
MoveSmall6::
        mov     ecx, dword ptr [rdx]    ; get four bytes from source
        movzx   r8d, word ptr 4[rdx]    ; get two bytes from source
        mov     [rax], ecx              ; write four bytes to destination
        mov     4[rax], r8w             ; write two bytes to destination
        ret

        align   16
MoveSmall7::
        mov     ecx, dword ptr [rdx]    ; get four bytes from source
        movzx   r8d, word ptr 4[rdx]    ; get two bytes from source
        movzx   r9d, byte ptr 6[rdx]    ; get last byte from source
        mov     [rax], ecx              ; write four bytes to destination
        mov     4[rax], r8w             ; write two bytes to destination
        mov     6[rax], r9b             ; write last byte to destination
        ret

MoveSmall13::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        mov     ecx, dword ptr 8[rdx]   ; get four bytes from source
        movzx   r9d, byte ptr 12[rdx]   ; get last bytes from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], ecx             ; write four bytes  to destination
        mov     12[rax], r9b            ; write last byte to destination
        ret

        align   16
MoveSmall9::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        movzx   ecx, byte ptr 8[rdx]    ; get last byte from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], cl              ; write last byte to destination
        ret

        align   16
MoveSmall10::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        movzx   ecx, word ptr 8[rdx]    ; get two bytes from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], cx              ; write two bytes to destination
        ret

        align   16
MoveSmall12::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        mov     ecx, dword ptr 8[rdx]   ; get four bytes from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], ecx             ; write four bytes to destination
        ret

        align   16
MoveSmall14::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        mov     ecx, dword ptr 8[rdx]   ; get four bytes from source
        movzx   r9d, word ptr 12[rdx]   ; get two bytes from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], ecx             ; write four bytes to destination
        mov     12[rax], r9w            ; write two bytes to destination
        ret

        align   16
MoveSmall15::
        mov     r8, qword ptr [rdx]     ; get eight bytes from source
        mov     ecx, dword ptr 8[rdx]   ; get four bytes from source
        movzx   r9d, word ptr 12[rdx]   ; get two bytes from source
        movzx   r10d, byte ptr 14[rdx]  ; get last byte from source
        mov     [rax], r8               ; write eight bytes to destination
        mov     8[rax], ecx             ; write four bytes to destination
        mov     12[rax], r9w            ; write two bytes to destination
        mov     14[rax], r10b           ; write last byte to destination
        ret

;
; Memcpy up using SSE instructions.
;
; Preconditions:
;       destination in rcx (destructable) and r11 (must preserve for return value)
;       source in r10
;       length in r8, must be greater than 16
;       offset from dest to src in rdx
;       source addr > dest addr or else buffers don't overlap
;
; Aligned stores are much faster on AMD hardware, so start by moving however many
; bytes must be moved so updated dst is 16-byte aligned. We need to copy
; (16 - (dest mod 16)) bytes, but it's faster to just do an unaligned copy of 16
; bytes and then start the aligned loop as usual at ((dest - (dest mod 16)) + 16).
; This results in (dest mod 16) bytes being copied twice. This is a lot faster
; than a bunch of code to copy maybe 1 then maybe 2 then maybe 4 then maybe 8
; bytes to achieve dst alignement.
;
; We know the src address is greater than the dst, but not by how much. In the
; case where the difference is less than 16 we must be careful about the bytes
; that will be stored twice. We must do both loads before either store, or the
; second load of those bytes will get the wrong values. We handle this by
; loading the last 16 bytes that can be stored at an aligned address, but
; deferring the store of those bytes to the remainder code, so it can load the
; remainder before storing the deferred bytes. Since either or both of the two
; loops can be skipped, the preconditions needed by the remainder  code must
; also apply to the loops. These conditions are:
;  - r8 is the count remaining, not including the deferred bytes
;  - [rcx + rdx] and [rcx] as usual point to the src and dst where the number
;    number of bytes given by r8 should be copied from and to.
;  - xmm0 holds the 16 deferred bytes that need to be stored at (-16)[rcx]
;
        align   16
XmmCopyUp:
        movups  xmm0, [rcx + rdx]       ; load deferred bytes
        add     r8, rcx                 ; r8 points 1 byte past end
        add     rcx, 16                 ; update to next block.
        test    r11b, 15                ; test if destination aligned
        jz      XmmCopyLargeTest        ; go try 128-byte blocks
;
; Move alignment bytes.
;
XmmCopyAlign:
        movaps  xmm1, xmm0              ; save initial bytes in xmm1
        and     rcx, -16                ; rcx is 16 bytes past first 16-byte align point
        movups  xmm0, [rcx + rdx]       ; load aligned deferred-store bytes
        add     rcx, 16                 ; update to next block
        movups  [r11], xmm1             ; now safe to store 16 unaligned at start
;
; See if we can move any 128-byte blocks.
;
XmmCopyLargeTest:
        sub     r8, rcx                 ; r8 restored to count remaining
        mov     r9, r8                  ; copy count of bytes remaining
        shr     r9, 7                   ; compute number of 128-byte blocks
        jz      XmmCopySmallTest        ; if z jump around to 2nd loop
        movaps  (-16)[rcx], xmm0        ; going into 1st loop, ok to store deferred bytes
        cmp     r9, __memcpy_nt_iters   ; threshold defined by cpu_disp.c
        jna     short XmmCopyLargeInner ; jump into 1st loop
        jmp     XmmCopyLargeInnerNT     ; long enough so non-temporal worth it, jump into nt loop

;
; Move 128-byte blocks
;
        align   16
;
; When possible, non-mov instructions are put between a load and store
; so their execution can overlap the store.
; The jnz is likewise moved earlier to come before the last store pair.
; Pairs of loads/stores are used to overlap cache latencies.
; movups and movaps are equally fast on aligned storage, we use movaps
; to document movs that we *know* are going to be aligned, movups otherwise.
; xmm0 must be preloaded before jumping into this loop, and the last
; store must be deferred (and the bytes to store left in xmm0) for the
; following loop and/or the remainder code.
;
XmmCopyLargeOuter:
        movaps  (-32)[rcx], xmm0        ; store 7th chunk from prior iteration
        movaps  (-16)[rcx], xmm1        ; store 8th chunk from prior iteration
XmmCopyLargeInner:                      ; enter loop here with xmm0 preloaded.
        movups  xmm0, [rcx + rdx]       ; load first 16 byte chunk
        movups  xmm1, 16[rcx + rdx]     ; load 2nd 16 byte chunk
        add     rcx, 128                ; advance destination address
        movaps  (-128)[rcx], xmm0       ; store first 16 byte chunk
        movaps  (-112)[rcx], xmm1       ; store 2nd 16 byte chunk
        movups  xmm0, (-96)[rcx + rdx]  ; load 3rd chunk
        movups  xmm1, (-80)[rcx + rdx]  ; load 4th chunk
        dec     r9                      ; dec block counter (set cc for jnz)
        movaps  (-96)[rcx], xmm0        ; store 3rd chunk
        movaps  (-80)[rcx], xmm1        ; store 4th chunk
        movups  xmm0, (-64)[rcx + rdx]  ; load 5th chunk
        movups  xmm1, (-48)[rcx + rdx]  ; load 6th chunk
        movaps  (-64)[rcx], xmm0        ; store 5th chunk
        movaps  (-48)[rcx], xmm1        ; store 6th chunk
        movups  xmm0, (-32)[rcx + rdx]  ; load 7th chunk
        movups  xmm1, (-16)[rcx + rdx]  ; load 8th chunk
        jnz     XmmCopyLargeOuter       ; loop if more blocks

XmmCopyFinish:                          ; non-temporal codepath rejoins here
        movaps  (-32)[rcx], xmm0        ; store 7th chunk from final iteration
        and     r8, 127                 ; compute remaining byte count
        movaps  xmm0, xmm1              ; 8th chunk becomes deferred bytes
        jmp     XmmCopySmallTest

XmmCopySmall:
        movups  xmm0, [rcx + rdx]       ; load deferred bytes
        add     rcx, 16
        sub     r8, 16
;
; See if we have any 16-byte blocks left to move
;
XmmCopySmallTest:
        mov     r9, r8                  ; copy count of bytes remaining
        shr     r9, 4                   ; compute number of 16-byte blocks
        jz      short XmmCopyTrail      ; on z, no 16-byte blocks, skip 2nd loop

        align   16

XmmCopySmallLoop:
        movups  (-16)[rcx], xmm0        ; the first time through this is the
                                        ; store of the deferred bytes from above
        movups  xmm0, [rcx + rdx]       ; load a block
        add     rcx, 16                 ; advance dest addr (store is deferred)
        dec     r9
        jnz     XmmCopySmallLoop

XmmCopyTrail:
        and     r8, 15                  ; compute remaining byte count
        jz      short XmmCopyReturn     ; if z, no remainder bytes to move
;
; Handle remainder bytes.
;
; As at the start, we are going to do an unaligned copy of 16 bytes which will double-write
; some bytes.  We must not touch rcx or xmm0 because they have what we need to store the
; deferred block. We use rax to point to the first byte after the end of the buffer and
; back up from there. Note rax is pointing to an address we must not read or write!
;
        lea     rax, [rcx+r8]           ; make rax point one past the end
        movups  xmm1, (-16)[rax + rdx]  ; load last 16 bytes of source buffer
        movups  (-16)[rax], xmm1        ; write last 16 bytes, including 16-r8 bytes
                                        ; from the last aligned block which we are about to
                                        ; overstore with identical values
XmmCopyReturn:
        movups  (-16)[rcx], xmm0        ; store the last deferred aligned block
        mov     rax, r11                ; we must return the original destination address
        ret                             ;

;
; Move 128-byte blocks non-temporal
;
        align   16
;
; non-temporal is exactly the same as the regular xmm loop above, except the movaps
; stores are movntps and we use prefetchnta. We are prefetching in two places, each
; prefetch gets 64 bytes about half an iteration ahead of time (about 10 instructions
; lead time). When we come to the end of the memcpy, we'll be prefetching bytes
; beyond the buffer we need to copy from, which may not be valid bytes. This is
; not illegal; if the memory address is invalid it does not trap, the hardware treats
; illegal prefetches as nops.
;

XmmCopyLargeOuterNT:
        movntps (-32)[rcx], xmm0        ; store 7th chunk from prior iteration
        movntps (-16)[rcx], xmm1        ; store 8th chunk from prior iteration
XmmCopyLargeInnerNT:                    ; enter loop here with xmm0 preloaded.
        prefetchnta [rcx + rdx + 512]   ; prefetch several cache lines ahead
        movups  xmm0, [rcx + rdx]       ; load first 16 byte chunk
        movups  xmm1, 16[rcx + rdx]     ; load 2nd 16 byte chunk
        add     rcx, 128                ; advance destination address
        movntps (-128)[rcx], xmm0       ; store first 16 byte chunk
        movntps (-112)[rcx], xmm1       ; store 2nd 16 byte chunk
        movups  xmm0, (-96)[rcx + rdx]  ; load 3rd chunk
        movups  xmm1, (-80)[rcx + rdx]  ; load 4th chunk
        dec     r9                      ; dec block counter (set cc for jnz)
        movntps (-96)[rcx], xmm0        ; store 3rd chunk
        movntps (-80)[rcx], xmm1        ; store 4th chunk
        movups  xmm0, (-64)[rcx + rdx]  ; load 5th chunk
        movups  xmm1, (-48)[rcx + rdx]  ; load 6th chunk
        prefetchnta [rcx + rdx + 576]   ; prefetch several cache lines ahead
        movntps (-64)[rcx], xmm0        ; store 5th chunk
        movntps (-48)[rcx], xmm1        ; store 6th chunk
        movups  xmm0, (-32)[rcx + rdx]  ; load 7th chunk
        movups  xmm1, (-16)[rcx + rdx]  ; load 8th chunk
        jnz     XmmCopyLargeOuterNT     ; loop if more blocks

        sfence
        jmp     XmmCopyFinish           ; rejoin regular memcpy codepath

;
; The source address is less than the destination address.
;

        align   16
;
; Move bytes down using SSE registers. The source address is less than
; the destination address and the buffers overlap. We will do everything back-to-front.
;
; Preconditions:
;       destination is r11 (must preserve for return value) and rcx
;       source in r10 (must preserve for remainder move)
;       length in r8, must have been verified to be greater than 16
;       offset from dest to src in rdx
;       source addr < dest addr and the buffers overlap
;
CopyDown:
        add     rcx, r8                 ; make rcx point one past the end of the dst buffer
        movups  xmm0, -16[rcx + rdx]    ; load deferred bytes
        sub     rcx, 16                 ; reduce dst addr
        sub     r8, 16                  ; r8 -= 16 in case aligned

;
; Aligned stores using movaps or movups are faster on AMD hardware than unaligned
; stores using movups. To achieve 16-byte dest alignment, we do an unaligned move
; of the last 16 bytes of the buffers, then reduce rcx only by the amount necessary
; to achieve alignment. This results in some bytes getting copied twice, unless we're
; already aligned.
;
; We know the src address is less than the dst, but not by exactly how much. In the
; case where the difference is less than 16 we must be careful about the bytes
; that will be stored twice. We must do both loads before either store, or the
; second load of those bytes will get the wrong values. We handle this by
; deferring the store of 16 aligned bytes to the remainder code, so it can load the
; remainder before storing the deferred bytes. Since either or both of the two
; loops can be skipped, the preconditions needed by the remainder  code must
; also apply to the loops. These conditions are:
;  - r8 is the count remaining, not including the deferred bytes
;  - [rcx] points one past the end of the remainder bytes
;  - rdx is the offset from the dst to the source
;  - xmm0 holds the 16 deferred bytes that need to be stored at [rcx]
;
        test    cl, 15                  ; test if dest aligned
        jz      XmmMovLargeTest         ; go try 128-byte blocks
;
; Move alignment bytes.
;
XmmMovAlign:
        mov     rax, rcx                ; save unaligned store address
        and     rcx, -16                ; rcx is deferred store address
        movups  xmm1, xmm0              ; copy unaligned last bytes to xmm1
        movups  xmm0, [rcx + rdx]       ; load deferred-store bytes
        movups  [rax], xmm1             ; now safe to do unaligned store
        mov     r8, rcx                 ; easier to recalc r8 using rcx-r11 ...
        sub     r8, r11                 ; ... than calc how much to subtract from r8

;
; See if we can move any 128-byte blocks.
;
XmmMovLargeTest:
        mov     r9, r8                  ; copy count of bytes remaining
        shr     r9, 7                   ; compute number of 128-byte blocks
        jz      short XmmMovSmallTest   ; if z jump around to 2nd loop
        movaps  [rcx], xmm0             ; going into 1st loop, ok to store deferred bytes
        jmp     short XmmMovLargeInner  ; jump into 1st loop
;
; Move 128-byte blocks
;
        align   16

XmmMovLargeOuter:
        movaps  (128-112)[rcx], xmm0    ; store 7th chunk from prior iteration
        movaps  (128-128)[rcx], xmm1    ; store 8th chunk from prior iteration
XmmMovLargeInner:
        movups  xmm0, (-16)[rcx + rdx]      ; load first 16 byte chunk
        movups  xmm1, (-32)[rcx + rdx]      ; load 2nd 16 byte chunk
        sub     rcx, 128                    ; reduce destination address
        movaps  (128-16)[rcx], xmm0         ; store first 16 byte chunk
        movaps  (128-32)[rcx], xmm1         ; store 2nd 16 byte chunk
        movups  xmm0, (128-48)[rcx + rdx]   ; load 3rd chunk
        movups  xmm1, (128-64)[rcx + rdx]   ; load 4th chunk
        dec     r9                          ; dec block counter (set cc for jnz)
        movaps  (128-48)[rcx], xmm0         ; store 3rd chunk
        movaps  (128-64)[rcx], xmm1         ; store 4th chunk
        movups  xmm0, (128-80)[rcx + rdx]   ; load 5th chunk
        movups  xmm1, (128-96)[rcx + rdx]   ; load 6th chunk
        movaps  (128-80)[rcx], xmm0         ; store 5th chunk
        movaps  (128-96)[rcx], xmm1         ; store 6th chunk
        movups  xmm0, (128-112)[rcx + rdx]  ; load 7th chunk
        movups  xmm1, (128-128)[rcx + rdx]  ; load 8th chunk
        jnz     short XmmMovLargeOuter      ; loop if more blocks

        movaps  (128-112)[rcx], xmm0    ; store 7th chunk from final iteration
        and     r8, 127                 ; compute remaining byte count
        movaps  xmm0, xmm1              ; 8th chunk becomes deferred bytes
;
; See if we have any 16-byte blocks left to move
;
XmmMovSmallTest:
        mov     r9, r8                  ; copy count of bytes remaining
        shr     r9, 4                   ; compute number of 16-byte blocks
        jz      short XmmMovTrailing    ; if z, no 16-byte blocks

        align   16

XmmMovSmallLoop:
        movups  [rcx], xmm0             ; the first time through this is the
                                        ; store of the deferred bytes from above
        sub     rcx, 16                 ; reduce dest addr
        movups  xmm0, [rcx + rdx]       ; load a block
        dec     r9
        jnz     XmmMovSmallLoop

XmmMovTrailing:
        and     r8, 15                  ; compute remaining byte count
        jz      short XmmMovReturn      ; if z, no residual bytes to move

;
; Handle remainder bytes.
;
; As at the start, we are going to do an unaligned copy of 16 bytes which will double-write
; some bytes.  We must not touch rcx or xmm0 because they have what we need to store the
; deferred block. But unlike for mcpyxmm code above, we have r10 and r11 we can just use
; to copy the lowest 16 bytes.
;
        movups  xmm1, [r10]             ; load lowest 16 bytes, which includes remainder
        movups  [r11], xmm1             ; store lowest 16 bytes, which includes remainder

XmmMovReturn:
        movups  [rcx], xmm0             ; store deferred bytes
        mov     rax, r11                ; we must return destination address
        ret

        LEAF_END memcpy, _TEXT

        end
