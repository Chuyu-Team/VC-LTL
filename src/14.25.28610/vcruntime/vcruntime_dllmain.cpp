//
// vcruntime_dllmain.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// The entry point for the VCRuntime dynamic library.
//
#include <vcruntime_internal.h>

extern "C" {

// Flag set iff DllMain was called with DLL_PROCESS_ATTACH
static int __vcrt_process_attached = 0;

static BOOL DllMainProcessAttach()
{
    if (!__vcrt_initialize())
        return FALSE;

    // Increment flag indicating the process attach completed successfully:
    ++__vcrt_process_attached;
    return TRUE;
}

static BOOL DllMainProcessDetach(bool const terminating)
{
    // If there was no prior process attach or if it failed, return immediately:
    if (__vcrt_process_attached <= 0)
        return FALSE;

    --__vcrt_process_attached;
    if (!__vcrt_uninitialize(terminating))
        return FALSE;

    return TRUE;
}

__declspec(noinline) void __vcrt_end_boot()
{
    // Do nothing.  This function is used to mark the end of the init process.
}

static __declspec(noinline) BOOL WINAPI DllMainDispatch(HINSTANCE, DWORD const fdwReason, LPVOID const lpReserved)
{
    BOOL result = FALSE;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        result = DllMainProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        result = DllMainProcessDetach(lpReserved);
        break;

    case DLL_THREAD_ATTACH:
        result = __vcrt_thread_attach();
        break;

    case DLL_THREAD_DETACH:
        result = __vcrt_thread_detach();
        break;
    }

    __vcrt_end_boot();
    return result;
}

BOOL WINAPI __vcrt_DllMain(HINSTANCE const hInstance, DWORD const fdwReason, LPVOID const lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        // The /GS security cookie must be initialized before any exception
        // handling targetting the current image is registered.  No function
        // using exception handling can be called in the current image until
        // after __security_init_cookie has been called.
        __security_init_cookie();
    }

    // The remainder of the DllMain implementation is in a separate, noinline
    // function to ensure that no code that might touch the security cookie
    // runs before the __security_init_cookie function is called.  (If code
    // that uses EH or array-type local variables were to be inlined into
    // this function, that would cause the compiler to introduce use of the
    // cookie into this function, before the call to __security_init_cookie.
    // The separate, noinline function ensures that this does not occur.)
    return DllMainDispatch(hInstance, fdwReason, lpReserved);
}

} // extern "C"
