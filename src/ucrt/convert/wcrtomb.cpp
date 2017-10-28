/***
*wcrtomb.c - Convert wide character to multibyte character, with locale.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a wide character into the equivalent multibyte character.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <corecrt_internal_securecrt.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <locale.h>

/***
*errno_t _wcrtomb_s_l() - Helper function to convert wide character to multibyte character.
*
*Purpose:
*       Convert a wide character into the equivalent multi-byte character,
*       according to the specified LC_CTYPE category, or the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       int *return_value      = the number of chars written (-1 in error case)
*       char *destination           = pointer to multibyte character
*       size_t destination_count  = size of the destinarion buffer
*       wchar_t wchar       = source wide character
*       mbstate_t *state      = pointer to state (not used)
*       _locale_t locale  = locale info
*
*Exit:
*       Returns:
*       Value of errno if errors, 0 otherwise. *return_value is set to -1 in error case.
*
*Exceptions:
*
*******************************************************************************/

_Success_(return == 0)
static errno_t __cdecl _wcrtomb_s_l(
                                            int*        const   return_value,
    __out_bcount_z_opt(destination_count)   char*       const   destination,
                                            size_t      const   destination_count,
                                            wchar_t     const   wchar,
                                            mbstate_t*  const   state,
                                            _locale_t   const   locale
    )
{
    _ASSERTE(destination != nullptr && destination_count > 0);

    _LocaleUpdate locale_update(locale);

    _ASSERTE(
        locale_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max == 1 ||
        locale_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max == 2);

    if (state)
        state->_Wchar = 0;

    if (!locale_update.GetLocaleT()->locinfo->locale_name[LC_CTYPE])
    {
        if (wchar > 255) // Validate high byte
        {
            if (return_value)
                *return_value = -1;

            return errno = EILSEQ;
        }

        *destination = static_cast<char>(wchar);
        if (return_value)
            *return_value = 1;

        return 0;
    }

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
        if (return_value)
            *return_value = -1;

        return errno = EILSEQ;
    }

    if (return_value)
        *return_value = size;

    return 0;
}

/***
*errno_t wcrtomb_s(retValue, destination, destination_count, wchar, state) - translate wchar_t to multibyte, restartably
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

extern "C" errno_t __cdecl wcrtomb_s(
    size_t*    const return_value,
    char*      const destination,
    size_t     const destination_count,
    wchar_t    const wchar,
    mbstate_t* const state
    )
{
    // Note that we do not force destination_count > 0 in the destination !=
    // nullptr case because we do not need to add a null terminator, due to
    // the fact that the destination will receive a character and not a string.
    _VALIDATE_RETURN_ERRCODE((destination == nullptr && destination_count == 0) || (destination != nullptr), EINVAL);

    errno_t e = 0;
    int     int_return_value = -1;
    if (destination == nullptr)
    {
        char buf[MB_LEN_MAX];
        e = _wcrtomb_s_l(&int_return_value, buf, MB_LEN_MAX, wchar, state, nullptr);
    }
    else
    {
        e = _wcrtomb_s_l(&int_return_value, destination, destination_count, wchar, state, nullptr);
    }

    if (return_value != nullptr)
        *return_value = static_cast<size_t>(int_return_value);

    return e;
}

extern "C" size_t __cdecl wcrtomb(
    char*      const destination,
    wchar_t    const wchar,
    mbstate_t* const state
    )
{
    size_t return_value = static_cast<size_t>(-1);
    wcrtomb_s(&return_value, destination, (destination == nullptr ? 0 : MB_LEN_MAX), wchar, state);
    return return_value;
}

/***
*errno_t wcsrtombs_s(retValue, destination, destination_count, pwcs, n, state) - translate wide char string to multibyte
*       string
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

/* Helper shared by secure and non-secure functions. */

