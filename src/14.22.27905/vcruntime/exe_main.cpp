//
// exe_wwinmain.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The mainCRTStartup() entry point, linked into client executables that
// uses main().
//
#define _SCRT_STARTUP_MAIN
#include "exe_common.inl"



extern "C" int mainCRTStartup()
{
    return __scrt_common_main();
}
