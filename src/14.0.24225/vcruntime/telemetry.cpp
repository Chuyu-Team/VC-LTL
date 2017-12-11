//
// telemetry.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Empty implementation of these trigger functions to ensure binary compatibility with 
// applications built using early versions of the Visual C++ 2015 libs. 
//
#include <windows.h>

extern "C" __declspec(noinline) void __cdecl  __telemetry_main_invoke_trigger(HINSTANCE)
{
}

extern "C" __declspec(noinline) void __cdecl __telemetry_main_return_trigger(HINSTANCE)
{
}

extern "C" __declspec(noinline) void __cdecl __vcrt_initialize_telemetry_provider()
{
}

extern "C" __declspec(noinline) void __cdecl __vcrt_uninitialize_telemetry_provider()
{
}
