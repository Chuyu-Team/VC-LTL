//
// legacy_conio_definitions.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Provides external definitions of the inline conio printf and scanf functions
// for use by objects compiled with older versions of the CRT headers.
//

// Our references to "unsafe" functions count as "use" so we need to suppress
// the deprecation warnings.  Note that any clients using these symbols will
// also get these deprecation warnings.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _NO_CRT_STDIO_INLINE 1
#define _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS
#define _CRT_STDIO_LEGACY_WIDE_SPECIFIERS

#define _INC_SWPRINTF_INL_
#define _CRT_STDIO_INLINE extern
#include <conio.h>
#include <sdkddkver.h>


// Define the __imp_- or _imp__-prefixed IAT symbols, for compatibility with
// objects that were compiled with /MD or /MDd.
#include <vcstartup_internal.h>

#undef _NO_CRT_STDIO_INLINE


#if _CRT_NTDDI_MIN < NTDDI_WIN6

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cprintf_l(
        _In_z_ _Printf_format_string_params_(0) char const* const _Format,
        _In_opt_                                _locale_t   const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcprintf_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cprintf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cprintf_p(
        _In_z_ _Printf_format_string_ char const* const _Format,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Format);
        _Result = _vcprintf_p_l(_Format, NULL, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cprintf_p);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cprintf_p_l(
        _In_z_ _Printf_format_string_params_(0) char const* const _Format,
        _In_opt_                                _locale_t   const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcprintf_p_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cprintf_p_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cprintf_s(
        _In_z_ _Printf_format_string_ char const* const _Format,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Format);
        _Result = _vcprintf_s_l(_Format, NULL, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cprintf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cprintf_s_l(
        _In_z_ _Printf_format_string_params_(0) char const* const _Format,
        _In_opt_                                _locale_t   const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcprintf_s_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cprintf_s_l);

    _Check_return_opt_ _CRT_INSECURE_DEPRECATE(_cscanf_s_l)
    _CRT_STDIO_INLINE int __CRTDECL _cscanf_l(
        _In_z_ _Scanf_format_string_params_(0) char const* const _Format,
        _In_opt_                               _locale_t   const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);

        #pragma warning(push)
        #pragma warning(disable: 4996) // Deprecation
        _Result = _vcscanf_l(_Format, _Locale, _ArgList);
        #pragma warning(pop)

        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cscanf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cscanf_s(
        _In_z_ _Scanf_format_string_ char const* const _Format,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Format);
        _Result = _vcscanf_s_l(_Format, NULL, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cscanf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cscanf_s_l(
        _In_z_ _Scanf_format_string_params_(0) char const* const _Format,
        _In_opt_                               _locale_t   const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcscanf_s_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cscanf_s_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwprintf_l(
        _In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
        _In_opt_                                _locale_t      const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcwprintf_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwprintf_p(
        _In_z_ _Printf_format_string_ wchar_t const* const _Format,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Format);
        _Result = _vcwprintf_p_l(_Format, NULL, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_p);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwprintf_p_l(
        _In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
        _In_opt_                                _locale_t      const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcwprintf_p_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_p_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwprintf_s(
        _In_z_ _Printf_format_string_ wchar_t const* const _Format,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Format);
        _Result = _vcwprintf_s_l(_Format, NULL, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwprintf_s_l(
        _In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
        _In_opt_                                _locale_t      const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcwprintf_s_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_s_l);

    _Check_return_opt_ _CRT_INSECURE_DEPRECATE(_cwscanf_s_l)
    _CRT_STDIO_INLINE int __CRTDECL _cwscanf_l(
        _In_z_ _Scanf_format_string_params_(0) wchar_t const* const _Format,
        _In_opt_                               _locale_t      const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);

        #pragma warning(push)
        #pragma warning(disable: 4996) // Deprecation
        _Result = _vcwscanf_l(_Format, _Locale, _ArgList);
        #pragma warning(pop)

        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwscanf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwscanf_s(
        _In_z_ _Scanf_format_string_ wchar_t const* const _Format,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Format);
        _Result = _vcwscanf_s_l(_Format, NULL, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwscanf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _cwscanf_s_l(
        _In_z_ _Scanf_format_string_params_(0) wchar_t const* const _Format,
        _In_opt_                               _locale_t      const _Locale,
        ...)
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        int _Result;
        va_list _ArgList;
        __crt_va_start(_ArgList, _Locale);
        _Result = _vcwscanf_s_l(_Format, _Locale, _ArgList);
        __crt_va_end(_ArgList);
        return _Result;
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_cwscanf_s_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcprintf_l(
        _In_z_ _Printf_format_string_params_(2) char const* const _Format,
        _In_opt_                                _locale_t   const _Locale,
                                                va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcprintf(
            _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcprintf_p(
        _In_z_ char const* const _Format,
               va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return _vcprintf_p_l(_Format, NULL, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_p);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcprintf_p_l(
        _In_z_ _Printf_format_string_params_(2) char const* const _Format,
        _In_opt_                                _locale_t   const _Locale,
                                                va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcprintf_p(
            _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_p_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcprintf_s(
        _In_z_ _Printf_format_string_ char const* const _Format,
                                      va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return _vcprintf_s_l(_Format, NULL, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcprintf_s_l(
        _In_z_ _Printf_format_string_params_(2) char const* const _Format,
        _In_opt_                                _locale_t   const _Locale,
                                                va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcprintf_s(
            _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_s_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwprintf_l(
        _In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
        _In_opt_                                _locale_t      const _Locale,
                                                va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwprintf_p(
        _In_z_ _Printf_format_string_ const wchar_t* const _Format,
                                      va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return _vcwprintf_p_l(_Format, NULL, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_p);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwprintf_p_l(
        _In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
        _In_opt_                                _locale_t      const _Locale,
                                                va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcwprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_p_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwprintf_s(
        _In_z_ _Printf_format_string_ wchar_t const* const _Format,
                                      va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return _vcwprintf_s_l(_Format, NULL, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwprintf_s_l(
        _In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
        _In_opt_                                _locale_t      const _Locale,
                                                va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcwprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_s_l);
#endif

    _Check_return_opt_ _CRT_INSECURE_DEPRECATE(_vcscanf_s)
    _CRT_STDIO_INLINE int __CRTDECL _vcscanf(
        _In_z_ _Scanf_format_string_params_(1) char const* const _Format,
                                               va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        #pragma warning(push)
        #pragma warning(disable: 4996) // Deprecation
        return _vcscanf_l(_Format, NULL, _ArgList);
        #pragma warning(pop)
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcscanf);

    _Check_return_opt_ _CRT_INSECURE_DEPRECATE(_vcscanf_s_l)
    _CRT_STDIO_INLINE int __CRTDECL _vcscanf_l(
        _In_z_ _Scanf_format_string_params_(2) char const* const _Format,
        _In_opt_                               _locale_t   const _Locale,
                                               va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcscanf(
            _CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcscanf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcscanf_s(
        _In_z_ _Scanf_format_string_params_(1) char const* const _Format,
                                               va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return _vcscanf_s_l(_Format, NULL, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcscanf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcscanf_s_l(
        _In_z_ _Scanf_format_string_params_(2) char const* const _Format,
        _In_opt_                               _locale_t   const _Locale,
                                               va_list           _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcscanf(
            _CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcscanf_s_l);

    _Check_return_opt_ _CRT_INSECURE_DEPRECATE(_vcwscanf_s)
    _CRT_STDIO_INLINE int __CRTDECL _vcwscanf(
        _In_z_ _Scanf_format_string_params_(1) wchar_t const* const _Format,
                                               va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        #pragma warning(push)
        #pragma warning(disable: 4996) // Deprecation
        return _vcwscanf_l(_Format, NULL, _ArgList);
        #pragma warning(pop)
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf);

    _Check_return_opt_ _CRT_INSECURE_DEPRECATE(_vcwscanf_s_l)
    _CRT_STDIO_INLINE int __CRTDECL _vcwscanf_l(
        _In_z_ _Scanf_format_string_params_(2) wchar_t const* const _Format,
        _In_opt_                               _locale_t      const _Locale,
                                               va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcwscanf(
            _CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf_l);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwscanf_s(
        _In_z_ _Scanf_format_string_params_(1) wchar_t const* const _Format,
                                               va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return _vcwscanf_s_l(_Format, NULL, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf_s);

    _Check_return_opt_
    _CRT_STDIO_INLINE int __CRTDECL _vcwscanf_s_l(
        _In_z_ _Scanf_format_string_params_(2) wchar_t const* const _Format,
        _In_opt_                               _locale_t      const _Locale,
                                               va_list              _ArgList
        )
#if defined _NO_CRT_STDIO_INLINE
;
#else
    {
        return __conio_common_vcwscanf(
            _CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
            _Format, _Locale, _ArgList);
    }
#endif
	_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf_s_l);
