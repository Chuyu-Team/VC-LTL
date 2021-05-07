//
// thread_safe_statics.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Helper functions used by thread-safe static initialization.
//
#ifdef _M_CEE
    #error This file cannot be built as managed
#endif

#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <limits.h>

static DWORD const xp_timeout = 100; // ms
static int const uninitialized = 0;
static int const being_initialized = -1;
static int const epoch_start = INT_MIN;

// Access to these variables is guarded in the below functions.  They may only
// be modified while the lock is held.  _Tss_epoch is readable from user
// code and is read without taking the lock.
extern "C"
{
    int _Init_global_epoch = epoch_start;
    __declspec(thread) int _Init_thread_epoch = epoch_start;
}

// On Vista or newer, the native CONDITION_VARIABLE type is used.  On XP, we use a simple
// Windows event.  This is not safe to use as a complete condition variable, but for the purposes
// of this feature the event is sufficient but not optimal.  See the code in _Tss_wait
// below.
//
// For Windows OS components:  The OS supports APISets downlevel to Windows 7,
// and OS components that run downlevel to Windows 7 may build against APISets.
// However, these components cannot use CONDITION_VARIABLE directly because it
// is not available via APISets until Windows 8.  Thus, for Windows OS components,
// we use the "ancient" code path and first try the APISet and then fall back to
// kernel32.dll.
#if defined _SCRT_ENCLAVE_BUILD || defined _CRT_APP || defined _CRT_WINDOWS_USE_VISTA_TSS \
    || (!defined _CRT_WINDOWS && (defined _ONECORE || defined _M_ARM || defined _M_ARM64))
#define _USE_VISTA_THREAD_SAFE_STATICS 1
#else
#define _USE_VISTA_THREAD_SAFE_STATICS 0
#endif

static CONDITION_VARIABLE g_tss_cv;

#if _USE_VISTA_THREAD_SAFE_STATICS
static SRWLOCK g_tss_srw;
#else // ^^^ _USE_VISTA_THREAD_SAFE_STATICS ^^^ // vvv !_USE_VISTA_THREAD_SAFE_STATICS vvv //
static HANDLE             g_tss_event;
static CRITICAL_SECTION   g_tss_mutex;
static decltype(SleepConditionVariableCS)* g_sleep_condition_variable_cs;
static decltype(WakeAllConditionVariable)* g_wake_all_condition_variable;

