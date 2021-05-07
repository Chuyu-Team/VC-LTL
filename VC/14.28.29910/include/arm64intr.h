/***
*   arm64intr.h - definitions and declarations for ARM64 specific intrinsics
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the ARM64 specific intrinsics
*
****/

#pragma once


#if !defined(_M_ARM64) && !defined(_M_HYBRID_X86_ARM64) && !defined(_M_ARM64EC)
#error This header is specific to ARM64 targets
#endif

#define ARM64_SYSREG(op0, op1, crn, crm, op2) \
        ( ((op0 & 1) << 14) | \
          ((op1 & 7) << 11) | \
          ((crn & 15) << 7) | \
          ((crm & 15) << 3) | \
          ((op2 & 7) << 0) )

#define ARM64_FPCR              ARM64_SYSREG(3, 3, 4, 4, 0)   // Floating point control register (EL0)
#define ARM64_FPSR              ARM64_SYSREG(3, 3, 4, 4, 1)   // Floating point status register (EL0)

#if defined(__cplusplus)
extern "C" {
#endif

// ARM64-TRANSITION: define more. Only need barrier so far.

typedef enum _tag_ARM64INTR_BARRIER_TYPE
{
    _ARM64_BARRIER_SY    = 0xF,
    _ARM64_BARRIER_ST    = 0xE,
    _ARM64_BARRIER_LD    = 0xD,
    _ARM64_BARRIER_ISH   = 0xB,
    _ARM64_BARRIER_ISHST = 0xA,
    _ARM64_BARRIER_ISHLD = 0x9,
    _ARM64_BARRIER_NSH   = 0x7,
    _ARM64_BARRIER_NSHST = 0x6,
    _ARM64_BARRIER_NSHLD = 0x5,
    _ARM64_BARRIER_OSH   = 0x3,
    _ARM64_BARRIER_OSHST = 0x2,
    _ARM64_BARRIER_OSHLD = 0x1
}
_ARM64INTR_BARRIER_TYPE;

void __dmb(unsigned int _Type);
void __dsb(unsigned int _Type);
void __isb(unsigned int _Type);

unsigned __int8 __ldar8(unsigned __int8 volatile * _Target);
unsigned __int16 __ldar16(unsigned __int16 volatile * _Target);
unsigned __int32 __ldar32(unsigned __int32 volatile * _Target);
unsigned __int64 __ldar64(unsigned __int64 volatile * _Target);

unsigned __int8 __ldapr8(unsigned __int8 volatile * _Target);
unsigned __int16 __ldapr16(unsigned __int16 volatile * _Target);
unsigned __int32 __ldapr32(unsigned __int32 volatile * _Target);
unsigned __int64 __ldapr64(unsigned __int64 volatile * _Target);

void __stlr8(unsigned __int8 volatile * _Target, unsigned __int8 _Value);
void __stlr16(unsigned __int16 volatile * _Target, unsigned __int16 _Value);
void __stlr32(unsigned __int32 volatile * _Target, unsigned __int32 _Value);
void __stlr64(unsigned __int64 volatile * _Target, unsigned __int64 _Value);

unsigned __int8 __swp8(unsigned __int8 volatile * _Target, unsigned __int8 _Value);
unsigned __int16 __swp16(unsigned __int16 volatile * _Target, unsigned __int16 _Value);
unsigned __int32 __swp32(unsigned __int32 volatile * _Target, unsigned __int32 _Value);
unsigned __int64 __swp64(unsigned __int64 volatile * _Target, unsigned __int64 _Value);
unsigned __int8 __swpa8(unsigned __int8 volatile * _Target, unsigned __int8 _Value);
unsigned __int16 __swpa16(unsigned __int16 volatile * _Target, unsigned __int16 _Value);
unsigned __int32 __swpa32(unsigned __int32 volatile * _Target, unsigned __int32 _Value);
unsigned __int64 __swpa64(unsigned __int64 volatile * _Target, unsigned __int64 _Value);
unsigned __int8 __swpl8(unsigned __int8 volatile * _Target, unsigned __int8 _Value);
unsigned __int16 __swpl16(unsigned __int16 volatile * _Target, unsigned __int16 _Value);
unsigned __int32 __swpl32(unsigned __int32 volatile * _Target, unsigned __int32 _Value);
unsigned __int64 __swpl64(unsigned __int64 volatile * _Target, unsigned __int64 _Value);
unsigned __int8 __swpal8(unsigned __int8 volatile * _Target, unsigned __int8 _Value);
unsigned __int16 __swpal16(unsigned __int16 volatile * _Target, unsigned __int16 _Value);
unsigned __int32 __swpal32(unsigned __int32 volatile * _Target, unsigned __int32 _Value);
unsigned __int64 __swpal64(unsigned __int64 volatile * _Target, unsigned __int64 _Value);

unsigned __int8 __cas8(unsigned __int8 volatile * _Target, unsigned __int8 _Comp, unsigned __int8 _Value);
unsigned __int16 __cas16(unsigned __int16 volatile * _Target, unsigned __int16 _Comp, unsigned __int16 _Value);
unsigned __int32 __cas32(unsigned __int32 volatile * _Target, unsigned __int32 _Comp, unsigned __int32 _Value);
unsigned __int64 __cas64(unsigned __int64 volatile * _Target, unsigned __int64 _Comp, unsigned __int64 _Value);
unsigned __int8 __casa8(unsigned __int8 volatile * _Target, unsigned __int8 _Comp, unsigned __int8 _Value);
unsigned __int16 __casa16(unsigned __int16 volatile * _Target, unsigned __int16 _Comp, unsigned __int16 _Value);
unsigned __int32 __casa32(unsigned __int32 volatile * _Target, unsigned __int32 _Comp, unsigned __int32 _Value);
unsigned __int64 __casa64(unsigned __int64 volatile * _Target, unsigned __int64 _Comp, unsigned __int64 _Value);
unsigned __int8 __casl8(unsigned __int8 volatile * _Target, unsigned __int8 _Comp, unsigned __int8 _Value);
unsigned __int16 __casl16(unsigned __int16 volatile * _Target, unsigned __int16 _Comp, unsigned __int16 _Value);
unsigned __int32 __casl32(unsigned __int32 volatile * _Target, unsigned __int32 _Comp, unsigned __int32 _Value);
unsigned __int64 __casl64(unsigned __int64 volatile * _Target, unsigned __int64 _Comp, unsigned __int64 _Value);
unsigned __int8 __casal8(unsigned __int8 volatile * _Target, unsigned __int8 _Comp, unsigned __int8 _Value);
unsigned __int16 __casal16(unsigned __int16 volatile * _Target, unsigned __int16 _Comp, unsigned __int16 _Value);
unsigned __int32 __casal32(unsigned __int32 volatile * _Target, unsigned __int32 _Comp, unsigned __int32 _Value);
unsigned __int64 __casal64(unsigned __int64 volatile * _Target, unsigned __int64 _Comp, unsigned __int64 _Value);

void * __xpaci(void * _Pointer);

#ifdef __cplusplus
}
#endif
