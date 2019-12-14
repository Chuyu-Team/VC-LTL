//
// vcruntime_stubs.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This source file defines stub alternatives for several functions that are
// only present in the static VCRuntime (not in the VCRuntime DLLs).
//
#include <vcruntime.h>



extern "C" bool __cdecl __scrt_stub_for_vcrt_initialize()
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_vcrt_uninitialize(bool)
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_vcrt_uninitialize_critical(bool)
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_vcrt_thread_attach()
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_vcrt_thread_detach()
{
    return true;
}
