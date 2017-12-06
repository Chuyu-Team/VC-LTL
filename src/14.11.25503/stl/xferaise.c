/* _Feraise function */
#include <yvals.h>
#include <errno.h>
#include <ymath.h>

void __CLRCALL_PURE_OR_CDECL _Feraise(int except)
	{	/* report floating-point exception */
	if ((except & (_FE_DIVBYZERO | _FE_INVALID)) != 0)
		errno = EDOM;
	else if ((except & (_FE_UNDERFLOW | _FE_OVERFLOW)) != 0)
		errno = ERANGE;
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