extern "C" static size_t __cdecl internal_wcsrtombs(
    _Pre_maybenull_ _Post_z_    char*                   destination,
    _Inout_ _Deref_prepost_z_   wchar_t const** const   source,
    _In_                        size_t                  n,
    _Out_opt_                   mbstate_t*      const   state
    ) throw()
{
    /* validation section */
    _VALIDATE_RETURN(source != nullptr, EINVAL, (size_t)-1);

    char buf[MB_LEN_MAX];
    int i = 0;
    size_t nc = 0;
    wchar_t const* wcs = *source;
    _LocaleUpdate locale_update(nullptr);

    if (!destination)
    {
        for (; ; nc += i, ++wcs)
        {
            /* translate but don't store */
            _wcrtomb_s_l(&i, buf, MB_LEN_MAX, *wcs, state, locale_update.GetLocaleT());
            if (i <= 0)
            {
                return static_cast<size_t>(-1);
            }
            else if (buf[i - 1] == '\0')
            {
                return nc + i - 1;
            }
        }
    }

    for (; 0 < n; nc += i, ++wcs, destination += i, n -= i)
    {
        /* translate and store */
        char *t = nullptr;

        if (n < (size_t)locale_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max)
        {
            t = buf;
        }
        else
        {
            t = destination;
        }

        _wcrtomb_s_l(&i, t, MB_LEN_MAX, *wcs, state, locale_update.GetLocaleT());
        if (i <= 0)
        {
            /* encountered invalid sequence */
            nc = (size_t)-1;
            break;
        }

        if (destination == t)
        {
            /* do nothing */
        }
        else if (n < static_cast<size_t>(i))
        {
            break;  // Won't all fit
        }
        else
        {
            memcpy_s(destination, n, buf, i);
        }

        if (destination[i - 1] == '\0')
        {
            // Encountered terminating null
            *source = 0;
            return nc + i - 1;
        }
    }

    *source = wcs;
    return nc;
}

extern "C" size_t __cdecl wcsrtombs(
    char*           const destination,
    wchar_t const** const source,
    size_t          const n,
    mbstate_t*      const state
    )
{
    return internal_wcsrtombs(destination, source, n, state);
}

/***
*errno_t wcstombs_s() - Convert wide char string to multibyte char string.
*
*Purpose:
*       Convert a wide char string into the equivalent multibyte char string,
*       according to the LC_CTYPE category of the current locale.
*
*       The destination string is always null terminated.
*
*Entry:
*       size_t *return_value = Number of bytes modified including the terminating nullptr
*                           This pointer can be nullptr.
*       char *destination = pointer to destination multibyte char string
*       size_t destination_count = size of the destination buffer
*       const wchar_t *source = pointer to source wide character string
*       size_t n = maximum number of bytes to store in s (not including the terminating nullptr)
*       mbstate_t *state = pointer to state
*
*Exit:
*       The error code.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

extern "C" errno_t __cdecl wcsrtombs_s(
    size_t*         const return_value,
    char*           const destination,
    size_t          const destination_count,
    wchar_t const** const source,
    size_t          const n,
    mbstate_t*      const state
    )
{
    if (return_value != nullptr)
        *return_value = static_cast<size_t>(-1);

    _VALIDATE_RETURN_ERRCODE(
        (destination == nullptr && destination_count == 0) ||
        (destination != nullptr && destination_count >  0),
    EINVAL);

    if (destination != nullptr)
    {
        _RESET_STRING(destination, destination_count);
    }

    _VALIDATE_RETURN_ERRCODE(source != nullptr, EINVAL);

    size_t retsize = internal_wcsrtombs(destination, source, (n > destination_count ? destination_count : n), state);
    if (retsize == static_cast<size_t>(-1))
    {
        if (destination != nullptr)
        {
            _RESET_STRING(destination, destination_count);
        }

        errno_t const err = errno;
        _Analysis_assume_(err == EILSEQ);
        return err;
    }

    ++retsize; // Account for the null terminator

    if (destination != nullptr)
    {
        // Return error if the string does not fit:
        if (retsize > destination_count)
        {
            _RESET_STRING(destination, destination_count);
            _VALIDATE_RETURN_ERRCODE(retsize <= destination_count, ERANGE);
        }

        // Ensure the string is null terminated:
        destination[retsize - 1] = '\0';
    }

    if (return_value != nullptr)
        *return_value = retsize;

    return 0;
}



// Converts a wide character into a one-byte character
extern "C" int __cdecl wctob(wint_t const wchar)
{
    if (wchar == WEOF)
        return EOF;

    int  return_value = -1;
    char local_buffer[MB_LEN_MAX];

    errno_t const e = _wcrtomb_s_l(&return_value, local_buffer, MB_LEN_MAX, wchar, nullptr, nullptr);
    if (e == 0 && return_value == 1)
        return local_buffer[0];

    return EOF;
}
