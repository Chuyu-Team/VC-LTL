/***
*wcsstr.c - search for one wide-character string inside another
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines wcsstr() - search for one wchar_t string inside another
*
*******************************************************************************/

#include <vcruntime_internal.h>
#include <intrin.h>

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
#define f_srch_sub (f_pcmp_uw | f_pcmp_eo | f_pcmp_pp | f_pcmp_ls)

#define WCHAR_ALIGNED(p) (0 == ((sizeof(wchar_t)-1) & (intptr_t)(p)))

#define XMM_SIZE sizeof(__m128i)
#define XMM_ALIGNED(p) (0 == ((XMM_SIZE-1) & (intptr_t)(p)))
#define XMM_CHARS (XMM_SIZE / sizeof(wchar_t))

#define PAGE_SIZE ((intptr_t)0x1000)
#define PAGE_OFFSET(p) ((PAGE_SIZE-1) & (intptr_t)(p))
#define XMM_PAGE_SAFE(p) (PAGE_OFFSET(p) <= (PAGE_SIZE - XMM_SIZE))

/***
*wchar_t *wcsstr(string1, string2) - search for string2 in string1
*       (wide strings)
*
*Purpose:
*       finds the first occurrence of string2 in string1 (wide strings)
*
*Entry:
*       wchar_t *string1 - string to search in
*       wchar_t *string2 - string to search for
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

wchar_t * __cdecl wcsstr (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
    const wchar_t *stmp1, *stmp2;
    __m128i zero, pattern, characters1, characters2;

    // An empty search string matches everything.
    if (0 == *wcs2)
        return (wchar_t *)wcs1;

    if (__isa_available > __ISA_AVAILABLE_SSE2)
    {
        wchar_t c;
        unsigned i;

        // Load XMM with first characters of wcs2.
        if (XMM_PAGE_SAFE(wcs2))
        {
            pattern = _mm_loadu_si128((__m128i*)wcs2);
        }
        else
        {
            pattern = _mm_xor_si128(pattern, pattern);
            c = *(stmp2 = wcs2);
            for (i = 0; i < XMM_CHARS; ++i)
            {
                pattern = _mm_srli_si128(pattern, sizeof(wchar_t));
                pattern = _mm_insert_epi16(pattern, c, (XMM_CHARS-1));
                if (0 != c) c = *++stmp2;
            }
        }

        for(;;)
        {
            // Check for partial match, if none step forward and continue.
            if (XMM_PAGE_SAFE(wcs1))
            {
                characters1 = _mm_loadu_si128((__m128i*)wcs1);
                // If no potential match or end found, try next XMMWORD.
                if (_mm_cmpistra(pattern, characters1, f_srch_sub))
                {
                    wcs1 += XMM_CHARS;
                    continue;
                }
                // If end found there was no match.
                else if (!_mm_cmpistrc(pattern, characters1, f_srch_sub))
                {
                    return NULL;
                }

                // Get position of potential match.
                wcs1 += _mm_cmpistri(pattern, characters1, f_srch_sub);
            }
            else
            {
              // If end of string found there was no match.
              if (0 == *wcs1)
              {
                  return NULL;
              }

              // If current character doesn't match first character
              // of search string try next character.
              if (*wcs1 != *wcs2)
              {
                  ++wcs1;
                  continue;
              }
            }

            // Potential match, compare to check for full match.
            stmp1 = wcs1;
            stmp2 = wcs2;
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
                        return (wchar_t *)wcs1;
                    }

                    stmp1 += XMM_CHARS;
                    stmp2 += XMM_CHARS;
                    continue;
                }

                // Compare next character.
                else
                {
                    // If end of search string then match found.
                    if (0 == *stmp2)
                    {
                        return (wchar_t *)wcs1;
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
            ++wcs1;
        }
    }
    else if (__isa_available == __ISA_AVAILABLE_SSE2)
    {
        unsigned mask;
        unsigned long offset;

        // Build search pattern and zero pattern. Search pattern is
        // XMMWORD with the initial character of the search string
        // in every position. Zero pattern has a zero termination
        // character in every position.

        pattern = _mm_cvtsi32_si128(wcs2[0]);
        pattern = _mm_shufflelo_epi16(pattern, 0);
        pattern = _mm_shuffle_epi32(pattern, 0);
        zero = _mm_xor_si128(zero, zero);

        // Main loop for searching wcs1.

        for (;;)
        {
            // If XMM check is safe advance wcs1 to the next
            // possible match or end.

            if (XMM_PAGE_SAFE(wcs1))
            {
                characters1 = _mm_loadu_si128((__m128i*)wcs1);
                characters2 = _mm_cmpeq_epi16(characters1, zero);
                characters1 = _mm_cmpeq_epi16(characters1, pattern);
                characters1 = _mm_or_si128(characters1, characters2);
                mask = _mm_movemask_epi8(characters1);

                // If no character match or end found try next XMMWORD.

                if (0 == mask)
                {
                    wcs1 += XMM_CHARS;
                    continue;
                }

                // Advance wcs1 pointer to next possible match or end.

                _BitScanForward(&offset, mask);
                wcs1 += (offset/sizeof(wchar_t));
            }

            // If at the end of wcs1, then no match found.

            if (0 == wcs1[0]) return NULL;

            // If a first-character match is found compare
            // strings to look for match.

            if (wcs2[0] == wcs1[0])
            {
                stmp1 = wcs1;
                stmp2 = wcs2;
                for (;;)
                {
                    // If aligned as specified advance to next
                    // possible difference or wcs2 end.

                    if (XMM_PAGE_SAFE(stmp2) && XMM_PAGE_SAFE(stmp1))
                    {
                        characters1 = _mm_loadu_si128((__m128i*)stmp1);
                        characters2 = _mm_loadu_si128((__m128i*)stmp2);
                        characters1 = _mm_cmpeq_epi16(characters1, characters2);
                        characters2 = _mm_cmpeq_epi16(characters2, zero);
                        characters1 = _mm_cmpeq_epi16(characters1, zero);
                        characters1 = _mm_or_si128(characters1, characters2);
                        mask = _mm_movemask_epi8(characters1);

                        // If mask is zero there is no difference and
                        // wcs2 does not end in this XMMWORD. Continue
                        // with next XMMWORD.

                        if (0 == mask)
                        {
                            stmp1 += XMM_CHARS;
                            stmp2 += XMM_CHARS;
                            continue;
                        }

                        // Advance string pointers to next significant
                        // character.

                        _BitScanForward(&offset, mask);
                        stmp1 += (offset/sizeof(wchar_t));
                        stmp2 += (offset/sizeof(wchar_t));
                    }

                    // If we've reached the end of wcs2 then a match
                    // has been found.

                    if (0 == stmp2[0]) return (wchar_t *)wcs1;

                    // If we've reached a difference then no match
                    // was found.

                    if (stmp1[0] != stmp2[0]) break;

                    // Otherwise advance to next character and try
                    // again.

                    ++stmp1;
                    ++stmp2;
                }
            }

            // Current character wasn't a match, try next character.

            ++wcs1;
        }
    }
    else
    {
        const wchar_t *cp = wcs1;
        const wchar_t *s1, *s2;

        while (*cp)
        {
            s1 = cp;
            s2 = wcs2;

            while ( *s1 && *s2 && !(*s1-*s2) )
                s1++, s2++;

            if (!*s2)
                return (wchar_t *) cp;

            cp++;
        }

        return NULL;
    }
}
