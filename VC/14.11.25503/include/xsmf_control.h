// xsmf_control.h internal header
// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once
#ifndef _XSMF_CONTROL_H
#define _XSMF_CONTROL_H
#ifndef RC_INVOKED
#include <type_traits>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,_STL_WARNING_LEVEL)
 #pragma warning(disable: _STL_DISABLED_WARNINGS)
 #pragma push_macro("new")
 #undef new

_STD_BEGIN

	// ALIAS TEMPLATE _SMF_control_copy
template<class _Base>
	struct _Non_trivial_copy
		: _Base
	{	// non-trivial copy construction facade
	using _Base::_Base;

	_Non_trivial_copy() = default;
	_Non_trivial_copy(const _Non_trivial_copy& _That)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(this->_Construct_from(static_cast<const _Base&>(_That))))
		{
		this->_Construct_from(static_cast<const _Base&>(_That));
		}
	_Non_trivial_copy(_Non_trivial_copy&&) = default;
	_Non_trivial_copy& operator=(const _Non_trivial_copy&) = default;
	_Non_trivial_copy& operator=(_Non_trivial_copy&&) = default;
	};

template<class _Base,
	class... _Types>
	using _SMF_control_copy = conditional_t<
		conjunction<
			is_copy_constructible<_Types>...,
			negation<conjunction<is_trivially_copy_constructible<_Types>...>>>::value,
		_Non_trivial_copy<_Base>,
		_Base>;


	// ALIAS TEMPLATE _SMF_control_move
template<class _Base,
	class... _Types>
	struct _Non_trivial_move
		: _SMF_control_copy<_Base, _Types...>
	{	// non-trivial move construction facade
	using _SMF_control_copy<_Base, _Types...>::_SMF_control_copy;

	_Non_trivial_move() = default;
	_Non_trivial_move(const _Non_trivial_move&) = default;
	_Non_trivial_move(_Non_trivial_move&& _That)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(this->_Construct_from(static_cast<_Base&&>(_That))))
		{
		this->_Construct_from(static_cast<_Base&&>(_That));
		}
	_Non_trivial_move& operator=(const _Non_trivial_move&) = default;
	_Non_trivial_move& operator=(_Non_trivial_move&&) = default;
	};

template<class _Base,
	class... _Types>
	using _SMF_control_move = conditional_t<
		conjunction<
			is_move_constructible<_Types>...,
			negation<conjunction<is_trivially_move_constructible<_Types>...>>>::value,
		_Non_trivial_move<_Base, _Types...>,
		_SMF_control_copy<_Base, _Types...>>;


	// ALIAS TEMPLATE _SMF_control_copy_assign
template<class _Base,
	class... _Types>
	struct _Non_trivial_copy_assign
		: _SMF_control_move<_Base, _Types...>
	{	// non-trivial copy assignment facade
	using _SMF_control_move<_Base, _Types...>::_SMF_control_move;

	_Non_trivial_copy_assign() = default;
	_Non_trivial_copy_assign(const _Non_trivial_copy_assign&) = default;
	_Non_trivial_copy_assign(_Non_trivial_copy_assign&&) = default;
	_Non_trivial_copy_assign& operator=(const _Non_trivial_copy_assign& _That)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(this->_Assign_from(static_cast<const _Base&>(_That))))
		{
		this->_Assign_from(static_cast<const _Base&>(_That));
		return (*this);
		}
	_Non_trivial_copy_assign& operator=(_Non_trivial_copy_assign&&) = default;
	};

template<class _Base,
	class... _Types>
	struct _Deleted_copy_assign
		: _SMF_control_move<_Base, _Types...>
	{	// deleted copy assignment facade
	using _SMF_control_move<_Base, _Types...>::_SMF_control_move;

	_Deleted_copy_assign() = default;
	_Deleted_copy_assign(const _Deleted_copy_assign&) = default;
	_Deleted_copy_assign(_Deleted_copy_assign&&) = default;
	_Deleted_copy_assign& operator=(const _Deleted_copy_assign&) = delete;
	_Deleted_copy_assign& operator=(_Deleted_copy_assign&&) = default;
	};

