//
// dll_dllmain.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The DLL entry point that initializes the DLL and calls the client-defined
// DllMain entry point.
//
#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <rtcapi.h>

#if defined _M_IX86
#define MinSubSystemVersion "5.01"
#elif defined _M_AMD64
#define MinSubSystemVersion "5.02"
#endif

#if _CRT_NTDDI_MIN < NTDDI_VISTA
#pragma comment(linker,"/SUBSYSTEM:WINDOWS," MinSubSystemVersion)
#endif


// The client may define a _pRawDllMain.  This function gets called for attach
// notifications before any other function is called, and gets called for detach
// notifications after any other function is called.  If no _pRawDllMain is
// defined, it is aliased to the no-op _pDefaultRawDllMain.
extern "C" extern __scrt_dllmain_type const _pRawDllMain;
extern "C" extern __scrt_dllmain_type const _pDefaultRawDllMain = nullptr;
_VCRT_DECLARE_ALTERNATE_NAME_DATA(_pRawDllMain, _pDefaultRawDllMain)



// This flag is incremented each time DLL_PROCESS_ATTACH is processed successfully
// and is decremented each time DLL_PROCESS_DETACH is processed (the detach is
// always assumed to complete successfully).
static int __proc_attached = 0;



static BOOL __cdecl dllmain_crt_process_attach(HMODULE const instance,
    LPVOID const reserved)
{
    if (!__scrt_initialize_crt(__scrt_module_type::dll))
        return FALSE;

    bool const is_nested = __scrt_acquire_startup_lock();
    bool fail = true;
    __try
    {
        if (__scrt_current_native_startup_state != __scrt_native_startup_state::uninitialized)
            __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

        __scrt_current_native_startup_state = __scrt_native_startup_state::initializing;

        if (!__scrt_dllmain_before_initialize_c())
            __leave;

        #ifdef _RTC
        _RTC_Initialize();
        #endif

        __scrt_initialize_type_info();

        __scrt_initialize_default_local_stdio_options();

        if (_initterm_e(__xi_a, __xi_z) != 0)
            __leave;

        if (!__scrt_dllmain_after_initialize_c())
            __leave;

        _initterm(__xc_a, __xc_z);

        __scrt_current_native_startup_state = __scrt_native_startup_state::initialized;
        fail = false;
    }
    __finally
    {
        __scrt_release_startup_lock(is_nested);
    }
    if (fail)
        return FALSE;

    // If we have any dynamically initialized __declspec(thread) variables, we
    // invoke their initialization for the thread on which the DLL is being
    // loaded.  We cannot rely on the OS performing the initialization with the
    // DLL_PROCESS_ATTACH notification because, on Windows Server 2003 and below,
    // that call happens before the CRT is initialized.
    PIMAGE_TLS_CALLBACK const* const tls_init_callback = __scrt_get_dyn_tls_init_callback();
    if (*tls_init_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_init_callback))
    {
        (*tls_init_callback)(instance, DLL_THREAD_ATTACH, reserved);
    }

    ++__proc_attached;
    return TRUE;
}

static BOOL __cdecl dllmain_crt_process_detach(bool const is_terminating)
{
    // If the attach did not complete successfully, or if the detach was already
    // executed, do not execute the detach:
    if (__proc_attached <= 0)
        return FALSE;

    --__proc_attached;

    bool const is_nested = __scrt_acquire_startup_lock();
    __try
    {
        if (__scrt_current_native_startup_state != __scrt_native_startup_state::initialized)
            __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

        __scrt_dllmain_uninitialize_c();

        __scrt_uninitialize_type_info();

        #ifdef _RTC
        _RTC_Terminate();
        #endif

        __scrt_current_native_startup_state = __scrt_native_startup_state::uninitialized;
    }
    __finally
    {
        __scrt_dllmain_uninitialize_critical();
        __scrt_release_startup_lock(is_nested);
    }

    if (!__scrt_uninitialize_crt(is_terminating, false))
        return FALSE;

    return TRUE;
}



static BOOL WINAPI dllmain_crt_dispatch(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
    )
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH: return dllmain_crt_process_attach(instance, reserved);
    case DLL_PROCESS_DETACH: return dllmain_crt_process_detach(reserved != nullptr);
    case DLL_THREAD_ATTACH:  return __scrt_dllmain_crt_thread_attach();
    case DLL_THREAD_DETACH:  return __scrt_dllmain_crt_thread_detach();
    }

    return TRUE;
}

// Define the _CRT_INIT function for compatibility.
extern "C" BOOL WINAPI _CRT_INIT(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
    )
{
    return dllmain_crt_dispatch(instance, reason, reserved);
}

static BOOL WINAPI dllmain_raw(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
    )
{
    if (!_pRawDllMain)
        return TRUE;

    return _pRawDllMain(instance, reason, reserved);
}



static BOOL __cdecl dllmain_dispatch(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
    )
{
    // If this is a process detach notification, check that there was a prior
    // process attach notification that was processed successfully.  This is
    // to ensure that we don't detach more times than we attach.
    if (reason == DLL_PROCESS_DETACH && __proc_attached <= 0)
    {
        return FALSE;
    }

    BOOL result = TRUE;

    __try
    {
        if (reason == DLL_PROCESS_ATTACH || reason == DLL_THREAD_ATTACH)
        {
            result = dllmain_raw(instance, reason, reserved);
            if (!result)
                __leave;

            result = dllmain_crt_dispatch(instance, reason, reserved);
            if (!result)
                __leave;
        }

        result = DllMain(instance, reason, reserved);

        // If the client DllMain routine failed, unwind the initialization:
        if (reason == DLL_PROCESS_ATTACH && !result)
        {
            DllMain(instance, DLL_PROCESS_DETACH, reserved);
            dllmain_crt_dispatch(instance, DLL_PROCESS_DETACH, reserved);
            dllmain_raw(instance, DLL_PROCESS_DETACH, reserved);
        }

        if (reason == DLL_PROCESS_DETACH || reason == DLL_THREAD_DETACH)
        {
            result = dllmain_crt_dispatch(instance, reason, reserved);
            if (!result)
                __leave;

            result = dllmain_raw(instance, reason, reserved);
            if (!result)
                __leave;
        }
    }
    __except(__scrt_dllmain_exception_filter(
        instance,
        reason,
        reserved,
        dllmain_crt_dispatch,
        GetExceptionCode(),
        GetExceptionInformation()))
    {
        result = FALSE;
    }

    return result;
}



// The entry point for this DLL.
extern "C" BOOL WINAPI _DllMainCRTStartup(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
    )
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // The /GS security cookie must be initialized before any exception
        // handling targeting the current image is registered.  No function
        // using exception handling can be called in the current image until
        // after this call:
        __security_init_cookie();
    }

    return dllmain_dispatch(instance, reason, reserved);
}