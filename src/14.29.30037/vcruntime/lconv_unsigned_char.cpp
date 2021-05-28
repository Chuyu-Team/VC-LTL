//
// lconv_unsigned_char_static.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// lconv initialization used to support an unsigned 'char' type (enabled by the
// -J flag during compilation).  See lconv_unsigned_char_initialization.cpp for
// an explanation of how this works.
//
// Note:  this file is linked statically into user binaries.
//
#include <vcruntime_internal.h>

extern "C" {

    int __do_unsigned_char_lconv_initialization = 255;

    int __cdecl __initialize_lconv_for_unsigned_char();

    _CRTALLOC(".CRT$XIC") static _PIFV pinit = __initialize_lconv_for_unsigned_char;

} // extern "C"
