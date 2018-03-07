/* _Stolx function */
#include <yvals.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "xmath.h"
_C_STD_BEGIN

#if defined(__cplusplus) && !defined(MRTDLL)
extern "C"
#endif
_CRTIMP2_PURE unsigned long __CLRCALL_PURE_OR_CDECL _Stoulx(const char *, char **, int, int *);

#if defined(__cplusplus) && !defined(MRTDLL)
extern "C"
#endif
_CRTIMP2_PURE long __CLRCALL_PURE_OR_CDECL _Stolx(const char * s, char ** endptr,
	int base, int *perr)
	{	/* convert string to long, with checking */
	const char *sc;
	char *se, sign;
	unsigned long x;

	if (endptr == 0)
		endptr = &se;
	for (sc = s; isspace((unsigned char)*sc); ++sc)
		;
	sign = *sc == '-' || *sc == '+' ? *sc++ : '+';
	x = _Stoulx(sc, endptr, base, perr);
	if (sc == *endptr)
		*endptr = (char *)s;
	if (s == *endptr && x != 0 || sign == '+' && LONG_MAX < x
		|| sign == '-' && 0 - (unsigned long)LONG_MIN < x)
		{	/* overflow */
		errno = ERANGE;
		if (perr != 0)
			*perr = 1;
		return (sign == '-' ? LONG_MIN : LONG_MAX);
		}
	else
		return ((long)(sign == '-' ? 0 - x : x));
	}
_C_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
