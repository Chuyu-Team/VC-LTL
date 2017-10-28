//
// c16rtomb.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//

#include <uchar.h>
#include <corecrt_internal.h>

extern "C" size_t __cdecl c16rtomb(char * s, char16_t c16, mbstate_t * pst)
{ /* translate UTF-16 char16_t to UTF-8 multibyte, restartably */
    int nextra;
    char buf[6];
    unsigned char *su;

    static mbstate_t internal_pst{};
    if (!pst)
    {
        pst = &internal_pst;
    }

    char state = (char)pst->_State; /* number of extra words expected */
    unsigned long wc = pst->_Wchar; /* cumulative character */

    if (s == 0)
    { /* find homing sequence */
        s = &buf[0];
        c16 = 0;
    }

    su = (unsigned char *)s;

    if (state != 0)
    { /* fold in second word and convert */
        if (c16 < 0xdc00 || 0xe000 <= c16)
            return ((size_t)-1); /* invalid second word */
        pst->_State = 0;
        wc |= (unsigned long)(c16 - 0xdc00);
    }
    else if (c16 < 0xd800 || 0xdc00 <= c16)
    {
        wc = (unsigned long)c16; /* not first word */
    }
    else
    { /* save value bits of first word for later */
        pst->_State = 1;
        pst->_Wchar = (unsigned long)((c16 - 0xd800 + 0x0040) << 10);
        return (0);
    }

    if ((wc & ~0x7fUL) == 0)
    { /* generate a single byte */
        *su++ = (unsigned char)wc;
        nextra = 0;
    }
    else if ((wc & ~0x7ffUL) == 0)
    { /* generate two bytes */
        *su++ = (unsigned char)(0xc0 | wc >> 6);
        nextra = 1;
    }
    else if ((wc & ~0xffffUL) == 0)
    { /* generate three bytes */
        *su++ = (unsigned char)(0xe0 | wc >> 12);
        nextra = 2;
    }
    else if ((wc & ~0x1fffffUL) == 0)
    { /* generate four bytes */
        *su++ = (unsigned char)(0xf0 | wc >> 18);
        nextra = 3;
    }
    else if ((wc & ~0x3ffffffUL) == 0)
    { /* generate five bytes */
        *su++ = (unsigned char)(0xf8 | wc >> 24);
        nextra = 4;
    }
    else
    { /* generate six bytes */
        *su++ = (unsigned char)(0xfc | ((wc >> 30) & 0x03));
        nextra = 5;
    }

    for (; 0 < nextra;)
    {
        *su++ = (unsigned char)(0x80 | ((wc >> 6 * --nextra) & 0x3f));
    }

    return ((char *)su - s);
}

/*
 * Copyright (c) 1992-2013 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V6.40:0009 */
