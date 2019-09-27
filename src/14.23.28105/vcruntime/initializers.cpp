//
// initializers.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Data used in C and C++ initialization and termination.  Because this object
// is linked into every module that uses the CRT, we also use this module to
// pass flags to the linker to link with libraries upon which the CRT depends.
//
#include <vcstartup_internal.h>
#include <vcruntime_internal.h>



extern "C" _CRTALLOC(".CRT$XIA") _PIFV __xi_a[] = { nullptr }; // C initializers (first)
extern "C" _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[] = { nullptr }; // C initializers (last)
extern "C" _CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { nullptr }; // C++ initializers (first)
extern "C" _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { nullptr }; // C++ initializers (last)
extern "C" _CRTALLOC(".CRT$XPA") _PVFV __xp_a[] = { nullptr }; // C pre-terminators (first)
extern "C" _CRTALLOC(".CRT$XPZ") _PVFV __xp_z[] = { nullptr }; // C pre-terminators (last)
extern "C" _CRTALLOC(".CRT$XTA") _PVFV __xt_a[] = { nullptr }; // C terminators (first)
extern "C" _CRTALLOC(".CRT$XTZ") _PVFV __xt_z[] = { nullptr }; // C terminators (last)

#pragma comment(linker, "/merge:.CRT=.rdata")



// When we link statically to the Universal CRT and the VCRuntime, we have to use
// these functions to initialize and terminate the statically-linked CRT code.
// When we link to the Universal CRT DLL or the VCRuntime DLL, these functions do
// not exist (they are internal implementation details of their respective DLLs
// and they are not exported, so the symbols simply do not exist).
//
// In order to support linking with both the static libraries and the DLLs, we
// provide stub alternatives for each of these functions.  When we link statically
// to the Universal CRT and the VCRuntime, we'll get the real definitions.  When
// we link to the DLLs, the linker will fail to find the real definitions and
// will instead use these alternative stubs.
_VCRT_DECLARE_ALTERNATE_NAME(__acrt_initialize,            __scrt_stub_for_acrt_initialize           )
_VCRT_DECLARE_ALTERNATE_NAME(__acrt_uninitialize,          __scrt_stub_for_acrt_uninitialize         )
_VCRT_DECLARE_ALTERNATE_NAME(__acrt_uninitialize_critical, __scrt_stub_for_acrt_uninitialize_critical)
_VCRT_DECLARE_ALTERNATE_NAME(__acrt_thread_attach,         __scrt_stub_for_acrt_thread_attach        )
_VCRT_DECLARE_ALTERNATE_NAME(__acrt_thread_detach,         __scrt_stub_for_acrt_thread_detach        )
_VCRT_DECLARE_ALTERNATE_NAME(_is_c_termination_complete,   __scrt_stub_for_is_c_termination_complete )

_VCRT_DECLARE_ALTERNATE_NAME(__vcrt_initialize,            __scrt_stub_for_acrt_initialize           )
_VCRT_DECLARE_ALTERNATE_NAME(__vcrt_uninitialize,          __scrt_stub_for_acrt_uninitialize         )
_VCRT_DECLARE_ALTERNATE_NAME(__vcrt_uninitialize_critical, __scrt_stub_for_acrt_uninitialize_critical)
_VCRT_DECLARE_ALTERNATE_NAME(__vcrt_thread_attach,         __scrt_stub_for_acrt_thread_attach        )
_VCRT_DECLARE_ALTERNATE_NAME(__vcrt_thread_detach,         __scrt_stub_for_acrt_thread_detach        )



#if defined _CRT_APP
    // Link no platform libraries by default for Windows Store
#elif defined _ONECORE
    // Link with the APISet umbrella library for OneCore
    #pragma comment(linker, "/defaultlib:onecore.lib")
#else
    // Link with the legacy kernel32.lib for the "normal" libraries
    #pragma comment(linker, "/defaultlib:kernel32.lib")
    #if _VCRT_WIN32_WINNT >= _WIN32_WINNT_WIN8
        // Also link with the WinRT library when building for Windows 8 and above
        #pragma comment(linker, "/defaultlib:runtimeobject.lib")
    #endif
#endif




// Generate /defaultlib and /disallowlib options to drag the other parts of the
// CRT (the VCRuntime and the Universal CRT) into the link and prevent mismatching
// libraries from being linked.
#define EXPAND_LINKER_OPTION_MD(...)  __VA_ARGS__
#define EXPAND_LINKER_OPTION_XMD(...) __VA_ARGS__
#define EXPAND_LINKER_OPTION_MT(...)  __VA_ARGS__
#define EXPAND_LINKER_OPTION_XMT(...) __VA_ARGS__

#define LINKER_OPTION_MD  "/disallowlib"
#define LINKER_OPTION_XMD "/disallowlib"
#define LINKER_OPTION_MT  "/disallowlib"
#define LINKER_OPTION_XMT "/disallowlib"

#ifdef CRTDLL
    #ifdef _DEBUG
        #undef  LINKER_OPTION_XMD
        #define LINKER_OPTION_XMD "/defaultlib"
    #else
        #undef  LINKER_OPTION_MD
        #define LINKER_OPTION_MD "/defaultlib"
    #endif
#else // ^^^ CRTDLL ^^^ // vvv !CRTDLL vvv //
    #ifdef _DEBUG
        #undef  LINKER_OPTION_XMT
        #define LINKER_OPTION_XMT "/defaultlib"
    #else
        #undef  LINKER_OPTION_MT
        #define LINKER_OPTION_MT "/defaultlib"
    #endif
#endif // !CRTDLL

#ifdef _MSVCRT_WINDOWS
    // In all cases where the Windows libraries are used, /nodefaultlib is
    // also used, so suppress pragma generation for the Windows libraries.
    #define GENERATE_LINKER_OPTION(model, library)
#else
    #define GENERATE_LINKER_OPTION(model, library)                                         \
        _CRT_CONCATENATE(EXPAND_LINKER_OPTION_, model)(                                    \
            __pragma(comment(linker, _CRT_CONCATENATE(LINKER_OPTION_, model) ":" library)) \
        )
#endif

GENERATE_LINKER_OPTION(MD,  "msvcrt.lib" )
GENERATE_LINKER_OPTION(XMD, "msvcrtd.lib")
GENERATE_LINKER_OPTION(MT,  "libcmt.lib" )
GENERATE_LINKER_OPTION(XMT, "libcmtd.lib")

GENERATE_LINKER_OPTION(MD,  "vcruntime.lib"    )
GENERATE_LINKER_OPTION(XMD, "vcruntimed.lib"   )
GENERATE_LINKER_OPTION(MT,  "libvcruntime.lib" )
GENERATE_LINKER_OPTION(XMT, "libvcruntimed.lib")

// Suppress /disallowlib directives to allow use of both the static and DLL
// Universal CRT with the static Visual C++ libraries (we still prohibit mixing
// and matching of any debug libraries with any retail libraries).
#ifndef CRTDLL
    #ifdef _DEBUG
        #undef  EXPAND_LINKER_OPTION_XMD
        #define EXPAND_LINKER_OPTION_XMD(...)
    #else
        #undef  EXPAND_LINKER_OPTION_MD
        #define EXPAND_LINKER_OPTION_MD(...)
    #endif
#endif

GENERATE_LINKER_OPTION(MD,  "ucrt.lib"    )
GENERATE_LINKER_OPTION(XMD, "ucrtd.lib"   )
GENERATE_LINKER_OPTION(MT,  "libucrt.lib" )
GENERATE_LINKER_OPTION(XMT, "libucrtd.lib")
