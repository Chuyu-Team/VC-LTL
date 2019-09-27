//
// legacy_stdio_float_rounding_pure.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Fallback for printf_float_legacy_rounding link option.
//
#include <vcstartup_internal.h>



extern "C" void __clrcall __scrt_initialize_printf_standard_rounding_mode()
{
    // No-op
}
