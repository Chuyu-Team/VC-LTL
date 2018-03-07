// type_traits standard header
#pragma once
#ifndef _TYPE_TRAITS_
#define _TYPE_TRAITS_
#ifndef RC_INVOKED
#include <xstddef>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)
 #pragma push_macro("new")
 #undef new
 #pragma warning(disable: 4180 4296)

_STD_BEGIN
template<class _Ty>
	struct _Is_memfunptr
	{	// base class for member function pointer predicates
	typedef false_type _Bool_type;
	};

#define _IS_MEMFUNPTR(CALL_OPT, CV_OPT, REF_OPT) \
template<class _Ret, \
	class _Arg0, \
	class... _Types> \
	struct _Is_memfunptr<_Ret (CALL_OPT _Arg0::*)(_Types...) CV_OPT REF_OPT> \
		: _Arg_types<CV_OPT _Arg0 *, _Types...> \
	{	/* base class for member function pointer predicates */ \
	typedef true_type _Bool_type; \
	typedef _Ret result_type; \
	typedef _Arg0 _Class_type; \
	};

_MEMBER_CALL_CV_REF(_IS_MEMFUNPTR)
#undef _IS_MEMFUNPTR

#define _IS_MEMFUNPTR_ELLIPSIS(CV_REF_OPT, X1) \
template<class _Ret, \
	class _Arg0, \
	class... _Types> \
	struct _Is_memfunptr<_Ret (_Arg0::*)(_Types..., ...) CV_REF_OPT> \
	{	/* no calling conventions for ellipsis */ \
	typedef true_type _Bool_type; \
	typedef _Ret result_type; \
	typedef _Arg0 _Class_type; \
	};

_CLASS_DEFINE_CV_REF(_IS_MEMFUNPTR_ELLIPSIS)
#undef _IS_MEMFUNPTR_ELLIPSIS

	// TEMPLATE CLASS is_void
template<class _Ty>
	struct is_void
		: false_type
	{	// determine whether _Ty is void
	};

#define _IS_VOID(CV_OPT) \
template<> \
	struct is_void<CV_OPT void> \
		: true_type \
	{	/* determine whether _Ty is void */ \
	};

_CLASS_DEFINE_CV(_IS_VOID)
#undef _IS_VOID

	// Type modifiers
	// TEMPLATE CLASS add_const
template<class _Ty>
	struct add_const
	{	// add top level const qualifier
	typedef const _Ty type;
	};

	// TEMPLATE CLASS add_volatile
template<class _Ty>
	struct add_volatile
	{	// add top level volatile qualifier
	typedef volatile _Ty type;
	};

	// TEMPLATE CLASS add_cv
template<class _Ty>
	struct add_cv
	{	// add top level const and volatile qualifiers
	typedef const volatile _Ty type;
	};

	// TEMPLATE CLASS _Add_reference
template<class _Ty,
	bool = _Is_function<_Ty>::_Weird || is_void<_Ty>::value>
	struct _Add_reference
	{	// add reference
	typedef _Ty _Lvalue;
	typedef _Ty _Rvalue;
	};

template<class _Ty>
	struct _Add_reference<_Ty, false>
	{	// add reference
	typedef _Ty& _Lvalue;
	typedef _Ty&& _Rvalue;
	};

	// TEMPLATE CLASS add_lvalue_reference
template<class _Ty>
	struct add_lvalue_reference
	{	// add lvalue reference
	typedef typename _Add_reference<_Ty>::_Lvalue type;
	};

	// TEMPLATE CLASS add_rvalue_reference
template<class _Ty>
	struct add_rvalue_reference
	{	// add rvalue reference
	typedef typename _Add_reference<_Ty>::_Rvalue type;
	};

	// TEMPLATE FUNCTION declval
template<class _Ty>
	typename add_rvalue_reference<_Ty>::type
		declval() _NOEXCEPT;

	// TEMPLATE CLASS remove_extent
template<class _Ty>
	struct remove_extent
	{	// remove array extent
	typedef _Ty type;
	};

template<class _Ty, size_t _Ix>
	struct remove_extent<_Ty[_Ix]>
	{	// remove array extent
	typedef _Ty type;
	};

template<class _Ty>
	struct remove_extent<_Ty[]>
	{	// remove array extent
	typedef _Ty type;
	};

	// TEMPLATE CLASS remove_all_extents
template<class _Ty>
	struct remove_all_extents
	{	// remove all array extents
	typedef _Ty type;
	};

template<class _Ty, size_t _Ix>
	struct remove_all_extents<_Ty[_Ix]>
	{	// remove all array extents
	typedef typename remove_all_extents<_Ty>::type type;
	};

template<class _Ty>
	struct remove_all_extents<_Ty[]>
	{	// remove all array extents
	typedef typename remove_all_extents<_Ty>::type type;
	};

	// TEMPLATE CLASS remove_pointer
template<class _Ty>
	struct remove_pointer
	{	// remove pointer
	typedef _Ty type;
	};

#define _REMOVE_POINTER(CV_OPT) \
template<class _Ty> \
	struct remove_pointer<_Ty *CV_OPT> \
	{	/* remove pointer */ \
	typedef _Ty type; \
	};

_CLASS_DEFINE_CV(_REMOVE_POINTER)
#undef _REMOVE_POINTER

	// TEMPLATE CLASS add_pointer
template<class _Ty,
	bool = _Is_function<_Ty>::_Weird>
	struct _Add_pointer
	{	// add pointer
	typedef _Ty type;
	};

template<class _Ty>
	struct _Add_pointer<_Ty, false>
	{	// add pointer
	typedef typename remove_reference<_Ty>::type *type;
	};

template<class _Ty>
	struct add_pointer
	{	// add pointer
	typedef typename _Add_pointer<_Ty>::type type;
	};

	// TYPE PREDICATES
	// TEMPLATE CLASS is_array
template<class _Ty>
	struct is_array
		: false_type
	{	// determine whether _Ty is an array
	};

template<class _Ty, size_t _Nx>
	struct is_array<_Ty[_Nx]>
		: true_type
	{	// determine whether _Ty is an array
	};

template<class _Ty>
	struct is_array<_Ty[]>
		: true_type
	{	// determine whether _Ty is an array
	};

	// TEMPLATE CLASS is_lvalue_reference
template<class _Ty>
	struct is_lvalue_reference
		: false_type
	{	// determine whether _Ty is an lvalue reference
	};

template<class _Ty>
	struct is_lvalue_reference<_Ty&>
		: true_type
	{	// determine whether _Ty is an lvalue reference
	};

	// TEMPLATE CLASS is_rvalue_reference
template<class _Ty>
	struct is_rvalue_reference
		: false_type
	{	// determine whether _Ty is an rvalue reference
	};

template<class _Ty>
	struct is_rvalue_reference<_Ty&&>
		: true_type
	{	// determine whether _Ty is an rvalue reference
	};

	// TEMPLATE CLASS is_reference
template<class _Ty>
	struct is_reference
		: _Cat_base<is_lvalue_reference<_Ty>::value
		|| is_rvalue_reference<_Ty>::value>
	{	// determine whether _Ty is a reference
	};


	// TEMPLATE CLASS is_member_object_pointer
template<class _Ty,
	bool _Pmf = _Is_memfunptr<_Ty>::_Bool_type::value>
	struct _Is_member_object_pointer
		: false_type
	{	// determine whether _Ty is a pointer to member object
	};

template<class _Ty1,
	class _Ty2>
	struct _Is_member_object_pointer<_Ty1 _Ty2::*, false>
		: true_type
	{	// determine whether _Ty is a pointer to member object
	typedef _Ty2 _Class_type;
	};

template<class _Ty>
	struct is_member_object_pointer
		: _Is_member_object_pointer<typename remove_cv<_Ty>::type>::type
	{	// determine whether _Ty is a pointer to member object
	};

	// TEMPLATE CLASS is_member_function_pointer
template<class _Ty>
	struct is_member_function_pointer
		: _Is_memfunptr<typename remove_cv<_Ty>::type>::_Bool_type
	{	// determine whether _Ty is a pointer to member function
	};

	// TEMPLATE CLASS is_pointer
template<class _Ty>
	struct _Is_pointer
		: false_type
	{	// determine whether _Ty is a pointer
	};

