/* _Stof function */
#include <yvals.h>	/* include first, for native shared */
#include <stdlib.h>
#include "xmath.h"
#include "xxcctype.h"
#include "xxfftype.h"

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _Stofx(const CTYPE *s, CTYPE **endptr, long pten,
	int *perr)
	#include "xxstod.h"

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _Stof(const CTYPE *s, CTYPE **endptr, long pten)
	{	/* convert string, discard error code */
	return (_Stofx(s, endptr, pten, 0));
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
