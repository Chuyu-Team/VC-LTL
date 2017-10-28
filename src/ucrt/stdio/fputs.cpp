//
// fputs.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines fputs(), which writes a string to a stream.
//
#include <corecrt_internal_stdio.h>



// Writes the given string to the given stream.  Does not write the string's null
// terminator, and does not append a '\n' to the file.  Returns a nonnegative
// value on success; EOF on failure.
extern "C" int __cdecl fputs(char const* const string, FILE* const stream)
{
    _VALIDATE_RETURN(string != nullptr,  EINVAL, EOF);
    _VALIDATE_RETURN(stream != nullptr,  EINVAL, EOF);
    _VALIDATE_STREAM_ANSI_RETURN(stream, EINVAL, EOF);

    size_t const length = strlen(string);

    return __acrt_lock_stream_and_call(stream, [&]() -> int
    {
        __acrt_stdio_temporary_buffering_guard const buffering(stream);

        size_t const bytes_written = _fwrite_nolock(string, 1, length, stream);
        if (bytes_written == length)
        {
            return 0;
        }

        return EOF;
    });
}
