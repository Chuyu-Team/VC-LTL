//
// LoadLibraryExA.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definition of __acrt_LoadLibraryExA.
//

#include <corecrt_internal_win32_buffer.h>

HMODULE __cdecl __acrt_LoadLibraryExA(
    LPCSTR const lpFilename,
    HANDLE const hFile,
    DWORD const  dwFlags
    )
{
    _LocaleUpdate locale_update(nullptr);
    int const code_page = locale_update.GetLocaleT()->locinfo->_public._locale_lc_codepage;

    __crt_internal_win32_buffer<wchar_t> wide_file_name;

    errno_t const cvt1 = __acrt_mbs_to_wcs(
        lpFilename,
        wide_file_name
        );

    if (cvt1 != 0) {
        return nullptr;
    }

    return ::LoadLibraryExW(
        wide_file_name.data(),
        hFile,
        dwFlags
        );
}
