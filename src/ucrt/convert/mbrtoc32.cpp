//
// mbrtoc32.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//

#include <errno.h>
#include <uchar.h>
#include <corecrt_internal.h>

extern "C" size_t __cdecl mbrtoc32(char32_t * pwc, const char * s, size_t nin, mbstate_t * pst)
{ /* translate UTF-8 multibyte to UCS-4 char32_t, restartably */
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

    for (su = (unsigned char *)s; ; ++su, --nin)
    { /* consume an input byte */
        if (nin == 0)
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
            if (pwc != 0)
                *pwc = char32_t(wc);
            pst->_State = 0;
            return (wc == 0 ? 0 : (const char *)++su - s);
        }
    }
}

/*
 * Copyright (c) 1992-2013 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V6.40:0009 */
