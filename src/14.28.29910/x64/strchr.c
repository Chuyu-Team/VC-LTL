/*
 *  Copyright (C) 2009-2010 Intel Corporation.
 *
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/***
*strchr.c - search a string for a given character
*
*Purpose:
*       defines strchr() - search a string for a character
*
*******************************************************************************/

#include <vcruntime_internal.h>
#include <intrin.h>
#include <limits.h>

#define XMM_SIZE sizeof(__m128i)
#define XMM_OFFSET(p) ((XMM_SIZE-1) & (intptr_t)(p))
#define XMM_ALIGN(p) ((0-(intptr_t)XMM_SIZE) & (intptr_t)(p))

/***
*char *strchr(string, c) - search a string for a character
*
*Purpose:
*       Searches a string for a given character, which may be the
*       null character '\0'.
*
*Entry:
*       char *string - string to search in
*       char c - character to search for
*
*Exit:
*       returns pointer to the first occurrence of c in string
*       returns NULL if c does not occur in string
*
*Exceptions:
*
*******************************************************************************/

char * __cdecl strchr (
        const char * string,
        int ch
        )
{
    __m128i *src_a;
    char *end;
    __m128i zero, match, characters, temp;
    unsigned mask;
    unsigned long offset;

    // Start by getting the aligned XMMWORD containing the first
    // characters of the string. This is done first to partially
    // cover any memory access latency.

    src_a = (__m128i*)XMM_ALIGN(string);
    characters = _mm_load_si128(src_a);

    // Now create patterns to check for a terminating zero or match.
    // These characters are copied to every position of a XMMWORD.

    zero = _mm_xor_si128(zero, zero);
    ch = (unsigned char)ch | ((unsigned char)ch << CHAR_BIT);
    match = _mm_cvtsi32_si128(ch);
    match = _mm_shufflelo_epi16(match, 0);
    match = _mm_shuffle_epi32(match, 0);

    // Do initial check. Compare against each pattern to get flags for
    // each match, then copy one bit of each flag to a mask value.

    temp = _mm_cmpeq_epi8(characters, match);
    characters = _mm_cmpeq_epi8(characters, zero);
    temp = _mm_or_si128(temp, characters);

    // For the initial XMMWORD mask off any bits before the beginning
    // of the string.
    offset = XMM_OFFSET(string);
    mask = _mm_movemask_epi8(temp) & (~0u << offset);

    // Check each XMMWORD until a match or the end of the string is found.
    while (0 == mask)
    {
        // Load and check each subsequent XMMWORD.

        characters = _mm_load_si128(++src_a);
        temp = _mm_cmpeq_epi8(characters, match);
        characters = _mm_cmpeq_epi8(characters, zero);
        temp = _mm_or_si128(temp, characters);
        mask = _mm_movemask_epi8(temp);
    }

    // Scan the mask for the position of the first match or end.

    _BitScanForward(&offset, mask);

    // Add the offset to the XMMWORD address to get a pointer
    // to the significant character.

    end = (char *)(offset + (intptr_t)(src_a));

    // If the character is a match return pointer, otherwise
    // it must be the terminating zero and return NULL.

    return (*end == (char)ch) ? end : NULL;
}
