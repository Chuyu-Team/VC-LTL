        page    ,132
        title   memcpy - Copy source memory bytes to destination
;***
;memcpy.asm - contains memcpy and memmove routines
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       memcpy() copies a source memory buffer to a destination buffer.
;       memmove() copies a source memory buffer to a destination buffer.
;       Overlapping buffers are treated specially, to avoid propagation.
;
;Entry:
;       void *dst (rcx) - pointer to destination buffer
;       const void *src (rdx) - pointer to source buffer
;       size_t count (r8) - number of bytes to copy
;
;Exit:
;       Returns a pointer to the destination buffer (rax)
;
;*******************************************************************************


include ksamd64.inc
        subttl  "memcpy"

; the code implementing memory copy via rep movsb (enhanced strings)
; needs to be in a separate routine to be able to generate EH records
; that can used to restore non-volatile register value during EH unwinding

        NESTED_ENTRY memcpy_repmovs, _TEXT

        push_reg rdi
        push_reg rsi
        .endprolog

        mov      rdi, rcx                                        ; copy arguments into rep movsb inputs and execute the instruction
        mov      rsi, rdx
        mov      rcx, r8
        rep      movsb

        .beginepilog
        pop      rsi
        pop      rdi
        ret

        NESTED_END memcpy_repmovs, _TEXT

;*******************************************************************************
;        Main memmove/memcpy routine implementation
;
;    the implementation follows this approach:
;   1. for blocks less than 16 bytes in size use loads and stores through general registers
;   2. for blocks 16-32 bytes in size use two (possibly overlapping) loads and stores through SSE registers
;   3. for blocks 33-8K bytes in size use AVX (if available) or SSE loads and stores
;   4. for blocks 8K+1-256K bytes in size use enhanced strings (if available),
;      if not continue using AVX or SSE loads and stores
;   5. for blocks greater than 256K size use non-temporal AVX stores (if available), or enhanced strings (if available),
;      or SSE loads and stores.
;

        __FAVOR_ENFSTRG equ 1
        __ISA_AVAILABLE_AVX equ 3
        KB equ 1024

        public memmove

        LEAF_ENTRY_ARG3 memcpy, _TEXT, dst:ptr byte, src:ptr byte, count:dword

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        memmove = memcpy

        extrn    __favor:byte
        extrn    __ImageBase:byte
        extrn    __isa_available:dword

        mov      rax, rcx                                        ; pre-set the return value
        lea      r10, OFFSET __ImageBase                         ; pre-set the image base to be used in table based branching
        cmp      r8, 15                                          ; dispatch to code handling block sizes of 16 bytes or more
        ja       MoveAbove15                                     ; fall-thru to code handling length of 15 bytes or less

;
;   move blocks of less than 16 bytes in length
;
;   Entry:
;      r8 - length < 16
;      rcx - destination block address
;      rdx - source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the block length as an index
;      into the table of code labels where each entry points to the code block implementing the copy of this particular length
;      each block implements memory copy using the appropriate combination of loads and stores through general registers
;

        align    16
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r8, 0Fh
ENDIF
        mov      r9d, [(IMAGEREL MoveSmall) + r10 + r8*4]        ; load the appropriate code label RVA from the table (r9) using the length (r8) as an index
        add      r9, r10                                         ; convert the RVA into the VA
        jmp      r9                                              ; dispatch to the code
MoveSmall0::
        ret

        align    16
MoveSmall15::
        mov      r8, QWORD PTR [rdx]                             ; handle 15 bytes (8+4+2+1)
        mov      ecx, DWORD PTR (8)[rdx]
        movzx    r9d, WORD PTR (12)[rdx]
        movzx    r10d, BYTE PTR (14)[rdx]
        mov      [rax], r8
        mov      (8)[rax], ecx
        mov      (12)[rax], r9w
        mov      (14)[rax], r10b
        ret
MoveSmall11::
        mov      r8, QWORD PTR [rdx]                             ; handle 11 bytes (8+2+1)
        movzx    ecx, WORD PTR (8)[rdx]
        movzx    r9d, BYTE PTR (10)[rdx]
        mov      [rax], r8
        mov      (8)[rax], cx
        mov      (10)[rax], r9b
        ret
MoveSmall2::
        movzx    ecx, WORD PTR [rdx]                             ; handle 2 bytes (2)
        mov      [rax], cx
        ret

        align    16
MoveSmall7::
        mov      ecx, DWORD PTR [rdx]                            ; handle 7 bytes (4+2+1)
        movzx    r8d, WORD PTR (4)[rdx]
        movzx    r9d, BYTE PTR (6)[rdx]
        mov      [rax], ecx
        mov      (4)[rax], r8w
        mov      (6)[rax], r9b
        ret
MoveSmall14::
        mov      r8, QWORD PTR [rdx]                             ; handle 14 bytes (8+4+2)
        mov      ecx, DWORD PTR (8)[rdx]
        movzx    r9d, WORD PTR (12)[rdx]
        mov      [rax], r8
        mov      (8)[rax], ecx
        mov      (12)[rax], r9w
        ret
