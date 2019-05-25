// xxdftype.h -- parameters for double floating-point type
#include <yvals.h>

#include <float.h>

#define FTYPE double
#define FCTYPE _Dcomplex
#define FBITS DBL_MANT_DIG
#define FEPS DBL_EPSILON
#define FMAX DBL_MAX
#define FMIN DBL_MIN
#define FMAXEXP DBL_MAX_EXP

#define FFUN(fun) fun
#define FMACRO(x) x
#define FNAME(fun) _##fun
#define FCONST(obj) _##obj._Double
#define FLIT(lit) lit

#define FDIV(x, y) _FDIV(x, y)
#define FINVERT(x) _FINVERT(x)

#define FPMSW(x) (*_Pmsw(&(x)))
#define FSIGNBIT _DSIGN

#define FISNEG(x) (FPMSW(x) & FSIGNBIT)
#define FMAKENEG(x) (FPMSW(x) |= FSIGNBIT)
#define FMAKEPOS(x) (FPMSW(x) &= ~FSIGNBIT)
#define FNEGATE(x) (FPMSW(x) ^= FSIGNBIT)

#define FSETLSB(x) (*_Plsw(&(x)) |= 1)

#define FCPTYPE complex<double>

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
