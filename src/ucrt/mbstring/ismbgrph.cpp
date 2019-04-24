/***
*ismbgrph - Test if character is graphical (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Test if character is graphical (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>


/***
* _ismbcgraph - Test if character is graphical (MBCS)
*
*Purpose:
*       Test if the supplied character is graphical or not.
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
*       Returns TRUE if c is an graphical character; else FALSE
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbcgraph_l(unsigned int const c, _locale_t const locale)
{
	if (!locale)
		return _ismbcgraph(c);
    //_LocaleUpdate locale_update(locale);

    if (c <= 0x00FF)
    {
        return _ismbbgraph_l(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _PUNCT | _ALPHA | _DIGIT, true);
}
#endif

#if 0
extern "C" int __cdecl _ismbcgraph(unsigned int const c)
{
    return _ismbcgraph_l(c, nullptr);
}
#endif
