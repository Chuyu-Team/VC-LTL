// Copyright (c) Microsoft Corporation. All rights reserved.

// This allows a non-store application to initialize a COM MTA as part of process initialization.
// As a consequence, this MTA will persist until process shutdown.
// If you have a leak checker that's looking for balanced MTA increment/decrement, this could cause false positives.

// Note that this linkopt requires Win8+ because that's when CoIncrementMTAUsage was introduced

#include <vcruntime_internal.h>
#include <vcruntime_startup.h>
#include <combaseapi.h>

#if !defined _M_CEE
    #pragma comment (lib, "onecore.lib")

    extern "C" int __cdecl __scrt_exe_initialize_mta()
    {
        // Only available on Win8+
        CO_MTA_USAGE_COOKIE cookie;
        return static_cast<int>(::CoIncrementMTAUsage(&cookie));
    }

    #pragma warning(disable: 4483) // Allow use of __identifier
    extern "C" _PIFV __identifier("__PLEASE_LINK_WITH_exe_initialize_mta.lib") = __scrt_exe_initialize_mta;
#endif
