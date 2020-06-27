//
// legacy_x86_flt_exceptions.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Linking with this linkopt will disable the exception code filter
// for x86 SSE2 floating point exceptions.
//

#include <vcruntime_startup.h>

extern "C" long __CRTDECL _filter_x86_sse2_floating_point_exception(long const exception_code)
{
    return exception_code;
}
