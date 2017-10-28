//
// access.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _access() and _access_s() functions, which test file accessibility.
//
#include <corecrt_internal.h>
#include <io.h>



// See _waccess_s for information about this function's behavior.
extern "C" errno_t __cdecl _access_s(char const* const path, int const access_mode)
{
    if (path == nullptr)
        return _waccess_s(nullptr, access_mode);

    __crt_unique_heap_ptr<wchar_t> wide_path;
    if (!__acrt_copy_path_to_wide_string(path, wide_path.get_address_of()))
        return -1;

    return _waccess_s(wide_path.get(), access_mode);
}



// The same as _access_s, but transforms all errors into a -1 return value.
extern "C" int __cdecl _access(char const* const path, int const access_mode)
{
    return _access_s(path, access_mode) == 0 ? 0 : -1;
}
