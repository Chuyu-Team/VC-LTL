/***
*memicmp.c - compare memory, ignore case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _memicmp() - compare two blocks of memory for ordinal
*       order.  Case is ignored in the comparison.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <msvcrt_IAT.h>

/***
*int _memicmp(first, last, count) - compare two blocks of memory, ignore case
*
*Purpose:
*       Compares count bytes of the two blocks of memory stored at first
*       and last.  The characters are converted to lowercase before
*       comparing (not permanently), so case is ignored in the search.
*
*Entry:
*       char *first, *last - memory buffers to compare
*       size_t count - maximum length to compare
*
*Exit:
*       Returns < 0 if first < last
*       Returns 0 if first == last
*       Returns > 0 if first > last
*       Returns _NLSCMPERROR is something went wrong
*
*Uses:
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _memicmp_l_downlevel (
        const void * first,
        const void * last,
        size_t count,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _memicmp(first, last, count);

    int f = 0, l = 0;
    const char *dst = (const char *)first, *src = (const char *)last;
    //_LocaleUpdate _loc_update(plocinfo);

    /* validation section */
    _VALIDATE_RETURN(first != nullptr || count == 0, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(last != nullptr || count == 0, EINVAL, _NLSCMPERROR);

    if ( plocinfo->locinfo->lc_handle[LC_CTYPE] == 0 )
    {
        return __ascii_memicmp(first, last, count);
    }
    else
    {
        while (count-- && f==l)
        {
            f = _tolower_l( (unsigned char)(*(dst++)), plocinfo );
            l = _tolower_l( (unsigned char)(*(src++)), plocinfo );
        }
    }
    return ( f - l );
}

_LCRT_DEFINE_IAT_SYMBOL(_memicmp_l_downlevel);

#endif

#if !defined(_M_IX86) || defined(_M_HYBRID_X86_ARM64)

extern "C" int __cdecl __ascii_memicmp (
        const void * first,
        const void * last,
        size_t count
        )
{
    int f = 0;
    int l = 0;

#pragma warning(push)
#pragma warning(disable:__WARNING_DEREF_NULL_PTR)
// 6011 Dereferencing NULL pointer; validation is weak here, but let's leave it asis
// In particular, the pointers are annotated as opt, but they can only be null if count == 0.
    while ( count-- )
    {
        if ( (*(unsigned char *)first == *(unsigned char *)last) ||
             ((f = __ascii_tolower( *(unsigned char *)first )) ==
              (l = __ascii_tolower( *(unsigned char *)last ))) )
        {
                first = (char *)first + 1;
                last = (char *)last + 1;
        }
        else
            break;
    }
#pragma warning(pop)
    return ( f - l );
}

#endif  /* !_M_IX86 || _M_HYBRID_X86_ARM64 */

#if 0
extern "C" int __cdecl _memicmp (
        const void * first,
        const void * last,
        size_t count
        )
{
    if (!__acrt_locale_changed())
    {
        /* validation section */
        _VALIDATE_RETURN(first != nullptr || count == 0, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(last != nullptr || count == 0, EINVAL, _NLSCMPERROR);

        return __ascii_memicmp(first, last, count);
    }
    else
    {
        return _memicmp_l(first, last, count, nullptr);
    }
}
#endif
