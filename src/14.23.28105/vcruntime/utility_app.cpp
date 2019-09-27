//
// utility_app.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// CRT startup and termination functionality specific to use of the CRT in a
// Windows Store app.
//
#include <vcstartup_internal.h>
#include <roapi.h>



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Startup Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" WORD __cdecl __scrt_get_show_window_mode()
{
    // The window mode is not relevant for Windows Store apps:
    return 0;
}

extern "C" bool __cdecl __scrt_is_managed_app()
{
    // /clr is not supported for Windows Store apps, so this is always false:
    return false;
}

extern "C" int __cdecl __scrt_initialize_winrt()
{
    if (__crtWinrtInitType != 0)
    {
        long const hr = RoInitialize(RO_INIT_MULTITHREADED);
        if (hr < 0)
            return static_cast<int>(hr);
    }

    return 0;
}

extern "C" int __cdecl __scrt_initialize_mta()
{
    // Apps do their own thing with WinRT. No need to increment the MTA usage here.
    return 0;
}

extern "C" void __cdecl __scrt_set_unhandled_exception_filter()
{
    // No action is possible; we cannot register an unhandled exception filter
    // in a Windows Store app.
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Fatal Error Reporting
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" void __cdecl __scrt_fastfail(unsigned const code)
{
    // Windows Store apps only run on Windows 8 and above, and __fastfail is
    // always available on Windows 8, so we can just invoke it:
    __fastfail(code);
}
