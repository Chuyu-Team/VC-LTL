// tuple standard header
#pragma once
#ifndef _TUPLE_
#define _TUPLE_
#ifndef RC_INVOKED
#include <new>
#include <type_traits>
#include <xutility>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)
 #pragma push_macro("new")
 #undef new

_STD_BEGIN
	// ALIAS TEMPLATE _Tuple_implicit_t
template<bool _Same,
	class _Dest,
	class... _Srcs>
	struct _Tuple_implicit_val0
		: false_type
	{	// Constrain tuple's implicit constructors
	};

template<class... _Dests,
	class... _Srcs>
	struct _Tuple_implicit_val0<true, tuple<_Dests...>, _Srcs...>
		: conjunction<
			is_constructible<_Dests, _Srcs>...,
			is_convertible<_Srcs, _Dests>...
		>::type
	{	// Constrain tuple's implicit constructors
	};

template<class _Dest,
	class... _Srcs>
	struct _Tuple_implicit_val
		: _Tuple_implicit_val0<tuple_size<_Dest>::value == sizeof...(_Srcs), _Dest, _Srcs...>::type
	{	// Constrain tuple's implicit constructors
	};

template<class _Dest,
	class... _Srcs>
	using _Tuple_implicit_t = enable_if_t<_Tuple_implicit_val<_Dest, _Srcs...>::value, int>;

	// ALIAS TEMPLATE _Tuple_explicit_t
template<bool _Same,
	class _Dest,
	class... _Srcs>
	struct _Tuple_explicit_val0
		: false_type
	{	// Constrain tuple's explicit constructors
	};

template<class... _Dests,
	class... _Srcs>
	struct _Tuple_explicit_val0<true, tuple<_Dests...>, _Srcs...>
		: conjunction<
			is_constructible<_Dests, _Srcs>...,
			negation<conjunction<is_convertible<_Srcs, _Dests>...>>
		>::type
	{	// Constrain tuple's explicit constructors
	};

template<class _Dest,
	class... _Srcs>
	struct _Tuple_explicit_val
		: _Tuple_explicit_val0<tuple_size<_Dest>::value == sizeof...(_Srcs), _Dest, _Srcs...>::type
	{	// Constrain tuple's explicit constructors
	};

template<class _Dest,
	class... _Srcs>
	using _Tuple_explicit_t = enable_if_t<_Tuple_explicit_val<_Dest, _Srcs...>::value, int>;

	// ALIAS TEMPLATE _Tuple_convert_copy_t
template<class _Myt,
	class... _Other>
	struct _Tuple_convert_copy
	{	// Constrain tuple's converting copy constructor (LWG 2549)
	typedef int type;
	};

template<class _This,
	class _Uty>
	struct _Tuple_convert_copy<tuple<_This>, _Uty>
		: enable_if<!is_same<_This, _Uty>::value
			&& !is_constructible<_This, const tuple<_Uty>&>::value
			&& !is_convertible<const tuple<_Uty>&, _This>::value, int>
	{	// Constrain tuple's converting copy constructor (LWG 2549)
	};

template<class _Myt,
	class... _Other>
	using _Tuple_convert_copy_t = typename _Tuple_convert_copy<_Myt, _Other...>::type;

	// ALIAS TEMPLATE _Tuple_convert_move_t
template<class _Myt,
	class... _Other>
	struct _Tuple_convert_move
	{	// Constrain tuple's converting move constructor (LWG 2549)
	typedef int type;
	};

template<class _This,
	class _Uty>
	struct _Tuple_convert_move<tuple<_This>, _Uty>
		: enable_if<!is_same<_This, _Uty>::value
			&& !is_constructible<_This, tuple<_Uty> >::value
			&& !is_convertible<tuple<_Uty>, _This>::value, int>
	{	// Constrain tuple's converting move constructor (LWG 2549)
	};

template<class _Myt,
	class... _Other>
	using _Tuple_convert_move_t = typename _Tuple_convert_move<_Myt, _Other...>::type;

	// STRUCT TEMPLATE _Tuple_perfect_val
template<class _Myt,
	class _This2,
	class... _Rest2>
	struct _Tuple_perfect_val
		: true_type
	{	// Constrain tuple's perfect forwarding constructor (LWG issue not yet filed)
	};

template<class _Myt,
	class _This2>
	struct _Tuple_perfect_val<_Myt, _This2>
		: negation<is_same<_Myt, remove_const_t<remove_reference_t<_This2>>>>::type
	{	// Constrain tuple's perfect forwarding constructor (LWG issue not yet filed)
	};

	// STRUCT _Ignore
struct _Ignore
	{	// struct that ignores assignments
	template<class _Ty>
		void operator=(const _Ty&) const
		{	// do nothing
		}
	};

constexpr _Ignore ignore{};

		// STRUCT _Tuple_alloc_t
struct _Tuple_alloc_t
	{	// tag type to disambiguate added allocator argument
	};

constexpr _Tuple_alloc_t _Tuple_alloc{};

	// TEMPLATE CLASS _Tuple_val
