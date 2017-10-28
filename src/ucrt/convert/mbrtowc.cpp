/***
*mbrtowc.c - Convert multibyte char to wide char.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a multibyte character into the equivalent wide character.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <corecrt_internal_securecrt.h>
#include <locale.h>
#include <wchar.h>
#include <limits.h>
#include <stdio.h>

/***
*errno_t _mbrtowc_s_l() - Helper function to convert multibyte char to wide character.
*
*Purpose:
*       Convert a multi-byte character into the equivalent wide character,
*       according to the specified LC_CTYPE category, or the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       wchar_t *dst       = pointer to (single) destination wide character
*       const char *s      = pointer to multibyte character
*       size_t n           = maximum length of multibyte character to consider
*       mbstate_t *pmbst   = pointer to state (must be not nullptr)
*       _locale_t plocinfo = locale info
*
*Exit:
*       returns, in *pRetValue:
*       If s = nullptr, 0, indicating we only use state-independent
*       character encodings.
*       If s != nullptr:  0 (if *s = null char)
*                      -1 (if the next n or fewer bytes not valid mbc)
*                      number of bytes comprising converted mbc
*
*Exceptions:
*
*******************************************************************************/

_Success_(return != 0)
_Post_satisfies_(*pRetValue <= _String_length_(s))
static errno_t __cdecl _mbrtowc_s_l(
    _Inout_ _Out_range_(<=, 1)              int *           pRetValue,
    _Pre_maybenull_ _Out_writes_opt_z_(1)   wchar_t *       dst,
    _In_opt_z_                              const char *    s,
    _In_                                    size_t          n,
    _Inout_                                 mbstate_t *     pmbst,
    _In_opt_                                _locale_t       plocinfo
    ) throw()
{
    _ASSERTE(pmbst != nullptr);
    _ASSIGN_IF_NOT_NULL(dst, 0);

    if (!s || n == 0)
    {
        /* indicate do not have state-dependent encodings,
        handle zero length string */
        _ASSIGN_IF_NOT_NULL(pRetValue, 0);
        return 0;
    }

    if (!*s)
    {
        /* handle nullptr char */
        _ASSIGN_IF_NOT_NULL(pRetValue, 0);
        return 0;
    }

    _LocaleUpdate _loc_update(plocinfo);
    
    const int locale_mb_cur_max = _loc_update.GetLocaleT()->locinfo->_public._locale_mb_cur_max;
    _ASSERTE(locale_mb_cur_max == 1 || locale_mb_cur_max == 2);

    if (_loc_update.GetLocaleT()->locinfo->locale_name[LC_CTYPE] == nullptr)
    {
        _ASSIGN_IF_NOT_NULL(dst, (wchar_t) (unsigned char) *s);
        _ASSIGN_IF_NOT_NULL(pRetValue, 1);
        return 0;
    }

    if (pmbst->_Wchar != 0)
    {
        /* complete two-byte multibyte character */
        ((char *) pmbst)[1] = *s;
        if (locale_mb_cur_max <= 1 ||
            (MultiByteToWideChar(
            _loc_update.GetLocaleT()->locinfo->_public._locale_lc_codepage,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            (char *) pmbst,
            2,
            dst,
            (dst != nullptr ? 1 : 0)) == 0))
        {
            /* translation failed */
            pmbst->_Wchar = 0;
            errno = EILSEQ;
            _ASSIGN_IF_NOT_NULL(dst, 0);
            _ASSIGN_IF_NOT_NULL(pRetValue, -1);
            return errno;
        }
        pmbst->_Wchar = 0;
        _ASSIGN_IF_NOT_NULL(pRetValue, locale_mb_cur_max);
        return 0;
    }
    else if (_isleadbyte_l((unsigned char) *s, _loc_update.GetLocaleT()))
    {
        /* multi-byte char */
        if (n < (size_t) locale_mb_cur_max)
        {
            /* save partial multibyte character */
            ((char *) pmbst)[0] = *s;
            _ASSIGN_IF_NOT_NULL(pRetValue, -2);
            return 0;
        }
        else if (locale_mb_cur_max <= 1 ||
            (MultiByteToWideChar(_loc_update.GetLocaleT()->locinfo->_public._locale_lc_codepage,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            s,
            static_cast<int>(__min(strlen(s), INT_MAX)),
            dst,
            (dst != nullptr ? 1 : 0)) == 0))
        {
            /* validate high byte of mbcs char */
            if (!*(s + 1))
            {
                pmbst->_Wchar = 0;
                errno = EILSEQ;
                _ASSIGN_IF_NOT_NULL(dst, 0);
                _ASSIGN_IF_NOT_NULL(pRetValue, -1);
                return errno;
            }
        }
        _ASSIGN_IF_NOT_NULL(pRetValue, locale_mb_cur_max);
        return 0;
    }
    else {
        /* single byte char */
        if (MultiByteToWideChar(
            _loc_update.GetLocaleT()->locinfo->_public._locale_lc_codepage,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            s,
            1,
            dst,
            (dst != nullptr ? 1 : 0)) == 0)
        {
            errno = EILSEQ;
            _ASSIGN_IF_NOT_NULL(dst, 0);
            _ASSIGN_IF_NOT_NULL(pRetValue, -1);
            return errno;
        }

        _ASSIGN_IF_NOT_NULL(pRetValue, sizeof(char) );
        return 0;
    }
}


/***
*wint_t btowc(c) - translate single byte to wide char
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

extern "C" wint_t __cdecl btowc(
    int c
    )
{
    if (c == EOF)
    {
        return WEOF;
    }
    else
    {
        /* convert as one-byte string */
        char ch = (char) c;
        mbstate_t mbst = {};
        wchar_t wc = 0;
        int retValue = -1;

        _mbrtowc_s_l(&retValue, &wc, &ch, 1, &mbst, nullptr);
        return (retValue < 0 ? WEOF : wc);
    }
}