MoveSmall3::
        movzx    ecx, WORD PTR [rdx]                             ; handle 3 bytes (2+1)
        movzx    r8d, BYTE PTR (2)[rdx]
        mov      [rax], cx
        mov      (2)[rax], r8b
        ret

        align    16
MoveSmall13::
        mov      r8, QWORD PTR [rdx]                             ; handle 13 bytes (8+4+1)
        mov      ecx, DWORD PTR (8)[rdx]
        movzx    r9d, BYTE PTR (12)[rdx]
        mov      [rax], r8
        mov      (8)[rax], ecx
        mov      12[rax], r9b
        ret
MoveSmall10::
        mov      r8, QWORD PTR [rdx]                             ; handle 10 bytes (8+2)
        movzx    ecx, WORD PTR (8)[rdx]
        mov      [rax], r8
        mov      (8)[rax], cx
        ret
MoveSmall9::
        mov      r8, QWORD PTR [rdx]                             ; handle 9 bytes (8+1)
        movzx    ecx, BYTE PTR (8)[rdx]
        mov      [rax], r8
        mov      (8)[rax], cl
        ret
MoveSmall12::
        mov      r8, QWORD PTR [rdx]                             ; handle 12 bytes (8+4)
        mov      ecx, DWORD PTR (8)[rdx]
        mov      [rax], r8
        mov      (8)[rax], ecx
        ret

        align    16
MoveSmall6::
        mov      ecx, DWORD PTR [rdx]                            ; handle 6 bytes (4+2)
        movzx    r8d, WORD PTR (4)[rdx]
        mov      [rax], ecx
        mov      (4)[rax], r8w
        ret
MoveSmall5::
        mov      ecx, DWORD PTR [rdx]                            ; handle 5 bytes (4+1)
        movzx    r8d, BYTE PTR (4)[rdx]
        mov      [rax], ecx
        mov      (4)[rax], r8b
        ret
MoveSmall8::
        mov      rcx, QWORD PTR [rdx]                            ; handle 8 bytes (8)
        mov      [rax], rcx
        ret
MoveSmall1::
        movzx    ecx, BYTE PTR [rdx]                             ; handle 1 byte (1)
        mov      [rax], cl
        ret
MoveSmall4::
        mov      ecx, DWORD PTR [rdx]                            ; handle 4 bytes (4)
        mov      [rax], ecx
        ret

;
;   move blocks of 16 bytes or more in length
;
        align    16
MoveAbove15:
        cmp      r8, 32                                          ; dispatch to code handling blocks over 32 bytes in length
        ja       MoveAbove32                                     ; fall-through to code handling blocks between 16 and 32 in length

;
;   move blocks between 16 and 32 bytes in length
;
;   Entry:
;      r8 - length (16-32 bytes)
;      rcx - destination block address
;      rdx - source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented with two SSE loads and stores using XMM registers
;      the first 16-byte load/store moves the first 16 bytes beginning at the blocks' start addresses
;      the second 16-byte load/store moves the remaining bytes beginning at the adjusted address
;      with the load/store tail covering the remaining length
;
        movdqu   xmm1, XMMWORD PTR [rdx]
        movdqu   xmm2,  XMMWORD PTR (-16)[rdx + r8]
        movdqu   XMMWORD PTR [rcx], xmm1
        movdqu   XMMWORD PTR (-16)[rcx + r8], xmm2
        ret

;
;   move blocks of 32 bytes or more in length
;
MoveAbove32:
        cmp      rdx, rcx                                        ; if the source block address (rdx) is not lower than the destination block address (rcx) the copy can use the regular ("copy up")
        jae      CopyUp                                          ; fast implementation that proceeds from the lower to higher addresses, even if the blocks overlap

        lea      r9, [rdx + r8]                                  ; otherwise, if the source block address is lower than the destination block address we need to see if the blocks overlap,
        cmp      rcx, r9                                         ; i.e. if the destination block address (rcx) is lower than the source block end address (rdx + r8), if so the blocks overlap
        jb       CopyDown                                        ; in a way that requires the copy to use the special ("copy down") implementation that proceeds from higher to lower addresses

        align    16



;
;   regular ("copy up") fast implementation
;
CopyUp:
        cmp      __isa_available, __ISA_AVAILABLE_AVX            ; check if the HW supports wider AVX copies
        jb       NoAVX                                           ; if not, use the SSE based implementation


