/* _WStof function */
#include <wchar.h>
#include "xmath.h"
#include "xxwctype.h"
#include "xxfftype.h"

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStofx(const CTYPE *s, CTYPE **endptr, long pten,
	int *perr)
	#include "xxstod.h"

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStof(const CTYPE *s, CTYPE **endptr, long pten)
	{	/* convert string, discard error code */
	return (_WStofx(s, endptr, pten, 0));
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
