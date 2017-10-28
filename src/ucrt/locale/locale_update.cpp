//
// locale_update.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Encapsulated implementation details of _LocaleUpdate that cannot be defined
// in the header because they refer to data not exported from the AppCRT DLL.
//
#include <corecrt_internal.h>

extern "C" void __acrt_update_locale_info(
    __acrt_ptd*         const ptd,
    __crt_locale_data** const locale_info
    )
{
    if (*locale_info != __acrt_current_locale_data.value() && (ptd->_own_locale & __globallocalestatus) == 0)
    {
        *locale_info = __acrt_update_thread_locale_data();
    }
}

extern "C" void __acrt_update_multibyte_info(
    __acrt_ptd*            const ptd,
    __crt_multibyte_data** const multibyte_info
    )
{
    if (*multibyte_info != __acrt_current_multibyte_data.value() && (ptd->_own_locale & __globallocalestatus) == 0)
    {
        *multibyte_info = __acrt_update_thread_multibyte_data();
    }
}
