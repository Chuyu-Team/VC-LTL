// xstring_insert.h internal header
// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once
#ifndef _XSTRING_INSERT_H
#define _XSTRING_INSERT_H
#ifndef RC_INVOKED

#include <ostream>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,_STL_WARNING_LEVEL)
#pragma warning(disable: _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

template<class _Elem,
	class _Traits,
	class _SizeT> inline
	basic_ostream<_Elem, _Traits>& _Insert_string(
		basic_ostream<_Elem, _Traits>& _Ostr,
		const _Elem * const _Data, const _SizeT _Size)
	{	// insert a character-type sequence into _Ostr as if through a basic_string copy
	ios_base::iostate _State = ios_base::goodbit;
	_SizeT _Pad = _Ostr.width() <= 0 || static_cast<_SizeT>(_Ostr.width()) <= _Size
		? 0 : static_cast<_SizeT>(_Ostr.width()) - _Size;
	const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

	if (!_Ok)
		_State |= ios_base::badbit;
	else
		{	// state okay, insert characters
		_TRY_IO_BEGIN
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; 0 < _Pad; --_Pad)	// pad on left
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
					}

		if (_State == ios_base::goodbit
			&& _Ostr.rdbuf()->sputn(_Data, (streamsize)_Size)
				!= (streamsize)_Size)
				_State |= ios_base::badbit;
		else
			for (; 0 < _Pad; --_Pad)	// pad on right
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	// insertion failed, quit
					_State |= ios_base::badbit;
					break;
					}
		_Ostr.width(0);
		_CATCH_IO_(_Ostr)
		}

	_Ostr.setstate(_State);
	return (_Ostr);
	}

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _XSTRING_INSERT_H */
