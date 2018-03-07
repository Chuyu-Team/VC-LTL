/* xatomic0.h internal header */
#pragma once
#ifndef _XATOMIC0_H
#define _XATOMIC0_H
#ifndef RC_INVOKED
#include <yvals.h>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)
 #pragma push_macro("new")
 #undef new

_STD_BEGIN
		/* ENUM memory_order */
typedef enum memory_order {
	memory_order_relaxed,
	memory_order_consume,
	memory_order_acquire,
	memory_order_release,
	memory_order_acq_rel,
	memory_order_seq_cst
	} memory_order;

typedef _Uint32t _Uint4_t;
typedef _Uint4_t _Atomic_integral_t;

	/* SET SIZES AND FLAGS FOR COMPILER AND TARGET ARCHITECTURE */
	/* Note: the xxx_SIZE macros are used to generate function names,
		so they must expand to the digits representing
		the number of bytes in the type; they cannot be expressions
		that give the number of bytes. */

  #define _WCHAR_T_SIZE		2
  #define _SHORT_SIZE		2
  #define _INT_SIZE			4
  #define _LONG_SIZE		4
  #define _LONGLONG_SIZE	8

  #if defined(_WIN64)
   #define _ADDR_SIZE	8
  #else /* defined(_WIN64) */
   #define _ADDR_SIZE	4
  #endif /* defined(_WIN64) */

		/* TYPEDEF _Atomic_flag_t */

typedef long _Atomic_flag_t;

  #define _ATOMIC_MAXBYTES_LOCK_FREE	8

		/* ATOMIC REFERENCE COUNTING */
typedef _Atomic_integral_t _Atomic_counter_t;

inline _Atomic_integral_t
	_Get_atomic_count(const _Atomic_counter_t& _Counter)
	{	// get counter
	return (_Counter);
	}

inline void _Init_atomic_counter(_Atomic_counter_t& _Counter,
	_Atomic_integral_t _Value)
	{	// non-atomically initialize counter
	_Counter = _Value;
	}

 #ifndef _USE_INTERLOCKED_REFCOUNTING
  #if defined(_M_IX86) || defined(_M_X64) || defined(_M_CEE_PURE)
   #define _USE_INTERLOCKED_REFCOUNTING	1
  #else /* defined(_M_IX86) || defined(_M_X64) || defined(_M_CEE_PURE) */
   #define _USE_INTERLOCKED_REFCOUNTING	0
  #endif /* defined(_M_IX86) || defined(_M_X64) || defined(_M_CEE_PURE) */
 #endif /* _USE_INTERLOCKED_REFCOUNTING */

		/* SPIN LOCKS */
_EXTERN_C
_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock();
_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock();
_END_EXTERN_C
_STD_END
 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _XATOMIC0_H */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
