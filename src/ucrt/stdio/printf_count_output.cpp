//
// printf_count_output.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines the functions that control the enable state of %n.
//
#include <corecrt_internal_stdio.h>
#include <msvcrt_IAT.h>


static UINT_PTR enable_percent_n = 0;



// Enables or disables the %n format specifier for the printf family of functions.
// Note the use of the __security_cookie:  if the static variable was set to a
// known value, an attacker could potentially modify that value and then provide
// a malicious %n specifier.  The cookie is or'ed with 1 becuase a cookie with a
// value of zero is possible.
extern "C" int __cdecl _set_printf_count_output_downlevel(int const value)
{
    int const old = (enable_percent_n == (__security_cookie | 1));
    enable_percent_n = (value ? (__security_cookie | 1) : 0);
    return old;
}

_LCRT_DEFINE_IAT_SYMBOL(_set_printf_count_output_downlevel);

// Tests whether the %n format specifier for the printf family of functions is
// enabled.
extern "C" int __cdecl _get_printf_count_output_downlevel()
{
    return enable_percent_n == (__security_cookie | 1);
}

_LCRT_DEFINE_IAT_SYMBOL(_get_printf_count_output_downlevel);
