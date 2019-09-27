// xxfftype.h -- parameters for float floating-point type
#include <yvals.h>

#include <float.h>

#define FTYPE float
#define FCTYPE _Fcomplex
#define FBITS FLT_MANT_DIG
#define FEPS FLT_EPSILON
#define FMAX FLT_MAX
#define FMIN FLT_MIN
#define FMAXEXP FLT_MAX_EXP

#define FFUN(fun) fun##f
#define FMACRO(x) F##x
#define FNAME(fun) _F##fun
#define FCONST(obj) _F##obj._Float
#define FLIT(lit) lit##F

#define FDIV(x, y) _FDIV(x, y)
#define FINVERT(x) _FINVERT(x)

#define FPMSW(x) (*_FPmsw(&(x)))
#define FSIGNBIT _FSIGN

#define FISNEG(x) (FPMSW(x) & FSIGNBIT)
#define FMAKENEG(x) (FPMSW(x) |= FSIGNBIT)
#define FMAKEPOS(x) (FPMSW(x) &= ~FSIGNBIT)
#define FNEGATE(x) (FPMSW(x) ^= FSIGNBIT)

#define FSETLSB(x) (*_FPlsw(&(x)) |= 1)

#define FCPTYPE complex<float>

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
