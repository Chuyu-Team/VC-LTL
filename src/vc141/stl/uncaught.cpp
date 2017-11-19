// uncaught -- uncaught_exception for Microsoft

  #include <eh.h>
  #include <exception>
_STD_BEGIN

_CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL uncaught_exception() _NOEXCEPT
	{	// report if handling a throw
	return (__uncaught_exception());
	}

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL uncaught_exceptions() _NOEXCEPT
	{	// report number of throws being handled
	return (__uncaught_exceptions());
	}
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
