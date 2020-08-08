//
// commode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that defaults the global commit flag to true.
//
#include <vcruntime.h>
#include <fcntl.h>



extern "C" int __CRTDECL _get_startup_commit_mode()
{
    return 0x0800; // _IOCOMMIT
}
