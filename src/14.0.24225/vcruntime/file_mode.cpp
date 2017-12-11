//
// file_mode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Gets the default file open mode, used during executable startup.  This
// behavior is overridable at link-time by linking with one of the link options.
//
#include <vcruntime.h>
#include <fcntl.h>



extern "C" int __CRTDECL _get_startup_file_mode()
{
    return _O_TEXT;
}
