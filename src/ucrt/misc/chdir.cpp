/***
*chdir.c - change directory
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file has the _chdir() function - change current directory.
*
*******************************************************************************/

#include <direct.h>
#include <corecrt_internal_traits.h>
#include <malloc.h>
#include <stdlib.h>

/***
*int _chdir(path) - change current directory
*
*Purpose:
*       Changes the current working directory to that given in path.
*
*Entry:
*       _TSCHAR *path - directory to change to
*
*Exit:
*       returns 0 if successful,
*       returns -1 and sets errno if unsuccessful
*
*Exceptions:
*
*******************************************************************************/
template <typename Character>
_Success_(return == 0)
static int __cdecl set_cwd_environment_variable(_In_z_ Character const* const path) throw()
{
    typedef __crt_char_traits<Character> traits;

    // If the path is a UNC name, no need to update:
    if ((path[0] == '\\' || path[0] == '/') && path[0] == path[1])
        return 0;

#pragma warning(suppress:28931) // unused assignment of variable drive_letter
    Character const drive_letter = static_cast<Character>(toupper(static_cast<char>(path[0])));
    Character const name[] = { '=', drive_letter, ':', '\0' };

    if (traits::set_environment_variable(name, path))
        return 0;

    __acrt_errno_map_os_error(GetLastError());
    return -1;
}



template <typename Character>
_Success_(return == 0)
static int __cdecl common_chdir(_In_z_ Character const* const path) throw()
{
    typedef __crt_char_traits<Character> traits;

    _VALIDATE_CLEAR_OSSERR_RETURN(path != nullptr, EINVAL, -1);

    if (!traits::set_current_directory(path))
    {
        __acrt_errno_map_os_error(GetLastError());
        return -1;
    }

    // If the new current directory path is not a UNC path, we must update the
    // OS environment variable specifying the current full current directory,
    // build the environment variable string, and call SetEnvironmentVariable().
    // We need to do this because the SetCurrentDirectory() API does not update
    // the environment variables, and other functions (fullpath, spawn, etc.)
    // need them to be set.
    //
    // If associated with a 'drive', the current directory should have the
    // form of the example below:
    //
    //     D:\nt\private\mytests
    //
    // So that the environment variable should be of the form:
    //
    //     =D:=D:\nt\private\mytests

    // TODO class-ify this pattern.
    Character local_buffer[_MAX_PATH + 1];
    __crt_unique_heap_ptr<Character> heap_buffer;

    DWORD buffer_count = static_cast<DWORD>(_countof(local_buffer));
    Character* buffer = local_buffer;

    for (;;)
    {
        DWORD const result = traits::get_current_directory(buffer_count, buffer);
        if (result == 0)
        {
            __acrt_errno_map_os_error(GetLastError());
            return -1; // NOTE: The function has been partially succesful.
        }
        else if (result < buffer_count)
        {
#pragma warning(suppress:__WARNING_USING_UNINIT_VAR)                       // 6001 Prefast doesn't understand perfect forwarding.
#pragma warning(suppress:__WARNING_MISSING_ZERO_TERMINATION2)              // 6054 Prefast doesn't understand perfect forwarding.
#pragma warning(suppress:__WARNING_PRECONDITION_NULLTERMINATION_VIOLATION) // 26035 Prefast doesn't understand perfect forwarding.
            return set_cwd_environment_variable(local_buffer);
        }

        heap_buffer = _calloc_crt_t(Character, result);
        buffer = heap_buffer.get();
        if (buffer == nullptr)
        {
            return -1;
        }

        buffer_count = result;
    }
}



extern "C" int __cdecl _chdir(char const* const path)
{
    return common_chdir(path);
}

extern "C" int __cdecl _wchdir(wchar_t const* const path)
{
    return common_chdir(path);
}
