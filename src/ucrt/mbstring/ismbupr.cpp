/***
*ismbupr - Test if character is upper case (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Test if character is upper case (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>
/***
* _ismbcupper - Test if character is upper case (MBCS)
*
*Purpose:
*       Test if the supplied character is upper case or not.
*       Handles MBCS characters correctly.
*
*       Note:  Use test against 0x00FF instead of _ISLEADBYTE
*       to ensure that we don't call SBCS routine with a two-byte
*       value.
*
*Entry:
*       unsigned int c = character to test
*
*Exit:
*       Returns TRUE if c is an upper case character; else FALSE
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbcupper_l(unsigned int const c, _locale_t const locale)
{
    //_LocaleUpdate locale_update(locale);
	if (!locale)
		return _ismbcupper(c);

    if (c <= 0x00FF)
    {
        return _mbbisupper_l(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _UPPER, true);
}
#endif

#if 0
extern "C" int __cdecl _ismbcupper(unsigned int const c)
{
    return _ismbcupper_l(c, nullptr);
}
#endif
