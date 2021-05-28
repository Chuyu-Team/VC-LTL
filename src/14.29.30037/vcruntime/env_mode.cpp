//
// env_mode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Specifies whether the environment should be initialized.
// This behavior is overridable at link-time by linking with noenv.obj.
//
#include <vcruntime_startup.h>



extern "C" bool __CRTDECL _should_initialize_environment()
{
    return true;
}
