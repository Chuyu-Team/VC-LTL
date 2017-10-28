//
// _flsbuf.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Functions that flush a stdio stream buffer and write a character.
//
#include <corecrt_internal_stdio.h>



template <typename Character>
static bool __cdecl write_buffer_nolock(
    Character          const c,
    __crt_stdio_stream const stream
    ) throw()
{
    typedef __acrt_stdio_char_traits<Character> stdio_traits;

    int const fh = _fileno(stream.public_stream());

    // If the stream is buffered, write the buffer if there are characters to
    // be written, then push the character 'c' into the buffer:
    if (stream.has_big_buffer())
    {
        _ASSERTE(("inconsistent IOB fields", stream->_ptr - stream->_base >= 0));

        int const chars_to_write = static_cast<int>(stream->_ptr - stream->_base);
        stream->_ptr = stream->_base   + sizeof(Character);
        stream->_cnt = stream->_bufsiz - static_cast<int>(sizeof(Character));

        int chars_written = 0;
        if (chars_to_write > 0)
        {
            chars_written = _write(fh, stream->_base, chars_to_write);
        }
        else
        {
            if (_osfile_safe(fh) & FAPPEND)
            {
                if (_lseeki64(fh, 0, SEEK_END) == -1)
                {
                    stream.set_flags(_IOERROR);
                    return stdio_traits::eof;
                }
            }
        }

        *reinterpret_cast<Character*>(stream->_base) = c;
        return chars_written == chars_to_write;
    }
    // Otherwise, perform a single character write (if we get here, either
    // _IONBF is set or there is no buffering):
    else
    {
        return _write(fh, reinterpret_cast<char const*>(&c), sizeof(c)) == sizeof(Character);
    }
}



// Flushes the buffer of the given stream and writes the character.  If the stream
// does not have a buffer, one is obtained for it.  The character 'c' is written
// into the buffer after the buffer is flushed.  This function is only intended
// to be called from within the library.  Returns (W)EOF on failure; otherwise
// returns the character 'c' that was written to the file (note that 'c' may be
// truncated if the int value cannot be represented by the Character type).
template <typename Character>
static int __cdecl common_flush_and_write_nolock(
    int                const c,
    __crt_stdio_stream const stream
    ) throw()
{
    typedef __acrt_stdio_char_traits<Character> stdio_traits;

    unsigned const character_mask = (1 << (CHAR_BIT * sizeof(Character))) - 1;

    _ASSERTE(stream.valid());

    int const fh = _fileno(stream.public_stream());

    if (!stream.has_any_of(_IOWRITE | _IOUPDATE))
    {
        errno = EBADF;
        stream.set_flags(_IOERROR);
        return stdio_traits::eof;
    }
    else if (stream.is_string_backed())
    {
        errno = ERANGE;
        stream.set_flags(_IOERROR);
        return stdio_traits::eof;
    }

    // Check that _IOREAD is not set or, if it is, then so is _IOEOF. Note
    // that _IOREAD and IOEOF both being set implies switching from read to
    // write at end-of-file, which is allowed by ANSI. Note that resetting
    // the _cnt and _ptr fields amounts to doing an fflush() on the stream
    // in this case. Note also that the _cnt field has to be reset to 0 for
    // the error path as well (i.e., _IOREAD set but _IOEOF not set) as
    // well as the non-error path.

    if (stream.has_any_of(_IOREAD))
    {
        stream->_cnt = 0;
        if (stream.has_any_of(_IOEOF))
        {
            stream->_ptr = stream->_base;
            stream.unset_flags(_IOREAD);
        }
        else
        {
            stream.set_flags(_IOERROR);
            return stdio_traits::eof;
        }
    }

    stream.set_flags(_IOWRITE);
    stream.unset_flags(_IOEOF);

    stream->_cnt = 0;

    // Get a buffer for this stream, if one is necessary:
    if (!stream.has_any_buffer())
    {
        // Do not get a buffer if the stream is stdout or stderr and the stream
        // is not a TTY.  If one of these streams is a TTY, we do not set up a
        // single character buffer; this is so tha tlater temporary buffering
        // will not be thwarted by the _IONBF flag being set.  (See _stbuf() and
        // _ftbuf() for more information on stdout and stderr buffering.)
        if ((stream.public_stream() != stdout && stream.public_stream() != stderr) || !_isatty(fh))
            __acrt_stdio_allocate_buffer_nolock(stream.public_stream());
    }

    int char_count;
    int written;

    written = char_count = 0;

    // Write the character; return (W)EOF if it fails:
    if (!write_buffer_nolock(static_cast<Character>(c & character_mask), stream))
    {
        stream.set_flags(_IOERROR);
        return stdio_traits::eof;
    }

    return c & character_mask;
}



extern "C" int __cdecl __acrt_stdio_flush_and_write_narrow_nolock(
    int   const c,
    FILE* const stream
    )
{
    return common_flush_and_write_nolock<char>(c, __crt_stdio_stream(stream));
}



extern "C" int __cdecl __acrt_stdio_flush_and_write_wide_nolock(
    int   const c,
    FILE* const stream
    )
{
    return common_flush_and_write_nolock<wchar_t>(c, __crt_stdio_stream(stream));
}
