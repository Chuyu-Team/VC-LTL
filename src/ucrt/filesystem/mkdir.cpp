//
// mkdir.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _mkdir() function, which creates a directory.
//
#include <corecrt_internal.h>
#include <direct.h>



// Creates a directory.  Returns 0 on success; returns -1 and sets errno and
// _doserrno on failure.
extern "C" int __cdecl _mkdir(char const* const path)
{
    if (path == nullptr)
        return _wmkdir(nullptr);

    __crt_unique_heap_ptr<wchar_t> wide_path;
    if (!__acrt_copy_path_to_wide_string(path, wide_path.get_address_of()))
        return -1;

    return _wmkdir(wide_path.get());
}
