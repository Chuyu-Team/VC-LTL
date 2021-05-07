/***
*memchr.c - search block of memory for a given character
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines memchr() - search memory until a character is
*       found or a limit is reached.
*
*******************************************************************************/

#include <string.h>

#pragma function(memchr)

/***
*char *memchr(buf, chr, cnt) - search memory for given character.
*
*Purpose:
*       Searches at buf for the given character, stopping when chr is
*       first found or cnt bytes have been searched through.
*
*Entry:
*       void *buf  - memory buffer to be searched
*       int chr    - character to search for
*       size_t cnt - max number of bytes to search
*
*Exit:
*       returns pointer to first occurrence of chr in buf
*       returns NULL if chr not found in the first cnt bytes
*
*Exceptions:
*
*******************************************************************************/
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

#include <arm64string.h>

#if defined(_M_ARM64EC)
#include "..\arm64ec\arm64ec_string_thunk_decls.c"
#endif

void * __cdecl memchr (
        const void * buf,
        int chr,
        size_t cnt
        )
{
    __n64 *src_a64, characters64, match64, chmatch64, chmatchrev64;
    vector_t *src_a;
    unsigned __int64 ch_lword0, mask;
    int ch_bitoffset, offset;
    size_t count;
    char *end;

    if (cnt < 8) {
        // Must not read bytes beyond cnt, so process them bytewise if they are less than a vector width.
        // In any case, for less than about 4 bytes, bytewise is faster than the vector implementation.

        unsigned char *chbuf = (unsigned char *)buf;

        while ( cnt && (*chbuf++ != (unsigned char)chr) ) {
            cnt--;
        }

        return(cnt ? (void *)--chbuf : NULL);
    }

    count = cnt;

    // Start by getting the aligned XMMWORD containing the first
    // characters of the buf. This is done first to partially
    // cover any memory access latency.
    // No perf benefit from adjusting for 8 byte alignment.

    src_a64 = (__n64 *)buf;
    characters64 = *(src_a64);

    // Now create patterns to check for a terminating zero or match.
    // These characters are copied to every position of a XMMWORD.

    match64 = neon_dupr8(chr);

    // Do initial check. Compare against each pattern to get bytewise flags for
    // each match.

    chmatch64 = neon_cmeq8(characters64, match64);

    // reduce overhead in short case. handle first 64bits (half vector) quickly

    chmatchrev64 = neon_rev64_8(chmatch64);
    ch_lword0 = neon_umov64(chmatchrev64, 0);

    // Scan the lword0, lword1 for the position of the first character match

    ch_bitoffset = _CountLeadingZeros64(ch_lword0);

    if (ch_bitoffset != 64) {
        end = (char *)((ch_bitoffset >> 3) + (intptr_t)(src_a64));
        return (end < ((char *)buf) + cnt) ? end : NULL;
    }

    count -= 8;
    src_a64++;
    src_a = (vector_t*) src_a64;

    if (count >= 16) {
        vector_t characters, match, chmatch, chmatchrev;
        unsigned __int64 lword1, lword0;
        __n64 uaddlvq32;

        match = neon_dupqr8(chr);

        // Unrolling by a factor of 8 is slightly faster in long cases
        // but much slower in short cases.

        // Unroll by a factor of 4. Use 2 x LDP instruction to load 4 Q regs.
        // The loop is 7% faster than unrolling twice

        while ((count >= 64))
        {
            vector_t characters2, chmatch2;
            vector_t characters3, chmatch3;
            vector_t characters4, chmatch4;
            vector_t chmatch12, chmatch34, chmatch1234;

            // Load and check each subsequent XMMWORD.

            characters = *(src_a++);
            chmatch = neon_cmeqq8(characters, match);

            characters2 = *(src_a++);
            chmatch2 = neon_cmeqq8(characters2, match);

            characters3 = *(src_a++);
            chmatch3 = neon_cmeqq8(characters3, match);

            characters4 = *(src_a++);
            chmatch4 = neon_cmeqq8(characters4, match);

            chmatch12 = neon_orrq(chmatch, chmatch2);
            chmatch34 = neon_orrq(chmatch3, chmatch4);
            chmatch1234 = neon_orrq(chmatch12, chmatch34);

            // unsigned add across long vector. non zero means we have a match
            uaddlvq32 = neon_uaddlvq32(chmatch1234);
            mask = neon_umov64(uaddlvq32, 0);
            count -= 64;
            if (mask != 0) {
                // There is a match in chmatch or chmatch2 or chmatch3 or chmatch4.

                uaddlvq32 = neon_uaddlvq32(chmatch12);
                mask = neon_umov64(uaddlvq32, 0);
                if (mask != 0) { // match is in vectors 1 or 2
                    uaddlvq32 = neon_uaddlvq32(chmatch);
                    mask = neon_umov64(uaddlvq32, 0);
                    if (mask != 0) { // match is in vector 1
                        src_a -= 4;
                        chmatchrev = neon_rev64q_8(chmatch);
                    } else { // match is in vector 2
                        src_a -= 3;
                        chmatchrev = neon_rev64q_8(chmatch2);
                    }
                } else { // match is in vectors 3 or 4
                    uaddlvq32 = neon_uaddlvq32(chmatch3);
                    mask = neon_umov64(uaddlvq32, 0);
                    if (mask != 0) { // match is in vector 3
                        src_a -= 2;
                        chmatchrev = neon_rev64q_8(chmatch3);
                    } else { // match is in vector 4
                        src_a -= 1;
                        chmatchrev = neon_rev64q_8(chmatch4);
                    }
                }

                // move the chmatch into lword1, lword0

                lword0 = neon_umovq64(chmatchrev, 0);
                lword1 = neon_umovq64(chmatchrev, 1);

                // Scan the lword1,lword0 for the position of the first match

                ch_bitoffset = _CountLeadingZeros128(lword0, lword1);
                offset = ch_bitoffset >> 3;

                // Add the offset to the XMMWORD address to get a pointer
                // to the significant character.

                end = (char *)(offset + (intptr_t)(src_a));

                // Address is in range. Return pointer;

                return end;
            }
        }

        // Check each XMMWORD until a match or the end of the buf is found.

        while (count >= 16)
        {
            // Load and check each subsequent XMMWORD.

            characters = *(src_a++);
            chmatch = neon_cmeqq8(characters, match);

            // unsigned add across long vector. non zero means we have a match

            uaddlvq32 = neon_uaddlvq32(chmatch);
            mask = neon_umov64(uaddlvq32, 0);
            count -= 16;

            if (mask != 0) {
                chmatchrev = neon_rev64q_8(chmatch);

                // move the chmatch into lword1, lword0

                lword0 = neon_umovq64(chmatchrev, 0);
                lword1 = neon_umovq64(chmatchrev, 1);

                // Scan the lword1,lword0 for the position of the first match

                ch_bitoffset = _CountLeadingZeros128(lword0, lword1);
                offset = ch_bitoffset >> 3;

                // Add the offset to the XMMWORD address to get a pointer
                // to the significant character.

                end = (char *)(offset + (intptr_t)(src_a - 1));

                // We know address is in range, so return pointer;

                return end;
            }
        }
    }

    // Must not access memory beyond cnt.
    // So, for the last 16 bytes, do not use a 16 byte vector access
    // If we have 8 bytes left, it is safe to use an 8 byte vector access.

    if (count >= 8) {
        src_a64 = (__n64 *)src_a;
        characters64 = *src_a64;

        chmatch64 = neon_cmeq8(characters64, match64);

        // reduce overhead in short case. handle first 64bits (half vector) quickly

        chmatchrev64 = neon_rev64_8(chmatch64);
        ch_lword0 = neon_umov64(chmatchrev64, 0);

        // Scan the lword0, lword1 for the position of the first character match

        ch_bitoffset = _CountLeadingZeros64(ch_lword0);

        if (ch_bitoffset != 64) {
            end = (char *)((ch_bitoffset >> 3) + (intptr_t)(src_a64));
            return end;
        }

        count -= 8;
        src_a64++;
        src_a = (vector_t *)src_a64;
    }

    // Must not access memory beyond cnt.
    // So, for the last <8 bytes, use bytewise access.

    unsigned char *chbuf = (unsigned char *)src_a;

    while ( count && (*chbuf++ != (unsigned char)chr) ) {
        count--;
    }

    return(count ? (void *)--chbuf : NULL);
}

#else

void * __cdecl memchr (
        const void * buf,
        int chr,
        size_t cnt
        )
{
        while ( cnt && (*(unsigned char *)buf != (unsigned char)chr) ) {
                buf = (unsigned char *)buf + 1;
                cnt--;
        }

        return(cnt ? (void *)buf : NULL);
}

#endif
