/* xmath.h internal header */
#ifndef _XMATH
#define _XMATH
#include <ymath.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>

 #pragma warning(disable: 4244)

		/* FLOAT PROPERTIES */
#ifndef _D0
 #define _D0	3	/* little-endian, small long doubles */
 #define _D1	2
 #define _D2	1
 #define _D3	0

 #define _DBIAS	0x3fe
 #define _DOFF	4

 #define _FBIAS	0x7e
 #define _FOFF	7
 #define _FRND	1

 #define _DLONG	0
 #define _LBIAS	0x3fe
 #define _LOFF	4

#elif _D0 == 0		/* other params defined in <yvals.h> */
 #define _D1	1	/* big-endian */
 #define _D2	2
 #define _D3	3

#else /* _D0 */
 #define _D1	2	/* little-endian */
 #define _D2	1
 #define _D3	0
#endif /* _D0 */

		/* IEEE 754 double properties */
#define _DFRAC	((unsigned short)((1 << _DOFF) - 1))
#define _DMASK	((unsigned short)(0x7fff & ~_DFRAC))
#define _DMAX	((unsigned short)((1 << (15 - _DOFF)) - 1))
#define _DSIGN	((unsigned short)0x8000)

#define HUGE_EXP	(int)(_DMAX * 900L / 1000)

		/* IEEE 754 float properties */
#define _FFRAC	((unsigned short)((1 << _FOFF) - 1))
#define _FMASK	((unsigned short)(0x7fff & ~_FFRAC))
#define _FMAX	((unsigned short)((1 << (15 - _FOFF)) - 1))
#define _FSIGN	((unsigned short)0x8000)

#define FHUGE_EXP	(int)(_FMAX * 900L / 1000)

 #if _D0 == 0
  #define _F0	0	/* big-endian */
  #define _F1	1

 #else /* _D0 == 0 */
  #define _F0	1	/* little-endian */
  #define _F1	0
 #endif /* _D0 == 0 */

		/* IEEE 754 long double properties */
#define _LFRAC	((unsigned short)(-1))
#define _LMASK	((unsigned short)0x7fff)
#define _LMAX	((unsigned short)0x7fff)
#define _LSIGN	((unsigned short)0x8000)

#define LHUGE_EXP	(int)(_LMAX * 900L / 1000)

 #if _D0 == 0
  #define _L0	0	/* big-endian */
  #define _L1	1
  #define _L2	2
  #define _L3	3
  #define _L4	4
  #define _L5	5	/* 128-bit only */
  #define _L6	6
  #define _L7	7
  #define _Lg	_L7

 #elif _DLONG == 0
  #define _L0	3	/* little-endian, 64-bit long doubles */
  #define _L1	2
  #define _L2	1
  #define _L3	0
  #define _Lg	_L3
  #define _L4	xxx	/* should never be used */
  #define _L5	xxx
  #define _L6	xxx
  #define _L7	xxx

 #elif _DLONG == 1
  #define _L0	4	/* little-endian, 80-bit long doubles */
  #define _L1	3
  #define _L2	2
  #define _L3	1
  #define _L4	0
  #define _Lg	_L4
  #define _L5	xxx	/* should never be used */
  #define _L6	xxx
  #define _L7	xxx

 #else /* _DLONG */
  #define _L0	7	/* little-endian, 128-bit long doubles */
  #define _L1	6
  #define _L2	5
  #define _L3	4
  #define _L4	3
  #define _L5	2
  #define _L6	1
  #define _L7	0
  #define _Lg	_L7
 #endif /* _DLONG */

#define DSIGN(x)	(((_Dval *)(char *)&(x))->_Sh[_D0] & _DSIGN)
#define FSIGN(x)	(((_Fval *)(char *)&(x))->_Sh[_F0] & _FSIGN)
#define LSIGN(x)	(((_Lval *)(char *)&(x))->_Sh[_L0] & _LSIGN)

 #define _Fg	_F1	/* least-significant 16-bit word */
 #define _Dg	_D3

		/* return values for _Stopfx/_Stoflt */
#define FL_ERR	0
#define FL_DEC	1
#define FL_HEX	2
#define FL_INF	3
#define FL_NAN	4
#define FL_NEG	8

_C_STD_BEGIN
 #if !defined(MRTDLL) && !defined(_M_CEE_PURE)
_C_LIB_DECL
 #endif /* defined(MRTDLL) etc. */

int _Stopfx(const char **, char **);
int _Stoflt(const char *, const char *, char **,
	long[], int);
int _Stoxflt(const char *, const char *, char **,
	long[], int);
