//
// iso_stdio_wide_specifiers.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Configures the stdio library to use the confroming %s, %c, and %[] specifiers for
// the wide string printf and scanf functions.  The conforming behavior
// is that these format specifiers map to a narrow character buffer; the 'l' length
// modifier must be used when passing a wide character buffer.
//
// The legacy behavior (default) is that these format
// specifiers map to a wide character buffer; the 'h' length modifier must be used
// when passing a narrow character buffer.  This maintains compatibility with previous
// Visual C++ releases.
//
#undef  _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS
#define _CRT_STDIO_ISO_WIDE_SPECIFIERS

#include <vcruntime_internal.h>
#include <vcruntime_startup.h>
#include <stdio.h>

static int __CRTDECL initialize_iso_wide_specifiers() noexcept
{
    _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS &= ~_CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS;
    _CRT_INTERNAL_LOCAL_SCANF_OPTIONS  &= ~_CRT_INTERNAL_SCANF_LEGACY_WIDE_SPECIFIERS;
    return 0;
}

#ifdef _M_CEE_PURE

    extern "C" void __clrcall __scrt_initialize_iso_stdio_wide_specifier_mode() noexcept
    {
        initialize_iso_wide_specifiers();
    }

#else

    #pragma warning(disable: 4483) // Allow use of __identifier
    extern "C" _CRTALLOC(".CRT$XIC") _PIFV __identifier("__PLEASE_LINK_WITH_iso_stdio_wide_specifiers.lib") = initialize_iso_wide_specifiers;

#endif
