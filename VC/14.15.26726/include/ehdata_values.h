//
// Declares internal C++ exception value macros.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#ifndef _INC_EHDATA_VALUES
#define _INC_EHDATA_VALUES
#pragma once

#include "ehdata_forceinclude.h"

#define EH_EXCEPTION_NUMBER	('msc' | 0xE0000000)	// The NT Exception # that we use

// As magic numbers increase, we have to keep track of the versions that we are
// backwards compatible with.  The top 3 bits of the magic number DWORD are
// used for other purposes, so while the magic number started as a YYYYMMDD
// date, it can't represent the year 2000 or later.  Old CRTs also recognize
// unknown magic numbers with a >= test.  Therefore, we just increment the
// the magic number by one every time we need another.
//
// EH_MAGIC_NUMBER1     The original, still used in exception records for
//                      native or mixed C++ thrown objects.
// EH_MAGIC_NUMBER2     Used in the FuncInfo if exception specifications are
//                      supported and FuncInfo::pESTypeList is valid.
// EH_MAGIC_NUMBER3     Used in the FuncInfo if FuncInfo::EHFlags is valid, so
//                      we can check if the function was compiled -EHs or -EHa.

#define EH_MAGIC_NUMBER1        0x19930520
#define EH_MAGIC_NUMBER2        0x19930521
#define EH_MAGIC_NUMBER3        0x19930522

// We use a different magic number in the thrown object's exception record to
// indicate it arose from a pure region.  Pure throws can't be caught by native
// or mixed code, and vice-versa.  Note that the pure magic number is only 7
// digits, not 8, so it will be less than any of the native EH magic numbers
// and won't be detected as an unknown newer magic number by the native EH
// handlers.

#define EH_PURE_MAGIC_NUMBER1   0x01994000

#define FI_EHS_FLAG             0x00000001
#define FI_DYNSTKALIGN_FLAG     0x00000002
#define FI_EHNOEXCEPT_FLAG      0x00000004

#if (EH_MAGIC_NUMBER2 <= EH_MAGIC_NUMBER1) || (EH_MAGIC_NUMBER3 <= EH_MAGIC_NUMBER2)
#error new magic number must be greater than the old one
#endif

#if (EH_MAGIC_NUMBER1 & 0xE0000000) || (EH_MAGIC_NUMBER2 & 0xE0000000) || (EH_MAGIC_NUMBER3 & 0xE0000000)
#error magic number too big -- must fit into 29 bits
#endif

#if (EH_PURE_MAGIC_NUMBER1 >= EH_MAGIC_NUMBER1)
#error pure EH magic number must be less than native one
#endif

#define EH_MAGIC_HAS_ES	EH_MAGIC_NUMBER2	// Magic number is greater or equal than that
											// indicates presence of exception specification

#if (defined(_M_AMD64) || defined(_M_ARM) || defined(_M_ARM64)) && !defined(_CHPE_X86_ARM64_EH_)
#define EH_EXCEPTION_PARAMETERS 4			// Number of parameters in exception record
#else
#define EH_EXCEPTION_PARAMETERS 3			// Number of parameters in exception record
#endif

#ifdef ALLOW_UNWIND_ABORT
#define EH_ABORT_FRAME_UNWIND_PART (EH_EXCEPTION_NUMBER + 1)
#endif

#define EH_EMPTY_STATE	(-1)

#endif // _INC_EHDATA_VALUES
