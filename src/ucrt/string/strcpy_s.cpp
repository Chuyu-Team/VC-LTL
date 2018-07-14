//
// strcpy_s.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines strcpy_s(), which copies a string from one buffer to another.
//
#include <corecrt_internal_string_templates.h>
#include <string.h>
#include <msvcrt_IAT.h>



extern "C" errno_t __cdecl strcpy_s_downlevel(
    char*       const destination,
    size_t      const size_in_elements,
    char const* const source
    )
{
    return common_tcscpy_s(destination, size_in_elements, source);
}

_LCRT_DEFINE_IAT_SYMBOL(strcpy_s_downlevel);