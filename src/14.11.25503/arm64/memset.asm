;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;

#include "ksarm64.h"

        EXPORT memset [FUNC]

        AREA    |.text|,ALIGN=5,CODE,READONLY,CODEALIGN

;
; void * __cdecl memset (void *dst, int value, size_t count);
;
; On ARM64, if you actually have all the include paths and compiler-predefined
; defines correct and your crtdefs.h has been properly updated for ARM64,
; size_t will be unsigned __int64 rather than unsigned int.
;

; Sets the first count bytes of the block of memory pointed by dst to the
; specified value (interpreted as an unsigned char).

; The memset is done one of four ways based on length and the configuration
; variables memset_forceAlignThreshold and memset_nonTemporalThreshold
;  - Lengths 0 to 32 use a Jump Table
;  - Lengths 33 to <memset_forceAlignThreshold> use a loop that stores 32 bytes
;    per iteration using unaligned vector stores
;  - Lengths <memset_forceAlignThreshold> to <memset_nonTemporalThreshold> - 1
;    use an unaligned vector store to force alignment and then a loop that
;    stores
;    64 bytes per iteration using vector stores, followed by a  single 32-byte
;    vector store when from 33 to 63 bytes remain, followed by use of the Jump
;    Table for the final 1 to 32 bytes.
;  - Lengths of at least <memset_nonTemporalThreshold> use an unaligned vector
;    store to force alignment and then a loop that stores 32 bytes per
;    iteration using non-temporal vector stores, followed by use of the Jump
;    Table if there are 1 to 31 remaining bytes.
;
; Performance discussion: Generally, the limiting factor on longer memsets is
; the memory bandwitdh of the processor. The loop control (length decrements,
; pointer updates and conditional branches) can generally be pipelined by the
; processor's microarchitecture (the execution unit implementation) in such a
; way that they are fully overlapped by the stores and effectively add no
; latency. The cost of the instructions that need to be executed to get to the
; main store loop, however, can be significant if that code includes a number
; of conditional branches, because branch predictor  mis-predicts cost time
; equivalent to that of executing on the order of 100 sequential instructions
; or more. Understanding how the microarchitecture's branch predictor works on
; a given processor is thus key to organizing the flow of the code; significant
; gains can be made by flipping 'then' and 'else' branches if it is known that
; one is more likely. Unfortunately, the length of a given memset is fairly
; random, and enough instructions are executed in between calls to memset that
; much of the time the hardware branch predictor will have no history.
; Generally, shorter memsets are far more common than longer ones, so we can
; arrange the conditional branches so the shorter memset is on the path that is
; predicted when the branch predictor has no history. Beyond that, the most
; effecive technique is to minimize the number of conditional branches that
; must be executed on the way to the code that actually does the memset. This
; is why a jump table is generally preferred for short memsets over code that
; checks 'should we store 16? 8? 4? 2? 1?'. In the future, given a different
; target processor, it may be that this code can be improved by collapsing the
; 32-byte and 64-byte store loops into a single case, either always aligning or
; never aligning for all lengths up to the non-temporal threshold.
;
; Keeping loops small, rather than partially unrolling them, also permits
; hardware loop recognizers to optimize the power consumption of a loop by
; caching the stream of micro ops for the loop and replaying that for each
; iteration. While the loop replayer is active the instruction decode engine
; can be powered down. At the current state of technology development loop
; replayers are only a power optimization, they don't yet make the loops run
; faster. The point of partially unrolling loops in the past was to amortize
; the cost of loop control for a given iteration over more stores. With ARM64's
; ability to reduce loop control to just 2 instructions using post-index
; addressing, partial unrolling is no longer a win (barring microarchitecture
; hazards triggered by the current implementation).
;
; There is an advantage in aligning labels so the target of a conditional
; branch is on a 32-byte boundary. AMD branch predictor technology fetches and
; processes 32 bytes of instruction (8 instructions on ARM) at time. Each
; 32-byte 'chunk' is tracked separately, and assigned separate hardware
; resources. If there are too many conditional branches in a given chunk the
; hardware is less efficient and sometimes less accurate at predicting the
; later conditional branches in the chunk. When the loop top is not at the
; beginning of a chunk, if the chunk contains conditional branches ahead of
; and outside the loop, those conditional branches take up branch predictor
; resources during loop execution, and may degrade the predictor's handling of
; the conditional branch at the bottom of the loop (if the loop is small and
; fits in a single chunk, which is the ideal case). Instructions outside of,
; but ahead of, the loop also may cause the ends of larger loops to spill into
; an extra chunk, further increasing the resources (and power) consumed by the
; branch predictor. For labels that are not repeatedly branched to (in a given
; call to memset) this is less important, especially if the instructions ahead
; of the label in the same chunk are not conditional branches. This
; implementation has optimized for all this by viewing the disassembly,
; examining the offset (modulo 32) of branch targets, and using the ALIGN
; directive to force the loop labels to be aligned. The ALIGN directives have
; been written to eliminate any NOPs from the execution path. Code has also
; been rearranged to minimize the amount of padding and thus overall module
; size, which also affects performance and power consumption.

