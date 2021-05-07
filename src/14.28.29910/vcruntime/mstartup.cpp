/***
*mstartup.cpp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*******************************************************************************/

#define _CPP_MSTARTUP

// #using is not used in the Windows build.
#ifndef _CRT_WINDOWS
    #ifdef _DEBUG
        #using <system.dll>
    #endif
#endif

/*
 * SPECIAL BUILD MACROS! Note that crtexe.c (and crtexew.c) is linked in with
 * the client's code. It does not go into crtdll.dll! Therefore, it must be
 * built under the _DLL switch (like user code) and CRTDLL must be undefined.
 * The symbol SPECIAL_CRTEXE is turned on to suppress the normal CRT DLL
 * definition of _fmode and _commode using __declspec(dllexport).  Otherwise
 * this module would not be able to refer to both the local and DLL versions
 * of these two variables.
 */

#undef MRTDLL

#define SPECIAL_CRTEXE

#include <crtdefs.h>

#ifdef _M_CEE_MIXED
    // Some inline functions (like NtCurrentTeb) need to be unmanaged
    #pragma managed(push, off)
    #include <Windows.h>
    #pragma managed(pop)
#else
    #include <Windows.h>
#endif

#include <conio.h>
#include <direct.h>
#include <eh.h>
#include <gcroot.h>
#include <malloc.h>
#include <math.h>
#include <minternal.h>
#include <process.h>
#include <time.h>
#include <vcclr.h>
#include <vcruntime_internal.h>
#include <vcstartup_internal.h>

#pragma warning(disable:4483)   // disable warning/error about __identifier(<string>)

// Note that managed items held in these arrays are really metadata tokens
// not clrcall function pointers so it's OK for these to be _PGLOBAL.

#if defined _M_CEE_MIXED

    #pragma const_seg(".CRTMP$XCA")
    extern "C" _PGLOBAL const _CRTALLOC(".CRTMP$XCA") _PVFVM __xc_mp_a[1] = { nullptr };

    #pragma const_seg(".CRTMP$XCZ")
    extern "C" _PGLOBAL const _CRTALLOC(".CRTMP$XCZ") _PVFVM __xc_mp_z[1] = { nullptr };

#endif

#pragma const_seg(".CRTMA$XCA")
extern "C" _PGLOBAL const _CRTALLOC(".CRTMA$XCA") _PVFVM __xc_ma_a[1] = { nullptr };

#pragma const_seg(".CRTMA$XCZ")
extern "C" _PGLOBAL const _CRTALLOC(".CRTMA$XCZ") _PVFVM __xc_ma_z[1] = { nullptr };

#pragma const_seg(".CRTVT$XCA")
extern "C" _PGLOBAL const _CRTALLOC(".CRTVT$XCA") _PVFVM __xi_vt_a[1] = { nullptr };

#pragma const_seg(".CRTVT$XCZ")
extern "C" _PGLOBAL const _CRTALLOC(".CRTVT$XCZ") _PVFVM __xi_vt_z[1] = { nullptr };

#pragma const_seg()  // Reset

#pragma comment(linker, "/merge:.CRTMP=.rdata")
#pragma comment(linker, "/merge:.CRTMA=.rdata")
#pragma comment(linker, "/merge:.CRTVT=.rdata")

#pragma comment(linker, "/defaultlib:kernel32.lib")

#ifdef _DEBUG
    #pragma comment(linker, "/defaultlib:ucrtd.lib")
#else
    #pragma comment(linker, "/defaultlib:ucrt.lib")
#endif

#if defined _M_CEE_MIXED
    extern "C" _PGLOBAL _CRTALLOC(".CRT$XIA") _PIFV __xi_a[];
    extern "C" _PGLOBAL _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[];    /* C initializers */
    extern "C" _PGLOBAL _CRTALLOC(".CRT$XCA") _PVFV __xc_a[];
    extern "C" _PGLOBAL _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[];    /* C++ initializers */
#endif

#if defined _M_CEE_MIXED
    extern "C" int __clrcall _initatexit_m();
    extern "C" void __clrcall _exit_callback();
#endif

extern "C" int __clrcall _initatexit_app_domain();
extern "C" void __clrcall _app_exit_callback();

int __clrcall _initterm_e ( _PIFV *, _PIFV *);
void __clrcall _initterm ( _PVFV *, _PVFV *);
void __clrcall _initterm_m ( const _PVFVM *, const _PVFVM *);

