// _LDint function -- IEEE 754 version
#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _LDint(long double* px, short xexp) { // test and drop (scaled) fraction bits -- 64-bit
    return _Dint((double*) px, xexp);
}

_END_EXTERN_C_UNLESS_PURE

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
