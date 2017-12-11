// xsmf_control.h internal header
// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once
#ifndef _XSMF_CONTROL_H
#define _XSMF_CONTROL_H
#ifndef RC_INVOKED
#include <yvals.h>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,_STL_WARNING_LEVEL)
 #pragma warning(disable: _STL_DISABLED_WARNINGS)
 #pragma push_macro("new")
 #undef new

_STD_BEGIN

	// CLASS TEMPLATE _SMF_control_copy
template<bool _Enable>
	struct _SMF_control_copy
	{	// default all special member functions
	};

template<>
	struct _SMF_control_copy<false>
	{	// delete copy constructor
	_SMF_control_copy() = default;
	_SMF_control_copy(const _SMF_control_copy&) = delete;
	_SMF_control_copy(_SMF_control_copy&&) = default;
	_SMF_control_copy& operator=(const _SMF_control_copy&) = default;
	_SMF_control_copy& operator=(_SMF_control_copy&&) = default;
	};

	// CLASS TEMPLATE _SMF_control_move
template<bool _Enable>
	struct _SMF_control_move
	{	// default all special member functions
	};

template<>
	struct _SMF_control_move<false>
	{	// delete move constructor
	_SMF_control_move() = default;
	_SMF_control_move(const _SMF_control_move&) = default;
	_SMF_control_move(_SMF_control_move&&) = delete;
	_SMF_control_move& operator=(const _SMF_control_move&) = default;
	_SMF_control_move& operator=(_SMF_control_move&&) = default;
	};

	// CLASS TEMPLATE _SMF_control_copy_assign
template<bool _Enable>
	struct _SMF_control_copy_assign
	{	// default all special member functions
	};

template<>
	struct _SMF_control_copy_assign<false>
	{	// delete copy assignment
	_SMF_control_copy_assign() = default;
	_SMF_control_copy_assign(const _SMF_control_copy_assign&) = default;
	_SMF_control_copy_assign(_SMF_control_copy_assign&&) = default;
	_SMF_control_copy_assign& operator=(const _SMF_control_copy_assign&) = delete;
	_SMF_control_copy_assign& operator=(_SMF_control_copy_assign&&) = default;
	};

	// CLASS TEMPLATE _SMF_control_move_assign
template<bool _Enable>
	struct _SMF_control_move_assign
	{	// default all special member functions
	};

template<>
	struct _SMF_control_move_assign<false>
	{	// delete move assignment
	_SMF_control_move_assign() = default;
	_SMF_control_move_assign(const _SMF_control_move_assign&) = default;
	_SMF_control_move_assign(_SMF_control_move_assign&&) = default;
	_SMF_control_move_assign& operator=(const _SMF_control_move_assign&) = default;
	_SMF_control_move_assign& operator=(_SMF_control_move_assign&&) = delete;
	};

_STD_END

 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)

#endif /* RC_INVOKED */
#endif /* _XSMF_CONTROL_H */