template<class _Ty>
	struct _Tuple_val
	{	// stores each value in a tuple
	constexpr _Tuple_val()
		: _Val()
		{	// default construct
		}

	template<class _Other>
		constexpr _Tuple_val(_Other&& _Arg)
		: _Val(_STD forward<_Other>(_Arg))
		{	// construct with argument
		}

	template<class _Other>
		_Tuple_val& operator=(_Other&& _Right)
		{	// assign
		_Val = _STD forward<_Other>(_Right);
		return (*this);
		}

	template<class _Alloc,
		class... _Other>
		_Tuple_val(const _Alloc&,
			typename enable_if<!uses_allocator<_Ty, _Alloc>::value,
				_Tuple_alloc_t>::type, _Other&&... _Arg)
		: _Val(_STD forward<_Other>(_Arg)...)
		{	// construct with optional arguments, no allocator
		}

	template<class _Alloc,
		class... _Other>
		_Tuple_val(const _Alloc& _Al,
			typename enable_if<uses_allocator<_Ty, _Alloc>::value
				&& is_constructible<_Ty,
					allocator_arg_t, _Alloc>::value,
				_Tuple_alloc_t>::type, _Other&&... _Arg)
		: _Val(allocator_arg, _Al, _STD forward<_Other>(_Arg)...)
		{	// construct with optional arguments, leading allocator
		}

	template<class _Alloc,
		class... _Other>
		_Tuple_val(const _Alloc& _Al,
			typename enable_if<uses_allocator<_Ty, _Alloc>::value
				&& !is_constructible<_Ty,
					allocator_arg_t, _Alloc>::value,
				_Tuple_alloc_t>::type, _Other&&... _Arg)
		: _Val(_STD forward<_Other>(_Arg)..., _Al)
		{	// construct with optional arguments, trailing allocator
		}

	_Ty _Val;
	};

	// TEMPLATE CLASS tuple
struct _Exact_args_t
	{	// tag type to disambiguate construction (from one arg per element)
	};

struct _Unpack_tuple_t
	{	// tag type to disambiguate construction (from unpacking a tuple/pair)
	};

struct _Alloc_exact_args_t
	{	// tag type to disambiguate construction (from an allocator and one arg per element)
	};

struct _Alloc_unpack_tuple_t
	{	// tag type to disambiguate construction (from an allocator and unpacking a tuple/pair)
	};

template<class... _Types>
	class tuple;

template<>
	class tuple<>
	{	// empty tuple
public:
	typedef tuple<> _Myt;

	constexpr tuple() _NOEXCEPT
		{	// default construct
		}

	template<class _Alloc>
		tuple(allocator_arg_t, const _Alloc&) _NOEXCEPT
		{	// default construct, allocator
		}

	constexpr tuple(const tuple&) _NOEXCEPT
		{	// copy construct
		}

	template<class _Alloc>
		tuple(allocator_arg_t, const _Alloc&, const _Myt&) _NOEXCEPT
		{	// copy construct, allocator
		}

	template<class _Tag,
		enable_if_t<is_same<_Tag, _Exact_args_t>::value, int> = 0>
		constexpr tuple(_Tag) _NOEXCEPT
		{	// construct from one arg per element
		}

	template<class _Tag,
		enable_if_t<is_same<_Tag, _Unpack_tuple_t>::value, int> = 0>
		constexpr tuple(_Tag, const _Myt&) _NOEXCEPT
		{	// construct from unpacking a tuple (TRANSITION, VSO#194815)
		}

	template<class _Tag,
		class _Alloc,
		enable_if_t<is_same<_Tag, _Alloc_exact_args_t>::value, int> = 0>
		tuple(_Tag, const _Alloc&) _NOEXCEPT
		{	// construct from an allocator and one arg per element
		}

	void swap(_Myt&) _NOEXCEPT
		{	// swap elements
		}

	constexpr bool _Equals(const _Myt&) const _NOEXCEPT
		{	// test if *this == _Right
		return (true);
		}

	constexpr bool _Less(const _Myt&) const _NOEXCEPT
		{	// test if *this < _Right
		return (false);
		}
	};

