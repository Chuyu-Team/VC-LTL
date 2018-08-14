//
// exe_common.inl
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The implementation of the common executable entry point code.  There are four
// executable entry points defined by the CRT, one for each of the user-definable
// entry points:
//
//  * mainCRTStartup     => main
//  * wmainCRTStartup    => wmain
//  * WinMainCRTStartup  => WinMain
//  * wWinMainCRTStartup => wWinMain
//
// These functions all behave the same, except for which user-definable main
// function they call and whether they accumulate and pass narrow or wide string
// arguments.  This file contains the common code shared by all four of those
// entry points.
//
// The actual entry points are defined in four .cpp files alongside this .inl
// file.  At most one of these .cpp files will be linked into the resulting
// executable, so we can treat this .inl file as if its contents are only linked
// into the executable once as well.
//
#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <locale.h>
#include <math.h>
#include <new.h>
#include <process.h>
#include <rtcapi.h>
#include <stdio.h>
#include <stdlib.h>



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Common main()/WinMain() implementation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct __scrt_main_policy
{
    static _crt_app_type get_app_type() throw() { return _crt_console_app; }
};

struct __scrt_winmain_policy
{
    static _crt_app_type get_app_type() throw() { return _crt_gui_app; }
};

#if defined _M_IX86
#define MinSubSystemVersion "5.01"
#elif defined _M_AMD64
#define MinSubSystemVersion "5.02"
#endif



#if defined _SCRT_STARTUP_MAIN

    using main_policy = __scrt_main_policy;
    using argv_policy = __scrt_narrow_argv_policy;
	#define __SUBSYSTEM "CONSOLE"

    static void __cdecl initialize_environment() throw()
    {
        _initialize_narrow_environment();
    }

    static int __cdecl invoke_main() throw()
    {
        return main(__argc, __argv, _environ);
    }

#elif defined _SCRT_STARTUP_WMAIN

    using main_policy = __scrt_main_policy;
    using argv_policy = __scrt_wide_argv_policy;
	#define __SUBSYSTEM "CONSOLE"

    static void __cdecl initialize_environment() throw()
    {
        _initialize_wide_environment();
    }

    static int __cdecl invoke_main() throw()
    {
        return wmain(__argc, __wargv, _wenviron);
    }

#elif defined _SCRT_STARTUP_WINMAIN

    using main_policy = __scrt_winmain_policy;
    using argv_policy = __scrt_narrow_argv_policy;
	#define __SUBSYSTEM "WINDOWS"

    static void __cdecl initialize_environment() throw()
    {
        _initialize_narrow_environment();
    }

    static int __cdecl invoke_main() throw()
    {
        return WinMain(
            reinterpret_cast<HINSTANCE>(&__ImageBase),
            nullptr,
            _get_narrow_winmain_command_line(),
            __scrt_get_show_window_mode());
    }

#elif defined _SCRT_STARTUP_WWINMAIN

    using main_policy = __scrt_winmain_policy;
    using argv_policy = __scrt_wide_argv_policy;
	#define __SUBSYSTEM "WINDOWS"

    static void __cdecl initialize_environment() throw()
    {
        _initialize_wide_environment();
    }

    static int __cdecl invoke_main() throw()
    {
        return wWinMain(
            reinterpret_cast<HINSTANCE>(&__ImageBase),
            nullptr,
            _get_wide_winmain_command_line(),
            __scrt_get_show_window_mode());
    }

#endif


#if _CRT_NTDDI_MIN < NTDDI_VISTA
#pragma comment(linker,"/SUBSYSTEM:" __SUBSYSTEM "," MinSubSystemVersion)
#endif

static int __cdecl pre_c_initialization() throw()
{
    _set_app_type(main_policy::get_app_type());

    _set_fmode(_get_startup_file_mode());
    *__p__commode() = _commode = _get_startup_commit_mode();

    if (!__scrt_initialize_onexit_tables(__scrt_module_type::exe))
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

    #ifdef _M_IX86
    // Clear the x87 exception flags.  Any other floating point initialization
    // should already have taken place before this function is called.
    _asm { fnclex }
    #endif

    #ifdef _RTC
    _RTC_Initialize();
    atexit(_RTC_Terminate);
    #endif

    if (argv_policy::configure_argv() != 0)
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

    __scrt_initialize_type_info();

    // If the user provided a _matherr handler, register it with the CoreCRT:
    if (__scrt_is_user_matherr_present())
        __setusermatherr(_matherr);

	#ifndef __Build_LTL //反正是2个空函数，就不调用了
    _initialize_invalid_parameter_handler();
    _initialize_denormal_control();
	#endif

    #ifdef _M_IX86
    _initialize_default_precision();
    #endif

	#ifndef __Build_LTL
	//默认禁用_get_startup_thread_locale_mode调用以支持WinXP，反正默认也是禁用thread locale。
    _configthreadlocale(_get_startup_thread_locale_mode());

	//LTL模式中环境变量由argv_policy::configure_argv()初始化
    if (_should_initialize_environment())
        initialize_environment();

    __scrt_initialize_winrt();
	#endif
    return 0;
}

