//
// wcscat_s.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines wcscat_s(), which concatenates (appends) a copy of the source string
// to the end of the destiation string.
//
#include <corecrt_internal_string_templates.h>
#include <string.h>
#include <msvcrt_IAT.h>


extern "C" errno_t __cdecl wcscat_s_downlevel(
    wchar_t*       const destination,
    size_t         const size_in_elements,
    wchar_t const* const source
    )
{
    return common_tcscat_s(destination, size_in_elements, source);
}

_LCRT_DEFINE_IAT_SYMBOL(wcscat_s_downlevel);