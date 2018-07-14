/***
*mbsnbcat_s.c - concatenate string2 onto string1, max length n bytes
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       defines mbsnbcat_s() - concatenate maximum of n bytes
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <msvcrt_IAT.h>

_REDIRECT_TO_L_VERSION_4_downlevel(errno_t, _mbsnbcat_s, unsigned char *, size_t, const unsigned char *, size_t)

_LCRT_DEFINE_IAT_SYMBOL(_mbsnbcat_s_downlevel);