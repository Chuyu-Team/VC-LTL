/***
*ismbcknj.c - contains the Kanji specific is* functions.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Provide non-portable Kanji support for MBCS libs.
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>


/***
*int _ismbchira(c) - test character for hiragana (Japanese)
*
*Purpose:
*       Test if the character c is a hiragana character.
*
*Entry:
*       unsigned int c - character to test
*
*Exit:
*       returns TRUE if CP == KANJI and character is hiragana, else FALSE
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbchira_l(
        unsigned int c,
        _locale_t plocinfo
        )
{
    //_LocaleUpdate _loc_update(plocinfo);

    return((plocinfo ? plocinfo->mbcinfo->mbcodepage : _getmbcp()) == _KANJI_CP && c >= 0x829f && c <= 0x82f1);
}
#endif

#if 0
extern "C" int __cdecl _ismbchira(
        unsigned int c
        )
{
    return _ismbchira_l(c, nullptr);
}
#endif

/***
*int _ismbckata(c) - test character for katakana (Japanese)
*
*Purpose:
*       Tests to see if the character c is a katakana character.
*
*Entry:
*       unsigned int c - character to test
*
*Exit:
*       Returns TRUE if CP == KANJI and c is a katakana character, else FALSE.
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbckata_l (
        unsigned int c,
        _locale_t plocinfo
        )
{
    //_LocaleUpdate _loc_update(plocinfo);

    return((plocinfo ? plocinfo->mbcinfo->mbcodepage : _getmbcp()) == _KANJI_CP && c >= 0x8340 && c <= 0x8396 && c != 0x837f);
}
#endif

#if 0
extern "C" int __cdecl _ismbckata(
        unsigned int c
        )
{
    return _ismbckata_l(c, nullptr);
}
#endif

/***
*int _ismbcsymbol(c) - Tests if char is punctuation or symbol of Microsoft Kanji
*                  code.
*
*Purpose:
*       Returns non-zero if the character is kanji punctuation.
*
*Entry:
*       unsigned int c - character to be tested
*
*Exit:
*       Returns non-zero if CP == KANJI and the specified char is punctuation or symbol of
*               Microsoft Kanji code, else 0.
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbcsymbol_l(
        unsigned int c,
        _locale_t plocinfo
        )
{
    //_LocaleUpdate _loc_update(plocinfo);

    return((plocinfo ? plocinfo->mbcinfo->mbcodepage : _getmbcp()) == _KANJI_CP && c >= 0x8141 && c <= 0x81ac && c != 0x817f);
}
#endif

#if 0
extern "C" int (__cdecl _ismbcsymbol)(
        unsigned int c
        )
{
    return _ismbcsymbol_l(c, nullptr);
}
#endif