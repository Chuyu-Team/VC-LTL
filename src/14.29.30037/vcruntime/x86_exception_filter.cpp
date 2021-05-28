//
// x86_exception_filter.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Floating point exceptions from SSE2 instructions when running on x86/wow64 are
// reported as either STATUS_FLOAT_MULTIPLE_FAULTS or STATUS_FLOAT_MULTIPLE_TRAPS
// instead of the expected EXCEPTION_FLT_* exception codes. This is an issue in
// the x86/wow64 exception trap that we are addressing by adding an
// exception code filter to _except_handler4 that will check for these conditions,
// re-parse the exception code, and pass the correct exception code to the rest
// of the EH machinery.
//
#include <vcruntime_internal.h>
#include <intrin.h>
#include <Windows.h>

extern "C" long __CRTDECL _filter_x86_sse2_floating_point_exception_default(long const exception_code)
{
    // If we're using SSE2 instructions on x86, the OS exception trap does not correctly
    // return accurate floating point exception codes.
    // If we don't support SSE2, we don't need to worry.
    if (__isa_available < __ISA_AVAILABLE_SSE2) {
        return exception_code;
    }

    // Check for one of two "blanket" exception codes that cover the more specific ones.
    if (   exception_code != STATUS_FLOAT_MULTIPLE_FAULTS
        && exception_code != STATUS_FLOAT_MULTIPLE_TRAPS) {
        return exception_code;
   }

    // _mm_getcsr() (SSE2 fp status register 'mxcsr') format:
    //           Excpt Mask          Excpt Set
    // [32-13] | P U O Z D I | [6] | P U O Z D I |
    // P = Precision (inexact)
    // U = Underflow
    // O = Overflow
    // Z = Divide-by-zero
    // D = Denormal
    // I = Invalid Operation

    int const invalid_operation_set  = 0x0001;
    int const denormal_operation_set = 0x0002;
    int const zero_divide_set        = 0x0004;
    int const overflow_set           = 0x0008;
    int const underflow_set          = 0x0010;
    int const precision_set          = 0x0020;
    int const exception_set_mask     = 0x003f;

    int const invalid_operation_disabled  = 0x0080;
    int const denormal_operation_disabled = 0x0100;
    int const zero_divide_disabled        = 0x0200;
    int const overflow_disabled           = 0x0400;
    int const underflow_disabled          = 0x0800;
    int const precision_disabled          = 0x1000;

    // Check whether exception is enabled and set with one operation by inverting the set bits.
    //     ((status & exception_disabled) == 0) && ((status & exception_set) != 0)
    // is equivalent to:
    //     ((status ^ exception_set_mask) & (exception_disabled | exception_set)) == 0

    int const invalid_operation  = invalid_operation_disabled  | invalid_operation_set;
    int const denormal_operation = denormal_operation_disabled | denormal_operation_set;
    int const zero_divide        = zero_divide_disabled        | zero_divide_set;
    int const overflow           = overflow_disabled           | overflow_set;
    int const underflow          = underflow_disabled          | underflow_set;
    int const precision          = precision_disabled          | precision_set;

    int const status_with_inverted_set = _mm_getcsr() ^ exception_set_mask;

    // Same flag check ordering as x64 exception trap.
    if ((status_with_inverted_set & invalid_operation) == 0) {
        return EXCEPTION_FLT_INVALID_OPERATION;
    }

    if ((status_with_inverted_set & zero_divide) == 0) {
        return EXCEPTION_FLT_DIVIDE_BY_ZERO;
    }

    if ((status_with_inverted_set & denormal_operation) == 0) {
        return EXCEPTION_FLT_INVALID_OPERATION;
    }

    if ((status_with_inverted_set & overflow) == 0) {
        return EXCEPTION_FLT_OVERFLOW;
    }

    if ((status_with_inverted_set & underflow) == 0) {
        return EXCEPTION_FLT_UNDERFLOW;
    }

    if ((status_with_inverted_set & precision) == 0) {
        return EXCEPTION_FLT_INEXACT_RESULT;
    }

    return exception_code;
}