template<class _This,
	class... _Rest>
	class tuple<_This, _Rest...>
		: private tuple<_Rest...>
	{	// recursive tuple definition
public:
	typedef _This _This_type;
	typedef tuple<_This, _Rest...> _Myt;
	typedef tuple<_Rest...> _Mybase;
	static constexpr size_t _Mysize = 1 + sizeof...(_Rest);

	template<class _Tag,
		class _This2,
		class... _Rest2,
		enable_if_t<is_same<_Tag, _Exact_args_t>::value, int> = 0>
		constexpr tuple(_Tag, _This2&& _This_arg, _Rest2&&... _Rest_arg)
		: _Mybase(_Exact_args_t{}, _STD forward<_Rest2>(_Rest_arg)...),
			_Myfirst(_STD forward<_This2>(_This_arg))
		{	// construct from one arg per element
		}

/* TRANSITION, VSO#194815
	template<class _Tag,
		class _Tpl,
		size_t... _Indices,
		enable_if_t<is_same<_Tag, _Unpack_tuple_t>::value, int> = 0> inline
		constexpr tuple(_Tag, _Tpl&& _Right, integer_sequence<size_t, _Indices...>);
*/

/* TRANSITION, VSO#194815
	template<class _Tag,
		class _Tpl,
		enable_if_t<is_same<_Tag, _Unpack_tuple_t>::value, int> = 0>
		constexpr tuple(_Tag, _Tpl&& _Right)
		: tuple(_Unpack_tuple_t{}, _STD forward<_Tpl>(_Right),
			make_integer_sequence<size_t, tuple_size<remove_reference_t<_Tpl>>::value>{})
		{	// construct from unpacking a tuple/pair
		}
*/

	template<class _Tag,
		class... _Other,
		enable_if_t<is_same<_Tag, _Unpack_tuple_t>::value, int> = 0>
		constexpr tuple(_Tag, const tuple<_Other...>& _Right)
		: _Mybase(_Unpack_tuple_t{}, _Right._Get_rest()),
			_Myfirst(_Right._Myfirst._Val)
		{	// construct from unpacking a tuple (TRANSITION, VSO#194815)
		}

	template<class _Tag,
		class... _Other,
		enable_if_t<is_same<_Tag, _Unpack_tuple_t>::value, int> = 0>
		constexpr tuple(_Tag, tuple<_Other...>&& _Right)
		: _Mybase(_Unpack_tuple_t{}, (typename tuple<_Other...>::_Mybase&&) _Right),
			_Myfirst(_STD forward<typename tuple<_Other...>::_This_type>(_Right._Myfirst._Val))
		{	// construct from unpacking a tuple (TRANSITION, VSO#194815)
		}

	template<class _Tag,
		class _Alloc,
		class _This2,
		class... _Rest2,
		enable_if_t<is_same<_Tag, _Alloc_exact_args_t>::value, int> = 0>
		tuple(_Tag, const _Alloc& _Al, _This2&& _This_arg, _Rest2&&... _Rest_arg)
		: _Mybase(_Alloc_exact_args_t{}, _Al, _STD forward<_Rest2>(_Rest_arg)...),
			_Myfirst(_Al, _Tuple_alloc, _STD forward<_This2>(_This_arg))
		{	// construct from an allocator and one arg per element
		}

	template<class _Tag,
		class _Alloc,
		class _Tpl,
		size_t... _Indices,
		enable_if_t<is_same<_Tag, _Alloc_unpack_tuple_t>::value, int> = 0> inline
		tuple(_Tag, const _Alloc& _Al, _Tpl&& _Right, integer_sequence<size_t, _Indices...>);

	template<class _Tag,
		class _Alloc,
		class _Tpl,
		enable_if_t<is_same<_Tag, _Alloc_unpack_tuple_t>::value, int> = 0>
		tuple(_Tag, const _Alloc& _Al, _Tpl&& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _STD forward<_Tpl>(_Right),
			make_integer_sequence<size_t, tuple_size<remove_reference_t<_Tpl>>::value>{})
		{	// construct from an allocator and unpacking a tuple/pair
		}

	template<class _This2 = _This,
		class = enable_if_t<conjunction<is_default_constructible<_This2>,
										is_default_constructible<_Rest>...>::value> >
		constexpr tuple()
		: _Mybase(), _Myfirst()
		{	// construct default
		}

	template<class... _Other,
		_Tuple_implicit_t<_Myt, const _Other&...> = 0,
		_Tuple_convert_copy_t<_Myt, _Other...> = 0>
		constexpr tuple(const tuple<_Other...>& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _Right)
		: _Mybase(_Unpack_tuple_t{}, _Right._Get_rest()),
			_Myfirst(_Right._Myfirst._Val)
		{	// construct by copying same size tuple
		}

	template<class... _Other,
		_Tuple_explicit_t<_Myt, const _Other&...> = 0,
		_Tuple_convert_copy_t<_Myt, _Other...> = 0>
		constexpr explicit tuple(const tuple<_Other...>& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _Right)
		: _Mybase(_Unpack_tuple_t{}, _Right._Get_rest()),
			_Myfirst(_Right._Myfirst._Val)
		{	// construct by copying same size tuple
		}

	template<class _Alloc,
		class... _Other,
		_Tuple_implicit_t<_Myt, const _Other&...> = 0,
		_Tuple_convert_copy_t<_Myt, _Other...> = 0>
		tuple(allocator_arg_t, const _Alloc& _Al,
			const tuple<_Other...>& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
		{	// construct by copying same size tuple, allocator
		}

	template<class _Alloc,
		class... _Other,
		_Tuple_explicit_t<_Myt, const _Other&...> = 0,
		_Tuple_convert_copy_t<_Myt, _Other...> = 0>
		explicit tuple(allocator_arg_t, const _Alloc& _Al,
			const tuple<_Other...>& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
		{	// construct by copying same size tuple, allocator
		}

	template<class _This2 = _This,
		_Tuple_implicit_t<_Myt, const _This2&, const _Rest&...> = 0>
		constexpr tuple(const _This& _This_arg, const _Rest&... _Rest_arg)
// TRANSITION, VSO#194815
// : tuple(_Exact_args_t{}, _This_arg, _Rest_arg...)
		: _Mybase(_Exact_args_t{}, _Rest_arg...), _Myfirst(_This_arg)
		{	// construct from one or more copied elements
		}

	template<class _This2 = _This,
		_Tuple_explicit_t<_Myt, const _This2&, const _Rest&...> = 0>
		constexpr explicit tuple(const _This& _This_arg, const _Rest&... _Rest_arg)
// TRANSITION, VSO#194815
// : tuple(_Exact_args_t{}, _This_arg, _Rest_arg...)
		: _Mybase(_Exact_args_t{}, _Rest_arg...), _Myfirst(_This_arg)
		{	// construct from one or more copied elements
		}

	template<class _Alloc,
		class _This2 = _This,
		_Tuple_implicit_t<_Myt, const _This2&, const _Rest&...> = 0>
		tuple(allocator_arg_t, const _Alloc& _Al,
			const _This& _This_arg, const _Rest&... _Rest_arg)
		: tuple(_Alloc_exact_args_t{}, _Al, _This_arg, _Rest_arg...)
		{	// construct from one or more copied elements, allocator
		}

	template<class _Alloc,
		class _This2 = _This,
		_Tuple_explicit_t<_Myt, const _This2&, const _Rest&...> = 0>
		explicit tuple(allocator_arg_t, const _Alloc& _Al,
			const _This& _This_arg, const _Rest&... _Rest_arg)
		: tuple(_Alloc_exact_args_t{}, _Al, _This_arg, _Rest_arg...)
		{	// construct from one or more copied elements, allocator
		}

	template<class _This2,
		class... _Rest2,
		enable_if_t<conjunction<
			_Tuple_perfect_val<_Myt, _This2, _Rest2...>,
			_Tuple_implicit_val<_Myt, _This2, _Rest2...>
		>::value, int> = 0>
		constexpr tuple(_This2&& _This_arg, _Rest2&&... _Rest_arg)
// TRANSITION, VSO#194815
// : tuple(_Exact_args_t{}, _STD forward<_This2>(_This_arg), _STD forward<_Rest2>(_Rest_arg)...)
		: _Mybase(_Exact_args_t{}, _STD forward<_Rest2>(_Rest_arg)...),
			_Myfirst(_STD forward<_This2>(_This_arg))
		{	// construct from one or more moved elements
		}

	template<class _This2,
		class... _Rest2,
		enable_if_t<conjunction<
			_Tuple_perfect_val<_Myt, _This2, _Rest2...>,
			_Tuple_explicit_val<_Myt, _This2, _Rest2...>
		>::value, int> = 0>
		constexpr explicit tuple(_This2&& _This_arg, _Rest2&&... _Rest_arg)
// TRANSITION, VSO#194815
// : tuple(_Exact_args_t{}, _STD forward<_This2>(_This_arg), _STD forward<_Rest2>(_Rest_arg)...)
		: _Mybase(_Exact_args_t{}, _STD forward<_Rest2>(_Rest_arg)...),
			_Myfirst(_STD forward<_This2>(_This_arg))
		{	// construct from one or more moved elements
		}

	template<class _Alloc,
		class _This2,
		class... _Rest2,
		enable_if_t<conjunction<
			_Tuple_perfect_val<_Myt, _This2, _Rest2...>,
			_Tuple_implicit_val<_Myt, _This2, _Rest2...>
		>::value, int> = 0>
		tuple(allocator_arg_t, const _Alloc& _Al,
			_This2&& _This_arg, _Rest2&&... _Rest_arg)
		: tuple(_Alloc_exact_args_t{}, _Al, _STD forward<_This2>(_This_arg), _STD forward<_Rest2>(_Rest_arg)...)
		{	// construct from one or more moved elements, allocator
		}

	template<class _Alloc,
		class _This2,
		class... _Rest2,
		enable_if_t<conjunction<
			_Tuple_perfect_val<_Myt, _This2, _Rest2...>,
			_Tuple_explicit_val<_Myt, _This2, _Rest2...>
		>::value, int> = 0>
		explicit tuple(allocator_arg_t, const _Alloc& _Al,
			_This2&& _This_arg, _Rest2&&... _Rest_arg)
		: tuple(_Alloc_exact_args_t{}, _Al, _STD forward<_This2>(_This_arg), _STD forward<_Rest2>(_Rest_arg)...)
		{	// construct from one or more moved elements, allocator
		}

	template<class... _Other,
		_Tuple_implicit_t<_Myt, _Other...> = 0,
		_Tuple_convert_move_t<_Myt, _Other...> = 0>
		constexpr tuple(tuple<_Other...>&& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _STD move(_Right))
		: _Mybase(_Unpack_tuple_t{}, (typename tuple<_Other...>::_Mybase&&) _Right),
			_Myfirst(_STD forward<typename tuple<_Other...>::_This_type>(_Right._Myfirst._Val))
		{	// construct by moving same size tuple
		}

	template<class... _Other,
		_Tuple_explicit_t<_Myt, _Other...> = 0,
		_Tuple_convert_move_t<_Myt, _Other...> = 0>
		constexpr explicit tuple(tuple<_Other...>&& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _STD move(_Right))
		: _Mybase(_Unpack_tuple_t{}, (typename tuple<_Other...>::_Mybase&&) _Right),
			_Myfirst(_STD forward<typename tuple<_Other...>::_This_type>(_Right._Myfirst._Val))
		{	// construct by moving same size tuple
		}

	template<class _Alloc,
		class... _Other,
		_Tuple_implicit_t<_Myt, _Other...> = 0,
		_Tuple_convert_move_t<_Myt, _Other...> = 0>
		tuple(allocator_arg_t, const _Alloc& _Al,
			tuple<_Other...>&& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _STD move(_Right))
		{	// construct by moving same size tuple, allocator
		}

	template<class _Alloc,
		class... _Other,
		_Tuple_explicit_t<_Myt, _Other...> = 0,
		_Tuple_convert_move_t<_Myt, _Other...> = 0>
		explicit tuple(allocator_arg_t, const _Alloc& _Al,
			tuple<_Other...>&& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _STD move(_Right))
		{	// construct by moving same size tuple, allocator
		}

	template<class... _Other>
		_Myt& operator=(const tuple<_Other...>& _Right)
		{	// assign by copying same size tuple
		_Myfirst._Val = _Right._Myfirst._Val;
		_Get_rest() = _Right._Get_rest();
		return (*this);
		}

	template<class... _Other>
		_Myt& operator=(tuple<_Other...>&& _Right)
		{	// assign by moving same size tuple
		_Myfirst._Val = _STD forward<typename tuple<_Other...>::_This_type>
			(_Right._Myfirst._Val);
		_Get_rest() = _STD forward<typename tuple<_Other...>::_Mybase>
			(_Right._Get_rest());
		return (*this);
		}

	template<class... _Other>
		constexpr bool _Equals(const tuple<_Other...>& _Right) const
		{	// test if *this == _Right
		static_assert(_Mysize == sizeof...(_Other),
			"comparing tuple to object with different size");
		return (_Myfirst._Val == _Right._Myfirst._Val
			&& _Mybase::_Equals(_Right._Get_rest()));
		}

	template<class... _Other>
		constexpr bool _Less(const tuple<_Other...>& _Right) const
		{	// test if *this < _Right
		static_assert(_Mysize == sizeof...(_Other),
			"comparing tuple to object with different size");
		return (_Myfirst._Val < _Right._Myfirst._Val
			|| (!(_Right._Myfirst._Val < _Myfirst._Val)
				&& _Mybase::_Less(_Right._Get_rest())));
		}

	template<class _Alloc,
		class _This2 = _This,
		class = enable_if_t<conjunction<is_default_constructible<_This2>,
										is_default_constructible<_Rest>...>::value> >
		tuple(allocator_arg_t, const _Alloc& _Al)
		: _Mybase(allocator_arg, _Al), _Myfirst(_Al, _Tuple_alloc)
		{	// construct default, allocator
		}

	template<class _Alloc>
		tuple(allocator_arg_t, const _Alloc& _Al,
			const _Myt& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
		{	// construct by copying, allocator
		}

	tuple(const _Myt&) = default;
	tuple(_Myt&&) = default;

	template<class _First,
		class _Second,
		_Tuple_implicit_t<_Myt, const _First&, const _Second&> = 0>
		constexpr tuple(const pair<_First, _Second>& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _Right)
		: _Mybase(_Exact_args_t{}, _Right.second), _Myfirst(_Right.first)
		{	// construct by copying pair
		}

	template<class _First,
		class _Second,
		_Tuple_explicit_t<_Myt, const _First&, const _Second&> = 0>
		constexpr explicit tuple(const pair<_First, _Second>& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _Right)
		: _Mybase(_Exact_args_t{}, _Right.second), _Myfirst(_Right.first)
		{	// construct by copying pair
		}

	template<class _Alloc,
		class _First,
		class _Second,
		_Tuple_implicit_t<_Myt, const _First&, const _Second&> = 0>
		tuple(allocator_arg_t, const _Alloc& _Al,
			const pair<_First, _Second>& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
		{	// construct by copying pair, allocator
		}

	template<class _Alloc,
		class _First,
		class _Second,
		_Tuple_explicit_t<_Myt, const _First&, const _Second&> = 0>
		explicit tuple(allocator_arg_t, const _Alloc& _Al,
			const pair<_First, _Second>& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
		{	// construct by copying pair, allocator
		}

	_Myt& operator=(const _Myt& _Right)
		{	// assign
		_Myfirst._Val = _Right._Myfirst._Val;
		_Get_rest() = _Right._Get_rest();
		return (*this);
		}

	template<class _First,
		class _Second>
		_Myt& operator=(const pair<_First, _Second>& _Right)
		{	// assign by copying pair
		static_assert(_Mysize == 2,
			"assigning to tuple from object with different size");
		_Myfirst._Val = _Right.first;
		_Get_rest()._Myfirst._Val = _Right.second;
		return (*this);
		}

	template<class _Alloc>
		tuple(allocator_arg_t, const _Alloc& _Al,
			_Myt&& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _STD move(_Right))
		{	// construct by moving, allocator
		}

	template<class _First,
		class _Second,
		_Tuple_implicit_t<_Myt, _First, _Second> = 0>
		constexpr tuple(pair<_First, _Second>&& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _STD move(_Right))
		: _Mybase(_Exact_args_t{}, _STD forward<_Second>(_Right.second)),
			_Myfirst(_STD forward<_First>(_Right.first))
		{	// construct by moving pair
		}

	template<class _First,
		class _Second,
		_Tuple_explicit_t<_Myt, _First, _Second> = 0>
		constexpr explicit tuple(pair<_First, _Second>&& _Right)
// TRANSITION, VSO#194815
// : tuple(_Unpack_tuple_t{}, _STD move(_Right))
		: _Mybase(_Exact_args_t{}, _STD forward<_Second>(_Right.second)),
			_Myfirst(_STD forward<_First>(_Right.first))
		{	// construct by moving pair
		}

	template<class _Alloc,
		class _First,
		class _Second,
		_Tuple_implicit_t<_Myt, _First, _Second> = 0>
		tuple(allocator_arg_t, const _Alloc& _Al,
			pair<_First, _Second>&& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _STD move(_Right))
		{	// construct by moving pair, allocator
		}

	template<class _Alloc,
		class _First,
		class _Second,
		_Tuple_explicit_t<_Myt, _First, _Second> = 0>
		explicit tuple(allocator_arg_t, const _Alloc& _Al,
			pair<_First, _Second>&& _Right)
		: tuple(_Alloc_unpack_tuple_t{}, _Al, _STD move(_Right))
		{	// construct by moving pair, allocator
		}

	_Myt& operator=(_Myt&& _Right)
		_NOEXCEPT_OP(is_nothrow_move_assignable<_This>::value
			&& is_nothrow_move_assignable<_Mybase>::value)
		{	// assign by moving
		_Myfirst._Val = _STD forward<_This>(_Right._Myfirst._Val);
		_Get_rest() = _STD forward<_Mybase>(_Right._Get_rest());
		return (*this);
		}

	template<class _First,
		class _Second>
		_Myt& operator=(pair<_First, _Second>&& _Right)
		{	// assign by moving pair
		static_assert(_Mysize == 2,
			"assigning to tuple from object with different size");
		_Myfirst._Val = _STD forward<_First>(_Right.first);
		_Get_rest()._Myfirst._Val = _STD forward<_Second>(_Right.second);
		return (*this);
		}

	_Mybase& _Get_rest() _NOEXCEPT
		{	// get reference to rest of elements
		return (*this);
		}

	constexpr const _Mybase& _Get_rest() const _NOEXCEPT
		{	// get const reference to rest of elements
		return (*this);
		}

	_Tuple_val<_This> _Myfirst;	// the stored element

	void swap(tuple& _Right)
		_NOEXCEPT_OP((conjunction<_Is_nothrow_swappable<_This>,
			_Is_nothrow_swappable<_Rest>...>::value))
		{	// swap *this and _Right
		_Swap_adl(_Myfirst._Val, _Right._Myfirst._Val);
		_Mybase::swap(_Right._Get_rest());
		}
	};


	// OPERATORS FOR tuple

template<class... _Types1,
	class... _Types2> inline
	constexpr bool operator==(const tuple<_Types1...>& _Left,
		const tuple<_Types2...>& _Right)
	{	// test if _Left == _Right
	return (_Left._Equals(_Right));
	}

template<class... _Types1,
	class... _Types2> inline
	constexpr bool operator!=(const tuple<_Types1...>& _Left,
		const tuple<_Types2...>& _Right)
	{	// test if _Left != _Right
	return (!(_Left == _Right));
	}

template<class... _Types1,
	class... _Types2> inline
	constexpr bool operator<(const tuple<_Types1...>& _Left,
		const tuple<_Types2...>& _Right)
	{	// test if _Left < _Right
	return (_Left._Less(_Right));
	}

template<class... _Types1,
	class... _Types2> inline
	constexpr bool operator>=(const tuple<_Types1...>& _Left,
		const tuple<_Types2...>& _Right)
	{	// test if _Left >= _Right
	return (!(_Left < _Right));
	}

template<class... _Types1,
	class... _Types2> inline
	constexpr bool operator>(const tuple<_Types1...>& _Left,
		const tuple<_Types2...>& _Right)
	{	// test if _Left > _Right
	return (_Right < _Left);
	}

template<class... _Types1,
	class... _Types2> inline
	constexpr bool operator<=(const tuple<_Types1...>& _Left,
		const tuple<_Types2...>& _Right)
	{	// test if _Left <= _Right
	return (!(_Right < _Left));
	}

template<class... _Types,
	class = enable_if_t<conjunction<_Is_swappable<_Types>...>::value>> inline
	void swap(tuple<_Types...>& _Left,
		tuple<_Types...>& _Right)
			_NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap _Left and _Right
	return (_Left.swap(_Right));
	}


	// CLASS _Tuple_element (find element by type)
template<class _Ty,
	class _Tuple>
	struct _Tuple_element;

template<class _This,
	class... _Rest>
	struct _Tuple_element<_This, tuple<_This, _Rest...> >
	{	// select first element
	typedef int _Check_type;
	static_assert(is_void<typename _Tuple_element<_This,
		tuple<_Rest...> >::_Check_type>::value,
		"duplicate type T in get<T>(tuple)");

	typedef _This type;
	typedef tuple<_This, _Rest...> _Ttype;
	};

template<class _Ty,
	class _This,
	class... _Rest>
	struct _Tuple_element<_Ty, tuple<_This, _Rest...> >
		: public _Tuple_element<_Ty, tuple<_Rest...> >
	{	// recursive _Tuple_element definition
	};

template<class _Ty>
	struct _Tuple_element<_Ty, tuple<> >
	{	// backstop _Tuple_element definition
	typedef void _Check_type;	// proof that no duplicate type exists
	};

template<class _Ty,
	class _Tuple>
	struct _Tuple_element<_Ty, const _Tuple>
		: public _Tuple_element<_Ty, _Tuple>
	{	// _Tuple_element for const
	typedef _Tuple_element<_Ty, _Tuple> _Mybase;
	typedef typename add_const<typename _Mybase::type>::type type;
	};

template<class _Ty,
	class _Tuple>
	struct _Tuple_element<_Ty, volatile _Tuple>
		: public _Tuple_element<_Ty, _Tuple>
	{	// _Tuple_element for volatile
	typedef _Tuple_element<_Ty, _Tuple> _Mybase;
	typedef typename add_volatile<typename _Mybase::type>::type type;
	};

template<class _Ty,
	class _Tuple>
	struct _Tuple_element<_Ty, const volatile _Tuple>
		: public _Tuple_element<_Ty, _Tuple>
	{	// _Tuple_element for const volatile
	typedef _Tuple_element<_Ty, _Tuple> _Mybase;
	typedef typename add_cv<typename _Mybase::type>::type type;
	};

	// TEMPLATE FUNCTION get (by index)
template<size_t _Index,
	class... _Types> inline
	constexpr typename tuple_element<_Index, tuple<_Types...> >::type&
		get(tuple<_Types...>& _Tuple) _NOEXCEPT
	{	// get reference to _Index element of tuple
	typedef typename tuple_element<_Index, tuple<_Types...> >::_Ttype
		_Ttype;
	return (((_Ttype&)_Tuple)._Myfirst._Val);
	}

template<size_t _Index,
	class... _Types> inline
	constexpr const typename tuple_element<_Index, tuple<_Types...> >::type&
		get(const tuple<_Types...>& _Tuple) _NOEXCEPT
	{	// get const reference to _Index element of tuple
	typedef typename tuple_element<_Index, tuple<_Types...> >::_Ttype
		_Ttype;
	return (((_Ttype&)_Tuple)._Myfirst._Val);
	}

template<size_t _Index,
	class... _Types> inline
	constexpr typename tuple_element<_Index, tuple<_Types...> >::type&&
		get(tuple<_Types...>&& _Tuple) _NOEXCEPT
	{	// get rvalue reference to _Index element of tuple
	typedef typename tuple_element<_Index, tuple<_Types...> >::_Ttype
		_Ttype;
	typedef typename tuple_element<_Index, tuple<_Types...> >::type&&
		_RRtype;
	return (_STD forward<_RRtype>(((_Ttype&)_Tuple)._Myfirst._Val));
	}

	// TEMPLATE FUNCTION get (by type)
template<class _Ty,
	class... _Types> inline
	constexpr _Ty& get(tuple<_Types...>& _Tuple) _NOEXCEPT
	{	// get reference to _Ty element of tuple
	typedef typename _Tuple_element<_Ty, tuple<_Types...> >::_Ttype _Ttype;
	return (((_Ttype&)_Tuple)._Myfirst._Val);
	}

template<class _Ty,
	class... _Types> inline
	constexpr const _Ty& get(const tuple<_Types...>& _Tuple) _NOEXCEPT
	{	// get const reference to _Ty element of tuple
	typedef typename _Tuple_element<_Ty, tuple<_Types...> >::_Ttype _Ttype;
	return (((_Ttype&)_Tuple)._Myfirst._Val);
	}

template<class _Ty,
	class... _Types> inline
	constexpr _Ty&& get(tuple<_Types...>&& _Tuple) _NOEXCEPT
	{	// get rvalue reference to _Ty element of tuple
	typedef typename _Tuple_element<_Ty, tuple<_Types...> >::_Ttype _Ttype;
	return (_STD forward<_Ty&&>(((_Ttype&)_Tuple)._Myfirst._Val));
	}

	// CONSTRUCTOR TEMPLATES FOR tuple
/* TRANSITION, VSO#194815
template<class _This,
	class... _Rest>
	template<class _Tag,
		class _Tpl,
		size_t... _Indices,
		enable_if_t<is_same<_Tag, _Unpack_tuple_t>::value, int>> inline
		constexpr tuple<_This, _Rest...>::tuple(_Tag, _Tpl&& _Right, integer_sequence<size_t, _Indices...>)
		: tuple(_Exact_args_t{}, _STD get<_Indices>(_STD forward<_Tpl>(_Right))...)
		{	// construct from unpacking a tuple/pair, using get<I>()
		}
*/

template<class _This,
	class... _Rest>
	template<class _Tag,
		class _Alloc,
		class _Tpl,
		size_t... _Indices,
		enable_if_t<is_same<_Tag, _Alloc_unpack_tuple_t>::value, int>> inline
		tuple<_This, _Rest...>::tuple(_Tag, const _Alloc& _Al, _Tpl&& _Right, integer_sequence<size_t, _Indices...>)
		: tuple(_Alloc_exact_args_t{}, _Al, _STD get<_Indices>(_STD forward<_Tpl>(_Right))...)
		{	// construct from an allocator and unpacking a tuple/pair, using get<I>()
		}

	// TEMPLATE FUNCTION make_tuple
template<class... _Types> inline
	constexpr tuple<typename _Unrefwrap<_Types>::type...>
		make_tuple(_Types&&... _Args)
	{	// make tuple from elements
	typedef tuple<typename _Unrefwrap<_Types>::type...> _Ttype;
	return (_Ttype(_STD forward<_Types>(_Args)...));
	}

	// TEMPLATE FUNCTION tie
template<class... _Types> inline
	constexpr tuple<_Types&...>
		tie(_Types&... _Args) _NOEXCEPT
	{	// make tuple from elements
	typedef tuple<_Types&...> _Ttype;
	return (_Ttype(_Args...));
	}


	// TEMPLATE FUNCTION forward_as_tuple

template<class... _Types> inline
	constexpr tuple<_Types&&...>
		forward_as_tuple(_Types&&... _Args) _NOEXCEPT
	{	// forward arguments in a tuple
	return (tuple<_Types&&...>(_STD forward<_Types>(_Args)...));
	}


	// TEMPLATE STRUCT _Cat_sequences
template<class _Seq_type1,
	class _Seq_type2>
	struct _Cat_sequences;

template<size_t... _Indexes1,
	size_t... _Indexes2>
	struct _Cat_sequences<integer_sequence<size_t, _Indexes1...>,
		integer_sequence<size_t, _Indexes2...> >
	{	// concatenates two integer_sequence types
	typedef integer_sequence<size_t, _Indexes1..., _Indexes2...> type;
	};

	// FORWARD DECLARATIONS
template<class _Ty,
	size_t _Size>
	class array;

template<size_t _Idx,
	class _Ty,
	size_t _Size>
	constexpr _Ty& get(array<_Ty, _Size>& _Arr) _NOEXCEPT;

template<size_t _Idx,
	class _Ty,
	size_t _Size>
	constexpr const _Ty& get(const array<_Ty, _Size>& _Arr) _NOEXCEPT;

template<size_t _Idx,
	class _Ty,
	size_t _Size>
	constexpr _Ty&& get(array<_Ty, _Size>&& _Arr) _NOEXCEPT;

	// TEMPLATE STRUCT _View_as_tuple
template<class _Ty,
	class... _For_array>
	struct _View_as_tuple
	{	// tuple_cat() supports only tuples, pairs, and arrays
	static_assert(_Always_false<_Ty>::value,
		"Unsupported tuple_cat arguments.");
	};

template<class... _Types>
	struct _View_as_tuple<tuple<_Types...> >
	{	// view a tuple as a tuple
	typedef tuple<_Types...> type;
	};

template<class _Ty1,
	class _Ty2>
	struct _View_as_tuple<pair<_Ty1, _Ty2> >
	{	// view a pair as a tuple
	typedef tuple<_Ty1, _Ty2> type;
	};

template<class _Ty,
	class... _Types>
	struct _View_as_tuple<array<_Ty, 0>, _Types...>
	{	// view an array as a tuple; ends recursion at 0
	typedef tuple<_Types...> type;
	};

template<class _Ty,
	size_t _Size,
	class... _Types>
	struct _View_as_tuple<array<_Ty, _Size>, _Types...>
		: _View_as_tuple<array<_Ty, _Size - 1>, _Ty, _Types...>
	{	// view an array as a tuple; counts down to 0
	};

	// TEMPLATE STRUCT _Repeat_for
template<size_t _Nx,
	class _Ty>
	struct _Repeat_for
		: integral_constant<size_t, _Nx>
	{	// repeats _Nx for each _Ty in a parameter pack
	};

	// TEMPLATE FUNCTION tuple_cat
template<class _Ret,
	class _Kx_arg,
	class _Ix_arg,
	size_t _Ix_next,
	class... _Tuples>
	struct _Tuple_cat2
	{	// determine tuple_cat's return type and _Kx/_Ix indices
	static_assert(sizeof...(_Tuples) == 0,
		"Unsupported tuple_cat arguments.");
	typedef _Ret type;
	typedef _Kx_arg _Kx_arg_seq;
	typedef _Ix_arg _Ix_arg_seq;
	};

template<class... _Types1,
	class _Kx_arg,
	size_t... _Ix,
	size_t _Ix_next,
	class... _Types2,
	class... _Rest>
	struct _Tuple_cat2<tuple<_Types1...>, _Kx_arg,
		integer_sequence<size_t, _Ix...>, _Ix_next,
		tuple<_Types2...>, _Rest...>
		: _Tuple_cat2<
			tuple<_Types1..., _Types2...>,
			typename _Cat_sequences<_Kx_arg,
				make_integer_sequence<size_t, sizeof...(_Types2)> >::type,
			integer_sequence<size_t, _Ix...,
				_Repeat_for<_Ix_next, _Types2>::value...>,
			_Ix_next + 1,
			_Rest...>
	{	// determine tuple_cat's return type and _Kx/_Ix indices
	};

template<class... _Tuples>
	struct _Tuple_cat1
		: _Tuple_cat2<tuple<>, integer_sequence<size_t>,
				integer_sequence<size_t>, 0,
			typename _View_as_tuple<typename decay<_Tuples>::type>::type...>
	{	// prepare to determine tuple_cat's return type and _Kx/_Ix indices
	};

template<class _Ret,
	size_t... _Kx,
	size_t... _Ix,
	class _Ty> inline
	constexpr _Ret _Tuple_cat(integer_sequence<size_t, _Kx...>,
		integer_sequence<size_t, _Ix...>, _Ty&& _Arg)
	{	// concatenate tuples
	return (_Ret(_STD get<_Kx>(_STD get<_Ix>(_STD forward<_Ty>(_Arg)))...));
	}

template<class _Ret,
	class _Ty> inline
	constexpr _Ret _Tuple_cat(integer_sequence<size_t>,
		integer_sequence<size_t>, _Ty&&)
	{	// TRANSITION, VSO#181496
	return (_Ret());
	}

template<class... _Tuples> inline
	constexpr typename _Tuple_cat1<_Tuples...>::type
		tuple_cat(_Tuples&&... _Tpls)
	{	// concatenate tuples
	typedef _Tuple_cat1<_Tuples...> _Cat1;
	return (_Tuple_cat<typename _Cat1::type>(
		typename _Cat1::_Kx_arg_seq(), typename _Cat1::_Ix_arg_seq(),
		_STD forward_as_tuple(_STD forward<_Tuples>(_Tpls)...)));
	}


	// FUNCTION TEMPLATE _For_each_tuple_element
template<class _Tpl,
	class _Fx,
	size_t... _Indices> inline
	void _For_each_tuple_element_impl(_Tpl&& _Tuple,
		_Fx _Func, integer_sequence<size_t, _Indices...>)
	{	// call _Func() on the _Indices elements of _Tuple
	int _Ignored[] = { (static_cast<void>(_Func(
		_STD get<_Indices>(_STD forward<_Tpl>(_Tuple))
		)), 0)... };
	(void)_Ignored;
	}

template<class _Tpl,
	class _Fx> inline
	void _For_each_tuple_element(_Tpl&& _Tuple, _Fx _Func)
	{	// call _Func() on each element in _Tuple
	_For_each_tuple_element_impl(
		_STD forward<_Tpl>(_Tuple),
		_Func,
		make_integer_sequence<size_t,
			tuple_size<remove_reference_t<_Tpl>>::value>()
		);
	}


	// TEMPLATE CONSTRUCTOR pair::pair(tuple, tuple, sequence, sequence)
template<class _Ty1,
	class _Ty2>
	template<class _Tuple1,
		class _Tuple2,
		size_t... _Indexes1,
		size_t... _Indexes2> inline
		pair<_Ty1, _Ty2>::pair(_Tuple1& _Val1,
			_Tuple2& _Val2,
			integer_sequence<size_t, _Indexes1...>,
			integer_sequence<size_t, _Indexes2...>)
		: first(_STD get<_Indexes1>(_STD move(_Val1))...),
			second(_STD get<_Indexes2>(_STD move(_Val2))...)
		{	// construct from pair of tuples
		(void) _Val1;	// TRANSITION, VSO#181496
		(void) _Val2;
		}

	// TEMPLATE CONSTRUCTOR pair::pair(piecewise_construct_t, tuple, tuple)
template<class _Ty1,
	class _Ty2>
	template<class... _Types1,
		class... _Types2> inline
		pair<_Ty1, _Ty2>::pair(piecewise_construct_t,
			tuple<_Types1...> _Val1,
			tuple<_Types2...> _Val2)
		: pair(_Val1, _Val2,
			make_integer_sequence<size_t, sizeof...(_Types1)>(),
			make_integer_sequence<size_t, sizeof...(_Types2)>())
		{	// construct from pair of tuples
		}

_STD_END

namespace std {
	// TEMPLATE STRUCT uses_allocator
template<class... _Types,
	class _Alloc>
	struct uses_allocator<tuple<_Types...>, _Alloc>
		: true_type
	{	// true_type if container allocator enabled
	};

}	// namespace std

#if _HAS_TR1_NAMESPACE
_STD_BEGIN
namespace tr1 {	// TR1 ADDITIONS
using _STD get;
using _STD ignore;
using _STD make_tuple;
using _STD ref;
using _STD tie;
using _STD tuple;
}	// namespace tr1
_STD_END
#endif /* _HAS_TR1_NAMESPACE */

 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _TUPLE_ */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
