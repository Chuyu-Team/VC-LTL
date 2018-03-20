/***
*xstrcoll.c - Collate locale strings
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information.
*
*******************************************************************************/

#include <crtdbg.h>
#include <string.h>
#include <xlocinfo.h>   /* for _Collvec, _Strcoll */
#include <stdlib.h>
#include <malloc.h>
#include <locale.h>
#include <errno.h>
#include <awint.h>

/***
*int _Strcoll() - Collate locale strings
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information.
*       [ANSI].
*
*       Non-C locale support available under _INTL switch.
*       In the C locale, strcoll() simply resolves to strcmp().
*Entry:
*       const char *s1b = pointer to beginning of the first string
*       const char *s1e = pointer past end of the first string
*       const char *s2b = pointer to beginning of the second string
*       const char *s1e = pointer past end of the second string
*       const _Collvec *ploc = pointer to locale info
*
*Exit:
*       Less than 0    = first string less than second string
*       0              = strings are equal
*       Greater than 0 = first string greater than second string
*
*Exceptions:
*       _NLSCMPERROR    = error
*       errno = EINVAL
*
*******************************************************************************/

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Strcoll (
        const char *_string1,
        const char *_end1,
        const char *_string2,
        const char *_end2,
        const _Collvec *ploc
        )
{
        int ret=0;
        UINT codepage;
        int n1 = (int)(_end1 - _string1);
        int n2 = (int)(_end2 - _string2);
        //const wchar_t *locale_name;
		LCID _Locale;

        if (ploc == 0)
        {
            //locale_name = ___lc_locale_name_func()[LC_COLLATE];
			_Locale = ___lc_handle_func()[LC_COLLATE];
            codepage = ___lc_collate_cp_func();
        }
        else
        {
            //locale_name = ploc->_LocaleName;
			_Locale = __acrt_LocaleNameToLCID(ploc->_LocaleName, 0);
            codepage = ploc->_Page;
        }

        //if (locale_name == NULL)
		if (_Locale == 0)
        {
            int ans;
            ans = memcmp(_string1, _string2, n1 < n2 ? n1 : n2);
            ret=(ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1);
        }
        else
        {
            if ( 0 == (ret = __crtCompareStringA(_Locale,
                                                  SORT_STRINGSORT,
                                                  _string1,
                                                  n1,
                                                  _string2,
                                                  n2,
                                                  codepage )) )
            {
                errno=EINVAL;
                ret=_NLSCMPERROR;
            }
            else
            {
                ret-=2;
            }
        }

        return ret;
}


/***
*_Collvec _Getcoll() - get collation info for current locale
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

_CRTIMP2_PURE _Collvec __CLRCALL_PURE_OR_CDECL _Getcoll()
{
        _Collvec coll;

        coll._Page = ___lc_collate_cp_func();
        //coll._LocaleName = ___lc_locale_name_func()[LC_COLLATE];
		auto __lc_collate = ___lc_handle_func()[LC_COLLATE];

		wchar_t _LocaleName[LOCALE_NAME_MAX_LENGTH];

        //if (coll._LocaleName)
		if (__lc_collate&&__acrt_LCIDToLocaleName(__lc_collate, _LocaleName, _countof(_LocaleName), 0))
			coll._LocaleName = _wcsdup_dbg(_LocaleName, _CRT_BLOCK, __FILE__, __LINE__);
		else
			coll._LocaleName = NULL;

        return (coll);
}
