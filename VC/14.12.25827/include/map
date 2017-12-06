// map standard header
#pragma once
#ifndef _MAP_
#define _MAP_
#ifndef RC_INVOKED

#include <tuple>
#include <xtree>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,_STL_WARNING_LEVEL)
 #pragma warning(disable: _STL_DISABLED_WARNINGS)
 #pragma push_macro("new")
 #undef new
_STD_BEGIN
		// CLASS TEMPLATE _Tmap_traits
template<class _Kty,	// key type
	class _Ty,	// mapped type
	class _Pr,	// comparator predicate type
	class _Alloc,	// actual allocator type (should be value allocator)
	bool _Mfl>	// true if multiple equivalent keys are permitted
	class _Tmap_traits
	{	// traits required to make _Tree behave like a map
public:
	using key_type = _Kty;
	using value_type = pair<const _Kty, _Ty>;
	using key_compare = _Pr;
	using allocator_type = _Alloc;
#if _HAS_CXX17
	using node_type = _Node_handle<
		_Tree_node<value_type, typename allocator_traits<_Alloc>::void_pointer>,
		_Alloc, _Node_handle_map_base, _Kty, _Ty>;
#endif /* _HAS_CXX17 */

	enum
		{	// make multi parameter visible as an enum constant
		_Multi = _Mfl};

	class value_compare
		{	// functor for comparing two element values
	public:
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef value_type first_argument_type;
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef value_type second_argument_type;
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bool result_type;

		bool operator()(const value_type& _Left, const value_type& _Right) const
			{	// test if _Left precedes _Right by comparing just keys
			return (comp(_Left.first, _Right.first));
			}

	protected:
		friend _Tree<_Tmap_traits>;

		value_compare(key_compare _Pred)
			: comp(_Pred)
			{	// construct with specified predicate
			}

		key_compare comp;	// the comparator predicate for keys
		};

	template<class _Ty1,
		class _Ty2>
		static const _Kty& _Kfn(const pair<_Ty1, _Ty2>& _Val)
		{	// extract key from element value
		return (_Val.first);
		}
	};

		// CLASS TEMPLATE map
