//
// fputc.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines the fputc() family of functions, which write a character to a stream.
//
#include <corecrt_internal_stdio.h>



extern "C" int __cdecl _fputc_nolock(int const c, FILE* const public_stream)
{
    __crt_stdio_stream const stream(public_stream);

    --stream->_cnt;

    // If there is no room for the character in the buffer, flush the buffer and
    // write the character:
    if (stream->_cnt < 0)
        return __acrt_stdio_flush_and_write_narrow_nolock(c, stream.public_stream());

    // Otherwise, there is sufficient room, so we can write directly to the buffer:
    char const char_value = static_cast<char>(c);
    *stream->_ptr++ = char_value;
    return char_value & 0xff;
}

extern "C" int __cdecl _putc_nolock(int const c, FILE* const stream)
{
    return _fputc_nolock(c, stream);
}



// Writes a character to a stream.  Returns the character on success; returns
// EOF on failure.
extern "C" int __cdecl fputc(int const c, FILE* const stream)
{
    _VALIDATE_RETURN(stream != nullptr, EINVAL, EOF);

    int return_value = 0;

    _lock_file(stream);
    __try
    {
        _VALIDATE_STREAM_ANSI_RETURN(stream, EINVAL, EOF);

        return_value = _fputc_nolock(c, stream);
    }
    __finally
    {
        _unlock_file(stream);
    }

    return return_value;
}



// Writes a character to a stream.  See fputc() for details.
extern "C" int __cdecl putc(int const c, FILE* const stream)
{
    return fputc(c, stream);
}



// Writes a character to stdout.  See fputc() for details.
extern "C" int __cdecl _fputchar(int const c)
{
    return fputc(c, stdout);
}



// Writes a character to stdout.  See fputc() for details.
extern "C" int __cdecl putchar(int const c)
{
    return _fputchar(c);
}
