//
// threadlocale.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that enables the per-thread locale by default.
//
#include <vcruntime.h>



extern "C" int __CRTDECL _get_startup_thread_locale_mode()
{
    return -1;
}