;
;      AVX based implementation
;

        __AVX_LEN_BIT equ 5
        __AVX_STEP_LEN equ 1 SHL __AVX_LEN_BIT
        __AVX_LOOP_LEN equ __AVX_STEP_LEN*8
        __FAST_STRING_AVX_THRESHOLD equ 8*KB
        __NT_AVX_THRESHOLD equ 1536*KB                           ; must be greater than or equal to __FAST_STRING_AVX_THRESHOLD

        cmp      r8, __FAST_STRING_AVX_THRESHOLD                 ; it's best to use AVX loads with regular stores to copy blocks up to a certain threshold (__FAST_STRING_AVX_THRESHOLD) in length
        jbe      MoveWithYMM                                     ; check the block length (r8) and dispatch to the regular AVX code

        cmp      r8, __NT_AVX_THRESHOLD                          ; it's best to use AVX loads with non-temporal stores to copy blocks above a certain threshold (__NT_AVX_THRESHOLD) in length
        ja       MoveWithYMM                                     ; check the block length (r8) and dispatch to the AVX code that will take care of dispatching to the non-temporal version

        test     __favor, 1 SHL __FAVOR_ENFSTRG                  ; it's best to use "fast string" rep movsb (if present) to copy blocks above __FAST_STRING_AVX_THRESHOLD and up to __NT_THRESHOLD in length
        jnz      memcpy_repmovs                                  ; if "fast string" rep movsb is not present fall-through to regular AVX code

;
;   move memory with AVX loads and stores
;
;   Entry:
;      r8 - length (> 32 bytes)
;      rcx - destination block address
;      rdx - source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is based on a loop with eight AVX loads and stores copying 256 bytes in an iteration with the remainder (or blocks themselves)
;      handled through a table based dispatch to the code that handles the remaining length with the appropriate number of AVX loads and stores
;

MoveWithYMM:
        vmovdqu  ymm0, YMMWORD PTR (-__AVX_STEP_LEN*0)[rdx]      ; save the first 32-byte chunk which may be overwritten while copying overlapping blocks with destination not 32-byte aligned
        vmovdqu  ymm5, YMMWORD PTR (-__AVX_STEP_LEN*1)[rdx + r8] ; save the last 32-byte chunk which may be overwritten by the next to last AVX store
        cmp      r8, __AVX_LOOP_LEN                              ; blocks up to the loop iteration size (256 bytes) go straight to the loop remainder handling code
        jbe      MovUpTo256WithYMM                               ; fall-through to the loop code for blocks greater than the loop iteration size (256 bytes)

        mov      r9, rcx                                         ; it is generally profitable to align the loop stores on the 32-byte boundary, this is done by shifting
        and      r9, __AVX_STEP_LEN-1                            ; the starting source (rdx) and destination (rcx) copy addresses by the number of bytes (complement r9) required
        sub      r9, __AVX_STEP_LEN                              ; to make the destination block aligned on the next 32-byte boundary and reducing the loop copy length accordingly,
        sub      rcx, r9                                         ; the skipped head bytes will be copied in the remainder handling code;
        sub      rdx, r9
        add      r8, r9
        cmp      r8, __AVX_LOOP_LEN                              ; compare the block copy length (which may have gotten shorter) to the loop iteration length,
        jbe      MovUpTo256WithYMM                               ; if it is less than it go to the loop remainder handling code

        cmp      r8, __NT_AVX_THRESHOLD                          ; blocks greater than a certain threshold are best handled with non-temporal stores
        ja       YmmLoopNT                                       ; for blocks up to this threshold fall-through to the regular AVX loop

;
;    move > 256 bytes with an AVX based loop
;
;   Entry:
;      r8 - length (> 256 bytes)
;      rcx - destination block address (32-byte aligned)
;      rdx - source block address
;   Exit:
;      r8 - remaining length (< 256 bytes)
;      rcx - remaining destination block address
;      rdx - remaining source block address
;      rax - destination block address
;
;   Implementation:
;      each iteration copies 256 bytes through two sets of 4 AVX loads and 4 AVX stores using YMM registers
;
       align     16
YmmLoop:
        vmovdqu  ymm1, YMMWORD PTR (__AVX_STEP_LEN*0)[rdx]       ; load the first set of 128 bytes from source
        vmovdqu  ymm2, YMMWORD PTR (__AVX_STEP_LEN*1)[rdx]
        vmovdqu  ymm3, YMMWORD PTR (__AVX_STEP_LEN*2)[rdx]
        vmovdqu  ymm4, YMMWORD PTR (__AVX_STEP_LEN*3)[rdx]
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*0)[rcx], ymm1       ; store the first set of 128 bytes to destination
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*1)[rcx], ymm2
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*2)[rcx], ymm3
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*3)[rcx], ymm4
        vmovdqu  ymm1, YMMWORD PTR (__AVX_STEP_LEN*4)[rdx]       ; load the second set of 128 bytes from source
        vmovdqu  ymm2, YMMWORD PTR (__AVX_STEP_LEN*5)[rdx]
        vmovdqu  ymm3, YMMWORD PTR (__AVX_STEP_LEN*6)[rdx]
        vmovdqu  ymm4, YMMWORD PTR (__AVX_STEP_LEN*7)[rdx]
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*4)[rcx], ymm1       ; store the second set of 128 bytes to destination
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*5)[rcx], ymm2
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*6)[rcx], ymm3
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*7)[rcx], ymm4
        add      rcx, __AVX_LOOP_LEN                             ; advance destination address
        add      rdx, __AVX_LOOP_LEN                             ; advance source address
        sub      r8, __AVX_LOOP_LEN                              ; adjust remaining length
        cmp      r8, __AVX_LOOP_LEN                              ; if at least 256 bytes remain to be copied do another iteration
        jae      YmmLoop                                         ; otherwise fall-through to the remainder handling code

