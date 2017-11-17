/***
*mbsicoll.c - Collate MBCS strings, ignoring case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Collate MBCS strings, ignoring case
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal.h>
#include <corecrt_internal_mbstring.h>
#include <locale.h>
#include "..\..\winapi_thunks.h"

/***
* _mbsicoll - Collate MBCS strings, ignoring case
*
*Purpose:
*       Collates two strings for lexical order.   Strings
*       are collated on a character basis, not a byte basis.
*
*Entry:
*       char *s1, *s2 = strings to collate
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
#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _mbsicoll_l(
        const unsigned char *s1,
        const unsigned char *s2,
        _locale_t plocinfo
        )
{
        int ret;
        //_LocaleUpdate _loc_update(plocinfo);

        /* validation section */
        _VALIDATE_RETURN(s1 != nullptr, EINVAL, _NLSCMPERROR);
        _VALIDATE_RETURN(s2 != nullptr, EINVAL, _NLSCMPERROR);

        if ((plocinfo ? plocinfo->mbcinfo->ismbcodepage : _getmbcp()) == 0)
            return _stricoll_l((const char *)s1, (const char *)s2, plocinfo);

		int mblcid;
		int mbcodepage;
		if (plocinfo)
		{
			mbcodepage = plocinfo->mbcinfo->mbcodepage;
			mblcid = plocinfo->mbcinfo->mblcid;
		}
		else
		{
			mbcodepage = _getmbcp();

			auto mbcinfo = __acrt_getptd()->_multibyte_info;
			mblcid = mbcinfo->mblcid;
		}

        if ( 0 == (ret = __crtCompareStringA(
                        plocinfo,
                        mblcid,
                        SORT_STRINGSORT | NORM_IGNORECASE,
                        (LPCSTR)s1,
                        -1,
                        (LPSTR)s2,
                        -1,
                        mbcodepage )) )
        {
            errno = EINVAL;
            return _NLSCMPERROR;
        }

        return ret - 2;

}
#endif

//extern "C" int (__cdecl _mbsicoll)(
//        const unsigned char *s1,
//        const unsigned char *s2
//        )
//{
//    return _mbsicoll_l(s1, s2, nullptr);
//}
