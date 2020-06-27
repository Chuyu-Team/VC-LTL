//
// vcstartup_internal.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Declarations and utilities used by the statically-linked CRT startup code.
//
#pragma once

#include <corecrt_startup.h>
#include <internal_shared.h>
#include <intrin.h>
#include <process.h>
#include <vcruntime_startup.h>
#include <Windows.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// EncodePointer and DecodePointer Redeclarations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// In the managed startup code, these functions must be re-declared with the
// appropriate security attributes, othrewise calls to them will result in
// System::Security::SecurityException exceptions being thrown.
_CRT_SUPPRESS_UNMANAGED_CODE_SECURITY
_CRT_SECURITYCRITICAL_ATTRIBUTE
_CRT_RELIABILITY_CONTRACT

#ifndef _ONECORE
    _CRT_INTEROPSERVICES_DLLIMPORT("KERNEL32.dll", "EncodePointer", _CRT_CALLING_CONVENTION_WINAPI)
#else  // _ONECORE
    _CRT_INTEROPSERVICES_DLLIMPORT("API-MS-WIN-CORE-UTIL-L1-1-0.dll", "EncodePointer", _CRT_CALLING_CONVENTION_WINAPI)
#endif // _ONECORE

WINBASEAPI _Ret_maybenull_ PVOID WINAPI EncodePointer(_In_opt_ PVOID _Ptr);

_CRT_SUPPRESS_UNMANAGED_CODE_SECURITY
_CRT_SECURITYCRITICAL_ATTRIBUTE
_CRT_RELIABILITY_CONTRACT

#ifndef _ONECORE
    _CRT_INTEROPSERVICES_DLLIMPORT("KERNEL32.dll", "DecodePointer", _CRT_CALLING_CONVENTION_WINAPI)
#else  // _ONECORE
    _CRT_INTEROPSERVICES_DLLIMPORT("API-MS-WIN-CORE-UTIL-L1-1-0.dll", "DecodePointer", _CRT_CALLING_CONVENTION_WINAPI)
#endif // _ONECORE

WINBASEAPI _Ret_maybenull_ PVOID WINAPI DecodePointer(_In_opt_ PVOID _Ptr);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Utility Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// The _VCRT_DECLARE_ALTERNATE_NAME macro provides an architecture-neutral way
// of specifying /alternatename comments to the linker.  It prepends the leading
// decoration character for x86 and hybrid and leaves names unmodified for other
// architectures.
#if defined _M_IX86
    #if defined _M_HYBRID
        #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "#"
    #else
        #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "_"
    #endif
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA "_"
#elif defined _M_ARM64EC
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "#"
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA ""
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64 || defined _M_ARM64EC
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX ""
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA ""
#else
    #error Unsupported architecture
#endif

#if defined _M_ARM64EC

#define _VCRT_DECLARE_ALTERNATE_NAME(name, alternate_name)  \
    __pragma(comment(linker,                                \
        "/alternatename:"                                   \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #name           \
        "="                                                 \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #alternate_name \
        ))                                                  //\
//    __pragma(comment(linker,                                \
//        "/alternatename:"                                   \
//        #name                                               \
//        "="                                                 \
//        #alternate_name \
//        ))

#else

#define _VCRT_DECLARE_ALTERNATE_NAME(name, alternate_name)  \
    __pragma(comment(linker,                                \
        "/alternatename:"                                   \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #name           \
        "="                                                 \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #alternate_name \
        ))

#endif

#define _VCRT_DECLARE_ALTERNATE_NAME_DATA(name, alternate_name)     \
    __pragma(comment(linker,                                        \
        "/alternatename:"                                           \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA #name              \
        "="                                                         \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA #alternate_name    \
        ))


// The _VCRT_DEFINE_IAT_SYMBOL macro provides an architecture-neutral way of
// defining IAT symbols (__imp_- or _imp__-prefixed symbols).
#ifdef _M_IX86
    #define _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(_imp__, f)
#else
    #define _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(__imp_, f)
#endif

#define _VCRT_DEFINE_IAT_SYMBOL(f)                                                          \
    extern "C" __declspec(selectany) void const* const _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) \
        = reinterpret_cast<void const*>(f)



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Client Entry Point Declarations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef _M_CEE

    extern "C" int __CRTDECL main(
        _In_                     int    argc,
        _In_reads_(argc) _Pre_z_ char** argv,
        _In_z_                   char** envp
        );

    extern "C" int __CRTDECL wmain(
        _In_                     int       argc,
        _In_reads_(argc) _Pre_z_ wchar_t** argv,
        _In_z_                   wchar_t** envp
        );

    extern "C" int WINAPI WinMain(
        _In_     HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_     LPSTR     lpCmdLine,
        _In_     int       nShowCmd
        );

    extern "C" int WINAPI wWinMain(
        _In_     HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_     LPWSTR    lpCmdLine,
        _In_     int       nShowCmd
        );

#endif

#ifdef MRTDLL

    extern "C" BOOL __clrcall DllMain(
        _In_ HINSTANCE hInstance,
        _In_ DWORD     fdwReason,
        _In_ LPVOID    lpvReserved
        );

