//
// default_precision.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Initializes the floating point precision to the default state.  This behavior
// is overridable at link-time by linking with one of the link options.
//
#ifndef _M_IX86
    #error This file may only be built for x86
#endif

#include <float.h>
#include <vcstartup_internal.h>



extern "C" void __CRTDECL _initialize_default_precision()
{
    if (_controlfp_s(nullptr, _PC_53, _MCW_PC) != 0)
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
}