template<class _Ty>
	struct _Is_pointer<_Ty *>
		: _Cat_base<!is_member_object_pointer<_Ty *>::value
		&& !is_member_function_pointer<_Ty *>::value>
	{	// determine whether _Ty is a pointer
	};

template<class _Ty>
	struct is_pointer
		: _Is_pointer<typename remove_cv<_Ty>::type>
	{	// determine whether _Ty is a pointer
	};

	// TEMPLATE CLASS is_null_pointer

template<class _Ty>
	struct is_null_pointer
		: _Cat_base<is_same<typename remove_cv<_Ty>::type, nullptr_t>::value>
	{	// determine whether _Ty is nullptr_t
	};

	// TEMPLATE CLASS is_union
template<class _Ty>
	struct is_union
		: _Cat_base<__is_union(_Ty)>
	{	// determine whether _Ty is a union
	};

	// TEMPLATE CLASS is_class
template<class _Ty>
	struct is_class
		: _Cat_base<__is_class(_Ty)>
	{	// determine whether _Ty is a class
	};

	// TEMPLATE CLASS is_fundamental
template<class _Ty>
	struct is_fundamental
		: _Cat_base<is_arithmetic<_Ty>::value
		|| is_void<_Ty>::value
		|| is_null_pointer<_Ty>::value>
	{	// determine whether _Ty is a fundamental type
	};

	// TEMPLATE CLASS is_object
template<class _Ty>
	struct is_object
		: _Cat_base<!is_function<_Ty>::value
		&& !is_reference<_Ty>::value
		&& !is_void<_Ty>::value>
	{	// determine whether _Ty is an object type
	};

	// TEMPLATE CLASS is_convertible

template<class _From,
	class _To>
	struct is_convertible
		: _Cat_base<__is_convertible_to(_From, _To)>
	{	// determine whether _From is convertible to _To
	};

	// TEMPLATE CLASS is_enum
template<class _Ty>
	struct is_enum
		: _Cat_base<__is_enum(_Ty)>
	{	// determine whether _Ty is an enumerated type
	};

	// TEMPLATE CLASS is_compound
template<class _Ty>
	struct is_compound
		: _Cat_base<!is_fundamental<_Ty>::value>
	{	// determine whether _Ty is a compound type
	};

	// TEMPLATE CLASS is_member_pointer
template<class _Ty>
	struct is_member_pointer
		: _Cat_base<is_member_object_pointer<_Ty>::value
		|| is_member_function_pointer<_Ty>::value>
	{	// determine whether _Ty is a pointer to member
	};

	// TEMPLATE CLASS is_scalar
template<class _Ty>
	struct is_scalar
		: _Cat_base<is_arithmetic<_Ty>::value
		|| is_enum<_Ty>::value
		|| is_pointer<_Ty>::value
		|| is_member_pointer<_Ty>::value
		|| is_null_pointer<_Ty>::value>
	{	// determine whether _Ty is a scalar type
	};

	// TEMPLATE CLASS is_const
template<class _Ty>
	struct is_const
		: false_type
	{	// determine whether _Ty is const qualified
	};

template<class _Ty>
	struct is_const<const _Ty>
		: true_type
	{	// determine whether _Ty is const qualified
	};

	// TEMPLATE CLASS is_volatile
template<class _Ty>
	struct is_volatile
		: false_type
	{	// determine whether _Ty is volatile qualified
	};

template<class _Ty>
	struct is_volatile<volatile _Ty>
		: true_type
	{	// determine whether _Ty is volatile qualified
	};

	// TEMPLATE CLASS is_pod
template<class _Ty>
	struct is_pod
		: _Cat_base<__is_pod(_Ty)>
	{	// determine whether _Ty is a POD type
	};

	// TEMPLATE CLASS is_empty
template<class _Ty>
	struct is_empty
		: _Cat_base<__is_empty(_Ty)>
	{	// determine whether _Ty is an empty class
	};

	// TEMPLATE CLASS is_polymorphic
template<class _Ty>
	struct is_polymorphic
		: _Cat_base<__is_polymorphic(_Ty)>
	{	// determine whether _Ty is a polymorphic type
	};

	// TEMPLATE CLASS is_abstract
template<class _Ty>
	struct is_abstract
		: _Cat_base<__is_abstract(_Ty)>
	{	// determine whether _Ty is an abstract class
	};

	// TEMPLATE CLASS is_final
template<class _Ty>
	struct is_final
		: _Cat_base<__is_final(_Ty)>
	{	// determine whether _Ty is a final class
	};

	// TEMPLATE CLASS is_standard_layout
template<class _Ty>
	struct is_standard_layout
		: _Cat_base<__is_standard_layout(_Ty)>
	{	// determine whether _Ty is standard layout
	};

	// TEMPLATE CLASS is_literal_type
template<class _Ty>
	struct is_literal_type
		: _Cat_base<__is_literal_type(_Ty)>
	{	// determine whether _Ty is a literal type
	};

	// TEMPLATE CLASS is_trivial
template<class _Ty>
	struct is_trivial
		: _Cat_base<__is_trivial(_Ty)>
	{	// determine whether _Ty is trivial
	};

	// TEMPLATE CLASS is_trivially_copyable
template<class _Ty>
	struct is_trivially_copyable
		: _Cat_base<__is_trivially_copyable(_Ty)>
	{	// determine whether _Ty is trivially copyable
	};

	// TEMPLATE CLASS has_virtual_destructor
template<class _Ty>
	struct has_virtual_destructor
		: _Cat_base<__has_virtual_destructor(_Ty)>
	{	// determine whether _Ty has a virtual destructor
	};

		// CONSTRUCTIBLE/ASSIGNABLE TRAITS
	// TEMPLATE CLASS is_constructible

template<class _Ty,
	class... _Args>
	struct is_constructible
		: _Cat_base<__is_constructible(_Ty, _Args...)>
	{	// determine whether _Ty(_Args...) is constructible
	};

	// TEMPLATE CLASS is_copy_constructible
template<class _Ty>
	struct is_copy_constructible
		: is_constructible<
			_Ty,
			typename add_lvalue_reference<
				typename add_const<_Ty>::type
			>::type
		>::type
	{	// determine whether _Ty has a copy constructor
	};

	// TEMPLATE CLASS is_default_constructible
template<class _Ty>
	struct is_default_constructible
		: is_constructible<_Ty>::type
	{	// determine whether _Ty has a default constructor
	};

	// TEMPLATE CLASS is_move_constructible
template<class _Ty>
	struct is_move_constructible
		: is_constructible<
			_Ty,
			typename add_rvalue_reference<_Ty>::type
		>::type
	{	// determine whether _Ty has a move constructor
	};

	// TEMPLATE CLASS is_assignable
template<class _To,
	class _From>
	struct is_assignable
		: integral_constant<bool, __is_assignable(_To, _From)>
	{	// determine whether _From can be assigned to _To
	};

	// TEMPLATE CLASS is_copy_assignable
template<class _Ty>
	struct is_copy_assignable
		: is_assignable<
			typename add_lvalue_reference<_Ty>::type,
			typename add_lvalue_reference<
				typename add_const<_Ty>::type
			>::type
		>::type
	{	// determine whether _Ty has a copy assignment operator
	};

	// TEMPLATE CLASS is_move_assignable
template<class _Ty>
	struct is_move_assignable
		: is_assignable<
			typename add_lvalue_reference<_Ty>::type,
			typename add_rvalue_reference<_Ty>::type
		>::type
	{	// determine whether _Ty has a move assignment operator
	};

	// TEMPLATE CLASS is_destructible
template<class _Ty>
	struct is_destructible
		: _Cat_base<__is_destructible(_Ty)>
	{	// determine whether _Ty has a destructor
	};

		// TRIVIAL TRAITS
	// TEMPLATE CLASS is_trivially_constructible

template<class _Ty,
	class... _Args>
	struct is_trivially_constructible
		: _Cat_base<__is_trivially_constructible(_Ty, _Args...)>
	{	// determine whether _Ty(_Args...) is trivially constructible
	};

	// TEMPLATE CLASS is_trivially_copy_constructible
template<class _Ty>
	struct is_trivially_copy_constructible
		: is_trivially_constructible<
			_Ty,
			typename add_lvalue_reference<
				typename add_const<_Ty>::type
			>::type
		>::type
	{	// determine whether _Ty has a trivial copy constructor
	};

	// TEMPLATE CLASS is_trivially_default_constructible
