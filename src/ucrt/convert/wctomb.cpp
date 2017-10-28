//
// wctomb.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Functions to convert a wide character to the equivalent multibyte character,
// according to the LC_CTYPE category of the current locale.  The return value
// (or *return_value, for the _s-suffixed functions) depends on the destination
// and destination_count:
//  * If destination == nullptr && count == 0, number of bytes needed for conversion
//  * If destination == nullptr && count >  0, the state information
//  * If destination != nullptr:  the number of bytes used for the conversion
// The return_value pointer may be null.
//
#include <corecrt_internal.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>
#include <limits.h>



extern "C" int __cdecl _wctomb_s_l(
    int*      const return_value,
    char*     const destination,
    size_t    const destination_count,
    wchar_t   const wchar,
    _locale_t const locale
    )
{
    if (!destination && destination_count > 0)
    {
        // Indicate do not have state-dependent encodings:
        if (return_value != nullptr)
            *return_value = 0;

        return 0;
    }

    if (return_value)
        *return_value = -1;

    // We need to cast destination_count to int, so we make sure we are not
    // going to truncate destination_count:
    _VALIDATE_RETURN_ERRCODE(destination_count <= INT_MAX, EINVAL);

    _LocaleUpdate locale_update(locale);

    if (!locale_update.GetLocaleT()->locinfo->locale_name[LC_CTYPE])
    {
        if (wchar > 255)  // Validate high byte
        {
            if (destination != nullptr && destination_count > 0)
            {
                memset(destination, 0, destination_count);
            }
            
            return errno = EILSEQ;
        }

        if (destination != nullptr)
        {
            _VALIDATE_RETURN_ERRCODE(destination_count > 0, ERANGE);
            *destination = static_cast<char>(wchar);
        }

        if (return_value != nullptr)
        {
            *return_value = 1;
        }

        return 0;
    }
    else
    {
        BOOL default_used{};
        int const size = WideCharToMultiByte(
            locale_update.GetLocaleT()->locinfo->_public._locale_lc_codepage,
            0,
            &wchar,
            1,
            destination,
            static_cast<int>(destination_count),
            nullptr,
            &default_used);

        if (size == 0 || default_used)
        {
            if (size == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (destination && destination_count > 0)
                {
                    memset(destination, 0, destination_count);
                }

                _VALIDATE_RETURN_ERRCODE(("Buffer too small", 0), ERANGE);
            }
            
            return errno = EILSEQ;
        }

        if (return_value)
            *return_value = size;

        return 0;
    }
}

extern "C" errno_t __cdecl wctomb_s (
    int*    const return_value,
    char*   const destination,
    size_t  const destination_count,
    wchar_t const wchar
    )
{
    return _wctomb_s_l(return_value, destination, destination_count, wchar, nullptr);
}

extern "C" int __cdecl _wctomb_l(
    char*     const destination,
    wchar_t   const wchar,
    _locale_t const locale
    )
{
    _LocaleUpdate locale_update(locale);

    int return_value{};
    errno_t const e = _wctomb_s_l(
        &return_value,
        destination,
        locale_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max,
        wchar,
        locale_update.GetLocaleT());

    if (e != 0)
        return -1;

    return return_value;
}

// Disable the OACR error that warns that 'MB_CUR_MAX' doesn't properly constrain buffer 'destination'.
// wctomb() doesn't take a buffer size, so the function's contract is inherently dangerous.
__pragma(warning(push))
__pragma(warning(disable:__WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY)) // 26015

extern "C" int __cdecl wctomb(
    char*   const destination,
    wchar_t const wchar
    )
{
    int return_value{};
    errno_t const e = _wctomb_s_l(&return_value, destination, MB_CUR_MAX, wchar, nullptr);
    if (e != 0)
        return -1;

    return return_value;
}

__pragma(warning(pop))
