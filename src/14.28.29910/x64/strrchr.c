/*
 *  Copyright (C) 2009-2010 Intel Corporation.
 *
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/***
*strrchr.c - find last occurrence of character in string
*
*Purpose:
*       defines strrchr() - find the last occurrence of a given character
*       in a string.
*
*******************************************************************************/

#include <vcruntime_internal.h>
#include <intrin.h>
#include <limits.h>

// These flags select the operation performed by _mm_cmp?str? functions.

// PCMPxSTRx character type
#define f_pcmp_ub  0x00
#define f_pcmp_uw  0x01
#define f_pcmp_sb  0x02
#define f_pcmp_sw  0x03
// PCMPxSTRx aggregation operation
#define f_pcmp_ea  0x00
#define f_pcmp_rng 0x04
#define f_pcmp_ee  0x08
#define f_pcmp_eo  0x0C
// PCMPxSTRx result polarity
#define f_pcmp_pp  0x00
#define f_pcmp_np  0x10
#define f_pcmp_mn  0x30
// PCMPxSTRI index order
#define f_pcmp_ls  0x00
#define f_pcmp_ms  0x40
// PCMPxSTRM result unit size
#define f_pcmp_bit  0x00
#define f_pcmp_byte 0x40

// Define flag combinations to use.
#define f_srch_rng (f_pcmp_ub | f_pcmp_rng | f_pcmp_np | f_pcmp_ls)
#define f_srch_set (f_pcmp_ub | f_pcmp_ea | f_pcmp_pp | f_pcmp_ms)

// Define XMM-related constants.
#define XMM_SIZE sizeof(__m128i)
#define XMM_CHARS (sizeof(__m128i)/sizeof(char))
#define XMM_ALIGN(p) ((0-(intptr_t)XMM_SIZE) & (intptr_t)(p))
#define XMM_OFFSET(p) ((XMM_SIZE-1) & (intptr_t)(p))

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

char * __cdecl strrchr (
        const char * string,
        int ch
        )
{
    __m128i *src_a;
    __m128i zero, pattern, characters, temp;
    unsigned __int32 zmask, mmask;
    const char *match, *last_match = NULL;
    unsigned long offset;

    // Search for zero is a special case because the normal algorithms
    // don't work to search for a zero character.

    if (0 == ch)
    {
        src_a = (__m128i*)XMM_ALIGN(string);
        characters = _mm_load_si128(src_a);

        // Get offset to first valid character in block.

        offset = XMM_OFFSET(string);

        // Get terminator character match template.

        zero = _mm_xor_si128(zero, zero);

        // Search for terminator character.

        characters = _mm_cmpeq_epi8(characters, zero);
        zmask = _mm_movemask_epi8(characters) & (~0u << offset);
        while (0 == zmask)
        {
            characters = _mm_load_si128(++src_a);
            characters = _mm_cmpeq_epi8(characters, zero);
            zmask = _mm_movemask_epi8(characters);
        }

        // Set return value to point to terminating zero character.

        _BitScanForward(&offset, zmask);
        return (char *)(offset + (intptr_t)(src_a));
    }

    // Use SSE2 if SSE 4.2 (STTNI) is not available.

    else if (__isa_available < __ISA_AVAILABLE_SSE42)
    {
        // Align characters block pointer and get first character block.

        src_a = (__m128i*)XMM_ALIGN(string);
        characters = _mm_load_si128(src_a);

        // Get offset to first valid character in block.

        offset = XMM_OFFSET(string);

        // Get terminator character match template.

        zero = _mm_xor_si128(zero, zero);

        // Copy character to all positions of XMM register.

        ch = (unsigned char)ch | ((unsigned char)ch << CHAR_BIT);
        pattern = _mm_cvtsi32_si128(ch);
        pattern = _mm_shufflelo_epi16(pattern, 0);
        pattern = _mm_shuffle_epi32(pattern, 0);

        // Check first characters block for ch character and terminator.

        temp = _mm_cmpeq_epi8(characters, zero);
        characters = _mm_cmpeq_epi8(characters, pattern);
        zmask = _mm_movemask_epi8(temp) & (~0u << offset);
        mmask = _mm_movemask_epi8(characters) & (~0u << offset);
        while (0 == zmask)
        {
            // Update last_match if any pattern matches found.

            _BitScanReverse(&offset, mmask);
            match = (char *)(offset + (intptr_t)(src_a));
            last_match = (0 != mmask) ? match : last_match;

            // Get next character block.

            characters = _mm_load_si128(++src_a);

            // Check next character block for ch character and terminator.

            temp = _mm_cmpeq_epi8(characters, pattern);
            characters = _mm_cmpeq_epi8(characters, zero);
            mmask = _mm_movemask_epi8(temp);
            zmask = _mm_movemask_epi8(characters);
        }

        // Note: the following code is structured to avoid generating
        // conditional branches because it is difficult to predict
        // whether mmask will be zero or not.

        // Mask off any bits after terminator.

        mmask &= (zmask & (0 - zmask)) - 1;

        // Update last_match if any pattern matches found.

        _BitScanReverse(&offset, mmask);
        match = (char *)(offset + (intptr_t)(src_a));
        last_match = (0 != mmask) ? match : last_match;

        // Return last match found.

        return (char *)last_match;
    }

    // Use SSE 4.2 (STTNI) if available.

    else
    {
        // Check each character until string pointer reaches the next
        // aligned XMMWORD.
        for (; (XMM_OFFSET(string) != 0); ++string)
        {
            if (*string == ch) last_match = string;
            if (*string == 0) return (char *)last_match;
        }

        // Pattern is one-character string of the target character.
        pattern = _mm_cvtsi32_si128((unsigned char)ch);
        for (;;)
        {
            characters = _mm_load_si128((__m128i*)string);
            if (_mm_cmpistrc(pattern, characters, f_srch_set))
            {
                last_match = _mm_cmpistri(pattern, characters, f_srch_set) + string;
            }
            if (_mm_cmpistrz(pattern, characters, f_srch_set))
            {
                return (char *)last_match;
            }
            string += XMM_SIZE;
        }
    }
}
