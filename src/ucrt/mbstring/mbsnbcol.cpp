/***
*mbsnbcol.c - Collate n bytes of two MBCS strings
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Collate n bytes of two MBCS strings
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <limits.h>
#include <locale.h>
#include <string.h>
#include <winapi_thunks.h>

/***
* _mbsnbcoll(s1, s2, n) - Collate n bytes of two MBCS strings
*
*Purpose:
*       Collates up to n bytes of two strings for lexical order.
*
*Entry:
*       unsigned char *s1, *s2 = strings to collate
*       size_t n = maximum number of bytes to collate
*
*Exit:
*       Returns <0 if s1 < s2
*       Returns  0 if s1 == s2
*       Returns >0 if s1 > s2
*       Returns _NLSCMPERROR is something went wrong
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _mbsnbcoll_l(
        const unsigned char *s1,
        const unsigned char *s2,
        size_t n,
        _locale_t plocinfo
        )
{
		if (!plocinfo)
			return _mbsnbcoll(s1, s2, n);

        int ret;

        if (n == 0)
            return 0;

        /* validation section */
        _VALIDATE_RETURN(s1 != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(s2 != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(n <= INT_MAX, EINVAL, _NLSCMPERROR);

        //_LocaleUpdate _loc_update(plocinfo);

        if (plocinfo->mbcinfo->ismbcodepage == 0)
            return _strncoll_l((const char *)s1, (const char *)s2, n, plocinfo);

        if ( 0 == (ret = __acrt_CompareStringA(plocinfo,
                                              plocinfo->mbcinfo->mblcid,
                                              SORT_STRINGSORT,
                                              (const char *)s1,
                                              (int)n,
                                              (char *)s2,
                                              (int)n,
                                              plocinfo->mbcinfo->mbcodepage )) )
        {
            errno = EINVAL;
            return _NLSCMPERROR;
        }

        return ret - 2;

}
#endif

#if 0
extern "C" int (__cdecl _mbsnbcoll)(
        const unsigned char *s1,
        const unsigned char *s2,
        size_t n
        )
{
    return _mbsnbcoll_l(s1, s2, n, nullptr);
}
#endif
