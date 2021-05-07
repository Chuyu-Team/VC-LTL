//
// legacy_stdio_msvcrt_compatibility.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Configures the stdio library to use the legacy msvcrt.dll behavior for the
// following options when the printf functions are called from this module:
//
//  * The F and N length modifiers are supported for pointers, and the %F format
//    specifier is not supported.
//  * The %e and %g format specifiers always generate an exponent with three
//    digits, whereas the C Standard requires two digits if three are not required.
//  * The legacy infinity and NaN strings (1.#INF, 1.#NAN, etc.) are used instead
//    of the C Standard strings (inf and nan).
//

#include <vcruntime_internal.h>
#include <vcruntime_startup.h>
#include <corecrt_stdio_config.h>

static int __CRTDECL initialize_msvcrt_compatibility() noexcept
{
    _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |=
        _CRT_INTERNAL_PRINTF_LEGACY_MSVCRT_COMPATIBILITY  |
        _CRT_INTERNAL_PRINTF_LEGACY_THREE_DIGIT_EXPONENTS;

    _CRT_INTERNAL_LOCAL_SCANF_OPTIONS |=
        _CRT_INTERNAL_SCANF_LEGACY_MSVCRT_COMPATIBILITY;
    return 0;
}

#ifdef _M_CEE_PURE

    extern "C" void __clrcall __scrt_initialize_stdio_msvcrt_compatibility_mode() noexcept
    {
        initialize_msvcrt_compatibility();
    }

#else

    extern "C" _CRTALLOC(".CRT$XIC") _PIFV __scrt_stdio_legacy_msvcrt_compatibility = initialize_msvcrt_compatibility;

#endif
