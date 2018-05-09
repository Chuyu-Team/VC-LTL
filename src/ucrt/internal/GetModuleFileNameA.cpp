//
// GetModuleFileNameA.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definition of __acrt_GetModuleFileNameA.
//

#include <corecrt_internal_win32_buffer.h>

DWORD __cdecl __acrt_GetModuleFileNameA(
    HMODULE const hModule,
    LPSTR const   lpFilename,
    DWORD const   nSize
    )
{
    size_t const wide_buffer_size = MAX_PATH + 1;
    wchar_t wide_buffer[wide_buffer_size];

    DWORD const amount_copied = GetModuleFileNameW(
        hModule,
        wide_buffer,
        wide_buffer_size
        );

    if (amount_copied == 0) {
        __acrt_errno_map_os_error(GetLastError());
        return 0;
    }

    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        size_t const amount_converted =  mbstowcs(
            wide_buffer,
            lpFilename,
            nSize
            );
    _END_SECURE_CRT_DEPRECATION_DISABLE

    return static_cast<DWORD>(amount_converted);
}