;
;   move up to 256 bytes using AVX loads and stores
;
;   Entry:
;      r8 - length (<= 256 bytes)
;      rcx - remaining destination block address
;      rdx - remaining source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the number of remaining 32-byte copies as an index
;      into the table of staggered code labels where each entry points to the beginning code address that will handle the remaining length
;

MovUpTo256WithYMM:
        lea      r9, (__AVX_STEP_LEN-1)[r8]                      ; calculate the length of full 32-byte blocks (r9) required to handle the remaining length (r8), i.e. (0 for 0 length, 1 for 1-32,
        and      r9, -__AVX_STEP_LEN                             ; 2 for 33-64, etc.), this number will be used as a base to calculate the source and destination block addresses for remaining copies
        mov      r11, r9                                         ; also use this number to calculate the number of copies (r11) required to handle the remaining length
        shr      r11, __AVX_LEN_BIT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r11, 0Fh
ENDIF
        mov      r11d, [(IMAGEREL MoveSmallYmm) + r10 + r11*4]   ; load the appropriate code label RVA from the table using the number of remaining copies (r11) as an index
        add      r11, r10                                        ; convert the RVA into the VA
        jmp      r11                                             ; dispatch to the code

Mov8YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*8)[rdx + r9] ; entry point to perform 8 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*8)[rcx + r9], ymm1
Mov7YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*7)[rdx + r9] ; entry point to perform 7 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*7)[rcx + r9], ymm1
Mov6YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*6)[rdx + r9] ; entry point to perform 6 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*6)[rcx + r9], ymm1
Mov5YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*5)[rdx + r9] ; entry point to perform 5 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*5)[rcx + r9], ymm1
Mov4YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*4)[rdx + r9] ; entry point to perform 4 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*4)[rcx + r9], ymm1
Mov3YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*3)[rdx + r9] ; entry point to perform 3 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*3)[rcx + r9], ymm1
Mov2YmmBlocks::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*2)[rdx + r9] ; entry point to perform 2 remaining copies
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*2)[rcx + r9], ymm1
Mov1YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*1)[rcx + r8], ymm5 ; entry point to perform 1 remaining copy - store the previously saved last 32 byte chunk
Mov0YmmBlocks::
        vmovdqu  YMMWORD PTR [rax], ymm0                         ; store the previously saved first 32 byte chunk that may have been skipped by aligning the destination
        vzeroupper                                               ; clean up upper AVX
        ret

;
;   move > 256 bytes with an AVX based loop using non-temporal stores
;
;   Entry:
;      r8 - length (> 256 bytes)
;      rcx - destination block address (32-byte aligned)
;      rdx - source block address
;   Exit:
;      r8 - remaining length (< 256 bytes)
;      rcx - remaining destination block address
;      rdx - remaining source block address
;      rax - destination block address
;
;   Implementation:
;      each iteration copies 256 bytes through two sets of 4 AVX loads and 4 non-temporal AVX stores using YMM registers
;
       align   16
YmmLoopNT:
        vmovdqu  ymm1, YMMWORD PTR (__AVX_STEP_LEN*0)[rdx]       ; load the first set of 128 bytes from source
        vmovdqu  ymm2, YMMWORD PTR (__AVX_STEP_LEN*1)[rdx]
        vmovdqu  ymm3, YMMWORD PTR (__AVX_STEP_LEN*2)[rdx]
        vmovdqu  ymm4, YMMWORD PTR (__AVX_STEP_LEN*3)[rdx]
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*0)[rcx], ymm1       ; store the first set of 128 bytes to destination
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*1)[rcx], ymm2
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*2)[rcx], ymm3
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*3)[rcx], ymm4
        vmovdqu  ymm1, YMMWORD PTR (__AVX_STEP_LEN*4)[rdx]       ; load the second set of 128 bytes from source
        vmovdqu  ymm2, YMMWORD PTR (__AVX_STEP_LEN*5)[rdx]
        vmovdqu  ymm3, YMMWORD PTR (__AVX_STEP_LEN*6)[rdx]
        vmovdqu  ymm4, YMMWORD PTR (__AVX_STEP_LEN*7)[rdx]
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*4)[rcx], ymm1       ; store the second set of 128 bytes to destination
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*5)[rcx], ymm2
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*6)[rcx], ymm3
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*7)[rcx], ymm4
        add      rcx, __AVX_LOOP_LEN                             ; advance destination address
        add      rdx, __AVX_LOOP_LEN                             ; advance source address
        sub      r8, __AVX_LOOP_LEN                              ; adjust remaining length
        cmp      r8, __AVX_LOOP_LEN                              ; if at least 256 bytes remain to be copied do another iteration
        jae      YmmLoopNT                                       ; otherwise fall-through to the remainder handling code

