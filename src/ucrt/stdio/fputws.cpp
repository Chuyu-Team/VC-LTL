//
// fputws.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines fputws(), which writes a wide character string to a stream.
//
#include <corecrt_internal_stdio.h>



// Writes the given wide character string to the given stream.  Does not write
// the string's null terminator, and does not append a '\n' to the file.  Returns
// a nonnegative value on success; EOF on failure.  (Note well that we return EOF
// and not WEOF on failure.  This is intentional, and is the correct behavior per
// the C Language Standard).
extern "C" int __cdecl fputws(wchar_t const* const string, FILE* const stream)
{
    _VALIDATE_RETURN(string != nullptr, EINVAL, EOF);
    _VALIDATE_RETURN(stream != nullptr, EINVAL, EOF);

    int return_value = EOF;

    _lock_file(stream);
    __try
    {
        for (wchar_t const* it = string; *it != L'\0'; ++it)
        {
            if (_fputwc_nolock(*it, stream) == WEOF)
                __leave;
        }

        return_value = 0;
    }
    __finally
    {
        _unlock_file(stream);
    }

    return return_value;
}