template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty>,
	class _Alloc = allocator<pair<const _Kty, _Ty>>>
	class map
		: public _Tree<_Tmap_traits<_Kty, _Ty, _Pr, _Alloc, false>>
	{	// ordered red-black tree of {key, mapped} values, unique keys
public:
	static_assert(!_ENFORCE_MATCHING_ALLOCATORS || is_same_v<pair<const _Kty, _Ty>, typename _Alloc::value_type>,
		_MISMATCHED_ALLOCATOR_MESSAGE("map<Key, Value, Compare, Allocator>", "pair<const Key, Value>"));

	using _Mybase = _Tree<_Tmap_traits<_Kty, _Ty, _Pr, _Alloc, false>>;
	using key_type = _Kty;
	using mapped_type = _Ty;
	using key_compare = _Pr;
	using value_compare = typename _Mybase::value_compare;
	using value_type = typename _Mybase::value_type;
	using allocator_type = typename _Mybase::allocator_type;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = typename _Mybase::iterator;
	using const_iterator = typename _Mybase::const_iterator;
	using reverse_iterator = typename _Mybase::reverse_iterator;
	using const_reverse_iterator = typename _Mybase::const_reverse_iterator;

	using _Alnode = typename _Mybase::_Alnode;
	using _Alnode_traits = typename _Mybase::_Alnode_traits;
	using _Pairib = typename _Mybase::_Pairib;

#if _HAS_CXX17
	using insert_return_type = _Insert_return_type<iterator, typename _Mybase::node_type>;
#endif /* _HAS_CXX17 */

	map()
		: _Mybase(key_compare())
		{	// construct empty map from defaults
		}

	explicit map(const allocator_type& _Al)
		: _Mybase(key_compare(), _Al)
		{	// construct empty map from defaults, allocator
		}

	map(const map& _Right)
		: _Mybase(_Right, _Alnode_traits::select_on_container_copy_construction(_Right._Getal()))
		{	// construct map by copying _Right
		}

	map(const map& _Right, const allocator_type& _Al)
		: _Mybase(_Right, _Al)
		{	// construct map by copying _Right, allocator
		}

	explicit map(const key_compare& _Pred)
		: _Mybase(_Pred)
		{	// construct empty map from comparator
		}

	map(const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
		{	// construct empty map from comparator and allocator
		}

	template<class _Iter>
		map(_Iter _First, _Iter _Last)
		: _Mybase(key_compare())
		{	// construct map from [_First, _Last), defaults
		insert(_First, _Last);
		}

	template<class _Iter>
		map(_Iter _First, _Iter _Last,
			const key_compare& _Pred)
		: _Mybase(_Pred)
		{	// construct map from [_First, _Last), comparator
		insert(_First, _Last);
		}

	template<class _Iter>
		map(_Iter _First, _Iter _Last, const allocator_type& _Al)
		: _Mybase(key_compare(), _Al)
		{	// construct map from [_First, _Last), allocator
		insert(_First, _Last);
		}

	template<class _Iter>
		map(_Iter _First, _Iter _Last,
			const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
		{	// construct map from [_First, _Last), comparator, and allocator
		insert(_First, _Last);
		}

	map& operator=(const map& _Right)
		{	// assign by copying _Right
		_Mybase::operator=(_Right);
		return (*this);
		}

	map(map&& _Right)
		: _Mybase(_STD move(_Right))
		{	// construct map by moving _Right
		}

	map(map&& _Right, const allocator_type& _Al)
		: _Mybase(_STD move(_Right), _Al)
		{	// construct map by moving _Right, allocator
		}

	map& operator=(map&& _Right)
		_NOEXCEPT_COND(_Alnode_traits::is_always_equal::value
			&& is_nothrow_move_assignable_v<_Pr>)
		{	// assign by moving _Right
		_Mybase::operator=(_STD move(_Right));
		return (*this);
		}

	mapped_type& operator[](key_type&& _Keyval)
		{	// find element matching _Keyval or insert with default mapped
		return (try_emplace(_STD move(_Keyval)).first->second);
		}

	void swap(map& _Right) _NOEXCEPT_COND(_NOEXCEPT_OPER(_Mybase::swap(_Right))) // Strengthened
		{	// exchange contents with non-movable _Right
		_Mybase::swap(_Right);
		}

	using _Mybase::insert;

	template<class _Valty,
		class = enable_if_t<is_constructible_v<value_type, _Valty>>>
		_Pairib insert(_Valty&& _Val)
		{	// insert _Val
		return (this->emplace(_STD forward<_Valty>(_Val)));
		}

	template<class _Valty,
		class = enable_if_t<is_constructible_v<value_type, _Valty>>>
		iterator insert(const_iterator _Where, _Valty&& _Val)
		{	// insert _Val with hint
		return (this->emplace_hint(_Where, _STD forward<_Valty>(_Val)));
		}

	template<class _Keyty,
		class... _Mappedty>
		_Pairib _Try_emplace(_Keyty&& _Keyval,
			_Mappedty&&... _Mapval)
		{	// fail if _Keyval present, else emplace
		iterator _Where = _Mybase::lower_bound(_Keyval);
		if (_Where == _Mybase::end()
			|| _DEBUG_LT_PRED(_Mybase::_Getcomp(),
				_Keyval, _Mybase::_Key(_Where._Ptr)))
			return (_Pairib(
				_Mybase::emplace_hint(_Where,
					piecewise_construct,
					_STD forward_as_tuple(
						_STD forward<_Keyty>(_Keyval)),
					_STD forward_as_tuple(
						_STD forward<_Mappedty>(_Mapval)...)),
				true));
		else
			return (_Pairib(_Where, false));
		}

	template<class... _Mappedty>
		_Pairib try_emplace(const key_type& _Keyval,
			_Mappedty&&... _Mapval)
		{	// fail if _Keyval present, else emplace
		return (_Try_emplace(_Keyval, _STD forward<_Mappedty>(_Mapval)...));
		}

	template<class... _Mappedty>
		iterator try_emplace(const_iterator, const key_type& _Keyval,
			_Mappedty&&... _Mapval)
		{	// fail if _Keyval present, else emplace, ignore hint
		return (_Try_emplace(_Keyval,
			_STD forward<_Mappedty>(_Mapval)...).first);
		}

	template<class... _Mappedty>
		_Pairib try_emplace(key_type&& _Keyval,
			_Mappedty&&... _Mapval)
		{	// fail if _Keyval present, else emplace
		return (_Try_emplace(_STD move(_Keyval),
			_STD forward<_Mappedty>(_Mapval)...));
		}

	template<class... _Mappedty>
		iterator try_emplace(const_iterator, key_type&& _Keyval,
			_Mappedty&&... _Mapval)
		{	// fail if _Keyval present, else emplace, ignore hint
		return (_Try_emplace(_STD move(_Keyval),
			_STD forward<_Mappedty>(_Mapval)...).first);
		}

	template<class _Keyty,
		class _Mappedty>
		_Pairib _Insert_or_assign(_Keyty&& _Keyval,
			_Mappedty&& _Mapval)
		{	// assign if _Keyval present, else insert
		iterator _Where = _Mybase::lower_bound(_Keyval);
		if (_Where == _Mybase::end()
			|| _DEBUG_LT_PRED(_Mybase::_Getcomp(),
				_Keyval, _Mybase::_Key(_Where._Ptr)))
			return (_Pairib(
				_Mybase::emplace_hint(_Where,
					_STD forward<_Keyty>(_Keyval),
					_STD forward<_Mappedty>(_Mapval)),
				true));
		else
			{	// _Keyval present, assign new value
			_Where->second = _STD forward<_Mappedty>(_Mapval);
			return (_Pairib(_Where, false));
			}
		}

	template<class _Mappedty>
		_Pairib insert_or_assign(const key_type& _Keyval,
			_Mappedty&& _Mapval)
		{	// assign if _Keyval present, else insert
		return (_Insert_or_assign(_Keyval,
			_STD forward<_Mappedty>(_Mapval)));
		}

	template<class _Mappedty>
		iterator insert_or_assign(const_iterator, const key_type& _Keyval,
			_Mappedty&& _Mapval)
		{	// assign if _Keyval present, else insert, ignore hint
		return (_Insert_or_assign(_Keyval,
			_STD forward<_Mappedty>(_Mapval)).first);
		}

	template<class _Mappedty>
		_Pairib insert_or_assign(key_type&& _Keyval,
			_Mappedty&& _Mapval)
		{	// assign if _Keyval present, else insert
		return (_Insert_or_assign(_STD move(_Keyval),
			_STD forward<_Mappedty>(_Mapval)));
		}

	template<class _Mappedty>
		iterator insert_or_assign(const_iterator, key_type&& _Keyval,
			_Mappedty&& _Mapval)
		{	// assign if _Keyval present, else insert, ignore hint
		return (_Insert_or_assign(_STD move(_Keyval),
			_STD forward<_Mappedty>(_Mapval)).first);
		}

	map(initializer_list<value_type> _Ilist)
		: _Mybase(key_compare())
		{	// construct from initializer_list, defaults
		insert(_Ilist);
		}

	map(initializer_list<value_type> _Ilist,
		const key_compare& _Pred)
		: _Mybase(_Pred)
		{	// construct from initializer_list, comparator
		insert(_Ilist);
		}

	map(initializer_list<value_type> _Ilist, const allocator_type& _Al)
		: _Mybase(key_compare(), _Al)
		{	// construct from initializer_list, allocator
		insert(_Ilist);
		}

	map(initializer_list<value_type> _Ilist,
		const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
		{	// construct from initializer_list, comparator, and allocator
		insert(_Ilist);
		}

	map& operator=(initializer_list<value_type> _Ilist)
		{	// assign initializer_list
		_Mybase::clear();
		insert(_Ilist);
		return (*this);
		}

	mapped_type& operator[](const key_type& _Keyval)
		{	// find element matching _Keyval or insert with default mapped
		return (try_emplace(_Keyval).first->second);
		}

	mapped_type& at(const key_type& _Keyval)
		{	// find element matching _Keyval
		iterator _Where = _Mybase::lower_bound(_Keyval);
		if (_Where == _Mybase::end()
			|| _Mybase::_Getcomp()(_Keyval, _Mybase::_Key(_Where._Ptr)))
			_Xout_of_range("invalid map<K, T> key");
		return (_Where->second);
		}

	const mapped_type& at(const key_type& _Keyval) const
		{	// find element matching _Keyval
		const_iterator _Where = _Mybase::lower_bound(_Keyval);
		if (_Where == _Mybase::end()
			|| _Mybase::_Getcomp()(_Keyval, _Mybase::_Key(_Where._Ptr)))
			_Xout_of_range("invalid map<K, T> key");
		return (_Where->second);
		}

	using _Mybase::_Unchecked_begin;
	using _Mybase::_Unchecked_end;
	};

template<class _Kty,
	class _Ty,
	class _Pr,
	class _Alloc> inline
	void swap(map<_Kty, _Ty, _Pr, _Alloc>& _Left,
		map<_Kty, _Ty, _Pr, _Alloc>& _Right)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(_Left.swap(_Right)))
	{	// swap _Left and _Right maps
	_Left.swap(_Right);
	}

		// CLASS TEMPLATE multimap
template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty>,
	class _Alloc = allocator<pair<const _Kty, _Ty>>>
	class multimap
		: public _Tree<_Tmap_traits<_Kty, _Ty, _Pr, _Alloc, true>>
	{	// ordered red-black tree of {key, mapped} values, non-unique keys
public:
	static_assert(!_ENFORCE_MATCHING_ALLOCATORS || is_same_v<pair<const _Kty, _Ty>, typename _Alloc::value_type>,
		_MISMATCHED_ALLOCATOR_MESSAGE("multimap<Key, Value, Compare, Allocator>", "pair<const Key, Value>"));

	using _Mybase = _Tree<_Tmap_traits<_Kty, _Ty, _Pr, _Alloc, true>>;
	using key_type = _Kty;
	using mapped_type = _Ty;
	using key_compare = _Pr;
	using value_compare = typename _Mybase::value_compare;
	using value_type = typename _Mybase::value_type;
	using allocator_type = typename _Mybase::allocator_type;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = typename _Mybase::iterator;
	using const_iterator = typename _Mybase::const_iterator;
	using reverse_iterator = typename _Mybase::reverse_iterator;
	using const_reverse_iterator = typename _Mybase::const_reverse_iterator;

	using _Alnode = typename _Mybase::_Alnode;
	using _Alnode_traits = typename _Mybase::_Alnode_traits;

	multimap()
		: _Mybase(key_compare())
		{	// construct empty map from defaults
		}

	explicit multimap(const allocator_type& _Al)
		: _Mybase(key_compare(), _Al)
		{	// construct empty map from defaults, allocator
		}

	multimap(const multimap& _Right)
		: _Mybase(_Right, _Alnode_traits::select_on_container_copy_construction(_Right._Getal()))
		{	// construct map by copying _Right
		}

	multimap(const multimap& _Right, const allocator_type& _Al)
		: _Mybase(_Right, _Al)
		{	// construct map by copying _Right, allocator
		}

	explicit multimap(const key_compare& _Pred)
		: _Mybase(_Pred)
		{	// construct empty map from comparator
		}

	multimap(const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
		{	// construct empty map from comparator and allocator
		}

	template<class _Iter>
		multimap(_Iter _First, _Iter _Last)
		: _Mybase(key_compare())
		{	// construct map from [_First, _Last), defaults
		insert(_First, _Last);
		}

	template<class _Iter>
		multimap(_Iter _First, _Iter _Last,
			const key_compare& _Pred)
		: _Mybase(_Pred)
		{	// construct map from [_First, _Last), comparator
		insert(_First, _Last);
		}

	template<class _Iter>
		multimap(_Iter _First, _Iter _Last,
			const allocator_type& _Al)
		: _Mybase(key_compare(), _Al)
		{	// construct map from [_First, _Last), allocator
		insert(_First, _Last);
		}

	template<class _Iter>
		multimap(_Iter _First, _Iter _Last,
			const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
		{	// construct map from [_First, _Last), comparator, and allocator
		insert(_First, _Last);
		}

	multimap& operator=(const multimap& _Right)
		{	// assign by copying _Right
		_Mybase::operator=(_Right);
		return (*this);
		}

	multimap(multimap&& _Right)
		: _Mybase(_STD move(_Right))
		{	// construct map by moving _Right
		}

	multimap(multimap&& _Right, const allocator_type& _Al)
		: _Mybase(_STD move(_Right), _Al)
		{	// construct map by moving _Right
		}

	multimap& operator=(multimap&& _Right)
		_NOEXCEPT_COND(_Alnode_traits::is_always_equal::value
			&& is_nothrow_move_assignable_v<_Pr>)
		{	// assign by moving _Right
		_Mybase::operator=(_STD move(_Right));
		return (*this);
		}

	template<class... _Valty>
		iterator emplace(_Valty&&... _Val)
		{	// try to insert value_type(_Val...), favoring right side
		return (_Mybase::emplace(_STD forward<_Valty>(_Val)...).first);
		}

	void swap(multimap& _Right) _NOEXCEPT_COND(_NOEXCEPT_OPER(_Mybase::swap(_Right))) // Strengthened
		{	// exchange contents with non-movable _Right
		_Mybase::swap(_Right);
		}

	using _Mybase::insert;

	template<class _Valty,
		class = enable_if_t<is_constructible_v<value_type, _Valty>>>
		iterator insert(_Valty&& _Val)
		{	// insert _Val
		return (this->emplace(_STD forward<_Valty>(_Val)));
		}

	template<class _Valty,
		class = enable_if_t<is_constructible_v<value_type, _Valty>>>
		iterator insert(const_iterator _Where, _Valty&& _Val)
		{	// insert _Val with hint
		return (this->emplace_hint(_Where, _STD forward<_Valty>(_Val)));
		}

	multimap(initializer_list<value_type> _Ilist)
		: _Mybase(key_compare())
		{	// construct from initializer_list, defaults
		insert(_Ilist);
		}

	multimap(initializer_list<value_type> _Ilist,
		const key_compare& _Pred)
		: _Mybase(_Pred)
		{	// construct from initializer_list, comparator
		insert(_Ilist);
		}

	multimap(initializer_list<value_type> _Ilist,
		const allocator_type& _Al)
		: _Mybase(key_compare(), _Al)
		{	// construct from initializer_list, allocator
		insert(_Ilist);
		}

	multimap(initializer_list<value_type> _Ilist,
		const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
		{	// construct from initializer_list, comparator, and allocator
		insert(_Ilist);
		}

	multimap& operator=(initializer_list<value_type> _Ilist)
		{	// assign initializer_list
		_Mybase::clear();
		insert(_Ilist);
		return (*this);
		}

	using _Mybase::_Unchecked_begin;
	using _Mybase::_Unchecked_end;
	};

template<class _Kty,
	class _Ty,
	class _Pr,
	class _Alloc> inline
	void swap(multimap<_Kty, _Ty, _Pr, _Alloc>& _Left,
		multimap<_Kty, _Ty, _Pr, _Alloc>& _Right)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(_Left.swap(_Right)))
	{	// swap _Left and _Right multimaps
	_Left.swap(_Right);
	}
_STD_END
 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _MAP_ */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
