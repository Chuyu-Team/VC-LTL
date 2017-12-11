//
// commit_mode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Gets the default commit mode, used during executable startup.  This behavior
// is overridable at link-time by linking with one of the link options.
//
#include <vcruntime.h>



extern "C" int __CRTDECL _get_startup_commit_mode()
{
    return 0; // No commit
}