template<class _Ty>
	struct is_trivially_default_constructible
		: is_trivially_constructible<_Ty>::type
	{	// determine whether _Ty has a trivial default constructor
	};

	// TEMPLATE CLASS is_trivially_move_constructible
template<class _Ty>
	struct is_trivially_move_constructible
		: is_trivially_constructible<
			_Ty,
			typename add_rvalue_reference<_Ty>::type
		>::type
	{	// determine whether _Ty has a trivial move constructor
	};

	// TEMPLATE CLASS is_trivially_assignable
template<class _To,
	class _From>
	struct is_trivially_assignable
		: _Cat_base<__is_trivially_assignable(_To, _From)>
	{	// determine whether _From can be assigned to _To, trivially
	};

	// TEMPLATE CLASS is_trivially_copy_assignable
template<class _Ty>
	struct is_trivially_copy_assignable
		: is_trivially_assignable<
			typename add_lvalue_reference<_Ty>::type,
			typename add_lvalue_reference<
				typename add_const<_Ty>::type
			>::type
		>::type
	{	// determine whether _Ty has a trivial copy assignment operator
	};

	// TEMPLATE CLASS is_trivially_move_assignable
template<class _Ty>
	struct is_trivially_move_assignable
		: is_trivially_assignable<
			typename add_lvalue_reference<_Ty>::type,
			typename add_rvalue_reference<_Ty>::type
		>::type
	{	// determine whether _Ty has a trivial move assignment operator
	};

	// TEMPLATE CLASS is_trivially_destructible
template<class _Ty>
	struct is_trivially_destructible
		: _Cat_base<__has_trivial_destructor(_Ty)>
	{	// determine whether _Ty has a trivial destructor
	};

		// NOTHROW TRAITS
	// TEMPLATE CLASS is_nothrow_constructible

template<class _Ty,
	class... _Args>
	struct is_nothrow_constructible
		: _Cat_base<__is_nothrow_constructible(_Ty, _Args...)>
	{	// determine whether _Ty(_Args...) is nothrow constructible
	};

	// TEMPLATE CLASS is_nothrow_copy_constructible
template<class _Ty>
	struct is_nothrow_copy_constructible
		: is_nothrow_constructible<
			_Ty,
			typename add_lvalue_reference<
				typename add_const<_Ty>::type
			>::type
		>::type
	{	// determine whether _Ty has a nothrow copy constructor
	};

	// TEMPLATE CLASS is_nothrow_default_constructible
template<class _Ty>
	struct is_nothrow_default_constructible
		: is_nothrow_constructible<_Ty>::type
	{	// determine whether _Ty has a nothrow default constructor
	};

	// TEMPLATE CLASS is_nothrow_move_constructible
template<class _Ty>
	struct is_nothrow_move_constructible
		: is_nothrow_constructible<
			_Ty,
			typename add_rvalue_reference<_Ty>::type
		>::type
	{	// determine whether _Ty has a nothrow move constructor
	};

	// TEMPLATE CLASS is_nothrow_assignable
template<class _To,
	class _From>
	struct is_nothrow_assignable
		: _Cat_base<__is_nothrow_assignable(_To, _From)>
	{	// determine whether _From can be assigned to _To, nothrow
	};

	// TEMPLATE CLASS is_nothrow_copy_assignable
template<class _Ty>
	struct is_nothrow_copy_assignable
		: is_nothrow_assignable<
			typename add_lvalue_reference<_Ty>::type,
			typename add_lvalue_reference<
				typename add_const<_Ty>::type
			>::type
		>::type
	{	// determine whether _Ty has a nothrow copy assignment operator
	};

	// TEMPLATE CLASS is_nothrow_move_assignable
template<class _Ty>
	struct is_nothrow_move_assignable
		: is_nothrow_assignable<
			typename add_lvalue_reference<_Ty>::type,
			typename add_rvalue_reference<_Ty>::type
		>::type
	{	// determine whether _Ty has a nothrow move assignment operator
	};

	// TEMPLATE CLASS is_nothrow_destructible
template<class _Ty>
	struct is_nothrow_destructible
		: _Cat_base<__is_nothrow_destructible(_Ty)>
	{	// determine whether _Ty has a nothrow destructor
	};

	// TEMPLATE CLASS is_signed
template<class _Ty,
	bool = is_integral<_Ty>::value>
	struct _Sign_base
	{	// determine whether integral _Ty is a signed or unsigned type
	typedef typename remove_cv<_Ty>::type _Uty;
	typedef _Cat_base<_Uty(-1) < _Uty(0)> _Signed;
	typedef _Cat_base<_Uty(0) < _Uty(-1)> _Unsigned;
	};

template<class _Ty>
	struct _Sign_base<_Ty, false>
	{	// floating-point _Ty is signed
		// non-arithmetic _Ty is neither signed nor unsigned
	typedef is_floating_point<_Ty> _Signed;
	typedef false_type _Unsigned;
	};

template<class _Ty>
	struct is_signed
		: _Sign_base<_Ty>::_Signed
	{	// determine whether _Ty is a signed type
	};

	// TEMPLATE CLASS is_unsigned
template<class _Ty>
	struct is_unsigned
		: _Sign_base<_Ty>::_Unsigned
	{	// determine whether _Ty is an unsigned type
	};

	// TEMPLATE CLASS _Change_sign
template<class _Ty>
	struct _Change_sign
	{	// signed/unsigned partners to _Ty
	static_assert(
		((is_integral<_Ty>::value || is_enum<_Ty>::value)
			&& !is_same<_Ty, bool>::value),
		"make_signed<T>/make_unsigned<T> require that T shall be a (possibly "
		"cv-qualified) integral type or enumeration but not a bool type.");

	typedef
		typename _If<is_same<_Ty, signed char>::value
			|| is_same<_Ty, unsigned char     >::value, signed char,
		typename _If<is_same<_Ty, short       >::value
			|| is_same<_Ty, unsigned short    >::value, short,
		typename _If<is_same<_Ty, int         >::value
			|| is_same<_Ty, unsigned int      >::value, int,
		typename _If<is_same<_Ty, long        >::value
			|| is_same<_Ty, unsigned long     >::value, long,
		typename _If<is_same<_Ty, long long   >::value
			|| is_same<_Ty, unsigned long long>::value, long long,
		typename _If<sizeof (_Ty) == sizeof (signed char), signed char,
		typename _If<sizeof (_Ty) == sizeof (short      ), short,
		typename _If<sizeof (_Ty) == sizeof (int        ), int,
		typename _If<sizeof (_Ty) == sizeof (long       ), long,
			long long
		>::type>::type>::type>::type>::type>::type>::type>::type>::type
			_Signed;

	typedef
		typename _If<is_same<_Signed, signed char>::value, unsigned char,
		typename _If<is_same<_Signed, short      >::value, unsigned short,
		typename _If<is_same<_Signed, int        >::value, unsigned int,
		typename _If<is_same<_Signed, long       >::value, unsigned long,
			unsigned long long
		>::type>::type>::type>::type
			_Unsigned;
	};

template<class _Ty>
	struct _Change_sign<const _Ty>
	{	// signed/unsigned partners to _Ty
	typedef const typename _Change_sign<_Ty>::_Signed _Signed;
	typedef const typename _Change_sign<_Ty>::_Unsigned _Unsigned;
	};

template<class _Ty>
	struct _Change_sign<volatile _Ty>
	{	// signed/unsigned partners to _Ty
	typedef volatile typename _Change_sign<_Ty>::_Signed _Signed;
	typedef volatile typename _Change_sign<_Ty>::_Unsigned _Unsigned;
	};

template<class _Ty>
	struct _Change_sign<const volatile _Ty>
	{	// signed/unsigned partners to _Ty
	typedef const volatile typename _Change_sign<_Ty>::_Signed _Signed;
	typedef const volatile typename _Change_sign<_Ty>::_Unsigned _Unsigned;
	};

	// TEMPLATE CLASS make_signed
template<class _Ty>
	struct make_signed
	{	// signed partner to _Ty
	typedef typename _Change_sign<_Ty>::_Signed type;
	};

	// TEMPLATE CLASS make_unsigned
template<class _Ty>
	struct make_unsigned
	{	// unsigned partner to _Ty
	typedef typename _Change_sign<_Ty>::_Unsigned type;
	};

	// TEMPLATE CLASS alignment_of

