//
// purevirt_data.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines __pPurecall.  This definition is in a separate source file from the
// definition of _purecall() to allow the user to override _purecall without
// making the linker angry.
//
#include <vcruntime_internal.h>
#include <stdlib.h>



extern "C" _purecall_handler __pPurecall = nullptr;



extern "C" void __cdecl __vcrt_initialize_pure_virtual_call_handler()
{
    __pPurecall = __crt_fast_encode_pointer(nullptr);
}
