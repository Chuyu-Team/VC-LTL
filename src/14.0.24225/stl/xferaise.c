/* _Feraise function */
#include <yvals.h>
#include <errno.h>
#include <ymath.h>
_C_STD_BEGIN

#ifdef _Feraise
#undef _Feraise
#endif
void __CLRCALL_PURE_OR_CDECL _Feraise(int except)
	{	/* report floating-point exception */
	if ((except & (_FE_DIVBYZERO | _FE_INVALID)) != 0)
		errno = EDOM;
	else if ((except & (_FE_UNDERFLOW | _FE_OVERFLOW)) != 0)
		errno = ERANGE;
	}
_C_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
