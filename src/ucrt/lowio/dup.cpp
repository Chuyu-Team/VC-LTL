//
// dup.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines _dup() and _dup_nolock, which duplicate lowio file handles
//
#include <corecrt_internal_lowio.h>



static int __cdecl dup_nolock(int const fh) throw()
{
    if (!(_osfile(fh) & FOPEN))
        return -1;

    // Allocate a duplicate handle
    int new_fh = _alloc_osfhnd();
    if (new_fh == -1)
    {
        errno = EMFILE;
        _doserrno = 0;
        return -1;
    }

    bool success = false;
    __try
    {
        // Duplicate the file handle:
        intptr_t new_osfhandle;

        BOOL const result = DuplicateHandle(
            GetCurrentProcess(),
            reinterpret_cast<HANDLE>(_get_osfhandle(fh)),
            GetCurrentProcess(),
            &reinterpret_cast<HANDLE&>(new_osfhandle),
            0L,
            TRUE,
            DUPLICATE_SAME_ACCESS);

        if (!result)
        {
            __acrt_errno_map_os_error(GetLastError());
            new_fh = -1;
            __leave;
        }

        // Duplicate the handle state:
        __acrt_lowio_set_os_handle(new_fh, new_osfhandle);
        _osfile(new_fh) = _osfile(fh) & ~FNOINHERIT;
        _textmode(new_fh) = _textmode(fh);
        _tm_unicode(new_fh) = _tm_unicode(fh);
        success = true;
    }
    __finally
    {
        // The handle returned by _alloc_osfhnd is both open and locked.  If we
        // failed to duplicate the handle, we need to abandon the handle by
        // unsetting the open flag.  We always need to unlock the handle:
        if (!success)
        {
#pragma warning(disable:__WARNING_BUFFER_UNDERFLOW) // 26001 new_fh can't be -1 here
            _osfile(new_fh) &= ~FOPEN;
        }

        __acrt_lowio_unlock_fh(new_fh);
    }
    return new_fh;
}



// _dup() duplicates a file handle and returns the duplicate.  If the function
// fails, -1 is returned and errno is set.
extern "C" int __cdecl _dup(int const fh)
{
    _CHECK_FH_CLEAR_OSSERR_RETURN(fh, EBADF, -1);
    _VALIDATE_CLEAR_OSSERR_RETURN((fh >= 0 && (unsigned)fh < (unsigned)_nhandle), EBADF, -1);
    _VALIDATE_CLEAR_OSSERR_RETURN((_osfile(fh) & FOPEN), EBADF, -1);

    __acrt_lowio_lock_fh(fh);
    int result = -1;
    __try
    {
        if (_osfile(fh) & FOPEN)
        {
            result = dup_nolock(fh);
        }
        else
        {
            errno = EBADF;
            _doserrno = 0;
            _ASSERTE(("Invalid file descriptor. File possibly closed by a different thread",0));
            __leave;
        }
    }
    __finally
    {
        __acrt_lowio_unlock_fh(fh);
    }
    return result;
}
