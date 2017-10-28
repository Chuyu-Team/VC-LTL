//
// ismbstr.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines _ismbstrail, which is the context-sensitive MBCS trail-byte function.
// While much less efficient than _ismbbtrail, it is also much more sophisticated.
// It determines whether a given sub-string pointer points to a trail byte or not,
// taking into account the context of the string.
//
// These functions return -1 if the pointer points to a trail byte; 0 if it does
// not.
//
#ifndef _MBCS
    #error This file should only be compiled with _MBCS defined
#endif

#include <corecrt_internal_mbstring.h>
#include <locale.h>



extern "C" int __cdecl _ismbstrail_l(
    unsigned char const*       string,
    unsigned char const*       current,
    _locale_t            const locale
    )
{
    _VALIDATE_RETURN(string != nullptr,  EINVAL, 0);
    _VALIDATE_RETURN(current != nullptr, EINVAL, 0);

    _LocaleUpdate locale_update(locale);

    if (locale_update.GetLocaleT()->mbcinfo->ismbcodepage == 0)
        return 0;

    while (string <= current && *string)
    {
        if (_ismbblead_l((*string), locale_update.GetLocaleT()))
        {
            if (++string == current) // check trail byte
                return -1;

            if (*string == 0)
                return 0;
        }
        ++string;
    }

    return 0;
}

extern "C" int __cdecl _ismbstrail(
    unsigned char const* const string,
    unsigned char const* const current
    )
{
    return _ismbstrail_l(string, current, nullptr);
}
