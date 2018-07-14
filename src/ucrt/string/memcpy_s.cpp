//
// memcpy_s.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Provides external definitions of the inline functions memcpy_s and memmove_s
// for use by objects compiled with older versions of the CRT headers.
//

#define _CRT_MEMCPY_S_INLINE extern __inline
#define memcpy_s memcpy_s_downlevel
#define memmove_s memmove_s_downlevel

#include <corecrt_memcpy_s.h>

#include <msvcrt_IAT.h>
_LCRT_DEFINE_IAT_SYMBOL(memcpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL(memmove_s_downlevel);