#else

    extern "C" BOOL WINAPI DllMain(
        _In_ HINSTANCE hInstance,
        _In_ DWORD     fdwReason,
        _In_ LPVOID    lpvReserved
        );

#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Types
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef BOOL (WINAPI* __scrt_dllmain_type)(HINSTANCE, DWORD, LPVOID);

enum class __scrt_module_type
{
    dll,
    exe
};




//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Startup State Machine and Managed/Native Startup Synchronization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef _M_CEE_PURE

    enum class __scrt_native_startup_state
    {
        uninitialized,
        initializing,
        initialized
    };

    extern "C" extern __scrt_native_startup_state __scrt_current_native_startup_state;
    extern "C" extern void*                       __scrt_native_startup_lock;
    extern "C" extern unsigned int                __scrt_native_dllmain_reason;

    // When this module is executing its DllMain, the __scrt_native_dllmain_reason
    // flag stores the 'reason' with which DllMain was called.  When this module
    // is not executing its DllMain, the flag is set to this 'no reason' sentinel
    // value.
    #define NATIVE_DLLMAIN_REASON_NO_REASON UINT_MAX



    // Returns true if it is currently safe for managed code to execute in this
    // module; false otherwise.
    inline bool __scrt_is_safe_for_managed_code() noexcept
    {
        switch (__scrt_native_dllmain_reason)
        {
        case DLL_PROCESS_ATTACH:
        case DLL_PROCESS_DETACH:
            return false;

        default:
            return true;
        }
    }

#endif // _M_CEE_PURE



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// General Startup Utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" _crt_argv_mode __CRTDECL _get_startup_argv_mode();
extern "C" int __CRTDECL _get_startup_commit_mode();
extern "C" int __CRTDECL _get_startup_file_mode();
extern "C" int __CRTDECL _get_startup_new_mode();
extern "C" int __CRTDECL _get_startup_thread_locale_mode();

void __CRTDECL __scrt_initialize_type_info();
void __CRTDECL __scrt_uninitialize_type_info();

extern "C" bool __CRTDECL _should_initialize_environment();
extern "C" void __CRTDECL _initialize_default_precision();
extern "C" void __CRTDECL _initialize_invalid_parameter_handler();
extern "C" void __CRTDECL _initialize_denormal_control();

_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" void __CRTDECL __scrt_initialize_default_local_stdio_options();
extern "C" bool __cdecl __scrt_is_nonwritable_in_current_image(void const* target);
extern "C" int  __cdecl __scrt_is_user_matherr_present();
extern "C" int  __cdecl __scrt_is_ucrt_dll_in_use();

extern "C" _tls_callback_type const* __cdecl __scrt_get_dyn_tls_init_callback();
extern "C" _tls_callback_type const* __cdecl __scrt_get_dyn_tls_dtor_callback();

#ifdef _M_CEE_PURE
    extern "C" void __clrcall __scrt_initialize_stdio_msvcrt_compatibility_mode();
    extern "C" void __clrcall __scrt_initialize_legacy_stdio_wide_specifier_mode();
    extern "C" void __clrcall __scrt_initialize_printf_standard_rounding_mode();
    extern "C" void __clrcall __scrt_initialize_iso_stdio_wide_specifier_mode();
#endif

extern "C" extern int __crtWinrtInitType;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Static / Dynamic CRT Library Specific Functionality
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// These functions have different implementations for the static CRT and the CRT
// DLLs.  Startup and termination are subtly different between the two kinds of
// CRTs.

// When the CRT DLL is used, we must synchronize access to the startup code path.
// When all modules loaded in the process are native, there is no opportunity for
// problems here.  Managed module startup may cause unexpected reentrancy into
// the startup code path if something goes wrong.  These "locks" are used to
// accurately test for those problems.
extern "C" bool __cdecl __scrt_acquire_startup_lock();
extern "C" void __cdecl __scrt_release_startup_lock(bool is_nested);

// These are called during startup and termination to initialize or uninitialize
// the CRT.  When the CRT is statically linked, these actually perform complete
// CRT initialization, calling into each part of the CRT initialize them.  When
// the CRT DLLs are used, the CRT DLLs initialize themselves when they are loaded.
// However, some initialization is still required, to bind the module-specific
// VCRuntime DLL to the global AppCRT DLL.
extern "C" bool __cdecl __scrt_initialize_crt(__scrt_module_type module_type);
extern "C" bool __cdecl __scrt_uninitialize_crt(bool is_terminating, bool from_exit);

// When the CRT DLLs are used, special handling is required for the onexit table
// to ensure that functions are called at the right time during exit (during
// actual process termination for the EXE; during detach for DLLs).
extern "C" bool __cdecl __scrt_initialize_onexit_tables(__scrt_module_type module_type);

// When the CRT is statically linked and an exception occurs in the client DllMain
// during DLL_PROCESS_ATTACH, we must roll-back the static CRT initialization
// before calling into the exception filter to ensure that the CRT is not left in
// an unknown state.
extern "C" int __cdecl __scrt_dllmain_exception_filter(
    HINSTANCE           instance,
    DWORD               reason,
    LPVOID              reserved,
    __scrt_dllmain_type  crt_dllmain,
    unsigned long       exception_code_,
    PEXCEPTION_POINTERS exception_info_
    );

