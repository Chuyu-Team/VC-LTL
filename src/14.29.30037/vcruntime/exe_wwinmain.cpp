//
// exe_wwinmain.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The wWinMainCRTStartup() entry point, linked into client executables that
// uses wWinMain().
//
#define _SCRT_STARTUP_WWINMAIN
#include "exe_common.inl"



extern "C" DWORD wWinMainCRTStartup(LPVOID)
{
    return __scrt_common_main();
}