;
;   move <= 256 bytes using YMM loads and non-temporal stores
;
;   Entry:
;      r8 - length (<= 256 bytes)
;      rcx - remaining destination block address (32-byte aligned)
;      rdx - remaining source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the number of remaining 32-byte copies as an index
;      into the table of staggered code labels where each entry points to the beginning address of code that will handle the remaining length
;
        lea      r9, (__AVX_STEP_LEN-1)[r8]                      ; calculate the length of full 32-byte blocks (r9) required to handle the remaining length (r8), i.e. (0 for 0 length, 1 for 1-32,
        and      r9, -__AVX_STEP_LEN                             ; 2 for 33-64, etc.), this number will be used as a base to calculate the source and destination block addresses for remaining copies
        mov      r11, r9                                         ; also use this number to calculate the number of copies (r11) required to handle the remaining length
        shr      r11, __AVX_LEN_BIT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r11, 0Fh
ENDIF
        mov      r11d, [(IMAGEREL MoveSmallYmmNT) + r10 + r11*4] ; load the appropriate code label RVA from the table using the number of remaining copies (r11) as an index
        add      r11, r10                                        ; convert the RVA into the VA
        jmp      r11                                             ; dispatch to the code

Mov8YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*8)[rdx + r9] ; entry point to perform 8 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*8)[rcx + r9], ymm1
Mov7YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*7)[rdx + r9] ; entry point to perform 7 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*7)[rcx + r9], ymm1
Mov6YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*6)[rdx + r9] ; entry point to perform 6 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*6)[rcx + r9], ymm1
Mov5YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*5)[rdx + r9] ; entry point to perform 5 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*5)[rcx + r9], ymm1
Mov4YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*4)[rdx + r9] ; entry point to perform 4 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*4)[rcx + r9], ymm1
Mov3YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*3)[rdx + r9] ; entry point to perform 3 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*3)[rcx + r9], ymm1
Mov2YmmBlocksNT::
        vmovdqu  ymm1, YMMWORD PTR (-__AVX_STEP_LEN*2)[rdx + r9] ; entry point to perform 2 remaining copies
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*2)[rcx + r9], ymm1
Mov1YmmBlocksNT::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*1)[rcx + r8], ymm5 ; entry point to perform 1 remaining copy - store the previously saved last 32 byte chunk (destination may be unaligned)
Mov0YmmBlocksNT::
        vmovdqu  YMMWORD PTR [rax], ymm0                         ; store the previously saved first 32 byte chunk (destination may be unaligned)
        sfence                                                   ; non-temporal stores require sfence to maintain memory semantics
        vzeroupper                                               ; clean up upper AVX
        ret

;
;      SSE based implementation
;

       align     16
NoAVX:
        __SSE_LEN_BIT equ 4
        __SSE_STEP_LEN equ 1 SHL __SSE_LEN_BIT
        __SSE_LOOP_LEN equ __SSE_STEP_LEN*8
        __FAST_STRING_SSE_THRESHOLD equ 2*KB

        cmp      r8, __FAST_STRING_SSE_THRESHOLD                 ; it's best to use SSE loads with regular stores to copy blocks up to a certain threshold (__FAST_STRING_SSE_THRESHOLD) in length
        jbe      MoveWithXMM                                     ; check the block length (r8) and dispatch to regular SSE code

        test     __favor, 1 SHL __FAVOR_ENFSTRG                  ; it's best to use "fast string" rep movsb (if present) to copy blocks above __FAST_STRING_SSE_THRESHOLD
        jnz      memcpy_repmovs                                  ; if "fast string" rep movsb is not present fall-through to regular SSE code

;
;   move memory with SSE loads and stores
;
;   Entry:
;      r8 - length (> 32 bytes)
;      rcx - destination block address
;      rdx - source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is based on a loop with eight SSE loads and stores copying 128 bytes in an iteration with the remainder (or blocks themselves)
;      handled through a table based dispatch to the code that handles the remaining length with the appropriate number of SSE loads and stores
;

