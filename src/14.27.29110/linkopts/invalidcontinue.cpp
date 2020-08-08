//
// invalidcontinue.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that sets the invalid parameter to an empty function.
//
#include <stdlib.h>
#include <vcruntime.h>



static void empty_invalid_parameter_handler(
    wchar_t const* pszExpression,
    wchar_t const* pszFunction,
    wchar_t const* pszFile,
    unsigned int   nLine,
    uintptr_t      pReserved
    ) noexcept
{
    (pszExpression);
    (pszFunction);
    (pszFile);
    (nLine);
    (pReserved);
    return;
}


#ifdef _M_CEE_PURE

    typedef void (__clrcall *_invalid_parameter_handler)(wchar_t const*, wchar_t const*, wchar_t const*, unsigned int, uintptr_t);
    typedef _invalid_parameter_handler _invalid_parameter_handler_m;
    _MRTIMP _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(_In_opt_ _invalid_parameter_handler _Handlerh);

#endif



extern "C" void _initialize_invalid_parameter_handler()
{
    _set_invalid_parameter_handler(empty_invalid_parameter_handler);
}
