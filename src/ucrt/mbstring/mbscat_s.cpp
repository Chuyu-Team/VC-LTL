/***
*mbscat_s.c - Concatenate one string to another (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Concatenate one string to another (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <msvcrt_IAT.h>

_REDIRECT_TO_L_VERSION_3_downlevel(errno_t, _mbscat_s, unsigned char *, size_t, const unsigned char *)

_LCRT_DEFINE_IAT_SYMBOL(_mbscat_s_downlevel);