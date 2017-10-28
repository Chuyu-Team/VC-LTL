//
// corecrt_internal_mbstring.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This internal header defines internal utilities for working with the multibyte
// character and string library.
//
#pragma once

#include <corecrt_internal.h>
#include <mbctype.h>
#include <mbstring.h>

_CRT_BEGIN_C_HEADER



 // Multibyte full-width-latin upper/lower info
#define NUM_ULINFO 6


#define _ismbbtruelead(_lb,_ch) (!(_lb) && _ismbblead((_ch)))

/* internal use macros since tolower/toupper are locale-dependent */
#define _mbbisupper(_c) ((_mbctype.value()[(_c) + 1] & _SBUP) == _SBUP)
#define _mbbislower(_c) ((_mbctype.value()[(_c) + 1] & _SBLOW) == _SBLOW)

#define _mbbtolower(_c) (_mbbisupper(_c) ? _mbcasemap.value()[_c] : _c)
#define _mbbtoupper(_c) (_mbbislower(_c) ? _mbcasemap.value()[_c] : _c)

#define _ismbbtruelead_l(_lb,_ch,p)   (!(_lb) && _ismbblead_l((_ch), p))
#define _mbbisupper_l(_c, p)      ((p->mbcinfo->mbctype[(_c) + 1] & _SBUP) == _SBUP)
#define _mbbislower_l(_c, p)      ((p->mbcinfo->mbctype[(_c) + 1] & _SBLOW) == _SBLOW)
#define _mbbtolower_l(_c, p)      (_mbbisupper_l(_c, p) ? p->mbcinfo->mbcasemap[_c] : _c)
#define _mbbtoupper_l(_c, p)      (_mbbislower_l(_c, p) ? p->mbcinfo->mbcasemap[_c] : _c)

/* define full-width-latin upper/lower ranges */

#define _MBUPPERLOW1_MT(p)  p->mbcinfo->mbulinfo[0]
#define _MBUPPERHIGH1_MT(p) p->mbcinfo->mbulinfo[1]
#define _MBCASEDIFF1_MT(p)  p->mbcinfo->mbulinfo[2]

#define _MBUPPERLOW2_MT(p)  p->mbcinfo->mbulinfo[3]
#define _MBUPPERHIGH2_MT(p) p->mbcinfo->mbulinfo[4]
#define _MBCASEDIFF2_MT(p)  p->mbcinfo->mbulinfo[5]

// Kanji-specific ranges
#define _MBHIRALOW      0x829f  // Hiragana
#define _MBHIRAHIGH     0x82f1

#define _MBKATALOW      0x8340  // Katakana
#define _MBKATAHIGH     0x8396
#define _MBKATAEXCEPT   0x837f  // Exception

#define _MBKIGOULOW     0x8141  // Kanji punctuation
#define _MBKIGOUHIGH    0x81ac
#define _MBKIGOUEXCEPT  0x817f  // Exception

// Macros used in the implementation of the classification functions.
#define _ismbbalnum_l(_c, pt)  ((((pt)->locinfo->_public._locale_pctype)[(unsigned char)(_c)] & \
                                (_ALPHA|_DIGIT)) || \
                                (((pt)->mbcinfo->mbctype+1)[(unsigned char)(_c)] & _MS))
#define _ismbbalpha_l(_c, pt)  ((((pt)->locinfo->_public._locale_pctype)[(unsigned char)(_c)] & \
                            (_ALPHA)) || \
                            (((pt)->mbcinfo->mbctype+1)[(unsigned char)(_c)] & _MS))
#define _ismbbgraph_l(_c, pt)  ((((pt)->locinfo->_public._locale_pctype)[(unsigned char)(_c)] & \
                            (_PUNCT|_ALPHA|_DIGIT)) || \
                            (((pt)->mbcinfo->mbctype+1)[(unsigned char)(_c)] & (_MS|_MP)))
#define _ismbbprint_l(_c, pt)  ((((pt)->locinfo->_public._locale_pctype)[(unsigned char)(_c)] & \
                            (_BLANK|_PUNCT|_ALPHA|_DIGIT)) || \
                            (((pt)->mbcinfo->mbctype + 1)[(unsigned char)(_c)] & (_MS|_MP)))
#define _ismbbpunct_l(_c, pt)  ((((pt)->locinfo->_public._locale_pctype)[(unsigned char)(_c)] & _PUNCT) || \
                                (((pt)->mbcinfo->mbctype+1)[(unsigned char)(_c)] & _MP))
#define _ismbbblank_l(_c, pt)  (((_c) == '\t') ? _BLANK : (((pt)->locinfo->_public._locale_pctype)[(unsigned char)(_c)] & _BLANK) || \
                               (((pt)->mbcinfo->mbctype+1)[(unsigned char)(_c)] & _MP))
#define _ismbblead_l(_c, p)   ((p->mbcinfo->mbctype + 1)[(unsigned char)(_c)] & _M1)
#define _ismbbtrail_l(_c, p)  ((p->mbcinfo->mbctype + 1)[(unsigned char)(_c)] & _M2)



#ifdef __cplusplus
extern "C" inline int __cdecl __dcrt_multibyte_check_type(
    unsigned int   const c,
    _locale_t      const locale,
    unsigned short const category_bits,
    bool           const expected
    )
{
    // Return false if we are not in a supported multibyte codepage:
    if (!locale->mbcinfo->ismbcodepage)
        return FALSE;

    int const code_page = locale->mbcinfo->mbcodepage;

    char const bytes[] = { (c >> 8) & 0xff, c & 0xff };

    // The 'c' "character" could be two one-byte multibyte characters, so we
    // need room in the type array to handle this.  If 'c' is two one-byte
    // multibyte characters, the second element in the type array will be
    // nonzero.
    unsigned short ctypes[2] = { };

    if (__acrt_GetStringTypeA(locale, CT_CTYPE1, bytes, _countof(bytes), ctypes, code_page, TRUE) == 0)
        return FALSE;

    // Ensure 'c' is a single multibyte character:
    if (ctypes[1] != 0)
        return FALSE;

    // Test the category:
    return static_cast<bool>((ctypes[0] & category_bits) != 0) == expected ? TRUE : FALSE;
}
#endif



_CRT_END_C_HEADER
