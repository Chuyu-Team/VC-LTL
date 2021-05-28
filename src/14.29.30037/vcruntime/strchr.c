/***
*strchr.c - search a string for a given character
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines strchr() - search a string for a character
*
*******************************************************************************/

#include <vcruntime_internal.h>

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

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

// use NEON intrinsics
// Use strchr with Neon intrinsics ported from amd64

#include <arm64string.h>

char * __cdecl strchr (
        const char * string,
        int ch
        )
{
        return strchr_internal(string, ch);
}

#else

char * __cdecl strchr (
        const char * string,
        int ch
        )
{
        while (*string && *string != (char)ch)
                string++;

        if (*string == (char)ch)
                return((char *)string);
        return(NULL);
}

#endif
