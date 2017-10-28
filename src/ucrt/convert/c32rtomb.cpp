//
// c32rtomb.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//

#include <uchar.h>

extern "C" size_t __cdecl c32rtomb(char * s, char32_t c32, mbstate_t * /* pst */)
{ /* translate UCS-4 char32_t to UTF-8 multibyte, restartably */
    unsigned char *su;
    unsigned long wc;
    int nextra;
    char buf[6];

    if (s == 0)
    { /* find homing sequence */
        s = &buf[0];
        c32 = 0;
    }

    su = (unsigned char *)s;

    wc = (unsigned long)c32;
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
