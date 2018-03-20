/***
*_tolower.c - convert character to lower case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines _Tolower().
*
*******************************************************************************/

#include <internal_shared.h>
#include <ctype.h>
#include <stddef.h>
#include <xlocinfo.h>
#include <locale.h>
#include <awint.h>
#include <stdlib.h>
#include <yvals.h>

/* remove macro definitions of _tolower() and tolower()
 */
#undef  _tolower
#undef  tolower

/***
*int _Tolower(c) - convert character to lower case
*
*Purpose:
*       _Tolower() is a version of tolower with a locale argument.
*
*Entry:
*       c - int value of character to be converted
*       const _Ctypevec * = pointer to locale info
*
*Exit:
*       returns int value of lower case representation of c
*
*Exceptions:
*
*******************************************************************************/

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Tolower (
        int c,
        const _Ctypevec *ploc
        )
{
        int size;
        unsigned char inbuffer[3];
        unsigned char outbuffer[3];

        UINT codepage;


		LCID _Locale;

        //const wchar_t *locale_name;

        if (ploc == 0)
        {
            //locale_name = ___lc_locale_name_func()[LC_CTYPE];
			_Locale = ___lc_handle_func()[LC_CTYPE];

            codepage = ___lc_codepage_func();
        }
        else
        {
            //locale_name = ploc->_LocaleName;
			_Locale = __acrt_LocaleNameToLCID(ploc->_LocaleName, 0);
            codepage = ploc->_Page;
        }

        //if (locale_name == NULL)
		if(_Locale==0)
        {
            if ( (c >= 'A') && (c <= 'Z') )
                c = c + ('a' - 'A');
            return c;
        }

        /* if checking case of c does not require API call, do it */
        if ((unsigned)c < 256)
        {
            if (ploc == 0)
            {
                if (!isupper(c))
                {
                    return c;
                }
            }
            else
            {
                if (!(ploc->_Table[c] & _UPPER))
                {
                    return c;
                }
            }
        }

        /* convert int c to multibyte string */
        if (ploc == 0 ? _cpp_isleadbyte((c >> 8) & 0xff)
                      : (ploc->_Table[(c >> 8) & 0xff] & _LEADBYTE) != 0)
        {
            inbuffer[0] = (c >> 8 & 0xff);
            inbuffer[1] = (unsigned char)c;
            inbuffer[2] = 0;
            size = 2;
        } else {
            inbuffer[0] = (unsigned char)c;
            inbuffer[1] = 0;
            size = 1;
        }

        /* convert wide char to lowercase */
        if (0 == (size = __crtLCMapStringA(_Locale, LCMAP_LOWERCASE,
            (const char *)inbuffer, size, (char *)outbuffer, 3, codepage, TRUE)))
        {
            return c;
        }

        /* construct integer return value */
        if (size == 1)
            return ((int)outbuffer[0]);
        else
            return ((int)outbuffer[1] | ((int)outbuffer[0] << 8));

}


/***
*_Ctypevec _Getctype() - get ctype info for current locale
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

_CRTIMP2_PURE _Ctypevec __CLRCALL_PURE_OR_CDECL _Getctype()
{
        /* get ctype info for current locale */
        _Ctypevec ctype;

        ctype._Page = ___lc_codepage_func();
        ctype._Table = (const short *)_calloc_crt(256, sizeof (*__pctype_func()));
        if (ctype._Table != 0)
        {
            memcpy((void *)ctype._Table, __pctype_func(), 256 * sizeof (*__pctype_func()));
            ctype._Delfl = 1;
        }
        else
        {
            ctype._Table = (const short *)__pctype_func();
            ctype._Delfl = 0;
        }

		wchar_t _LocaleName[LOCALE_NAME_MAX_LENGTH];

        //ctype._LocaleName = ___lc_locale_name_func()[LC_COLLATE];
		auto __lc_collate = ___lc_handle_func()[LC_COLLATE];

		if (__lc_collate&&__acrt_LCIDToLocaleName(__lc_collate, _LocaleName, _countof(_LocaleName), 0))
			ctype._LocaleName = _wcsdup_dbg(_LocaleName, _CRT_BLOCK, __FILE__, __LINE__);
		else
			ctype._LocaleName = NULL;

        return (ctype);
}
