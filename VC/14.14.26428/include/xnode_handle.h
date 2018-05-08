// xnode_handle.h internal header
// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once
#ifndef _XNODE_HANDLE_H
#define _XNODE_HANDLE_H
#ifndef RC_INVOKED
#include <xmemory0>

#if !_HAS_CXX17
 #error Node handles are only available with C++17. (Also, you should not include this internal header.)
#endif /* _HAS_CXX17 */

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,_STL_WARNING_LEVEL)
 #pragma warning(disable: _STL_DISABLED_WARNINGS)
 _STL_DISABLE_CLANG_WARNINGS
 #pragma push_macro("new")
 #undef new
_STD_BEGIN

		// STRUCT TEMPLATE _Insert_return_type
template<class _Iterator,
	class _NodeType>
	struct _Insert_return_type
	{
	_Iterator position;
	bool inserted;
	_NodeType node;
	};

		// CLASS TEMPLATE _Node_handle
template<class _Derived_type,
	class _KeyTy,
	class _ValueTy>
	struct _Node_handle_map_base
	{	// map-specific node handle behavior
	using key_type = _KeyTy;
	using mapped_type = _ValueTy;

	key_type& key() const _NOEXCEPT // strengthened
		{
		return (_Datum().first);
		}
	mapped_type& mapped() const _NOEXCEPT // strengthened
		{
		return (_Datum().second);
		}
private:
	using _Pair_type = pair<key_type, mapped_type>;
	_Pair_type& _Datum() const
		{
		const auto& _Self = static_cast<const _Derived_type&>(*this);
		auto& _Data = _Self._Getptr()->_Myval;
		// NB: Node handle requires formally-undefined behavior *somewhere*, this
		// implementation chooses to type-pun pair<const K, T> as pair<K, T>.
		return (reinterpret_cast<_Pair_type&>(_Data));
		}
	};

template<class _Derived_type,
	class _ValueTy>
	struct _Node_handle_set_base
	{	// set-specific node handle behavior
	using value_type = _ValueTy;

	value_type& value() const _NOEXCEPT // strengthened
		{
		const auto& _Self = static_cast<const _Derived_type&>(*this);
		return (_Self._Getptr()->_Myval);
		}
	};

