//
// strncnt.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines __strncnt(), which returns the number of characters in a string.  If
// the string is longer than the given 'count', 'count' is returned.
//
#include <string.h>
#include <msvcrt_IAT.h>


#ifdef _ATL_XP_TARGETING
extern "C" size_t __cdecl __strncnt_downlevel(
    char const* const string,
    size_t      const count
    )
{
    char const* it = string;
    size_t n = 0;

    for (; *it && n != count; ++it, ++n) { }

    return n;
}

_LCRT_DEFINE_IAT_SYMBOL(__strncnt_downlevel);

#endif