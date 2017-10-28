// generics STL.net header
#ifndef _CLI_GENERICS_
 #define _CLI_GENERICS_

#define _STLCLR_FIELD_ACCESS	internal

namespace Microsoft {
	namespace VisualC {
		namespace StlClr {

	namespace Generic {
//
// GENERIC INTERFACES
//

//
// GENERIC INTERFACE CLASS IBidirectionalContainer
//
generic<typename TValue>
	public interface class IBidirectionalContainer
	{	// interface for a bidirectional container
	unsigned long get_generation();
	};

//
// GENERIC INTERFACE CLASS IRandomAccessContainer
//
generic<typename TValue>
	public interface class IRandomAccessContainer
	{	// interface for a random-access container
	typedef int size_type;
//	typedef int difference_type;
	typedef TValue value_type;
	typedef value_type% reference;
//	typedef value_type% const_reference;

	bool valid_bias(size_type);
	reference at_bias(size_type);
	};

//
// GENERIC INTERFACE CLASS INode
//
generic<typename TValue>
	public interface class INode
	{	// interface for a node
	typedef INode<TValue> _Mynode_it;
	typedef IBidirectionalContainer<TValue> _Mycont_it;

	_Mycont_it^ container();
	bool is_head();
	_Mynode_it^ next_node();
	_Mynode_it^ prev_node();

