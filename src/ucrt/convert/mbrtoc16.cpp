//
// mbrtoc16.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//

#include <errno.h>
#include <uchar.h>
#include <corecrt_internal.h>

#define PART_2 16 /* 0 => new char, 1-5 => extra bytes, else 2nd half */

extern "C" size_t __cdecl mbrtoc16(char16_t * pwc, const char * s, size_t nin, mbstate_t * pst)
{ /* translate UTF-8 multibyte to UTF-16 char16_t, restartably */
    unsigned char *su;

    static mbstate_t internal_pst{};
    if (!pst)
    {
        pst = &internal_pst;
    }

    char state = (char)pst->_State; /* number of extra bytes expected */
    unsigned long wc = pst->_Wchar; /* cumulative character */

    if (s == 0)
    { /* find homing sequence */
        pwc = 0;
        s = "";
        nin = 1;
    }

    for (su = (unsigned char *)s;; ++su, --nin)
    { /* deliver 2nd word or consume an input byte */
        if (state == PART_2)
        { /* deliver second half of UTF-16 code */
            if (pwc != 0)
                *pwc = char16_t(0xdc00 | (wc & 0x03ff));
            pst->_State = 0;
            return ((size_t)-3);
        }
        else if (nin == 0)
        { /* report incomplete conversion */
            pst->_Wchar = wc;
            pst->_State = state;
            return ((size_t)-2);
        }
        else if (0 < state)
        { /* fold in a successor byte */
            if ((*su & 0xc0) != 0x80)
            { /* report invalid sequence */
                errno = EILSEQ;
                return ((size_t)-1);
            }
            wc = (unsigned long)((wc << 6) | (*su & 0x3f));
            --state;
        }
        else if ((*su & 0x80) == 0)
        {
            wc = *su; /* consume a single byte */
        }
        else if ((*su & 0xe0) == 0xc0)
        { /* consume first of two bytes */
            wc = (unsigned long)(*su & 0x1f);
            state = 1;
        }
        else if ((*su & 0xf0) == 0xe0)
        { /* consume first of three bytes */
            wc = (unsigned long)(*su & 0x0f);
            state = 2;
        }
        else if ((*su & 0xf8) == 0xf0)
        { /* consume first of four bytes */
            wc = (unsigned long)(*su & 0x07);
            state = 3;
        }
        else if ((*su & 0xfc) == 0xf8)
        { /* consume first of five bytes */
            wc = (unsigned long)(*su & 0x03);
            state = 4;
        }
        else if ((*su & 0xfc) == 0xfc)
        { /* consume first of six bytes */
            wc = (unsigned long)(*su & 0x03);
            state = 5;
        }
        else
        { /* report invalid sequence */
            errno = EILSEQ;
            return ((size_t)-1);
        }

        if (state == 0)
        { /* produce an output wchar */
            if (0x10ffff < wc)
            { /* code too large, report invalid sequence */
                errno = EILSEQ;
                return ((size_t)-1);
            }
            else if (0xffff < wc)
            { /* two-word code, deliver first word */
                if (pwc != 0)
                    *pwc = char16_t(0xd800 | (wc >> 10) - 0x0040);
                pst->_State = PART_2;
                return ((const char *)++su - s);
            }
            else
            { /* one-word code, deliver it */
                if (pwc != 0)
                    *pwc = char16_t(wc);
                pst->_State = 0;
                return (wc == 0 ? 0 : (const char *)++su - s);
            }
        }
    }
}

/*
 * Copyright (c) 1992-2013 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V6.40:0009 */