; =============================================================================
; Configuration variables

; The cost of forcing alignment will be about 24 cycles. The extra cost of an
; unaligned 4-vector store over an aligned 4-vector store is either going to be
; 1 or 4 cycles per store. So the memset must be long enough to require 24
; 4-vector stores to break even, at 64 bytes per vector store.

#define memset_forceAlignThreshold  1536
                                    ; 64 * 24, must be >= 64 or will crash

; Non-temporal typically becomes worth it when the length of the memset equals
; the size of the cache

#define memset_nonTemporalThreshold 0x80000
                                    ; 512 * 1024, values less than
                                    ;  memset_forceAlignThreshold have no
                                    ;  effect

; =============================================================================

        ; This version will not work for device memory because it does
        ; unaligned stores of vector registers. This version should work for
        ; regular memory, because all unaligned vector stores are of byte
        ; elements.

        LEAF_ENTRY memset, "", 5

        ; parameters
        ; x0 = dst      (never modified, used as the return value)
        ; w1 = value    (usually modified with the lowest byte replicated to
        ;               the other bytes)
        ; x2 = count    (modified)

        ; register usage in addition to above, no others used
        ; x8   scratch reg, used in various temporary calcuations
        ; x9   used as loop iteration count
        ; x10  copy of x0, updated as the memset progresses to point to start
        ;      of remaining buffer
        ; v16  value replicated to all bytes (also referenced as q16 and d16)
        ; v17  value replicated to all bytes (also referenced as q17)
        ; v18  value replicated to all bytes
        ; v19  value replicated to all bytes

        mov     x10, x0             ; copy dst because we must return it
                                    ;  unchanged in x0
        dup     v16.16b, w1         ; form repeated byte pattern in a simd reg
        mov     v17.16b, v16.16b
        mov     v18.16b, v16.16b
        mov     v19.16b, v16.16b

        cmp     x2, #32             ; Lengths 0 to 32 use a Jump Table
        bgt     SimdMemset

        ; Use jump table for short memsets (0-32 bytes) and remainder handling
        ; (1-31 bytes)

UseJumpTable
        fmov    x1, d16             ; copy repeated byte pattern back to w1/x1
                                    ;  for 2, 4, & 8-byte stores
        adr     x8, Table           ; point at jump table
        ldrb    w9, [x8, x2]        ; get offset to code
        add     x8, x8, x9, lsl #2  ; compute final destination
        br      x8

Table   dcb     (Memset_0     - Table) / 4
        dcb     (Memset_1     - Table) / 4
        dcb     (Memset_2     - Table) / 4
        dcb     (Memset_3     - Table) / 4
        dcb     (Memset_4     - Table) / 4
        dcb     (Memset_5     - Table) / 4
        dcb     (Memset_6     - Table) / 4
        dcb     (Memset_7     - Table) / 4
        dcb     (Memset_8     - Table) / 4
        dcb     (Memset_9     - Table) / 4
        dcb     (Memset_10    - Table) / 4
        dcb     (Memset_11    - Table) / 4
        dcb     (Memset_12    - Table) / 4
        dcb     (Memset_13    - Table) / 4
        dcb     (Memset_14    - Table) / 4
        dcb     (Memset_15    - Table) / 4
        dcb     (Memset_16    - Table) / 4
        dcb     (Memset_17_32 - Table) / 4                  ; 17
        dcb     (Memset_17_32 - Table) / 4                  ; 18
        dcb     (Memset_17_32 - Table) / 4                  ; 19
        dcb     (Memset_17_32 - Table) / 4                  ; 20
        dcb     (Memset_17_32 - Table) / 4                  ; 21
        dcb     (Memset_17_32 - Table) / 4                  ; 22
        dcb     (Memset_17_32 - Table) / 4                  ; 23
        dcb     (Memset_17_32 - Table) / 4                  ; 24
        dcb     (Memset_17_32 - Table) / 4                  ; 25
        dcb     (Memset_17_32 - Table) / 4                  ; 26
        dcb     (Memset_17_32 - Table) / 4                  ; 27
        dcb     (Memset_17_32 - Table) / 4                  ; 28
        dcb     (Memset_17_32 - Table) / 4                  ; 29
        dcb     (Memset_17_32 - Table) / 4                  ; 30
        dcb     (Memset_17_32 - Table) / 4                  ; 31
        dcb     (Memset_17_32 - Table) / 4                  ; 32

        ALIGN   32

