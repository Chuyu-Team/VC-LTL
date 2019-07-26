//
// newmode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This link option replaces the default _get_startup_new_mode() implementation.
// It configures malloc() to not call the new handler on allocation failure.
//
#include <vcruntime.h>



extern "C" int __CRTDECL _get_startup_new_mode()
{
    return 1;
}
