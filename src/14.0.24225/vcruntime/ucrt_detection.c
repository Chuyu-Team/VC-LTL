//
// ucrt_detection.c
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This source file allows us to detect whether the Universal CRT DLL or static
// library is used by this module.  If the Universal CRT DLL is used, the stub
// alternatives of several static Universal CRT startup functions will be used
// (from ucrt_stubs.cpp).  That source file defines __scrt_ucrt_dll_is_in_use
// with a value of 1.  If the Universal CRT static library is used, those stub
// alternatives will not be used, so this definition of __scrt_ucrt_dll_is_in_use
// will be used instead.  It has a value of 0.
//
// This relies on a feature of the C compiler known as "communal variables."
// This does not work in C++, and the linker's alternatename feature is not
// sufficient here.
//
#pragma warning(disable: 4132) // const object should be initialized
int const __scrt_ucrt_dll_is_in_use;

int __scrt_is_ucrt_dll_in_use()
{
    return __scrt_ucrt_dll_is_in_use != 0;
}