; Stores are all done using offsets from the same starting address. Beginning
; of buffer or beginning of buffer remainder should be in x10, with length or
; remaining length in x2. Cascading fall-throughs reduce code size, eliminating
; otherwise repeated instructions.

Memset_15
        st1     {v16.8b}, [x10], #8                        ; store 8 
Memset_7
        st4     {v16.b,v17.b,v18.b,v19.b}[0], [x10], #4    ; store 4
        st3     {v16.b,v17.b,v18.b}[0], [x10]              ; store 3
        ret

Memset_14
        st1     {v16.8b}, [x10], #8                        ; store 8 
Memset_6
        st4     {v16.b,v17.b,v18.b,v19.b}[0], [x10], #4    ; store 4
Memset_2
        st2     {v16.b,v17.b}[0], [x10]                    ; store 2
        ret

Memset_13
        st1     {v16.8b}, [x10], #8                        ; store 8 
Memset_5
        st4     {v16.b,v17.b,v18.b,v19.b}[0], [x10], #4    ; store 4
Memset_1
        strb    w1, [x10]                                  ; store 1
        ret

Memset_12
        st1     {v16.8b}, [x10], #8                        ; store 8 
Memset_4
        st4     {v16.b,v17.b,v18.b,v19.b}[0], [x10]        ; store 4
        ret

Memset_11
        st1     {v16.8b}, [x10], #8                        ; store 8 
Memset_3
        st3     {v16.b,v17.b,v18.b}[0], [x10]              ; store 3
        ret

Memset_10
        st1     {v16.8b}, [x10], #8                        ; store 8 
        st2     {v16.b,v17.b}[0], [x10]                    ; store 2
        ret

Memset_9
        st1     {v16.8b}, [x10], #8                        ; store 8 
        strb    w1, [x10]                                  ; store 1
        ret

Memset_8
        st1     {v16.8b}, [x10]                            ; store 8
        ret

Memset_17_32
        sub     x8, x2, #16         ; calc offset to beginning of the last 16
                                    ;  bytes of buffer
        add     x8, x8, x10         ; add base because st1 doesn't have an
                                    ;  offset-mode addressing form
        st1     v16.16b, [x8]       ; store last 16 bytes of buffer, it will
                                    ;  overlap the store below for lengths
                                    ;  17-31
Memset_16
        st1     v16.16b, [x10]      ; store first 16 bytes of buffer
Memset_0
Done
        ret                         ; this ret happens to be 32-byte aligned,
                                    ;  so it is the preferred one to jump to
                                    ;  for Done & Memset_0

        ALIGN   32,12
        ; Align here to 12 mod 32 so the label Loop_32 is aligned to 0 mod 32.
        ; 12 = 32 - 5*4 (5 intervening instructions)
SimdMemset
                                    ; copy byte pattern to a second SIMD reg
        mov     w8, #memset_forceAlignThreshold
                                    ; see if this is long enough to be worth
                                    ;  forcing alignment for loop stores
        cmp     x2, x8
        bge     ForceAlignment

        ; When not worth aligning, we store 32 bytes per iteration using
        ; potentially unaligned vector byte stores Only storing 32 vs 64 per
        ; iteration simplifies remainder handling, we can go straight to the
        ; jump table.  We are assuming that a 64-byte 4-vector store is going
        ; to have to be double-pumped anyway, so a 32-byte store only costs
        ; extra for the subs & bgt which should cost no latency due to being
        ; fully overlapped with the store
        asr     x9, x2, #5          ; 32-byte iterations in x9 is count idiv 32