template<class _Ty>
	struct alignment_of
		: integral_constant<size_t, alignof(_Ty)>
	{	// determine alignment of _Ty
	};

	// TEMPLATE CLASS aligned_storage
#define _FITS(_Ty)	_Align <= alignment_of<_Ty>::value
#define _NEXT_ALIGN(_Ty)	\
	typedef typename _Aligned<_Len, _Align, _Ty, _FITS(_Ty)>::type type

template<class _Ty,
	size_t _Len>
	union _Align_type
	{	// union with size _Len bytes and alignment of _Ty
	_Ty _Val;
	char _Pad[_Len];
	};

template<size_t _Len,
	size_t _Align,
	class _Ty,
	bool _Ok>
	struct _Aligned;

template<size_t _Len,
	size_t _Align,
	class _Ty>
	struct _Aligned<_Len, _Align, _Ty, true>
	{	// define type with size _Len and alignment _Ty
	typedef _Align_type<_Ty, _Len> type;
	};

template<size_t _Len,
	size_t _Align>
	struct _Aligned<_Len, _Align, double, false>
	{	// define type with size _Len and alignment _Ty
	typedef _Align_type<max_align_t, _Len> type;
	};

template<size_t _Len,
	size_t _Align>
	struct _Aligned<_Len, _Align, int, false>
	{	// define type with size _Len and alignment _Ty
	_NEXT_ALIGN(double);
	};

template<size_t _Len,
	size_t _Align>
	struct _Aligned<_Len, _Align, short, false>
	{	// define type with size _Len and alignment _Ty
	_NEXT_ALIGN(int);
	};

template<size_t _Len,
	size_t _Align>
	struct _Aligned<_Len, _Align, char, false>
	{	// define type with size _Len and alignment _Ty
	_NEXT_ALIGN(short);
	};

template<size_t _Len,
	size_t _Align = alignment_of<max_align_t>::value>
	struct aligned_storage
	{	// define type with size _Len and alignment _Align
	typedef typename _Aligned<_Len, _Align, char, _FITS(char)>::type type;
	};

#undef _FITS
#undef _NEXT_ALIGN

	// TEMPLATE CLASS aligned_union
template<size_t... _Vals>
	struct _Maximum;

template<>
	struct _Maximum<>
	{	// maximum of nothing is 0
	static constexpr size_t value = 0;
	};

template<size_t _Val>
	struct _Maximum<_Val>
	{	// maximum of _Val is _Val
	static constexpr size_t value = _Val;
	};

template<size_t _First,
	size_t _Second,
	size_t... _Rest>
	struct _Maximum<_First, _Second, _Rest...>
		: _Maximum<(_First < _Second ? _Second : _First), _Rest...>
	{	// find maximum value in _First, _Second, _Rest...
	};

template<size_t _Len,
	class... _Types>
	struct aligned_union
	{	// define type with size at least _Len, for storing anything in _Types
	static constexpr size_t _Max_len = _Maximum<
		_Len, sizeof(_Types)...>::value;	// NOT sizeof...(_Types)
	static constexpr size_t alignment_value = _Maximum<
		alignment_of<_Types>::value...>::value;
	typedef typename aligned_storage<_Max_len, alignment_value>::type type;
	};

	// TEMPLATE CLASS underlying_type
template<class _Ty>
	struct underlying_type
	{	// determine type underlying type for enum
	typedef __underlying_type(_Ty) type;
	};

	// TEMPLATE CLASS rank
template<class _Ty>
	struct rank
		: integral_constant<size_t, 0>
	{	// determine number of dimensions of array _Ty
	};

template<class _Ty, size_t _Ix>
	struct rank<_Ty[_Ix]>
		: integral_constant<size_t, rank<_Ty>::value + 1>
	{	// determine number of dimensions of array _Ty
	};

template<class _Ty>
	struct rank<_Ty[]>
		: integral_constant<size_t, rank<_Ty>::value + 1>
	{	// determine number of dimensions of array _Ty
	};

	// TEMPLATE CLASS extent
template<class _Ty, unsigned int _Nx>
	struct _Extent
		: integral_constant<size_t, 0>
	{	// determine extent of dimension _Nx of array _Ty
	};

template<class _Ty, size_t _Ix>
	struct _Extent<_Ty[_Ix], 0>
		: integral_constant<size_t, _Ix>
	{	// determine extent of dimension _Nx of array _Ty
	};

template<class _Ty, unsigned int _Nx, size_t _Ix>
	struct _Extent<_Ty[_Ix], _Nx>
		: _Extent<_Ty, _Nx - 1>
	{	// determine extent of dimension _Nx of array _Ty
	};

template<class _Ty, unsigned int _Nx>
	struct _Extent<_Ty[], _Nx>
		: _Extent<_Ty, _Nx - 1>
	{	// determine extent of dimension _Nx of array _Ty
	};

template<class _Ty, unsigned int _Nx = 0>
	struct extent
		: _Extent<_Ty, _Nx>
	{	// determine extent of dimension _Nx of array _Ty
	};

	// TEMPLATE CLASS is_base_of
template<class _Base,
	class _Der>
	struct is_base_of
		: _Cat_base<__is_base_of(_Base, _Der)>
	{	// determine whether _Base is a base of or the same as _Der
	};

	// TEMPLATE CLASS decay
template<class _Ty>
	struct decay
	{	// determines decayed version of _Ty
	typedef typename remove_reference<_Ty>::type _Ty1;

	typedef typename _If<is_array<_Ty1>::value,
		typename remove_extent<_Ty1>::type *,
		typename _If<is_function<_Ty1>::value,
			typename add_pointer<_Ty1>::type,
			typename remove_cv<_Ty1>::type>::type>::type type;
	};

	// TEMPLATE CLASS conjunction
template<class...>
	struct _Conjunction;

template<bool,
	class _Lhs,
	class... _Traits>
	struct _Choose_conjunction
	{	// Select _Lhs if it is false
	typedef _Lhs type;
	};

template<class _Lhs,
	class... _Traits>
	struct _Choose_conjunction<true, _Lhs, _Traits...>
	{	// Select the conjunction of _Traits if _Lhs is true
	typedef typename _Conjunction<_Traits...>::type type;
	};

template<>
	struct _Conjunction<>
	{	// Implement conjunction for 0 arguments
	typedef true_type type;
	};

template<class _Trait>
	struct _Conjunction<_Trait>
	{	// Implement conjunction for 1 argument
	typedef _Trait type;
	};

template<class _Lhs,
	class... _Traits>
	struct _Conjunction<_Lhs, _Traits...>
	{	// Implement conjunction for N arguments
	typedef typename _Choose_conjunction<_Lhs::value, _Lhs, _Traits...>::type type;
	};

template<class... _Traits>
	struct conjunction
		: _Conjunction<_Traits...>::type
	{	// If _Traits is empty, true_type
		// Otherwise, if any of _Traits are false, the first false trait
		// Otherwise, the last trait in _Traits
	};

	// TEMPLATE CLASS disjunction
template<class...>
	struct _Disjunction;

template<bool,
	class _Lhs,
	class... _Traits>
	struct _Choose_disjunction
	{	// Select the disjunction of _Traits if _Lhs is true
	typedef _Lhs type;
	};

template<class _Lhs,
	class... _Traits>
	struct _Choose_disjunction<false, _Lhs, _Traits...>
	{	// Select _Lhs if it is false
	typedef typename _Disjunction<_Traits...>::type type;
	};

template<>
	struct _Disjunction<>
	{	// Implement disjunction for 0 arguments
	typedef false_type type;
	};

template<class _Trait>
	struct _Disjunction<_Trait>
	{	// Implement disjunction for 1 argument
	typedef _Trait type;
	};

template<class _Lhs,
	class... _Traits>
	struct _Disjunction<_Lhs, _Traits...>
	{	// Implement disjunction for N arguments
	typedef typename _Choose_disjunction<_Lhs::value, _Lhs, _Traits...>::type type;
	};

template<class... _Traits>
	struct disjunction
		: _Disjunction<_Traits...>::type
	{	// If _Traits is empty, false_type
		// Otherwise, if any of _Traits are true, the first true trait
		// Otherwise, the last trait in _Traits
	};

	// TEMPLATE CLASS negation
template<class _Trait>
	struct negation
		: bool_constant<!_Trait::value>
	{	// The negated result of _Trait
	};

