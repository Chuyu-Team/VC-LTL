/***
*   arm64intr.h - definitions and declarations for ARM64 specific intrinsics
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the ARM64 specific intriniscs
*
****/

#pragma once


#if !defined(_M_ARM64) && !defined(_M_HYBRID_X86_ARM64)
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

// ARM64-TODO: define more. Only need barrier so far.

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


#ifdef __cplusplus
}
#endif
