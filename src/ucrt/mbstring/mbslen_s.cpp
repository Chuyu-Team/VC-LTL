/***
*mbslen_s.c - Find length of MBCS string
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Find length of MBCS string
*
*******************************************************************************/
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal.h>
#include <corecrt_internal_mbstring.h>
#include <locale.h>
#include <string.h>

/***
* _mbsnlen - Find length of MBCS string
*
*Purpose:
*       Find the length of the MBCS string (in characters).
*
*Entry:
*       unsigned char *s = string
*       size_t maxsize
*
*Exit:
*       Returns the number of MBCS chars in the string.
*       Only the first sizeInBytes bytes of the string are inspected: if the null
*       terminator is not found, sizeInBytes is returned.
*       If the string is null terminated in sizeInBytes bytes, the return value
*       will always be less than sizeInBytes.
*       Returns (size_t)-1 if something went wrong.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#if _CRT_NTDDI_MIN < 0x06000000
size_t __cdecl _mbsnlen_l(
        const unsigned char *s,
        size_t sizeInBytes,
        _locale_t plocinfo
        )
{
		if (!plocinfo)
			return _mbsnlen(s, sizeInBytes);

        size_t n, size;
        //_LocaleUpdate _loc_update(plocinfo);

        if (plocinfo->mbcinfo->ismbcodepage == 0)
            return strnlen((const char *)s, sizeInBytes);

        /* Note that we do not check if s == nullptr, because we do not
         * return errno_t...
         */

        /* Note that sizeInBytes here is the number of bytes, not mb characters! */
        for (n = 0, size = 0; size < sizeInBytes && *s; n++, s++, size++)
        {
            if ( _ismbblead_l(*s, plocinfo) )
                        {
                                size++;
                                if (size >= sizeInBytes)
                                {
                                        break;
                                }
                if (*++s == '\0')
                                {
                    break;
                                }
            }
        }

                return (size >= sizeInBytes ? sizeInBytes : n);
}
#endif

#if _CRT_NTDDI_MIN < 0x06000000
size_t __cdecl _mbsnlen(
        const unsigned char *s,
        size_t maxsize
        )
{
	size_t n, size;

	if (_getmbcp() == 0)
		return strnlen((const char *)s, maxsize);

	/* Note that we do not check if s == nullptr, because we do not
	* return errno_t...
	*/

	/* Note that sizeInBytes here is the number of bytes, not mb characters! */
	for (n = 0, size = 0; size < maxsize && *s; n++, s++, size++)
	{
		if (_ismbblead(*s))
		{
			size++;
			if (size >= maxsize)
			{
				break;
			}
			if (*++s == '\0')
			{
				break;
			}
		}
	}

	return (size >= maxsize ? maxsize : n);
}
#endif