#if _HAS_TR1_NAMESPACE
namespace tr1 {	// TR1 additions
using _STD add_const;
using _STD add_cv;
using _STD add_pointer;
using _STD add_volatile;
using _STD aligned_storage;
using _STD alignment_of;
using _STD conditional;
using _STD decay;
using _STD enable_if;
using _STD extent;
using _STD false_type;
using _STD has_virtual_destructor;
using _STD integral_constant;
using _STD is_abstract;
using _STD is_arithmetic;
using _STD is_array;
using _STD is_base_of;
using _STD is_class;
using _STD is_compound;
using _STD is_const;
using _STD is_convertible;
using _STD is_empty;
using _STD is_enum;
using _STD is_floating_point;
using _STD is_function;
using _STD is_fundamental;
using _STD is_integral;
using _STD is_member_function_pointer;
using _STD is_member_object_pointer;
using _STD is_member_pointer;
using _STD is_object;
using _STD is_pod;
using _STD is_pointer;
using _STD is_polymorphic;
using _STD is_reference;
using _STD is_same;
using _STD is_scalar;
using _STD is_signed;
using _STD is_union;
using _STD is_unsigned;
using _STD is_void;
using _STD is_volatile;
using _STD make_signed;
using _STD make_unsigned;
using _STD rank;
using _STD remove_all_extents;
using _STD remove_const;
using _STD remove_cv;
using _STD remove_extent;
using _STD remove_pointer;
using _STD remove_reference;
using _STD remove_volatile;
using _STD true_type;
	}	// namespace tr1
#endif /* _HAS_TR1_NAMESPACE */

		// TEMPLATE STRUCT common_type
template<class... _Ty>
	struct common_type;

template<class _Ty>
	struct common_type<_Ty>
	{	// type is typename decay<_Ty>::type for one argument
	typedef typename decay<_Ty>::type type;
	};

template<class _Ty0,
	class _Ty1>
	struct common_type<_Ty0, _Ty1>
	{	// type is common type of _Ty0 and _Ty1 for two arguments
	typedef typename decay<
		decltype(_Always_false<_Ty0>::value
			? _STD declval<_Ty0>()
			: _STD declval<_Ty1>())
	>::type type;
	};

template<class _Ty0,
	class _Ty1,
	class... _Ty>
	struct common_type<_Ty0, _Ty1, _Ty...>
	{	// type is defined recursively for more than two arguments
	typedef typename common_type<
		typename common_type<_Ty0, _Ty1>::type, _Ty...
	>::type type;
	};

	// TEMPLATE STRUCT integer_sequence
template<class _Ty,
	_Ty... _Vals>
	struct integer_sequence
	{	// sequence of integer parameters
	static_assert(is_integral<_Ty>::value,
		"integer_sequence<T, I...> requires T to be an integral type.");

	typedef integer_sequence<_Ty, _Vals...> type;
	typedef _Ty value_type;

	static constexpr size_t size() _NOEXCEPT
		{	// get length of parameter list
		return (sizeof...(_Vals));
		}
	};

	// ALIAS TEMPLATE make_integer_sequence
 #ifdef __EDG__ /* TRANSITION, VSO#205481, VSO#205484 */
template<bool _Negative,
	bool _Zero,
	class _Int_con,
	class _Int_seq>
	struct _Make_seq
	{	// explodes gracefully below 0
	static_assert(!_Negative,
		"make_integer_sequence<T, N> requires N to be non-negative.");
	};

template<class _Ty,
	_Ty... _Vals>
	struct _Make_seq<false, true,
		integral_constant<_Ty, 0>,
		integer_sequence<_Ty, _Vals...> >
		: integer_sequence<_Ty, _Vals...>
	{	// ends recursion at 0
	};

template<class _Ty,
	_Ty _Ix,
	_Ty... _Vals>
	struct _Make_seq<false, false,
		integral_constant<_Ty, _Ix>,
		integer_sequence<_Ty, _Vals...> >
		: _Make_seq<false, _Ix == 1,
			integral_constant<_Ty, _Ix - 1>,
			integer_sequence<_Ty, _Ix - 1, _Vals...> >
	{	// counts down to 0
	};

template<class _Ty,
	_Ty _Size>
	using make_integer_sequence = typename _Make_seq<_Size < 0, _Size == 0,
		integral_constant<_Ty, _Size>, integer_sequence<_Ty> >::type;
 #else /* __EDG__ */
template<class _Ty,
	_Ty _Size>
	using make_integer_sequence = __make_integer_seq<integer_sequence, _Ty, _Size>;
 #endif /* __EDG__ */

template<size_t... _Vals>
	using index_sequence = integer_sequence<size_t, _Vals...>;

template<size_t _Size>
	using make_index_sequence = make_integer_sequence<size_t, _Size>;

template<class... _Types>
	using index_sequence_for = make_index_sequence<sizeof...(_Types)>;

#if _HAS_IDENTITY_STRUCT
	// TEMPLATE CLASS identity
template<class _Ty>
	struct identity
	{	// map _Ty to type unchanged
	typedef _Ty type;

	const _Ty& operator()(const _Ty& _Left) const
		{	// apply identity operator to operand
		return (_Left);
		}
	};
#endif /* _HAS_IDENTITY_STRUCT */

	// TEMPLATE FUNCTION forward
template<class _Ty> inline
	constexpr _Ty&& forward(
		typename remove_reference<_Ty>::type& _Arg) _NOEXCEPT
	{	// forward an lvalue as either an lvalue or an rvalue
	return (static_cast<_Ty&&>(_Arg));
	}

template<class _Ty> inline
	constexpr _Ty&& forward(
		typename remove_reference<_Ty>::type&& _Arg) _NOEXCEPT
	{	// forward an rvalue as an rvalue
	static_assert(!is_lvalue_reference<_Ty>::value, "bad forward call");
	return (static_cast<_Ty&&>(_Arg));
	}

		// TEMPLATE FUNCTION move
template<class _Ty> inline
	constexpr typename remove_reference<_Ty>::type&&
		move(_Ty&& _Arg) _NOEXCEPT
	{	// forward _Arg as movable
	return (static_cast<typename remove_reference<_Ty>::type&&>(_Arg));
	}

		// TEMPLATE FUNCTION move_if_noexcept
template<class _Ty> inline
	constexpr typename _If<!is_nothrow_move_constructible<_Ty>::value
		&& is_copy_constructible<_Ty>::value,
			const _Ty&, _Ty&&>::type
	move_if_noexcept(_Ty& _Arg) _NOEXCEPT
	{	// forward _Arg as movable, sometimes
	return (_STD move(_Arg));
	}

	// TEMPLATE STRUCT _Param_tester
template<class...>
	struct _Param_tester
	{	// test if parameters are valid
	typedef void type;
	};

	// ALIAS TEMPLATE void_t
template<class... _Types>	// TRANSITION, VSO#138046
	using void_t = typename _Param_tester<_Types...>::type;

	// TEMPLATE FUNCTION invoke
struct _Invoker_pmf_object
	{	// INVOKE a pointer to member function on an object
	template<class _Decayed,
		class _Ty1,
		class... _Types2>
		static auto _Call(_Decayed _Pmf, _Ty1&& _Arg1, _Types2&&... _Args2)
		-> decltype((_STD forward<_Ty1>(_Arg1).*_Pmf)(
			_STD forward<_Types2>(_Args2)...))
		{	// INVOKE a pointer to member function on an object
		return ((_STD forward<_Ty1>(_Arg1).*_Pmf)(
			_STD forward<_Types2>(_Args2)...));
		}
	};

struct _Invoker_pmf_pointer
	{	// INVOKE a pointer to member function on a [smart] pointer
	template<class _Decayed,
		class _Ty1,
		class... _Types2>
		static auto _Call(_Decayed _Pmf, _Ty1&& _Arg1, _Types2&&... _Args2)
		-> decltype(((*_STD forward<_Ty1>(_Arg1)).*_Pmf)(
			_STD forward<_Types2>(_Args2)...))
		{	// INVOKE a pointer to member function on a [smart] pointer
		return (((*_STD forward<_Ty1>(_Arg1)).*_Pmf)(
			_STD forward<_Types2>(_Args2)...));
		}
	};

