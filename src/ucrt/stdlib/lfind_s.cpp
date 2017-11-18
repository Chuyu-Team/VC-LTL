//
// lfind_s.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines _lfind_s(), which performs a linear search over an array.
//
#ifdef __USE_CONTEXT
    #error __USE_CONTEXT should be undefined
#endif

#ifdef _ATL_XP_TARGETING
#define __USE_CONTEXT
#include "lfind.cpp"
#endif
