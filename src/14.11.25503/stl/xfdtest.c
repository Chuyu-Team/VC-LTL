/* _FDtest function -- IEEE 754 version */
#include "xmath.h"

#if 0
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float *px)
	{	/* categorize *px */
	_Fval *ps = (_Fval *)(char *)px;

	if ((ps->_Sh[_F0] & _FMASK) == _FMAX << _FOFF)
		return ((short)((ps->_Sh[_F0] & _FFRAC) != 0 || ps->_Sh[_F1] != 0
			? _NANCODE : _INFCODE));
	else if ((ps->_Sh[_F0] & ~_FSIGN) != 0 || ps->_Sh[_F1] != 0)
		return ((ps->_Sh[_F0] & _FMASK) == 0 ? _DENORM : _FINITE);
	else
		return (0);
	}
#endif

unsigned short *_FPlsw(float *px)
	{	/* get pointer to lsw */
	return (&((_Fval *)(char *)px)->_Sh[_Fg]);
	}

unsigned short *_FPmsw(float *px)
	{	/* get pointer to msw */
	return (&((_Fval *)(char *)px)->_Sh[_F0]);
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
