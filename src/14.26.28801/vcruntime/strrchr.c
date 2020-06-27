/***
*strrchr.c - find last occurrence of character in string
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines strrchr() - find the last occurrence of a given character
*       in a string.
*
*******************************************************************************/

#include <vcruntime_internal.h>

/***
*char *strrchr(string, ch) - find last occurrence of ch in string
*
*Purpose:
*       Finds the last occurrence of ch in string.  The terminating
*       null character is used as part of the search.
*
*Entry:
*       char *string - string to search in
*       char ch - character to search for
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

#include <arm64string.h>

// ARM64 Neon Intrinsics variant

char * __cdecl strrchr (
        const char * string,
        int ch
        )
{
    vector_t *src_a, characters, match;
    vector_t chmatch, chmatchrev, zeromatch, zeromatchrev;
    unsigned __int64 ch_lword0, ch_lword1;
    unsigned __int64 zero_lword0, zero_lword1;
    unsigned __int64 andmask;
    unsigned long ch_bitoffset, zero_bitoffset;
    unsigned long offset;
    char *found = (char )0;

    if (ch == 0) {
        if (*string == 0)
            return (char *)string;
        else
            return strchr_zero_internal(string);
    }

    // Start by getting the aligned XMMWORD containing the first
    // characters of the string. This is done first to partially
    // cover any memory access latency.
    // Use 16 byte alignment throughout, to guarantee page-safe loads.

    src_a = (vector_t*)N128_ALIGN(string);

    characters = *(src_a);

    // Now create patterns to check for a terminating zero or match.
    // These characters are copied to every position of a XMMWORD.

    match = neon_dupqr8(ch);

    // prepare to mask off any bits before the beginning of the string.

    offset = N128_OFFSET(string);

    // Check each XMMWORD until the end of the string is found.

    // Compare against each pattern to get flags for each match

    chmatch = neon_cmeqq8(characters, match);
    zeromatch = neon_cmeqzq8(characters);

    // reverse order of 8bit elements within each 64-bit vector:
    // ABCDEFGH => HGFEDCBA

    chmatchrev = neon_rev64q_8(chmatch);
    zeromatchrev = neon_rev64q_8(zeromatch);

    ch_lword1 = neon_umovq64(chmatchrev, 1);
    zero_lword1 = neon_umovq64(zeromatchrev, 1);

    // For the initial XMMWORD mask off any bits before the beginning
    // of the string.

    if ((offset & 0x8) == 0) {
        ch_lword0 = neon_umovq64(chmatchrev, 0);
        zero_lword0 = neon_umovq64(zeromatchrev, 0);

        andmask = ~0ull >> (offset << 3);
        ch_lword0 = ch_lword0 & andmask;
        zero_lword0 = zero_lword0 & andmask;
    } else {
        andmask = ~0ull >> ((offset - 8) << 3);
        ch_lword0 = 0;
        zero_lword0 = 0;
        ch_lword1 = ch_lword1 & andmask;
        zero_lword1 = zero_lword1 & andmask;
    }

    for (;;) {
        if ((zero_lword0 != 0) || (ch_lword0 != 0) || (zero_lword1 != 0) || (ch_lword1 != 0)) {

            // Scan zero_lword0, lword1 for the position of the first zero match

            zero_bitoffset = _CountLeadingZeros128(zero_lword0, zero_lword1);

            // Scan ch_lword0, lword1 for the position of the first character match

            ch_bitoffset = _CountLeadingZeros128(ch_lword0, ch_lword1);

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

                ch_bitoffset = 124 - _CountLeadingZeros128(ch_lword1, ch_lword0);
                found = (char*)((ch_bitoffset >> 3) + (intptr_t)(src_a));
            } else {

                // We have zero match after 1 or more character matches in this vector.
                // Mask off all character matches after the FIRST zero match,
                // RETURN the bit position of the LAST character match

                if (zero_lword0 != 0) {
                    // Found zero match in first lword. 1 <= zero_bitoffset <= 63
                    ch_lword0 = ch_lword0 & ((1ull << zero_bitoffset) - 1);
                    ch_bitoffset = (124 - 64) - _CountLeadingZeros64(ch_lword0);
                } else {
                    // Found zero match in second lword. 64 <= zero_bitoffset <= 127
                    ch_lword1 = ch_lword1 & ((1ull << (zero_bitoffset - 64)) - 1);
                    ch_bitoffset = 124 - _CountLeadingZeros128(ch_lword1, ch_lword0);
                }

                found = (char*)((ch_bitoffset >> 3) + (intptr_t)(src_a));

                return found;
            }
        }
        characters = *(++src_a);

        // Check each XMMWORD until the end of the string is found.

        // Compare against each pattern to get flags for each match

        chmatch = neon_cmeqq8(characters, match);
        zeromatch = neon_cmeqzq8(characters);

        // reverse order of 8bit elements within each 64-bit vector:
        // ABCDEFGH => HGFEDCBA

        chmatchrev = neon_rev64q_8(chmatch);
        zeromatchrev = neon_rev64q_8(zeromatch);

        ch_lword0 = neon_umovq64(chmatchrev, 0);
        ch_lword1 = neon_umovq64(chmatchrev, 1);

        zero_lword0 = neon_umovq64(zeromatchrev, 0);
        zero_lword1 = neon_umovq64(zeromatchrev, 1);
    }
}

#else

char * __cdecl strrchr (
        const char * string,
        int ch
        )
{
        char *start = (char *)string;

        while (*string++)                       /* find end of string */
                ;
                                                /* search towards front */
        while (--string != start && *string != (char)ch)
                ;

        if (*string == (char)ch)                /* char found ? */
                return( (char *)string );

        return(NULL);
}

#endif
