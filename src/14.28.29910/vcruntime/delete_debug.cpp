//
// delete_debug.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the debug operator delete (both scalar and array forms).
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



#ifdef _DEBUG

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void __CRTDECL operator delete(
        void*       block,
        int const   block_use,
        char const* file_name,
        int const   line_number
        ) noexcept
    {
        UNREFERENCED_PARAMETER(file_name);
        UNREFERENCED_PARAMETER(line_number);

        _free_dbg(block, block_use);
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void __CRTDECL operator delete[](
        void*       block,
        int const   block_use,
        char const* file_name,
        int const   line_number
        ) noexcept
    {
        return operator delete(block, block_use, file_name, line_number);
    }

#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv //

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void __CRTDECL operator delete(
        void*       block,
        int const   block_use,
        char const* file_name,
        int const   line_number
        ) noexcept
    {
        UNREFERENCED_PARAMETER(block_use);
        UNREFERENCED_PARAMETER(file_name);
        UNREFERENCED_PARAMETER(line_number);

        return operator delete(block);
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void __CRTDECL operator delete[](
        void*       block,
        int const   block_use,
        char const* file_name,
        int const   line_number
        ) noexcept
    {
        UNREFERENCED_PARAMETER(block_use);
        UNREFERENCED_PARAMETER(file_name);
        UNREFERENCED_PARAMETER(line_number);

        return operator delete[](block);
    }

#endif
