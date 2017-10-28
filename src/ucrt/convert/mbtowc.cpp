/***
*mbtowc.c - Convert multibyte char to wide char.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a multibyte character into the equivalent wide character.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <stdlib.h>
#include <locale.h>

/***
*int mbtowc() - Convert multibyte char to wide character.
*
*Purpose:
*       Convert a multi-byte character into the equivalent wide character,
*       according to the LC_CTYPE category of the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       wchar_t  *pwc = pointer to destination wide character
*       const char *s = pointer to multibyte character
*       size_t      n = maximum length of multibyte character to consider
*
*Exit:
*       If s = nullptr, returns 0, indicating we only use state-independent
*       character encodings.
*       If s != nullptr, returns:  0 (if *s = null char)
*                               -1 (if the next n or fewer bytes not valid mbc)
*                               number of bytes comprising converted mbc
*
*Exceptions:
*       If errors are encountered, -1 is returned and errno is set to EILSEQ.
*
*******************************************************************************/

extern "C" int __cdecl _mbtowc_l(
    wchar_t  *pwc,
    const char *s,
    size_t n,
    _locale_t plocinfo
    )
{
    if (!s || n == 0)
        /* indicate do not have state-dependent encodings,
        handle zero length string */
        return 0;

    if (!*s)
    {
        /* handle nullptr char */
        if (pwc)
            *pwc = 0;
        return 0;
    }


    _LocaleUpdate _loc_update(plocinfo);
    _ASSERTE(_loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max == 1 || _loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max == 2);

    if (_loc_update.GetLocaleT()->locinfo->locale_name[LC_CTYPE] == nullptr)
    {
        if (pwc)
            *pwc = (wchar_t) (unsigned char) *s;
        return sizeof(char) ;
    }

    if (_isleadbyte_l((unsigned char) *s, _loc_update.GetLocaleT()))
    {
        /* multi-byte char */

        if ((_loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max <= 1) || ((int) n < _loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max) ||
            (MultiByteToWideChar(_loc_update.GetLocaleT()->locinfo->_public._locale_lc_codepage,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            s,
            _loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max,
            pwc,
            (pwc) ? 1 : 0) == 0))
        {
            /* validate high byte of mbcs char */
            if ((n < (size_t) _loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max) || (!*(s + 1)))
            {
                errno = EILSEQ;
                return -1;
            }
        }
        return _loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max;
    }
    else {
        /* single byte char */
        if (MultiByteToWideChar(_loc_update.GetLocaleT()->locinfo->_public._locale_lc_codepage,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            s,
            1,
            pwc,
            (pwc) ? 1 : 0) == 0)
        {
            errno = EILSEQ;
            return -1;
        }
        return sizeof(char) ;
    }

}

extern "C" int __cdecl mbtowc(
    wchar_t  *pwc,
    const char *s,
    size_t n
    )
{
    return _mbtowc_l(pwc, s, n, nullptr);
}
