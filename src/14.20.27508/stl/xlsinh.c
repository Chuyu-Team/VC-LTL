// _LSinh function
#include "xmath.h"

// coefficients
#define NP (sizeof(p) / sizeof(p[0]) - 1)

static const long double p[] = { // courtesy Dr. Tim Prince
    0.0000000000000028486835L, 0.0000000000007646464279L, 0.0000000001605905091647L, 0.0000000250521083436962L,
    0.0000027557319224130455L, 0.0001984126984126956009L, 0.0083333333333333336073L, 0.1666666666666666666564L,
    1.0000000000000000000001L};

_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LSinh(long double x, long double y) { // compute y*sinh(x), |y| <= 1
    short neg;

    switch (_LDtest(&x)) { // test for special codes
    case _NANCODE:
        return x;
    case _INFCODE:
        return y != 0.0L ? x : LSIGN(x) ? -y : y;
    case 0:
        return x * y;
    default: // finite
        if (y == 0.0L) {
            return x < 0.0L ? -y : y;
        }

        if (x < 0.0L) {
            x   = -x;
            neg = 1;
        } else {
            neg = 0;
        }

        if (x < _LRteps._Long_double) {
            x *= y; // x tiny
        } else if (x < 1.0L) {
            long double w = x * x;

            x += x * w * _LPoly(w, p, NP - 1);
            x *= y;
        } else if (x < _LXbig) { // worth adding in exp(-x)
            _LExp(&x, 1.0L, -1);
            x = y * (x - 0.25L / x);
        } else {
            switch (_LExp(&x, y, -1)) { // report over/underflow
            case 0:
                _Feraise(_FE_UNDERFLOW);
                break;
            case _INFCODE:
                _Feraise(_FE_OVERFLOW);
            }
        }

        return neg ? -x : x;
    }
}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