struct _Invoker_pmd_object
	{	// INVOKE a pointer to member data on an object
	template<class _Decayed,
		class _Ty1>
		static auto _Call(_Decayed _Pmd, _Ty1&& _Arg1)
		-> decltype(_STD forward<_Ty1>(_Arg1).*_Pmd)
		{	// INVOKE a pointer to member data on an object
		return (_STD forward<_Ty1>(_Arg1).*_Pmd);
		}
	};

struct _Invoker_pmd_pointer
	{	// INVOKE a pointer to member data on a [smart] pointer
	template<class _Decayed,
		class _Ty1>
		static auto _Call(_Decayed _Pmd, _Ty1&& _Arg1)
		-> decltype((*_STD forward<_Ty1>(_Arg1)).*_Pmd)
		{	// INVOKE a pointer to member data on a [smart] pointer
		return ((*_STD forward<_Ty1>(_Arg1)).*_Pmd);
		}
	};

struct _Invoker_functor
	{	// INVOKE a function object
	template<class _Callable,
		class... _Types>
		static auto _Call(_Callable&& _Obj, _Types&&... _Args)
		-> decltype(_STD forward<_Callable>(_Obj)(
			_STD forward<_Types>(_Args)...))
		{	// INVOKE a function object
		return (_STD forward<_Callable>(_Obj)(
			_STD forward<_Types>(_Args)...));
		}
	};

template<class _Callable,
	class _Ty1,
	class _Decayed = typename decay<_Callable>::type,
	bool _Is_pmf = is_member_function_pointer<_Decayed>::value,
	bool _Is_pmd = is_member_object_pointer<_Decayed>::value>
	struct _Invoker1;

template<class _Callable,
	class _Ty1,
	class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, true, false>
		: _If<is_base_of<
			typename _Is_memfunptr<_Decayed>::_Class_type,
			typename decay<_Ty1>::type>::value,
		_Invoker_pmf_object,
		_Invoker_pmf_pointer>::type
	{	// pointer to member function
	};

template<class _Callable,
	class _Ty1,
	class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, false, true>
		: _If<is_base_of<
			typename _Is_member_object_pointer<_Decayed>::_Class_type,
			typename decay<_Ty1>::type>::value,
		_Invoker_pmd_object,
		_Invoker_pmd_pointer>::type
	{	// pointer to member data
	};

template<class _Callable,
	class _Ty1,
	class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, false, false>
		: _Invoker_functor
	{	// function object
	};

template<class _Callable,
	class... _Types>
	struct _Invoker;

template<class _Callable>
	struct _Invoker<_Callable>
		: _Invoker_functor
	{	// zero arguments
	};

template<class _Callable,
	class _Ty1,
	class... _Types2>
	struct _Invoker<_Callable, _Ty1, _Types2...>
		: _Invoker1<_Callable, _Ty1>
	{	// one or more arguments
	};

template<class _Callable,
	class... _Types> inline
	auto invoke(_Callable&& _Obj, _Types&&... _Args)
	-> decltype(_Invoker<_Callable, _Types...>::_Call(
		_STD forward<_Callable>(_Obj), _STD forward<_Types>(_Args)...))
	{	// INVOKE a callable object
	return (_Invoker<_Callable, _Types...>::_Call(
		_STD forward<_Callable>(_Obj), _STD forward<_Types>(_Args)...));
	}

template<class _Rx,
	bool = is_void<_Rx>::value>
	struct _Forced
	{	// tag to give INVOKE an explicit return type
	};

struct _Unforced
	{	// tag to distinguish bind() from bind<R>()
	};

template<class _Cv_void,
	class... _Valtys> inline
	void _Invoke_ret(_Forced<_Cv_void, true>, _Valtys&&... _Vals)
	{	// INVOKE, "implicitly" converted to void
	_STD invoke(_STD forward<_Valtys>(_Vals)...);
	}

template<class _Rx,
	class... _Valtys> inline
	_Rx _Invoke_ret(_Forced<_Rx, false>, _Valtys&&... _Vals)
	{	// INVOKE, implicitly converted to _Rx
	return (_STD invoke(_STD forward<_Valtys>(_Vals)...));
	}

template<class... _Valtys> inline
	auto _Invoke_ret(_Forced<_Unforced, false>, _Valtys&&... _Vals)
	-> decltype(_STD invoke(_STD forward<_Valtys>(_Vals)...))
	{	// INVOKE, unchanged
	return (_STD invoke(_STD forward<_Valtys>(_Vals)...));
	}

	// TEMPLATE CLASS result_of
struct _Unique_tag_result_of
	{	// used by workaround below
	};

template<class _Void,
	class... _Types>
	struct _Result_of
	{	// selected when _Fty isn't callable with _Args
	};

template<class... _Types>
	struct _Result_of<
		void_t<
			_Unique_tag_result_of,	// TRANSITION, C1XX
			decltype(_STD invoke(_STD declval<_Types>()...))>,
		_Types...>
	{	// selected when _Fty is callable with _Args
	typedef decltype(_STD invoke(_STD declval<_Types>()...)) type;
	};

template<class _Fty>
	struct result_of
	{	// explain usage
	static_assert(_Always_false<_Fty>::value,
		"result_of<CallableType> is invalid; use "
		"result_of<CallableType(zero or more argument types)> instead.");
	};

#define _RESULT_OF(CALL_OPT, X1, X2) \
template<class _Fty, \
	class... _Args> \
	struct result_of<_Fty CALL_OPT (_Args...)> \
		: _Result_of<void, _Fty, _Args...> \
	{	/* template to determine result of call operation */ \
	};

_NON_MEMBER_CALL(_RESULT_OF, , )
#undef _RESULT_OF

	// TEMPLATE STRUCT _Weak_types
template<class _Ty,
	class = void>
	struct _Weak_result_type
	{	// default definition
	};

template<class _Ty>
	struct _Weak_result_type<_Ty, void_t<
		typename _Ty::result_type> >
	{	// defined if _Ty::result_type exists
	typedef typename _Ty::result_type result_type;
	};

template<class _Ty,
	class = void>
	struct _Weak_argument_type
		: _Weak_result_type<_Ty>
	{	// default definition
	};

template<class _Ty>
	struct _Weak_argument_type<_Ty, void_t<
		typename _Ty::argument_type> >
		: _Weak_result_type<_Ty>
	{	// defined if _Ty::argument_type exists
	typedef typename _Ty::argument_type argument_type;
	};

template<class _Ty,
	class = void>
	struct _Weak_binary_args
		: _Weak_argument_type<_Ty>
	{	// default definition
	};

template<class _Ty>
	struct _Weak_binary_args<_Ty, void_t<
		typename _Ty::first_argument_type,
		typename _Ty::second_argument_type> >
		: _Weak_argument_type<_Ty>
	{	// defined if both types exist
	typedef typename _Ty::first_argument_type first_argument_type;
	typedef typename _Ty::second_argument_type second_argument_type;
	};

template<class _Ty>
	struct _Weak_types
	{	// provide nested types (sometimes)
	typedef _Is_function<typename remove_pointer<_Ty>::type> _Is_f_or_pf;
	typedef _Is_memfunptr<typename remove_cv<_Ty>::type> _Is_pmf;
	typedef typename _If<_Is_f_or_pf::_Bool_type::value, _Is_f_or_pf,
		typename _If<_Is_pmf::_Bool_type::value, _Is_pmf,
		_Weak_binary_args<_Ty> >::type>::type type;
	};

	// TEMPLATE CLASS reference_wrapper
template<class _Ty>
	class reference_wrapper
		: public _Weak_types<_Ty>::type
	{	// stand-in for an assignable reference
public:
	static_assert(is_object<_Ty>::value || is_function<_Ty>::value,
		"reference_wrapper<T> requires T to be an object type "
		"or a function type.");

	typedef _Ty type;

	reference_wrapper(_Ty& _Val) _NOEXCEPT
		: _Ptr(_STD addressof(_Val))
		{	// construct
		}

	operator _Ty&() const _NOEXCEPT
		{	// return reference
		return (*_Ptr);
		}

	_Ty& get() const _NOEXCEPT
		{	// return reference
		return (*_Ptr);
		}

	template<class... _Types>
		auto operator()(_Types&&... _Args) const
		-> decltype(_STD invoke(get(), _STD forward<_Types>(_Args)...))
		{	// invoke object/function
		return (_STD invoke(get(), _STD forward<_Types>(_Args)...));
		}

	reference_wrapper(_Ty&&) = delete;

private:
	_Ty *_Ptr;
	};

	// TEMPLATE FUNCTIONS ref AND cref
