//
// stdio.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The C Standard Library <wctype.h> header.
//
#pragma once
#define _INC_WCTYPE

#include <corecrt.h>
#include <corecrt_wctype.h>

_CRT_BEGIN_C_HEADER



typedef wchar_t wctrans_t;
_ACRTIMP wint_t __cdecl towctrans(wint_t c, wctrans_t value);
_ACRTIMP wctrans_t __cdecl wctrans(const char *name);
_ACRTIMP wctype_t __cdecl wctype(const char *name);



_CRT_END_C_HEADER
