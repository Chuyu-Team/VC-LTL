//
// _ctype.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the function equivalents of the character classification macros in
// <ctype.h>.  These function definitions make use of the macros.  The functions
// return zero if the character does not meet the requirements, and nonzero if
// the character does meet the requirements.
//
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>



static __forceinline int __cdecl fast_check(int const c, int const mask) throw()
{
    #ifdef _DEBUG
    return _chvalidator(c, mask);
    #else
    return __acrt_initial_locale_data._public._locale_pctype[c] & mask;
    #endif
}



extern "C" extern __inline int (__cdecl _isalpha_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isalpha_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isalpha)(int const c)
{
    return __acrt_locale_changed()
        ? (_isalpha_l)(c, nullptr)
        : fast_check(c, _ALPHA);
}

extern "C" extern __inline int (__cdecl _isupper_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isupper_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isupper)(int const c)
{
    return __acrt_locale_changed()
        ? (_isupper_l)(c, nullptr)
        : fast_check(c, _UPPER);
}

extern "C" extern __inline int (__cdecl _islower_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _islower_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl islower)(int const c)
{
    return __acrt_locale_changed()
        ? (_islower_l)(c, nullptr)
        : fast_check(c, _LOWER);
}

extern "C" extern __inline int (__cdecl _isdigit_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isdigit_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isdigit)(int const c)
{
    return __acrt_locale_changed()
        ? (_isdigit_l)(c, nullptr)
        : fast_check(c, _DIGIT);
}

extern "C" extern __inline int (__cdecl _isxdigit_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isxdigit_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isxdigit)(int const c)
{
    return __acrt_locale_changed()
        ? (_isxdigit_l)(c, nullptr)
        : fast_check(c, _HEX);
}

extern "C" extern __inline int (__cdecl _isspace_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isspace_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isspace)(int const c)
{
    return __acrt_locale_changed()
        ? (_isspace_l)(c, nullptr)
        : fast_check(c, _SPACE);
}

extern "C" extern __inline int (__cdecl _ispunct_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _ispunct_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl ispunct)(int const c)
{
    return __acrt_locale_changed()
        ? (_ispunct_l)(c, nullptr)
        : fast_check(c, _PUNCT);
}

extern "C" extern __inline int (__cdecl _isblank_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isblank_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isblank)(int const c)
{
    return __acrt_locale_changed()
        ? (_isblank_l)(c, nullptr)
        : ((c == '\t') ? _BLANK : fast_check(c, _BLANK));
}

extern "C" extern __inline int (__cdecl _isalnum_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isalnum_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isalnum)(int const c)
{
    return __acrt_locale_changed()
        ? (_isalnum_l)(c, nullptr)
        : fast_check(c, _ALPHA | _DIGIT);
}

extern "C" extern __inline int (__cdecl _isprint_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isprint_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isprint)(int const c)
{
    return __acrt_locale_changed()
        ? (_isprint_l)(c, nullptr)
        : fast_check(c, _BLANK | _PUNCT | _ALPHA | _DIGIT);
}

extern "C" extern __inline int (__cdecl _isgraph_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _isgraph_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl isgraph)(int const c)
{
    return __acrt_locale_changed()
        ? (_isgraph_l)(c, nullptr)
        : fast_check(c, _PUNCT | _ALPHA | _DIGIT);
}

extern "C" extern __inline int (__cdecl _iscntrl_l)(int const c, _locale_t const locale)
{
    _LocaleUpdate locale_update(locale);

    return _iscntrl_l(c, locale_update.GetLocaleT());
}

extern "C" extern __inline int (__cdecl iscntrl)(int const c)
{
    return __acrt_locale_changed()
        ? (_iscntrl_l)(c, nullptr)
        : fast_check(c, _CONTROL);
}

extern "C" extern __inline int (__cdecl __isascii)(int const c)
{
    return __isascii(c);
}

extern "C" extern __inline int (__cdecl __toascii)(int const c)
{
    return __toascii(c);
}

extern "C" extern __inline int (__cdecl _iscsymf_l)(int const c, _locale_t const locale)
{
    return (_isalpha_l)(c, locale) || (c) == '_';
}

extern "C" extern __inline int (__cdecl __iscsymf)(int const c)
{
    return __iscsymf(c);
}

extern "C" extern __inline int (__cdecl _iscsym_l)(int const c, _locale_t const locale)
{
    return (_isalnum_l)(c, locale) || (c) == '_';
}

extern "C" extern __inline int (__cdecl __iscsym)(int const c)
{
    return __iscsym((unsigned char)(c));
}
