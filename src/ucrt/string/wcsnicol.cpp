/***
*wcsnicoll.c - Collate wide-character locale strings without regard to case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Compare two wchar_t strings using the locale LC_COLLATE information
*       without regard to case.
*       Compares at most n characters of two strings.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <winapi_thunks.h>

/***
*int _wcsnicoll() - Collate wide-character locale strings without regard to case
*
*Purpose:
*       Compare two wchar_t strings using the locale LC_COLLATE information
*       without regard to case.
*       Compares at most n characters of two strings.
*       In the C locale, _wcsicmp() is used to make the comparison.
*
*Entry:
*       const wchar_t *s1 = pointer to the first string
*       const wchar_t *s2 = pointer to the second string
*       size_t count - maximum number of characters to compare
*
*Exit:
*       -1 = first string less than second string
*        0 = strings are equal
*        1 = first string greater than second string
*       Returns _NLSCMPERROR is something went wrong
*       This range of return values may differ from other *cmp/*coll functions.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _wcsnicoll_l (
        const wchar_t *_string1,
        const wchar_t *_string2,
        size_t count,
        _locale_t plocinfo
        )
{
    int ret;

    if (!count)
    {
        return 0;
    }

    /* validation section */
    _VALIDATE_RETURN(_string1 != nullptr, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(_string2 != nullptr, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(count <= INT_MAX, EINVAL, _NLSCMPERROR);

    //_LocaleUpdate _loc_update(plocinfo);
	auto _lc_collate = (plocinfo ? plocinfo->locinfo->lc_handle : ___lc_handle_func())[LC_COLLATE];

    if ( _lc_collate == 0 )
    {
        return __ascii_wcsnicmp(_string1, _string2, count);
    }

    if ( 0 == (ret = __acrt_CompareStringW(
                    _lc_collate,
                               SORT_STRINGSORT | NORM_IGNORECASE,
                               _string1,
                               (int)count,
                               _string2,
                               (int)count)) )
    {
        errno = EINVAL;
        return _NLSCMPERROR;
    }

    return (ret - 2);
}
#endif

#if 0
extern "C" int __cdecl _wcsnicoll (
        const wchar_t *_string1,
        const wchar_t *_string2,
        size_t count
        )
{
    if (!__acrt_locale_changed())
    {
        /* validation section */
        _VALIDATE_RETURN(_string1 != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(_string2 != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(count <= INT_MAX, EINVAL, _NLSCMPERROR);

        return __ascii_wcsnicmp(_string1, _string2, count);
    }
    else
    {
        return _wcsnicoll_l(_string1, _string2, count, nullptr);
    }
}
#endif
