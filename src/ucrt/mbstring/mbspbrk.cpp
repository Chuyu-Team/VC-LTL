/***
*mbspbrk.c - Find first string char in charset, pointer return (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Find first string char in charset, pointer return (MBCS)
*       Shares common source file with mbscspn.c.
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#define _RETURN_PTR
#ifdef _ATL_XP_TARGETING
#include "mbscspn.cpp"
#endif
