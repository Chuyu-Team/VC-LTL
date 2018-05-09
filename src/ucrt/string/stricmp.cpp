/***
*stricmp.c - contains case-insensitive string comp routine _stricmp/_strcmpi
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       contains _stricmp(), also known as _strcmpi()
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <msvcrt_IAT.h>

#pragma warning(disable:__WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED) // 26018 Prefast can't see that we are checking for terminal nul.

/***
*int _strcmpi(dst, src), _strcmpi(dst, src) - compare strings, ignore case
*
*Purpose:
*       _stricmp/_strcmpi perform a case-insensitive string comparision.
*       For differences, upper case letters are mapped to lower case.
*       Thus, "abc_" < "ABCD" since "_" < "d".
*
*Entry:
*       char *dst, *src - strings to compare
*
*Return:
*       Returns <0 if dst < src
*       Returns 0 if dst = src
*       Returns >0 if dst > src
*       Returns _NLSCMPERROR is something went wrong
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _stricmp_l_downlevel (
        const char * dst,
        const char * src,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _stricmp(dst, src);

    int f,l;
    //_LocaleUpdate _loc_update(plocinfo);

    /* validation section */
    _VALIDATE_RETURN(dst != nullptr, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(src != nullptr, EINVAL, _NLSCMPERROR);

    if ( plocinfo->locinfo->lc_handle[LC_CTYPE] == 0 )
    {
        return __ascii_stricmp(dst, src);
    }
    else
    {
        do
        {
            f = _tolower_l( (unsigned char)(*(dst++)), plocinfo );
            l = _tolower_l( (unsigned char)(*(src++)), plocinfo );
        } while ( f && (f == l) );
    }

    return(f - l);
}

_LCRT_DEFINE_IAT_SYMBOL(_stricmp_l_downlevel);

#endif

extern "C" int __cdecl __ascii_stricmp (
        const char * dst,
        const char * src
        )
{
    int f, l;

    do
    {
        if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
            f -= 'A' - 'a';
        if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
            l -= 'A' - 'a';
    }
    while ( f && (f == l) );

    return(f - l);
}

#if 0
extern "C" int __cdecl _stricmp (
        const char * dst,
        const char * src
        )
{
    if (!__acrt_locale_changed())
    {
        /* validation section */
        _VALIDATE_RETURN(dst != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(src != nullptr, EINVAL, _NLSCMPERROR);

        return __ascii_stricmp(dst, src);
    }
    else
    {
        return _stricmp_l(dst, src, nullptr);
    }
}
#endif