// When the CRT is statically linked, the CRT and client C termination is shared
// (there is only one atexit vector and one set of terminators to be run), so the
// DLL unload C uninitialization just calls the normal C termination code.  When
// the CRT DLLs are used, DLL unload does not cause C termination; it only causes
// execution of the onexit vector for the terminators in that DLL.
extern "C" bool __cdecl __scrt_dllmain_before_initialize_c();
extern "C" bool __cdecl __scrt_dllmain_after_initialize_c();
extern "C" void __cdecl __scrt_dllmain_uninitialize_c();
extern "C" void __cdecl __scrt_dllmain_uninitialize_critical();

// When the CRT is statically linked, the statically linked CRT startup entry
// point (the "real" DllMain) must notify the statically linked CRT components of
// the process and thread attach and detach notifications.  When the CRT DLLs are
// used, each CRT DLL will receive its own attach and detach notifications.
extern "C" bool __cdecl __scrt_dllmain_crt_thread_attach();
extern "C" bool __cdecl __scrt_dllmain_crt_thread_detach();



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Desktop / Windows Store App Specific Functionality
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// These parts of the startup routines have different implementations for Desktop
// apps and Windows Store apps.  In some cases, this is because some functionality
// applies only to one kind of app or the other.  In other cases, it is because we
// must do different things in Windows Store apps, which may not work on downlevel
// operating systems.

// Gets the current wShowWindow flag from the STARTUPINFO for this process, to
// be passed to WinMain.  In a Windows Store app, this function always returns
// zero, because the value is not required.
extern "C" WORD __cdecl __scrt_get_show_window_mode();

// Tests whether the EXE for this process is a managed module.  In Windows Store
// apps, this function always returns false, because we do not support C++/CLI
// and we do not need to support the complex mixed managed/native startup use
// cases for which this function is required.
extern "C" bool __cdecl __scrt_is_managed_app();

// Initializes the Windows Runtime (via RoInitialize) in the calling thread.
// In Windows Store apps, this is called during EXE startup.  In Desktop apps,
// this function has no effect.
extern "C" int __cdecl __scrt_initialize_winrt();

// Optionally initializes a process-wide MTA (via CoIncrementMTAUsage).
// In Desktop apps, this is conditional upon the exe_initialize_mta.lib linkopt.
// In Windows Store apps, enclaves, and DLLs, this has no effect.
extern "C" int __cdecl __scrt_initialize_mta();

// Initializes the unhandled exception filter.  In Desktop apps, this filter is
// what calls std::terminate when a C++ exception goes unhandled.  In Windows
// Store apps, this function has no effect (we cannot register an unhandled
// exception filter in a Windows Store app, and in most cases exceptions are
// caught at the ABI boundary and translated either into an HRESULT or a
// __failfast).  This function is called during the initialization of each EXE
// that uses this CRT.
extern "C" void __cdecl __scrt_set_unhandled_exception_filter();

// In Windows Store apps, and on computers where __fastfail is supported, this
// function invokes __fastfail.  Otherwise, this function resets the unhandled
// exception filter and calls the default unhandled exception filter with a
// noncontinuable exception, which is as close as we can get to __fastfail.
extern "C" __declspec(noreturn) void __cdecl __scrt_fastfail(unsigned code);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Heap Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_alloc();
__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_array_new_length();


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Thread-safe local static initialization support (shared with the managed implementation)
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern "C" int _Init_global_epoch;
extern "C" void __cdecl _Init_thread_lock();
extern "C" void __cdecl _Init_thread_unlock();
extern "C" void __cdecl _Init_thread_wait(DWORD const timeout);
extern "C" void __cdecl _Init_thread_notify();

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Common argv initialization utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct __scrt_narrow_argv_policy
{
    static int configure_argv() noexcept { return _configure_narrow_argv(_get_startup_argv_mode()); }
};

struct __scrt_wide_argv_policy
{
    static int configure_argv() noexcept { return _configure_wide_argv(_get_startup_argv_mode()); }
};

struct __scrt_no_argv_policy
{
    static int configure_argv() noexcept { return 0; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Common environment initialization utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct __scrt_narrow_environment_policy
{
    static int initialize_environment() noexcept { return _initialize_narrow_environment(); }
};

struct __scrt_wide_environment_policy
{
    static int initialize_environment() noexcept { return _initialize_wide_environment(); }
};

struct __scrt_no_environment_policy
{
    static int initialize_environment() noexcept { return 0; }
};

// Dll initialization policies

#ifdef _SCRT_ENCLAVE_BUILD

using __scrt_dll_argv_policy = __scrt_no_argv_policy;
using __scrt_dll_environment_policy = __scrt_no_environment_policy;

#else

using __scrt_dll_argv_policy = __scrt_narrow_argv_policy;
using __scrt_dll_environment_policy = __scrt_narrow_environment_policy;

#endif
