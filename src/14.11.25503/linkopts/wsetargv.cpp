//
// wsetargv.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that enables wildcard expansion for the arguments to main().
//
#include <vcruntime_startup.h>



extern "C" _crt_argv_mode __CRTDECL _get_startup_argv_mode()
{
    return _crt_argv_expanded_arguments;
}
