//
// new_mode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This object is always statically linked into the client module.  It defines
// the default _get_startup_new_mode() implementation, which configures malloc()
// to not call the new handler on allocation failure.  This function is called
// during executable startup.  It may be replaced by linking with the newmode
// link option.
//
#include <vcruntime.h>



extern "C" int __CRTDECL _get_startup_new_mode()
{
    return 0;
}
