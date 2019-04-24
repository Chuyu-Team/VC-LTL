/***
*ismblgl.c - Tests to see if a given character is a legal MBCS char.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Tests to see if a given character is a legal MBCS character.
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>


/***
*int _ismbclegal(c) - tests for a valid MBCS character.
*
*Purpose:
*       Tests to see if a given character is a legal MBCS character.
*
*Entry:
*       unsigned int c - character to test
*
*Exit:
*       returns non-zero if Microsoft Kanji code, else 0
*
*Exceptions:
*
******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbclegal_l(
        unsigned int c,
        _locale_t plocinfo
        )
{
        //_LocaleUpdate _loc_update(plocinfo);
        if (!plocinfo)
            return _ismbclegal(c);

        return( (_ismbblead_l(c >> 8, plocinfo)) &&
                (_ismbbtrail_l(c & 0x0ff, plocinfo)) );
}
#endif

#if 0
extern "C" int (__cdecl _ismbclegal)(
        unsigned int c
        )
{
    return _ismbclegal_l(c, nullptr);
}
#endif