	property TValue% _Value;
	};

//
// GENERIC INTERFACE CLASS IBaseIterator
//
generic<typename TValue>
	public interface class IBaseIterator
	:	public System::ICloneable
	{	// interface for base of all iterators, generic
	int get_bias();
	System::Object^ get_node();

	bool valid();
	System::Object^ container();
	void next();
	};

//
// GENERIC INTERFACE CLASS IInputIterator
//
generic<typename TValue>
	public interface class IInputIterator
	:	public IBaseIterator<TValue>
	{	// interface for an input iterator, generic
	typedef IInputIterator<TValue> _Mytype_t;
	typedef TValue value_type;

	bool equal_to(IInputIterator<TValue>^);
	value_type% get_cref();
	};

//
// GENERIC INTERFACE CLASS IOutputIterator
//
generic<typename TValue>
	public interface class IOutputIterator
	:	public IBaseIterator<TValue>
	{	// interface for an output iterator, generic
	typedef TValue value_type;

	value_type% get_ref();
	};

//
// GENERIC INTERFACE CLASS IForwardIterator
//
generic<typename TValue>
	public interface class IForwardIterator
	:	public IInputIterator<TValue>,
		public IOutputIterator<TValue>
	{	// interface for a forward iterator, generic
	};

//
// GENERIC INTERFACE CLASS IBidirectionalIterator
//
generic<typename TValue>
	public interface class IBidirectionalIterator
	:	public IForwardIterator<TValue>
	{	// interface for a bidirectional iterator, generic
	void prev();
	};

//
// GENERIC INTERFACE CLASS IRandomAccessIterator
//
generic<typename TValue>
	public interface class IRandomAccessIterator
	:	public IBidirectionalIterator<TValue>
	{	// interface for a random-access iterator, generic
	typedef IRandomAccessIterator<TValue> _Mytype_t;
	typedef int difference_type;

	difference_type move(difference_type _Offset);
	difference_type distance(IRandomAccessIterator<TValue>^ _Right);
	bool less_than(IRandomAccessIterator<TValue>^ _Right);
	};

//
// GENERIC ITERATOR CLASSES
//

//
// GENERIC REF CLASS ReverseBidirectionalIterator
//
generic<typename TValue>
	public ref class ReverseBidirectionalIterator
	:	public Generic::IBidirectionalIterator<TValue>
	{	// mutable reverse bidirectional iterator, generic
public:
	// types
	typedef ReverseBidirectionalIterator<TValue> _Mytype_t;
	typedef Generic::IBidirectionalIterator<TValue> _Myiter_t;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ReverseBidirectionalIterator()
		:	_Myiter(nullptr)
		{	// construct default
		}

	ReverseBidirectionalIterator(ReverseBidirectionalIterator% _Right)
		:	_Myiter((_Myiter_t^)_Right._Myiter->Clone())
		{	// construct by copying an iterator
		}

	ReverseBidirectionalIterator% operator=(
		ReverseBidirectionalIterator% _Right)
		{	// assign an iterator
		if ((Object^)this != %_Right)
			_Myiter = (_Myiter_t^)_Right._Myiter->Clone();
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors and special member functions
	ReverseBidirectionalIterator(_Myiter_t^ _Iter)
		:	_Myiter((_Myiter_t^)_Iter->Clone())
		{	// construct from wrapped iterator
		}

	_Myiter_t^ base()
		{	// get underlying base iterator
		return (_Myiter);
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ReverseBidirectionalIterator(
			(_Myiter_t^)_Myiter->Clone()));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (0);
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (_Myiter->get_node());
		}

	virtual bool valid()
		{	// test if wrapped iterator valid
		return (_Myiter->valid());
		}

	virtual System::Object^ container()
		{	// return container for wrapped iterator
		return (_Myiter->container());
		}

	virtual void next()
		{	// decrement wrapped iterator
		_Myiter->prev();
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1011")]
	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ReverseBidirectionalIterator% _Right)
		{	// test if wrapped iterators are equal, class specific
		return (_Myiter->equal_to(_Right._Myiter));
		}

	virtual const_reference get_cref()
		{	// return const reference from wrapped iterator
		_Myiter->prev();
		const_reference _Ref = _Myiter->get_cref();
		_Myiter->next();
		return (_Ref);
		}

	virtual reference get_ref()
		{	// return reference from wrapped iterator
		_Myiter->prev();
		reference _Ref = _Myiter->get_ref();
		_Myiter->next();
		return (_Ref);
		}

	virtual void prev()
		{	// increment wrapped iterator
		_Myiter->next();
		}

//	difference_type move(difference_type _Offset);
//	difference_type distance(
//		Generic::IRandomAccessIterator<TValue>^ _Right);
//	difference_type distance(ReverseBidirectionalIterator^ _Right);
//	bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right);
//	bool less_than(ReverseBidirectionalIterator^ _Right);

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator->(ReverseBidirectionalIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_ref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator*(ReverseBidirectionalIterator% _Left)
		{	// return reference to designated element
		return (_Left.get_ref());
		}

	ReverseBidirectionalIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ReverseBidirectionalIterator operator++(int)
		{	// return incremented
		ReverseBidirectionalIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

	bool operator==(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator!=(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ReverseBidirectionalIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ReverseBidirectionalIterator operator--(int)
		{	// return decremented
		ReverseBidirectionalIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

//	ReverseBidirectionalIterator^ operator+(difference_type _Right);
//	static ReverseBidirectionalIterator^
//		operator+(difference_type _Left, ReverseBidirectionalIterator _Right);
//	ReverseBidirectionalIterator^ operator-(difference_type _Right);
//	difference_type operator-(ReverseBidirectionalIterator^ _Right);
//	bool operator<(ReverseBidirectionalIterator^ _Right);
//	bool operator>=(ReverseBidirectionalIterator^ _Right);
//	bool operator>(ReverseBidirectionalIterator^ _Right);
//	bool operator<=(ReverseBidirectionalIterator^ _Right);

_STLCLR_FIELD_ACCESS:
	_Myiter_t^ _Myiter;	// wrapped iterator
	};

//
// GENERIC REF CLASS ConstReverseBidirectionalIterator
//
generic<typename TValue>
	public ref class ConstReverseBidirectionalIterator
	:	public Generic::IBidirectionalIterator<TValue>
	{	// nonmutable reverse bidirectional iterator, generic
public:
	// types
	typedef ConstReverseBidirectionalIterator<TValue> _Mytype_t;
	typedef Generic::IBidirectionalIterator<TValue> _Myiter_t;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ConstReverseBidirectionalIterator()
		:	_Myiter(nullptr)
		{	// construct default
		}

	ConstReverseBidirectionalIterator(
		ConstReverseBidirectionalIterator% _Right)
		:	_Myiter((_Myiter_t^)_Right._Myiter->Clone())
		{	// construct by copying an iterator
		}

	ConstReverseBidirectionalIterator% operator=(
		ConstReverseBidirectionalIterator% _Right)
		{	// assign an iterator
		if ((Object^)this != %_Right)
			_Myiter = (_Myiter_t^)_Right._Myiter->Clone();
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors and special member functions
	ConstReverseBidirectionalIterator(_Myiter_t^ _Iter)
		:	_Myiter((_Myiter_t^)_Iter->Clone())
		{	// construct from wrapped iterator
		}

	ConstReverseBidirectionalIterator(
		ReverseBidirectionalIterator<TValue>% _Iter)
		:	_Myiter((_Myiter_t^)_Iter.base()->Clone())
		{	// construct from mutable iterator
		}

	ConstReverseBidirectionalIterator%
		operator=(ReverseBidirectionalIterator<TValue>% _Right)
		{	// assign from mutable iterator
		_Myiter = (_Myiter_t^)_Right.base()->Clone();
		return (*this);
		}

	_Myiter_t^ base()
		{	// get underlying base iterator
		return (_Myiter);
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ConstReverseBidirectionalIterator(
			(_Myiter_t^)_Myiter->Clone()));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (0);
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (_Myiter->get_node());
		}

	virtual bool valid()
		{	// test if wrapped iterator valid
		return (_Myiter->valid());
		}

	virtual System::Object^ container()
		{	// return container for wrapped iterator
		return (_Myiter->container());
		}

	virtual void next()
		{	// decrement wrapped iterator
		_Myiter->prev();
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ConstReverseBidirectionalIterator% _Right)
		{	// test if wrapped iterators are equal, class specific
		return (_Myiter->equal_to(_Right._Myiter));
		}

	virtual const_reference get_cref()
		{	// return const reference from wrapped iterator
		_Myiter->prev();
		const_reference _Ref = _Myiter->get_cref();
		_Myiter->next();
		return (_Ref);
		}

	virtual reference get_ref()
		{	// return reference from wrapped iterator
#pragma warning(push)
#pragma warning(disable: 4715)
		throw gcnew System::InvalidOperationException();
#pragma warning(pop)
		}

	virtual void prev()
		{	// increment wrapped iterator
		_Myiter->next();
		}

//	difference_type move(difference_type _Offset);
//	difference_type distance(
//		Generic::IRandomAccessIterator<TValue>^ _Right);
//	difference_type distance(ConstReverseBidirectionalIterator^ _Right);
//	bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right);
//	bool less_than(ConstReverseBidirectionalIterator^ _Right);

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator->(
		ConstReverseBidirectionalIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_cref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator*(
		ConstReverseBidirectionalIterator% _Left)
		{	// return const reference to designated element
		return (_Left.get_cref());
		}

	ConstReverseBidirectionalIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ConstReverseBidirectionalIterator operator++(int)
		{	// return incremented
		ConstReverseBidirectionalIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

	bool operator==(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator!=(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ConstReverseBidirectionalIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ConstReverseBidirectionalIterator operator--(int)
		{	// return decremented
		ConstReverseBidirectionalIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

//	ConstReverseBidirectionalIterator^ operator+(difference_type _Right);
//	static ConstReverseBidirectionalIterator^
//		operator+(difference_type _Left,
//			ConstReverseBidirectionalIterator _Right);
//	ConstReverseBidirectionalIterator^ operator-(difference_type _Right);
//	difference_type operator-(ConstReverseBidirectionalIterator^ _Right);
//	bool operator<(ConstReverseBidirectionalIterator^ _Right);
//	bool operator>=(ConstReverseBidirectionalIterator^ _Right);
//	bool operator>(ConstReverseBidirectionalIterator^ _Right);
//	bool operator<=(ConstReverseBidirectionalIterator^ _Right);

_STLCLR_FIELD_ACCESS:
	_Myiter_t^ _Myiter;	// wrapped iterator
	};

//
// GENERIC REF CLASS ReverseRandomAccessIterator
//
generic<typename TValue>
	public ref class ReverseRandomAccessIterator
	:	public Generic::IRandomAccessIterator<TValue>
	{	// mutable reverse random_access iterator, generic
public:
	// types
	typedef ReverseRandomAccessIterator<TValue> _Mytype_t;
	typedef Generic::IRandomAccessIterator<TValue> _Myiter_t;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ReverseRandomAccessIterator()
		:	_Myiter(nullptr)
		{	// construct default
		}

	ReverseRandomAccessIterator(ReverseRandomAccessIterator% _Right)
		:	_Myiter((_Myiter_t^)_Right._Myiter->Clone())
		{	// construct by copying an iterator
		}

	ReverseRandomAccessIterator% operator=(
		ReverseRandomAccessIterator% _Right)
		{	// assign an iterator
		if ((Object^)this != %_Right)
			_Myiter = (_Myiter_t^)_Right._Myiter->Clone();
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors and special member functions
	ReverseRandomAccessIterator(_Myiter_t^ _Iter)
		:	_Myiter((_Myiter_t^)_Iter->Clone())
		{	// construct from wrapped iterator
		}

	_Myiter_t^ base()
		{	// get underlying base iterator
		return (_Myiter);
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ReverseRandomAccessIterator(
			(_Myiter_t^)_Myiter->Clone()));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (_Myiter->get_bias());
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (nullptr);
		}

	virtual bool valid()
		{	// test if wrapped iterator valid
		return (_Myiter->valid());
		}

	virtual System::Object^ container()
		{	// return container for wrapped iterator
		return (_Myiter->container());
		}

	virtual void next()
		{	// decrement wrapped iterator
		_Myiter->prev();
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ReverseRandomAccessIterator% _Right)
		{	// test if wrapped iterators are equal, class specific
		return (_Myiter->equal_to(_Right._Myiter));
		}

	virtual const_reference get_cref()
		{	// return const reference from wrapped iterator
		_Myiter->prev();
		const_reference _Ref = _Myiter->get_cref();
		_Myiter->next();
		return (_Ref);
		}

	virtual reference get_ref()
		{	// return reference from wrapped iterator
		_Myiter->prev();
		reference _Ref = _Myiter->get_ref();
		_Myiter->next();
		return (_Ref);
		}

	virtual void prev()
		{	// increment wrapped iterator
		_Myiter->next();
		}

	virtual difference_type move(difference_type _Offset)
		{	// incremented by integer
		return (_Myiter->move(-_Offset));	// can overflow
		}

	virtual difference_type distance(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (_Right->get_bias() - get_bias());
		}

	difference_type distance(ReverseRandomAccessIterator% _Right)
		{	// return difference of two iterators, class specific
		return (_Myiter->distance(_Right._Myiter));
		}

	virtual bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this < _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (!(get_bias() < _Right->get_bias()));
		}

	bool less_than(ReverseRandomAccessIterator% _Right)
		{	// test if *this < _Right, class specific
		return (!_Myiter->less_than(_Right));
		}

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator->(ReverseRandomAccessIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_ref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator*(ReverseRandomAccessIterator% _Left)
		{	// return reference to designated element
		return (_Left.get_ref());
		}

	ReverseRandomAccessIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ReverseRandomAccessIterator operator++(int)
		{	// return incremented
		ReverseRandomAccessIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

//	bool operator==(Generic::IInputIterator<TValue>^ _Right);

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

//	bool operator!=(Generic::IInputIterator<TValue>^ _Right);

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ReverseRandomAccessIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ReverseRandomAccessIterator operator--(int)
		{	// return decremented
		ReverseRandomAccessIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

	ReverseRandomAccessIterator operator+(difference_type _Right)
		{	// return incremented by integer
		ReverseRandomAccessIterator _Iter = *this;

		_Iter.move(_Right);
		return (_Iter);
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static ReverseRandomAccessIterator operator+(difference_type _Left,
		ReverseRandomAccessIterator% _Right)
		{	// return incremented by integer
		ReverseRandomAccessIterator _Iter = _Right;

		_Iter.move(_Left);
		return (_Iter);
		}

	ReverseRandomAccessIterator operator-(difference_type _Right)
		{	// return decremented by integer
		ReverseRandomAccessIterator _Iter = *this;

		_Iter.move(-_Right);	// can overflow
		return (_Iter);
		}

	difference_type operator-(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		return (distance(_Right));
		}

//	bool operator<(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator<(_Mytype_t% _Right)
		{	// test if *this < _Right
		return (less_than(_Right));
		}

//	bool operator>=(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator>=(_Mytype_t% _Right)
		{	// test if *this >= _Right
		return (!(*this < _Right));
		}

//	bool operator>(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator>(_Mytype_t% _Right)
		{	// test if *this > _Right
		return (_Right < *this);
		}

//	bool operator<=(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator<=(_Mytype_t% _Right)
		{	// test if *this <= _Right
		return (!(_Right < *this));
		}

	property value_type default[difference_type]
		{	// get or set subscripted element
		value_type get(difference_type _Pos)
			{	// get _Pos element
			ReverseRandomAccessIterator _Where = *this + _Pos;

			return (_Where.get_ref());
			}

		void set(difference_type _Pos, value_type _Val)
			{	// set _Pos element
			ReverseRandomAccessIterator _Where = *this + _Pos;

			_Where.get_ref() = _Val;
			}
		};

_STLCLR_FIELD_ACCESS:
	_Myiter_t^ _Myiter;	// wrapped iterator
	};

//
// GENERIC REF CLASS ConstReverseRandomAccessIterator
//
generic<typename TValue>
	public ref class ConstReverseRandomAccessIterator
	:	public Generic::IRandomAccessIterator<TValue>
	{	// nonmutable reverse random_access iterator, generic
public:
	// types
	typedef ConstReverseRandomAccessIterator<TValue> _Mytype_t;
	typedef Generic::IRandomAccessIterator<TValue> _Myiter_t;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ConstReverseRandomAccessIterator()
		:	_Myiter(nullptr)
		{	// construct default
		}

	ConstReverseRandomAccessIterator(
		ConstReverseRandomAccessIterator% _Right)
		:	_Myiter((_Myiter_t^)_Right._Myiter->Clone())
		{	// construct by copying an iterator
		}

	ConstReverseRandomAccessIterator% operator=(
		ConstReverseRandomAccessIterator% _Right)
		{	// assign an iterator
		if ((Object^)this != %_Right)
			_Myiter = (_Myiter_t^)_Right._Myiter->Clone();
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors and special member functions
	ConstReverseRandomAccessIterator(_Myiter_t^ _Iter)
		:	_Myiter((_Myiter_t^)_Iter->Clone())
		{	// construct from wrapped iterator
		}

	ConstReverseRandomAccessIterator(
		ReverseRandomAccessIterator<TValue>% _Iter)
		:	_Myiter((_Myiter_t^)_Iter.base()->Clone())
		{	// construct from mutable iterator
		}

	ConstReverseRandomAccessIterator% operator=(
		ReverseRandomAccessIterator<TValue>% _Right)
		{	// assign from mutable iterator
		_Myiter = (_Myiter_t^)_Right.base()->Clone();
		return (*this);
		}

	_Myiter_t^ base()
		{	// get underlying base iterator
		return (_Myiter);
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ConstReverseRandomAccessIterator(
			(_Myiter_t^)_Myiter->Clone()));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (_Myiter->get_bias());
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (nullptr);
		}

	virtual bool valid()
		{	// test if wrapped iterator valid
		return (_Myiter->valid());
		}

	virtual System::Object^ container()
		{	// return container for wrapped iterator
		return (_Myiter->container());
		}

	virtual void next()
		{	// decrement wrapped iterator
		_Myiter->prev();
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ConstReverseRandomAccessIterator% _Right)
		{	// test if wrapped iterators are equal, class specific
		return (_Myiter->equal_to(_Right._Myiter));
		}

	virtual const_reference get_cref()
		{	// return const reference from wrapped iterator
		_Myiter->prev();
		const_reference _Ref = _Myiter->get_cref();
		_Myiter->next();
		return (_Ref);
		}

	virtual reference get_ref()
		{	// return reference from wrapped iterator
#pragma warning(push)
#pragma warning(disable: 4715)
		throw gcnew System::InvalidOperationException();
#pragma warning(pop)
		}

	virtual void prev()
		{	// increment wrapped iterator
		_Myiter->next();
		}

	virtual difference_type move(difference_type _Offset)
		{	// incremented by integer
		return (_Myiter->move(-_Offset));	// can overflow
		}

	virtual difference_type distance(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (_Right->get_bias() - get_bias());
		}

	difference_type distance(ConstReverseRandomAccessIterator% _Right)
		{	// return difference of two iterators, class specific
		return (_Myiter->distance(_Right._Myiter));
		}

	virtual bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this < _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (!(get_bias() < _Right->get_bias()));
		}

	bool less_than(ConstReverseRandomAccessIterator% _Right)
		{	// test if *this < _Right, class specific
		return (!_Myiter->less_than(_Right));
		}

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator->(
		ConstReverseRandomAccessIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_cref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator*(
		ConstReverseRandomAccessIterator% _Left)
		{	// return const reference to designated element
		return (_Left.get_cref());
		}

	ConstReverseRandomAccessIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ConstReverseRandomAccessIterator operator++(int)
		{	// return incremented
		ConstReverseRandomAccessIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

//	bool operator==(Generic::IInputIterator<TValue>^ _Right);

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

//	bool operator!=(Generic::IInputIterator<TValue>^ _Right);

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ConstReverseRandomAccessIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ConstReverseRandomAccessIterator operator--(int)
		{	// return decremented
		ConstReverseRandomAccessIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

	ConstReverseRandomAccessIterator operator+(difference_type _Right)
		{	// return incremented by integer
		ConstReverseRandomAccessIterator _Iter = *this;

		_Iter.move(_Right);
		return (_Iter);
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static ConstReverseRandomAccessIterator operator+(difference_type _Left,
		ConstReverseRandomAccessIterator% _Right)
		{	// return incremented by integer
		ConstReverseRandomAccessIterator _Iter = _Right;

		_Iter.move(_Left);
		return (_Iter);
		}

	ConstReverseRandomAccessIterator operator-(difference_type _Right)
		{	// return decremented by integer
		ConstReverseRandomAccessIterator _Iter = *this;

		_Iter.move(-_Right);	// can overflow
		return (_Iter);
		}

	difference_type operator-(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		return (distance(_Right));
		}

//	bool operator<(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator<(_Mytype_t% _Right)
		{	// test if *this < _Right
		return (less_than(_Right));
		}

//	bool operator>=(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator>=(_Mytype_t% _Right)
		{	// test if *this >= _Right
		return (!(*this < _Right));
		}

//	bool operator>(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator>(_Mytype_t% _Right)
		{	// test if *this > _Right
		return (_Right < *this);
		}

//	bool operator<=(Generic::IRandomAccessIterator<TValue>^ _Right);

	bool operator<=(_Mytype_t% _Right)
		{	// test if *this <= _Right
		return (!(_Right < *this));
		}

	property const_reference default[difference_type]
		{	// get subscripted element
		const_reference get(difference_type _Pos)
			{	// get _Pos element
			ConstReverseRandomAccessIterator _Where = *this + _Pos;

			return (_Where.get_cref());
			}
		};

_STLCLR_FIELD_ACCESS:
	_Myiter_t^ _Myiter;	// wrapped iterator
	};

//
// GENERIC REF CLASS ContainerBidirectionalIterator
//
generic<typename TValue>
	public ref class ContainerBidirectionalIterator
	:	public Generic::IBidirectionalIterator<TValue>
	{	// iterator for mutable bidirectional container
public:
	// types
	typedef ContainerBidirectionalIterator<TValue> _Mytype_t;
	typedef Generic::IBidirectionalIterator<TValue> _Myiter_t;
	typedef Generic::IBidirectionalContainer<TValue> TCont;
	typedef INode<TValue> _Mynode_it;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ContainerBidirectionalIterator()
		:	_Mynode(nullptr)
		{	// construct default
		}

	ContainerBidirectionalIterator(ContainerBidirectionalIterator% _Right)
		:	_Mynode(_Right._Mynode)
		{	// construct by copying an iterator
		}

	ContainerBidirectionalIterator% operator=(
		ContainerBidirectionalIterator% _Right)
		{	// assign an iterator
		_Mynode = _Right._Mynode;
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors
	ContainerBidirectionalIterator(_Mynode_it^ _Node)
		:	_Mynode(_Node)
		{	// construct from node
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ContainerBidirectionalIterator(_Mynode));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (0);
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (_Mynode);
		}

	virtual bool valid()
		{	// test if iterator valid
		return (_Mynode != nullptr && _Mynode->container() != nullptr);
		}

	virtual System::Object^ container()
		{	// return owning container
		return (_Mynode == nullptr ? nullptr : _Mynode->container());
		}

	virtual void next()
		{	// increment
		_Mynode = _Mynode->next_node();
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ContainerBidirectionalIterator% _Right)
		{	// test if *this == _Right, class specific
		if (container() == nullptr
			|| container() != _Right.container())
			throw gcnew System::ArgumentException();
		return (_Mynode == _Right._Mynode);
		}

	virtual const_reference get_cref()
		{	// return const reference to designated element
		return (_Mynode->_Value);
		}

	virtual reference get_ref()
		{	// return reference to designated element
		return (_Mynode->_Value);
		}

	virtual void prev()
		{	// decrement
		_Mynode = _Mynode->prev_node();
		}

//	difference_type move(difference_type _Offset);
//	difference_type distance(
//		Generic::IRandomAccessIterator<TValue>^ _Right);
//	bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right);

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator->(ContainerBidirectionalIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_ref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator*(ContainerBidirectionalIterator% _Left)
		{	// return reference to designated element
		return (_Left.get_ref());
		}

	ContainerBidirectionalIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ContainerBidirectionalIterator operator++(int)
		{	// return incremented
		ContainerBidirectionalIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

	bool operator==(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator!=(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ContainerBidirectionalIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ContainerBidirectionalIterator operator--(int)
		{	// return decremented
		ContainerBidirectionalIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

//	ContainerBidirectionalIterator^ operator+(difference_type _Right);
//	static ContainerBidirectionalIterator^ operator+(
//		difference_type _Left, ContainerBidirectionalIterator _Right);
//	ContainerBidirectionalIterator^ operator-(difference_type _Right);
//	difference_type operator-(ContainerBidirectionalIterator^ _Right);
//	bool operator<(ContainerBidirectionalIterator^ _Right);
//	bool operator>=(ContainerBidirectionalIterator^ _Right);
//	bool operator>(ContainerBidirectionalIterator^ _Right);
//	bool operator<=(ContainerBidirectionalIterator^ _Right);

_STLCLR_FIELD_ACCESS:
	// data members
	_Mynode_it^ _Mynode;	// node into list
	};

//
// GENERIC REF CLASS ConstContainerBidirectionalIterator
//
generic<typename TValue>
	public ref class ConstContainerBidirectionalIterator
	:	public Generic::IBidirectionalIterator<TValue>
	{	// iterator for nonmutable bidirectional container
public:
	// types
	typedef ConstContainerBidirectionalIterator<TValue> _Mytype_t;
	typedef Generic::IBidirectionalIterator<TValue> _Myiter_t;
	typedef Generic::IBidirectionalContainer<TValue> TCont;
	typedef INode<TValue> _Mynode_it;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ConstContainerBidirectionalIterator()
		:	_Mynode(nullptr)
		{	// construct default
		}

	ConstContainerBidirectionalIterator(
		ConstContainerBidirectionalIterator% _Right)
		:	_Mynode(_Right._Mynode)
		{	// construct by copying an iterator
		}

	ConstContainerBidirectionalIterator% operator=(
		ConstContainerBidirectionalIterator% _Right)
		{	// assign an iterator
		_Mynode = _Right._Mynode;
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors and special members
	ConstContainerBidirectionalIterator(_Mynode_it^ _Node)
		:	_Mynode(_Node)
		{	// construct from node
		}

	ConstContainerBidirectionalIterator(
		Generic::ContainerBidirectionalIterator<TValue>% _Right)
		:	_Mynode((_Mynode_it^)_Right.get_node())
		{	// construct from mutable iterator
		}

	ConstContainerBidirectionalIterator% operator=(
		Generic::ContainerBidirectionalIterator<TValue>% _Right)
		{	// assign from mutable iterator
		_Mynode = (_Mynode_it^)_Right.get_node();
		return (*this);
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ConstContainerBidirectionalIterator(_Mynode));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (0);
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (_Mynode);
		}

	virtual bool valid()
		{	// test if iterator valid
		return (container() != nullptr);
		}

	virtual System::Object^ container()
		{	// return owning container
		return (_Mynode == nullptr ? nullptr : _Mynode->container());
		}

	virtual void next()
		{	// increment
		_Mynode = _Mynode->next_node();
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ConstContainerBidirectionalIterator% _Right)
		{	// test if *this == _Right, class specific
		if (container() == nullptr
			|| container() != _Right.container())
			throw gcnew System::ArgumentException();
		return (_Mynode == _Right._Mynode);
		}

	virtual const_reference get_cref()
		{	// return const reference to designated element
		return (_Mynode->_Value);
		}

	virtual reference get_ref()
		{	// return reference to designated element
#pragma warning(push)
#pragma warning(disable: 4715)
		throw gcnew System::InvalidOperationException();
#pragma warning(pop)
		}

	virtual void prev()
		{	// decrement
		_Mynode = _Mynode->prev_node();
		}

//	difference_type move(difference_type _Offset);
//	difference_type distance(
//		Generic::IRandomAccessIterator<TValue>^ _Right);
//	bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right);

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator->(
		ConstContainerBidirectionalIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_cref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator*(
		ConstContainerBidirectionalIterator% _Left)
		{	// return const reference to designated element
		return (_Left.get_cref());
		}

	ConstContainerBidirectionalIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ConstContainerBidirectionalIterator operator++(int)
		{	// return incremented
		ConstContainerBidirectionalIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

	bool operator==(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator!=(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ConstContainerBidirectionalIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ConstContainerBidirectionalIterator operator--(int)
		{	// return decremented
		ConstContainerBidirectionalIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

//	ConstContainerBidirectionalIterator^ operator+(difference_type _Right);
//	static ConstContainerBidirectionalIterator^ operator+(
//		difference_type _Left, ConstContainerBidirectionalIterator _Right);
//	ConstContainerBidirectionalIterator^ operator-(difference_type _Right);
//	difference_type operator-(ConstContainerBidirectionalIterator^ _Right);
//	bool operator<(ConstContainerBidirectionalIterator^ _Right);
//	bool operator>=(ConstContainerBidirectionalIterator^ _Right);
//	bool operator>(ConstContainerBidirectionalIterator^ _Right);
//	bool operator<=(ConstContainerBidirectionalIterator^ _Right);

_STLCLR_FIELD_ACCESS:
	// data members
	_Mynode_it^ _Mynode;	// node into list
	};

//
// GENERIC REF CLASS ContainerRandomAccessIterator
//
generic<typename TValue>
	public ref class ContainerRandomAccessIterator
	:	public Generic::IRandomAccessIterator<TValue>
	{	// iterator for mutable random-access container
public:
	// types
	typedef ContainerRandomAccessIterator<TValue> _Mytype_t;
	typedef Generic::IRandomAccessIterator<TValue> _Myiter_t;
	typedef Generic::IRandomAccessContainer<TValue> TCont;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ContainerRandomAccessIterator()
		:	_Mycont(nullptr), _Myoffset(0)
		{	// construct default
		}

	ContainerRandomAccessIterator(ContainerRandomAccessIterator% _Right)
		:	_Mycont(_Right._Mycont), _Myoffset(_Right._Myoffset)
		{	// construct by copying an iterator
		}

	ContainerRandomAccessIterator% operator=(
		ContainerRandomAccessIterator% _Right)
		{	// assign an iterator
		_Mycont = _Right._Mycont;
		_Myoffset = _Right._Myoffset;
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors
	ContainerRandomAccessIterator(TCont^ _Cont, int _Offset)
		:	_Mycont(_Cont), _Myoffset(_Offset)
		{	// construct from container pointer and offset
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ContainerRandomAccessIterator(_Mycont, _Myoffset));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (_Myoffset);
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (nullptr);
		}

	virtual bool valid()
		{	// test if iterator valid
		return (container() != nullptr
			&& _Mycont->valid_bias(_Myoffset));
		}

	virtual System::Object^ container()
		{	// return owning container
		return (_Mycont);
		}

	virtual void next()
		{	// increment
		if (!_Mycont->valid_bias(_Myoffset + 1))
			throw gcnew System::InvalidOperationException();
		++_Myoffset;
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ContainerRandomAccessIterator% _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right._Mycont)
			throw gcnew System::ArgumentException();
		return (_Myoffset == _Right._Myoffset);
		}

	virtual const_reference get_cref()
		{	// return const reference to designated element
		return (_Mycont->at_bias(_Myoffset));
		}

	virtual reference get_ref()
		{	// return reference to designated element
		return (_Mycont->at_bias(_Myoffset));
		}

	virtual void prev()
		{	// decrement
		if (!_Mycont->valid_bias(_Myoffset - 1))
			throw gcnew System::InvalidOperationException();
		--_Myoffset;
		}

	virtual difference_type move(difference_type _Offset)
		{	// incremented by integer
		difference_type _Newoffset = _Myoffset + _Offset;	// can overflow

		if (!_Mycont->valid_bias(_Newoffset))
			throw gcnew System::InvalidOperationException();
		_Myoffset = _Newoffset;
		return (_Myoffset);
		}

	virtual difference_type distance(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() - _Right->get_bias());
		}

	difference_type distance(
		_Mytype_t% _Right)
		{	// return difference of two iterators
		if (container() == nullptr
			|| container() != _Right.container())
			throw gcnew System::ArgumentException();
		return (get_bias() - _Right.get_bias());
		}

	virtual bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this < _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() < _Right->get_bias());
		}

	bool less_than(_Mytype_t% _Right)
		{	// test if *this < _Right
		if (container() == nullptr
			|| container() != _Right.container())
			throw gcnew System::ArgumentException();
		return (get_bias() < _Right.get_bias());
		}

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator->(ContainerRandomAccessIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_ref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static reference operator*(ContainerRandomAccessIterator% _Left)
		{	// return reference to designated element
		return (_Left.get_ref());
		}

	ContainerRandomAccessIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ContainerRandomAccessIterator operator++(int)
		{	// return incremented
		ContainerRandomAccessIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

	bool operator==(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator!=(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ContainerRandomAccessIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ContainerRandomAccessIterator operator--(int)
		{	// return decremented
		ContainerRandomAccessIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

	ContainerRandomAccessIterator operator+(difference_type _Right)
		{	// return incremented by integer
		ContainerRandomAccessIterator _Iter = *this;

		_Iter.move(_Right);
		return (_Iter);
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static ContainerRandomAccessIterator operator+(difference_type _Left,
		ContainerRandomAccessIterator% _Right)
		{	// return incremented by integer
		ContainerRandomAccessIterator _Iter = _Right;

		_Iter.move(_Left);
		return (_Iter);
		}

	ContainerRandomAccessIterator operator-(difference_type _Right)
		{	// return decremented by integer
		ContainerRandomAccessIterator _Iter = *this;

		_Iter.move(-_Right);	// can overflow
		return (_Iter);
		}

	difference_type operator-(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		return (distance(_Right));
		}

	bool operator<(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this < _Right
		return (less_than(_Right));
		}

	bool operator<(_Mytype_t% _Right)
		{	// test if *this < _Right
		return (less_than(_Right));
		}

	bool operator>=(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this >= _Right
		return (!(*this < _Right));
		}

	bool operator>=(_Mytype_t% _Right)
		{	// test if *this >= _Right
		return (!(*this < _Right));
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static bool operator>(Generic::IRandomAccessIterator<TValue>^ _Left,
		_Mytype_t% _Right)
		{	// test if _Left > _Right
		return (_Right < _Left);
		}

	bool operator>(_Mytype_t%_Right)
		{	// test if *this > _Right
		return (_Right < *this);
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static bool operator<=(Generic::IRandomAccessIterator<TValue>^ _Left,
		_Mytype_t% _Right)
		{	// test if _Left <= _Right
		return (!(_Right < _Left));
		}

	bool operator<=(_Mytype_t%_Right)
		{	// test if *this <= _Right
		return (!(_Right < *this));
		}

	property value_type default[difference_type]
		{	// get or set subscripted element
		value_type get(difference_type _Pos)
			{	// get _Pos element
			ContainerRandomAccessIterator _Where = *this + _Pos;

			return (*_Where);
			}

		void set(difference_type _Pos, value_type _Val)
			{	// set _Pos element
			ContainerRandomAccessIterator _Where = *this + _Pos;

			*_Where = _Val;
			}
		};

_STLCLR_FIELD_ACCESS:
	// data members
	TCont^ _Mycont;	// owning container
	difference_type _Myoffset;	// offset into container
	};

//
// GENERIC REF CLASS ConstContainerRandomAccessIterator
//
generic<typename TValue>
	public ref class ConstContainerRandomAccessIterator
	:	public Generic::IRandomAccessIterator<TValue>
	{	// iterator for nonmutable random-access container
public:
	// types
	typedef ConstContainerRandomAccessIterator<TValue> _Mytype_t;
	typedef Generic::IRandomAccessIterator<TValue> _Myiter_t;
	typedef Generic::IRandomAccessContainer<TValue> TCont;

	typedef TValue value_type;
	typedef int difference_type;
	typedef value_type% pointer;
	typedef value_type% reference;
	typedef value_type% const_reference;

	// basics
	ConstContainerRandomAccessIterator()
		:	_Mycont(nullptr), _Myoffset(0)
		{	// construct default
		}

	ConstContainerRandomAccessIterator(
		ConstContainerRandomAccessIterator% _Right)
		:	_Mycont(_Right._Mycont), _Myoffset(_Right._Myoffset)
		{	// construct by copying an iterator
		}

	ConstContainerRandomAccessIterator% operator=(
		ConstContainerRandomAccessIterator% _Right)
		{	// assign an iterator
		_Mycont = _Right._Mycont;
		_Myoffset = _Right._Myoffset;
		return (*this);
		}

	operator _Myiter_t^()
		{	// convert to interface
		return (this);
		}

	// constructors and special members
	ConstContainerRandomAccessIterator(TCont^ _Cont, int _Offset)
		:	_Mycont(_Cont), _Myoffset(_Offset)
		{	// construct from container pointer and offset
		}

	ConstContainerRandomAccessIterator(
		Generic::ContainerRandomAccessIterator<TValue>% _Right)
		:	_Mycont((TCont^)_Right.container()),
				_Myoffset(_Right.get_bias())
		{	// construct from mutable iterator
		}

	ConstContainerRandomAccessIterator% operator=(
		Generic::ContainerRandomAccessIterator<TValue>% _Right)
		{	// assign from mutable iterator
		_Mycont = (TCont^)_Right.container();
		_Myoffset = _Right.get_bias();
		return (*this);
		}

	// member functions
	virtual System::Object^ Clone()
		{	// return a copy
		return (gcnew ConstContainerRandomAccessIterator(_Mycont, _Myoffset));
		}

	virtual int get_bias()
		{	// get offset from wrapped iterator
		return (_Myoffset);
		}

	virtual System::Object^ get_node()
		{	// get node from wrapped iterator
		return (nullptr);
		}

	virtual bool valid()
		{	// test if iterator valid
		return (container() != nullptr
			&& _Mycont->valid_bias(_Myoffset));
		}

	virtual System::Object^ container()
		{	// return owning container
		return (_Mycont);
		}

	virtual void next()
		{	// increment
		if (!_Mycont->valid_bias(_Myoffset + 1))
			throw gcnew System::InvalidOperationException();
		++_Myoffset;
		}

	virtual bool equal_to(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() == _Right->get_bias()
			&& get_node() == _Right->get_node());
		}

	bool equal_to(ConstContainerRandomAccessIterator% _Right)
		{	// test if *this == _Right
		if (container() == nullptr
			|| container() != _Right._Mycont)
			throw gcnew System::ArgumentException();
		return (_Myoffset == _Right._Myoffset);
		}

	virtual const_reference get_cref()
		{	// return const reference to designated element
		return (_Mycont->at_bias(_Myoffset));
		}

	virtual reference get_ref()
		{	// return reference to designated element
#pragma warning(push)
#pragma warning(disable: 4715)
		throw gcnew System::InvalidOperationException();
#pragma warning(pop)
		}

	virtual void prev()
		{	// decrement
		if (!_Mycont->valid_bias(_Myoffset - 1))
			throw gcnew System::InvalidOperationException();
		--_Myoffset;
		}

	virtual difference_type move(difference_type _Offset)
		{	// incremented by integer
		difference_type _Newoffset = _Myoffset + _Offset;	// can overflow

		if (!_Mycont->valid_bias(_Newoffset))
			throw gcnew System::InvalidOperationException();
		_Myoffset = _Newoffset;
		return (_Myoffset);
		}

	virtual difference_type distance(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() - _Right->get_bias());
		}

	difference_type distance(
		_Mytype_t% _Right)
		{	// return difference of two iterators
		if (container() == nullptr
			|| container() != _Right.container())
			throw gcnew System::ArgumentException();
		return (get_bias() - _Right.get_bias());
		}

	virtual bool less_than(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this < _Right
		if (container() == nullptr
			|| container() != _Right->container())
			throw gcnew System::ArgumentException();
		return (get_bias() < _Right->get_bias());
		}

	bool less_than(_Mytype_t% _Right)
		{	// test if *this < _Right
		if (container() == nullptr
			|| container() != _Right.container())
			throw gcnew System::ArgumentException();
		return (get_bias() < _Right.get_bias());
		}

	// operators
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator->(
		ConstContainerRandomAccessIterator% _Left)
		{	// return pointer to class object
		return (_Left.get_cref());
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static const_reference operator*(
		ConstContainerRandomAccessIterator% _Left)
		{	// return const reference to designated element
		return (_Left.get_cref());
		}

	ConstContainerRandomAccessIterator operator++()
		{	// return incremented
		next();
		return (*this);
		}

	ConstContainerRandomAccessIterator operator++(int)
		{	// return incremented
		ConstContainerRandomAccessIterator _Iter = *this;

		++*this;
		return (_Iter);
		}

	bool operator==(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator==(_Mytype_t% _Right)
		{	// test if *this == _Right
		return (equal_to(_Right));
		}

	bool operator!=(Generic::IInputIterator<TValue>^ _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	bool operator!=(_Mytype_t% _Right)
		{	// test if *this != _Right
		return (!(*this == _Right));
		}

	ConstContainerRandomAccessIterator operator--()
		{	// return decremented
		prev();
		return (*this);
		}

	ConstContainerRandomAccessIterator operator--(int)
		{	// return decremented
		ConstContainerRandomAccessIterator _Iter = *this;

		--*this;
		return (_Iter);
		}

	ConstContainerRandomAccessIterator operator+(difference_type _Right)
		{	// return incremented by integer
		ConstContainerRandomAccessIterator _Iter = *this;

		_Iter.move(_Right);
		return (_Iter);
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static ConstContainerRandomAccessIterator operator+(
		difference_type _Left, ConstContainerRandomAccessIterator _Right)
		{	// return incremented by integer
		ConstContainerRandomAccessIterator _Iter = _Right;

		_Iter.move(_Left);
		return (_Iter);
		}

	ConstContainerRandomAccessIterator operator-(difference_type _Right)
		{	// return decremented by integer
		ConstContainerRandomAccessIterator _Iter = *this;

		_Iter.move(-_Right);	// can overflow
		return (_Iter);
		}

	difference_type operator-(
		Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// return difference of two iterators
		return (distance(_Right));
		}

	bool operator<(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this < _Right
		return (less_than(_Right));
		}

	bool operator<(_Mytype_t% _Right)
		{	// test if *this < _Right
		return (less_than(_Right));
		}

	bool operator>=(Generic::IRandomAccessIterator<TValue>^ _Right)
		{	// test if *this >= _Right
		return (!(*this < _Right));
		}

	bool operator>=(_Mytype_t% _Right)
		{	// test if *this >= _Right
		return (!(*this < _Right));
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static bool operator>(Generic::IRandomAccessIterator<TValue>^ _Left,
		_Mytype_t% _Right)
		{	// test if _Left > _Right
		return (_Right < _Left);
		}

	bool operator>(_Mytype_t%_Right)
		{	// test if *this > _Right
		return (_Right < *this);
		}

	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1000")]
	static bool operator<=(Generic::IRandomAccessIterator<TValue>^ _Left,
		_Mytype_t% _Right)
		{	// test if _Left <= _Right
		return (!(_Right < _Left));
		}

	bool operator<=(_Mytype_t%_Right)
		{	// test if *this <= _Right
		return (!(_Right < *this));
		}

	property const_reference default[difference_type]
		{	// get subscripted element
		const_reference get(difference_type _Pos)
			{	// get _Pos element
			ConstContainerRandomAccessIterator _Where = *this + _Pos;

			return (*_Where);
			}
		};

_STLCLR_FIELD_ACCESS:
	// data members
	TCont^ _Mycont;	// owning container
	difference_type _Myoffset;	// offset into container
	};
	}	// namespace Generic

		// GENERICS FOR deque //
//
// GENERIC INTERFACE CLASS IDeque
//
generic<typename TValue>
	public interface class IDeque
	:	public Generic::IRandomAccessContainer<TValue>,
			System::ICloneable,
			System::Collections::ICollection,
			System::Collections::IEnumerable
	{	// interface for a deque
	typedef IDeque<TValue> _Mycont_it;
	typedef System::Collections::IEnumerable _Myenum_it;

	typedef Generic::ContainerRandomAccessIterator<TValue>
		iterator;
	typedef Generic::ReverseRandomAccessIterator<TValue>
		reverse_iterator;

	typedef int size_type;
	typedef TValue value_type;
	typedef value_type% reference;

	// accessors
	reference at(size_type _Pos);
	property value_type front_item;
	property value_type back_item;
	reference front();
	reference back();

	property value_type default[size_type]
		{	// subscript
		value_type get(size_type _Pos);
		void set(size_type _Pos, value_type _Val);
		}

	// iterator generators
	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();

	// size controllers
//	void reserve(size_type _Capacity);
//	size_type capacity();
	void resize(size_type _Newsize);
	void resize(size_type _Newsize, value_type _Val);
	size_type size();
	bool empty();

	// mutators
	void push_front(value_type _Val);
	void pop_front();
	void push_back(value_type _Val);
	void pop_back();

	void assign(size_type _Count, value_type _Val);
	void assign(Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void assign(_Myenum_it^ _Right);

	iterator insert(iterator _Where, value_type _Val);
	void insert(iterator _Where,
		size_type _Count, value_type _Val);
	void insert(iterator _Where,
		Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void insert(iterator _Where_iter,
		_Myenum_it^ _Right);

	iterator erase(iterator _Where);
	iterator erase(iterator _First_iter, iterator _Last_iter);
	void clear();

	void swap(_Mycont_it^);

//_STLCLR_FIELD_ACCESS:
//
	size_type begin_bias();
	size_type end_bias();
//	reference at_bias(size_type);
//	bool valid_bias(size_type);
	unsigned long get_generation();
	};

//
// GENERIC REF CLASS DequeEnumeratorBase
//
generic<typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1038")]
	public ref class DequeEnumeratorBase
	:	public System::Collections::IEnumerator
	{	// enumerator for a deque
public:
	typedef DequeEnumeratorBase<TValue> _Mytype_t;
	typedef IDeque<TValue> _Mycont_it;
	typedef int difference_type;
	typedef TValue value_type;

	DequeEnumeratorBase(_Mycont_it^ _Cont, difference_type _First)
		:	_Mycont(_Cont), _Myfirst(_First), _Is_reset(true)
		{	// construct from container and bias
		_Mygen = _Mycont->get_generation();
		}

	virtual bool MoveNext()
		{	// move to next element and test if done
		_Validate();

		if (_Is_reset)
			{	// starting, point to front and clear flag
			_Is_reset = false;
			_Mybias = _Myfirst;
			}
		else if ((unsigned int)_Mybias - _Mycont->begin_bias()
			< (unsigned int)_Mycont->size())
			++_Mybias;
		return ((unsigned int)_Mybias - _Mycont->begin_bias()
			< (unsigned int)_Mycont->size());
		}

	property System::Object^ Current
		{	// get or set next element
		virtual System::Object^ get()
			{	// get next element
			return (_Getval());
			}

		virtual void set(System::Object^)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset()
		{	// restart enumerator
		_Validate();

		_Is_reset = true;
		}

_STLCLR_FIELD_ACCESS:
	value_type _Getval()
		{	// get next element
		_Validate();

		if (_Is_reset)
			throw gcnew System::InvalidOperationException();
		return (_Mycont->at_bias(_Mybias));
		}

	void _Validate()
		{	// test if container has been modified
		if (_Mycont->get_generation() != _Mygen)
			throw gcnew System::InvalidOperationException();
		}

	bool _Is_reset;	// true when starting/reset
	difference_type _Myfirst;	// initial bias
	difference_type _Mybias;	// current bias
	_Mycont_it^ _Mycont;	// owning container
	unsigned long _Mygen;	// container generation
	};

//
// GENERIC REF CLASS DequeEnumerator
//
generic<typename TValue>
	public ref class DequeEnumerator
	:	public DequeEnumeratorBase<TValue>,
			System::Collections::Generic::IEnumerator<TValue>
	{	// enumerator for a deque
public:
	typedef DequeEnumerator<TValue> _Mytype_t;
	typedef DequeEnumeratorBase<TValue> _Mybase_t;
	typedef int difference_type;
	typedef TValue value_type;

	DequeEnumerator(_Mycont_it^ _Cont, difference_type _First)
		:	_Mybase_t(_Cont, _First)
		{	// construct from container and bias
		}

	~DequeEnumerator()
		{	// destroy the object
		}

	virtual bool MoveNext() override
		{	// move to next element and test if done
		return (_Mybase_t::MoveNext());
		}

	property value_type Current
		{	// get or set next element
		virtual value_type get() new
			{	// get next element
			return (_Mybase_t::_Getval());
			}

		virtual void set(value_type)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset() override
		{	// restart enumerator
		_Mybase_t::Reset();
		}
	};

		// GENERICS FOR functional //
//
// GENERIC DELEGATE UnaryDelegate
//
generic<typename TArg,
	typename TResult>
	public delegate TResult UnaryDelegate(TArg);

//
// GENERIC DELEGATE BinaryDelegate
//
generic<typename TArg1,
	typename TArg2,
	typename TResult>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1005")]
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Naming", "CA1708")]
	public delegate TResult BinaryDelegate(TArg1, TArg2);

		// GENERICS FOR list //
//
// GENERIC INTERFACE CLASS IList
//
generic<typename TValue>
	public interface class IList
	:	public Generic::IBidirectionalContainer<TValue>,
			System::ICloneable,
			System::Collections::ICollection,
			System::Collections::IEnumerable
	{	// interface for a list
	typedef IList<TValue> _Mycont_it;
	typedef System::Collections::IEnumerable _Myenum_it;

	typedef Generic::ContainerBidirectionalIterator<TValue>
		iterator;
	typedef Generic::ReverseBidirectionalIterator<TValue>
		reverse_iterator;

	typedef int size_type;
	typedef TValue value_type;
	typedef value_type% reference;

	typedef BinaryDelegate<value_type, value_type, bool> _Valcomp_dt;
	typedef UnaryDelegate<value_type, bool> _Valtest_dt;

	// accessors
	property value_type front_item;
	property value_type back_item;
	reference front();
	reference back();

	// iterator generators
	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();

	// size controllers
//	void reserve(size_type _Capacity);
//	size_type capacity();
	void resize(size_type _Newsize);
	void resize(size_type _Newsize, value_type _Val);
	size_type size();
	bool empty();

	// mutators
	void push_front(value_type _Val);
	void pop_front();
	void push_back(value_type _Val);
	void pop_back();

	void assign(size_type _Count, value_type _Val);
	void assign(Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void assign(_Myenum_it^ _Right);

	iterator insert(iterator _Where, value_type _Val);
	void insert(iterator _Where,
		size_type _Count, value_type _Val);
	void insert(iterator _Where,
		Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void insert(iterator _Where_iter,
		_Myenum_it^ _Right);

	iterator erase(iterator _Where);
	iterator erase(iterator _First_iter, iterator _Last_iter);
	void clear();

	void swap(_Mycont_it^ _Right);

	// special functions
	void splice(iterator _Where, _Mycont_it^ _Right);
	void splice(iterator _Where, _Mycont_it^ _Right,
		iterator _First);
	void splice(iterator _Where, _Mycont_it^ _Right,
		iterator _First, iterator _Last);

	void remove(value_type _Val);
	void remove_if(_Valtest_dt^ _Pred);
	void unique(_Valcomp_dt^ _Pred);
	void merge(_Mycont_it^ _Right, _Valcomp_dt^ _Pred);
	void sort(_Valcomp_dt^ _Pred);
	void reverse();

//_STLCLR_FIELD_ACCESS:
//	unsigned long get_generation();
	};

//
// GENERIC REF CLASS ListEnumeratorBase
//
generic<typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1038")]
	public ref class ListEnumeratorBase
	:	public System::Collections::IEnumerator
	{	// enumerator for a list
public:
	typedef ListEnumeratorBase<TValue> _Mytype_t;
	typedef Generic::INode<TValue> _Mynode_it;

	typedef TValue value_type;

	ListEnumeratorBase(_Mynode_it^ _First)
		:	_Myfirst(_First), _Is_reset(true)
		{	// construct from initial list node
		_Mygen = _Myfirst->container()->get_generation();
		}

	virtual bool MoveNext()
		{	// move to next element and test if done
		_Validate();

		if (_Is_reset)
			{	// starting, point to front and clear flag
			_Is_reset = false;
			_Mynode = _Myfirst;
			}
		else if (!_Mynode->is_head())
			_Mynode = _Mynode->next_node();
		return (!_Mynode->is_head());
		}

	property System::Object^ Current
		{	// get or set next element
		virtual System::Object^ get()
			{	// get next element
			return (_Getval());
			}

		virtual void set(System::Object^)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset()
		{	// restart enumerator
		_Validate();

		_Is_reset = true;
		}

_STLCLR_FIELD_ACCESS:
	value_type _Getval()
		{	// get next element
		_Validate();

		if (_Is_reset || _Mynode->is_head())
			throw gcnew System::InvalidOperationException();
		return (_Mynode->_Value);
		}

	void _Validate()
		{	// test if container has been modified
		if (_Myfirst->container()->get_generation() != _Mygen)
			throw gcnew System::InvalidOperationException();
		}

	bool _Is_reset;	// true when starting/reset
	_Mynode_it^ _Myfirst;	// initial node
	_Mynode_it^ _Mynode;	// current node
	unsigned long _Mygen;	// container generation
	};

//
// GENERIC REF CLASS ListEnumerator
//
generic<typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1038")]
	public ref class ListEnumerator
	:	public ListEnumeratorBase<TValue>,
			System::Collections::Generic::IEnumerator<TValue>
	{	// enumerator for a list
public:
	typedef ListEnumerator<TValue> _Mytype_t;
	typedef ListEnumeratorBase<TValue> _Mybase_t;
	typedef Generic::INode<TValue> _Mynode_it;

	ListEnumerator(_Mynode_it^ _First)
		:	_Mybase_t(_First)
		{	// construct from initial list node
		}

	~ListEnumerator()
		{	// destroy the object
		}

	virtual bool MoveNext() override
		{	// move to next element and test if done
		return (_Mybase_t::MoveNext());
		}

	property value_type Current
		{	// get or set next element
		virtual value_type get() new
			{	// get next element
			return (_Mybase_t::_Getval());
			}

		virtual void set(value_type)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset() override
		{	// restart enumerator
		_Mybase_t::Reset();
		}
	};

		// GENERICS FOR queue //
//
// GENERIC INTERFACE CLASS IQueue
//
generic<typename TValue,
	typename TCont>
	public interface class IQueue
	:	public System::ICloneable
	{	// interface for a queue
	typedef IQueue<TValue, TCont> _Mycont_it;
	typedef cli::array<TValue> _Myarray_t;

	typedef int size_type;
	typedef TValue value_type;
	typedef value_type% reference;

	// accessors
	reference front();
	reference back();
	TCont get_container();

	// size controllers
	size_type size();
	bool empty();

	// mutators
	void push(value_type _Val);
	void pop();
	void assign(_Mycont_it^ _Right);
	};

//
// GENERIC INTERFACE CLASS IPriorityQueue
//
generic<typename TValue,
	typename TCont>
	public interface class IPriorityQueue
	:	public System::ICloneable
	{	// interface for a priority_queue
	typedef IPriorityQueue<TValue, TCont> _Mycont_it;
	typedef cli::array<TValue> _Myarray_t;

	typedef int size_type;
	typedef TValue value_type;
	typedef value_type% reference;

	typedef BinaryDelegate<TValue, TValue, bool> value_compare;

	// accessors
	property value_type top_item;
	reference top();
	TCont get_container();
	value_compare^ value_comp();

	// size controllers
	size_type size();
	bool empty();

	// mutators
	void push(value_type _Val);
	void pop();
	void assign(_Mycont_it^ _Right);
	};

		// GENERICS FOR stack //
//
// GENERIC INTERFACE CLASS IStack
//
generic<typename TValue,
	typename TCont>
	public interface class IStack
	:	public System::ICloneable
	{	// interface for a stack
	typedef IStack<TValue, TCont> _Mycont_it;
	typedef cli::array<TValue> _Myarray_t;

	typedef int size_type;
	typedef TValue value_type;
	typedef value_type% reference;

	// accessors
	property value_type top_item;
	reference top();
	TCont get_container();

	// size controllers
	size_type size();
	bool empty();

	// mutators
	void push(value_type _Val);
	void pop();
	void assign(_Mycont_it^ _Right);
	};

		// GENERICS FOR utility //
//
// GENERIC REF CLASS GenericPair
//
generic<typename TValue1,
	typename TValue2>
	public ref class GenericPair
	{	// store a pair of refs
public:
	typedef GenericPair<TValue1, TValue2> _Mytype_t;
	typedef System::Collections::Generic::KeyValuePair<TValue1, TValue2>
		_Mykvpair_t;
	typedef TValue1 first_type;
	typedef TValue2 second_type;

	// basics
	GenericPair()
		{	// default constructor
		}

	GenericPair(GenericPair% _Right)
		:	first(_Right.first), second(_Right.second)
		{	// construct by copying _Right
		}

	GenericPair(_Mykvpair_t% _Right)
		:	first(_Right.Key), second(_Right.Value)
		{	// construct by copying _Right
		}

	GenericPair% operator=(GenericPair% _Right)
		{	// assign
		first = _Right.first;
		second = _Right.second;
		return (*this);
		}

	// constructors
	GenericPair(TValue1 _Val1)
		:	first(_Val1)
		{	// construct from first value only
		}

	GenericPair(TValue1 _Val1, TValue2 _Val2)
		:	first(_Val1), second(_Val2)
		{	// construct from specified values
		}

	// mutators
	void swap(GenericPair% _Right)
		{	// exchange contents with _Right
		if ((Object^)this != %_Right)
			{	// worth swapping, do it
			TValue1 _Tfirst = first;
			TValue2 _Tsecond = second;

			first = _Right.first;
			_Right.first = _Tfirst;

			second = _Right.second;
			_Right.second = _Tsecond;
			}
		}

	// comparisons
	virtual bool Equals(System::Object^ _Right_arg) override
		{	// test for equality
		_Mytype_t^ _Right = (_Mytype_t^)_Right_arg;
		return (first->Equals(_Right->first)
			&& second->Equals(_Right->second));
		}

public:
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1051")]
	TValue1 first;	// first stored value
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1051")]
	TValue2 second;	// second stored value
	};

		// GENERICS FOR vector //
//
// GENERIC INTERFACE CLASS IVector
//
generic<typename TValue>
	public interface class IVector
	:	public Generic::IRandomAccessContainer<TValue>,
			System::ICloneable,
			System::Collections::ICollection,
			System::Collections::IEnumerable
	{	// interface for a vector
	typedef IVector<TValue> _Mycont_it;
	typedef System::Collections::IEnumerable _Myenum_it;

	typedef Generic::ContainerRandomAccessIterator<TValue>
		iterator;
	typedef Generic::ReverseRandomAccessIterator<TValue>
		reverse_iterator;

	typedef int size_type;
	typedef TValue value_type;
	typedef value_type% reference;

	// accessors
	reference at(size_type _Pos);
	property value_type front_item;
	property value_type back_item;
	reference front();
	reference back();

	property value_type default[size_type]
		{	// subscript
		value_type get(size_type _Pos);
		void set(size_type _Pos, value_type _Val);
		}

	// iterator generators
	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();

	// size controllers
	void reserve(size_type _Capacity);
	size_type capacity();
	void resize(size_type _Newsize);
	void resize(size_type _Newsize, value_type _Val);
	size_type size();
	bool empty();

	// mutators
//	void push_front(value_type _Val);
//	void pop_front();
	void push_back(value_type _Val);
	void pop_back();

	void assign(size_type _Count, value_type _Val);
	void assign(Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void assign(_Myenum_it^ _Right);

	iterator insert(iterator _Where, value_type _Val);
	void insert(iterator _Where,
		size_type _Count, value_type _Val);
	void insert(iterator _Where,
		Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void insert(iterator _Where_iter,
		_Myenum_it^ _Right);

	iterator erase(iterator _Where);
	iterator erase(iterator _First_iter, iterator _Last_iter);
	void clear();

	void swap(_Mycont_it^);

//_STLCLR_FIELD_ACCESS:
//
//	reference at_bias(size_type);
//	bool valid_bias(size_type);
	unsigned long get_generation();
	};

//
// GENERIC REF CLASS VectorEnumeratorBase
//
generic<typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1038")]
	public ref class VectorEnumeratorBase
	:	public System::Collections::IEnumerator
	{	// enumerator for a vector
public:
	typedef VectorEnumeratorBase<TValue> _Mytype_t;
	typedef IVector<TValue> _Mycont_it;

	typedef int difference_type;
	typedef TValue value_type;

	VectorEnumeratorBase(_Mycont_it^ _Cont, difference_type _First)
		:	_Mycont(_Cont), _Myfirst(_First), _Is_reset(true)
		{	// construct from container and bias
		_Mygen = _Mycont->get_generation();
		}

	virtual bool MoveNext()
		{	// move to next element and test if done
		_Validate();

		if (_Is_reset)
			{	// starting, point to front and clear flag
			_Is_reset = false;
			_Mybias = _Myfirst;
			}
		else if (_Mybias < _Mycont->size())
			++_Mybias;
		return (_Mybias < _Mycont->size());
		}

	property System::Object^ Current
		{	// get or set next element
		virtual System::Object^ get()
			{	// get next element
			return (_Getval());
			}

		virtual void set(System::Object^)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset()
		{	// restart enumerator
		_Validate();

		_Is_reset = true;
		}

_STLCLR_FIELD_ACCESS:
	value_type _Getval()
		{	// get next element
		_Validate();

		if (_Is_reset)
			throw gcnew System::InvalidOperationException();
		return (_Mycont->at_bias(_Mybias));
		}

	void _Validate()
		{	// test if container has been modified
		if (_Mycont->get_generation() != _Mygen)
			throw gcnew System::InvalidOperationException();
		}


	bool _Is_reset;	// true when starting/reset
	difference_type _Myfirst;	// initial bias
	difference_type _Mybias;	// current bias
	_Mycont_it^ _Mycont;	// owning container
	unsigned long _Mygen;	// container generation
	};

//
// GENERIC REF CLASS VectorEnumerator
//
generic<typename TValue>
	public ref class VectorEnumerator
	:	public VectorEnumeratorBase<TValue>,
			System::Collections::Generic::IEnumerator<TValue>
	{	// enumerator for a vector
public:
	typedef VectorEnumerator<TValue> _Mytype_t;
	typedef VectorEnumeratorBase<TValue> _Mybase_t;

	typedef int difference_type;
	typedef TValue value_type;

	VectorEnumerator(_Mycont_it^ _Cont, difference_type _First)
		:	_Mybase_t(_Cont, _First)
		{	// construct from container and bias
		}

	~VectorEnumerator()
		{	// destroy the object
		}

	virtual bool MoveNext() override
		{	// move to next element and test if done
		return (_Mybase_t::MoveNext());
		}

	property value_type Current
		{	// get or set next element
		virtual value_type get() new
			{	// get next element
			return (_Mybase_t::_Getval());
			}

		virtual void set(value_type)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset() override
		{	// restart enumerator
		_Mybase_t::Reset();
		}
	};

		// GENERICS FOR xhash //
//
// GENERIC INTERFACE CLASS IHash
//
generic<typename TKey,
	typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1005")]
	public interface class IHash
	:	public Generic::IBidirectionalContainer<TValue>,
			System::ICloneable,
			System::Collections::ICollection,
			System::Collections::IEnumerable
	{	// interface for a hash
	typedef IHash<TKey, TValue> _Mycont_it;
	typedef Generic::INode<TValue> _Mynode_it;
	typedef System::Collections::IEnumerable _Myenum_it;

	typedef Generic::ContainerBidirectionalIterator<TValue>
		iterator;
	typedef Generic::ReverseBidirectionalIterator<TValue>
		reverse_iterator;

	typedef int size_type;
	typedef TKey key_type;
	typedef TValue value_type;
	typedef value_type% reference;

	typedef GenericPair<iterator^, bool> pair_iter_bool;
	typedef GenericPair<iterator^, iterator^> pair_iter_iter;

	typedef BinaryDelegate<TKey, TKey, bool> key_compare;
	typedef BinaryDelegate<TValue, TValue, bool> value_compare;
	typedef UnaryDelegate<key_type, int> hasher;

	// accessors
//	property value_type front_item;
//	property value_type back_item;
//	reference front();
//	reference back();

	// converters
	key_compare^ key_comp();
	value_compare^ value_comp();

	// iterator generators
	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();

	// size controllers
//	void reserve(size_type);
//	size_type capacity();
//	void resize(size_type _Newsize);
//	void resize(size_type _Newsize, value_type _Val);
	size_type size();
	bool empty();

	// hash controllers
	hasher^ hash_delegate();
	int bucket_count();
	float load_factor();
	float max_load_factor();
	void max_load_factor(float _Newmax);
	void rehash(int _Buckets);

	// mutators
//	void push_front(value_type _Val);
//	void pop_front();
//	void push_back(value_type _Val);
//	void pop_back();

//	void assign(size_type _Count, value_type _Val);
//	void assign(Generic::IInputIterator<TValue>^ _First,
//		Generic::IInputIterator<TValue>^ _Last);
//	void assign(_Myenum_it^ _Right);

	pair_iter_bool insert(value_type _Val);
	iterator insert(iterator _Where, value_type _Val);
//	void insert(iterator _Where,
//		size_type _Count, value_type _Val);
//	void insert(iterator _Where,
//		Generic::IInputIterator<TValue>^ _First,
//		Generic::IInputIterator<TValue>^ _Last);
//	void insert(iterator _Where_iter,
//		_Myenum_it^ _Right);
	void insert(
		Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void insert(_Myenum_it^ _Right);

	iterator erase(iterator _Where);
	iterator erase(iterator _First_iter, iterator _Last_iter);
	size_type erase(key_type _Keyval);
	void clear();

	void swap(_Mycont_it^ _Right);

	// searches
	iterator find(key_type _Keyval);
	size_type count(key_type _Keyval);
	iterator lower_bound(key_type _Keyval);
	iterator upper_bound(key_type _Keyval);
	pair_iter_iter equal_range(key_type _Keyval);

//_STLCLR_FIELD_ACCESS:
//
//	unsigned long get_generation();
	};

//
// GENERIC REF CLASS HashEnumeratorBase
//
generic<typename TKey,
	typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1038")]
	public ref class HashEnumeratorBase
	:	public System::Collections::IEnumerator
	{	// enumerator for a hash
public:
	typedef HashEnumeratorBase<TKey, TValue> _Mytype_t;
	typedef Generic::INode<TValue> _Mynode_it;
	typedef TValue value_type;

	HashEnumeratorBase(_Mynode_it^ _First)
		:	_Myfirst(_First), _Is_reset(true)
		{	// construct from initial hash node
		_Mygen = _Myfirst->container()->get_generation();
		}

	virtual bool MoveNext()
		{	// move to next element and test if done
		_Validate();

		if (_Is_reset)
			{	// starting, point to front and clear flag
			_Is_reset = false;
			_Mynode = _Myfirst;
			}
		else if (!_Mynode->is_head())
			_Mynode = _Mynode->next_node();
		return (!_Mynode->is_head());
		}

	property System::Object^ Current
		{	// get or set next element
		virtual System::Object^ get()
			{	// get next element
			return (_Getval());
			}

		virtual void set(System::Object^)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset()
		{	// restart enumerator
		_Validate();

		_Is_reset = true;
		}

_STLCLR_FIELD_ACCESS:
	value_type _Getval()
		{	// get next element
		_Validate();

		if (_Is_reset || _Mynode->is_head())
			throw gcnew System::InvalidOperationException();
		return (_Mynode->_Value);
		}

	void _Validate()
		{	// test if container has been modified
		if (_Myfirst->container()->get_generation() != _Mygen)
			throw gcnew System::InvalidOperationException();
		}

	bool _Is_reset;	// true when starting/reset
	_Mynode_it^ _Myfirst;	// initial node
	_Mynode_it^ _Mynode;	// current node
	unsigned long _Mygen;	// container generation
	};

//
// GENERIC REF CLASS HashEnumerator
//
generic<typename TKey,
	typename TValue>
	public ref class HashEnumerator
	:	public HashEnumeratorBase<TKey, TValue>,
			System::Collections::Generic::IEnumerator<TValue>
	{	// typed enumerator for a hash
public:
	typedef HashEnumerator<TKey, TValue> _Mytype_t;
	typedef HashEnumeratorBase<TKey, TValue> _Mybase_t;
	typedef Generic::INode<TValue> _Mynode_it;

	HashEnumerator(_Mynode_it^ _First)
		:	_Mybase_t(_First)
		{	// construct from initial hash node
		}

	~HashEnumerator()
		{	// destroy the object
		}

	virtual bool MoveNext() override
		{	// move to next element and test if done
		return (_Mybase_t::MoveNext());
		}

	property value_type Current
		{	// get or set next element
		virtual value_type get() new
			{	// get next element
			return (_Mybase_t::_Getval());
			}

		virtual void set(value_type)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset() override
		{	// restart enumerator
		_Mybase_t::Reset();
		}
	};

		// GENERICS FOR xtree //
//
// GENERIC INTERFACE CLASS ITree
//
generic<typename TKey,
	typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1005")]
	public interface class ITree
	:	public Generic::IBidirectionalContainer<TValue>,
			System::ICloneable,
			System::Collections::ICollection,
			System::Collections::IEnumerable
	{	// interface for a tree
	typedef ITree<TKey, TValue> _Mycont_it;
	typedef System::Collections::Generic::IEnumerable<TValue> _Myenum_it;

	typedef Generic::ContainerBidirectionalIterator<TValue>
		iterator;
	typedef Generic::ReverseBidirectionalIterator<TValue>
		reverse_iterator;

	typedef int size_type;
	typedef TKey key_type;
	typedef TValue value_type;
	typedef value_type% reference;

	typedef GenericPair<iterator^, bool> pair_iter_bool;
	typedef GenericPair<iterator^, iterator^> pair_iter_iter;

	typedef BinaryDelegate<TKey, TKey, bool> key_compare;
	typedef BinaryDelegate<TValue, TValue, bool> value_compare;

	// accessors
//	property value_type front_item;
//	property value_type back_item;
//	reference front();
//	reference back();

	// converters
	key_compare^ key_comp();
	value_compare^ value_comp();

	// iterator generators
	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();

	// size controllers
//	void reserve(size_type);
//	size_type capacity();
//	void resize(size_type _Newsize);
//	void resize(size_type _Newsize, value_type _Val);
	size_type size();
	bool empty();

	// mutators
//	void push_front(value_type _Val);
//	void pop_front();
//	void push_back(value_type _Val);
//	void pop_back();

//	void assign(size_type _Count, value_type _Val);
//	void assign(Generic::IInputIterator<TValue>^ _First,
//		Generic::IInputIterator<TValue>^ _Last);
//	void assign(_Myenum_it^ _Right);

	pair_iter_bool insert(value_type _Val);
	iterator insert(iterator _Where, value_type _Val);
//	void insert(iterator _Where,
//		size_type _Count, value_type _Val);
//	void insert(iterator _Where,
//		Generic::IInputIterator<TValue>^ _First,
//		Generic::IInputIterator<TValue>^ _Last);
//	void insert(iterator _Where_iter,
//		_Myenum_it^ _Right);
	void insert(
		Generic::IInputIterator<TValue>^ _First,
		Generic::IInputIterator<TValue>^ _Last);
	void insert(_Myenum_it^ _Right);

	iterator erase(iterator _Where);
	iterator erase(iterator _First_iter, iterator _Last_iter);
	size_type erase(key_type _Keyval);
	void clear();

	void swap(_Mycont_it^ _Right);

	// searches
	iterator find(key_type _Keyval);
	size_type count(key_type _Keyval);
	iterator lower_bound(key_type _Keyval);
	iterator upper_bound(key_type _Keyval);
	pair_iter_iter equal_range(key_type _Keyval);

//_STLCLR_FIELD_ACCESS:
//
//	unsigned long get_generation();
	};

//
// GENERIC REF CLASS TreeEnumeratorBase
//
generic<typename TKey,
	typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1038")]
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1005")]
	public ref class TreeEnumeratorBase
	:	public System::Collections::IEnumerator
	{	// enumerator for a tree
public:
	typedef TreeEnumeratorBase<TKey, TValue> _Mytype_t;
	typedef Generic::INode<TValue> _Mynode_it;
	typedef TValue value_type;

	TreeEnumeratorBase(_Mynode_it^ _First)
		:	_Myfirst(_First), _Is_reset(true)
		{	// construct from initial tree node
		_Mygen = _Myfirst->container()->get_generation();
		}

	virtual bool MoveNext()
		{	// move to next element and test if done
		_Validate();

		if (_Is_reset)
			{	// starting, point to front and clear flag
			_Is_reset = false;
			_Mynode = _Myfirst;
			}
		else if (!_Mynode->is_head())
			_Mynode = _Mynode->next_node();
		return (!_Mynode->is_head());
		}

	property System::Object^ Current
		{	// get or set next element
		virtual System::Object^ get()
			{	// get next element
			return (_Getval());
			}

		virtual void set(System::Object^)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset()
		{	// restart enumerator
		_Validate();

		_Is_reset = true;
		}

_STLCLR_FIELD_ACCESS:
	value_type _Getval()
		{	// get next element
		_Validate();

		if (_Is_reset || _Mynode->is_head())
			throw gcnew System::InvalidOperationException();
		return (_Mynode->_Value);
		}

	void _Validate()
		{	// test if container has been modified
		if (_Myfirst->container()->get_generation() != _Mygen)
			throw gcnew System::InvalidOperationException();
		}

	bool _Is_reset;	// true when starting/reset
	_Mynode_it^ _Myfirst;	// initial node
	_Mynode_it^ _Mynode;	// current node
	unsigned long _Mygen;	// container generation
	};

//
// GENERIC REF CLASS TreeEnumerator
//
generic<typename TKey,
	typename TValue>
	[System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.Design", "CA1005")]
	public ref class TreeEnumerator
	:	public TreeEnumeratorBase<TKey, TValue>,
			System::Collections::Generic::IEnumerator<TValue>
	{	// typed enumerator for a tree
public:
	typedef TreeEnumerator<TKey, TValue> _Mytype_t;
	typedef TreeEnumeratorBase<TKey, TValue> _Mybase_t;
	typedef Generic::INode<TValue> _Mynode_it;

	TreeEnumerator(_Mynode_it^ _First)
		:	_Mybase_t(_First)
		{	// construct from initial tree node
		}

	~TreeEnumerator()
		{	// destroy the object
		}

	virtual bool MoveNext() override
		{	// move to next element and test if done
		return (_Mybase_t::MoveNext());
		}

	property value_type Current
		{	// get or set next element
		virtual value_type get() new
			{	// get next element
			return (_Mybase_t::_Getval());
			}

		virtual void set(value_type)
			{	// set next element
			throw gcnew System::InvalidOperationException();
			}
		};

	virtual void Reset() override
		{	// restart enumerator
		_Mybase_t::Reset();
		}
	};
		}	// namespace StlClr
	}	// namespace VisualC
}	// namespace Microsoft
#endif // _CLI_GENERICS_

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
