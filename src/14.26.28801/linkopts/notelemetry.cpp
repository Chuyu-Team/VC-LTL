//
// notelemetry.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that disables the module startup and shutdown telemetry calls.
//
#include <Windows.h>



extern "C" void __cdecl __vcrt_initialize_telemetry_provider()
{
}

extern "C" void __cdecl __vcrt_uninitialize_telemetry_provider()
{
}

extern "C" void __cdecl __telemetry_main_invoke_trigger(HINSTANCE)
{
}

extern "C" void __cdecl __telemetry_main_return_trigger(HINSTANCE)
{
}
