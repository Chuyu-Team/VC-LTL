/***
*getcwd.c - get current working directory
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*       contains functions _getcwd, _getdcwd and _getcdrv for getting the
*       current working directory.  getcwd gets the c.w.d. for the default disk
*       drive, whereas _getdcwd allows one to get the c.w.d. for whatever disk
*       drive is specified. _getcdrv gets the current drive.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <direct.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>



// Tests whether the specified drive number is valid.  Returns zero if the drive
// does not exist; returns one if the drive exists.  Pass zero as an argument to
// check the default drive.
static int __cdecl is_valid_drive(unsigned const drive_number) throw()
{
    if (drive_number > 26)
    {
        _doserrno = ERROR_INVALID_DRIVE;
        _VALIDATE_RETURN(("Invalid Drive Index" ,0), EACCES, 0);
    }

    if (drive_number == 0)
        return 1;

#pragma warning(suppress:__WARNING_UNUSED_SCALAR_ASSIGNMENT) // 28931
    wchar_t const drive_letter   = static_cast<wchar_t>(L'A' + drive_number - 1);
    wchar_t const drive_string[] = { drive_letter, L':', L'\\', L'\0' };

    UINT const drive_type = GetDriveTypeW(drive_string);
    if (drive_type == DRIVE_UNKNOWN || drive_type == DRIVE_NO_ROOT_DIR)
        return 0;

    return 1;
}



/***
*_TSCHAR *_getcwd(pnbuf, maxlen) - get current working directory of default drive
*
*Purpose:
*       _getcwd gets the current working directory for the user,
*       placing it in the buffer pointed to by pnbuf.  It returns
*       the length of the string put in the buffer.  If the length
*       of the string exceeds the length of the buffer, maxlen,
*       then nullptr is returned.  If pnbuf = nullptr, maxlen is ignored.
*       An entry point "_getdcwd()" is defined with takes the above
*       parameters, plus a drive number.  "_getcwd()" is implemented
*       as a call to "_getcwd()" with the default drive (0).
*
*       If pnbuf = nullptr, maxlen is ignored, and a buffer is automatically
*       allocated using malloc() -- a pointer to which is returned by
*       _getcwd().
*
*       side effects: no global data is used or affected
*
*Entry:
*       _TSCHAR *pnbuf = pointer to a buffer maintained by the user;
*       int maxlen = length of the buffer pointed to by pnbuf;
*
*Exit:
*       Returns pointer to the buffer containing the c.w.d. name
*       (same as pnbuf if non-nullptr; otherwise, malloc is
*       used to allocate a buffer)
*
*Exceptions:
*
*******************************************************************************/

/***
*_TSCHAR *_getdcwd(drive, pnbuf, maxlen) - get c.w.d. for given drive
*
*Purpose:
*       _getdcwd gets the current working directory for the user,
*       placing it in the buffer pointed to by pnbuf.  It returns
*       the length of the string put in the buffer.  If the length
*       of the string exceeds the length of the buffer, maxlen,
*       then nullptr is returned.  If pnbuf = nullptr, maxlen is ignored,
*       and a buffer is automatically allocated using malloc() --
*       a pointer to which is returned by _getdcwd().
*
*       side effects: no global data is used or affected
*
*Entry:
*       int drive   - number of the drive being inquired about
*                     0 = default, 1 = 'a:', 2 = 'b:', etc.
*       _TSCHAR *pnbuf - pointer to a buffer maintained by the user;
*       int maxlen  - length of the buffer pointed to by pnbuf;
*
*Exit:
*       Returns pointer to the buffer containing the c.w.d. name
*       (same as pnbuf if non-nullptr; otherwise, malloc is
*       used to allocate a buffer)
*
*Exceptions:
*
*******************************************************************************/
__success(return != 0 && return < max_count)
static unsigned __cdecl get_full_path_name(
    _Out_writes_opt_z_(max_count) char* const   buffer,
    _In_z_ char const*                  const   path,
    unsigned                            const   max_count
    ) throw()
{
    return GetFullPathNameA(path, max_count, buffer, nullptr);
}

__success(return != 0 && return < max_count)
static unsigned __cdecl get_full_path_name(
    _Out_writes_opt_z_(max_count) wchar_t*  const   buffer,
    _In_z_ wchar_t const*                   const   path,
    unsigned                                const   max_count
    ) throw()
{
    return GetFullPathNameW(path, max_count, buffer, nullptr);
}

template <typename Character>
_Success_(return != 0)
_Ret_z_
static Character* __cdecl common_getdcwd_user_buffer(
    _In_z_ Character*                       const   drive_string,
    _Out_writes_z_(buffer_count) Character* const   buffer,
    unsigned                                const   buffer_count
    ) throw()
{
    _VALIDATE_RETURN(buffer_count > 0, EINVAL, nullptr);

    unsigned const path_count = get_full_path_name(buffer, drive_string, buffer_count);
    if (path_count >= buffer_count)
    {
        errno = ERANGE;
        return nullptr;
    }

    if (path_count == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return nullptr;
    }

    return buffer;
}

