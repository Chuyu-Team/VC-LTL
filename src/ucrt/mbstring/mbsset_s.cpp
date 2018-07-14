/***
*mbsset_s.c - Sets all charcaters of string to given character (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Sets all charcaters of string to given character (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <msvcrt_IAT.h>

_REDIRECT_TO_L_VERSION_3_downlevel(errno_t, _mbsset_s, unsigned char*, size_t, unsigned int)

_LCRT_DEFINE_IAT_SYMBOL(_mbsset_s_downlevel);