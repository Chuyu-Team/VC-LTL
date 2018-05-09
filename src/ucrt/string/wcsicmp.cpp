/***
*wcsicmp.c - contains case-insensitive wide string comp routine _wcsicmp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       contains _wcsicmp()
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <msvcrt_IAT.h>

#pragma warning(disable:__WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED) // 26018 Prefast can't see that we are checking for terminal nul.

/***
*int _wcsicmp(dst, src) - compare wide-character strings, ignore case
*
*Purpose:
*       _wcsicmp perform a case-insensitive wchar_t string comparision.
*       _wcsicmp is independent of locale.
*
*Entry:
*       wchar_t *dst, *src - strings to compare
*
*Return:
*       Returns <0 if dst < src
*       Returns 0 if dst = src
*       Returns >0 if dst > src
*       Returns _NLSCMPERROR is something went wrong
*       This range of return values may differ from other *cmp/*coll functions.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _wcsicmp_l_downlevel (
        const wchar_t * dst,
        const wchar_t * src,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _wcsicmp(dst, src);

    wchar_t f,l;
    //_LocaleUpdate _loc_update(plocinfo);

    /* validation section */
    _VALIDATE_RETURN(dst != nullptr, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(src != nullptr, EINVAL, _NLSCMPERROR);

    if ( plocinfo->locinfo->lc_handle[LC_CTYPE] == 0)
    {
        do
        {
            f = __ascii_towlower(*dst);
            l = __ascii_towlower(*src);
            dst++;
            src++;
        }
        while ( (f) && (f == l) );
    }
    else
    {
        do
        {
            f = _towlower_l((unsigned short)*(dst++), plocinfo);
            l = _towlower_l((unsigned short)*(src++), plocinfo);
        }
        while ( (f) && (f == l) );
    }
    return (int)(f - l);
}

_LCRT_DEFINE_IAT_SYMBOL(_wcsicmp_l_downlevel);

#endif

#if 0
extern "C" int __cdecl _wcsicmp (
        const wchar_t * dst,
        const wchar_t * src
        )
{
    if (!__acrt_locale_changed())
    {
        wchar_t f,l;

        /* validation section */
        _VALIDATE_RETURN(dst != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(src != nullptr, EINVAL, _NLSCMPERROR);

        do  {
            f = __ascii_towlower(*dst);
            l = __ascii_towlower(*src);
            dst++;
            src++;
        } while ( (f) && (f == l) );
        return (int)(f - l);
    }
    else
    {
        return _wcsicmp_l(dst, src, nullptr);
    }
}
#endif

