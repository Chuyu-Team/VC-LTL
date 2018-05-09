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
//  * If destination != nullptr:  the number of bytes used for the conversion
//  * If destination == nullptr && count >  0, the state information
//    (0 for no state dependency, 1 if a state dependent encoding)
//
// Historically Windows has not supported state-dependent encodings for wctomb,
// however UTF-8 can possible require more than one input wchar_t and maintain
// state between calls.
//
// The return_value pointer may be null.
//
#include <corecrt_internal.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>
#include <limits.h>
#include "..\..\winapi_thunks.h"
#include <msvcrt_IAT.h>


#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _wctomb_s_l_downlevel(
    int*      const return_value,
    char*     const destination,
    size_t    const destination_count,
    wchar_t   const wchar,
    _locale_t const locale
    )
{
    // Did the caller request if this is a state dependent encoding?
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

    //_LocaleUpdate locale_update(locale);

    // Check for C-locale behavior, which merely casts it to char (if in range)
    // for ASCII-ish behavior.
    if (!(locale ? locale->locinfo->lc_handle : ___lc_handle_func())[LC_CTYPE])
    {
        // See if the WCHAR is > ASCII-ish range
        if (wchar > 255)  // Validate high byte
        {
            // Too big, can't convert, clear buffer and return error
            if (destination != nullptr && destination_count > 0)
            {
                memset(destination, 0, destination_count);
            }

            return errno = EILSEQ;
        }

        // ASCII-ish, just cast to a (char)
        if (destination != nullptr)
        {
            _VALIDATE_RETURN_ERRCODE(destination_count > 0, ERANGE);
            *destination = static_cast<char>(wchar);
        }

        if (return_value != nullptr)
        {
            // casting one WCHAR emits a single char
            *return_value = 1;
        }

        // We're done
        return 0;
    }
    else
    {
        BOOL default_used{};
        int const size = __acrt_WideCharToMultiByte(
            locale? locale->locinfo->_locale_lc_codepage:___lc_codepage_func(),
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

    // The last thing was an if/else, so we already returned.
}

_LCRT_DEFINE_IAT_SYMBOL(_wctomb_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" errno_t __cdecl wctomb_s_downlevel (
    int*    const return_value,
    char*   const destination,
    size_t  const destination_count,
    wchar_t const wchar
    )
{
    return _wctomb_s_l(return_value, destination, destination_count, wchar, nullptr);
}

_LCRT_DEFINE_IAT_SYMBOL(wctomb_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _wctomb_l_downlevel(
    char*     const destination,
    wchar_t   const wchar,
    _locale_t const locale
    )
{
    //_LocaleUpdate locale_update(locale);
	if (!locale)
		return wctomb(destination, wchar);

    int return_value{};
    errno_t const e = _wctomb_s_l(
        &return_value,
        destination,
        locale->locinfo->_locale_mb_cur_max,
        wchar,
		locale);

    if (e != 0)
        return -1;

    return return_value;
}

_LCRT_DEFINE_IAT_SYMBOL(_wctomb_l_downlevel);

#endif

// Disable the OACR error that warns that 'MB_CUR_MAX' doesn't properly constrain buffer 'destination'.
// wctomb() doesn't take a buffer size, so the function's contract is inherently dangerous.
/*__pragma(warning(push))
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

__pragma(warning(pop))*/
