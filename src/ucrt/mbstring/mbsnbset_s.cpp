/***
*mbsnbset_s.c - Sets first n bytes of string to given character (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Sets first n bytes of string to given character (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <corecrt_internal.h>
#include <msvcrt_IAT.h>

_REDIRECT_TO_L_VERSION_4_downlevel(errno_t, _mbsnbset_s, unsigned char *, size_t, unsigned int, size_t)

_LCRT_DEFINE_IAT_SYMBOL(_mbsnbset_s_downlevel);