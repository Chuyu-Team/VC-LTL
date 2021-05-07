/*
 *  Copyright (C) 2009-2010 Intel Corporation.
 *
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/***
*wcschr.c - search a wchar_t string for a given wchar_t character
*
*Purpose:
*       defines wcschr() - search a wchar_t string for a wchar_t character
*
*******************************************************************************/

#include <vcruntime_internal.h>
#include <intrin.h>

#define XMM_SIZE sizeof(__m128i)
#define XMM_CHARS (XMM_SIZE / sizeof(wchar_t))

#define PAGE_SIZE ((intptr_t)0x1000)
#define PAGE_OFFSET(p) ((PAGE_SIZE-1) & (intptr_t)(p))
#define XMM_PAGE_SAFE(p) (PAGE_OFFSET(p) <= (PAGE_SIZE - XMM_SIZE))

/***
*wchar_t *wcschr(string, c) - search a string for a wchar_t character
*
*Purpose:
*       Searches a wchar_t string for a given wchar_t character,
*       which may be the null character L'\0'.
*
*Entry:
*       wchar_t *string - wchar_t string to search in
*       wchar_t c - wchar_t character to search for
*
*Exit:
*       returns pointer to the first occurrence of c in string
*       returns NULL if c does not occur in string
*
*Exceptions:
*
*******************************************************************************/

wchar_t * __cdecl wcschr (
        const wchar_t * string,
        wchar_t ch
        )
{
    __m128i match, characters, temp;
    unsigned mask;
    unsigned long offset;

    // Build match pattern with target character in every position.

    match = _mm_cvtsi32_si128(ch);
    match = _mm_shufflelo_epi16(match, 0);
    match = _mm_shuffle_epi32(match, 0);

    for (;;)
    {
        // If the next XMMWORD does not overlap a page boundary check
        // it for match of character or zero.

        if (XMM_PAGE_SAFE(string))
        {
            // Check for match with either the search or zero character.
            // There may be more than one match, but only the first is
            // significant.

            characters = _mm_loadu_si128((__m128i*)string);
            temp = _mm_xor_si128(temp, temp);
            temp = _mm_cmpeq_epi16(temp, characters);
            characters = _mm_cmpeq_epi16(characters, match);
            temp = _mm_or_si128(temp, characters);
            mask = _mm_movemask_epi8(temp);

            // If one or more matches was found, get the position of
            // the first one. If that character is the same as the
            // search character return the pointer to it, otherwise
            // it must be the terminating zero so return NULL.

            if (mask != 0)
            {
                _BitScanForward(&offset, mask);
                string = (wchar_t *)(offset + (intptr_t)string);
                return (*string == ch) ? (wchar_t *)string : NULL;
            }

            // No match found in this XMMWORD so skip to next.

            string += XMM_CHARS;
        }
        else
        {
            // If it is not safe to check an entire XMMWORD, check
            // a single character and try again.

            if (*string == ch) return (wchar_t *)string;
            if (*string == 0) return NULL;

            // No match so skip to next character.

            ++string;
        }
    }
}
