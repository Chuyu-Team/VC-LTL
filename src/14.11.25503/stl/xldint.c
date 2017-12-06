/* _LDint function -- IEEE 754 version */
#include "xmath.h"

short _LDint(long double *px, short xexp)
	{	/* test and drop (scaled) fraction bits -- 64-bit */
	return (_Dint((double *)px, xexp));
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
