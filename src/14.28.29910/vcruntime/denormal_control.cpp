//
// denormal_control.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Initializes the denormal mode to the default state.  This behavior is
// overridable at link-time by linking with one of the link options.
//
#include <vcruntime.h>



extern "C" void __CRTDECL _initialize_denormal_control()
{
    // No-op
}
