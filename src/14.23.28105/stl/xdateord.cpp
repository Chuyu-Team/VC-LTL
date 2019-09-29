// xdateord -- look up date ordering for Microsoft
#include <awint.h>
#include <locale>

#if !defined(MRTDLL) && !defined(_M_CEE_PURE)
extern "C"
#endif
    int __CLRCALL_PURE_OR_CDECL
    _Getdateorder() { // return date order for current locale
    wchar_t buf[2] = {0};
    //__crtGetLocaleInfoEx(___lc_locale_name_func()[LC_TIME], LOCALE_ILDATE, buf, sizeof(buf) / sizeof(buf[0]));
    GetLocaleInfoW(___lc_handle_func()[LC_TIME], LOCALE_ILDATE, buf, sizeof(buf) / sizeof(buf[0]));
    return buf[0] == L'0'
               ? std::time_base::mdy
               : buf[0] == L'1' ? std::time_base::dmy : buf[0] == L'2' ? std::time_base::ymd : std::time_base::no_order;
}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
