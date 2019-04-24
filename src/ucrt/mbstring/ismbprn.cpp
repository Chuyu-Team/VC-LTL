/***
*ismbprn.c - Test character for display character (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Test character for display character (MBCS)
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>


/***
* _ismbcprint - Test character for display character (MBCS)
*
*Purpose:
*       Test if the character is a display character.
*       Handles MBCS chars correctly.
*
*       Note:  Use test against 0x00FF to ensure that we don't
*       call SBCS routine with a two-byte value.
*
*Entry:
*       unsigned int c = character to test
*
*Exit:
*       Returns TRUE if character is display character, else FALSE
*
*Exceptions:
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
extern "C" int __cdecl _ismbcprint_l(unsigned int const c, _locale_t const locale)
{
	if (!locale)
		return _ismbcprint(c);
    //_LocaleUpdate locale_update(locale);

    if (c <= 0x00FF)
    {
        return _ismbbprint_l(c, locale);
    }

    return __dcrt_multibyte_check_type(c, locale, _CONTROL, false);
}
#endif

#if 0
extern "C" int __cdecl _ismbcprint(unsigned int const c)
{
    return _ismbcprint_l(c, nullptr);
}
#endif
