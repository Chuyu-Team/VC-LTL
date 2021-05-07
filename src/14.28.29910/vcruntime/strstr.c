/*
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/***
*strstr.c - search for one string inside another
*
*       Copyright (c) Microsoft Corporation.
*
*Purpose:
*       defines strstr() - search for one string inside another
*
*******************************************************************************/

#include <vcruntime_internal.h>

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

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

// Use strchr with Neon intrinsics ported from AMD64.

#include <arm64string.h>

/***
*strstr.c - search for one string inside another
*
*   Copyright (C) 2009-2010 Intel Corporation.
*
*Purpose:
*       defines strstr() - search for one string inside another
*
*******************************************************************************/

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
    vector_t pattern, characters1, characters2;
    vector_t zeromatches, zeromatches2, chmatches, orrmatches, orrmatchesrev;
    __n64 uaddlvq32;
    unsigned __int64 lword1, lword0;
    unsigned char chr;

    // An empty search string matches everything.

    if (0 == *str2) {
        return (char *)str1;
    }

    unsigned __int64 mask;
    unsigned __int64 offset;    //    unsigned long offset;

    // Build search pattern. Search pattern is XMMWORD with the initial
    // character of the search string in every position.

    chr = (unsigned char)str2[0];
    pattern = neon_dupqr8(chr);

    // Main loop for searching str1. We look for the next character that
    // matches the first character of the search string then compare to
    // the end of the search string.

    for (;;)
    {
        // If string pointer is page safe look for the next possible match
        // or end. If none are found we can immediately check the next block,
        // otherwise set the string pointer to the address of the significant
        // character.

        if (N128_PAGE_SAFE(str1))
        {
            characters1 = *(vector_t *)str1;
            str1 += N128_SIZE;
            zeromatches = neon_cmeqzq8(characters1);
            chmatches = neon_cmeqq8(characters1, pattern);
            orrmatches = neon_orrq(chmatches, zeromatches);

            // unsigned add across long vector. non zero means we have a match

            uaddlvq32 = neon_uaddlvq32(orrmatches);
            mask = neon_umov64(uaddlvq32, 0);

            // If no character match or end found try next XMMWORD.

            if (0 == mask)
            {
                continue;
            }

            str1 -= N128_SIZE;

            // Advance str1 pointer to next possible match or end.

            orrmatchesrev = neon_rev64q_8(orrmatches);
            lword0 = neon_umovq64(orrmatchesrev, 0);
            lword1 = neon_umovq64(orrmatchesrev, 1);
            offset = _CountLeadingZeros128(lword0, lword1) >> 3;

            str1 += offset;
        }

        // If at the end of str1, then no match found.

        if (0 == str1[0]) {
            return NULL;
        }

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

                if (N128_PAGE_SAFE(stmp2) && N128_PAGE_SAFE(stmp1))
                {
                    characters2 = *(vector_t *)stmp2;
                    characters1 = *(vector_t *)stmp1;
                    zeromatches2 = neon_cmeqzq8(characters2);
                    chmatches = neon_cmeqq8(characters1, characters2);
                    zeromatches = neon_cmeqzq8(chmatches);
                    orrmatches = neon_orrq(zeromatches, zeromatches2);

                    // unsigned add across long vector. non zero means we have a match

                    uaddlvq32 = neon_uaddlvq32(orrmatches);
                    mask = neon_umov64(uaddlvq32, 0);

                    // If no character difference or end found try next XMMWORD.

                    if (0 == mask)
                    {
                        stmp1 += N128_SIZE;
                        stmp2 += N128_SIZE;
                        continue;
                    }

                    // Advance string pointers to next significant character.

                    orrmatchesrev = neon_rev64q_8(orrmatches);
                    lword0 = neon_umovq64(orrmatchesrev, 0);
                    lword1 = neon_umovq64(orrmatchesrev, 1);
                    offset = _CountLeadingZeros128(lword0, lword1) >> 3;
                    stmp1 += offset;
                    stmp2 += offset;
                }

                // If we've reached the end of str2 then a match has been found.

                if (0 == stmp2[0]) {
                    return (char *)str1;
                }

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

#else

char * __cdecl strstr (
        const char * str1,
        const char * str2
        )
{
        char *cp = (char *) str1;
        char *s1, *s2;

        if ( !*str2 )
            return((char *)str1);

        while (*cp)
        {
                s1 = cp;
                s2 = (char *) str2;

                while ( *s2 && !(*s1-*s2) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);

}

#endif
