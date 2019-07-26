//
// throw_bad_alloc.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines functions that throw std::bad_alloc and std::bad_array_new_length.
// We cannot include the <vcruntime_exception.h> header in the source files
// that define the various operator new functions because that header defines
// inline functions that may be referenced by user code.  The operators
// themselves are replaceable by the user.
//
#include <vcruntime_exception.h>
#include <vcstartup_internal.h>



#if defined MRTDLL && defined _M_CEE

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void __CRTDECL __scrt_throw_std_bad_alloc()
    {
        TerminateProcess(GetCurrentProcess(), 3);
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void __CRTDECL __scrt_throw_std_bad_array_new_length()
    {
        TerminateProcess(GetCurrentProcess(), 3);
    }

#else // ^^^ Managed ^^^ // vvv Native vvv //

    __declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_alloc()
    {
        throw std::bad_alloc{};
    }

    __declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_array_new_length()
    {
        throw std::bad_array_new_length{};
    }

#endif
