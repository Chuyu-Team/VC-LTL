//
// initialization.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This file defines the main initialization and uninitialization routines for
// the VCRuntime, shared by both the static and dynamic VCRuntime libraries.  In
// the dynamic VCRuntime library, these are called by DllMain.  In the static
// VCRuntime library, these are called by the initialization code.
//
#include <vcruntime_internal.h>

extern "C" {

#if defined(_M_ARM) && !defined(_CRT_WINDOWS)

    // These functions are used to set the default behaviour of memcpy operations
    // on ARM.  If an external cache is available, memcpy() uses the *_integer
    // functions.  Otherwise, memcpy() will keep the defaults, which are the
    // *_neon functions.
    typedef void * _memcpy_t(void *, const void *, size_t);

    extern _memcpy_t * __memcpy_forward_large_func;
    extern _memcpy_t * __memcpy_reverse_large_func;
    extern _memcpy_t __memcpy_forward_large_integer;
    extern _memcpy_t __memcpy_reverse_large_integer;

    // Update memcpy function pointers:
    static void __cdecl init_memcpy_functions()
    {
        if (IsProcessorFeaturePresent(PF_ARM_EXTERNAL_CACHE_AVAILABLE))
        {
            /*
             * Set memcpy() to use the integer functions; overriding
             * the defaults, which are the NEON functions.
             */
            __memcpy_forward_large_func = &__memcpy_forward_large_integer;
            __memcpy_reverse_large_func = &__memcpy_reverse_large_integer;
        }
    }

#else

    static void __cdecl init_memcpy_functions()
    {
        // No-op on non-ARM platforms
    }

#endif



bool __cdecl __vcrt_initialize()
{
    #if defined CRTDLL
    __isa_available_init();
    #endif

    init_memcpy_functions();

    if (!__vcrt_initialize_locks())
    {
        return false;
    }

    if (!__vcrt_initialize_ptd())
    {
        __vcrt_uninitialize_locks();
        return false;
    }

    return true;
}

bool __cdecl __vcrt_uninitialize(bool const terminating)
{
    UNREFERENCED_PARAMETER(terminating);

    // If the process is terminating, there's no point in cleaning up, except
    // in debug builds.
    #ifndef _DEBUG
    if (!terminating)
    #endif
    {
        __vcrt_uninitialize_ptd();
        __vcrt_uninitialize_locks();
    }

    return true;
}

bool __cdecl __vcrt_uninitialize_critical()
{
    __vcrt_uninitialize_ptd();

    return true;
}

bool __cdecl __vcrt_thread_attach()
{
    // Create a per-thread data structure for this thread (getptd will attempt
    // to create a new per-thread data structure if one does not already exist
    // for this thread):
    if (__vcrt_getptd_noexit() == nullptr)
        return false;

    return true;
}

bool __cdecl __vcrt_thread_detach()
{
    // Free the per-thread data structure for this thread:
    __vcrt_freeptd(nullptr);
    return true;
}

}
