//
// gettemppath.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The __acrt_GetTempPathA() function, which calls GetTempPathW and converts the string to multibyte.
//
#include <corecrt_internal.h>
#include <io.h>



// This function simply calls GetTempPathW() and converts the wide string to multibyte.
extern "C" DWORD __cdecl __acrt_GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer)
{
    wchar_t wide_buffer[_MAX_PATH + 1] = {};

    DWORD const get_temp_path_result = GetTempPathW(_countof(wide_buffer), wide_buffer);
    if (get_temp_path_result == 0)
    {
        return 0;
    }

    bool const use_oem_code_page = !__acrt_AreFileApisANSI();
    int const code_page = use_oem_code_page ? CP_OEMCP : CP_ACP;
#pragma warning(suppress:__WARNING_W2A_BEST_FIT) // 38021 Prefast recommends WC_NO_BEST_FIT_CHARS.
    int const wctmb_result = WideCharToMultiByte(code_page, 0, wide_buffer, -1, lpBuffer, nBufferLength, nullptr, nullptr);
    if (wctmb_result == 0)
    {
        return 0;
    }

    // The return value of WideCharToMultiByte includes the null terminator; the
    // return value of GetTempPathA does not.
    return wctmb_result - 1;
}
