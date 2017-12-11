/***
*mlsearch.cpp - do a binary search
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _lsearch() - do a binary search an an array
*
*******************************************************************************/
#ifndef _M_CEE
    #error This file should only be compiled as managed code.
#endif

#undef MRTDLL

#include "lsearch.cpp"

#ifdef __USE_CONTEXT
    #error __USE_CONTEXT should be undefined
#endif

#define __USE_CONTEXT
#include "lsearch.cpp"