template<class _Node,
	class _Alloc,
	template <class...> class _Base,
	class... _Types>
	class _Node_handle
		: public _Base<_Node_handle<_Node, _Alloc, _Base, _Types...>, _Types...>
	{	// storage for a node from one of the node-based standard containers
public:
	using allocator_type = _Alloc;
private:
	using _Alty_traits = allocator_traits<_Alloc>;
	using _Alnode = _Rebind_alloc_t<_Alloc, _Node>;
	using _Alnode_traits = allocator_traits<_Alnode>;
	using _Nodeptr = typename _Alnode_traits::pointer;

	_Nodeptr _Ptr{};
	aligned_union_t<0, _Alloc> _Alloc_storage;	// Invariant: contains a live _Alloc iff _Ptr != nullptr

	void _Clear() _NOEXCEPT
		{	// destroy any contained node and return to the empty state
		if (_Ptr)
			{
			_Alloc& _Al = _Getal();
			_Alnode _Node_alloc{_Al};
			_Alnode_traits::destroy(_Node_alloc, _STD addressof(_Ptr->_Myval));
			_Node::_Freenode0(_Al, _Ptr);
			_Destroy_in_place(_Al);
			_Ptr = nullptr;
			}
		}

	_Node_handle(const _Nodeptr _My_ptr, const _Alloc& _My_alloc) _NOEXCEPT
		: _Ptr{_My_ptr}
		{	// Initialize a _Node_handle that holds the specified node
			// pre: _My_ptr != nullptr
			// pre: _Alloc can release _Ptr
		_Construct_in_place(_Getal(), _My_alloc);
		}

public:
	constexpr _Node_handle() _NOEXCEPT
		: _Alloc_storage{}
		{	// initialize node handle in the empty state
		}

	~_Node_handle() _NOEXCEPT
		{	// destroy any contained node/allocator
		_Clear();
		}

	_Node_handle(_Node_handle&& _That) _NOEXCEPT
		: _Ptr{_That._Ptr}
		{	// steal node and allocator (if any) from _That
		if (_Ptr)
			{
			_That._Ptr = nullptr;
			_Alloc& _That_al = _That._Getal();
			_Construct_in_place(_Getal(), _STD move(_That_al));
			_Destroy_in_place(_That_al);
			}
		}

	_Node_handle& operator=(_Node_handle&& _That) _NOEXCEPT // strengthened
		{	// steal state from _That
		if (!_Ptr)
			{
			if (_That._Ptr)
				{
				_Alloc& _That_al = _That._Getal();
				_Construct_in_place(_Getal(), _STD move(_That_al));
				_Destroy_in_place(_That_al);
				_Ptr = _STD exchange(_That._Ptr, nullptr);
				}

			return (*this);
			}

		if (!_That._Ptr || this == _STD addressof(_That))
			{
			_Clear();
			return (*this);
			}

		_Alloc& _Al = _Getal();
		_Alnode _Node_alloc{_Al};
		_Alnode_traits::destroy(_Node_alloc, _STD addressof(_Ptr->_Myval));
		_Alnode_traits::deallocate(_Node_alloc, _Ptr, 1);

		_Alloc& _That_al = _That._Getal();
#if 1  /* TRANSITION, if constexpr */
		_Pocma(_Al, _That_al);
#else  /* TRANSITION, if constexpr */
		if constexpr (_Alty_traits::propagate_on_container_move_assignment::value)
			{
			_Al = _STD move(_That_al);
			}
#endif /* TRANSITION, if constexpr */
		_Destroy_in_place(_That_al);

		_Ptr = _STD exchange(_That._Ptr, nullptr);
		return (*this);
		}

	_Nodeptr _Getptr() const _NOEXCEPT
		{
		return (_Ptr);
		}

	_Alloc& _Getal() _NOEXCEPT
		{	// pre: !empty()
		return (reinterpret_cast<_Alloc&>(_Alloc_storage));
		}
	const _Alloc& _Getal() const _NOEXCEPT
		{	// pre: !empty()
		return (reinterpret_cast<const _Alloc&>(_Alloc_storage));
		}

	_NODISCARD allocator_type get_allocator() const _NOEXCEPT // strengthened
		{	// pre: !empty()
		return (_Getal());
		}

	explicit operator bool() const _NOEXCEPT
		{	// determine if node handle is not empty
		return (_Ptr != nullptr);
		}

	_NODISCARD bool empty() const _NOEXCEPT
		{	// determine if node handle is empty
		return (_Ptr == nullptr);
		}

	_Nodeptr _Release() _NOEXCEPT
		{	// extract the node from *this
			// pre: !empty()
		_Destroy_in_place(_Getal());
		return (_STD exchange(_Ptr, nullptr));
		}

	void swap(_Node_handle& _That) _NOEXCEPT // strengthened
		{	// exchange values of *this and _That
		if (_Ptr)
			{
			if (_That._Ptr)
				{
#if 1  /* TRANSITION, if constexpr */
				_Pocs(_Getal(), _That._Getal());
#else  /* TRANSITION, if constexpr */
				if constexpr (_Alty_traits::propagate_on_container_swap::value)
					{
					_Swap_adl(_Getal(), _That._Getal());
					}
				else if constexpr (_ITERATOR_DEBUG_LEVEL == 2)
					{
					if (_Getal() != _That._Getal())
						{
						_DEBUG_ERROR("Can't swap node handles with non-equal, non-POCS allocators");
						}
					}
#endif /* TRANSITION, if constexpr */
				}
			else
				{
				_Alloc& _Al = _Getal();
				_Construct_in_place(_That._Getal(), _STD move(_Al));
				_Destroy_in_place(_Al);
				}
			}
		else
			{
			if (!_That._Ptr)
				{
				return;
				}

			_Alloc& _That_al = _That._Getal();
			_Construct_in_place(_Getal(), _STD move(_That_al));
			_Destroy_in_place(_That_al);
			}
		_Swap_adl(_Ptr, _That._Ptr);
		}
	friend void swap(_Node_handle& _Left, _Node_handle& _Right) _NOEXCEPT // strengthened, sort of
		{
		_Left.swap(_Right);
		}

	static _Node_handle _Make(const _Nodeptr _Ptr, const allocator_type& _Al)
		{	// initialize a _Node_handle that holds _Ptr and _Al
			// pre: _Ptr != nullptr
			// pre: _Al can release _Ptr
		return (_Node_handle{_Ptr, _Al});
		}
	};

_STD_END
 #pragma pop_macro("new")
 _STL_RESTORE_CLANG_WARNINGS
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _XNODE_HANDLE_H */
