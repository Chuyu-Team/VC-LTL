//
// chsize.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines _chsize() and _chsize_s(), which change the size of a file.
//
#include <corecrt_internal_lowio.h>



// Changes the size of the given file, either extending or truncating it.  The
// file must hvae been opened with write permissions or this will fail.  Returns
// 0 on success; returns an errno error code on failure,
extern "C" errno_t __cdecl _chsize_s(int const fh, __int64 const size)
{
    _CHECK_FH_CLEAR_OSSERR_RETURN_ERRCODE(fh, EBADF);
    _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((fh >= 0 && (unsigned)fh < (unsigned)_nhandle), EBADF);
    _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((_osfile(fh) & FOPEN), EBADF);
    _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((size >= 0), EINVAL);

    return __acrt_lowio_lock_fh_and_call(fh, [&]()
    {
        if (_osfile(fh) & FOPEN)
        {
            return _chsize_nolock(fh, size);
        }
        else
        {
            _ASSERTE(("Invalid file descriptor. File possibly closed by a different thread", 0));
            return errno = EBADF;
        }
    });
}

struct __crt_seek_guard
{

    __crt_seek_guard(int const fh, __int64 const size)
        : place(_lseeki64_nolock(fh, 0, SEEK_CUR)),
        end(_lseeki64_nolock(fh, 0, SEEK_END)),
        extend(size - end),
        fhh(fh)
    {
    }

    ~__crt_seek_guard()
    {
        _lseeki64_nolock(fhh, place, SEEK_SET);
    }

    __crt_seek_guard(__crt_seek_guard const &) = delete;
    __crt_seek_guard& operator=(__crt_seek_guard const &) = delete;

    __int64 place;
    __int64 end;
    __int64 extend;
    int fhh;
};

extern "C" errno_t __cdecl _chsize_nolock(int const fh, __int64 const size)
{
    // Get current file position and seek to end
    __crt_seek_guard seek_guard(fh, size);

    if (seek_guard.place == -1 || seek_guard.end == -1) {
        return errno;
    }

    // Grow or shrink the file as necessary:
    if (seek_guard.extend > 0)
    {
        // Extend the file by filling the new area with zeroes:
        __crt_unique_heap_ptr<char> const zero_buffer(_calloc_crt_t(char, _INTERNAL_BUFSIZ));
        if (!zero_buffer)
        {
            errno = ENOMEM;
            return errno;
        }

        int const old_mode = _setmode_nolock(fh, _O_BINARY);

        do
        {
            int const bytes_to_write = seek_guard.extend >= static_cast<__int64>(_INTERNAL_BUFSIZ)
                ? _INTERNAL_BUFSIZ
                : static_cast<int>(seek_guard.extend);

            int const bytes_written = _write_nolock(fh, zero_buffer.get(), bytes_to_write);
            if (bytes_written == -1)
            {
                // Error on write:
                if (_doserrno == ERROR_ACCESS_DENIED)
                    errno = EACCES;

                return errno;
            }

            seek_guard.extend -= bytes_written;
        } while (seek_guard.extend > 0);

#pragma warning(suppress:6031) // return value ignored
        _setmode_nolock(fh, old_mode);
    }
    else if (seek_guard.extend < 0)
    {
        // Shorten the file by truncating it:
        __int64 const new_end = _lseeki64_nolock(fh, size, SEEK_SET);
        if (new_end == -1)
        {
            return errno;
        }

        if (!SetEndOfFile(reinterpret_cast<HANDLE>(_get_osfhandle(fh))))
        {
            errno = EACCES;
            _doserrno = GetLastError();
            return errno;
        }
    }

    return 0;
}



// Changes the size of the given file, either extending or truncating it.  The
// file must hvae been opened with write permissions or this will fail.  Returns
// 0 on success; returns -1 and sets errno on failure.
extern "C" int __cdecl _chsize(int const fh, long const size)
{
    return _chsize_s(fh, size) == 0 ? 0 : -1;
}