template<class _Base,
	class... _Types>
	using _SMF_control_copy_assign =
		conditional_t<
			conjunction<
				is_trivially_destructible<_Types>...,
				is_trivially_copy_constructible<_Types>...,
				is_trivially_copy_assignable<_Types>...>::value,
			_SMF_control_move<_Base, _Types...>,
			conditional_t<
				conjunction<
					is_copy_constructible<_Types>...,
					is_copy_assignable<_Types>...>::value,
				_Non_trivial_copy_assign<_Base, _Types...>,
				_Deleted_copy_assign<_Base, _Types...>>>;


	// ALIAS TEMPLATE _SMF_control_move_assign
template<class _Base,
	class... _Types>
	struct _Non_trivial_move_assign
		: _SMF_control_copy_assign<_Base, _Types...>
	{	// non-trivial move assignment facade
	using _SMF_control_copy_assign<_Base, _Types...>::_SMF_control_copy_assign;

	_Non_trivial_move_assign() = default;
	_Non_trivial_move_assign(const _Non_trivial_move_assign&) = default;
	_Non_trivial_move_assign(_Non_trivial_move_assign&&) = default;
	_Non_trivial_move_assign& operator=(const _Non_trivial_move_assign&) = default;
	_Non_trivial_move_assign& operator=(_Non_trivial_move_assign&& _That)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(this->_Assign_from(static_cast<_Base&&>(_That))))
		{
		this->_Assign_from(static_cast<_Base&&>(_That));
		return (*this);
		}
	};

template<class _Base,
	class... _Types>
	struct _Deleted_move_assign
		: _SMF_control_copy_assign<_Base, _Types...>
	{	// deleted move assignment facade
	using _SMF_control_copy_assign<_Base, _Types...>::_SMF_control_copy_assign;

	_Deleted_move_assign() = default;
	_Deleted_move_assign(const _Deleted_move_assign&) = default;
	_Deleted_move_assign(_Deleted_move_assign&&) = default;
	_Deleted_move_assign& operator=(const _Deleted_move_assign&) = default;
	_Deleted_move_assign& operator=(_Deleted_move_assign&&) = delete;
	};

template<class _Base,
	class... _Types>
	using _SMF_control_move_assign =
		conditional_t<
			conjunction<
				is_trivially_destructible<_Types>...,
				is_trivially_move_constructible<_Types>...,
				is_trivially_move_assignable<_Types>...>::value,
			_SMF_control_copy_assign<_Base, _Types...>,
			conditional_t<
				conjunction<
					is_move_constructible<_Types>...,
					is_move_assignable<_Types>...>::value,
				_Non_trivial_move_assign<_Base, _Types...>,
				_Deleted_move_assign<_Base, _Types...>>>;


	// ALIAS TEMPLATE _SMF_control
template<class _Base,
	class... _Types>
	using _SMF_control = _SMF_control_move_assign<_Base, _Types...>;


	// TYPE TRAIT _Is_trivially_swappable
namespace _Has_ADL_swap_detail {
	void swap(); // undefined (deliberate shadowing)

	struct _Has_ADL_swap_unique_type
		{	// TRANSITION, C1XX
		};

	template<class,
		class = void>
		struct _Has_ADL_swap
			: false_type
		{};
	template<class _Ty>
		struct _Has_ADL_swap<_Ty,
			void_t<_Has_ADL_swap_unique_type,
				decltype(swap(_STD declval<_Ty&>(), _STD declval<_Ty&>()))>>
			: true_type
		{};
} // namespace _Has_ADL_swap_detail
using _Has_ADL_swap_detail::_Has_ADL_swap;

template<class _Ty>
	struct _Is_trivially_swappable
		: _Conjunction_t<
			is_trivially_destructible<_Ty>,
			is_trivially_move_constructible<_Ty>,
			is_trivially_move_assignable<_Ty>,
			negation<_Has_ADL_swap<_Ty>>>
	{	// true_type if it is valid to swap two _Ty lvalues by exchanging
		// object representations.
	};

_STD_END

 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)

#endif /* RC_INVOKED */
#endif /* _XSMF_CONTROL_H */
