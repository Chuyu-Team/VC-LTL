/***
*mbsnbcat.c - concatenate string2 onto string1, max length n bytes
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       defines mbsnbcat() - concatenate maximum of n bytes
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal.h>
#include <corecrt_internal_mbstring.h>
#include <locale.h>
#include <string.h>
#include <msvcrt_IAT.h>


/***
* _mbsnbcat - concatenate max cnt bytes onto dst
*
*Purpose:
*       Concatenates src onto dst, with a maximum of cnt bytes copied.
*       Handles 2-byte MBCS characters correctly.
*
*Entry:
*       unsigned char *dst - string to concatenate onto
*       unsigned char *src - string to concatenate from
*       int cnt - number of bytes to copy
*
*Exit:
*       returns dst, with src (at least part) concatenated on
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" unsigned char * __cdecl _mbsnbcat_l_downlevel(
        unsigned char *dst,
        const unsigned char *src,
        size_t cnt,
        _locale_t plocinfo
        )
{
		if(!plocinfo)
			return _mbsnbcat(dst, src, cnt);

        unsigned char *start;

        if (!cnt)
                return(dst);

        /* validation section */
        _VALIDATE_RETURN(dst != nullptr, EINVAL, nullptr);
        _VALIDATE_RETURN(src != nullptr, EINVAL, nullptr);

        //_LocaleUpdate _loc_update(plocinfo);

        _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        if (plocinfo->mbcinfo->ismbcodepage == 0)
            return (unsigned char *)strncat((char *)dst, (const char *)src, cnt);
        _END_SECURE_CRT_DEPRECATION_DISABLE

        start = dst;
        while (*dst++)
                ;
        --dst;          // dst now points to end of dst string

        /* if last char in string is a lead byte, back up pointer */
        if ( dst!=start && _mbsbtype_l(start, (int) ((dst - start) - 1), plocinfo) == _MBC_LEAD )
        {
            --dst;
        }

        /* copy over the characters */

        while (cnt--) {

            if ( _ismbblead_l(*src, plocinfo) ) {
                *dst++ = *src++;
                if (cnt == 0) {   /* write null if cnt exhausted */
                    dst[-1] = '\0';
                    break;
                }
                cnt--;
                if ((*dst++ = *src++)=='\0') { /* or if no trail byte */
                    dst[-2] = '\0';
                    break;
                }
            }
            else if ((*dst++ = *src++) == '\0')
                break;

        }

        if ( dst!=start && _mbsbtype_l(start, (int) ((dst - start) - 1), plocinfo) == _MBC_LEAD )
        {
            dst[-1] = '\0';
        }
        else
        {
            *dst = '\0';
        }

        return(start);
}

_LCRT_DEFINE_IAT_SYMBOL(_mbsnbcat_l_downlevel);

#endif

/*extern "C" unsigned char * (__cdecl _mbsnbcat)(
        unsigned char *dst,
        const unsigned char *src,
        size_t cnt
        )
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    return _mbsnbcat_l(dst, src, cnt, nullptr);
    _END_SECURE_CRT_DEPRECATION_DISABLE
}*/
