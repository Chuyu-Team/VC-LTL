/* values used by math functions -- IEEE 754 long version */
#if defined(_M_CEE_PURE)
#if defined(MRTDLL)
#undef MRTDLL
#endif
#if defined(MRTDLL)
#undef CRTDLL
#endif
#endif

#include "xmath.h"
		/* macros -- 64-bit */
#define NBITS	(48 + _DOFF)

#define INIT(w0)		{0, 0, 0, w0}
#define INIT2(w0, w1)	{w1, 0, 0, w0}

		/* static data */
extern /* const */ _Dconst _LDenorm = {INIT2(0, 1)};
extern /* const */ _Dconst _LEps = {
	INIT((_DBIAS - NBITS - 1) << _DOFF)};
extern /* const */ _Dconst _LInf = {INIT(_DMAX << _DOFF)};
extern /* const */ _Dconst _LNan = {INIT((_DMAX << _DOFF)
	| (1 << (_DOFF - 1)))};
extern /* const */ _Dconst _LSnan = {INIT2(_DMAX << _DOFF, 1)};
extern /* const */ _Dconst _LRteps = {
	INIT((_DBIAS - NBITS / 2) << _DOFF)};

extern /* const */ long double _LXbig = (NBITS + 1) * 347L / 1000;
extern /* const */ long double _LZero = 0.0L;

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
