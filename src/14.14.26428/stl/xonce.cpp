/* xonce.cpp -- _Execute_once function */
#include <mutex>

#include <awint.h>

_STD_BEGIN
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Execute_once(
	once_flag& _Flag, _Execute_once_fp_t _Callback, void *_Pv) noexcept
	{	// wrap Win32 InitOnceExecuteOnce()
	static_assert(sizeof(_Flag._Opaque) == sizeof(INIT_ONCE), "invalid size");

	return (__crtInitOnceExecuteOnce(
		reinterpret_cast<PINIT_ONCE>(&_Flag._Opaque),
		reinterpret_cast<PINIT_ONCE_FN>(_Callback),
		_Pv, 0));
	}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _XGetLastError()
	{	// throw system_error containing GetLastError()
	error_code _Code(static_cast<int>(GetLastError()),
		_STD system_category());
	_THROW(system_error(_Code));
	}
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
