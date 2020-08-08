;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;

#include "ksarm64.h"
#include "arm64/arm64asmsymbolname.h"


        AREA    |.text|,ALIGN=5,CODE,READONLY

;
; void *memcpy(void *dst, const void *src, size_t length)
; void *memmove(void *dst, const void *src, size_t length)
; void *_memcpy_strict_align(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward direction.
;


#if !defined(_BOOTCRT_)
        LEAF_ENTRY A64NAME(memcpy), "", 5
        ALTERNATE_ENTRY A64NAME(memmove)

        sub     x3, x0, x1                              ; compute dest - source
        cmp     x3, x2                                  ; compare against size
        blo     __memcpy_reverse                        ; if overlap, we must do it in reverse
        cbz     x3, __memcpy_ret                        ; if dest==source, no work to do.

        ALTERNATE_ENTRY A64NAME(_memcpy_strict_align)
        ALTERNATE_ENTRY A64NAME(__memcpy_forward)

        prfm    pldl1strm, [x1]                         ; preload the first cache line
        cmp     x2, #8                                  ; less than 8 bytes?
        mov     x3, x0                                  ; use r3 as our destination
        bhs     __memcpy_forward_large_neon             ; if not, just do the large copy case
        tbz     x2, #2, %F16                            ; if not enough remaining, skip this load/store
        ld4     {v0.b,v1.b,v2.b,v3.b}[0], [x1], #4      ; load 4 bytes
        st4     {v0.b,v1.b,v2.b,v3.b}[0], [x3], #4      ; store 4 bytes
16
        tbz     x2, #1, %F17                            ; if not enough remaining, skip this load/store
        ld2     {v0.b,v1.b}[0], [x1], #2                ; load 2 bytes
        st2     {v0.b,v1.b}[0], [x3], #2                ; store 2 bytes
17
        tbz     x2, #0, %F18                            ; if not enough remaining, skip this load/store
        ldrb    w2, [x1]                                ; load 1 byte
        strb    w2, [x3]                                ; store 1 byte
18
__memcpy_ret
        ret                                             ; return

        LEAF_END A64NAME(memcpy)

;
; __memcpy_forward_large_neon (internal calling convention)
;
; Copy large (>= 16 bytes) blocks of memory in a forward direction,
; using NEON registers.
;

        LEAF_ENTRY __memcpy_forward_large_neon, "", 5

        subs    x2, x2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    x2, x2, #32                             ; pre-decrement to simplify the loop
        blo     %F12                                    ; skip over the loop if we don't have enough
        prfm    pldl1strm, [x1, #64]                    ; prefetch the next cacheline
        subs    x2, x2, #64                             ; pre-decrement to simplify the loop
        blo     %F11                                    ; skip over the loop if we don't have enough

        tst     x1, #15                                 ; x1 = (src mod 16)
        bne     %F9                                     ; no q-register if src not 16 byte aligned
        tst     x3, #15                                 ; x3 = (dst mod 16)
        beq     %F10                                    ; no q-register if dst not 16 byte aligned
9
        ld1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x1], #64; load 64 bytes
        ld1     {v4.16b,v5.16b,v6.16b,v7.16b}, [x1], #64; load 64 bytes

        prfm    pldl1strm, [x1, #128]                   ; prefetch a cacheline one block ahead
        prfm    pldl1strm, [x1, #192]                   ; prefetch another cacheline ahead

        st1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x3], #64; store 64 bytes
        st1     {v4.16b,v5.16b,v6.16b,v7.16b}, [x3], #64; store 64 bytes

        subs    x2, x2, #128                            ; count the bytes for this block
        bhs     %B9                                     ; keep going until we're done

        ;; use tbz instead of incrementing counter
        tbz     x2, #6, %F12                            ; if not enough remaining, skip this load/store
        b       %F11
10
        ldp     q0, q1, [x1]
        ldp     q2, q3, [x1, #32]
        ldp     q4, q5, [x1, #64]
        ldp     q6, q7, [x1, #96]

        add     x1, x1, #128

        prfm    pldl1strm, [x1, #128]                   ; prefetch a cacheline one block ahead
        prfm    pldl1strm, [x1, #192]                   ; prefetch another cacheline ahead

        stp     q0, q1, [x3]
        stp     q2, q3, [x3, #32]
        stp     q4, q5, [x3, #64]
        stp     q6, q7, [x3, #96]

        add     x3, x3, #128

        subs    x2, x2, #128                            ; count the bytes for this block
        bhs     %B10                                    ; keep going until we're done

        ;; use tbz instead of incrementing counter
        tbz     x2, #6, %F12                            ; if not enough remaining, skip this load/store
11
        ld1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x1], #64; load 64 bytes
        st1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x3], #64; store 64 bytes
12
        tbz     x2, #5, %F13                            ; if not enough remaining, skip this load/store
        ld1     {v0.16b,v1.16b}, [x1], #32              ; load 32 bytes
        st1     {v0.16b,v1.16b}, [x3], #32              ; store 32 bytes
13
        tbz     x2, #4, %F14                            ; if not enough remaining, skip this load/store
        ld1     {v0.16b}, [x1], #16                     ; load 16 bytes
        st1     {v0.16b}, [x3], #16                     ; store 16 bytes
14
        tbz     x2, #3, %F15                            ; if not enough remaining, skip this load/store
        ld1     {v0.8b}, [x1], #8                       ; load 8 bytes
        st1     {v0.8b}, [x3], #8                       ; store 8 bytes
15
        tbz     x2, #2, %F16                            ; if not enough remaining, skip this loop
        ld4     {v0.b,v1.b,v2.b,v3.b}[0], [x1], #4      ; load 4 bytes
        st4     {v0.b,v1.b,v2.b,v3.b}[0], [x3], #4      ; store 4 bytes
16
        tbz     x2, #1, %F17                            ; if not enough remaining, skip this load/store
        ld2     {v0.b,v1.b}[0], [x1], #2                ; load 2 bytes
        st2     {v0.b,v1.b}[0], [x3], #2                ; store 2 bytes
17
        tbz     x2, #0, %F18                            ; if not enough remaining, skip this load/store
        ldrb    w2, [x1]                                ; load 1 byte
        strb    w2, [x3]                                ; store 1 byte
18
        ret

        LEAF_END __memcpy_forward_large_neon

;
; void *memmove(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward or reverse direction, ensuring that
; overlapping source/destination regions are copied correctly.
;

        LEAF_ENTRY __memcpy_reverse, "", 5

        add     x1, x1, x2                              ; advance source to end
        prfum   pldl1strm, [x1,#-1]                     ; preload the first cache line
        cmp     x2, #8                                  ; less than 8 bytes?
        add     x3, x0, x2                              ; advance destination to end
        bhs     __memcpy_reverse_large_neon             ; go to the small copy case directly
        ;; use tbz instead of decrementing counter
        tbz     x2, #2, %F16                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #4
        sub     x3, x3, #4
        ld4     {v0.b,v1.b,v2.b,v3.b}[0], [x1]          ; load 4 bytes
        st4     {v0.b,v1.b,v2.b,v3.b}[0], [x3]          ; store 4 bytes
16
        tbz     x2, #1, %F17                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #2
        sub     x3, x3, #2
        ld2     {v0.b,v1.b}[0], [x1]                    ; load 2 bytes
        st2     {v0.b,v1.b}[0], [x3]                    ; store 2 bytes
17
        tbz     x2, #0, %F18                            ; if not enough remaining, skip this load/store
        ldrb    w2, [x1, #-1]!                          ; load 1 byte
        strb    w2, [x3, #-1]!                          ; store 1 byte
18
        ret                                             ; return

        LEAF_END __memcpy_reverse


;
; __memcpy_reverse_large_neon (internal calling convention)
;
; Copy large (>= 16 bytes) block of memory in a reverse direction,
; using NEON registers.
;

        LEAF_ENTRY __memcpy_reverse_large_neon, "", 5

        subs    x2, x2, #32                             ; pre-decrement to simplify the loop
        blo     %F13                                    ; skip over the loop if we don't have enough
        subs    x2, x2, #32                             ; pre-decrement to simplify the loop
        blo     %F12                                    ; skip over the loop if we don't have enough
        prfum   pldl1strm, [x1, #-65]                   ; prefetch the next cacheline backwards
        subs    x2, x2, #64                             ; pre-decrement to simplify the loop
        blo     %F11                                    ; skip over the loop if we don't have enough

        tst     x1, #15                                 ; x1 = (src mod 16)
        bne     %F9                                     ; no q-register if src not 16 byte aligned
        tst     x3, #15                                 ; x3 = (dst mod 16)
        beq     %F10                                    ; no q-register if dst not 16 byte aligned
9
        sub     x1, x1, #64
        ld1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x1]     ; load 64 bytes
        sub     x1, x1, #64
        ld1     {v4.16b,v5.16b,v6.16b,v7.16b}, [x1]     ; load 64 bytes

        prfum   pldl1strm, [x1, #-129]                  ; prefetch a cacheline one block backwards
        prfum   pldl1strm, [x1, #-193]                  ; prefetch another cacheline backwards

        sub     x3, x3, #64
        st1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x3]     ; store 64 bytes
        sub     x3, x3, #64
        st1     {v4.16b,v5.16b,v6.16b,v7.16b}, [x3]     ; store 64 bytes

        subs    x2, x2, #128                            ; count the bytes for this block
        bhs     %B9                                     ; keep going until we're done

        b       %F11
10
        sub     x1, x1, #128

        ldp     q6, q7, [x1, #96]
        ldp     q4, q5, [x1, #64]

        sub     x3, x3, #128

        ldp     q2, q3, [x1, #32]
        ldp     q0, q1, [x1]

        prfum   pldl1strm, [x1, #-129]                  ; prefetch a cacheline one block backwards
        prfum   pldl1strm, [x1, #-193]                  ; prefetch another cacheline backwards

        stp     q6, q7, [x3, #96]
        stp     q4, q5, [x3, #64]
        stp     q2, q3, [x3, #32]
        stp     q0, q1, [x3]

        subs    x2, x2, #128                            ; count the bytes for this block
        bhs     %B10                                    ; keep going until we're done
11
        tbz     x2, #6, %F12                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #64
        sub     x3, x3, #64
        ld1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x1]     ; load 64 bytes
        st1     {v0.16b,v1.16b,v2.16b,v3.16b}, [x3]     ; store 64 bytes
12
        tbz     x2, #5, %F13                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #32
        sub     x3, x3, #32
        ld1     {v0.16b,v1.16b}, [x1]                   ; load 32 bytes
        st1     {v0.16b,v1.16b}, [x3]                   ; store 32 bytes
13
        tbz     x2, #4, %F14                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #16
        sub     x3, x3, #16
        ld1     {v0.16b}, [x1]                          ; load 16 bytes
        st1     {v0.16b}, [x3]                          ; store 16 bytes
14
        tbz     x2, #3, %F15                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #8
        sub     x3, x3, #8
        ld1     {v0.8b}, [x1]                           ; load 8 bytes
        st1     {v0.8b}, [x3]                           ; store 8 bytes
15
        tbz     x2, #2, %F16                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #4
        sub     x3, x3, #4
        ld4     {v0.b,v1.b,v2.b,v3.b}[0], [x1]          ; load 4 bytes
        st4     {v0.b,v1.b,v2.b,v3.b}[0], [x3]          ; store 4 bytes
16
        tbz     x2, #1, %F17                            ; if not enough remaining, skip this load/store
        sub     x1, x1, #2
        sub     x3, x3, #2
        ld2     {v0.b,v1.b}[0], [x1]                    ; load 2 bytes
        st2     {v0.b,v1.b}[0], [x3]                    ; store 2 bytes
17
        tbz     x2, #0, %F18                            ; if not enough remaining, skip this load/store
        ldrb    w2, [x1, #-1]!                          ; load 1 byte
        strb    w2, [x3, #-1]!                          ; store 1 byte
18
        ret

        LEAF_END __memcpy_reverse_large_neon


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

        NESTED_ENTRY memcpy, "", "", 5

        ALTERNATE_ENTRY memmove

        sub     x3, x0, x1                   ; compute dest - source
        cmp     x3, x2                       ; compare against size
        blo     __memcpy_reverse             ; if overlap, we must do it in reverse

        ALTERNATE_ENTRY _memcpy_strict_align
        ALTERNATE_ENTRY __memcpy_forward

;
; Verify alignment between source and destination
;

        sub     x4, x0, x1                              ; get relative alignment of source and destination
        neg     x5, x0                                  ; neg -dest in x4
        cbz     x2, CopyExit                            ; exit if 0 count
        and     x4, x4, #7                              ; relative alignment
        and     x5, x5, #7                              ; determine how many bytes to align dest

        subs    x2, x2, x5                              ; pre-decrement by the bytes needed to align
        mov     x3, x0                                  ; use x3 as dest
        ble     CantEvenAlign                           ; if that's too many, just use the final copy
                                                        ; x4 is untrusted and feeds into an indir, but
                                                        ; does not need a speculation block because
                                                        ; it's already bounded 0-7
        adr     x6, Table                               ; point to lookup table
        ldrb    w7, [x6, x4]                            ; get offset to code
        cbz     x5, %F2                                 ; skip if no alignment needed
1
        subs    x5, x5, #1                              ; count this byte
        ldrb    w8, [x1], #1                            ; fetch byte
        strb    w8, [x3], #1                            ; store it
        bne     %B1                                     ; loop until done
2
        add     x6, x6, x7, lsl #2                      ; compute final destination
        br      x6                                      ; go there

Table   dcb     (CopyMisaligned_0 - Table) / 4
        dcb     (CopyMisaligned_1 - Table) / 4
        dcb     (CopyMisaligned_2 - Table) / 4
        dcb     (CopyMisaligned_3 - Table) / 4
        dcb     (CopyMisaligned_4 - Table) / 4
        dcb     (CopyMisaligned_5 - Table) / 4
        dcb     (CopyMisaligned_6 - Table) / 4
        dcb     (CopyMisaligned_7 - Table) / 4

CantEvenAlign
        add     x2, x2, x5                              ; recover the original count
CopyFinalBytes
        subs    x2, x2, #1                              ; count this byte
        ldrb    w8, [x1], #1                            ; fetch byte
        strb    w8, [x3], #1                            ; store it
        bne     CopyFinalBytes                          ; loop until done
CopyExit
        ret


        MACRO
        COPY_MISALIGNED $Align

CopyMisaligned_$Align
        subs    x2, x2, #8+$Align                       ; at least 8 + $Align bytes remaining?
        blt     %F2                                     ; if so, skip the main loop

        IF $Align==1
        ldrb    w8, [x1], #1                            ; preload source
        ENDIF
        IF $Align==2
        ldrh    w8, [x1], #2                            ; preload source
        ENDIF
        IF $Align==3
        ldrb    w8, [x1], #1                            ; preload source
        ldrh    w9, [x1], #2                            ; preload source
        orr     x8, x8, x9, lsl #8                      ; combine
        ENDIF
        IF $Align==4
        ldr     w8, [x1], #4                            ; preload source
        ENDIF
        IF $Align==5
        ldrb    w8, [x1], #1                            ; preload source
        ldr     w9, [x1], #4                            ; preload source
        orr     x8, x8, x9, lsl #8                      ; combine
        ENDIF
        IF $Align==6
        ldrh    w8, [x1], #2                            ; preload source
        ldr     w9, [x1], #4                            ; preload source
        orr     x8, x8, x9, lsl #16                     ; combine
        ENDIF
        IF $Align==7
        ldrb    w8, [x1], #1                            ; preload source
        ldrh    w9, [x1], #2                            ; preload source
        ldr     w10, [x1], #4                           ; preload source
        orr     x8, x8, x9, lsl #8                      ; combine
        orr     x8, x8, x10, lsl #24                    ; combine
        ENDIF

        IF $Align!=0
        sub     x2, x2, #$Align                         ; count what we just loaded
        ENDIF
1
        subs    x2, x2, #8                              ; decrement count
        ldr     x9, [x1], #8                            ; fetch dword
        IF $Align==0
        str     x9, [x3], #8                            ; store it
        ELSE
        orr     x8, x8, x9, lsl #(8*$Align)             ; copy low bits to upper bits of x8
        str     x8, [x3], #8                            ; store it
        lsr     x8, x9, #(64 - 8*$Align)                ; copy upper 8 bits to lower 8 of x8
        ENDIF
        bge     %B1                                     ; stop if done

        IF $Align==1
        strb    w8, [x3], #1                            ; write to destination
        ENDIF
        IF $Align==2
        strh    w8, [x3], #2                            ; write to destination
        ENDIF
        IF $Align==3
        lsr     x9, x8, #16                             ; extract
        strh    w8, [x3], #2                            ; write to destination
        strb    w9, [x3], #1                            ; write to destination
        ENDIF
        IF $Align==4
        str     w8, [x3], #4                            ; write to destination
        ENDIF
        IF $Align==5
        lsr     x9, x8, #32                             ; extract
        str     w8, [x3], #4                            ; write to destination
        strb    w9, [x3], #1                            ; write to destination
        ENDIF
        IF $Align==6
        lsr     x9, x8, #32                             ; extract
        str     w8, [x3], #4                            ; write to destination
        strh    w9, [x3], #2                            ; write to destination
        ENDIF
        IF $Align==7
        lsr     x9, x8, #32                             ; extract
        lsr     x10, x8, #48                            ; extract
        str     w8, [x3], #4                            ; write to destination
        strh    w9, [x3], #2                            ; write to destination
        strb    w10, [x3], #1                           ; write to destination
        ENDIF

2
        adds    x2, x2, #8+$Align                       ; recover the extra 5 we subtracted
        beq     CopyExit                                ; stop if that's everything
        b       CopyFinalBytes                          ; otherwise, copy remainder

        MEND

        COPY_MISALIGNED 0
        COPY_MISALIGNED 1
        COPY_MISALIGNED 2
        COPY_MISALIGNED 3
        COPY_MISALIGNED 4
        COPY_MISALIGNED 5
        COPY_MISALIGNED 6
        COPY_MISALIGNED 7

        LEAF_END


;
; void *memmove(void *dst, const void *src, size_t length)
;
; Copy a block of memory in a forward or reverse direction, ensuring that
; overlapping source/destination regions are copied correctly.
;

        LEAF_ENTRY __memcpy_reverse, "", 5

;
; This is a limited-case scenario, just do the dumbest thing
;

        add     x3, x0, x2                              ; advance destination to end
        add     x1, x1, x2                              ; advance source to end
        cbz     x2, %F2                                 ; skip if nothing to copy
1
        subs    x2, x2, #1                              ; decrement count
        ldrb    w8, [x1, #-1]!                          ; load
        strb    w8, [x3, #-1]!                          ; store
        bne     %B1                                     ; loop until done
2
        ret

        LEAF_END

#endif   /* defined(_BOOTCRT_) */

        END
