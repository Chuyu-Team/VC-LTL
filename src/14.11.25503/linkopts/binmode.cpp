//
// binmode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that sets the default file open mode to binary at program start.
//
#include <vcruntime.h>
#include <fcntl.h>



extern "C" int __CRTDECL _get_startup_file_mode()
{
    return _O_BINARY;
}