MoveWithXMM:
        movdqu   xmm0, XMMWORD PTR (-__SSE_STEP_LEN*0)[rdx]      ; save the first 16-byte chunk which may be overwritten while copying overlapping blocks with destination not 16-byte aligned
        movdqu   xmm5, XMMWORD PTR (-__SSE_STEP_LEN*1)[rdx + r8] ; save the last 16-byte chunk which may be overwritten by the next to last SSE store
        cmp      r8, __SSE_LOOP_LEN                              ; blocks up to the loop iteration size (128 bytes) go straight to the loop remainder handling code
        jbe      MovUpTo128WithXMM                               ; fall-through to the loop code for blocks greater than the loop iteration size (128 bytes)

        mov      r9, rcx                                         ; it is generally profitable to align the loop stores on the 16-byte boundary, this is done by shifting
        and      r9, __SSE_STEP_LEN-1                            ; the starting source (rdx) and destination (rcx) copy addresses by the number of bytes (complement r9) required
        sub      r9, __SSE_STEP_LEN                              ; to make the destination block aligned on the next 16-byte boundary and reducing the loop copy length accordingly,
        sub      rcx, r9                                         ; the skipped head bytes will be copied in the remainder handling code;
        sub      rdx, r9
        add      r8, r9
        cmp      r8, __SSE_LOOP_LEN                              ; compare the block copy length (that may have gotten shorter) to the loop iteration length,
        jbe      MovUpTo128WithXMM                               ; if it is less than it go to the loop remainder handling code

;
;    move > 128 bytes with an AVX based loop
;
;   Entry:
;      r8 - length (> 128 bytes)
;      rcx - destination block address (16-byte aligned)
;      rdx - source block address
;   Exit:
;      r8 - remaining length (< 128 bytes)
;      rcx - remaining destination block address
;      rdx - remaining source block address
;      rax - destination block address
;
;   Implementation:
;      each iteration copies 128 bytes through two sets of 4 SSE loads and 4 SSE stores using XMM registers
;
        align    16
XmmLoop:
        movdqu   xmm1, XMMWORD PTR (__SSE_STEP_LEN*0)[rdx]       ; load the first set of 64 bytes from source
        movdqu   xmm2, XMMWORD PTR (__SSE_STEP_LEN*1)[rdx]
        movdqu   xmm3, XMMWORD PTR (__SSE_STEP_LEN*2)[rdx]
        movdqu   xmm4, XMMWORD PTR (__SSE_STEP_LEN*3)[rdx]
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*0)[rcx], xmm1       ; store the first set of 64 bytes to destination
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*1)[rcx], xmm2
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*2)[rcx], xmm3
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*3)[rcx], xmm4
        movdqu   xmm1, XMMWORD PTR (__SSE_STEP_LEN*4)[rdx]       ; load the second set of 64 bytes from source
        movdqu   xmm2, XMMWORD PTR (__SSE_STEP_LEN*5)[rdx]
        movdqu   xmm3, XMMWORD PTR (__SSE_STEP_LEN*6)[rdx]
        movdqu   xmm4, XMMWORD PTR (__SSE_STEP_LEN*7)[rdx]
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*4)[rcx], xmm1       ; store the second set of 64 bytes to destination
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*5)[rcx], xmm2
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*6)[rcx], xmm3
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*7)[rcx], xmm4
        add      rcx, __SSE_LOOP_LEN                             ; advance destination address
        add      rdx, __SSE_LOOP_LEN                             ; advance source address
        sub      r8, __SSE_LOOP_LEN                              ; adjust remaining length
        cmp      r8, __SSE_LOOP_LEN                              ; if at least 128 bytes remain to be copied do another iteration
        jae      XmmLoop                                         ; otherwise fall-through to the remainder handling code

;
;   move up to 128 bytes using SSE loads and stores
;
;   Entry:
;      r8 - length (<= 128 bytes)
;      rcx - remaining destination block address
;      rdx - remaining source block address
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the number of remaining 16-byte copies as an index
;      into the table of staggered code labels where each entry points to the beginning address of code that will handle the remaining length
;

MovUpTo128WithXMM:
        lea      r9, (__SSE_STEP_LEN-1)[r8]                      ; calculate the length of full 16-byte blocks (r9) required to handle the remaining length (r8), i.e. (0 for 0 length, 1 for 1-16
        and      r9, -__SSE_STEP_LEN                             ; 2 for 17-32, etc.), this number will be used as a base to calculate the source and destination block addresses for remaining copies
        mov      r11, r9                                         ; also use this number to calculate the number of copies (r11) required to handle the remaining length
        shr      r11, __SSE_LEN_BIT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r11, 0Fh
ENDIF
        mov      r11d, [(IMAGEREL  MoveSmallXmm) + r10 + r11*4]  ; load the appropriate code label RVA from the table using the number of remaining copies (r11) as an index
        add      r11, r10                                        ; convert the RVA into the VA
        jmp      r11                                             ; dispatch to the code

