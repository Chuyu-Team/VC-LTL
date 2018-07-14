/***
*mbsnbcpy_s_l.c - Copy one string to another, n bytes only (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Copy one string to another, n bytes only (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <corecrt_internal_securecrt.h>
#include <msvcrt_IAT.h>

#define _FUNC_NAME _mbsnbcpy_s_l_downlevel
#define _COUNT _CountInBytes
#define _COUNT_IN_BYTES 1

#ifdef _ATL_XP_TARGETING
#include "mbsncpy_s.inl"

_LCRT_DEFINE_IAT_SYMBOL(_mbsnbcpy_s_l_downlevel);

#endif
