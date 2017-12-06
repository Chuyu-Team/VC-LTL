/* _LPoly function */
#include "xmath.h"

long double _LPoly(long double x,
	const long double *tab, int n)
	{	/* compute polynomial */
	long double y;

	for (y = *tab; 0 <= --n; )
		y = y * x + *++tab;
	return (y);
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