static int __cdecl post_pgo_initialization() throw()
{
    // This function calls the __local_stdio_{printf,scanf}_options() functions.
    // These functions are defined in public headers with external linkage and
    // thus may be PGO-instrumented.  We must not call these functions before the
    // PGO instrumentation library is initialized.
    __scrt_initialize_default_local_stdio_options();

    return 0;
}

static void __cdecl pre_cpp_initialization() throw()
{
    // Before we begin C++ initialization, set the unhandled exception
    // filter so that unhandled C++ exceptions result in std::terminate
    // being called:
    __scrt_set_unhandled_exception_filter();

#ifndef __Build_LTL //LTL模式中由argv_policy::configure_argv()初始化
    _set_new_mode(_get_startup_new_mode());
#endif
}

// When both the PGO instrumentation library and the CRT are statically linked,
// PGO will initialize itself in XIAB.  We do most pre-C initialization before
// PGO is initialized, but defer some initialization steps to after.  See the
// commentary in post_pgo_initialization for details.
_CRTALLOC(".CRT$XIAA") static _PIFV pre_c_initializer    = pre_c_initialization;
_CRTALLOC(".CRT$XIAC") static _PIFV post_pgo_initializer = post_pgo_initialization;
_CRTALLOC(".CRT$XCAA") static _PVFV pre_cpp_initializer  = pre_cpp_initialization;



static __declspec(noinline) int __cdecl __scrt_common_main_seh() throw()
{
    if (!__scrt_initialize_crt(__scrt_module_type::exe))
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

    bool has_cctor = false;
    __try
    {
        bool const is_nested = __scrt_acquire_startup_lock();

        if (__scrt_current_native_startup_state == __scrt_native_startup_state::initializing)
        {
            __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
        }
        else if (__scrt_current_native_startup_state == __scrt_native_startup_state::uninitialized)
        {
            __scrt_current_native_startup_state = __scrt_native_startup_state::initializing;

            if (_initterm_e(__xi_a, __xi_z) != 0)
                return 255;

            _initterm(__xc_a, __xc_z);

            __scrt_current_native_startup_state = __scrt_native_startup_state::initialized;
        }
        else
        {
            has_cctor = true;
        }

        __scrt_release_startup_lock(is_nested);

        // If this module has any dynamically initialized __declspec(thread)
        // variables, then we invoke their initialization for the primary thread
        // used to start the process:
        _tls_callback_type const* const tls_init_callback = __scrt_get_dyn_tls_init_callback();
        if (*tls_init_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_init_callback))
        {
            (*tls_init_callback)(nullptr, DLL_THREAD_ATTACH, nullptr);
        }

        // If this module has any thread-local destructors, register the
        // callback function with the Unified CRT to run on exit.
		#ifndef __Build_LTL //无法正确实现_register_thread_local_exe_atexit_callback，反正进程都要退出了，thread-local destructors不调关系也不大。
        _tls_callback_type const * const tls_dtor_callback = __scrt_get_dyn_tls_dtor_callback();
        if (*tls_dtor_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_dtor_callback))
        {
            _register_thread_local_exe_atexit_callback(*tls_dtor_callback);
        }
		#endif
        //
        // Initialization is complete; invoke main...
        //

        int const main_result = invoke_main();

        //
        // main has returned; exit somehow...
        //

        if (!__scrt_is_managed_app())
            exit(main_result);

        if (!has_cctor)
            _cexit();

        // Finally, we terminate the CRT:
        __scrt_uninitialize_crt(true, false);
        return main_result;
    }
    __except (_seh_filter_exe(GetExceptionCode(), GetExceptionInformation()))
    {
        // Note:  We should never reach this except clause.
        int const main_result = GetExceptionCode();

        if (!__scrt_is_managed_app())
            _exit(main_result);

        if (!has_cctor)
            _c_exit();

        return main_result;
    }
}



// This is the common main implementation to which all of the CRT main functions
// delegate (for executables; DLLs are handled separately).
static __forceinline int __cdecl __scrt_common_main() throw()
{
    // The /GS security cookie must be initialized before any exception handling
    // targeting the current image is registered.  No function using exception
    // handling can be called in the current image until after this call:
    __security_init_cookie();

    return __scrt_common_main_seh();
}
