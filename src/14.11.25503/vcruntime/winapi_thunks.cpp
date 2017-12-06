//
// winapi_thunks.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of wrappers for Windows API functions that cannot be called
// by the statically-linked CRT code because they are not part of the MSDK.
//
#include <vcruntime_internal.h>

#if defined _CRT_APP && !defined _DEBUG
    #define USE_ONLY_MSDK_APIS
#endif

extern "C" DWORD __cdecl __vcrt_GetModuleFileNameW(
    HMODULE const module_handle,
    LPWSTR  const buffer,
    DWORD   const buffer_count
    )
{
    #ifdef USE_ONLY_MSDK_APIS

    UNREFERENCED_PARAMETER(module_handle);
    UNREFERENCED_PARAMETER(buffer);
    UNREFERENCED_PARAMETER(buffer_count);

    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;

    #else // ^^^ MSDK ^^^ // vvv !MSDK vvv //

    return GetModuleFileNameW(module_handle, buffer, buffer_count);

    #endif // !MSDK
}

extern "C" HMODULE __cdecl __vcrt_GetModuleHandleW(
    LPCWSTR const file_name
    )
{
    #ifdef USE_ONLY_MSDK_APIS

    UNREFERENCED_PARAMETER(file_name);

    SetLastError(ERROR_NOT_SUPPORTED);
    return nullptr;

    #else // ^^^ MSDK ^^^ // vvv !MSDK vvv //

    return GetModuleHandleW(file_name);

    #endif // !MSDK
}

extern "C" HMODULE __cdecl __vcrt_LoadLibraryExW(
    LPCWSTR const file_name,
    HANDLE  const file_handle,
    DWORD   const flags
    )
{
    #ifdef USE_ONLY_MSDK_APIS

    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(file_handle);
    UNREFERENCED_PARAMETER(flags);

    SetLastError(ERROR_NOT_SUPPORTED);
    return nullptr;

    #else // ^^^ MSDK ^^^ // vvv !MSDK vvv //

    return LoadLibraryExW(file_name, file_handle, flags);

    #endif // !MSDK
}