/***
*size_t mbrlen(s, n, pst) - determine next multibyte code, restartably
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

extern "C" size_t __cdecl mbrlen(
    const char *s,
    size_t n,
    mbstate_t *pst
    )
{
    static mbstate_t mbst = {};
    int retValue = -1;

    _mbrtowc_s_l(&retValue, nullptr, s, n, (pst != nullptr ? pst : &mbst), nullptr);
    return retValue;
}


/***
*size_t mbrtowc(pwc, s, n, pst) - translate multibyte to wchar_t, restartably
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

extern "C" size_t __cdecl mbrtowc(
    wchar_t *dst,
    const char *s,
    size_t n,
    mbstate_t *pst
    )
{
    static mbstate_t mbst = {};
    int retValue = -1;

    if (s != nullptr)
    {
        _mbrtowc_s_l(&retValue, dst, s, n, (pst != nullptr ? pst : &mbst), nullptr);
    }
    else
    {
        _mbrtowc_s_l(&retValue, nullptr, "", 1, (pst != nullptr ? pst : &mbst), nullptr);
    }
    return retValue;
}


/***
*size_t mbsrtowcs(wcs, ps, n, pst) - translate multibyte string to wide,
*       restartably
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

/* Helper function shared by the secure and non-secure versions. */

_Success_(return == 0)
static size_t __cdecl _mbsrtowcs_helper(
    _Out_writes_opt_z_(n)               wchar_t *wcs,
    _Deref_pre_opt_z_                   const char **ps,
    _In_                                size_t n,
    _Inout_                             mbstate_t *pst
    ) throw()
{
    /* validation section */
    _VALIDATE_RETURN(ps != nullptr, EINVAL, (size_t) - 1);

    static mbstate_t mbst = {};
    const char *s = *ps;
    int i = 0;
    size_t nwc = 0;
    _LocaleUpdate _loc_update(nullptr);

    if (pst == nullptr)
    {
        pst = &mbst;
    }

    if (wcs == nullptr)
    {
        for (;; ++nwc, s += i)
        {
            /* translate but don't store */
            wchar_t wc;
            _mbrtowc_s_l(&i, &wc, s, INT_MAX, pst, _loc_update.GetLocaleT());
            if (i < 0)
            {
                return (size_t) - 1;
            }
            else if (i == 0)
            {
                return nwc;
            }
        }
    }

    for (; 0 < n; ++nwc, s += i, ++wcs, --n)
    {
        /* translate and store */
        _mbrtowc_s_l(&i, wcs, s, INT_MAX, pst, _loc_update.GetLocaleT());
        if (i < 0)
        {
            /* encountered invalid sequence */
            nwc = (size_t) - 1;
            break;
        }
        else if (i == 0)
        {
            /* encountered terminating null */
            s = 0;
            break;
        }
    }

    *ps = s;
    return nwc;
}

extern "C" size_t __cdecl mbsrtowcs(
    wchar_t *wcs,
    const char **ps,
    size_t n,
    mbstate_t   *pst
    )
{
    /* Call a non-deprecated helper to do the work. */

    return _mbsrtowcs_helper(wcs, ps, n, pst);
}


/***
*errno_t mbsrtowcs_s() - Convert multibyte char string to wide char string.
*
*Purpose:
*       Convert a multi-byte char string into the equivalent wide char string,
*       according to the LC_CTYPE category of the current locale.
*       Same as mbstowcs(), but the destination is ensured to be null terminated.
*       If there's not enough space, we return EINVAL.
*
*Entry:
*       size_t *pRetValue = Number of bytes modified including the terminating nullptr
*                           This pointer can be nullptr.
*       wchar_t *pwcs = pointer to destination wide character string buffer
*       size_t sizeInWords = size of the destination buffer
*       const char **s = pointer to source multibyte character string
*       size_t n = maximum number of wide characters to store (not including the terminating nullptr)
*       mbstate_t *pst = pointer to the conversion state
*
*Exit:
*       The error code.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

extern "C" errno_t __cdecl mbsrtowcs_s(
    size_t *pRetValue,
    wchar_t *dst,
    size_t sizeInWords,
    const char **ps,
    size_t n,
    mbstate_t *pmbst
    )
{
    size_t retsize;

    /* validation section */
    _ASSIGN_IF_NOT_NULL(pRetValue, (size_t) - 1);
    _VALIDATE_RETURN_ERRCODE((dst == nullptr && sizeInWords == 0) || (dst != nullptr && sizeInWords > 0), EINVAL);
    if (dst != nullptr)
    {
        _RESET_STRING(dst, sizeInWords);
    }
    _VALIDATE_RETURN_ERRCODE(ps != nullptr, EINVAL);

    /* Call a non-deprecated helper to do the work. */

    retsize = _mbsrtowcs_helper(dst, ps, (n > sizeInWords ? sizeInWords : n), pmbst);

    if (retsize == (size_t) - 1)
    {
        if (dst != nullptr)
        {
            _RESET_STRING(dst, sizeInWords);
        }
        return errno;
    }

    /* count the null terminator */
    retsize++;

    if (dst != nullptr)
    {
        /* return error if the string does not fit */
        if (retsize > sizeInWords)
        {
            _RESET_STRING(dst, sizeInWords);
            _VALIDATE_RETURN_ERRCODE(sizeInWords <= retsize, ERANGE);
        }
        else
        {
            /* ensure the string is null terminated */
            dst[retsize - 1] = '\0';
        }
    }

    _ASSIGN_IF_NOT_NULL(pRetValue, retsize);

    return 0;
}
