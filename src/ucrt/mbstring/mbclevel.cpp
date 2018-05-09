/***
*mbclevel.c - Tests if char is hiragana, katakana, alphabet or digit.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Tests for the various industry defined levels of Microsoft Kanji
*       Code.
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>
#include <msvcrt_IAT.h>


/***
*int _ismbcl0(c) - Tests if char is hiragana, katakana, alphabet or digit.
*
*Purpose:
*       Tests if a given char is hiragana, katakana, alphabet, digit or symbol
*       of Microsoft Kanji code.
*
*Entry:
*       unsigned int c - Character to test.
*
*Exit:
*       Returns non-zero if 0x8140 <= c <= 0x889E, else 0.
*
*Exceptions:
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _ismbcl0_l_downlevel(
        unsigned int c,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _ismbcl0(c);
    //_LocaleUpdate _loc_update(plocinfo);

    return( (plocinfo->mbcinfo->mbcodepage == _KANJI_CP) &&
            (_ismbblead_l(c >> 8, plocinfo)) &&
            (_ismbbtrail_l(c & 0x0ff, plocinfo)) &&
            (c < 0x889f) );
}

_LCRT_DEFINE_IAT_SYMBOL(_ismbcl0_l_downlevel);

#endif

/*extern "C" int (__cdecl _ismbcl0)(
        unsigned int c
        )
{
    return _ismbcl0_l(c, nullptr);
}*/


/***
*int _ismbcl1(c) - Tests for 1st-level Microsoft Kanji code set.
*
*Purpose:
*       Tests if a given char belongs to Microsoft 1st-level Kanji code set.
*
*Entry:
*       unsigned int c - character to test.
*
*Exit:
*       Returns non-zero if 1st-level, else 0.
*
*Exceptions:
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _ismbcl1_l_downlevel(
        unsigned int c,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _ismbcl1(c);
    //_LocaleUpdate _loc_update(plocinfo);

    return( (plocinfo->mbcinfo->mbcodepage == _KANJI_CP) &&
            (_ismbblead_l(c >> 8, plocinfo)) &&
            (_ismbbtrail_l(c & 0x0ff, plocinfo)) &&
            (c >= 0x889f) && (c <= 0x9872) );
}

_LCRT_DEFINE_IAT_SYMBOL(_ismbcl1_l_downlevel);

#endif

/*extern "C" int (__cdecl _ismbcl1)(
    unsigned int c
    )
{
    return _ismbcl1_l(c, nullptr);
}*/


/***
*int _ismbcl2(c) - Tests for a 2nd-level Microsoft Kanji code character.
*
*Purpose:
*       Tests if a given char belongs to the Microsoft 2nd-level Kanji code set.
*
*Entry:
*       unsigned int c - character to test.
*
*Exit:
*       Returns non-zero if 2nd-level, else 0.
*
*Exceptions:
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _ismbcl2_l_downlevel(
        unsigned int c,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _ismbcl2(c);
    //_LocaleUpdate _loc_update(plocinfo);

    return( (plocinfo->mbcinfo->mbcodepage == _KANJI_CP) &&
            (_ismbblead_l(c >> 8, plocinfo)) &&
            (_ismbbtrail_l(c & 0x0ff, plocinfo)) &&
            (c >= 0x989f) && (c <= 0xEAA4) );
}

_LCRT_DEFINE_IAT_SYMBOL(_ismbcl2_l_downlevel);

#endif
/*extern "C" int __cdecl _ismbcl2(
        unsigned int c
        )
{
    return _ismbcl2_l(c, nullptr);
}*/