#if defined _M_CEE_MIXED

    #pragma managed(push, off)
    [System::Diagnostics::DebuggerStepThroughAttribute]
    void * __cdecl _getFiberPtrId()
    {
        return ((PNT_TIB)NtCurrentTeb())->StackBase;
    }

    extern "C"
    void __cdecl __security_init_cookie(void);
    #pragma managed(pop)

#endif

namespace __identifier("<CrtImplementationDetails>")
{

enum class TriBool
{
    False   =  0,
    True    = -1,
    Unknown =  2
};

enum class Progress
{
    NotStarted,
    Started,
    Finished
};

#if defined (_M_CEE_MIXED)

class AllDomains
{
public:
    // The number of domains in which the module is loaded
    _PGLOBAL static int Count;
};
_PGLOBAL int AllDomains::Count = 0;

class DefaultDomain
{
private:
    _PGLOBAL static TriBool hasPerProcess;
    _PGLOBAL static TriBool hasNative;
    /*static void __cdecl DoNothing()
    {
    }*/
    // No secret exposed; no side effect
    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    static HRESULT __stdcall DoNothing(void* cookie)
    {
        cookie;
        // Execute simple managed code
        System::GC::KeepAlive(System::Int32::MaxValue);
        return S_OK;
    }

public:
    _PGLOBAL static bool Entered;
    _PGLOBAL static bool InitializedNative;
    _PGLOBAL static bool InitializedNativeFromCCTOR;
    _PGLOBAL static bool InitializedPerProcess;

    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    inline static bool HasPerProcess()
    {
        if (hasPerProcess == TriBool::Unknown)
        {
            void **pBeg = (void **)(void *)__xc_mp_a;
            void **pEnd = (void **)(void *)__xc_mp_z;
            while (pBeg < pEnd)
            {
                if (*pBeg != 0)
                {
                    hasPerProcess = TriBool::True;
                    return true;
                }
                pBeg++;
            }

            hasPerProcess = TriBool::False;
            return false;
        }
        return hasPerProcess == TriBool::True;
    }

    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    inline static bool HasNative()
    {
        if (hasNative == TriBool::Unknown)
        {
            void **pBeg = (void **)(void *)__xi_a;
            void **pEnd = (void **)(void *)__xi_z;

            while (pBeg < pEnd)
            {
                if (*pBeg != 0)
                {
                    hasNative = TriBool::True;
                    return true;
                }
                pBeg++;
            }

            pBeg = (void **)(void *)__xc_a;
            pEnd = (void **)(void *)__xc_z;
            while (pBeg < pEnd)
            {
                if (*pBeg != 0)
                {
                    hasNative = TriBool::True;
                    return true;
                }
                pBeg++;
            }

            hasNative = TriBool::False;
            return false;
        }
        return hasNative == TriBool::True;
    }

    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    inline static bool NeedsInitialization()
    {
        return ((HasPerProcess() && !InitializedPerProcess) || (HasNative() && !InitializedNative && (__scrt_current_native_startup_state == __scrt_native_startup_state::uninitialized)));
    }

    inline static bool NeedsUninitialization()
    {
        // If the default domain has been entered then there's a chance that
        // atexit has been called and we need to transition into it to uninitialize it.
        return Entered;
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline static void Initialize()
    {
        // Trigger call to the module constructor
        DoCallBackInDefaultDomain(&DoNothing, nullptr);
    }

private:
    DefaultDomain();
    ~DefaultDomain();
};

_PGLOBAL bool DefaultDomain::Entered = false;
_PGLOBAL bool DefaultDomain::InitializedNative = false;
_PGLOBAL bool DefaultDomain::InitializedNativeFromCCTOR = false;
_PGLOBAL bool DefaultDomain::InitializedPerProcess = false;
_PGLOBAL TriBool DefaultDomain::hasPerProcess = TriBool::Unknown;
_PGLOBAL TriBool DefaultDomain::hasNative = TriBool::Unknown;

#endif  /* defined (_M_CEE_MIXED) */

class CurrentDomain
{
public:
    _AGLOBAL static int Initialized;
    _AGLOBAL static int Uninitialized;
    _AGLOBAL static bool IsDefaultDomain;
    _AGLOBAL static Progress InitializedVtables;
    _AGLOBAL static Progress InitializedNative;
    _AGLOBAL static Progress InitializedPerProcess;
    _AGLOBAL static Progress InitializedPerAppDomain;
private:
    CurrentDomain();  // Not instantiable
    ~CurrentDomain(); // Not instantiable
};

_AGLOBAL int CurrentDomain::Initialized = 0;
_AGLOBAL int CurrentDomain::Uninitialized = 0;
_AGLOBAL bool CurrentDomain::IsDefaultDomain = false;
_AGLOBAL Progress CurrentDomain::InitializedVtables = Progress::NotStarted;
_AGLOBAL Progress CurrentDomain::InitializedNative = Progress::NotStarted;
_AGLOBAL Progress CurrentDomain::InitializedPerProcess = Progress::NotStarted;
_AGLOBAL Progress CurrentDomain::InitializedPerAppDomain = Progress::NotStarted;

class LanguageSupport
{
private:
    // If a non-CRT exception is thrown, this member
    // holds a constant describing the current activity
    gcroot<String^> ErrorMessage;

