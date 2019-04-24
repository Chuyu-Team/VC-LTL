/***
*ismbpunc - Test if character is punctuation (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Test if character is punctuation (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>


/***
* _ismbcpunct - Test if character is punctuation (MBCS)
*
*Purpose:
*       Test if the supplied character is punctuation or not.
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
*       Returns TRUE if c is an punctuation character; else FALSE
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbcpunct_l(unsigned int const c, _locale_t const locale)
{
	if (!locale)
		return _ismbcpunct(c);
    //_LocaleUpdate locale_update(locale);

    if (c <= 0x00FF)
    {
        return _ismbbpunct_l(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _PUNCT, true);
}
#endif

#if 0
extern "C" int __cdecl _ismbcpunct(unsigned int const c)
{
    return _ismbcpunct_l(c, nullptr);
}
#endif

/***
* _ismbcblank - Test if character is blank (MBCS)
*
*Purpose:
*       Test if the supplied character is blank or not.
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
*       Returns TRUE if c is a blank character; else FALSE
*
*Exceptions:
*
*******************************************************************************/

extern "C" int __cdecl _ismbcblank_l(unsigned int const c, _locale_t const locale)
{
    //_LocaleUpdate locale_update(locale);

    if (c <= 0x00FF)
    {
        return (_ismbbblank_l)(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _BLANK, true);
}

extern "C" int __cdecl _ismbcblank(unsigned int const c)
{
    return _ismbcblank_l(c, nullptr);
}
