/***
*mbsncpy_s_l.c - Copy one string to another, n chars only (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Copy one string to another, n chars only (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_securecrt.h>
#include <mbstring.h>
#include <msvcrt_IAT.h>

#define _FUNC_NAME _mbsncpy_s_l_downlevel
#define _COUNT _CountInChars
#define _COUNT_IN_BYTES 0

#ifdef _ATL_XP_TARGETING
#include "mbsncpy_s.inl"

_LCRT_DEFINE_IAT_SYMBOL(_mbsncpy_s_l_downlevel);

#endif
