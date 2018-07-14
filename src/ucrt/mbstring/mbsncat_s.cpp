/***
*mbsncat_s.c - concatenate string2 onto string1, max length n
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       defines mbsncat_s() - concatenate maximum of n characters
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <msvcrt_IAT.h>

_REDIRECT_TO_L_VERSION_4_downlevel(errno_t, _mbsncat_s, unsigned char *, size_t, const unsigned char *, size_t)

_LCRT_DEFINE_IAT_SYMBOL(_mbsncat_s_downlevel);