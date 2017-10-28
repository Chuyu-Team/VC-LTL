/***
*wcsnicmp.c - compare n chars of wide-character strings, ignoring case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _wcsnicmp() - Compares at most n characters of two wchar_t
*       strings, without regard to case.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <corecrt_internal_securecrt.h>
#include <locale.h>
#include <string.h>

/***
*int _wcsnicmp(first, last, count) - compares count wchar_t of strings,
*       ignore case
*
*Purpose:
*       Compare the two strings for lexical order.  Stops the comparison
*       when the following occurs: (1) strings differ, (2) the end of the
*       strings is reached, or (3) count characters have been compared.
*       For the purposes of the comparison, upper case characters are
*       converted to lower case (wide-characters).
*
*Entry:
*       wchar_t *first, *last - strings to compare
*       size_t count - maximum number of characters to compare
*
*Exit:
*       Returns -1 if first < last
*       Returns 0 if first == last
*       Returns 1 if first > last
*       Returns _NLSCMPERROR is something went wrong
*       This range of return values may differ from other *cmp/*coll functions.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

extern "C" int __cdecl _wcsnicmp_l (
        const wchar_t * first,
        const wchar_t * last,
        size_t count,
        _locale_t plocinfo
        )
{
    wchar_t f,l;
    int result = 0;

    if ( count )
    {
        /* validation section */
        _VALIDATE_RETURN(first != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(last != nullptr, EINVAL, _NLSCMPERROR);

        _LocaleUpdate _loc_update(plocinfo);

        if ( _loc_update.GetLocaleT()->locinfo->locale_name[LC_CTYPE] == nullptr )
        {
            do
            {
                f = __ascii_towlower(*first);
                l = __ascii_towlower(*last);
                first++;
                last++;
            }
            while ( (--count) && f && (f == l) );
        }
        else
        {
            do
            {
                f = _towlower_l( (unsigned short)(*first),_loc_update.GetLocaleT());
                l = _towlower_l( (unsigned short)(*last),_loc_update.GetLocaleT());
                first++;
                last++;
            }
            while ( (--count) && f && (f == l) );
        }

        result = (int)(f - l);
    }
    return result;
}

extern "C" int __cdecl _wcsnicmp (
        const wchar_t * first,
        const wchar_t * last,
        size_t count
        )
{
    if (!__acrt_locale_changed())
    {

        wchar_t f,l;
        int result = 0;

        if(count)
        {
            /* validation section */
            _VALIDATE_RETURN(first != nullptr, EINVAL, _NLSCMPERROR);
            _VALIDATE_RETURN(last != nullptr, EINVAL, _NLSCMPERROR);

            do {
                f = __ascii_towlower(*first);
                l = __ascii_towlower(*last);
                first++;
                last++;
            } while ( (--count) && f && (f == l) );

            result = (int)(f-l);
        }

        return result;

    }
    else
    {
        return _wcsnicmp_l(first, last, count, nullptr);
    }
}