Mov8XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*8)[rdx + r9] ; entry point to perform 8 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*8)[rcx + r9], xmm1
Mov7XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*7)[rdx + r9] ; entry point to perform 7 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*7)[rcx + r9], xmm1
Mov6XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*6)[rdx + r9] ; entry point to perform 6 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*6)[rcx + r9], xmm1
Mov5XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*5)[rdx + r9] ; entry point to perform 5 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*5)[rcx + r9], xmm1
Mov4XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*4)[rdx + r9] ; entry point to perform 4 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*4)[rcx + r9], xmm1
Mov3XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*3)[rdx + r9] ; entry point to perform 3 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*3)[rcx + r9], xmm1
Mov2XmmBlocks::
        movdqu   xmm1, XMMWORD PTR (-__SSE_STEP_LEN*2)[rdx + r9] ; entry point to perform 2 remaining copies
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*2)[rcx + r9], xmm1
Mov1XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*1)[rcx + r8], xmm5 ; entry point to perform 1 remaining copy - store the previously saved last 16 byte chunk
Mov0XmmBlocks::
        movdqu   XMMWORD PTR [rax], xmm0                         ; store the previously saved first 16 byte chunk
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; memmove
;
; The source address is less than the destination address and the buffers overlap. Move bytes down using SSE registers.
;
;   Entry:
;      r8 - length (> 32 bytes)
;      rcx - destination block address
;      rdx - source block address
;   Exit:
;      rax - destination block address
;
        align    16
CopyDown:
        mov      r11, rcx
        mov      r10, rdx
        sub      rdx, rcx
        add      rcx, r8                 ; make rcx point one past the end of the dst buffer
        movups   xmm0, -16[rcx + rdx]    ; load deferred bytes
        sub      rcx, 16                 ; reduce dst addr
        sub      r8, 16                  ; r8 -= 16 in case aligned

;
; Aligned stores using movaps or movups are faster on AMD hardware than unaligned
; stores using movups. To achieve 16-byte dest alignment, we do an unaligned move
; of the last 16 bytes of the buffers, then reduce rcx only by the amount necessary
; to achieve alignment. This results in some bytes getting copied twice, unless we're
; already aligned.
;
; We know the src address is less than the dst, but not by exactly how much. In the
; case where the difference is less than 16, we must be careful about the bytes
; that will be stored twice. We must do both loads before either store, or the
; second load of those bytes will get the wrong values. We handle this by
; deferring the store of 16 aligned bytes to the remainder code, so it can load the
; remainder before storing the deferred bytes. Since either or both of the two
; loops can be skipped, the preconditions needed by the remainder code must
; also apply to the loops. These conditions are:
;  - r8 is the count remaining, not including the deferred bytes
;  - [rcx] points one past the end of the remainder bytes
;  - rdx is the offset from the dst to the source
;  - xmm0 holds the 16 deferred bytes that need to be stored at [rcx]
;
        test     cl, 15                  ; test if dest aligned
        jz       XmmMovLargeTest         ; go try 128-byte blocks
;
; Move alignment bytes.
;
XmmMovAlign:
        mov      rax, rcx                ; save unaligned store address
        and      rcx, -16                ; rcx is deferred store address
        movups   xmm1, xmm0              ; copy unaligned last bytes to xmm1
        movups   xmm0, [rcx + rdx]       ; load deferred-store bytes
        movups   [rax], xmm1             ; now safe to do unaligned store
        mov      r8, rcx                 ; easier to recalc r8 using rcx-r11 ...
        sub      r8, r11                 ; ... than calc how much to subtract from r8

;
; See if we can move any 128-byte blocks.
;
XmmMovLargeTest:
        mov      r9, r8                  ; copy count of bytes remaining
        shr      r9, 7                   ; compute number of 128-byte blocks
        jz       short XmmMovSmallTest   ; if z jump around to 2nd loop
        movaps   [rcx], xmm0             ; going into 1st loop, ok to store deferred bytes
        jmp      short XmmMovLargeInner  ; jump into 1st loop
;
; Move 128-byte blocks
;
        align    16

XmmMovLargeOuter:
        movaps   (128-112)[rcx], xmm0    ; store 7th chunk from prior iteration
        movaps   (128-128)[rcx], xmm1    ; store 8th chunk from prior iteration
XmmMovLargeInner:
        movups   xmm0, (-16)[rcx + rdx]      ; load 1st 16 byte chunk
        movups   xmm1, (-32)[rcx + rdx]      ; load 2nd 16 byte chunk
        sub      rcx, 128                    ; reduce destination address
        movaps   (128-16)[rcx], xmm0         ; store 1st 16 byte chunk
        movaps   (128-32)[rcx], xmm1         ; store 2nd 16 byte chunk
        movups   xmm0, (128-48)[rcx + rdx]   ; load 3rd chunk
        movups   xmm1, (128-64)[rcx + rdx]   ; load 4th chunk
        dec      r9                          ; dec block counter (set cc for jnz)
        movaps   (128-48)[rcx], xmm0         ; store 3rd chunk
        movaps   (128-64)[rcx], xmm1         ; store 4th chunk
        movups   xmm0, (128-80)[rcx + rdx]   ; load 5th chunk
        movups   xmm1, (128-96)[rcx + rdx]   ; load 6th chunk
        movaps   (128-80)[rcx], xmm0         ; store 5th chunk
        movaps   (128-96)[rcx], xmm1         ; store 6th chunk
        movups   xmm0, (128-112)[rcx + rdx]  ; load 7th chunk
        movups   xmm1, (128-128)[rcx + rdx]  ; load 8th chunk
        jnz      short XmmMovLargeOuter      ; loop if more blocks

        movaps   (128-112)[rcx], xmm0    ; store 7th chunk from final iteration
        and      r8, 127                 ; compute remaining byte count
        movaps   xmm0, xmm1              ; 8th chunk becomes deferred bytes
