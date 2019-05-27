// _LDtest function -- IEEE 754 version
#include "xmath.h"

#if 0
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDtest(long double* px) { // categorize *px -- 64-bit
    return _Dtest((double*) px);
}
#endif

unsigned short* _LPlsw(long double* px) { // get pointer to lsw
    return &((_Lval*) (char*) px)->_Sh[_L3];
}

unsigned short* _LPmsw(long double* px) { // get pointer to msw
    return &((_Lval*) (char*) px)->_Sh[_L0];
}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
