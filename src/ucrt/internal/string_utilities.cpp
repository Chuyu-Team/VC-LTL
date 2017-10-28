//
// string_utilities.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Utilities for converting between string types.
//
#include <corecrt_internal.h>
#include <corecrt_internal_traits.h>



// Converts a narrow string to a wide string.  The wide string is dynamically
// allocated; if this function returns successfully, '*result' will be a pointer
// to the narrow string and the caller is responsible for freeing it.  Returns
// TRUE on success; returns FALSE and sets errno on failure.
extern "C" BOOL __cdecl __acrt_copy_path_to_wide_string(char const* const path, wchar_t** const result)
{
    _VALIDATE_RETURN(path != nullptr,   EINVAL, FALSE);
    _VALIDATE_RETURN(result != nullptr, EINVAL, FALSE);

    bool const use_oem_code_page = !__acrt_AreFileApisANSI();
    int const code_page = use_oem_code_page ? CP_OEMCP : CP_ACP;

    *result = nullptr;

    // Compute the required size of the wide character buffer:
    int const length = MultiByteToWideChar(code_page, 0, path, -1, nullptr, 0);
    if (length == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return FALSE;
    }

    __crt_unique_heap_ptr<wchar_t> buffer(_malloc_crt_t(wchar_t, length));
    if (buffer.get() == nullptr)
    {
        // malloc will set errno
        return FALSE;
    }

    // Do the conversion:
    if (MultiByteToWideChar(code_page, 0, path, -1, buffer.get(), length) == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return FALSE;
    }

    *result = buffer.detach();
    return TRUE;
}



// Converts a wide character string to an ANSI/MBCS string.  The narrow string
// is dynamically allocated; if this function returns successfully, '*result'
// will be a pointer to the narrow string and the caller is responsible for
// freeing it.  Returns TRUE on success; returns FALSE and sets errno on failure..
extern "C" BOOL __cdecl __acrt_copy_to_char(wchar_t const* const string, char** const result)
{
    _VALIDATE_RETURN(string != nullptr, EINVAL, FALSE);
    _VALIDATE_RETURN(result != nullptr, EINVAL, FALSE);

    bool const use_oem_code_page = !__acrt_AreFileApisANSI();
    int const code_page = use_oem_code_page ? CP_OEMCP : CP_ACP;

    *result = nullptr;

    size_t const required_count = __crt_compute_required_transform_buffer_count(code_page, string);
    if (required_count == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return FALSE;
    }

    __crt_unique_heap_ptr<char> buffer(_malloc_crt_t(char, required_count));
    if (!buffer)
    {
        // malloc will set errno
        return FALSE;
    }

    // Do the conversion:
    if (__crt_transform_string(code_page, string, buffer.get(), required_count) == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return FALSE;
    }

    *result = buffer.detach();
    return TRUE;
}
