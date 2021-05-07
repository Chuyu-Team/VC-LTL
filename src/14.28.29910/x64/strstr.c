/*
 *  Copyright (C) 2009-2010 Intel Corporation.
 *
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/***
*strstr.c - search for one string inside another
*
*Purpose:
*       defines strstr() - search for one string inside another
*
*******************************************************************************/

#include <vcruntime_internal.h>
#include <intrin.h>
#include <limits.h>

// These flags select the operation performed by _mm_cmp?str? functions.

// PCMPxSTRx character type
#define f_pcmp_ub   0x00
#define f_pcmp_uw   0x01
#define f_pcmp_sb   0x02
#define f_pcmp_sw   0x03
// PCMPxSTRx aggregation operation
#define f_pcmp_ea   0x00
#define f_pcmp_rng  0x04
#define f_pcmp_ee   0x08
#define f_pcmp_eo   0x0C
// PCMPxSTRx result polarity
#define f_pcmp_pp   0x00
#define f_pcmp_np   0x10
#define f_pcmp_mp   0x20
#define f_pcmp_mn   0x30
// PCMPxSTRI index order
#define f_pcmp_ls   0x00
#define f_pcmp_ms   0x40
// PCMPxSTRM result unit size
#define f_pcmp_bit  0x00
#define f_pcmp_byte 0x40

// Define flag combinations to use.
#define f_srch_sub (f_pcmp_ub | f_pcmp_eo | f_pcmp_pp | f_pcmp_ls)

#define XMM_SIZE sizeof(__m128i)
#define XMM_CHARS (XMM_SIZE / sizeof(char))
#define XMM_OFFSET(p) ((sizeof(__m128i) - 1) & (intptr_t)(p))
#define XMM_ALIGNED(p) (0 == XMM_OFFSET(p))

#define PAGE_SIZE ((intptr_t)0x1000)
#define PAGE_OFFSET(p) ((PAGE_SIZE-1) & (intptr_t)(p))
#define XMM_PAGE_SAFE(p) (PAGE_OFFSET(p) <= (PAGE_SIZE - XMM_SIZE))

