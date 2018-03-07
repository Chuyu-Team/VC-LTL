// future.cpp -- future static objects
#include <cstdlib>

 #include <future>
 #include <exception>
_STD_BEGIN

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Throw_future_error(const error_code& _Code)
	{	// throw an exception
	_THROW(future_error(_Code));
	}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Rethrow_future_exception(
	exception_ptr _Ptr)
	{	// rethrow an exception
	_STD rethrow_exception(_Ptr);
	}

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