;
; See if we have any 16-byte blocks left to move
;
XmmMovSmallTest:
        mov      r9, r8                  ; copy count of bytes remaining
        shr      r9, 4                   ; compute number of 16-byte blocks
        jz       short XmmMovTrailing    ; if z, no 16-byte blocks

        align    16

XmmMovSmallLoop:
        movups   [rcx], xmm0             ; the first time through this is the
                                         ; store of the deferred bytes from above
        sub      rcx, 16                 ; reduce dest addr
        movups   xmm0, [rcx + rdx]       ; load a block
        dec      r9
        jnz      XmmMovSmallLoop

XmmMovTrailing:
        and      r8, 15                  ; compute remaining byte count
        jz       short XmmMovReturn      ; if z, no residual bytes to move

;
; Handle remainder bytes.
;
; As at the start, we are going to do an unaligned copy of 16 bytes which will double-write
; some bytes.  We must not touch rcx or xmm0 because they have what we need to store the
; deferred block. But unlike for mcpyxmm code above, we have r10 and r11 we can just use
; to copy the lowest 16 bytes.
;
        movups   xmm1, [r10]             ; load lowest 16 bytes, which includes remainder
        movups   [r11], xmm1             ; store lowest 16 bytes, which includes remainder

XmmMovReturn:
        movups   [rcx], xmm0             ; store deferred bytes
        mov      rax, r11                ; we must return destination address
        ret

        LEAF_END memcpy, _TEXT

        _RDATA   segment read para 'DATA'

;
;  Code address table for handling small (< 16 bytes) moves
;

MoveSmall       dd        IMAGEREL MoveSmall0
                dd        IMAGEREL MoveSmall1
                dd        IMAGEREL MoveSmall2
                dd        IMAGEREL MoveSmall3
                dd        IMAGEREL MoveSmall4
                dd        IMAGEREL MoveSmall5
                dd        IMAGEREL MoveSmall6
                dd        IMAGEREL MoveSmall7
                dd        IMAGEREL MoveSmall8
                dd        IMAGEREL MoveSmall9
                dd        IMAGEREL MoveSmall10
                dd        IMAGEREL MoveSmall11
                dd        IMAGEREL MoveSmall12
                dd        IMAGEREL MoveSmall13
                dd        IMAGEREL MoveSmall14
                dd        IMAGEREL MoveSmall15
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 0 dup (IMAGEREL MoveSmall0) ; padding
ENDIF

;
;  Code address table for handling small (<= 256 byte) moves using regular AVX stores
;

MoveSmallYmm    dd        IMAGEREL Mov0YmmBlocks
                dd        IMAGEREL Mov1YmmBlocks
                dd        IMAGEREL Mov2YmmBlocks
                dd        IMAGEREL Mov3YmmBlocks
                dd        IMAGEREL Mov4YmmBlocks
                dd        IMAGEREL Mov5YmmBlocks
                dd        IMAGEREL Mov6YmmBlocks
                dd        IMAGEREL Mov7YmmBlocks
                dd        IMAGEREL Mov8YmmBlocks
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 7 dup (IMAGEREL Mov0YmmBlocks) ; padding
ENDIF

;
;  Code address table for handling small (<= 256 byte) moves using non-temporal AVX stores
;

MoveSmallYmmNT  dd        IMAGEREL Mov0YmmBlocksNT
                dd        IMAGEREL Mov1YmmBlocksNT
                dd        IMAGEREL Mov2YmmBlocksNT
                dd        IMAGEREL Mov3YmmBlocksNT
                dd        IMAGEREL Mov4YmmBlocksNT
                dd        IMAGEREL Mov5YmmBlocksNT
                dd        IMAGEREL Mov6YmmBlocksNT
                dd        IMAGEREL Mov7YmmBlocksNT
                dd        IMAGEREL Mov8YmmBlocksNT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 7 dup (IMAGEREL Mov0YmmBlocks) ; padding
ENDIF

;
;  Code address table for handling small (<=128 byte) moves using regular SSE stores
;

MoveSmallXmm    dd        IMAGEREL Mov0XmmBlocks
                dd        IMAGEREL Mov1XmmBlocks
                dd        IMAGEREL Mov2XmmBlocks
                dd        IMAGEREL Mov3XmmBlocks
                dd        IMAGEREL Mov4XmmBlocks
                dd        IMAGEREL Mov5XmmBlocks
                dd        IMAGEREL Mov6XmmBlocks
                dd        IMAGEREL Mov7XmmBlocks
                dd        IMAGEREL Mov8XmmBlocks
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 7 dup  (IMAGEREL Mov0XmmBlocks) ; padding
ENDIF

        _RDATA  ends

        end
