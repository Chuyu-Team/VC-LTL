//
// exe_wmain.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The wmainCRTStartup() entry point, linked into client executables that
// uses wmain().
//
#define _SCRT_STARTUP_WMAIN
#include "exe_common.inl"



extern "C" DWORD wmainCRTStartup(LPVOID)
{
    return __scrt_common_main();
}
