/***
*xmbtowc.c - Convert multibyte char to wide char.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a multibyte character into the equivalent wide character.
*
*******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>              /* for INT_MAX */
#include <stdio.h>               /* for EOF */
#include <xlocinfo.h>            /* for _Cvtvec, _Mbrtowc */
#include <crtdbg.h>
#include <locale.h>
#include <mbctype.h>             /* for _ismbblead_l */
#include <internal_shared.h>

/***
*int _Mbrtowc() - Convert multibyte char to wide character.
*
*Purpose:
*       Convert a multi-byte character into the equivalent wide character,
*       according to the specified LC_CTYPE category, or the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       wchar_t  *pwc = pointer to destination wide character
*       const char *s = pointer to multibyte character
*       size_t      n = maximum length of multibyte character to consider
*       mbstate_t *pst      = pointer to state
*       const _Cvtvec *     = pointer to locale info
*
*Exit:
*       If s = NULL, returns 0, indicating we only use state-independent
*       character encodings.
*       If s != NULL, returns:  0 (if *s = null char)
*                               -1 (if the next n or fewer bytes not valid mbc)
*                               number of bytes comprising converted mbc
*
*Exceptions:
*
*******************************************************************************/

_MRTIMP2 int __cdecl _Mbrtowc(
        wchar_t  *pwc,
        const char *s,
        size_t n,
        mbstate_t *pst,
        const _Cvtvec *ploc
        )
{
        if ( !s || n == 0 )
            /* indicate do not have state-dependent encodings,
               handle zero length string */
            return 0;

        if ( !*s )
        {
            /* handle NULL char */
            if (pwc)
                *pwc = 0;
            return 0;
        }

        {   /* perform locale-dependent parse */
            _Cvtvec cvtvec;
            unsigned char ch;

            if (ploc == 0)
            {
                cvtvec = _Getcvt();
                ploc = &cvtvec;
            }

            if (ploc->_Isclocale)
            {
                if (pwc)
                    *pwc = (wchar_t)(unsigned char)*s;
                return sizeof(char);
            }

            _ASSERTE (ploc->_Mbcurmax == 1 ||
                      ploc->_Mbcurmax == 2);

            if (pst->_Wchar != 0)
            {   /* complete two-byte multibyte character */
                ((char *)pst)[1] = *s;
                if (ploc->_Mbcurmax <= 1 ||
                    (MultiByteToWideChar(ploc->_Page,
                                         MB_PRECOMPOSED|MB_ERR_INVALID_CHARS,
                                         (char *)pst,
                                         2,
                                         pwc,
                                         (pwc) ? 1 : 0) == 0))
                {   /* translation failed */
                    pst->_Wchar = 0;
                    errno = EILSEQ;
                    return -1;
                }
                pst->_Wchar = 0;
                return ploc->_Mbcurmax;
            }

            ch = (unsigned char)*s;
            if ( ploc->_Isleadbyte[ch >> 3] & (1 << (ch & 7)) )
            {
                /* multi-byte char */
                if (n < (size_t)ploc->_Mbcurmax)
                {   /* save partial multibyte character */
                    ((char *)pst)[0] = *s;
                    return (-2);
                }
                else if ( ploc->_Mbcurmax <= 1 ||
                          (MultiByteToWideChar( ploc->_Page,
                                                MB_PRECOMPOSED |
                                                    MB_ERR_INVALID_CHARS,
                                                s,
                                                ploc->_Mbcurmax,
                                                pwc,
                                                (pwc) ? 1 : 0) == 0) )
                {
                    /* validate high byte of mbcs char */
                    if (!*(s+1))
                    {
                        pst->_Wchar = 0;
                        errno = EILSEQ;
                        return -1;
                    }
/*                  else translation failed with no complaint? [pjp] */
                }
                return ploc->_Mbcurmax;
            }
            else {
                /* single byte char */

                if ( MultiByteToWideChar( ploc->_Page,
                                          MB_PRECOMPOSED|MB_ERR_INVALID_CHARS,
                                          s,
                                          1,
                                          pwc,
                                          (pwc) ? 1 : 0) == 0 )
                {
                    errno = EILSEQ;
                    return -1;
                }

                return sizeof(char);
            }
        }
}

#ifdef MRTDLL
_MRTIMP2 int __cdecl _Mbrtowc(
        unsigned short* pwc,
        const char *s,
        size_t n,
        mbstate_t *pst,
        const _Cvtvec *ploc
        )
    {
    return _Mbrtowc((wchar_t *)pwc, s, n, pst, ploc);
    }
#endif  /* MRTDLL */
