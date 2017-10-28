// nothrow -- define nothrow object

#ifdef CRTDLL2
#undef CRTDLL2
#endif

#ifdef MRTDLL
#undef MRTDLL
#endif

#include <new>
#include <yvals.h>
_STD_BEGIN

const nothrow_t nothrow = nothrow_t();	// define nothrow
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
