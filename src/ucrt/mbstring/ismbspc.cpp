/***
*ismbspc.c - Test is character is whitespace (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Test is character is whitespace (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>
#include <msvcrt_IAT.h>

/***
* _ismbcspace - Test is character is whitespace (MBCS)
*
*Purpose:
*       Test if the character is a whitespace character.
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
*       Returns TRUE if character is whitespace, else FALSE
*
*Exceptions:
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _ismbcspace_l_downlevel(unsigned int const c, _locale_t const locale)
{
    //_LocaleUpdate locale_update(locale);
	if (!locale)
		return _ismbcspace(c);

    if (c <= 0x00FF)
    {
        return _isspace_l(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _SPACE, true);
}

_LCRT_DEFINE_IAT_SYMBOL(_ismbcspace_l_downlevel);

#endif

/*extern "C" int __cdecl _ismbcspace(unsigned int const c)
{
    return _ismbcspace_l(c, nullptr);
}*/
