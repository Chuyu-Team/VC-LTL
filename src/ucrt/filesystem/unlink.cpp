//
// unlink.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The remove() and unlink() functions, which remove (delete) files.
//
#include <corecrt_internal.h>
#include <stdio.h>



// Deletes the specified file.  Returns zero if successful; returns -1 and sets
// errno and _doserrno on failure.
extern "C" int __cdecl remove(char const* const path)
{
    if (path == nullptr)
        return _wremove(nullptr);

    __crt_unique_heap_ptr<wchar_t> wide_path;
    if (!__acrt_copy_path_to_wide_string(path, wide_path.get_address_of()))
        return -1;

    return _wremove(wide_path.get());
}



extern "C" int __cdecl _unlink(char const* const path)
{
    return remove(path);
}