template<class _Ty> inline
	reference_wrapper<_Ty>
		ref(_Ty& _Val) _NOEXCEPT
	{	// create reference_wrapper<_Ty> object
	return (reference_wrapper<_Ty>(_Val));
	}

template<class _Ty>
	void ref(const _Ty&&) = delete;

template<class _Ty> inline
	reference_wrapper<_Ty>
		ref(reference_wrapper<_Ty> _Val) _NOEXCEPT
	{	// create reference_wrapper<_Ty> object
	return (_STD ref(_Val.get()));
	}

template<class _Ty> inline
	reference_wrapper<const _Ty>
		cref(const _Ty& _Val) _NOEXCEPT
	{	// create reference_wrapper<const _Ty> object
	return (reference_wrapper<const _Ty>(_Val));
	}

template<class _Ty>
	void cref(const _Ty&&) = delete;

template<class _Ty> inline
	reference_wrapper<const _Ty>
		cref(reference_wrapper<_Ty> _Val) _NOEXCEPT
	{	// create reference_wrapper<const _Ty> object
	return (_STD cref(_Val.get()));
	}

	// TEMPLATE CLASS _Unrefwrap
template<class _Ty>
	struct _Unrefwrap_helper
	{	// leave unchanged if not a reference_wrapper
	typedef _Ty type;
	static constexpr bool _Is_refwrap = false;
	};

template<class _Ty>
	struct _Unrefwrap_helper<reference_wrapper<_Ty> >
	{	// make a reference from a reference_wrapper
	typedef _Ty& type;
	static constexpr bool _Is_refwrap = true;
	};

template<class _Ty>
	struct _Unrefwrap
	{	// decay, then unwrap a reference_wrapper
	typedef typename decay<_Ty>::type _Ty1;
	typedef typename _Unrefwrap_helper<_Ty1>::type type;
	static constexpr bool _Is_refwrap = _Unrefwrap_helper<_Ty1>::_Is_refwrap;
	};

#if _HAS_TR1_NAMESPACE
namespace tr1 {	// TR1 additions
using _STD cref;
using _STD ref;
using _STD reference_wrapper;
using _STD result_of;
	}	// namespace tr1
#endif /* _HAS_TR1_NAMESPACE */

		// CLASS TEMPLATE _Is_swappable
template<class _Ty>
	struct _Is_swappable;

		// CLASS TEMPLATE _Is_nothrow_swappable
template<class _Ty>
	struct _Is_nothrow_swappable;

		// FUNCTION TEMPLATE swap
#if _HAS_CXX17
template<class _Ty,
	class = typename enable_if<is_move_constructible<_Ty>::value
		&& is_move_assignable<_Ty>::value>::type> inline
#else /* _HAS_CXX17 */
template<class _Ty,
	class = void> inline
#endif /* _HAS_CXX17 */
	void swap(_Ty&, _Ty&)
		_NOEXCEPT_OP(is_nothrow_move_constructible<_Ty>::value
			&& is_nothrow_move_assignable<_Ty>::value);

template<class _Ty,
	size_t _Size,
	class = typename enable_if<_Is_swappable<_Ty>::value>::type> inline
	void swap(_Ty (&)[_Size], _Ty (&)[_Size])
		_NOEXCEPT_OP(_Is_nothrow_swappable<_Ty>::value);

		// CLASS TEMPLATE _Swappable_with_helper
template<class _Ty1,
	class _Ty2,
	class = void>
	struct _Swappable_with_helper
		: false_type
	{	// swap(declval<_Ty1>(), declval<_Ty2>()) is not valid
	};

struct _Swappable_with_helper_unique_type {}; // TRANSITION, C1XX
template<class _Ty1,
	class _Ty2>
	struct _Swappable_with_helper<_Ty1, _Ty2, void_t<
		_Swappable_with_helper_unique_type,
		decltype(swap(_STD declval<_Ty1>(), _STD declval<_Ty2>()))>>
		: true_type
	{	// swap(declval<_Ty1>(), declval<_Ty2>()) is valid
	};

		// CLASS TEMPLATE _Is_swappable_with
template<class _Ty1,
	class _Ty2>
	struct _Is_swappable_with
		: conjunction<
			_Swappable_with_helper<_Ty1, _Ty2>,
			_Swappable_with_helper<_Ty2, _Ty1>>::type
	{	// Determine if expressions with type and value category _Ty1 and _Ty2
		// can be swapped (and vice versa)
	};

		// CLASS TEMPLATE _Is_swappable
template<class _Ty>
	struct _Is_swappable
		: _Is_swappable_with<
			typename add_lvalue_reference<_Ty>::type,
			typename add_lvalue_reference<_Ty>::type>::type
	{	// Determine if _Ty lvalues satisfy is_swappable_with
	};

		// CLASS TEMPLATE _Swap_cannot_throw
template<class _Ty1,
	class _Ty2>
	struct _Swap_cannot_throw
	{	// Determine if expressions with type and value category _Ty1 and _Ty2
		// (presumed to satisfy is_swappable_with) can be swapped without
		// emitting exceptions
#if _HAS_EXCEPTIONS
	static constexpr bool value = // TRANSITION, VSO#207029
		noexcept(swap(_STD declval<_Ty1>(), _STD declval<_Ty2>()))
		&& noexcept(swap(_STD declval<_Ty2>(), _STD declval<_Ty1>()));
#else /* _HAS_EXCEPTIONS */
	static constexpr bool value = true;
#endif /* _HAS_EXCEPTIONS */
	using type = bool_constant<value>;
	};

		// CLASS TEMPLATE _Is_nothrow_swappable_with
template<class _Ty1,
	class _Ty2>
	struct _Is_nothrow_swappable_with
		: conjunction<
			_Is_swappable_with<_Ty1, _Ty2>,
			_Swap_cannot_throw<_Ty1, _Ty2>>::type
	{	// Determine if expressions with type and value category _Ty1 and _Ty2
		// satisfy is_swappable_with, and can be swapped without emitting exceptions
	};

		// CLASS TEMPLATE _Is_nothrow_swappable
template<class _Ty>
	struct _Is_nothrow_swappable
		: _Is_nothrow_swappable_with<
			typename add_lvalue_reference<_Ty>::type,
			typename add_lvalue_reference<_Ty>::type>::type
	{	// Determine if _Ty lvalues satisfy is_nothrow_swappable_with
	};

#if _HAS_CXX17
		// CLASS TEMPLATE is_swappable_with
template<class _Ty1,
	class _Ty2>
	struct is_swappable_with
		: _Is_swappable_with<_Ty1, _Ty2>::type
	{	// Determine if expressions with type and value category _Ty1 and _Ty2
		// can be swapped (and vice versa)
	};

		// CLASS TEMPLATE is_swappable
template<class _Ty>
	struct is_swappable
		: _Is_swappable<_Ty>::type
	{	// Determine if _Ty lvalues satisfy is_swappable_with
	};

		// CLASS TEMPLATE is_nothrow_swappable_with
template<class _Ty1,
	class _Ty2>
	struct is_nothrow_swappable_with
		: _Is_nothrow_swappable_with<_Ty1, _Ty2>::type
	{	// Determine if expressions with type and value category _Ty1 and _Ty2
		// satisfy is_swappable_with, and can be swapped without emitting exceptions
	};

		// CLASS TEMPLATE is_nothrow_swappable
template<class _Ty>
	struct is_nothrow_swappable
		: _Is_nothrow_swappable<_Ty>::type
	{	// Determine if _Ty lvalues satisfy is_nothrow_swappable_with
	};
#endif /* _HAS_CXX17 */

		// ALIAS TEMPLATES
template<class _Ty>
	using remove_const_t = typename remove_const<_Ty>::type;

template<class _Ty>
	using remove_volatile_t = typename remove_volatile<_Ty>::type;

template<class _Ty>
	using remove_cv_t = typename remove_cv<_Ty>::type;

template<class _Ty>
	using add_const_t = typename add_const<_Ty>::type;

template<class _Ty>
	using add_volatile_t = typename add_volatile<_Ty>::type;

template<class _Ty>
	using add_cv_t = typename add_cv<_Ty>::type;

template<class _Ty>
	using remove_reference_t = typename remove_reference<_Ty>::type;

template<class _Ty>
	using add_lvalue_reference_t = typename add_lvalue_reference<_Ty>::type;