    // Initializes vtables (which are per-appdomain)
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    inline void InitializeVtables()
    {
        ErrorMessage = L"The C++ module failed to load during vtable initialization.\n";
        CurrentDomain::InitializedVtables = Progress::Started;
        _initterm_m((const _PVFVM *)__xi_vt_a, (const _PVFVM *)__xi_vt_z);
        CurrentDomain::InitializedVtables = Progress::Finished;
    }

#ifdef _M_CEE_MIXED

    // This is a blocking call which won't return until the default app domain is initialized
    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    inline void InitializeDefaultAppDomain()
    {
        ErrorMessage = L"The C++ module failed to load while attempting to initialize the default appdomain.\n";
        DefaultDomain::Initialize();
    }

    // Initializes native globals
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    inline void InitializeNative()
    {
        MANAGED_ASSERT(CurrentDomain::IsDefaultDomain, "Native globals must be initialized in the default domain");

        ErrorMessage = L"The C++ module failed to load during native initialization.\n";

        __security_init_cookie();

        DefaultDomain::InitializedNative = true;

        if (!__scrt_is_safe_for_managed_code())
        {
            abort();
        }

        /*
        * In mixed code we make sure to initialize native globals as well.
        */
        if (__scrt_current_native_startup_state == __scrt_native_startup_state::initializing)
        {
            abort();
        }
        else if (__scrt_current_native_startup_state == __scrt_native_startup_state::uninitialized)
        {
            CurrentDomain::InitializedNative = Progress::Started;
            __scrt_current_native_startup_state = __scrt_native_startup_state::initializing;
            if (_initterm_e(__xi_a, __xi_z) != 0)
            {
                ThrowModuleLoadException(ErrorMessage);
            }

            _initterm(__xc_a, __xc_z);
            __scrt_current_native_startup_state = __scrt_native_startup_state::initialized;
            DefaultDomain::InitializedNativeFromCCTOR = true;
            CurrentDomain::InitializedNative = Progress::Finished;
        }
    }

