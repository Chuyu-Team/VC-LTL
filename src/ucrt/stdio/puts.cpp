//
// puts.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines puts(), which writes a string to stdout.
//
#include <corecrt_internal_stdio.h>



// Writes a string to stdout.  Does not write the string's null terminator, but
// _does_ append a newline to the output.  Return 0 on success; EOF on failure.
extern "C" int __cdecl puts(char const* const string)
{
    _VALIDATE_RETURN(string != nullptr,  EINVAL, EOF);

    FILE* const stream = stdout;
    _VALIDATE_STREAM_ANSI_RETURN(stream, EINVAL, EOF);

    size_t const length = strlen(string);

    return __acrt_lock_stream_and_call(stream, [&]() -> int
    {
        __acrt_stdio_temporary_buffering_guard const buffering(stream);

        size_t const bytes_written = _fwrite_nolock(string, 1, length, stream);

        // If we failed to write the entire string, or if we fail to write the
        // newline, reset the buffering and return failure:
        if (bytes_written != length || _fputc_nolock('\n', stream) == EOF)
        {
            return EOF;
        }

        return 0;
    });
}
