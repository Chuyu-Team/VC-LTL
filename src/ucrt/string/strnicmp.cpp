/***
*strnicmp.c - compare n chars of strings, ignoring case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _strnicmp() - Compares at most n characters of two strings,
*       without regard to case.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <msvcrt_IAT.h>

/***
*int _strnicmp(first, last, count) - compares count char of strings, ignore case
*
*Purpose:
*       Compare the two strings for ordinal order.  Stops the comparison
*       when the following occurs: (1) strings differ, (2) the end of the
*       strings is reached, or (3) count characters have been compared.
*       For the purposes of the comparison, upper case characters are
*       converted to lower case.
*
*Entry:
*       char *first, *last - strings to compare
*       size_t count - maximum number of characters to compare
*
*Exit:
*       returns <0 if first < last
*       returns 0 if first == last
*       returns >0 if first > last
*       Returns _NLSCMPERROR is something went wrong
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _strnicmp_l_downlevel (
        const char * dst,
        const char * src,
        size_t count,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _strnicmp(dst, src, count);

    int f,l;

    if ( count )
    {
        //_LocaleUpdate _loc_update(plocinfo);

        /* validation section */
        _VALIDATE_RETURN(dst != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(src != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(count <= INT_MAX, EINVAL, _NLSCMPERROR);

        if ( plocinfo->locinfo->lc_handle[LC_CTYPE] == 0 )
        {
            return __ascii_strnicmp(dst, src, count);
        }
        else
        {
            do
            {
                f = _tolower_l( (unsigned char)(*(dst++)), plocinfo );
                l = _tolower_l( (unsigned char)(*(src++)), plocinfo );
            }
            while (--count && f && (f == l) );
        }
        return( f - l );
    }

    return( 0 );
}

_LCRT_DEFINE_IAT_SYMBOL(_strnicmp_l_downlevel);

#endif

#if !defined(_M_IX86) || defined(_M_HYBRID_X86_ARM64)

extern "C" int __cdecl __ascii_strnicmp (
        const char * first,
        const char * last,
        size_t count
        )
{
    if(count)
    {
        int f=0;
        int l=0;

        do
        {

            if ( ((f = (unsigned char)(*(first++))) >= 'A') &&
                    (f <= 'Z') )
                f -= 'A' - 'a';

            if ( ((l = (unsigned char)(*(last++))) >= 'A') &&
                    (l <= 'Z') )
                l -= 'A' - 'a';

        }
        while ( --count && f && (f == l) );

        return ( f - l );
    }
    else
    {
        return 0;
    }
}

#endif  /* !_M_IX86 || _M_HYBRID_X86_ARM64 */

#if 0
extern "C" int __cdecl _strnicmp (
        const char * dst,
        const char * src,
        size_t count
        )
{

    if (!__acrt_locale_changed())
    {
        /* validation section */
        _VALIDATE_RETURN(dst != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(src != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(count <= INT_MAX, EINVAL, _NLSCMPERROR);

        return __ascii_strnicmp(dst, src, count);
    }
    else
    {
        return _strnicmp_l(dst, src, count, nullptr);
    }

}
#endif
