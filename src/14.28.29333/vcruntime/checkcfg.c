/***
*checkcfg.c - logic and globals to support the Guard security feature
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

#include <vcruntime_internal.h>

extern PVOID __guard_check_icall_fptr;

typedef
void
(__fastcall *GUARDCF_CHECK_ROUTINE) (
    uintptr_t Target
    );

#pragma warning(suppress: 4918) // 'y' is not necessarily supported on all architectures
BEGIN_PRAGMA_OPTIMIZE_ENABLE("y", DevDivVSO:162582, "Required to avoid poor codegen--notably on ARM")

extern
__declspec(guard(ignore))
__inline
void
__fastcall
_guard_check_icall (
    _In_ uintptr_t Target
    )

/*++

Routine Description:

    This function performs an ICall check when invoked by the compiler to
    check the integrity of a function pointer for Control Flow Guard (/guard).

    N.B.  This function is only retained for compatibility with pre-17.1 LKG4
          compilers that do not directly invoke through the
          __guard_check_icall_fptr function pointer.

Arguments:

    Target - Supplies the function pointer to check.

Return Value:

    None.  If the function pointer supplied was invalid, then a fast fail event
    or an access violation is raised.

--*/

{

    ((GUARDCF_CHECK_ROUTINE)__guard_check_icall_fptr)(Target);
    return;
}
