// handler.cpp -- set_new_handler for Microsoft
#include <new>

typedef int (__cdecl *new_hand)(size_t);
extern "C" new_hand __cdecl _set_new_handler(new_hand);

_STD_BEGIN
static new_handler _New_handler;

int __cdecl _New_handler_interface(size_t)
	{	// interface to existing Microsoft _callnewh mechanism
	_New_handler();
	return (1);
	}

_CRTIMP2 new_handler __cdecl set_new_handler(new_handler pnew) _THROW0()
	{	// remove current handler
	_BEGIN_LOCK(_LOCK_MALLOC)	// lock thread to ensure atomicity
		new_handler pold = _New_handler;
		_New_handler = pnew;
		_set_new_handler(pnew ? _New_handler_interface : 0);
		return (pold);
	_END_LOCK()
	}

_CRTIMP2 new_handler __cdecl get_new_handler() _THROW0()
	{	// get current new handler
	_BEGIN_LOCK(_LOCK_MALLOC)	// lock thread to ensure atomicity
		return (_New_handler);
	_END_LOCK()
	}

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
