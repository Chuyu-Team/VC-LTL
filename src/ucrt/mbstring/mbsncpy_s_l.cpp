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

#define _FUNC_NAME _mbsncpy_s_l
#define _COUNT _CountInChars
#define _COUNT_IN_BYTES 0

#if _CRT_NTDDI_MIN < 0x06000000
#include "mbsncpy_s.inl"
#endif
