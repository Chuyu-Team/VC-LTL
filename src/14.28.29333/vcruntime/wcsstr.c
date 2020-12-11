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

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

#include <arm64string.h>

wchar_t * __cdecl wcsstr (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
    const wchar_t *stmp1, *stmp2;
    vector_t pattern, characters1, characters2;
    vector_t zeromatches, zeromatches2, chmatches, orrmatches, orrmatchesrev;
    __n64 uaddlvq32;
    unsigned __int64 lword1, lword0;
    wchar_t chr;

    // An empty search string matches everything.

    if (0 == *wcs2) {
        return (wchar_t *)wcs1;
    }

    unsigned __int64 mask;
    unsigned long offset;

    // An empty search string matches anything.

    if (0 == wcs2[0]) {
        return (wchar_t *)wcs1;
    }

    // Build search pattern. Search pattern is XMMWORD with the initial
    // character of the search string in every position.

    chr = (wchar_t)wcs2[0];
    pattern = neon_dupqr16(chr);

    // Main loop for searching wcs1. We look for the next character that
    // matches the first character of the search string then compare to
    // the end of the search string.

    for (;;)
    {
        // If string pointer is page safe look for the next possible match
        // or end. If none are found we can immediately check the next block,
        // otherwise set the string pointer to the address of the significant
        // character.

        if (N128_PAGE_SAFE(wcs1))
        {
            characters1 = *(vector_t *)wcs1;
            wcs1 += N128_WCHARS;
            zeromatches = neon_cmeqzq16(characters1);
            chmatches = neon_cmeqq16(characters1, pattern);
            orrmatches = neon_orrq(chmatches, zeromatches);

            // unsigned add across long vector. non zero means we have a match

            uaddlvq32 = neon_uaddlvq32(orrmatches);
            mask = neon_umov64(uaddlvq32, 0);

            // If no character match or end found try next XMMWORD.

            if (0 == mask)
            {
                continue;
            }

            wcs1 -= N128_WCHARS;

            // Advance wcs1 pointer to next possible match or end.

            orrmatchesrev = neon_rev64q_16(orrmatches);
            lword0 = neon_umovq64(orrmatchesrev, 0);
            lword1 = neon_umovq64(orrmatchesrev, 1);
            offset = _CountLeadingZeros128(lword0, lword1) >> 3;

            wcs1 += (offset/sizeof(wchar_t));
        }

        // If at the end of wcs1, then no match found.

        if (0 == wcs1[0]) {
            return NULL;
        }

        // If a first-character match is found compare strings to look for
        // a complete match.

        if (wcs2[0] == wcs1[0])
        {
            stmp1 = wcs1;
            stmp2 = wcs2;
            for (;;)
            {
                // If search string is aligned and searched string is page-safe
                // check a character block for differences or the end of string2.
                // If nothing is found then go immediately to the next block.

                if (N128_PAGE_SAFE(stmp2) && N128_PAGE_SAFE(stmp1))
                {
                    characters2 = *(vector_t *)stmp2;
                    characters1 = *(vector_t *)stmp1;
                    zeromatches2 = neon_cmeqzq16(characters2);
                    chmatches = neon_cmeqq16(characters1, characters2);
                    zeromatches = neon_cmeqzq16(chmatches);
                    orrmatches = neon_orrq(zeromatches, zeromatches2);

                    // unsigned add across long vector. non zero means we have a match

                    uaddlvq32 = neon_uaddlvq32(orrmatches);
                    mask = neon_umov64(uaddlvq32, 0);

                    // If no character difference or end found try next XMMWORD.

                    if (0 == mask)
                    {
                        stmp1 += N128_WCHARS;
                        stmp2 += N128_WCHARS;
                        continue;
                    }

                    // Advance string pointers to next significant character.

                    orrmatchesrev = neon_rev64q_8(orrmatches);
                    lword0 = neon_umovq64(orrmatchesrev, 0);
                    lword1 = neon_umovq64(orrmatchesrev, 1);
                    offset = _CountLeadingZeros128(lword0, lword1) >> 3;
                    stmp1 += (offset/sizeof(wchar_t));
                    stmp2 += (offset/sizeof(wchar_t));
                }

                // If we've reached the end of wcs2 then a match has been found.

                if (0 == stmp2[0]) {
                    return (wchar_t *)wcs1;
                }

                // If we've reached a difference then no match was found.

                if (stmp1[0] != stmp2[0]) break;

                // Otherwise advance to next character and try again.

                ++stmp1;
                ++stmp2;
            }
        }

        // Current character wasn't a match, try next character.

        ++wcs1;
    }
}

#else

wchar_t * __cdecl wcsstr (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
        wchar_t *cp = (wchar_t *) wcs1;
        wchar_t *s1, *s2;

        if ( !*wcs2)
            return (wchar_t *)wcs1;

        while (*cp)
        {
                s1 = cp;
                s2 = (wchar_t *) wcs2;

                while ( *s2 && !(*s1-*s2) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);
}

#endif