int _WStopfx(const wchar_t **, wchar_t **);
int _WStoflt(const wchar_t *, const wchar_t *, wchar_t **,
	long[], int);
int _WStoxflt(const wchar_t *, const wchar_t *, wchar_t **,
	long[], int);

		/* double declarations */
typedef union
	{	/* pun floating type as integer array */
	unsigned short _Sh[8];
	double _Val;
	} _Dval;

unsigned short *_Plsw(double *);
unsigned short *_Pmsw(double *);

short _Dint(double *, short);
short _Dnorm(_Dval *);
short _Dscale(double *, long);
short _Dunscale(short *, double *);

double _Poly(double, const double *, int);

extern /* const */ _Dconst _Eps, _Rteps;
extern /* const */ double _Xbig, _Zero;

double _Xp_getw(const double *, int);
double *_Xp_setn(double *, int, long);
double *_Xp_setw(double *, int, double);
double *_Xp_addh(double *, int, double);
double *_Xp_mulh(double *, int, double);
double *_Xp_movx(double *, int, const double *);
double *_Xp_addx(double *, int,
	const double *, int);
double *_Xp_subx(double *, int,
	const double *, int);
double *_Xp_ldexpx(double *, int, int);
double *_Xp_mulx(double *, int,
	const double *, int, double *);
double *_Xp_invx(double *, int, double *);
double *_Xp_sqrtx(double *, int, double *);

		/* float declarations */
typedef union
	{	/* pun floating type as integer array */
	unsigned short _Sh[8];
	float _Val;
	} _Fval;

unsigned short *_FPlsw(float *);
unsigned short *_FPmsw(float *);

short _FDint(float *, short);
short _FDnorm(_Fval *);
short _FDscale(float *, long);
short _FDunscale(short *, float *);

float _FPoly(float, const float *, int);

extern /* const */ _Dconst _FEps, _FRteps;
extern /* const */ float _FXbig, _FZero;

float _FXp_getw(const float *, int);
float *_FXp_setn(float *, int, long);
float *_FXp_setw(float *, int, float);
float *_FXp_addh(float *, int, float);
float *_FXp_mulh(float *, int, float);
float *_FXp_movx(float *, int, const float *);
float *_FXp_addx(float *, int,
	const float *, int);
float *_FXp_subx(float *, int,
	const float *, int);
float *_FXp_ldexpx(float *, int, int);
float *_FXp_mulx(float *, int,
	const float *, int, float *);
float *_FXp_invx(float *, int, float *);
float *_FXp_sqrtx(float *, int, float *);

		/* long double declarations */
typedef union
	{	/* pun floating type as integer array */
	unsigned short _Sh[8];
	long double _Val;
	} _Lval;

#define LISNEG(x)	(*_LPmsw(&(x)) & _LSIGN)
#define LMAKEPOS(x)	(*_LPmsw(&(x)) &= ~_LSIGN)
#define LNEGATE(x)	(*_LPmsw(&(x)) ^= _LSIGN)

unsigned short *_LPlsw(long double *);
unsigned short *_LPmsw(long double *);

short _LDint(long double *, short);
short _LDnorm(_Lval *);
short _LDscale(long double *, long);
short _LDunscale(short *, long double *);
long double _LPoly(long double, const long double *, int);

extern /* const */ _Dconst _LEps, _LRteps;
extern /* const */ long double _LXbig, _LZero;

long double _LXp_getw(const long double *, int);
long double *_LXp_setn(long double *, int, long);
long double *_LXp_setw(long double *, int, long double);
long double *_LXp_addh(long double *, int, long double);
long double *_LXp_mulh(long double *, int, long double);
long double *_LXp_movx(long double *, int,
	const long double *);
long double *_LXp_addx(long double *, int,
	const long double *, int);
long double *_LXp_subx(long double *, int,
	const long double *, int);
long double *_LXp_ldexpx(long double *, int, int);
long double *_LXp_mulx(long double *, int,
	const long double *, int, long double *);
long double *_LXp_invx(long double *, int, long double *);
long double *_LXp_sqrtx(long double *, int, long double *);

 #if !defined(MRTDLL) && !defined(_M_CEE_PURE)
_END_C_LIB_DECL
 #endif /* !defined(MRTDLL) etc. */
_C_STD_END

 #if !defined(_FINVERT)
  #define _FINVERT(x)	(FLIT(1.0) / (x))
 #endif /* !defined(_FINVERT) */

 #if !defined(_FDIV)
  #define _FDIV(x, y)	((x) / (y))
 #endif /* !defined(_FDIV) */
#endif /* _XMATH */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
