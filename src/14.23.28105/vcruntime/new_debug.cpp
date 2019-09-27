//
// new_debug.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the debug operator new (both scalar and array forms).
//
// The debug operator new and operator new[] are implemented in both the debug
// and retail CRT libraries.  In the retail CRT, they simply call the ordinary
// operator new.  These operators are only declared in the public header files
// when building with the debug CRT.  Implementing these operators in the CRT
// libraries makes it easier to override them by linking to nothrownew.obj.
//
#include <crtdbg.h>
#include <limits.h>
#include <malloc.h>
#include <vcruntime_new.h>
#include <vcstartup_internal.h>



#ifdef _DEBUG

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void* __CRTDECL operator new(
        size_t const size,
        int const    block_use,
        char const*  file_name,
        int const    line_number
        )
    {
        for (;;)
        {
            if (void* const block = _malloc_dbg(size, block_use, file_name, line_number))
            {
                return block;
            }

            if (_callnewh(size) == 0)
            {
                if (size == SIZE_MAX)
                {
                    __scrt_throw_std_bad_array_new_length();
                }
                else
                {
                    __scrt_throw_std_bad_alloc();
                }
            }
        }
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void* __CRTDECL operator new[](
        size_t const size,
        int const    block_use,
        char const*  file_name,
        int const    line_number
        )
    {
        return operator new(size, block_use, file_name, line_number);
    }

#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv //

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void* __CRTDECL operator new(
        size_t const size,
        int const    block_use,
        char const*  file_name,
        int const    line_number
        )
    {
        UNREFERENCED_PARAMETER(block_use);
        UNREFERENCED_PARAMETER(file_name);
        UNREFERENCED_PARAMETER(line_number);

        return operator new(size);
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void* __CRTDECL operator new[](
        size_t const size,
        int const    block_use,
        char const*  file_name,
        int const    line_number
        )
    {
        UNREFERENCED_PARAMETER(block_use);
        UNREFERENCED_PARAMETER(file_name);
        UNREFERENCED_PARAMETER(line_number);

        return operator new[](size);
    }

#endif // !_DEBUG