    // Initializes per-process managed globals
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline void InitializePerProcess()
    {
        MANAGED_ASSERT(CurrentDomain::IsDefaultDomain, "Per-process globals must be initialized in the default domain");

        ErrorMessage = L"The C++ module failed to load during process initialization.\n";

        CurrentDomain::InitializedPerProcess = Progress::Started;

        _initatexit_m();

        _initterm_m((const _PVFVM *) __xc_mp_a, (const _PVFVM *)__xc_mp_z );

        CurrentDomain::InitializedPerProcess = Progress::Finished;

        DefaultDomain::InitializedPerProcess = true;
    }

#endif  /* defined (_M_CEE_MIXED) */

#if defined _M_CEE_PURE
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline bool IsDllNameCppSupportLike(System::String^ dllName, System::String^ prefix)
    {
        using namespace System;

        // Similar to case insensitive regex: msvcp\d+d?\.dll
        if (!dllName->StartsWith(prefix, StringComparison::OrdinalIgnoreCase))
        {
            return false;
        }

        const Int32 dllNameLength = dllName->Length;
        const Int32 numbersStart = prefix->Length;
        Int32 index = numbersStart;
        while (index < dllNameLength && dllName[index] >= '0' && dllName[index] <= '9')
        {
            ++index;
        }

        if (index == numbersStart)
        {
            return false;
        }

        if (index < dllNameLength && (dllName[index] == 'd' || dllName[index] == 'D'))
        {
            ++index;
        }

        return index + 4 == dllNameLength && dllName->EndsWith(".dll",
            StringComparison::OrdinalIgnoreCase);
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline bool IsRuntimeDllName(System::String^ dllName)
    {
        using namespace System;
        // Debugging uCRT
        if (dllName->Equals("ucrtbased.dll", StringComparison::OrdinalIgnoreCase))
        {
            return true;
        }

        // Release uCRT
        if (dllName->StartsWith("api-ms-win-crt-", StringComparison::OrdinalIgnoreCase)
            && dllName->EndsWith(".dll", StringComparison::OrdinalIgnoreCase))
        {
            return true;
        }

        // C++ support
        return IsDllNameCppSupportLike(dllName, "msvcp")
            || IsDllNameCppSupportLike(dllName, "vcruntime")
            || IsDllNameCppSupportLike(dllName, "concrt");
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline void ForceLoadRuntimeApis()
    {
        using namespace System;
        using namespace System::Reflection;
        using namespace System::Runtime::InteropServices;

        // Load CRT-ish DLLs before we call into any user code, and prelink
        // methods implemented therein.
        //
        // If we do not force-load the DLLs here, they are loaded on demand the
        // first time we use a function from one of them, which can cause
        // unexpected reentrancy into code defined in the STL headers as global
        // initializers are processed for the msvcp DLL or in the CRT DLLs.
        //
        // If we do not prelink the functions here, the CLR will insert
        // LoadLibrary calls in each place they are called the first time,
        // leading to deadlocks if the first place they are called happens to be
        // holding a lock.
        //
        // To force-load the DLLs, loop over the methods for the native APIs
        // this assembly links with, and if they are in a DLL we care about,
        // Marshal::Prelink them.
        Assembly^ thisAsm = Assembly::GetExecutingAssembly();
        for each (Module^ mod in thisAsm->Modules)
        {
            auto methods = mod->GetMethods(BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::Static);
            for each (MethodInfo^ method in methods)
            {
                auto attributes = method->GetCustomAttributes(DllImportAttribute::typeid, false);
                if (attributes == nullptr || attributes->Length != 1)
                {
                    continue;
                }

                auto dllImportAttribute = safe_cast<DllImportAttribute^>(attributes[0]);
                if (!IsRuntimeDllName(dllImportAttribute->Value))
                {
                    continue;
                }

                try
                {
                    Marshal::Prelink(method);
                }
                catch (Exception^)
                {
                    // we want any loading exceptions to get handled by user code, not this startup code
                }
            }
        }
    }
#endif /* defined _M_CEE_PURE */

    // Initializes per-appdomain managed globals
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline void InitializePerAppDomain()
    {
        ErrorMessage = L"The C++ module failed to load during appdomain initialization.\n";

        CurrentDomain::InitializedPerAppDomain = Progress::Started;

        #if defined _M_CEE_PURE
        ForceLoadRuntimeApis();

        __scrt_initialize_stdio_msvcrt_compatibility_mode();
        __scrt_initialize_default_local_stdio_options();
        __scrt_initialize_printf_standard_rounding_mode();
        __scrt_initialize_iso_stdio_wide_specifier_mode();
        __scrt_initialize_legacy_stdio_wide_specifier_mode();
        #endif

        _initatexit_app_domain();

        _initterm_m( __xc_ma_a, __xc_ma_z );

        CurrentDomain::InitializedPerAppDomain = Progress::Finished;
    }

    // Registers for appdomain unload event
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline void InitializeUninitializer()
    {
        ErrorMessage = L"The C++ module failed to load during registration for the unload events.\n";
        EventHandler^ handler = gcnew EventHandler(&DomainUnload);
        RegisterModuleUninitializer(handler);
    }

    // Initializes vtables and globals required for C++ language
    [System::Diagnostics::DebuggerStepThroughAttribute]
    [System::Runtime::ConstrainedExecution::ReliabilityContract(
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState,
        System::Runtime::ConstrainedExecution::Cer::Success)]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline void _Initialize()
    {
        CurrentDomain::IsDefaultDomain = AppDomain::CurrentDomain->IsDefaultAppDomain();
#if defined (_M_CEE_MIXED)
        if (CurrentDomain::IsDefaultDomain)
        {
            DefaultDomain::Entered = true;
        }
#endif  /* defined (_M_CEE_MIXED) */

#if defined (_M_CEE_MIXED)
        void *lock_free=0;
        void *fiberid=_getFiberPtrId();
        int nested=false;
        int done=false;
        int doInitializeDefaultAppDomain=false;
        System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
        // this make sure that __scrt_native_startup_lock is released no matter what, in line with the
        // ReliabilityContract asserted above
        try
        {
            while (!done)
            {
                try { }
                finally
                {
#if defined (_WIN64)
                    lock_free = (void*)Interlocked::CompareExchange((System::Int64&)__scrt_native_startup_lock, (System::Int64)fiberid, (System::Int64)0);
#else  /* defined (_WIN64) */
                    lock_free = (void*)Interlocked::CompareExchange((System::IntPtr&)__scrt_native_startup_lock, (System::IntPtr)fiberid, (System::IntPtr)0);
#endif  /* defined (_WIN64) */
                    if (lock_free == 0)
                    {
                        done = true;
                    }
                    else if(lock_free == fiberid)
                    {
                        nested = true;
                        done = true;
                    }
                }

                if (!done)
                {
                    // Some other thread is running native startup/shutdown during a cctor/domain unload.
                    // Wait for the other thread to complete init before we return
                    Sleep(1000);
                }
            }

            InitializeVtables();

            if (CurrentDomain::IsDefaultDomain)
            {
                    InitializeNative();
                    InitializePerProcess();
            }
            else if(DefaultDomain::NeedsInitialization())
            {
                // Do not InitializeDefaultAppDomain() when holding __scrt_native_startup_lock.
                // Essentially InitializeDefaulAppDomain() is a callback to the
                // default appdomain.  If we do the call back while holding the lock, it can
                // result in a deadlock.
                //
                // For example:
                // 1. thread1: .cctor() of a non-default appdomain calls _Initialize() and
                //    grabs the _native_startup_lock
                // 2. thread2: .cctor() of default appdomain starts to execute and tries to grab the
                //             __scrt_native_startup_lock, but fails.  Hence, busy waiting for lock.
                // 3. thread1: (still holding the lock) and do a callback to default appdomain.
                //             The CLR detects that .cctor of default appdomain has started, but
                //             not finish.  Hence block until .cctor of default app domain finishes.
                // 4. deadlock.
                doInitializeDefaultAppDomain = true;
            }
        }
        finally
        {
            if(!nested)
            {
#if defined (_WIN64)
                Interlocked::Exchange((System::Int64&)__scrt_native_startup_lock, (System::Int64)0);
#else  /* defined (_WIN64) */
                Interlocked::Exchange((System::IntPtr&)__scrt_native_startup_lock, (System::IntPtr)0);
#endif  /* defined (_WIN64) */
            }
        }

        /*
         * Note: Since this out of the lock, it is possible that we are
         * doing extra calls to InitializeDefaultAppDomain().
         * Since InitializeDefaultAppDomain()is just a dummy callback to force the
         * .cctor() of the default appdomain to run, it is save to call
         * InitalizeDefaultAppDomain() more than once
         * (although we will suffer a domain transition performance penalty.)
         */
        if (doInitializeDefaultAppDomain)
        {
            InitializeDefaultAppDomain();
        }
#else  /* defined (_M_CEE_MIXED) */
        InitializeVtables();

#endif  /* defined (_M_CEE_MIXED) */

        InitializePerAppDomain();

        CurrentDomain::Initialized = 1;

        InitializeUninitializer();
    }

    // Uninitializes per-appdomain managed globals
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    inline static void UninitializeAppDomain()
    {
        _app_exit_callback();
    }

#if defined (_M_CEE_MIXED)
    //static void __cdecl _UninitializeDefaultDomain()
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static HRESULT __stdcall _UninitializeDefaultDomain(void* cookie)
    {
        cookie;
        MANAGED_ASSERT(AppDomain::CurrentDomain->IsDefaultAppDomain(), "This function must be called in the default domain");

        // Uninitialize per-process
        _exit_callback();

        DefaultDomain::InitializedPerProcess = false;

        if(DefaultDomain::InitializedNativeFromCCTOR)
        {
            // Uninitialize native
            _cexit();
            __scrt_current_native_startup_state = __scrt_native_startup_state::uninitialized;

            DefaultDomain::InitializedNativeFromCCTOR = false;
        }

        DefaultDomain::InitializedNative = false;

        return S_OK;
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void UninitializeDefaultDomain()
    {
        if (DefaultDomain::NeedsUninitialization())
        {
            if (AppDomain::CurrentDomain->IsDefaultAppDomain())
            {
                _UninitializeDefaultDomain(nullptr);
            }
            else
            {
                DoCallBackInDefaultDomain(&_UninitializeDefaultDomain, nullptr);
            }
        }
    }
#endif  /* defined (_M_CEE_MIXED) */

    // Called when the domain unloads
    // Compare with code in Cleanup
    [System::Runtime::ConstrainedExecution::PrePrepareMethod]
    [System::Runtime::ConstrainedExecution::ReliabilityContract(
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState,
        System::Runtime::ConstrainedExecution::Cer::Success)]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void DomainUnload(Object^, EventArgs^)
    {
        if (CurrentDomain::Initialized && !Interlocked::Exchange(CurrentDomain::Uninitialized, (int)1))
        {
#if defined (_M_CEE_MIXED)
            bool lastDomain = (Interlocked::Decrement(AllDomains::Count) == 0);
#endif  /* defined (_M_CEE_MIXED) */

            UninitializeAppDomain();

#if defined (_M_CEE_MIXED)
            if (lastDomain)
            {
                UninitializeDefaultDomain();
            }
#endif  /* defined (_M_CEE_MIXED) */
        }
    }

    // Cleans up initialization in the event of an error
    // Compare with code in DomainUnload
    [System::Diagnostics::DebuggerStepThroughAttribute]
    [System::Runtime::ConstrainedExecution::ReliabilityContract(
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState,
        System::Runtime::ConstrainedExecution::Cer::Success)]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void Cleanup(System::Exception^ innerException)
    {
        try
        {
#if defined (_M_CEE_MIXED)
            // If we end up here, DomainUnload will not be called
            bool lastDomain = (Interlocked::Decrement(AllDomains::Count) == 0);
#endif  /* defined (_M_CEE_MIXED) */

            /*
             * Even though the current AppDomain might not be fully initialized,
             * we still want to call whatever that got registered in the on exit
             * list.
             */
            UninitializeAppDomain();

#if defined (_M_CEE_MIXED)
            if (lastDomain)
            {
                UninitializeDefaultDomain();
            }
#endif  /* defined (_M_CEE_MIXED) */
        }
        catch (System::Exception^ exception)
        {
            ThrowNestedModuleLoadException(innerException, exception);
        }
        catch (System::Object^ exception)
        {
            exception;
            ThrowNestedModuleLoadException(innerException, nullptr);
        }
    }

public:
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    LanguageSupport(){}
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    ~LanguageSupport(){}

    [System::Diagnostics::DebuggerStepThroughAttribute]
    [System::Runtime::ConstrainedExecution::ReliabilityContract(
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState,
        System::Runtime::ConstrainedExecution::Cer::Success)]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void Initialize()
    {
#if defined (_M_CEE_MIXED)
        bool allDomainsCountIncremented = false;

        // this make sure that AllDomains::Count reflects the actual count no matter what,
        // in line with the ReliabilityContract asserted above
        System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
#endif  /* defined (_M_CEE_MIXED) */
        try
        {
            ErrorMessage = L"The C++ module failed to load.\n";
#if defined (_M_CEE_MIXED)
            System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
            try { } finally
            {
                Interlocked::Increment(AllDomains::Count);
                allDomainsCountIncremented = true;
            }
#endif  /* defined (_M_CEE_MIXED) */
            _Initialize();
        }
        catch (System::Exception^ exception)
        {
#if defined (_M_CEE_MIXED)
            if (allDomainsCountIncremented)
#endif  /* defined (_M_CEE_MIXED) */
            {
                Cleanup(exception);
            }
            ThrowModuleLoadException(ErrorMessage, exception);
        }
        catch (System::Object^ exception)
        {
            exception;
#if defined (_M_CEE_MIXED)
            if (allDomainsCountIncremented)
#endif  /* defined (_M_CEE_MIXED) */
            {
                Cleanup(nullptr);
            }
            ThrowModuleLoadException(ErrorMessage, nullptr);
        }
    }

};

// we do lots of nasty things for /clr & /clr:pure - consuming code should
// automatically be marked for compatibility with the security model enforced in CLR v4+
[ assembly: System::Security::SecurityRules( System::Security::SecurityRuleSet::Level1 ) ];

} // namespace __identifier("<CrtImplementationDetails>")

/***
* .cctor - This is global constructor for user module.
*
* Purpose:
*       This function is called during appdomain creation. Here is we
*       initialize all the vtables, and native/process/appdomain variables.
*
* Exit:
*
* Entry:
*
*******************************************************************************/
[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __clrcall __identifier(".cctor")()
{
    using namespace __identifier("<CrtImplementationDetails>");

    LanguageSupport languageSupport;
    languageSupport.Initialize();
}
