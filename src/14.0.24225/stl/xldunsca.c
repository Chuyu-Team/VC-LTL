/* _LDunscale function -- IEEE 754 version */
#include "xmath.h"
_C_STD_BEGIN
 #if !defined(MRTDLL)
_C_LIB_DECL
 #endif /* defined(MRTDLL) */

 #if _DLONG == 0
short _LDunscale(short *pex, long double *px)
	{	/* separate *px to 1/2 <= |frac| < 1 and 2^*pex -- 64-bit */
	return (_Dunscale(pex, (double *)px));
	}

 #elif _DLONG == 1
short _LDunscale(short *pex, long double *px)
	{	/* separate *px to 1/2 <= |frac| < 1 and 2^*pex -- 80-bit */
	_Lval *ps = (_Lval *)(char *)px;
	short xchar = ps->_Sh[_L0] & _LMASK;

	if (xchar == _LMAX)
		{	/* NaN or INF */
		*pex = 0;
		return ((ps->_Sh[_L1] & 0x7fff) != 0 || ps->_Sh[_L2] != 0
			|| ps->_Sh[_L3] != 0 || ps->_Sh[_L4] != 0 ? _NANCODE : _INFCODE);
		}
	else if (ps->_Sh[_L1] != 0 || ps->_Sh[_L2] != 0
		|| ps->_Sh[_L3] != 0 || ps->_Sh[_L4] != 0)
		{	/* finite, reduce to [1/2, 1) */
		if (xchar == 0)
			xchar = 1;	/* correct denormalized exponent */
		xchar += _LDnorm(ps);
		ps->_Sh[_L0] = (ps->_Sh[_L0] & _LSIGN) | _LBIAS;
		*pex = xchar - _LBIAS;
		return (_FINITE);
		}
	else
		{	/* zero */
		*pex = 0;
		return (0);
		}
	}

 #else	/* 1 < _DLONG */
short _LDunscale(short *pex, long double *px)
	{	/* separate *px to 1/2 <= |frac| < 1 and 2^*pex -- 128-bit SPARC */
	_Lval *ps = (_Lval *)(char *)px;
	short xchar = ps->_Sh[_L0] & _LMASK;

	if (xchar == _LMAX)
		{	/* NaN or INF */
		*pex = 0;
		return (ps->_Sh[_L1] != 0 || ps->_Sh[_L2] != 0 || ps->_Sh[_L3] != 0
			|| ps->_Sh[_L4] != 0 || ps->_Sh[_L5] != 0 || ps->_Sh[_L6] != 0
			|| ps->_Sh[_L7] != 0 ? _NANCODE : _INFCODE);
		}
	else if (0 < xchar || (xchar = _LDnorm(ps)) <= 0)
		{	/* finite, reduce to [1/2, 1) */
		ps->_Sh[_L0] = (ps->_Sh[_L0] & _LSIGN) | _LBIAS;
		*pex = xchar - _LBIAS;
		return (_FINITE);
		}
	else
		{	/* zero */
		*pex = 0;
		return (0);
		}
	}
 #endif /* _DLONG */

 #if !defined(MRTDLL)
_END_C_LIB_DECL
 #endif /* !defined(MRTDLL) */
_C_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
