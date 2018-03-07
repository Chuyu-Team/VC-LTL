/* _LDint function -- IEEE 754 version */
#include "xmath.h"
_C_STD_BEGIN
 #if !defined(MRTDLL)
_C_LIB_DECL
 #endif /* defined(MRTDLL) */

 #if _DLONG == 0
short _LDint(long double *px, short xexp)
	{	/* test and drop (scaled) fraction bits -- 64-bit */
	return (_Dint((double *)px, xexp));
	}

 #elif _DLONG == 1
short _LDint(long double *px, short xexp)
	{	/* test and drop (scaled) fraction bits -- 80-bit */
	_Lval *ps = (_Lval *)(char *)px;
	short xchar = ps->_Sh[_L0] & _LMASK;

	if (xchar == _LMAX)
		return ((ps->_Sh[_L1] & 0x7fff) == 0
			&& ps->_Sh[_L2] == 0 && ps->_Sh[_L3] == 0 && ps->_Sh[_L4] == 0
			? _INFCODE : _NANCODE);
	else if (ps->_Sh[_L1] == 0 && ps->_Sh[_L2] == 0
		&& ps->_Sh[_L3] == 0 && ps->_Sh[_L4] == 0)
		return (0);
	xchar = (_LBIAS + 64) - xchar - xexp;
	if (xchar <= 0)
		return (0);	/* no frac bits to drop */
	else if (64 <= xchar)
		{	/* all frac bits */
		ps->_Sh[_L0] &= _LSIGN;
		ps->_Sh[_L1] = 0;
		ps->_Sh[_L2] = 0;
		ps->_Sh[_L3] = 0;
		ps->_Sh[_L4] = 0;
		return (_FINITE);	/* report on frac, not result */
		}
	else
		{	/* strip out frac bits */
		static const unsigned short mask[] = {
			0x0000, 0x0001, 0x0003, 0x0007,
			0x000f, 0x001f, 0x003f, 0x007f,
			0x00ff, 0x01ff, 0x03ff, 0x07ff,
			0x0fff, 0x1fff, 0x3fff, 0x7fff};
		static const size_t sub[] = {_L4, _L3, _L2, _L1};
		unsigned short frac = mask[xchar & 0xf];

		xchar >>= 4;
		frac &= ps->_Sh[sub[xchar]];
		ps->_Sh[sub[xchar]] ^= frac;
		switch (xchar)
			{	/* cascade through! */
		case 3:
			frac |= ps->_Sh[_L2], ps->_Sh[_L2] = 0;
		case 2:
			frac |= ps->_Sh[_L3], ps->_Sh[_L3] = 0;
		case 1:
			frac |= ps->_Sh[_L4], ps->_Sh[_L4] = 0;
			}
		return (frac != 0 ? _FINITE : 0);
		}
	}

 #else /* 1 < _DLONG */
short _LDint(long double *px, short xexp)
	{	/* test and drop (scaled) fraction bits -- 128-bit SPARC */
	_Lval *ps = (_Lval *)(char *)px;
	short xchar = ps->_Sh[_L0] & _LMASK;

	if (xchar == _LMAX)
		return (ps->_Sh[_L1] == 0 && ps->_Sh[_L2] == 0 && ps->_Sh[_L3] == 0
			&& ps->_Sh[_L4] == 0 && ps->_Sh[_L5] == 0 && ps->_Sh[_L6] == 0
			&& ps->_Sh[_L7] == 0
			? _INFCODE : _NANCODE);
	else if (xchar == 0 && ps->_Sh[_L1] == 0 && ps->_Sh[_L2] == 0
		&& ps->_Sh[_L3] == 0 && ps->_Sh[_L4] == 0 && ps->_Sh[_L5] == 0
		&& ps->_Sh[_L6] == 0 && ps->_Sh[_L7] == 0)
		return (0);
	xchar = (_LBIAS + 112 + 1) - xchar - xexp;
	if (xchar <= 0)
		return (0);	/* no frac bits to drop */
	else if ((112 + 1) <= xchar)
		{	/* all frac bits */
		ps->_Sh[_L0] &= _LSIGN;
		ps->_Sh[_L1] = 0;
		ps->_Sh[_L2] = 0;
		ps->_Sh[_L3] = 0;
		ps->_Sh[_L4] = 0;
		ps->_Sh[_L5] = 0;
		ps->_Sh[_L6] = 0;
		ps->_Sh[_L7] = 0;
		return (_FINITE);	/* report on frac, not result */
		}
	else
		{	/* strip out frac bits */
		static const unsigned short mask[] = {
			0x0000, 0x0001, 0x0003, 0x0007,
			0x000f, 0x001f, 0x003f, 0x007f,
			0x00ff, 0x01ff, 0x03ff, 0x07ff,
			0x0fff, 0x1fff, 0x3fff, 0x7fff};
		static const size_t sub[] =
			{_L7, _L6, _L5, _L4, _L3, _L2, _L1, _L0};
		unsigned short frac = mask[xchar & 0xf];

		xchar >>= 4;
		frac &= ps->_Sh[sub[xchar]];
		ps->_Sh[sub[xchar]] ^= frac;
		switch (xchar)
			{	/* cascade through! */
		case 7:
			frac |= ps->_Sh[_L1], ps->_Sh[_L1] = 0;
		case 6:
			frac |= ps->_Sh[_L2], ps->_Sh[_L2] = 0;
		case 5:
			frac |= ps->_Sh[_L3], ps->_Sh[_L3] = 0;
		case 4:
			frac |= ps->_Sh[_L4], ps->_Sh[_L4] = 0;
		case 3:
			frac |= ps->_Sh[_L5], ps->_Sh[_L5] = 0;
		case 2:
			frac |= ps->_Sh[_L6], ps->_Sh[_L6] = 0;
		case 1:
			frac |= ps->_Sh[_L7], ps->_Sh[_L7] = 0;
			}
		return (frac != 0 ? _FINITE : 0);
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
