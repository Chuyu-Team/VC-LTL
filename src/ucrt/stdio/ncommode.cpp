//
// ncommode.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines _commode, which sets the default file commit mode to nocommit.
//
#include <corecrt_internal_stdio.h>
#include <msvcrt_IAT.h>


// Set default file commit mode to nocommit
extern "C" int _commode = 0;


#if defined _M_AMD64

extern "C" extern int* __imp__commode;

extern "C" int* __cdecl __p__commode_downlevel()
{
    return __imp__commode;
}

_LCRT_DEFINE_IAT_SYMBOL(__p__commode_downlevel);

#endif
