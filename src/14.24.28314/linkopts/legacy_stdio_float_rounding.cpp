//
// legacy_stdio_float_rounding.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Configures the stdio library to use the legacy rounding mode for printf functions.
//
//
// TRANSITION, VSO#855216
#define _CRT_INTERNAL_PRINTF_STANDARD_ROUNDING (1ULL << 5)
#include <vcruntime_internal.h>
#include <vcruntime_startup.h>
#include <corecrt_stdio_config.h>

static int __CRTDECL initialize_printf_standard_rounding() noexcept
{
    _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS &= ~_CRT_INTERNAL_PRINTF_STANDARD_ROUNDING;
    return 0;
}

#ifdef _M_CEE_PURE

    extern "C" void __clrcall __scrt_initialize_printf_standard_rounding_mode() noexcept
    {
        initialize_printf_standard_rounding();
    }

#else

extern "C" _CRTALLOC(".CRT$XIC") _PIFV __scrt_initialize_printf_standard_rounding_fp = initialize_printf_standard_rounding;

#endif
