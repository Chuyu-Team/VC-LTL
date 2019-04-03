// _LDunscale function -- IEEE 754 version
#include "xmath.h"

short _LDunscale(short* pex, long double* px) { // separate *px to 1/2 <= |frac| < 1 and 2^*pex -- 64-bit
    return _Dunscale(pex, (double*) px);
}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