static void __cdecl __scrt_initialize_thread_safe_statics_platform_specific() noexcept
{
    // This can fail pre-Vista and that is ignored.
    InitializeCriticalSectionAndSpinCount(&g_tss_mutex, 4000);

    // CONDITION_VARIABLE is available via this APISet starting on Windows 8.
    HMODULE kernel_dll = GetModuleHandleW(L"api-ms-win-core-synch-l1-2-0.dll");
    if (kernel_dll == nullptr)
    {
        kernel_dll = GetModuleHandleW(L"kernel32.dll");
    }

    if (kernel_dll == nullptr)
    {
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    #define GET_PROC_ADDRESS(m, f) reinterpret_cast<decltype(f)*>(GetProcAddress(m, _CRT_STRINGIZE(f)))

    auto const sleep_condition_variable_cs = GET_PROC_ADDRESS(kernel_dll, SleepConditionVariableCS);
    auto const wake_all_condition_variable = GET_PROC_ADDRESS(kernel_dll, WakeAllConditionVariable);

    #undef GET_PROC_ADDRESS

    if (sleep_condition_variable_cs && wake_all_condition_variable)
    {
        g_sleep_condition_variable_cs = sleep_condition_variable_cs;
        g_wake_all_condition_variable = wake_all_condition_variable;
    }
    else
    {
        g_tss_event = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (g_tss_event == nullptr)
        {
            __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
        }
    }
}

// Terminator for synchronization data structures.
static void __cdecl __scrt_uninitialize_thread_safe_statics() noexcept
{
    DeleteCriticalSection(&g_tss_mutex);
    if (g_tss_event != nullptr)
    {
        CloseHandle(g_tss_event);
    }
}

// Initializer for synchronization data structures.
static int __cdecl __scrt_initialize_thread_safe_statics() noexcept
{
    __scrt_initialize_thread_safe_statics_platform_specific();

    // If CRT initialization was skipped then we should initialize the atexit tables.
    // This will only be needed when using a managed DLL with /NOENTRY specified.
    if (!__scrt_initialize_onexit_tables(__scrt_module_type::dll))
    {
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }
    atexit(__scrt_uninitialize_thread_safe_statics);
    return 0;
}

_CRTALLOC(".CRT$XIC") static _PIFV __scrt_initialize_tss_var = __scrt_initialize_thread_safe_statics;
#endif // _USE_VISTA_THREAD_SAFE_STATICS

// Helper functions for accessing the mutex and condition variable.  Can be replaced with
// more suitable data structures provided by the CRT, preferably ones that use the most
// efficient synchronization primitives available on the platform.
// This is not intended to be a recursive lock.
extern "C" void __cdecl _Init_thread_lock()
{
#if _USE_VISTA_THREAD_SAFE_STATICS
    AcquireSRWLockExclusive(&g_tss_srw);
#else // ^^^ _USE_VISTA_THREAD_SAFE_STATICS ^^^ // vvv !_USE_VISTA_THREAD_SAFE_STATICS vvv
    EnterCriticalSection(&g_tss_mutex);
#endif // _USE_VISTA_THREAD_SAFE_STATICS
}

extern "C" void __cdecl _Init_thread_unlock()
{
#if _USE_VISTA_THREAD_SAFE_STATICS
    ReleaseSRWLockExclusive(&g_tss_srw);
#else // ^^^ _USE_VISTA_THREAD_SAFE_STATICS ^^^ // vvv !_USE_VISTA_THREAD_SAFE_STATICS vvv
    LeaveCriticalSection(&g_tss_mutex);
#endif // _USE_VISTA_THREAD_SAFE_STATICS
}

// Wait on the condition variable.  In the XP implementation using only a Windows event
// we can't guarantee that we'll ever actually receive the notification signal, so we
// must use a non-infinite timeout.  This is not optimal: we may wake up early if the
// initializer is long-running, or we may miss the signal and not wake up until the
// timeout expires.  The signal may be missed because the sleeping threads may be
// stolen by the kernel to service an APC, or due to the race condition between the
// unlock call and the WaitForSingleObject call.
extern "C" void __cdecl _Init_thread_wait(DWORD const timeout)
{
#if _USE_VISTA_THREAD_SAFE_STATICS
    SleepConditionVariableSRW(&g_tss_cv, &g_tss_srw, timeout, 0);
#else // ^^^ _USE_VISTA_THREAD_SAFE_STATICS ^^^ // vvv !_USE_VISTA_THREAD_SAFE_STATICS vvv //
    if (g_sleep_condition_variable_cs)
    {
        // Vista+ code is first because it's most likely
        g_sleep_condition_variable_cs(&g_tss_cv, &g_tss_mutex, timeout);
        return;
    }

    _ASSERT(timeout != INFINITE);
    _Init_thread_unlock();
    WaitForSingleObjectEx(g_tss_event, timeout, FALSE);
    _Init_thread_lock();
#endif // _USE_VISTA_THREAD_SAFE_STATICS
}

extern "C" void __cdecl _Init_thread_notify()
{
#if _USE_VISTA_THREAD_SAFE_STATICS
    WakeAllConditionVariable(&g_tss_cv);
#else // ^^^ _USE_VISTA_THREAD_SAFE_STATICS ^^^ // vvv !_USE_VISTA_THREAD_SAFE_STATICS vvv
    if (g_wake_all_condition_variable)
    {
        // Vista+ code is first because it's most likely
        g_wake_all_condition_variable(&g_tss_cv);
        return;
    }

    SetEvent(g_tss_event);
    ResetEvent(g_tss_event);
#endif // _USE_VISTA_THREAD_SAFE_STATICS
}

// Control access to the initialization expression.  Only one thread may leave
// this function before the variable has completed initialization, this thread
// will perform initialization.  All other threads are blocked until the
// initialization completes or fails due to an exception.
extern "C" void __cdecl _Init_thread_header(int* const pOnce) noexcept
{
    _Init_thread_lock();

    if (*pOnce == uninitialized)
    {
        *pOnce = being_initialized;
    }
    else
    {
        while (*pOnce == being_initialized)
        {
            // Timeout can be replaced with an infinite wait when XP support is
            // removed or the XP-based condition variable is sophisticated enough
            // to guarantee all waiting threads will be woken when the variable is
            // signalled.
            _Init_thread_wait(xp_timeout);

            if (*pOnce == uninitialized)
            {
                *pOnce = being_initialized;
                _Init_thread_unlock();
                return;
            }
        }
        _Init_thread_epoch = _Init_global_epoch;
    }

    _Init_thread_unlock();
}

// Abort processing of the initializer due to an exception.  Reset the state
// to uninitialized and release waiting threads (one of which will take over
// initialization, any remaining will again sleep).
extern "C" void __cdecl _Init_thread_abort(int* const pOnce) noexcept
{
    _Init_thread_lock();
    *pOnce = uninitialized;
    _Init_thread_unlock();
    _Init_thread_notify();
}

// Called by the thread that completes initialization of a variable.
// Increment the global and per thread counters, mark the variable as
// initialized, and release waiting threads.
extern "C" void __cdecl _Init_thread_footer(int* const pOnce) noexcept
{
    _Init_thread_lock();
    ++_Init_global_epoch;
    *pOnce = _Init_global_epoch;
    _Init_thread_epoch = _Init_global_epoch;
    _Init_thread_unlock();
    _Init_thread_notify();
}
