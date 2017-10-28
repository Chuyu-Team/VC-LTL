//
// chmod.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _chmod() function, which changes file attributes.
//
#include <corecrt_internal.h>
#include <io.h>
#include <sys\stat.h>



// Changes the mode of a file.  The only supported mode bit is _S_IWRITE, which
// controls the user write (read-only) attribute of the file.  Returns zero if
// successful; returns -1 and sets errno and _doserrno on failure.
extern "C" int __cdecl _chmod(char const* const path, int const mode)
{
    if (path == nullptr)
        return _wchmod(nullptr, mode);

    __crt_unique_heap_ptr<wchar_t> wide_path;
    if (!__acrt_copy_path_to_wide_string(path, wide_path.get_address_of()))
        return -1;

    return _wchmod(wide_path.get(), mode);
}
