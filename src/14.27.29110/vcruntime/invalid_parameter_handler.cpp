//
// invalid_parameter_handler.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Initializes the invalid parameter to its default behavior, which terminates
// the process.  This behavior is overridable at link-time by linking with one
// of the link options.
//
#include <vcruntime.h>



extern "C" void __CRTDECL _initialize_invalid_parameter_handler()
{
    // No-op
}
