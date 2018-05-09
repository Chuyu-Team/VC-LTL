/***
*ismbalph.c - Test if character is alphabetic (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Test if character is alphabetic (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>
#include <msvcrt_IAT.h>


/***
* _ismbcalpha - Test if character is alphabetic (MBCS)
*
*Purpose:
*       Test if character is alphabetic.
*       Handles MBCS chars correctly.
*
*       Note:  Use test against 0x00FF instead of _ISLEADBYTE
*       to ensure that we don't call SBCS routine with a two-byte
*       value.
*
*Entry:
*       unsigned int c = character to test
*
*Exit:
*       Returns TRUE if c is alphabetic, else FALSE
*
*Exceptions:
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _ismbcalpha_l_downlevel(unsigned int const c, _locale_t const locale)
{
    //_LocaleUpdate locale_update(locale);
	if (!locale)
		return _ismbcalpha(c);

    if (c <= 0x00FF)
    {
        return _ismbbalpha_l(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _ALPHA, true);
}

_LCRT_DEFINE_IAT_SYMBOL(_ismbcalpha_l_downlevel);

#endif

/*extern "C" int __cdecl _ismbcalpha(unsigned int const c)
{
    return _ismbcalpha_l(c, nullptr);
}*/
