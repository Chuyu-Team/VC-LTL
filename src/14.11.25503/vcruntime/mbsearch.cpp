/***
*mbsearch.cpp - do a binary search
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines bsearch() - do a binary search an an array
*
*******************************************************************************/
#ifndef _M_CEE
    #error This file should only be compiled as managed code.
#endif

#undef MRTDLL

#include "bsearch.cpp"

#ifdef __USE_CONTEXT
    #error __USE_CONTEXT should be undefined
#endif

#define __USE_CONTEXT
#include "bsearch.cpp"