/***
*char *strstr(string1, string2) - search for string2 in string1
*
*Purpose:
*       finds the first occurrence of string2 in string1
*
*Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
*
*Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

char * __cdecl strstr (
        const char * str1,
        const char * str2
        )
{
    const char *stmp1, *stmp2;
    __m128i zero, pattern, characters1, characters2;

    // An empty search string matches everything.
    if (0 == *str2)
        return (char *)str1;

    if (__isa_available < __ISA_AVAILABLE_SSE42)
    {
        unsigned mask;
        unsigned long offset;

        // Build search pattern and zero pattern. Search pattern is
        // XMMWORD with the initial character of the search string
        // in every position. Zero pattern has a zero termination
        // character in every position.

        pattern = _mm_cvtsi32_si128((unsigned char)str2[0] | ((unsigned char)str2[0] << CHAR_BIT));
        pattern = _mm_shufflelo_epi16(pattern, 0);
        pattern = _mm_shuffle_epi32(pattern, 0);
        zero = _mm_xor_si128(zero, zero);

        // Main loop for searching str1. We look for the next character that
        // matches the first character of the search string then compare to
        // the end of the search string.

        for (;;)
        {
            // If string pointer is page safe look for the next possible match
            // or end. If none are found we can immediately check the next block,
            // otherwise set the string pointer to the address of the significant
            // character.

            if (XMM_PAGE_SAFE(str1))
            {
                characters1 = _mm_loadu_si128((__m128i*)str1);
                characters2 = _mm_cmpeq_epi8(characters1, zero);
                characters1 = _mm_cmpeq_epi8(characters1, pattern);
                characters1 = _mm_or_si128(characters1, characters2);
                mask = _mm_movemask_epi8(characters1);

                // If no character match or end found try next XMMWORD.

                if (0 == mask)
                {
                    str1 += XMM_SIZE;
                    continue;
                }

                // Advance str1 pointer to next possible match or end.

                _BitScanForward(&offset, mask);
                str1 += offset;
            }

            // If at the end of str1, then no match found.

            if (0 == str1[0]) return NULL;

            // If a first-character match is found compare strings to look for
            // a complete match.

            if (str2[0] == str1[0])
            {
                stmp1 = str1;
                stmp2 = str2;
                for (;;)
                {
                    // If search string is aligned and searched string is page-safe
                    // check a character block for differences or the end of string2.
                    // If nothing is found then go immediately to the next block.

                    if (XMM_PAGE_SAFE(stmp2) && XMM_PAGE_SAFE(stmp1))
                    {
                        characters1 = _mm_loadu_si128((__m128i*)stmp1);
                        characters2 = _mm_loadu_si128((__m128i*)stmp2);
                        characters1 = _mm_cmpeq_epi8(characters1, characters2);
                        characters2 = _mm_cmpeq_epi8(characters2, zero);
                        characters1 = _mm_cmpeq_epi8(characters1, zero);
                        characters2 = _mm_or_si128(characters1, characters2);
                        mask = _mm_movemask_epi8(characters2);

                        // If no character difference or end found try next XMMWORD.

                        if (0 == mask)
                        {
                            stmp1 += XMM_SIZE;
                            stmp2 += XMM_SIZE;
                            continue;
                        }

                        // Advance string pointers to next significant character.

                        _BitScanForward(&offset, mask);
                        stmp1 += offset;
                        stmp2 += offset;
                    }

                    // If we've reached the end of str2 then a match has been found.

                    if (0 == stmp2[0]) return (char *)str1;

                    // If we've reached a difference then no match was found.

                    if (stmp1[0] != stmp2[0]) break;

                    // Otherwise advance to next character and try again.

                    ++stmp1;
                    ++stmp2;
                }
            }

            // Current character wasn't a match, try next character.

            ++str1;
        }
    }
    else
    {
        // SSE 4.2 supported, search & compare 16 bytes at a time if possible.
        char c;
        unsigned i;

        // Load XMM with first characters of str2.
        if (XMM_PAGE_SAFE(str2))
        {
            pattern = _mm_loadu_si128((__m128i*)str2);
        }
        else
        {
            pattern = _mm_xor_si128(pattern, pattern);
            c = *(stmp2 = str2);
            for (i = 0; i < XMM_CHARS; ++i)
            {
                pattern = _mm_srli_si128(pattern, sizeof(char));
                pattern = _mm_insert_epi8(pattern, c, (XMM_CHARS-1));
                if (0 != c) c = *++stmp2;
            }
        }

        for(;;)
        {
            // Check for partial match, if none step forward and continue.
            if (XMM_PAGE_SAFE(str1))
            {
                characters1 = _mm_loadu_si128((__m128i*)str1);
                // If no potential match or end found, try next XMMWORD.
                if (_mm_cmpistra(pattern, characters1, f_srch_sub))
                {
                    str1 += XMM_CHARS;
                    continue;
                }
                // If end found there was no match.
                else if (!_mm_cmpistrc(pattern, characters1, f_srch_sub))
                {
                    return NULL;
                }

                // Get position of potential match.
                str1 += _mm_cmpistri(pattern, characters1, f_srch_sub);
            }
            else
            {
                // If end of string found there was no match.
                if (0 == *str1)
                {
                    return NULL;
                }

                // If current character doesn't match first character
                // of search string try next character.
                if (*str1 != *str2)
                {
                    ++str1;
                    continue;
                }
            }

            // Potential match, compare to check for full match.
            stmp1 = str1;
            stmp2 = str2;
            for (;;)
            {
              // If next XMMWORD is page-safe for each string
              // do a XMMWORD comparison.
              if (XMM_PAGE_SAFE(stmp1) && XMM_PAGE_SAFE(stmp2))
              {
                  characters1 = _mm_loadu_si128((__m128i*)stmp1);
                  characters2 = _mm_loadu_si128((__m128i*)stmp2);

                  // If unequal then no match found.
                  if (!_mm_cmpistro(characters2, characters1, f_srch_sub))
                  {
                      break;
                  }

                  // If end of search string then match found.
                  else if (_mm_cmpistrs(characters2, characters1, f_srch_sub))
                  {
                      return (char *)str1;
                  }

                  stmp1 += XMM_CHARS;
                  stmp2 += XMM_CHARS;
              }

              // Compare next character.
              else
              {
                  // If end of search string then match found.
                  if (0 == *stmp2)
                  {
                      return (char *)str1;
                  }

                  // If unequal then no match found.
                  if (*stmp1 != *stmp2)
                  {
                      break;
                  }

                  // Character matched - try next character.
                  ++stmp1;
                  ++stmp2;
              }
          }

          // Match not found at current position, try next.
          ++str1;
        }
    }
}
