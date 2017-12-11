// instances -- force DLL instances for Microsoft
#define __FORCE_INSTANCE

#include <ios>
#include <istream>
#include <locale>
#include <ostream>
#include <streambuf>
#include <xlocale>
#include <xlocmes>
#include <xlocmon>
#include <xlocnum>
#include <xloctime>

_STD_BEGIN

template class _CRTIMP2_PURE _Yarn<char>;
template class _CRTIMP2_PURE _Yarn<wchar_t>;
template class _CRTIMP2_PURE _Yarn<unsigned short>;

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