template<class _Ty>
	using add_rvalue_reference_t = typename add_rvalue_reference<_Ty>::type;

template<class _Ty>
	using make_signed_t = typename make_signed<_Ty>::type;

template<class _Ty>
	using make_unsigned_t = typename make_unsigned<_Ty>::type;

template<class _Ty>
	using remove_extent_t = typename remove_extent<_Ty>::type;

template<class _Ty>
	using remove_all_extents_t = typename remove_all_extents<_Ty>::type;

template<class _Ty>
	using remove_pointer_t = typename remove_pointer<_Ty>::type;

template<class _Ty>
	using add_pointer_t = typename add_pointer<_Ty>::type;

template<size_t _Len,
	size_t _Align = alignment_of<max_align_t>::value>
	using aligned_storage_t = typename aligned_storage<_Len, _Align>::type;

template<size_t _Len,
	class... _Types>
	using aligned_union_t = typename aligned_union<_Len, _Types...>::type;

template<class _Ty>
	using decay_t = typename decay<_Ty>::type;

template<bool _Test,
	class _Ty = void>
	using enable_if_t = typename enable_if<_Test, _Ty>::type;

template<bool _Test,
	class _Ty1,
	class _Ty2>
	using conditional_t = typename conditional<_Test, _Ty1, _Ty2>::type;

template<class... _Ty>
	using common_type_t = typename common_type<_Ty...>::type;

template<class _Ty>
	using underlying_type_t = typename underlying_type<_Ty>::type;

template<class _Ty>
	using result_of_t = typename result_of<_Ty>::type;

	// VARIABLE TEMPLATES
 #if _HAS_VARIABLE_TEMPLATES
template<class _Ty>
	constexpr bool is_void_v = is_void<_Ty>::value;
template<class _Ty>
	constexpr bool is_null_pointer_v = is_null_pointer<_Ty>::value;
template<class _Ty>
	constexpr bool is_array_v = is_array<_Ty>::value;
template<class _Ty>
	constexpr bool is_pointer_v = is_pointer<_Ty>::value;
template<class _Ty>
	constexpr bool is_lvalue_reference_v = is_lvalue_reference<_Ty>::value;
template<class _Ty>
	constexpr bool is_rvalue_reference_v = is_rvalue_reference<_Ty>::value;
template<class _Ty>
	constexpr bool is_member_object_pointer_v = is_member_object_pointer<_Ty>::value;
template<class _Ty>
	constexpr bool is_member_function_pointer_v = is_member_function_pointer<_Ty>::value;
template<class _Ty>
	constexpr bool is_enum_v = is_enum<_Ty>::value;
template<class _Ty>
	constexpr bool is_union_v = is_union<_Ty>::value;
template<class _Ty>
	constexpr bool is_class_v = is_class<_Ty>::value;
template<class _Ty>
	constexpr bool is_reference_v = is_reference<_Ty>::value;
template<class _Ty>
	constexpr bool is_fundamental_v = is_fundamental<_Ty>::value;
template<class _Ty>
	constexpr bool is_object_v = is_object<_Ty>::value;
template<class _Ty>
	constexpr bool is_scalar_v = is_scalar<_Ty>::value;
template<class _Ty>
	constexpr bool is_compound_v = is_compound<_Ty>::value;
template<class _Ty>
	constexpr bool is_member_pointer_v = is_member_pointer<_Ty>::value;
template<class _Ty>
	constexpr bool is_const_v = is_const<_Ty>::value;
template<class _Ty>
	constexpr bool is_volatile_v = is_volatile<_Ty>::value;
template<class _Ty>
	constexpr bool is_trivial_v = is_trivial<_Ty>::value;
template<class _Ty>
	constexpr bool is_trivially_copyable_v = is_trivially_copyable<_Ty>::value;
template<class _Ty>
	constexpr bool is_standard_layout_v = is_standard_layout<_Ty>::value;
template<class _Ty>
	constexpr bool is_pod_v = is_pod<_Ty>::value;
template<class _Ty>
	constexpr bool is_literal_type_v = is_literal_type<_Ty>::value;
template<class _Ty>
	constexpr bool is_empty_v = is_empty<_Ty>::value;
template<class _Ty>
	constexpr bool is_polymorphic_v = is_polymorphic<_Ty>::value;
template<class _Ty>
	constexpr bool is_abstract_v = is_abstract<_Ty>::value;
template<class _Ty>
	constexpr bool is_final_v = is_final<_Ty>::value;
template<class _Ty>
	constexpr bool is_signed_v = is_signed<_Ty>::value;
template<class _Ty>
	constexpr bool is_unsigned_v = is_unsigned<_Ty>::value;
template<class _Ty,
	class... _Args>
	constexpr bool is_constructible_v = is_constructible<_Ty, _Args...>::value;
template<class _Ty>
	constexpr bool is_default_constructible_v = is_default_constructible<_Ty>::value;
template<class _Ty>
	constexpr bool is_copy_constructible_v = is_copy_constructible<_Ty>::value;
template<class _Ty>
	constexpr bool is_move_constructible_v = is_move_constructible<_Ty>::value;
template<class _Ty,
	class _Uty>
	constexpr bool is_assignable_v = is_assignable<_Ty, _Uty>::value;
template<class _Ty>
	constexpr bool is_copy_assignable_v = is_copy_assignable<_Ty>::value;
template<class _Ty>
	constexpr bool is_move_assignable_v = is_move_assignable<_Ty>::value;
#if _HAS_CXX17
template<class _Ty1,
	class _Ty2>
	constexpr bool is_swappable_with_v = is_swappable_with<_Ty1, _Ty2>::value;
template<class _Ty>
	constexpr bool is_swappable_v = is_swappable<_Ty>::value;
#endif /* _HAS_CXX17 */
template<class _Ty>
	constexpr bool is_destructible_v = is_destructible<_Ty>::value;
template<class _Ty,
	class... _Args>
	constexpr bool is_trivially_constructible_v = is_trivially_constructible<_Ty, _Args...>::value;
template<class _Ty>
	constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<_Ty>::value;
template<class _Ty>
	constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<_Ty>::value;
template<class _Ty>
	constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<_Ty>::value;
template<class _Ty,
	class _Uty>
	constexpr bool is_trivially_assignable_v = is_trivially_assignable<_Ty, _Uty>::value;
template<class _Ty>
	constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<_Ty>::value;
template<class _Ty>
	constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<_Ty>::value;
template<class _Ty>
	constexpr bool is_trivially_destructible_v = is_trivially_destructible<_Ty>::value;
template<class _Ty,
	class... _Args>
	constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<_Ty, _Args...>::value;
template<class _Ty>
	constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<_Ty>::value;
template<class _Ty>
	constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<_Ty>::value;
template<class _Ty>
	constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<_Ty>::value;
template<class _Ty,
	class _Uty>
	constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<_Ty, _Uty>::value;
template<class _Ty>
	constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<_Ty>::value;
template<class _Ty>
	constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<_Ty>::value;
#if _HAS_CXX17
template<class _Ty1,
	class _Ty2>
	constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<_Ty1, _Ty2>::value;
template<class _Ty>
	constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<_Ty>::value;
#endif /* _HAS_CXX17 */
template<class _Ty>
	constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<_Ty>::value;
template<class _Ty>
	constexpr bool has_virtual_destructor_v = has_virtual_destructor<_Ty>::value;
template<class _Ty>
	constexpr size_t alignment_of_v = alignment_of<_Ty>::value;
template<class _Ty>
	constexpr size_t rank_v = rank<_Ty>::value;
template<class _Ty,
	unsigned int _Ix = 0>
	constexpr size_t extent_v = extent<_Ty, _Ix>::value;
template<class _Base,
	class _Derived>
	constexpr bool is_base_of_v = is_base_of<_Base, _Derived>::value;
template<class _From,
	class _To>
	constexpr bool is_convertible_v = is_convertible<_From, _To>::value;
template<class... _Traits>
	constexpr bool conjunction_v = conjunction<_Traits...>::value;
template<class... _Traits>
	constexpr bool disjunction_v = disjunction<_Traits...>::value;
template<class _Trait>
	constexpr bool negation_v = negation<_Trait>::value;
 #endif /* _HAS_VARIABLE_TEMPLATES */

_STD_END

 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _TYPE_TRAITS_ */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
