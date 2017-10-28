//
// noarg.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that suppresses argument processing during startup.  If this
// link option is used, argv is left empty when main() is called and the global
// environment state (__argv and friends) are not initialized.
//
#include <vcruntime_startup.h>



extern "C" _crt_argv_mode __CRTDECL _get_startup_argv_mode()
{
    return _crt_argv_no_arguments;
}

extern "C" char* __CRTDECL _get_narrow_winmain_command_line()
{
    return nullptr;
}

extern "C" wchar_t* __CRTDECL _get_wide_winmain_command_line()
{
    return nullptr;
}
