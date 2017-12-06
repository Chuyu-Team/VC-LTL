//
// argv_mode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Gets the default argv population mode, used during executable startup.  This
// behavior is overridable at link-time by linking with one of the link options.
//
#include <vcruntime_startup.h>



extern "C" _crt_argv_mode __CRTDECL _get_startup_argv_mode()
{
    return _crt_argv_unexpanded_arguments;
}
