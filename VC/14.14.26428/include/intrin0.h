/***
*   intrin0.h - declarations of compiler intrinsics used by the C++ Standard Library.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*   This header file declares compiler intrinsics that are used by the
*   C++ Standard Library, especially <atomic>. Compiler throughput is
*   the only reason that intrin0.h is separate from intrin.h.
*
****/

#pragma once
#define __INTRIN0_H_
#ifndef RC_INVOKED
#ifndef __midl

#if defined(__cplusplus)
extern "C" {
#endif

/*
** __MACHINE              : everything
** __MACHINEX86           : x86 only
** __MACHINEX64           : x64 only
** __MACHINEX86_X64       : x86 and x64 only
** __MACHINEARM           : ARM only
** __MACHINEARM64         : ARM64 only
** __MACHINEARM_ARM64     : ARM and ARM64 only
** __MACHINEARM_ARM64_X64 : ARM and 64-bit Arch only
** __MACHINEARM64_X64     : ARM64 and x64 only
** __MACHINECHPEX86ARM64  : CHPE x86 on arm64 only
** __MACHINEWVMPURE       : /clr:pure only
** __MACHINEZ             : nothing
*/

#define __MACHINEX86           __MACHINE
#define __MACHINEX64           __MACHINE
#define __MACHINEX86_X64       __MACHINE
#define __MACHINEARM           __MACHINE
#define __MACHINEARM64         __MACHINE
#define __MACHINEARM_ARM64     __MACHINE
#define __MACHINEARM_ARM64_X64 __MACHINE
#define __MACHINEARM64_X64     __MACHINE
#define __MACHINECHPEX86ARM64  __MACHINE

/* Most intrinsics not available to pure managed code */
#if defined(_M_CEE_PURE)
#define __MACHINE(X)        __MACHINEZ(X)
#define __MACHINEWVMPURE(X) X;
#else
#define __MACHINE(X)        X;
#define __MACHINEWVMPURE(X) __MACHINEZ(X)
#endif

#define __MACHINEZ(X)       /* NOTHING */

#if !defined(_M_IX86) || defined(_CHPE_ONLY_)
#undef __MACHINEX86
#define __MACHINEX86        __MACHINEZ
#endif

#if !defined(_M_X64)
#undef __MACHINEX64
#define __MACHINEX64        __MACHINEZ
#endif

#if !(defined(_M_IX86) || defined(_M_X64)) || defined(_CHPE_ONLY_)
#undef __MACHINEX86_X64
#define __MACHINEX86_X64    __MACHINEZ
#endif

#if !defined(_M_ARM)
#undef  __MACHINEARM
#define __MACHINEARM        __MACHINEZ
#endif

/* For compatibility with <winnt.h>, some intrinsics are __cdecl except on x64 */
#if defined(_M_X64)
#define __MACHINECALL_CDECL_OR_DEFAULT
#else
#define __MACHINECALL_CDECL_OR_DEFAULT __cdecl
#endif

#if !defined(_M_ARM64) && !defined(_M_HYBRID_X86_ARM64)
#undef  __MACHINEARM64
#define __MACHINEARM64      __MACHINEZ
#endif

#if !(defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64))
#undef __MACHINEARM_ARM64
#define __MACHINEARM_ARM64  __MACHINEZ
#endif

#if !(defined(_M_ARM) || defined(_M_X64) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64))
#undef __MACHINEARM_ARM64_X64
#define __MACHINEARM_ARM64_X64     __MACHINEZ
#endif

#if !(defined(_M_X64) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64))
#undef __MACHINEARM64_X64
#define __MACHINEARM64_X64     __MACHINEZ
#endif

#if !defined(_M_HYBRID_X86_ARM64)
#undef __MACHINECHPEX86ARM64
#define __MACHINECHPEX86ARM64 __MACHINEZ
#endif

/*******************************************************************
* Note: New intrinsics should be added here IF AND ONLY IF they're *
* being used by the C++ Standard Library.                          *
* OTHERWISE, new intrinsics should be added to intrin.h.           *
*******************************************************************/

__MACHINEARM_ARM64(void __dmb(unsigned int _Type))

__MACHINE(unsigned char _BitScanReverse(unsigned long * _Index, unsigned long _Mask))
__MACHINEX64(unsigned char _BitScanReverse64(unsigned long * _Index, unsigned __int64 _Mask))
__MACHINEARM64(unsigned char _BitScanReverse64(unsigned long * _Index, unsigned __int64 _Mask))
__MACHINE(unsigned char _bittest(long const *, long))
__MACHINE(long _InterlockedAnd(long volatile * _Value, long _Mask))
__MACHINE(short _InterlockedAnd16(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedAnd16_acq(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedAnd16_nf(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedAnd16_rel(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64_X64(__int64 _InterlockedAnd64(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedAnd64_acq(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedAnd64_nf(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedAnd64_rel(__int64 volatile * _Value, __int64 _Mask))
__MACHINE(char _InterlockedAnd8(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedAnd8_acq(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedAnd8_nf(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedAnd8_rel(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(long _InterlockedAnd_acq(long volatile * _Value, long _Mask))
__MACHINEARM_ARM64(long _InterlockedAnd_nf(long volatile * _Value, long _Mask))
__MACHINEARM_ARM64(long _InterlockedAnd_rel(long volatile * _Value, long _Mask))
__MACHINE(long __MACHINECALL_CDECL_OR_DEFAULT _InterlockedCompareExchange(long volatile * _Destination, long _Exchange, long _Comparand))
__MACHINEWVMPURE(long _InterlockedCompareExchange(long volatile * _Destination, long _Exchange, long _Comparand))
__MACHINE(short _InterlockedCompareExchange16(short volatile * _Destination, short _Exchange, short _Comparand))
__MACHINEARM_ARM64(short _InterlockedCompareExchange16_acq(short volatile * _Destination, short _Exchange, short _Comparand))
__MACHINEARM_ARM64(short _InterlockedCompareExchange16_nf(short volatile * _Destination, short _Exchange, short _Comparand))
__MACHINEARM_ARM64(short _InterlockedCompareExchange16_rel(short volatile * _Destination, short _Exchange, short _Comparand))
__MACHINE(__int64 _InterlockedCompareExchange64(__int64 volatile * _Destination, __int64 _Exchange, __int64 _Comparand))
__MACHINEARM_ARM64(__int64 _InterlockedCompareExchange64_acq(__int64 volatile * _Destination, __int64 _Exchange, __int64 _Comparand))
__MACHINEARM_ARM64(__int64 _InterlockedCompareExchange64_nf(__int64 volatile * _Destination, __int64 _Exchange, __int64 _Comparand))
__MACHINEARM_ARM64(__int64 _InterlockedCompareExchange64_rel(__int64 volatile * _Destination, __int64 _Exchange, __int64 _Comparand))
__MACHINE(char _InterlockedCompareExchange8(char volatile * _Destination, char _Exchange, char _Comparand))
__MACHINEARM_ARM64(char _InterlockedCompareExchange8_acq(char volatile * _Destination, char _Exchange, char _Comparand))
__MACHINEARM_ARM64(char _InterlockedCompareExchange8_nf(char volatile * _Destination, char _Exchange, char _Comparand))
__MACHINEARM_ARM64(char _InterlockedCompareExchange8_rel(char volatile * _Destination, char _Exchange, char _Comparand))
__MACHINEARM_ARM64(long _InterlockedCompareExchange_acq(long volatile * _Destination, long _Exchange, long _Comparand))
__MACHINEARM_ARM64(long _InterlockedCompareExchange_nf(long volatile * _Destination, long _Exchange, long _Comparand))
__MACHINEARM_ARM64(long _InterlockedCompareExchange_rel(long volatile * _Destination, long _Exchange, long _Comparand))
__MACHINE(long __MACHINECALL_CDECL_OR_DEFAULT _InterlockedDecrement(long volatile * _Addend))
__MACHINEWVMPURE(long _InterlockedDecrement(long volatile * _Addend))
__MACHINE(long __MACHINECALL_CDECL_OR_DEFAULT _InterlockedExchange(long volatile * _Target, long _Value))
__MACHINEWVMPURE(long __MACHINECALL_CDECL_OR_DEFAULT _InterlockedExchange(long volatile * _Target, long _Value))
__MACHINE(short _InterlockedExchange16(short volatile * _Target, short _Value))
__MACHINEARM_ARM64(short _InterlockedExchange16_acq(short volatile * _Target, short _Value))
__MACHINEARM_ARM64(short _InterlockedExchange16_nf(short volatile * _Target, short _Value))
__MACHINEARM_ARM64(short _InterlockedExchange16_rel(short volatile * _Target, short _Value))
__MACHINEARM_ARM64_X64(__int64 _InterlockedExchange64(__int64 volatile * _Target, __int64 _Value))
__MACHINEARM_ARM64(__int64 _InterlockedExchange64_acq(__int64 volatile * _Target, __int64 _Value))
__MACHINEARM_ARM64(__int64 _InterlockedExchange64_nf(__int64 volatile * _Target, __int64 _Value))
__MACHINEARM_ARM64(__int64 _InterlockedExchange64_rel(__int64 volatile * _Target, __int64 _Value))
__MACHINE(char _InterlockedExchange8(char volatile * _Target, char _Value))
__MACHINEARM_ARM64(char _InterlockedExchange8_acq(char volatile * _Target, char _Value))
__MACHINEARM_ARM64(char _InterlockedExchange8_nf(char volatile * _Target, char _Value))
__MACHINEARM_ARM64(char _InterlockedExchange8_rel(char volatile * _Target, char _Value))
__MACHINE(long __MACHINECALL_CDECL_OR_DEFAULT _InterlockedExchangeAdd(long volatile * _Addend, long _Value))
__MACHINE(short _InterlockedExchangeAdd16(short volatile * _Addend, short _Value))
__MACHINEARM_ARM64(short _InterlockedExchangeAdd16_acq(short volatile * _Addend, short _Value))
__MACHINEARM_ARM64(short _InterlockedExchangeAdd16_nf(short volatile * _Addend, short _Value))
__MACHINEARM_ARM64(short _InterlockedExchangeAdd16_rel(short volatile * _Addend, short _Value))
__MACHINEARM_ARM64_X64(__int64 _InterlockedExchangeAdd64(__int64 volatile * _Addend, __int64 _Value))
__MACHINEARM_ARM64(__int64 _InterlockedExchangeAdd64_acq(__int64 volatile * _Addend, __int64 _Value))
__MACHINEARM_ARM64(__int64 _InterlockedExchangeAdd64_nf(__int64 volatile * _Addend, __int64 _Value))
__MACHINEARM_ARM64(__int64 _InterlockedExchangeAdd64_rel(__int64 volatile * _Addend, __int64 _Value))
__MACHINE(char _InterlockedExchangeAdd8(char volatile * _Addend, char _Value))
__MACHINEARM_ARM64(char _InterlockedExchangeAdd8_acq(char volatile * _Addend, char _Value))
__MACHINEARM_ARM64(char _InterlockedExchangeAdd8_nf(char volatile * _Addend, char _Value))
__MACHINEARM_ARM64(char _InterlockedExchangeAdd8_rel(char volatile * _Addend, char _Value))
__MACHINEARM_ARM64(long _InterlockedExchangeAdd_acq(long volatile * _Addend, long _Value))
__MACHINEARM_ARM64(long _InterlockedExchangeAdd_nf(long volatile * _Addend, long _Value))
__MACHINEARM_ARM64(long _InterlockedExchangeAdd_rel(long volatile * _Addend, long _Value))
__MACHINEARM_ARM64(long _InterlockedExchange_acq(long volatile * _Target, long _Value))
__MACHINEARM_ARM64(long _InterlockedExchange_nf(long volatile * _Target, long _Value))
__MACHINEARM_ARM64(long _InterlockedExchange_rel(long volatile * _Target, long _Value))
__MACHINE(long __MACHINECALL_CDECL_OR_DEFAULT _InterlockedIncrement(long volatile * _Addend))
__MACHINEWVMPURE(long _InterlockedIncrement(long volatile * _Addend))
__MACHINE(long _InterlockedOr(long volatile * _Value, long _Mask))
__MACHINE(short _InterlockedOr16(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedOr16_acq(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedOr16_nf(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedOr16_rel(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64_X64(__int64 _InterlockedOr64(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedOr64_acq(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedOr64_nf(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedOr64_rel(__int64 volatile * _Value, __int64 _Mask))
__MACHINE(char _InterlockedOr8(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedOr8_acq(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedOr8_nf(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedOr8_rel(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(long _InterlockedOr_acq(long volatile * _Value, long _Mask))
__MACHINEARM_ARM64(long _InterlockedOr_nf(long volatile * _Value, long _Mask))
__MACHINEARM_ARM64(long _InterlockedOr_rel(long volatile * _Value, long _Mask))
__MACHINE(long _InterlockedXor(long volatile * _Value, long _Mask))
__MACHINE(short _InterlockedXor16(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedXor16_acq(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedXor16_nf(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64(short _InterlockedXor16_rel(short volatile * _Value, short _Mask))
__MACHINEARM_ARM64_X64(__int64 _InterlockedXor64(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedXor64_acq(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedXor64_nf(__int64 volatile * _Value, __int64 _Mask))
__MACHINEARM_ARM64(__int64 _InterlockedXor64_rel(__int64 volatile * _Value, __int64 _Mask))
__MACHINE(char _InterlockedXor8(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedXor8_acq(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedXor8_nf(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(char _InterlockedXor8_rel(char volatile * _Value, char _Mask))
__MACHINEARM_ARM64(long _InterlockedXor_acq(long volatile * _Value, long _Mask))
__MACHINEARM_ARM64(long _InterlockedXor_nf(long volatile * _Value, long _Mask))
__MACHINEARM_ARM64(long _InterlockedXor_rel(long volatile * _Value, long _Mask))
__MACHINE(void _ReadWriteBarrier(void))
__MACHINEARM_ARM64(__int16 __iso_volatile_load16(const volatile __int16 *))
__MACHINEARM_ARM64(__int32 __iso_volatile_load32(const volatile __int32 *))
__MACHINEARM_ARM64(__int64 __iso_volatile_load64(const volatile __int64 *))
__MACHINEARM_ARM64(__int8 __iso_volatile_load8(const volatile __int8 *))
__MACHINEARM_ARM64(void __iso_volatile_store16(volatile __int16 *, __int16))
__MACHINEARM_ARM64(void __iso_volatile_store32(volatile __int32 *, __int32))
__MACHINEARM_ARM64(void __iso_volatile_store64(volatile __int64 *, __int64))
__MACHINEARM_ARM64(void __iso_volatile_store8(volatile __int8 *, __int8))
__MACHINEARM(__int64 __ldrexd(const volatile __int64 *))
__MACHINEARM_ARM64(void __yield(void))
__MACHINE(unsigned char _interlockedbittestandset(long volatile *, long))
__MACHINEARM_ARM64(unsigned char _interlockedbittestandset_acq(long volatile *, long))
__MACHINEARM_ARM64(unsigned char _interlockedbittestandset_nf(long volatile *, long))
__MACHINEARM_ARM64(unsigned char _interlockedbittestandset_rel(long volatile *, long))

/*******************************************************************
* Note: New intrinsics should be added here IF AND ONLY IF they're *
* being used by the C++ Standard Library.                          *
* OTHERWISE, new intrinsics should be added to intrin.h.           *
*******************************************************************/

#if defined(__cplusplus)
}
#endif
#endif  /* __midl */
#endif  /* RC_INVOKED */
