/* _Dtest function -- IEEE 754 version */
#include "xmath.h"

#if 0
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double *px)
	{	/* categorize *px */
	_Dval *ps = (_Dval *)(char *)px;

	if ((ps->_Sh[_D0] & _DMASK) == _DMAX << _DOFF)
		return ((short)((ps->_Sh[_D0] & _DFRAC) != 0 || ps->_Sh[_D1] != 0
			|| ps->_Sh[_D2] != 0 || ps->_Sh[_D3] != 0 ? _NANCODE : _INFCODE));
	else if ((ps->_Sh[_D0] & ~_DSIGN) != 0 || ps->_Sh[_D1] != 0
		|| ps->_Sh[_D2] != 0 || ps->_Sh[_D3] != 0)
		return ((ps->_Sh[_D0] & _DMASK) == 0 ? _DENORM : _FINITE);
	else
		return (0);
	}
#endif

unsigned short *_Plsw(double *px)
	{	/* get pointer to lsw */
	return (&((_Dval *)(char *)px)->_Sh[_Dg]);
	}

unsigned short *_Pmsw(double *px)
	{	/* get pointer to msw */
	return (&((_Dval *)(char *)px)->_Sh[_D0]);
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
