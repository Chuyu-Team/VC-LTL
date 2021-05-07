//
// exe_winmain.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The WinMainCRTStartup() entry point, linked into a client executable that
// uses WinMain().
//
#define _SCRT_STARTUP_WINMAIN
#include "exe_common.inl"



extern "C" DWORD WinMainCRTStartup(LPVOID)
{
    return __scrt_common_main();
}
