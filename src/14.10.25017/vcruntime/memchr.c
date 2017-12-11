/***
*memchr.c - search block of memory for a given character
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines memchr() - search memory until a character is
*       found or a limit is reached.
*
*******************************************************************************/

#include <string.h>

#ifdef _M_ARM
	#pragma function(memchr)
#endif

/***
*char *memchr(buf, chr, cnt) - search memory for given character.
*
*Purpose:
*       Searches at buf for the given character, stopping when chr is
*       first found or cnt bytes have been searched through.
*
*Entry:
*       void *buf  - memory buffer to be searched
*       int chr    - character to search for
*       size_t cnt - max number of bytes to search
*
*Exit:
*       returns pointer to first occurence of chr in buf
*       returns NULL if chr not found in the first cnt bytes
*
*Exceptions:
*
*******************************************************************************/

void * __cdecl memchr (
        const void * buf,
        int chr,
        size_t cnt
        )
{
        while ( cnt && (*(unsigned char *)buf != (unsigned char)chr) ) {
                buf = (unsigned char *)buf + 1;
                cnt--;
        }

        return(cnt ? (void *)buf : NULL);
}
