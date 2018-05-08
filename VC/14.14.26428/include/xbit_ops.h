// xbit_ops.h internal header
// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once
#ifndef _XBIT_OPS_H
#define _XBIT_OPS_H
#ifndef RC_INVOKED

#include <yvals.h>
#include <intrin0.h>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,_STL_WARNING_LEVEL)
 #pragma warning(disable: _STL_DISABLED_WARNINGS)
 _STL_DISABLE_CLANG_WARNINGS
 #pragma push_macro("new")
 #undef new

 #pragma warning(disable: 4793) // function compiled as native

_STD_BEGIN

_NODISCARD inline unsigned long _Floor_of_log_2(const size_t _Value) _NOEXCEPT
	{	// returns floor(log_2(_Value))
	unsigned long _Result;
 #ifdef _WIN64
	_BitScanReverse64(&_Result, _Value);
 #else /* ^^^ 64-bit ^^^ / vvv 32-bit vvv */
	_BitScanReverse(&_Result, _Value);
 #endif /* 64 vs. 32-bit */
	return (_Result);
	}

_NODISCARD inline unsigned long _Ceiling_of_log_2(const size_t _Value) _NOEXCEPT
	{	// returns ceil(log_2(_Value))
		// pre: _Value > 1
	return (1 + _Floor_of_log_2(_Value - 1));
	}

_STD_END

 #pragma pop_macro("new")
 _STL_RESTORE_CLANG_WARNINGS
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _XBIT_OPS_H */
