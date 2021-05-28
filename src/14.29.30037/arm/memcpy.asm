;***
; Copyright (c) Microsoft Corporation.  All rights reserved.
;**********

#include "ksarm.h"

#if !defined(_CRT_WINDOWS)

#if !defined(_BOOTCRT_)

        DATAAREA

#if defined(_SYSCRT)
__memcpy_forward_large_func dcd __memcpy_decide
        EXPORT __memcpy_forward_large_func
__memcpy_reverse_large_func dcd __memcpy_decide
        EXPORT __memcpy_reverse_large_func
#else
__memcpy_forward_large_func dcd __memcpy_forward_large_neon
        EXPORT __memcpy_forward_large_func
__memcpy_reverse_large_func dcd __memcpy_reverse_large_neon
        EXPORT __memcpy_reverse_large_func
#endif

#endif

        AREA    |.text|,ALIGN=5,CODE,READONLY

;
; void *memcpy(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward direction.
;

        ALIGN 32
        LEAF_ENTRY memcpy

        ALTERNATE_ENTRY __memcpy_forward_new

        pld     [r1]                                    ; preload the first cache line
        cmp     r2, #16                                 ; less than 16 bytes?
        mov     r3, r0                                  ; use r3 as our destination
        bhs     CpyLrge                                 ; go to the small copy case directly
#ifdef _VCRUNTIME_BUILD_QSPECTRE
        and     r2, r2, #0xF                            ; prevent speculation on r2 >= 16
#endif

CpySmal tbb     [pc, r2]                                ; branch to specialized bits for small copies
__SwitchTable1_Copy
CTable  dcb     (Copy0 - CTable) / 2                    ; 0B
        dcb     (Copy1 - CTable) / 2                    ; 1B
        dcb     (Copy2 - CTable) / 2                    ; 2B
        dcb     (Copy3 - CTable) / 2                    ; 3B
        dcb     (Copy4 - CTable) / 2                    ; 4B
        dcb     (Copy5 - CTable) / 2                    ; 5B
        dcb     (Copy6 - CTable) / 2                    ; 6B
        dcb     (Copy7 - CTable) / 2                    ; 7B
        dcb     (Copy8 - CTable) / 2                    ; 8B
        dcb     (Copy9 - CTable) / 2                    ; 9B
        dcb     (Copy10 - CTable) / 2                   ; 10B
        dcb     (Copy11 - CTable) / 2                   ; 11B
        dcb     (Copy12 - CTable) / 2                   ; 12B
        dcb     (Copy13 - CTable) / 2                   ; 13B
        dcb     (Copy14 - CTable) / 2                   ; 14B
        dcb     (Copy15 - CTable) / 2                   ; 15B
__SwitchTableEnd_Copy

Copy1   ldrb    r2, [r1]
        strb    r2, [r3]
Copy0   bx      lr

Copy2   ldrh    r2, [r1]
        strh    r2, [r3]
        bx      lr

Copy3   ldrh    r2, [r1]
        ldrb    r1, [r1, #2]
        strh    r2, [r3]
        strb    r1, [r3, #2]
        bx      lr

Copy4   ldr     r2, [r1]
        str     r2, [r3]
        bx      lr

Copy5   ldr     r2, [r1]
        ldrb    r1, [r1, #4]
        str     r2, [r3]
        strb    r1, [r3, #4]
        bx      lr

Copy6   ldr     r2, [r1]
        ldrh    r1, [r1, #4]
        str     r2, [r3]
        strh    r1, [r3, #4]
        bx      lr

Copy7   ldr     r12, [r1]
        ldrh    r2, [r1, #4]
        ldrb    r1, [r1, #6]
        str     r12, [r3]
        strh    r2, [r3, #4]
        strb    r1, [r3, #6]
        bx      lr

Copy8   ldr     r2, [r1]
        ldr     r1, [r1, #4]
        str     r2, [r3]
        str     r1, [r3, #4]
        bx      lr

Copy9   ldr     r12, [r1]
        ldr     r2, [r1, #4]
        ldrb    r1, [r1, #8]
        str     r12, [r3]
        str     r2, [r3, #4]
        strb    r1, [r3, #8]
        bx      lr

Copy10  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        ldrh    r1, [r1, #8]
        str     r12, [r3]
        str     r2, [r3, #4]
        strh    r1, [r3, #8]
        bx      lr

Copy11  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        str     r12, [r3]
        str     r2, [r3, #4]
        ldrh    r2, [r1, #8]
        ldrb    r1, [r1, #10]
        strh    r2, [r3, #8]
        strb    r1, [r3, #10]
        bx      lr

Copy12  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        ldr     r1, [r1, #8]
        str     r12, [r3]
        str     r2, [r3, #4]
        str     r1, [r3, #8]
        bx      lr

Copy13  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        str     r12, [r3]
        str     r2, [r3, #4]
        ldr     r2, [r1, #8]
        ldrb    r1, [r1, #12]
        str     r2, [r3, #8]
        strb    r1, [r3, #12]
        bx      lr

Copy14  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        str     r12, [r3]
        str     r2, [r3, #4]
        ldr     r2, [r1, #8]
        ldrh    r1, [r1, #12]
        str     r2, [r3, #8]
        strh    r1, [r3, #12]
        bx      lr

Copy15  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        str     r12, [r3]
        str     r2, [r3, #4]
        ldr     r12, [r1, #8]
        ldrh    r2, [r1, #12]
        ldrb    r1, [r1, #14]
        str     r12, [r3, #8]
        strh    r2, [r3, #12]
        strb    r1, [r3, #14]
        bx      lr

CpyLrge

#if defined(_BOOTCRT_)

        b       __memcpy_forward_large_integer          ; always use integer in boot code

#else

        eor     r12, r0, r1                             ; see if src/dst are equally aligned
        tst     r12, #3                                 ; at least to a 4 byte boundary
        bne     __memcpy_forward_large_neon             ; if not, always use NEON
        mov32   r12, __memcpy_forward_large_func        ; otherwise, load the large function pointer
        ldr     pc, [r12]                               ; and call it

#endif

        LEAF_END memcpy


;
; __memcpy_forward_large_integer (internal calling convention)
;
; Copy large (>= 16 bytes) blocks of memory in a forward direction,
; using integer registers only.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_forward_large_integer

        PROLOG_NOP lsls r12, r3, #31                    ; C = bit 1, N = bit 0
        PROLOG_PUSH {r4-r9, r11, lr}

;
; Align destination to a word boundary
;

        bpl     %F1
        ldrb    r4, [r1], #1                            ; fetch byte
        subs    r2, r2, #1                              ; decrement count
        strb    r4, [r3], #1                            ; store byte
        lsls    r12, r3, #31                            ; compute updated status
1
        bcc     %F2                                     ; if already aligned, just skip ahead
        ldrh    r4, [r1], #2                            ; fetch halfword
        subs    r2, r2, #2                              ; decrement count
        strh    r4, [r3], #2                            ; store halfword
2
        tst     r1, #3                                  ; is the source now word-aligned?
        bne     %F20                                    ; if not, we have to use the slow path

;
; Source is word-aligned; fast case
;

10
        subs    r2, r2, #32                             ; take 32 off the top
        blo     %F13                                    ; if not enough, recover and do small copies
        subs    r2, r2, #32                             ; take off another 32
        pld     [r1, #32]                               ; pre-load one block ahead
        blo     %F12                                    ; skip the loop if that's all we have
11
        pld     [r1, #64]                               ; prefetch ahead
        subs    r2, r2, #32                             ; count the bytes for this block
        ldm     r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        stm     r3!, {r4-r9, r12, lr}                   ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        ldm     r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        stm     r3!, {r4-r9, r12, lr}                   ; store 32 bytes
13
        adds    r2, r2, #(32 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this loop
14
        subs    r2, r2, #8                              ; decrement count
        ldrd    r4, r5, [r1], #8                        ; fetch pair of words
        strd    r4, r5, [r3], #8                        ; store pair of words
        bhs     %B14                                    ; loop while we still have data remaining
15
        adds    r2, r2, #8                              ; recover final count

        EPILOG_POP {r4-r9, r11, lr}
        EPILOG_NOP bne CpySmal                          ; if some left, continue with small
        EPILOG_RETURN                                   ; else just return

;
; Source is not word-aligned; slow case
;

20
        subs    r2, r2, #64                             ; pre-decrement to simplify the loop
        blo     %23                                     ; skip over the loop if we don't have enough
        pld     [r1, #32]                               ; pre-load one block ahead
21
        pld     [r1, #64]                               ; prefetch ahead
        ldr     r4, [r1, #0]                            ; load 32 bytes
        ldr     r5, [r1, #4]                            ;
        ldr     r6, [r1, #8]                            ;
        ldr     r7, [r1, #12]                           ;
        ldr     r8, [r1, #16]                           ;
        ldr     r9, [r1, #20]                           ;
        ldr     r12, [r1, #24]                          ;
        ldr     lr, [r1, #28]                           ;
        adds    r1, r1, #32                             ; update pointer
        subs    r2, r2, #32                             ; count the bytes for this block
        stm     r3!, {r4-r9, r12, lr}                   ; store 32 bytes
        bhs     %B21                                    ; keep going until we're done
23
        adds    r2, r2, #(64 - 8)                       ; recover original count, and pre-decrement
        blo     %F25                                    ; if not enough remaining, skip this loop
24
        ldr     r4, [r1]                                ; fetch pair of words
        ldr     r5, [r1, #4]                            ;
        adds    r1, r1, #8                              ; update pointer
        subs    r2, r2, #8                              ; decrement count
        strd    r4, r5, [r3], #8                        ; store pair of words
        bhs     %B24                                    ; loop while we still have data remaining
25
        adds    r2, r2, #8                              ; recover final count

        EPILOG_POP {r4-r9, r11, lr}
        EPILOG_NOP bne CpySmal                          ; if some left, continue with small
        EPILOG_RETURN                                   ; else just return

        NESTED_END __memcpy_forward_large_integer


;
; __memcpy_forward_large_neon (internal calling convention)
;
; Copy large (>= 16 bytes) blocks of memory in a forward direction,
; using NEON registers.
;

#if !defined(_BOOTCRT_)

        ALIGN 32
        NESTED_ENTRY __memcpy_forward_large_neon

        PROLOG_PUSH {r4-r5, r11, lr}

        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        pld     [r1, #32]                               ; pre-load one block ahead
        blo     %F12                                    ; skip over the loop if we don't have enough
11
        pld     [r1, #64]                               ; prefetch ahead
        subs    r2, r2, #32                             ; count the bytes for this block
        vld1.8  {d0-d3}, [r1]!                          ; load 32 bytes
        vst1.8  {d0-d3}, [r3]!                          ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        vld1.8  {d0-d3}, [r1]!                          ; load 32 bytes
        vst1.8  {d0-d3}, [r3]!                          ; store 32 bytes
13
        adds    r2, r2, #(32 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this loop
14
        ldr     r4, [r1]                                ; fetch pair of words
        ldr     r5, [r1, #4]                            ;
        adds    r1, r1, #8                              ; update pointer
        str     r4, [r3]                                ; store pair of words
        str     r5, [r3, #4]                            ;
        adds    r3, r3, #8
        subs    r2, r2, #8                              ; decrement count
        bhs     %B14                                    ; loop while we still have data remaining
15
        adds    r2, r2, #8                              ; recover final count

        EPILOG_POP {r4-r5, r11, lr}
        EPILOG_NOP bne CpySmal                          ; if some left, continue with small
        EPILOG_RETURN                                   ; else just return

        NESTED_END __memcpy_forward_large_neon

#endif


;
; void *memmove(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward or reverse direction, ensuring that
; overlapping source/destination regions are copied correctly.
;

        ALIGN 32
        LEAF_ENTRY memmove

        subs    r3, r0, r1                              ; compute dest - source
        cmp     r3, r2                                  ; compare against size
        bhs     memcpy                                  ; if no overlap, we can just do memcpy

        ALTERNATE_ENTRY __memcpy_reverse_new

        cmp     r2, #16                                 ; less than 16 bytes?
        pld     [r1]                                    ; preload the first cache line
        bhs     MovLrge                                 ; go to the small copy case directly
#ifdef _VCRUNTIME_BUILD_QSPECTRE
        and     r2, r2, #0xF                            ; prevent speculation on r2 >= 16
#endif

MovSmal tbb     [pc, r2]                                ; branch to specialized bits for small copies
__SwitchTable1_Move
MTable  dcb     (Move0 - MTable) / 2                    ; 0B
        dcb     (Move1 - MTable) / 2                    ; 1B
        dcb     (Move2 - MTable) / 2                    ; 2B
        dcb     (Move3 - MTable) / 2                    ; 3B
        dcb     (Move4 - MTable) / 2                    ; 4B
        dcb     (Move5 - MTable) / 2                    ; 5B
        dcb     (Move6 - MTable) / 2                    ; 6B
        dcb     (Move7 - MTable) / 2                    ; 7B
        dcb     (Move8 - MTable) / 2                    ; 8B
        dcb     (Move9 - MTable) / 2                    ; 9B
        dcb     (Move10 - MTable) / 2                   ; 10B
        dcb     (Move11 - MTable) / 2                   ; 11B
        dcb     (Move12 - MTable) / 2                   ; 12B
        dcb     (Move13 - MTable) / 2                   ; 13B
        dcb     (Move14 - MTable) / 2                   ; 14B
        dcb     (Move15 - MTable) / 2                   ; 15B
__SwitchTableEnd_Move

Move1   ldrb    r2, [r1]
        strb    r2, [r0]
Move0   bx      lr

Move2   ldrh    r2, [r1]
        strh    r2, [r0]
        bx      lr

Move3   ldrh    r2, [r1]
        ldrb    r1, [r1, #2]
        strh    r2, [r0]
        strb    r1, [r0, #2]
        bx      lr

Move4   ldr     r2, [r1]
        str     r2, [r0]
        bx      lr

Move5   ldr     r2, [r1]
        ldrb    r1, [r1, #4]
        str     r2, [r0]
        strb    r1, [r0, #4]
        bx      lr

Move6   ldr     r2, [r1]
        ldrh    r1, [r1, #4]
        str     r2, [r0]
        strh    r1, [r0, #4]
        bx      lr

Move7   ldr     r3, [r1]
        ldrh    r2, [r1, #4]
        ldrb    r1, [r1, #6]
        str     r3, [r0]
        strh    r2, [r0, #4]
        strb    r1, [r0, #6]
        bx      lr

Move8   ldr     r2, [r1]
        ldr     r1, [r1, #4]
        str     r2, [r0]
        str     r1, [r0, #4]
        bx      lr

Move9   ldr     r3, [r1]
        ldr     r2, [r1, #4]
        ldrb    r1, [r1, #8]
        str     r3, [r0]
        str     r2, [r0, #4]
        strb    r1, [r0, #8]
        bx      lr

Move10  ldr     r3, [r1]
        ldr     r2, [r1, #4]
        ldrh    r1, [r1, #8]
        str     r3, [r0]
        str     r2, [r0, #4]
        strh    r1, [r0, #8]
        bx      lr

Move11  ldr     r12, [r1]
        ldr     r3, [r1, #4]
        ldrh    r2, [r1, #8]
        ldrb    r1, [r1, #10]
        str     r12, [r0]
        str     r3, [r0, #4]
        strh    r2, [r0, #8]
        strb    r1, [r0, #10]
        bx      lr

Move12  ldr     r12, [r1]
        ldr     r2, [r1, #4]
        ldr     r1, [r1, #8]
        str     r12, [r0]
        str     r2, [r0, #4]
        str     r1, [r0, #8]
        bx      lr

Move13  ldr     r12, [r1]
        ldr     r3, [r1, #4]
        ldr     r2, [r1, #8]
        ldrb    r1, [r1, #12]
        str     r12, [r0]
        str     r3, [r0, #4]
        str     r2, [r0, #8]
        strb    r1, [r0, #12]
        bx      lr

Move14  ldr     r12, [r1]
        ldr     r3, [r1, #4]
        ldr     r2, [r1, #8]
        ldrh    r1, [r1, #12]
        str     r12, [r0]
        str     r3, [r0, #4]
        str     r2, [r0, #8]
        strh    r1, [r0, #12]
        bx      lr

Move15  ldrh    r3, [r1, #12]
        ldrb    r2, [r1, #14]
        strh    r3, [r0, #12]
        strb    r2, [r0, #14]
        ldr     r3, [r1]
        ldr     r2, [r1, #4]
        ldr     r1, [r1, #8]
        str     r3, [r0]
        str     r2, [r0, #4]
        str     r1, [r0, #8]
        bx      lr

MovLrge

#if defined(_BOOTCRT_)

        b       __memcpy_reverse_large_integer          ; always use integer in boot code

#else

        eor     r12, r0, r1                             ; see if src/dst are equally aligned
        tst     r12, #3                                 ; at least to a 4 byte boundary
        bne.w   __memcpy_reverse_large_neon             ; if not, always use NEON
        mov32   r12, __memcpy_reverse_large_func
        ldr     pc, [r12]

#endif

        LEAF_END memmove


;
; __memcpy_reverse_large_integer (internal calling convention)
;
; Copy large (>= 16 bytes) block of memory in a reverse direction,
; using NEON registers.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_reverse_large_integer

        PROLOG_NOP adds r3, r0, r2                      ; advance destination to end
        PROLOG_NOP adds r1, r1, r2                      ; advance source to end
        PROLOG_NOP lsls r12, r3, #31                    ; C = bit 1, N = bit 0
        PROLOG_NOP pld [r1, #-32]                       ; pre-load one block ahead
        PROLOG_PUSH {r4-r9, r11, lr}

;
; Align destination to a word boundary
;

        bpl     %F1
        ldrb    r4, [r1, #-1]!                          ; fetch byte
        subs    r2, r2, #1                              ; decrement count
        strb    r4, [r3, #-1]!                          ; store byte
        lsls    r12, r3, #31                            ; compute updated status
1
        bcc     %F2                                     ; if already aligned, just skip ahead
        ldrh    r4, [r1, #-2]!                          ; fetch halfword
        subs    r2, r2, #2                              ; decrement count
        strh    r4, [r3, #-2]!                          ; store halfword
2
        tst     r1, #3                                  ; is the source now word-aligned?
        bne     %F20                                    ; if not, we have to use the slow path

;
; Source is word-aligned; fast case
;

10
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        pld     [r1, #-64]                              ; pre-load one block ahead
        blo     %F12                                    ; skip over the loop if we don't have enough
11
        pld     [r1, #-96]                              ; prefetch ahead
        subs    r2, r2, #32                             ; count the bytes for this block
        ldmdb   r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        stmdb   r3!, {r4-r9, r12, lr}                   ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        ldmdb   r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        stmdb   r3!, {r4-r9, r12, lr}                   ; store 32 bytes
13
        adds    r2, r2, #(32 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this loop
14
        subs    r2, r2, #8                              ; decrement count
        ldrd    r4, r5, [r1, #-8]!                      ; fetch pair of words
        strd    r4, r5, [r3, #-8]!                      ; store pair of words
        bhs     %B14                                    ; loop while we still have data remaining
15
        adds    r2, r2, #8                              ; determine final count
        subs    r1, r1, r2                              ; recover original source

        EPILOG_POP {r4-r9, r11, lr}
        EPILOG_NOP bne MovSmal                          ; if some left, continue with small
        EPILOG_RETURN                                   ; else just return


;
; Source is not word-aligned; slow case
;

20
        subs    r2, r2, #64                             ; pre-decrement to simplify the loop
        blo     %F23                                    ; skip over the loop if we don't have enough
        pld     [r1, #-64]                              ; pre-load one block ahead
21
        pld     [r1, #-96]                              ; prefetch ahead
        subs    r2, r2, #32                             ; count the bytes for this block
        ldr     r4, [r1, #-32]!                         ; load 32 bytes
        ldr     r5, [r1, #4]                            ;
        ldr     r6, [r1, #8]                            ;
        ldr     r7, [r1, #12]                           ;
        ldr     r8, [r1, #16]                           ;
        ldr     r9, [r1, #20]                           ;
        ldr     r12, [r1, #24]                          ;
        ldr     lr, [r1, #28]                           ;
        stmdb   r3!, {r4-r9, r12, lr}                   ; store 32 bytes
        bhs     %B21                                    ; keep going until we're done
23
        adds    r2, r2, #(64 - 8)                       ; recover original count, and pre-decrement
        blo     %F25                                    ; if not enough remaining, skip this loop
24
        subs    r2, r2, #8                              ; decrement count
        ldr     r4, [r1, #-8]!                          ; fetch pair of words
        ldr     r5, [r1, #4]                            ;
        strd    r4, r5, [r3, #-8]!                      ; store pair of words
        bhs     %B24                                    ; loop while we still have data remaining
25
        adds    r2, r2, #8                              ; determine final count
        subs    r1, r1, r2                              ; recover original source

        EPILOG_POP {r4-r9, r11, lr}
        EPILOG_NOP bne MovSmal                          ; if some left, continue with small
        EPILOG_RETURN                                   ; else just return

        NESTED_END __memcpy_reverse_large_integer


;
; __memcpy_reverse_large_neon (internal calling convention)
;
; Copy large (>= 16 bytes) block of memory in a reverse direction,
; using NEON registers.
;

#if !defined(_BOOTCRT_)

        ALIGN 32
        NESTED_ENTRY __memcpy_reverse_large_neon

        PROLOG_NOP adds r3, r0, r2                      ; advance destination to end
        PROLOG_NOP adds r1, r1, r2                      ; advance source to end
        PROLOG_NOP lsls r12, r3, #31                    ; C = bit 1, N = bit 0
        PROLOG_NOP pld [r1, #-32]                       ; pre-load one block ahead
        PROLOG_PUSH {r4-r5, r11, lr}

;
; Align destination to a word boundary
;

        bpl     %F1
        ldrb    r4, [r1, #-1]!                          ; fetch byte
        subs    r2, r2, #1                              ; decrement count
        strb    r4, [r3, #-1]!                          ; store byte
        lsls    r12, r3, #31                            ; compute updated status
1
        bcc     %F2                                     ; if already aligned, just skip ahead
        ldrh    r4, [r1, #-2]!                          ; fetch halfword
        subs    r2, r2, #2                              ; decrement count
        strh    r4, [r3, #-2]!                          ; store halfword
2

;
; Perform main copy
;

        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        pld     [r1, #-64]                              ; pre-load one block ahead
        blo     %F12                                    ; skip over the loop if we don't have enough
11
        pld     [r1, #-96]                              ; prefetch ahead
        subs    r1, r1, #32
        subs    r3, r3, #32
        subs    r2, r2, #32                             ; count the bytes for this block
        vld1.8  {d0-d3}, [r1]                           ; load 32 bytes
        vst1.8  {d0-d3}, [r3]                           ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        subs    r1, r1, #32
        subs    r3, r3, #32
        vld1.8  {d0-d3}, [r1]                           ; load 32 bytes
        vst1.8  {d0-d3}, [r3]                           ; store 32 bytes
13
        adds    r2, r2, #(32 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this loop
14
        ldr     r4, [r1, #-8]!                          ; fetch pair of words
        ldr     r5, [r1, #4]                            ; fetch pair of words
        subs    r2, r2, #8                              ; decrement count
        str     r4, [r3, #-8]!                          ; store pair of words
        str     r5, [r3, #4]
        bhs     %B14                                    ; loop while we still have data remaining
15
        adds    r2, r2, #8                              ; determine final count
        subs    r1, r1, r2                              ; recover original source

        EPILOG_POP {r4-r5, r11, lr}
        EPILOG_NOP bne MovSmal                          ; if some left, continue with small
        EPILOG_RETURN                                   ; else just return

        NESTED_END __memcpy_reverse_large_neon

#endif


;
; __memcpy_decide (internal calling convention)
;
; Determine whether to use integer or NEON for future memcpy's.
;

#if defined(_SYSCRT) && !defined(_BOOTCRT_)

        ALIGN 32
        NESTED_ENTRY __memcpy_decide

        PROLOG_PUSH {r4-r5, r11, lr}

        ;
        ; We want to use integer memcpy's on the A9, which has an external cache.
        ;
        ; First determine if we're in user or kernel mode. Reading CPSR
        ; from user mode will either return the proper 5 mode bits, or all 0s.
        ; Conveniently, user mode is 0x10, and there is no mode 0x00, so if
        ; we read CPSR and the low 4 bits are 0, that's good enough.
        ;

        mrs     r4, cpsr                                ; get CPSR
        ands    r4, r4, #0xf                            ; isolate the low 4 bits of the mode
        beq     %F1                                     ; if 0, we're in user mode

        ;
        ; If we are in kernel mode, read the MIDR directly.
        ;

        CP_READ r4, CP15_MIDR                           ; read main ID register
        ubfx    r5, r4, #24, #8                         ; get implementer
        lsrs    r4, r4, #4                              ; shift off revision field
        cmp     r5, #0x41                               ; is implementer == ARM?
        bne     %F3                                     ; if not, use NEON
        bfc     r4, #12, #20                            ; clear upper bits
        ldr     r5, =0xc09                              ; A9 signature
        cmp     r4, r5                                  ; is this an A9?
        bne     %F3                                     ; if not, use NEON
        b       %F2                                     ; otherwise, use integer

        ;
        ; If we are in user mode, check the "external cache available" flag
        ;
1
        ldr     r4, =MM_SHARED_USER_DATA_VA + UsProcessorFeatures + PF_ARM_EXTERNAL_CACHE_AVAILABLE
        ldrb    r4, [r4]                                ; get external cache bit
        cbz     r4, %F3                                 ; if no external cache, do NEON

        ;
        ; Register for integer functions
        ;
2
        ldr     r4, =__memcpy_forward_large_integer     ; select integer functions
        ldr     r5, =__memcpy_forward_large_func        ;
        str     r4, [r5]                                ;
        ldr     r4, =__memcpy_reverse_large_integer     ; select integer functions
        ldr     r5, =__memcpy_reverse_large_func        ;
        str     r4, [r5]                                ;
        b       %F4

        ;
        ; Register for NEON functions
        ;
3
        ldr     r4, =__memcpy_forward_large_neon        ; select NEON functions
        ldr     r5, =__memcpy_forward_large_func        ;
        str     r4, [r5]                                ;
        ldr     r4, =__memcpy_reverse_large_neon        ; select NEON functions
        ldr     r5, =__memcpy_reverse_large_func        ;
        str     r4, [r5]                                ;
4
        EPILOG_POP {r4-r5, r11, lr}                     ; restore saved registers
        EPILOG_NOP ldr     pc, [r12]                    ; jump to the appropriate target

        NESTED_END __memcpy_decide

#endif


;
; void _memcpy_strict_align(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward direction, only performing naturally-aligned
; accesses.
;

        ALIGN 32
        LEAF_ENTRY _memcpy_strict_align

;
; Verify alignment between source and destination
;

        sub     r3, r0, r1                              ; get relative alignment of source and destination
        cbz     r2, CopyExit                            ; exit if 0 count
        ands    r3, r3, #3                              ; check DWORD alignment
        bne     CopyMisalignedHalf                      ; misaligned

;
; Source and destination are equally aligned: just align the
; destination and the source will end up aligned as well
;

        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_0                           ; if so, skip ahead
        tst     r0, #1                                  ; halfword aligned at the dest?
        beq     HalfAligned_0                           ; if so, skip ahead

        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        beq     WordAligned_0                           ; if so, skip ahead

HalfAligned_0
        cmp     r2, #2                                  ; do we have at least 2 bytes left?
        blo     CopyFinalBytes                          ; if not, copy bytes
        subs    r2, r2, #2                              ; decrement count
        ldrh    r3, [r1], #2                            ; fetch halfword
        strh    r3, [r0], #2                            ; store it
        beq     CopyExit                                ; stop if done

WordAligned_0
        subs    r2, r2, #4                              ; at least 4 bytes remaining?
        blt     WordLoopEnd_0                           ; if not, skip the main loop
WordLoop_0
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        str     r3, [r0], #4                            ; store it
        bge     WordLoop_0                              ; stop if done
WordLoopEnd_0
        adds    r2, r2, #4                              ; recover the extra 4 we subtracted
        beq     CopyExit                                ; stop if that's everything

CopyFinalHalfwords
        subs    r2, r2, #2                              ; at least 2 bytes remaining?
        blt     CopyFinalHalfwordsEnd                   ; if not, skip this
CopyFinalHalfwordsLoop
        subs    r2, r2, #2                              ; decrement count
        ldrh    r3, [r1], #2                            ; fetch halfword
        strh    r3, [r0], #2                            ; store it
        bge     CopyFinalHalfwordsLoop                  ; loop until done
CopyFinalHalfwordsEnd
        adds    r2, r2, #2                              ; recover the extra 2 we subtracted
        beq     CopyExit                                ; stop if that's everything

CopyFinalBytes
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        bne     CopyFinalBytes                          ; loop until done
CopyExit
        bx      lr                                      ; return


;
; Source and destination are misaligned by 2 bytes
;

CopyMisalignedHalf
        cmp     r3, #2                                  ; misaligned by a halfword?
        bne     CopyMisalignedByte                      ; if not, skip

        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_2                           ; if so, skip ahead
        tst     r0, #1                                  ; halfword aligned at the dest?
        beq     HalfAligned_2                           ; if so, skip ahead

        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        beq     WordAligned_2                           ; if so, skip ahead

HalfAligned_2
        cmp     r2, #2                                  ; do we have at least 2 bytes left?
        blo     CopyFinalBytes                          ; if not, copy bytes
        subs    r2, r2, #2                              ; decrement count
        ldrh    r3, [r1], #2                            ; fetch halfword
        strh    r3, [r0], #2                            ; store it
        beq     CopyExit                                ; stop if done

WordAligned_2
        subs    r2, r2, #6                              ; at least 6 bytes remaining?
        blt     WordLoopEnd_2                           ; if so, skip the main loop
        ldrh    r12, [r1], #2                           ; preload a halfword of source
        subs    r2, r2, #2                              ; count these 2 bytes
WordLoop_2
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        orr     r12, r12, r3, lsl #16                   ; copy low 16 bits to upper 16 of r12
        str     r12, [r0], #4                           ; store it
        lsr     r12, r3, #16                            ; copy upper 16 bits to lower 16 of r12
        bge     WordLoop_2                              ; stop if done
        strh    r12, [r0], #2                           ; store the extra halfword to the dest
WordLoopEnd_2
        adds    r2, r2, #6                              ; recover the extra 6 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalHalfwords                      ; otherwise, copy remainder


;
; Source and destination are misaligned by 1 byte
;

CopyMisalignedByte
        cmp     r3, #1                                  ; misaligned by a byte?
        bne     CopyMisalignedByte3                     ; if not, skip

        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_1                           ; if so, skip ahead
ByteAlign_1
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        bne     ByteAlign_1                             ; if not, keep copying bytes

WordAligned_1
        subs    r2, r2, #5                              ; at least 5 bytes remaining?
        blt     WordLoopEnd_1                           ; if so, skip the main loop
        ldrb    r12, [r1], #1                           ; preload a byte of source
        subs    r2, r2, #1                              ; count this byte
WordLoop_1
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        orr     r12, r12, r3, lsl #8                    ; copy low 24 bits to upper 24 of r12
        str     r12, [r0], #4                           ; store it
        lsr     r12, r3, #24                            ; copy upper 8 bits to lower 8 of r12
        bge     WordLoop_1                              ; stop if done
        strb    r12, [r0], #1                           ; store the extra byte to the dest
WordLoopEnd_1
        adds    r2, r2, #5                              ; recover the extra 5 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalBytes                          ; otherwise, copy remainder


;
; Source and destination are misaligned by 3 bytes
;

CopyMisalignedByte3
        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_3                           ; if so, skip ahead
ByteAlign_3
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        bne     ByteAlign_3                             ; if not, keep copying bytes

WordAligned_3
        subs    r2, r2, #7                              ; at least 7 bytes remaining?
        blt     WordLoopEnd_3                           ; if so, skip the main loop
        ldrb    r12, [r1], #1                           ; preload a byte of source
        ldrh    r3, [r1], #2                            ; preload a halfword of source
        orr     r12, r12, r3, lsl #8                    ; OR in the halfword
        subs    r2, r2, #3                              ; count these 3 bytes
WordLoop_3
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        orr     r12, r12, r3, lsl #24                   ; copy low 8 bits to upper 8 of r12
        str     r12, [r0], #4                           ; store it
        lsr     r12, r3, #8                             ; copy upper 24 bits to lower 24 of r12
        bge     WordLoop_3                              ; stop if done
        strh    r12, [r0], #2                           ; store the extra halfword to the dest
        lsr     r12, r12, #16                           ; down to the final byte
        strb    r12, [r0], #1                           ; store the extra byte to the dest
WordLoopEnd_3
        adds    r2, r2, #7                              ; recover the extra 7 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalBytes                          ; otherwise, copy remainder

        LEAF_END _memcpy_strict_align

        END

#else /* !defined(_CRT_WINDOWS) */

#if !defined(_BOOTCRT_)

        DATAAREA

__memcpy_implementation dcb 0

        AREA    |.text|,ALIGN=5,CODE,READONLY

;
; Function table
;

__memcpy_functions
        DCD     __memcpy_decide, __memcpy_decide
        DCD     __memcpy_forward_large_integer, __memcpy_reverse_large_integer
        DCD     __memcpy_forward_large_neon, __memcpy_reverse_large_neon
        DCD     __memcpy_forward_large_integer, __memcpy_reverse_large_integer

;
; void *memcpy(void *dst, const void *src, size_t length)
; void *memmove(void *dst, const void *src, size_t length)
; void *_memcpy_strict_align(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward direction.
;

        ALIGN 32
        LEAF_ENTRY memcpy

        ALTERNATE_ENTRY memmove

        subs    r3, r0, r1                              ; compute dest - source
        cmp     r3, r2                                  ; compare against size
        blo.w   __memcpy_reverse                        ; if overlap, we must do it in reverse

        ALTERNATE_ENTRY _memcpy_strict_align
        ALTERNATE_ENTRY __memcpy_forward

        pld     [r1]                                    ; preload the first cache line
        cmp     r2, #8                                  ; less than 8 bytes?
        bhs     CpyLrge                                 ; go to the small copy case directly
        cbz     r2, %F2                                 ; size 0 just exits
        mov     r3, r0                                  ; use r3 as our destination
1
        subs    r2, r2, #1                              ; decrement count
        ldrb    r12, [r1], #1                           ; load
        strb    r12, [r3], #1                           ; store
        bne     %B1                                     ; loop until done
2
        bx      lr                                      ; return

CpyLrge
        mov32   r12, __memcpy_implementation            ; fetch the implementation global address
        eors    r3, r0, r1                              ; compare src/dst addresses
        ldrb    r12, [r12]                              ; read the implementation index
        tst     r3, #3                                  ; see if src/dst are equally aligned to 4 bytes
        and     r12, r12, #3                            ; isolate low 2 bits of implementation index
        adr     r3, __memcpy_functions                  ; get a pointer to the function table
        ldr     r12, [r3, r12, lsl #3]                  ; load function at 8*function index
        mov     r3, r0                                  ; use r3 as our destination
        bne     __memcpy_forward_large_neon             ; if not aligned, always use NEON
        bx      r12                                     ; branch through

        LEAF_END memcpy


;
; __memcpy_forward_large_integer (internal calling convention)
;
; Copy large (>= 16 bytes) blocks of memory in a forward direction,
; using integer registers only.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_forward_large_integer

        PROLOG_NOP lsls r12, r3, #31                    ; C = bit 1, N = bit 0
        PROLOG_PUSH {r4-r9, r11, lr}

;
; Align destination to a word boundary; doing this will also
; align the source, since this version is only used for the
; case where source and destination are equally aligned.
;

        bpl     %F1
        ldrb    r4, [r1], #1                            ; fetch byte
        subs    r2, r2, #1                              ; decrement count
        strb    r4, [r3], #1                            ; store byte
        lsls    r12, r3, #31                            ; compute the updated flags
1
        bcc     %F2                                     ; if already aligned, just skip ahead
        ldrh    r4, [r1], #2                            ; fetch halfword
        subs    r2, r2, #2                              ; decrement count
        strh    r4, [r3], #2                            ; store halfword
2

;
; Copy using bulk register loads/stores
;
        subs    r2, r2, #32                             ; take 32 off the top
        blo     %F13                                    ; if not enough, recover and do small copies
        subs    r2, r2, #32                             ; take off another 32
        pld     [r1, #32]                               ; pre-load one block ahead
        blo     %F12                                    ; skip the loop if that's all we have
11
        pld     [r1, #64]                               ; prefetch ahead
        ldm     r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        subs    r2, r2, #32                             ; count the bytes for this block
        stm     r3!, {r4-r9, r12, lr}                   ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        ldm     r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        stm     r3!, {r4-r9, r12, lr}                   ; store 32 bytes
13
        adds    r2, r2, #(32 - 16)                      ; recover original count, and pre-decrement
        blo     %F14                                    ; if not enough remaining, skip this part
        ldm     r1!, {r4-r7}                            ; load 16 bytes
        subs    r2, r2, #16                             ; count the bytes for this block
        stm     r3!, {r4-r7}                            ; store 16 bytes
14
        adds    r2, r2, #(16 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this part
        ldrd    r4, r5, [r1], #8                        ; load 8 bytes
        subs    r2, r2, #8                              ; count the bytes for this block
        strd    r4, r5, [r3], #8                        ; store 8 bytes
15
        adds    r2, r2, #(8 - 4)                        ; recover original count, and pre-decrement
        blo     %F16                                    ; if not enough remaining, skip this part
        ldr     r4, [r1], #4                            ; load 4 bytes
        subs    r2, r2, #4                              ; count the bytes for this block
        str     r4, [r3], #4                            ; store 4 bytes
16
        adds    r2, r2, #(4 - 2)                        ; recover original count, and pre-decrement
        blo     %F17                                    ; if not enough remaining, skip this part
        ldrh    r4, [r1], #2                            ; load 2 bytes
        subs    r2, r2, #2                              ; count the bytes for this block
        strh    r4, [r3], #2                            ; store 2 bytes
17
        adds    r2, r2, #2                              ; recover remaining count
        beq     %F18                                    ; if not enough remaining, skip this part
        ldrb    r2, [r1]                                ; load 1 byte
        strb    r2, [r3]                                ; store 1 byte
18
        EPILOG_POP {r4-r9, r11, pc}

        NESTED_END __memcpy_forward_large_integer


;
; __memcpy_forward_large_neon (internal calling convention)
;
; Copy large (>= 16 bytes) blocks of memory in a forward direction,
; using NEON registers.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_forward_large_neon

        PROLOG_PUSH {r11, lr}

        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        pld     [r1, #32]                               ; pre-load one block ahead
        blo     %F12                                    ; skip over the loop if we don't have enough
11
        pld     [r1, #64]                               ; prefetch ahead
        vld1.8  {d0-d3}, [r1]!                          ; load 32 bytes
        subs    r2, r2, #32                             ; count the bytes for this block
        vst1.8  {d0-d3}, [r3]!                          ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        vld1.8  {d0-d3}, [r1]!                          ; load 32 bytes
        vst1.8  {d0-d3}, [r3]!                          ; store 32 bytes
13
        adds    r2, r2, #(32 - 16)                      ; recover original count, and pre-decrement
        blo     %F14                                    ; if not enough remaining, skip this part
        vld1.8  {d0-d1}, [r1]!                          ; load 16 bytes
        subs    r2, r2, #16                             ; count the bytes for this block
        vst1.8  {d0-d1}, [r3]!                          ; store 16 bytes
14
        adds    r2, r2, #(16 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this loop
        vld1.8  {d0}, [r1]!                             ; load 8 bytes
        subs    r2, r2, #8                              ; count the bytes for this block
        vst1.8  {d0}, [r3]!                             ; store 8 bytes
15
        adds    r2, r2, #(8 - 4)                        ; recover original count, and pre-decrement
        blo     %F16                                    ; if not enough remaining, skip this loop
        vld4.8  {d0[0]-d3[0]}, [r1]!                    ; load 4 bytes
        subs    r2, r2, #4                              ; count the bytes for this block
        vst4.8  {d0[0]-d3[0]}, [r3]!                    ; store 4 bytes
16
        adds    r2, r2, #(4 - 2)                        ; recover original count, and pre-decrement
        blo     %F17                                    ; if not enough remaining, skip this loop
        vld2.8  {d0[0]-d1[0]}, [r1]!                    ; load 2 bytes
        subs    r2, r2, #2                              ; count the bytes for this block
        vst2.8  {d0[0]-d1[0]}, [r3]!                    ; store 2 bytes
17
        adds    r2, r2, #2                              ; recover remaining count
        beq     %F18                                    ; if not enough remaining, skip this part
        ldrb    r2, [r1]                                ; load 1 byte
        strb    r2, [r3]                                ; store 1 byte
18
        EPILOG_POP {r11, pc}

        NESTED_END __memcpy_forward_large_neon

;
; void *memmove(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward or reverse direction, ensuring that
; overlapping source/destination regions are copied correctly.
;

        ALIGN 32
        LEAF_ENTRY __memcpy_reverse

        adds    r3, r0, r2                              ; advance destination to end
        adds    r1, r1, r2                              ; advance source to end
        pld     [r1]                                    ; preload the first cache line
        cmp     r2, #8                                  ; less than 8 bytes?
        bhs     MovLrge                                 ; go to the small copy case directly
        cbz     r2, %F2                                 ; size 0 just exits
1
        subs    r2, r2, #1                              ; decrement count
        ldrb    r12, [r1, #-1]!                         ; load
        strb    r12, [r3, #-1]!                         ; store
        bne     %B1                                     ; loop until done
2
        bx      lr                                      ; return

MovLrge
        mov32   r12, __memcpy_implementation            ; fetch the implementation global address
        eors    r3, r0, r1                              ; compare src/dst addresses
        ldrb    r12, [r12]                              ; read the implementation index
        tst     r3, #3                                  ; see if src/dst are equally aligned to 4 bytes
        and     r12, r12, #3                            ; isolate low 2 bits of implementation index
        adr     r3, __memcpy_functions+4                ; get a pointer to the function table + 4 (for reverse)
        ldr     r12, [r3, r12, lsl #3]                  ; load function at 8*function index
        adds    r3, r0, r2                              ; use r3 as our destination
        bne     __memcpy_reverse_large_neon             ; if not aligned, always use NEON
        bx      r12                                     ; branch through

        LEAF_END __memcpy_reverse


;
; __memcpy_reverse_large_integer (internal calling convention)
;
; Copy large (>= 16 bytes) block of memory in a reverse direction,
; using NEON registers.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_reverse_large_integer

        PROLOG_NOP lsls r12, r3, #31                    ; C = bit 1, N = bit 0
        PROLOG_NOP pld [r1, #-32]                       ; pre-load one block ahead
        PROLOG_PUSH {r4-r9, r11, lr}

;
; Align destination to a word boundary; doing this will also
; align the source, since this version is only used for the
; case where source and destination are equally aligned.
;

        bpl     %F1
        ldrb    r4, [r1, #-1]!                          ; fetch byte
        sub     r2, r2, #1                              ; decrement count
        strb    r4, [r3, #-1]!                          ; store byte
1
        bcc     %F2                                     ; if already aligned, just skip ahead
        ldrh    r4, [r1, #-2]!                          ; fetch halfword
        subs    r2, r2, #2                              ; decrement count
        strh    r4, [r3, #-2]!                          ; store halfword
2

;
; Copy using bulk register loads/stores
;

        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        pld     [r1, #-64]                              ; pre-load one block ahead
        blo     %F12                                    ; skip over the loop if we don't have enough
11
        pld     [r1, #-96]                              ; prefetch ahead
        ldmdb   r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        subs    r2, r2, #32                             ; count the bytes for this block
        stmdb   r3!, {r4-r9, r12, lr}                   ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        ldmdb   r1!, {r4-r9, r12, lr}                   ; load 32 bytes
        stmdb   r3!, {r4-r9, r12, lr}                   ; store 32 bytes
13
        adds    r2, r2, #(32 - 16)                      ; recover original count, and pre-decrement
        blo     %F14                                    ; if not enough remaining, skip this part
        ldmdb   r1!, {r4-r7}                            ; load 16 bytes
        subs    r2, r2, #16                             ; count the bytes for this block
        stmdb   r3!, {r4-r7}                            ; store 16 bytes
14
        adds    r2, r2, #(16 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this part
        ldrd    r4, r5, [r1, #-8]!                      ; load 8 bytes
        subs    r2, r2, #8                              ; count the bytes for this block
        strd    r4, r5, [r3, #-8]!                      ; store 8 bytes
15
        adds    r2, r2, #(8 - 4)                        ; recover original count, and pre-decrement
        blo     %F16                                    ; if not enough remaining, skip this part
        ldr     r4, [r1, #-4]!                          ; load 4 bytes
        subs    r2, r2, #4                              ; count the bytes for this block
        str     r4, [r3, #-4]!                          ; store 4 bytes
16
        adds    r2, r2, #(4 - 2)                        ; recover original count, and pre-decrement
        blo     %F17                                    ; if not enough remaining, skip this part
        ldrh    r4, [r1, #-2]!                          ; load 2 bytes
        subs    r2, r2, #2                              ; count the bytes for this block
        strh    r4, [r3, #-2]!                          ; store 2 bytes
17
        adds    r2, r2, #2                              ; recover remaining count
        beq     %F18                                    ; if not enough remaining, skip this part
        ldrb    r2, [r1, #-1]                           ; load 1 byte
        strb    r2, [r3, #-1]                           ; store 1 byte
18
        EPILOG_POP {r4-r9, r11, pc}

        NESTED_END __memcpy_reverse_large_integer


;
; __memcpy_reverse_large_neon (internal calling convention)
;
; Copy large (>= 16 bytes) block of memory in a reverse direction,
; using NEON registers.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_reverse_large_neon

        PROLOG_NOP pld [r1, #-32]                       ; pre-load one block ahead
        PROLOG_PUSH {r11, lr}

;
; On the A9 there are substantial penalties for reverse copying without
; aligning, not sure why.
;

        tst     r3, #3                                  ; dword aligned?
        beq     %F10                                    ; if so, skip
9
        subs    r2, r2, #1                              ; take 1 off the count
        ldrb    r12, [r1, #-1]!                         ; finish off remaining one byte
        strb    r12, [r3, #-1]!                         ; at a time, to deal with misaligned
        tst     r3, #3                                  ; dword aligned?
        bne     %F10                                    ; if not, loop
10
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    r2, r2, #32                             ; pre-decrement to simplify the loop
        pld     [r1, #-64]                              ; pre-load one block ahead
        blo     %F12                                    ; skip over the loop if we don't have enough
11
        pld     [r1, #-96]                              ; prefetch ahead
        subs    r1, r1, #32
        subs    r3, r3, #32
        vld1.8  {d0-d3}, [r1]                           ; load 32 bytes
        subs    r2, r2, #32                             ; count the bytes for this block
        vst1.8  {d0-d3}, [r3]                           ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        subs    r1, r1, #32
        subs    r3, r3, #32
        vld1.8  {d0-d3}, [r1]                           ; load 32 bytes
        vst1.8  {d0-d3}, [r3]                           ; store 32 bytes
13
        adds    r2, r2, #(32 - 16)                      ; recover original count, and pre-decrement
        blo     %F14                                    ; if not enough remaining, skip this part
        subs    r1, r1, #16
        subs    r3, r3, #16
        vld1.8  {d0-d1}, [r1]                           ; load 16 bytes
        subs    r2, r2, #16                             ; count the bytes for this block
        vst1.8  {d0-d1}, [r3]                           ; store 16 bytes
14
        adds    r2, r2, #(16 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this loop
        subs    r1, r1, #8
        subs    r3, r3, #8
        vld1.8  {d0}, [r1]                              ; load 8 bytes
        subs    r2, r2, #8                              ; count the bytes for this block
        vst1.8  {d0}, [r3]                              ; store 8 bytes
15
        adds    r2, r2, #(8 - 4)                        ; recover original count, and pre-decrement
        blo     %F16                                    ; if not enough remaining, skip this loop
        subs    r1, r1, #4
        subs    r3, r3, #4
        vld4.8  {d0[0]-d3[0]}, [r1]                     ; load 4 bytes
        subs    r2, r2, #4                              ; count the bytes for this block
        vst4.8  {d0[0]-d3[0]}, [r3]                     ; store 4 bytes
16
        adds    r2, r2, #(4 - 2)                        ; recover original count, and pre-decrement
        blo     %F17                                    ; if not enough remaining, skip this loop
        subs    r1, r1, #2
        subs    r3, r3, #2
        vld2.8  {d0[0]-d1[0]}, [r1]                     ; load 2 bytes
        subs    r2, r2, #2                              ; count the bytes for this block
        vst2.8  {d0[0]-d1[0]}, [r3]                     ; store 2 bytes
17
        adds    r2, r2, #2                              ; recover remaining count
        beq     %F18                                    ; if not enough remaining, skip this part
        ldrb    r2, [r1, #-1]                           ; load 1 byte
        strb    r2, [r3, #-1]                           ; store 1 byte
18
        EPILOG_POP {r11, pc}

        NESTED_END __memcpy_reverse_large_neon


;
; __memcpy_decide (internal calling convention)
;
; Determine whether to use integer or NEON for future memcpy's.
;

        ALIGN 32
        NESTED_ENTRY __memcpy_decide

        PROLOG_PUSH {r4-r5, r11, lr}

        ;
        ; We want to use integer memcpy's on the A9, which has an external cache.
        ;
        ; First determine if we're in user or kernel mode. Reading CPSR
        ; from user mode will either return the proper 5 mode bits, or all 0s.
        ; Conveniently, user mode is 0x10, and there is no mode 0x00, so if
        ; we read CPSR and the low 4 bits are 0, that's good enough.
        ;

        READ_CPSR r4                                    ; get CPSR
        ands    r4, r4, #0xf                            ; isolate the low 4 bits of the mode
        beq     %F1                                     ; if 0, we're in user mode

        ;
        ; If we are in kernel mode, read the MIDR directly.
        ;

        CP_READ r4, CP15_MIDR                           ; read main ID register
        ubfx    r5, r4, #24, #8                         ; get implementer
        lsrs    r4, r4, #4                              ; shift off revision field
        cmp     r5, #0x41                               ; is implementer == ARM?
        bne     %F3                                     ; if not, use NEON
        bfc     r4, #12, #20                            ; clear upper bits
        ldr     r5, =0xc09                              ; A9 signature
        cmp     r4, r5                                  ; is this an A9?
        bne     %F3                                     ; if not, use NEON
        b       %F2                                     ; otherwise, use integer

        ;
        ; If we are in user mode, check the "external cache available" flag
        ;
1
        ldr     r4, =MM_SHARED_USER_DATA_VA + UsProcessorFeatures + PF_ARM_EXTERNAL_CACHE_AVAILABLE
        ldrb    r4, [r4]                                ; get external cache bit
        cbz     r4, %F3                                 ; if no external cache, do NEON

        ;
        ; Register for integer functions
        ;
2
        ldr     r5, =__memcpy_implementation            ; point to the implementation
        mov     r4, #1                                  ; select integer functions
        strb    r4, [r5]                                ;
        b       %F4

        ;
        ; Register for NEON functions
        ;
3
        ldr     r5, =__memcpy_implementation            ; point to the implementation
        mov     r4, #2                                  ; select NEON functions
        strb    r4, [r5]                                ;
4
        adr     r5, __memcpy_functions                  ; get a pointer to the function table
        add     r12, r5, r4, lsl #3                     ; plus 8*function index
        EPILOG_POP {r4-r5, r11, lr}                     ; restore saved registers
        EPILOG_NOP ldr     pc, [r12]                    ; jump to the appropriate target

        NESTED_END __memcpy_decide


#else  /* defined(_BOOTCRT_) */

        AREA    |.text|,ALIGN=5,CODE,READONLY

;
; void *memcpy(void *dst, const void *src, size_t length)
; void *memmove(void *dst, const void *src, size_t length)
; void *_memcpy_strict_align(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward direction, only performing naturally-aligned
; accesses.
;

        ALIGN 32
        NESTED_ENTRY memcpy

        ALTERNATE_ENTRY memmove

        PROLOG_NOP subs    r3, r0, r1                   ; compute dest - source
        PROLOG_NOP cmp     r3, r2                       ; compare against size
        PROLOG_NOP blo.w   __memcpy_reverse             ; if overlap, we must do it in reverse

        ALTERNATE_ENTRY _memcpy_strict_align
        ALTERNATE_ENTRY __memcpy_forward

        PROLOG_PUSH {r4-r9, r11, lr}
        mov     lr, r0                                  ; save dest pointer

;
; Verify alignment between source and destination
;

        cmp     r2, #0
        sub     r3, r0, r1                              ; get relative alignment of source and destination
        beq     CopyExit                                ; exit if 0 count
        ands    r3, r3, #3                              ; check DWORD alignment
        bne     CopyMisalignedHalf                      ; misaligned

;
; Source and destination are equally aligned: just align the
; destination and the source will end up aligned as well
;

        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_0                           ; if so, skip ahead
ByteAlign_0
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        bne     ByteAlign_0                             ; if not, keep copying bytes
WordAligned_0

;
; Copy using bulk register loads/stores
;
; N.B. this code is just a copy of __memcpy_forward_large_integer,
;      altered to use r0 instead of r3 for the dest, and to use r3
;      instead of lr for the 32-byte loads/stores.
;

        subs    r2, r2, #32                             ; take 32 off the top
        blo     %F13                                    ; if not enough, recover and do small copies
        subs    r2, r2, #32                             ; take off another 32
        pld     [r1, #32]                               ; pre-load one block ahead
        blo     %F12                                    ; skip the loop if that's all we have
11
        pld     [r1, #64]                               ; prefetch ahead
        ldm     r1!, {r3-r9, r12}                       ; load 32 bytes
        subs    r2, r2, #32                             ; count the bytes for this block
        stm     r0!, {r3-r9, r12}                       ; store 32 bytes
        bhs     %B11                                    ; keep going until we're done
12
        ldm     r1!, {r3-r9, r12}                       ; load 32 bytes
        stm     r0!, {r3-r9, r12}                       ; store 32 bytes
13
        adds    r2, r2, #(32 - 16)                      ; recover original count, and pre-decrement
        blo     %F14                                    ; if not enough remaining, skip this part
        ldm     r1!, {r4-r7}                            ; load 16 bytes
        subs    r2, r2, #16                             ; count the bytes for this block
        stm     r0!, {r4-r7}                            ; store 16 bytes
14
        adds    r2, r2, #(16 - 8)                       ; recover original count, and pre-decrement
        blo     %F15                                    ; if not enough remaining, skip this part
        ldrd    r4, r5, [r1], #8                        ; load 8 bytes
        subs    r2, r2, #8                              ; count the bytes for this block
        strd    r4, r5, [r0], #8                        ; store 8 bytes
15
        adds    r2, r2, #(8 - 4)                        ; recover original count, and pre-decrement
        blo     %F16                                    ; if not enough remaining, skip this part
        ldr     r4, [r1], #4                            ; load 4 bytes
        subs    r2, r2, #4                              ; count the bytes for this block
        str     r4, [r0], #4                            ; store 4 bytes
16
        adds    r2, r2, #4                              ; recover original count

CopyFinalHalfwords
        subs    r2, r2, #2                              ; at least 2 bytes remaining?
        blt     CopyFinalHalfwordsEnd                   ; if not, skip this
CopyFinalHalfwordsLoop
        subs    r2, r2, #2                              ; decrement count
        ldrh    r3, [r1], #2                            ; fetch halfword
        strh    r3, [r0], #2                            ; store it
        bge     CopyFinalHalfwordsLoop                  ; loop until done
CopyFinalHalfwordsEnd
        adds    r2, r2, #2                              ; recover the extra 2 we subtracted
        beq     CopyExit                                ; stop if that's everything

CopyFinalBytes
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        bne     CopyFinalBytes                          ; loop until done
CopyExit
        mov     r0, lr                                  ; recover dest pointer
        EPILOG_POP {r4-r9, r11, pc}                     ; return

;
; Source and destination are misaligned by 2 bytes
;

CopyMisalignedHalf
        cmp     r3, #2                                  ; misaligned by a halfword?
        bne     CopyMisalignedByte                      ; if not, skip

        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_2                           ; if so, skip ahead
        tst     r0, #1                                  ; halfword aligned at the dest?
        beq     HalfAligned_2                           ; if so, skip ahead

        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        beq     WordAligned_2                           ; if so, skip ahead

HalfAligned_2
        cmp     r2, #2                                  ; do we have at least 2 bytes left?
        blo     CopyFinalBytes                          ; if not, copy bytes
        subs    r2, r2, #2                              ; decrement count
        ldrh    r3, [r1], #2                            ; fetch halfword
        strh    r3, [r0], #2                            ; store it
        beq     CopyExit                                ; stop if done

WordAligned_2
        subs    r2, r2, #6                              ; at least 6 bytes remaining?
        blt     WordLoopEnd_2                           ; if so, skip the main loop
        ldrh    r12, [r1], #2                           ; preload a halfword of source
        subs    r2, r2, #2                              ; count these 2 bytes
WordLoop_2
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        orr     r12, r12, r3, lsl #16                   ; copy low 16 bits to upper 16 of r12
        str     r12, [r0], #4                           ; store it
        lsr     r12, r3, #16                            ; copy upper 16 bits to lower 16 of r12
        bge     WordLoop_2                              ; stop if done
        strh    r12, [r0], #2                           ; store the extra halfword to the dest
WordLoopEnd_2
        adds    r2, r2, #6                              ; recover the extra 6 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalHalfwords                      ; otherwise, copy remainder


;
; Source and destination are misaligned by 1 byte
;

CopyMisalignedByte
        cmp     r3, #1                                  ; misaligned by a byte?
        bne     CopyMisalignedByte3                     ; if not, skip

        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_1                           ; if so, skip ahead
ByteAlign_1
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        bne     ByteAlign_1                             ; if not, keep copying bytes

WordAligned_1
        subs    r2, r2, #5                              ; at least 5 bytes remaining?
        blt     WordLoopEnd_1                           ; if so, skip the main loop
        ldrb    r12, [r1], #1                           ; preload a byte of source
        subs    r2, r2, #1                              ; count this byte
WordLoop_1
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        orr     r12, r12, r3, lsl #8                    ; copy low 24 bits to upper 24 of r12
        str     r12, [r0], #4                           ; store it
        lsr     r12, r3, #24                            ; copy upper 8 bits to lower 8 of r12
        bge     WordLoop_1                              ; stop if done
        strb    r12, [r0], #1                           ; store the extra byte to the dest
WordLoopEnd_1
        adds    r2, r2, #5                              ; recover the extra 5 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalBytes                          ; otherwise, copy remainder


;
; Source and destination are misaligned by 3 bytes
;

CopyMisalignedByte3
        tst     r0, #3                                  ; dword aligned at the dest?
        beq     WordAligned_3                           ; if so, skip ahead
ByteAlign_3
        subs    r2, r2, #1                              ; decrement count
        ldrb    r3, [r1], #1                            ; fetch byte
        strb    r3, [r0], #1                            ; store it
        beq     CopyExit                                ; stop if done
        tst     r0, #3                                  ; word aligned now?
        bne     ByteAlign_3                             ; if not, keep copying bytes

WordAligned_3
        subs    r2, r2, #7                              ; at least 7 bytes remaining?
        blt     WordLoopEnd_3                           ; if so, skip the main loop
        ldrb    r12, [r1], #1                           ; preload a byte of source
        ldrh    r3, [r1], #2                            ; preload a halfword of source
        orr     r12, r12, r3, lsl #8                    ; OR in the halfword
        subs    r2, r2, #3                              ; count these 3 bytes
WordLoop_3
        subs    r2, r2, #4                              ; decrement count
        ldr     r3, [r1], #4                            ; fetch word
        orr     r12, r12, r3, lsl #24                   ; copy low 8 bits to upper 8 of r12
        str     r12, [r0], #4                           ; store it
        lsr     r12, r3, #8                             ; copy upper 24 bits to lower 24 of r12
        bge     WordLoop_3                              ; stop if done
        strh    r12, [r0], #2                           ; store the extra halfword to the dest
        lsr     r12, r12, #16                           ; down to the final byte
        strb    r12, [r0], #1                           ; store the extra byte to the dest
WordLoopEnd_3
        adds    r2, r2, #7                              ; recover the extra 7 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalBytes                          ; otherwise, copy remainder

        NESTED_END

;
; void *memmove(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward or reverse direction, ensuring that
; overlapping source/destination regions are copied correctly.
;

        ALIGN 32
        LEAF_ENTRY __memcpy_reverse

;
; This is a limited-case scenario, just do the simplest thing
;

        adds    r3, r0, r2                              ; advance destination to end
        adds    r1, r1, r2                              ; advance source to end
        cbz     r2, %F2                                 ; skip if nothing to copy
1
        subs    r2, r2, #1                              ; decrement count
        ldrb    r12, [r1, #-1]!                         ; load
        strb    r12, [r3, #-1]!                         ; store
        bne     %B1                                     ; loop until done
2
        bx      lr

        LEAF_END

#endif   /* defined(_BOOTCRT_) */

        END

#endif /* !defined(_CRT_WINDOWS) */