Loop_32
        ; Loop storing 32 bytes/iteration - This label should be aligned to a
        ;  32-byte boundary
        subs    x9, x9, #1          ; decrement iterations count
        st1     {v16.16b, v17.16b}, [x10], #32
                                    ; store 32 bytes at [x10], then increase
                                    ;  x10 by 32
        bgt     Loop_32             ; loop until no more 32-byte iterations

        and     x2, x2, #31         ; remainder length now in x2
        cbz     x2, Done            ; early out if already done
        b       UseJumpTable        ; use jump table for remainder

        ALIGN 32,28
        ; Align NonTemporal to 28 mod 32 so label Loop_NonTemporal is aligned
        ; to 0 mod 32
NonTemporal
        asr     x9, x2, #5          ; 32-byte iterations in x9 is count idiv 32

        ; Loop storing 32 bytes/iteration using non-temporal stores. Sadly, the
        ; only non-temporal forms store only 32 bytes without a post-increment
        ; addressing mode. This results in an extra instruction per iteration,
        ; and 2x as many instructions as the 64-byte loop. It may not matter as
        ; memory bandwith is the limiting factor.
Loop_NonTemporal
        ; This label should be aligned to a 32-byte boundary

        subs    x9, x9, #1          ; decrement iterations count
        stnp    q16, q17, [x10]     ; store 32 bytes at [x10], then increase
                                    ;  x10 by 32
        add     x10, x10, #32       ; update dst ptr manually without setting
                                    ;  cc
        bgt     Loop_NonTemporal    ; loop until no more 32-byte iterations

        and     x2, x2, #31         ; remainder length now in x2
        cbz     x2, Done            ; early out if already done
        b       UseJumpTable        ; if remainder, finish up with jump table

        ALIGN   32,16
        ; Force alignment using overlapping stores method. This requires the
        ; total length be > 16 bytes, and not be device memory.
ForceAlignment
                                    ; calculate number of bytes to store to get
                                    ;  aligned (in x8)
        ands    x8, x10, #15        ; x8 = start address mod 16
        beq     ItsAligned          ; branch on x8 == 0
        sub     x8, x8, #16         ; x8 = (start address mod 16) - 16
        neg     x8, x8              ; x8 = 16 - (start address mod 16) = number
                                    ;  of bytes to store to get aligned
        sub     x2, x2, x8          ; reduce length remaining by number of
                                    ;  bytes needed to get aligned
        st1     v16.16b, [x10], x8  ; store 16, but add only length in x8 to
                                    ;  address

ItsAligned
        mov     w8, #memset_nonTemporalThreshold
                                    ; check for really long lengths worth using
                                    ;  the non-temporal version
        cmp     x2, x8
        bge     NonTemporal

        ; Store 64 bytes per iteration using 16-byte aligned vector multi-
        ; stores while these stores are not cache-line aligned, no individual
        ; vector register will be written across a cache line boundary. Since
        ; the microcode will be splitting the store out into 4 separate stores,
        ; this is as good as being 64-byte cache line aligned. Do not use for
        ; lengths less than 64.

        ; Generate iteration count for 64-byte loop - note: not testing for 0;
        ; don't come here with x2 < 64
        asr     x9, x2, #6          ; 64-byte iterations in x9 is count idiv 64

        ; The align directive above 'ForceAlignment' should be set up so that
        ; this ALIGN directive does not need to generate anything. This one is
        ; left here just in case someone modifies the number of instructions
        ; between 'ForceAlignment' and 'Loop_64' without adjusting that ALIGN
        ; directive.

        ; Due to bug 1106651, automatic padding with NOPs when in code areas is
        ; broken. The workaround is to use -721215457.

        ALIGN 32,0,-721215457,4
        ; Align label Loop_64 to 32-byte boundary using NOPs
Loop_64
        ; Loop storing 64 bytes/iteration - This label should be aligned to a
        ; 32-byte boundary

        subs    x9, x9, #1          ; decrement iterations count
        st1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x10], #64
                                    ; store 64 bytes at [x10], then increase
                                    ;  x10 by 64
        bgt     Loop_64             ; loop until no more 64-byte iterations

        and     x2, x2, #63         ; remainder length now in x2
        cbz     x2, Done            ; early out if already done

        ; Remainder handling for lengths 1-63
        cmp     x2, #32
        blt     UseJumpTable        ; use jump table for last 1-32
        st1     { v16.16b, v17.16b }, [x10], #32
                                    ; store 32 bytes at [x10], then increase
                                    ;  x10 by 32
        subs    x2, x2, #32         ; reduce remainder count
        beq     Done
        b       UseJumpTable        ; finish off with jump table

        LEAF_END memset

        END
