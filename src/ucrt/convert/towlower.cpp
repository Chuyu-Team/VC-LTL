/***
*towlower.c - convert wide character to lower case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines towlower().
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include "..\..\winapi_thunks.h"
#include <mbctype.h>

/***
*wint_t _towlower_l(c, ptloci) - convert wide character to lower case
*
*Purpose:
*       Multi-thread function only! Non-locking version of towlower.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" wint_t __cdecl _towlower_l (
        wint_t c,
        _locale_t plocinfo
        )
{
    wint_t widechar;

    if (c == WEOF)
        return c;

    //_LocaleUpdate _loc_update(plocinfo);
	auto _lc_ctype = (plocinfo ? plocinfo->locinfo->lc_handle : ___lc_handle_func())[LC_CTYPE];
    if (_lc_ctype==0)
        return __ascii_towlower(c);

    /* if checking case of c does not require API call, do it */
    if ( c < 256 )
    {
        if ( !iswupper(c) ) {
            return c;
        } else {
            return (plocinfo? plocinfo->locinfo : (_getmbcp(),__acrt_getptd()->_locale_info))->pclmap[c];
        }
    }

    /* convert wide char to lowercase */
    if ( 0 == __crtLCMapStringW(
                _lc_ctype,
                LCMAP_LOWERCASE,
                (LPCWSTR)&c,
                1,
                (LPWSTR)&widechar,
                1 ) )
    {
        return c;
    }

    return widechar;

}
#endif

/***
*wint_t towlower(c) - convert wide character to lower case
*
*Purpose:
*       towlower() returns the lowercase equivalent of its argument
*
*Entry:
*       c - wint_t value of character to be converted
*
*Exit:
*       if c is an upper case letter, returns wint_t value of lower case
*       representation of c. otherwise, it returns c.
*
*Exceptions:
*
*******************************************************************************/

//extern "C" wint_t __cdecl towlower (
//        wint_t c
//        )
//{
//
//    return _towlower_l(c, nullptr);
//}
