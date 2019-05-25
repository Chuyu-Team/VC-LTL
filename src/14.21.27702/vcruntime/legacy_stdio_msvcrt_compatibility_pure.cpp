//
// legacy_stdio_msvcrt_compatibility_pure.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Fallback for legacy_stdio_msvcrt_compatibility link option.
//
#include <vcstartup_internal.h>



extern "C" void __clrcall __scrt_initialize_stdio_msvcrt_compatibility_mode()
{
    // No-op
}
