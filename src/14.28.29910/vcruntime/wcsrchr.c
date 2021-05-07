/***
*wcsrchr.c - find last occurrence of wchar_t character in wide string
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines wcsrchr() - find the last occurrence of a given character
*       in a string (wide-characters).
*
*******************************************************************************/

#include <vcruntime_internal.h>

/***
*wchar_t *wcsrchr(string, ch) - find last occurrence of ch in wide string
*
*Purpose:
*       Finds the last occurrence of ch in string.  The terminating
*       null character is used as part of the search (wide-characters).
*
*Entry:
*       wchar_t *string - string to search in
*       wchar_t ch - character to search for
*
*Exit:
*       returns a pointer to the last occurrence of ch in the given
*       string
*       returns NULL if ch does not occur in the string
*
*Exceptions:
*
*******************************************************************************/

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

// ARM64 Neon Intrinsics variant
// For long strings, this is faster than the naive version.
// But for short strings there is overhead.

#include <arm64string.h>

// Traverse the string forwards, only once.
// Collect possible matches along the way.

wchar_t * __cdecl wcsrchr (
        const wchar_t * string,
        wchar_t ch
        )
{
    vector_t *src_a, characters, match;
    vector_t chmatch, chmatchrev, zeromatch, zeromatchrev, orrmatches;
    __n64 uaddlvq32;
    unsigned __int64 ch_lword0, ch_lword1, ch_lword0_rev, ch_lword1_rev;
    unsigned __int64 zero_lword0_rev, zero_lword1_rev, mask;
    unsigned __int64 andmask;
    unsigned long offset, ch_bitoffset, zero_bitoffset;
    wchar_t *found = (wchar_t)0;

    if (ch == 0) {
        if (*string == 0) {
            return (wchar_t *)string;
        } else {
            return wcschr_zero_internal(string);
        }
    }

    // Start by getting the aligned XMMWORD containing the first
    // characters of the string. This is done first to partially
    // cover any memory access latency.
    // Use 16 byte alignment throughout, to guarantee page-safe loads.

    src_a = (vector_t*)N128_ALIGN(string);

    // Now create patterns to check for a terminating zero or match.
    // These characters are copied to every position of a XMMWORD.

    match = neon_dupqr16(ch);

    // prepare to mask off any bits before the beginning of the string.

    offset = N128_OFFSET(string);

    {
        // Check initial full or partial XMMWORD

        characters = *src_a;

        // Compare against each pattern to get flags for each match

        chmatch = neon_cmeqq16(characters, match);
        zeromatch = neon_cmeqzq16(characters);

        // reverse order of 16bit elements within each of 2 64-bit vectors:
        // ABCD EFGH => DCBA HGFE

        chmatchrev = neon_rev64q_16(chmatch);
        zeromatchrev = neon_rev64q_16(zeromatch);

        ch_lword1_rev = neon_umovq64(chmatchrev, 1);
        zero_lword1_rev = neon_umovq64(zeromatchrev, 1);

        // For the initial XMMWORD mask off any bits before the beginning
        // of the string.

        if ((offset & 0x8) == 0) {
            zero_lword0_rev = neon_umovq64(zeromatchrev, 0);
            ch_lword0_rev = neon_umovq64(chmatchrev, 0);

            andmask = ~0ull >> (offset << 3);
            ch_lword0_rev = ch_lword0_rev & andmask;
            zero_lword0_rev = zero_lword0_rev & andmask;
        } else {
            andmask = ~0ull >> ((offset - 8) << 3);
            ch_lword0_rev = 0;
            ch_lword1_rev = ch_lword1_rev & andmask;
            zero_lword0_rev = 0;
            zero_lword1_rev = zero_lword1_rev & andmask;
        }

        if ((zero_lword0_rev != 0) || (ch_lword0_rev != 0) || (zero_lword1_rev != 0) || (ch_lword1_rev != 0)) {

            // Scan the lword0, lword1 for the position of the FIRST zero match

            zero_bitoffset = _CountLeadingZeros128(zero_lword0_rev, zero_lword1_rev);

            // Scan the lword0, lword1 for the position of the FIRST character match

            ch_bitoffset = _CountLeadingZeros128(ch_lword0_rev, ch_lword1_rev);

            if (zero_bitoffset < ch_bitoffset) {

                // The next match is the end of the string.

                return found;
            }

            // Take the UNREVERSED match:

            ch_lword0 = neon_umovq64(chmatch, 0);
            ch_lword1 = neon_umovq64(chmatch, 1);

            // zero_bitoffset is in range 0..128
            // Replace test for ==128 with test for bit 1<<7

            if ((zero_bitoffset & 128) != 0) {

                // There is no zero match in this vector.
                // Record the offset of the LAST character match,
                // and advance to the next vector.

                ch_bitoffset = (128 - 16) - _CountLeadingZeros128(ch_lword1, ch_lword0);
                found = (wchar_t*)((ch_bitoffset >> 3) + (intptr_t)(src_a));
            } else {

                // We have zero match after 1 or more character matches in this vector.
                // Mask off all character matches after the FIRST zero match,
                // RETURN the bit position of the LAST character match

                if (zero_lword0_rev != 0) {
                    // Found zero match in first lword. 1 <= zero_bitoffset <= 63
                    ch_lword0 = ch_lword0 & ((1ull << zero_bitoffset) - 1);
                    ch_bitoffset = (64 - 16) - _CountLeadingZeros64(ch_lword0);
                } else {
                    // Found zero match in second lword. 64 <= zero_bitoffset <= 127
                    ch_lword1 = ch_lword1 & ((1ull << (zero_bitoffset - 64)) - 1);
                    ch_bitoffset = (128 - 16) - _CountLeadingZeros128(ch_lword1, ch_lword0);
                }

                found = (wchar_t*)((ch_bitoffset >> 3) + (intptr_t)(src_a));

                return found;
            }
        }
    }

    for (;;) {

        // Check each XMMWORD until the end of the string is found.

        characters = *(++src_a);

        // Compare against each pattern to get flags for each match

        chmatch = neon_cmeqq16(characters, match);
        zeromatch = neon_cmeqzq16(characters);

        orrmatches = neon_orrq(chmatch, zeromatch);
        uaddlvq32 = neon_uaddlvq32(orrmatches);
        mask = neon_umov64(uaddlvq32, 0);

        if (mask != 0) {
            chmatchrev = neon_rev64q_16(chmatch);
            zeromatchrev = neon_rev64q_16(zeromatch);

            ch_lword0_rev = neon_umovq64(chmatchrev, 0);
            zero_lword0_rev = neon_umovq64(zeromatchrev, 0);

            ch_lword1_rev = neon_umovq64(chmatchrev, 1);
            zero_lword1_rev = neon_umovq64(zeromatchrev, 1);

            // Scan the lword0, lword1 for the position of the FIRST zero match

            zero_bitoffset = _CountLeadingZeros128(zero_lword0_rev, zero_lword1_rev);

            // Scan the lword0, lword1 for the position of the FIRST character match

            ch_bitoffset = _CountLeadingZeros128(ch_lword0_rev, ch_lword1_rev);

            if (zero_bitoffset < ch_bitoffset) {

                // The next match is the end of the string.

                return found;
            }

            // Take the UNREVERSED match:

            ch_lword0 = neon_umovq64(chmatch, 0);
            ch_lword1 = neon_umovq64(chmatch, 1);

            // zero_bitoffset is in range 0..128
            // Replace test for ==128 with test for bit 1<<7

            if ((zero_bitoffset & 128) != 0) {

                // There is no zero match in this vector.
                // Record the offset of the LAST character match,
                // and advance to the next vector.

                ch_bitoffset = (128 - 16) - _CountLeadingZeros128(ch_lword1, ch_lword0);
                found = (wchar_t*)((ch_bitoffset >> 3) + (intptr_t)(src_a));

            } else {

                // We have zero match after 1 or more character matches in this vector.
                // Mask off all character matches after the FIRST zero match,
                // RETURN the bit position of the LAST character match

                if (zero_lword0_rev != 0) {
                    // Found zero match in first lword. 1 <= zero_bitoffset <= 63
                    ch_lword0 = ch_lword0 & ((1ull << zero_bitoffset) - 1);
                    ch_bitoffset = (64 - 16) - _CountLeadingZeros64(ch_lword0);
                } else {
                    // Found zero match in second lword. 64 <= zero_bitoffset <= 127
                    ch_lword1 = ch_lword1 & ((1ull << (zero_bitoffset - 64)) - 1);
                    ch_bitoffset = (128 - 16) - _CountLeadingZeros128(ch_lword1, ch_lword0);
                }

                found = (wchar_t*)((ch_bitoffset >> 3) + (intptr_t)(src_a));

                return found;
            }
        }
    }
}

#else

wchar_t * __cdecl wcsrchr (
        const wchar_t * string,
        wchar_t ch
        )
{
        wchar_t *start = (wchar_t *)string;

        while (*string++)                       /* find end of string */
                ;
                                                /* search towards front */
        while (--string != start && *string != (wchar_t)ch)
                ;

        if (*string == (wchar_t)ch)             /* wchar_t found ? */
                return( (wchar_t *)string );

        return(NULL);
}

#endif