template <typename Character>
_Success_(return != 0)
_Ret_z_
static Character* __cdecl common_getdcwd_dynamic_buffer(
    _In_z_ Character*      const drive_string,
    unsigned               const max_count,
    int                    const block_use,
    _In_opt_z_ char const* const file_name,
    int                    const line_number
    ) throw()
{
    // These are referenced only in the Debug CRT build
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    unsigned const required_count = get_full_path_name(nullptr, drive_string, 0);
    if (required_count == 0)
    {
        __acrt_errno_map_os_error(GetLastError());
        return nullptr;
    }

    unsigned const actual_count = __max(max_count, required_count);

    __crt_unique_heap_ptr<Character, __crt_public_free_policy> buffer(
        static_cast<Character*>(_calloc_dbg(
            actual_count,
            sizeof(Character),
            block_use,
            file_name,
            line_number)));

    if (buffer.get() == nullptr)
    {
        _doserrno = ERROR_NOT_ENOUGH_MEMORY;
        errno = ENOMEM;
        return nullptr;
    }

    Character* const result = common_getdcwd_user_buffer(drive_string, buffer.get(), actual_count);
    if (result == nullptr)
        return nullptr;

    return buffer.detach();
}


template <typename Character>
_Success_(return != 0)
_Ret_z_
static Character* __cdecl common_getdcwd(
    int                                            drive_number,
    _Out_writes_opt_z_(max_count) Character* const user_buffer,
    int                                      const max_count,
    int                                      const block_use,
    _In_opt_z_ char const*                   const file_name,
    int                                      const line_number
    ) throw()
{
    _VALIDATE_RETURN(max_count >= 0, EINVAL, nullptr);

    if (drive_number != 0)
    {
        // A drive other than the default drive was requested; make sure it
        // is a valid drive number:
        if (!is_valid_drive(drive_number))
        {
            _doserrno = ERROR_INVALID_DRIVE;
            _VALIDATE_RETURN(("Invalid Drive", 0), EACCES, nullptr);
        }
    }
    else
    {
        // Otherwise, get the drive number of the default drive:
        drive_number = _getdrive();
    }

    Character drive_string[4];
    if (drive_number != 0)
    {
        drive_string[0] = static_cast<Character>('A' - 1 + drive_number);
        drive_string[1] = ':';
        drive_string[2] = '.';
        drive_string[3] = '\0';
    }
    else
    {
        drive_string[0] = '.';
        drive_string[1] = '\0';
    }

    if (user_buffer != nullptr)
    {
        _VALIDATE_RETURN(max_count > 0, EINVAL, nullptr);
        user_buffer[0] = '\0';
        return common_getdcwd_user_buffer(drive_string, user_buffer, static_cast<unsigned>(max_count));
    }

    return common_getdcwd_dynamic_buffer(drive_string, static_cast<unsigned>(max_count), block_use, file_name, line_number);
}



extern "C" char* __cdecl _getcwd(
    char* const user_buffer,
    int   const max_length
    )
{
    return common_getdcwd(0, user_buffer, max_length, _NORMAL_BLOCK, nullptr, 0);
}

extern "C" wchar_t* __cdecl _wgetcwd(
    wchar_t* const user_buffer,
    int      const max_length
    )
{
    return common_getdcwd(0, user_buffer, max_length, _NORMAL_BLOCK, nullptr, 0);
}

extern "C" char* __cdecl _getdcwd(
    int   const drive_number,
    char* const user_buffer,
    int   const max_length
    )
{
    return common_getdcwd(drive_number, user_buffer, max_length, _NORMAL_BLOCK, nullptr, 0);
}

extern "C" wchar_t* __cdecl _wgetdcwd(
    int      const drive_number,
    wchar_t* const user_buffer,
    int      const max_length
    )
{
    return common_getdcwd(drive_number, user_buffer, max_length, _NORMAL_BLOCK, nullptr, 0);
}

#ifdef _DEBUG

#undef _getcwd_dbg
#undef _getdcwd_dbg

extern "C" char* __cdecl _getcwd_dbg(
    char*       const user_buffer,
    int         const max_length,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    return common_getdcwd(0, user_buffer, max_length, block_use, file_name, line_number);
}

extern "C" wchar_t* __cdecl _wgetcwd_dbg(
    wchar_t*       const user_buffer,
    int            const max_length,
    int            const block_use,
    char const*    const file_name,
    int            const line_number
    )
{
    return common_getdcwd(0, user_buffer, max_length, block_use, file_name, line_number);
}

extern "C" char* __cdecl _getdcwd_dbg(
    int         const drive_number,
    char*       const user_buffer,
    int         const max_length,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    return common_getdcwd(drive_number, user_buffer, max_length, block_use, file_name, line_number);
}

extern "C" wchar_t* __cdecl _wgetdcwd_dbg(
    int         const drive_number,
    wchar_t*    const user_buffer,
    int         const max_length,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    return common_getdcwd(drive_number, user_buffer, max_length, block_use, file_name, line_number);
}

#endif // _DEBUG
