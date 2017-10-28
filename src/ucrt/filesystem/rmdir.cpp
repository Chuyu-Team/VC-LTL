//
// rmdir.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _rmdir() function, which removes a directory.
//
#include <corecrt_internal.h>
#include <direct.h>



// Removes the directory specified by the path.  The directory must be empty, it
// must not be the current working directory, and it must not be the root of any
// drive.  Returns 0 on success; returns -1 and sets errno and _doserrno on
// failure.
extern "C" int __cdecl _rmdir(char const* const path)
{
    if (path == nullptr)
        return _wrmdir(nullptr);

    __crt_unique_heap_ptr<wchar_t> wide_path;
    if (!__acrt_copy_path_to_wide_string(path, wide_path.get_address_of()))
        return -1;

    return _wrmdir(wide_path.get());
}
