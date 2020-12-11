/*++

Copyright (c) Microsoft Corporation

Module Name:

    helpexcept.cpp

Abstract:

    This module contains support routines for generating exceptions
    for helper functions.

--*/

#include <Windows.h>
#include "fpieee.h"

#define CPVFP_FPSCR            10, 7,  1,  0, 0

extern "C" UINT64
__fphelper_raise_exception(
    UINT32  FpscrReason,
    UINT32  Operation,
    UINT32  Param1Format,
    UINT32  ResultFormat,
    UINT64  Param1Value,
    UINT64  ResultValue
    )

/*++

Routine Description:

    This function accepts basic IEEE floating-point exception information from
    a software emulation routine that detected the exception.  All of the
    exception information is packaged into a standard _FPIEEE_RECORD structure
    and passed to RaiseException. On return from RaiseException, the structure
    is unpacked, causing anything that the user changed to be reflected in the
    floating-point status. The calling parameters have been selected and
    arranged for the convenience of the assembly language coded emulation
    routines.

Arguments:

    FpscrReason - Supplies the exception bit from the FPSCR (one of the low 5)
        which indicates the cause of this particular exception.

    Operation - Supplies a code describing the operation, which must be one
        of the _FP_OPERATION_CODE values described in fpieee.h.

    Param1Format - Supplies the format of the first (and only) source parameter,
        which must be one of the _FPIEEE_FORMAT values described in fpieee.h.

    ResultFormat - Supplies the format of the result, again using one of the
        _FPIEEE_FORMAT values.

    Param1Value - Supplies the value of the first source parameter as a 64-bit
        piece of data. If the parameter is only 32 bits, it should be placed
        into the low 32 bits of this value.

    ResultValue - Supplies the (likely incorrect or inexact) result value as
        a 64-bit piece of data. Again, if the result is only 32 bits, it
        should be placed into the low 32 bits of this value.

Return Value:

    The result, possibly modified by the user's exception handler, after return
    from RaiseException.

--*/

{
    _FPIEEE_RECORD FPIR;                        // Filled in for RaiseException
    _FPIEEE_RECORD *pFPIR = &FPIR;              // RaiseException wants &(&FPIR)
    ULONG_PTR ExceptionInformation[1];          // Exception arguments passed to RaiseException
    BOOL DoRaise = FALSE;                       // True if we need to raise an exception.
    DWORD StartingFpscr;
    DWORD UpdatedFpscr;
    DWORD Status = 0;                           // One of STATUS_FLOAT_*

    StartingFpscr = _MoveFromCoprocessor(CPVFP_FPSCR);

    // FPIEEE_ROUNDING_MODE maps to FPSCR rounding mode if you swap the two bits
    FPIR.RoundingMode = ((StartingFpscr >> 21) & 2) | ((StartingFpscr >> 23) & 1);

    // Precision is implied by the operation, so just set the precision to "Full"
    FPIR.Precision = _FpPrecisionFull;

    // Operation is specified by the caller
    FPIR.Operation = Operation;

    // Cause comes directly from the provided cause
    FPIR.Cause.InvalidOperation = (FpscrReason >> 0) & 1;
    FPIR.Cause.ZeroDivide = (FpscrReason >> 1) & 1;
    FPIR.Cause.Overflow = (FpscrReason >> 2) & 1;
    FPIR.Cause.Underflow = (FpscrReason >> 3) & 1;
    FPIR.Cause.Inexact = (FpscrReason >> 4) & 1;

    // Enable comes from the FPSCR enable bits
    FPIR.Enable.InvalidOperation = (StartingFpscr >> 8) & 1;
    FPIR.Enable.ZeroDivide = (StartingFpscr >> 9) & 1;
    FPIR.Enable.Overflow = (StartingFpscr >> 10) & 1;
    FPIR.Enable.Underflow = (StartingFpscr >> 11) & 1;
    FPIR.Enable.Inexact = (StartingFpscr >> 12) & 1;

    // Status comes from the FPSCR status bits
    FPIR.Status.InvalidOperation = (StartingFpscr >> 0) & 1;
    FPIR.Status.ZeroDivide = (StartingFpscr >> 1) & 1;
    FPIR.Status.Overflow = (StartingFpscr >> 2) & 1;
    FPIR.Status.Underflow = (StartingFpscr >> 3) & 1;
    FPIR.Status.Inexact = (StartingFpscr >> 4) & 1;

    // Operand 1 is provided explicitly
    FPIR.Operand1.Value.Q64Value = Param1Value;
    FPIR.Operand1.OperandValid = 1;
    FPIR.Operand1.Format = Param1Format;

    // Operand 2 is not valid
    FPIR.Operand2.OperandValid = 0;

    // Result is provided explicitly
    FPIR.Result.Value.Q64Value = ResultValue;
    FPIR.Result.OperandValid = 1;
    FPIR.Result.Format = ResultFormat;

    // Raise the appropriate exception
    if (FPIR.Status.InvalidOperation && FPIR.Enable.InvalidOperation) {
        Status = STATUS_FLOAT_INVALID_OPERATION;
        DoRaise = TRUE;
    }
    else if (FPIR.Status.ZeroDivide && FPIR.Enable.ZeroDivide) {
        Status = STATUS_FLOAT_DIVIDE_BY_ZERO;
        DoRaise = TRUE;
    }
    else if (FPIR.Status.Overflow && FPIR.Enable.Overflow) {
        Status = STATUS_FLOAT_OVERFLOW;
        DoRaise = TRUE;
    }
    else if (FPIR.Status.Underflow && FPIR.Enable.Underflow) {
        Status = STATUS_FLOAT_UNDERFLOW;
        DoRaise = TRUE;
    }
    else if (FPIR.Status.Inexact && FPIR.Enable.Inexact) {
        Status = STATUS_FLOAT_INEXACT_RESULT;
        DoRaise = TRUE;
    }

    // bail if we don't need to do anything
    if (!DoRaise) {
        return ResultValue;
    }

    // build the exception information
    *(_FPIEEE_RECORD ***)&ExceptionInformation[0] = &pFPIR;
    RaiseException(Status, 0, 1, ExceptionInformation);

    // update the enable mask
    UpdatedFpscr = StartingFpscr & ~((0x1f << 8) | (3 << 22));
    UpdatedFpscr |= FPIR.Enable.InvalidOperation ? (1 << 8) : 0;
    UpdatedFpscr |= FPIR.Enable.ZeroDivide ? (1 << 9) : 0;
    UpdatedFpscr |= FPIR.Enable.Overflow ? (1 << 10) : 0;
    UpdatedFpscr |= FPIR.Enable.Underflow ? (1 << 11) : 0;
    UpdatedFpscr |= FPIR.Enable.Inexact ? (1 << 12) : 0;

    // update the rounding mode
    UpdatedFpscr |= (FPIR.RoundingMode & 1) << 23;
    UpdatedFpscr |= (FPIR.RoundingMode & 2) << 21;

    _MoveToCoprocessor(UpdatedFpscr, CPVFP_FPSCR);

    // return the possibly modified value
    return FPIR.Result.Value.Q64Value;
}


extern "C" void
__helper_divide_by_0(
    )

/*++

Routine Description:

    Raise a basic integer divide by zero exception.

Arguments:

    None

Return Value:

    None

--*/

{
    RaiseException(STATUS_INTEGER_DIVIDE_BY_ZERO, 0, 0, NULL);
}
