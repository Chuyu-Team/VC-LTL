//
// mqsort.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines managed versions of the qsort functions that take __clrcall callbacks
//
#ifndef _M_CEE
    #error This file should only be compiled as managed code.
#endif

#undef MRTDLL

#include "qsort.cpp"

#ifdef __USE_CONTEXT
    #error __USE_CONTEXT should be undefined
#endif

#define __USE_CONTEXT
#include "qsort.cpp"
