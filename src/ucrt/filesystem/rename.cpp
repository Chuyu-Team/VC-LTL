//
// rename.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The rename() function, which renames a file.
//
#include <corecrt_internal.h>
#include <io.h>



// See _wrename() for details about the behavior of this function.  (This
// function simply converts the multibyte strings to wide strings and calls
// _wrename().)
extern "C" int __cdecl rename(char const* const old_name, char const* const new_name)
{
    // MinWin does not support the OEM codepage for conversion via the FileAPI
    // APIs, so we unconditionally use the ACP for conversion if the Desktop
    // CRT is not loaded.
    bool const use_oemcp = !__acrt_AreFileApisANSI();
    UINT const code_page = use_oemcp ? CP_OEMCP : CP_ACP;

    // We're going to convert the multibyte strings to wide strings.  Figure out
    // how much space we need and allocate buffers:
    int const old_name_count = MultiByteToWideChar(code_page, 0, old_name, -1, 0, 0);
    if (old_name_count == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return -1;
    }

    int const new_name_count = MultiByteToWideChar(code_page, 0, new_name, -1, 0, 0);
    if (new_name_count == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return -1;
    }

    int const buffer_count = old_name_count + new_name_count;

    __crt_unique_heap_ptr<wchar_t> const buffer(_malloc_crt_t(wchar_t, buffer_count));
    if (buffer.get() == nullptr)
        return -1; // errno will be set by the heap

    wchar_t* const old_name_buffer = buffer.get();
    wchar_t* const new_name_buffer = buffer.get() + old_name_count;

    // Now do the conversion:
    if (MultiByteToWideChar(code_page, 0, old_name, -1, old_name_buffer, old_name_count) == 0 ||
        MultiByteToWideChar(code_page, 0, new_name, -1, new_name_buffer, new_name_count) == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return -1;
    }

    return _wrename(old_name_buffer, new_name_buffer);
}
