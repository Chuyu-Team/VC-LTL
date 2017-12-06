//
// ucrt_stubs.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This source file defines stub alternatives for several functions that are
// only present in the static Universal CRT (not in the Universal CRT DLLs).
//
#include <vcruntime.h>



extern "C" int __scrt_ucrt_dll_is_in_use = 1;



extern "C" bool __cdecl __scrt_stub_for_acrt_initialize()
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_acrt_uninitialize(bool)
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_acrt_uninitialize_critical(bool)
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_acrt_thread_attach()
{
    return true;
}

extern "C" bool __cdecl __scrt_stub_for_acrt_thread_detach()
{
    return true;
}

extern "C" int __cdecl __scrt_stub_for_is_c_termination_complete()
{
    return 0;
}
