//
// fp10.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that defaults the default FP precision to 64 bits ("10-byte
// long double")
//
#ifndef _M_IX86
    #error This file may only be built for x86
#endif



#include <float.h>
#include <vcstartup_internal.h>



extern "C" void __cdecl _initialize_default_precision()
{
    if (_controlfp_s(nullptr, _PC_64, _MCW_PC) != 0)
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
}
