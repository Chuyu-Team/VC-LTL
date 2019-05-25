/***
*guard_support.c - logic and globals to support the Guard security feature
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Define the globals and default check routine for the Guard security
*       feature.
*
*       N.B.  Code and data declared by this module must be in the form of
*             selectany COMDATs, so that duplicate definitions between the
*             CRT and no_cfg_support are permitted for the restricted use
*             cases of the no_cfg_support module.
*
*******************************************************************************/

#include <Windows.h>

typedef
void
(__fastcall *GUARDCF_CHECK_ROUTINE) (
    uintptr_t Target
    );

#if defined(_AMD64_)

//
// Deliberately misdeclared as data and not code so that the free branch
// instruction is not itself considered a valid call target.
//

extern const unsigned char _guard_dispatch_icall_nop;

#endif

//
// The CHPE version of the _guard_check_icall_nop function does not preserve
// ECX, as is required by the custom CFG calling convention on x86. Force the
// guest version of this function to be chosen instead.
//

#if defined(_M_HYBRID_X86_ARM64)

__declspec(hybrid_guest)
void
__fastcall
_guard_check_icall_nop (
    _In_ uintptr_t Target
    );

#else

extern
__inline
void
__fastcall
_guard_check_icall_nop (
    _In_ uintptr_t Target
    )

/*++

Routine Description:

    This function performs a no-op check when invoked by the compiler to check
    the integrity of a function pointer for Control Flow Guard (/guard).

Arguments:

    Target - Supplies the function pointer to check.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Target);
    return;
}

#endif

#pragma section(".00cfg", read)

// Devdiv:1093613 - __guard_icall_check_fptr is only modified by the loader and
// so it appears to be read-only during compilation. We mark it volatile to
// ensure uses of it are not optimized away.
__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
volatile PVOID __guard_check_icall_fptr = (PVOID)_guard_check_icall_nop;

#if defined(_AMD64_)

__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
PVOID __guard_dispatch_icall_fptr = (PVOID)&_guard_dispatch_icall_nop;

#endif

extern
__inline
int
_guard_icall_checks_enforced (
    VOID
    )

/*++

Routine Description:

    This function determines whether Control Flow Guard is enforced for the
    current module.

Arguments:

    None.

Return Value:

    A nonzero value is returned as the function value if Control Flow Guard
    is enforced for the current module.

--*/

{

    return (ReadPointerNoFence(&__guard_check_icall_fptr) != (PVOID)_guard_check_icall_nop);
}


#if defined(_AMD64_)
extern
__inline
int
__cdecl
_guard_rf_checks_enforced (
    VOID
    )

/*++

Routine Description:

    This function determines whether Return Flow Guard is enforced for the
    current module.

Arguments:

    None.

Return Value:

    A nonzero value is returned as the function value if Return Flow Guard
    is enforced for the current module.

--*/

{
    return 0;
}

#endif
