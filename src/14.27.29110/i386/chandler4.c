/***
*chandler4.c - Structured Exception Handling part of _except_handler4
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines _except_handler4, the language specific handler for structured
*       exception handling on x86.  First created for Visual C++ 2005, replacing
*       _except_handler3, with the addition of security cookie checks.
*
*       For the CRT DLL, this actually defines _except_handler4_common, which
*       performs the real processing for chandler4gs.c's stub routine
*       _except_handler4.  We need a stub that is linked statically into an
*       image, so it can access the image-local global security cookie.  But all
*       it needs to do is pass through to this code, sending in the address of
*       the global cookie and the image's __security_check_cookie routine.  This
*       larger routine can then be placed in the CRT DLL.
*
*******************************************************************************/

#include <vcruntime_internal.h>
#include <Windows.h>

// copied from ntxcapi.h
#define EXCEPTION_UNWINDING 0x2         // Unwind is in progress
#define EXCEPTION_EXIT_UNWIND 0x4       // Exit unwind is in progress
#define EXCEPTION_STACK_INVALID 0x8     // Stack out of limits or unaligned
#define EXCEPTION_NESTED_CALL 0x10      // Nested exception handler call
#define EXCEPTION_TARGET_UNWIND 0x20    // Target unwind in progress
#define EXCEPTION_COLLIDED_UNWIND 0x40  // Collided exception handler call

#define EXCEPTION_UNWIND (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND | \
                          EXCEPTION_TARGET_UNWIND | EXCEPTION_COLLIDED_UNWIND)

#define IS_UNWINDING(Flag) ((Flag & EXCEPTION_UNWIND) != 0)
#define IS_DISPATCHING(Flag) ((Flag & EXCEPTION_UNWIND) == 0)
#define IS_TARGET_UNWIND(Flag) (Flag & EXCEPTION_TARGET_UNWIND)

BEGIN_PRAGMA_OPTIMIZE_ENABLE("t", DevDivVSO:162582, "This file is performance-critical and should always be optimized for speed")

/*
 * Define __except filter/handler and __finally handler function types.
 */

typedef LONG (__cdecl *PEXCEPTION_FILTER_X86)(void);
typedef void (__cdecl *PEXCEPTION_HANDLER_X86)(void);
typedef void (__fastcall *PTERMINATION_HANDLER_X86)(BOOL);

/*
 * Define type of pointer to __security_check_cookie.
 */

typedef void (__fastcall *PCOOKIE_CHECK)(UINT_PTR);

/*
 * The function-specific scope table pointed to by the on-stack exception
 * registration record.  This describes the nesting structure of __try blocks
 * in the function, the location of all __except filters, __except blocks, and
 * __finally blocks, and the data required to check the security cookies in
 * the function
 */

typedef struct _EH4_SCOPETABLE_RECORD
{
    ULONG                           EnclosingLevel;
    PEXCEPTION_FILTER_X86           FilterFunc;
    union
    {
        PEXCEPTION_HANDLER_X86      HandlerAddress;
        PTERMINATION_HANDLER_X86    FinallyFunc;
    } u;
} EH4_SCOPETABLE_RECORD, *PEH4_SCOPETABLE_RECORD;

typedef struct _EH4_SCOPETABLE
{
    ULONG                       GSCookieOffset;
    ULONG                       GSCookieXOROffset;
    ULONG                       EHCookieOffset;
    ULONG                       EHCookieXOROffset;
    EH4_SCOPETABLE_RECORD       ScopeRecord[1];
} EH4_SCOPETABLE, *PEH4_SCOPETABLE;

#define NO_GS_COOKIE        ((ULONG)-2)

#define TOPMOST_TRY_LEVEL   ((ULONG)-2)

/*
 * The exception registration record stored in the stack frame.  The linked
 * list of registration records goes through the EXCEPTION_REGISTRATION_RECORD
 * sub-struct, so some fields here are at negative offsets with regards to
 * the registration record pointer we are passed.
 */

typedef struct _EH4_EXCEPTION_REGISTRATION_RECORD
{
    PVOID                           SavedESP;
    PEXCEPTION_POINTERS             ExceptionPointers;
    EXCEPTION_REGISTRATION_RECORD   SubRecord;
    UINT_PTR                        EncodedScopeTable;
    ULONG                           TryLevel;
} EH4_EXCEPTION_REGISTRATION_RECORD, *PEH4_EXCEPTION_REGISTRATION_RECORD;

/*
 * External dependencies not found in headers.
 */

#pragma warning(disable: 4132)  // communal object is intentionally const
void (__cdecl * const _pDestructExceptionObject)(
    PEXCEPTION_RECORD   pExcept,
    int                 fThrowNotAllowed
    );

#define EH_EXCEPTION_NUMBER ('msc' | 0xE0000000)

extern LONG __fastcall
_EH4_CallFilterFunc(
    _In_ PEXCEPTION_FILTER_X86  FilterFunc,
    _In_ PCHAR                  FramePointer
    );

extern void __declspec(noreturn) __fastcall
_EH4_TransferToHandler(
    _In_ PEXCEPTION_HANDLER_X86 HandlerAddress,
    _In_ PCHAR                  FramePointer
    );

extern void __fastcall
_EH4_GlobalUnwind2(
    _In_opt_ PEXCEPTION_REGISTRATION_RECORD  EstablisherFrame,
    _In_opt_ PEXCEPTION_RECORD               ExceptionRecord
    );

extern void __fastcall
_EH4_LocalUnwind(
    _In_ PEXCEPTION_REGISTRATION_RECORD  EstablisherFrame,
    _In_ ULONG                           TargetLevel,
    _In_ PCHAR                           FramePointer,
    _In_ PUINT_PTR                       CookiePointer
    );

#pragma warning(disable: 4100)  // ignore unreferenced formal parameters

#ifdef _M_HYBRID_X86_ARM64

#pragma intrinsic(_HybridGenerateThunks)

//
// Generate wrappers for routines implemented in asm.
//

LONG __fastcall
_EH4_CallFilterFunc(
    PEXCEPTION_FILTER_X86   FilterFunc,
    PCHAR                   FramePointer
    )
{
    _HybridGenerateThunks((void *)_EH4_CallFilterFunc, 1);
    return 0;
}

void __declspec(noreturn) __fastcall
_EH4_TransferToHandler(
    PEXCEPTION_HANDLER_X86  HandlerAddress,
    PCHAR                   FramePointer
    )
{
    _HybridGenerateThunks((void *)_EH4_TransferToHandler, 1);
}

void __fastcall
_EH4_GlobalUnwind2(
    PEXCEPTION_REGISTRATION_RECORD  EstablisherFrame,
    PEXCEPTION_RECORD               ExceptionRecord
    )
{
    _HybridGenerateThunks((void *)_EH4_GlobalUnwind2, 1);
}

void __fastcall
_EH4_LocalUnwind(
    PEXCEPTION_REGISTRATION_RECORD  EstablisherFrame,
    ULONG                           TargetLevel,
    PCHAR                           FramePointer,
    PUINT_PTR                       CookiePointer
    )
{
    _HybridGenerateThunks((void *)_EH4_LocalUnwind, 1);
}

#endif

extern void
__except_validate_context_record(
    _In_ PCONTEXT ContextRecord
    );

/***
*ValidateLocalCookies - perform local cookie validation during SEH processing
*
*Purpose:
*   Perform the security checks for _except_handler4.
*
*Entry:
*   CookieCheckFunction - (CRT DLL only) pointer to __security_check_cookie in
*       the target image
*   ScopeTable - pointer to the unencoded scope table from the exception
*       registration record
*   FramePointer - EBP frame pointer for the function that established the
*       exception registration record
*
*Return:
*   If the security checks fail, the process is terminated via a Watson dump.
*
*******************************************************************************/

static __declspec(guard(ignore)) void
ValidateLocalCookies(
#if defined(CRTDLL)
    IN PCOOKIE_CHECK    CookieCheckFunction,
#endif
    IN PEH4_SCOPETABLE  ScopeTable,
    _When_(ScopeTable->GSCookieOffset >= ScopeTable->EHCookieOffset,
        _In_reads_bytes_(ScopeTable->GSCookieOffset + sizeof(UINT_PTR)))
    _When_(ScopeTable->GSCookieOffset <  ScopeTable->EHCookieOffset,
        _In_reads_bytes_(ScopeTable->EHCookieOffset + sizeof(UINT_PTR)))
       PCHAR            FramePointer
    )
{
    UINT_PTR        GSCookie;
    UINT_PTR        EHCookie;

    if (ScopeTable->GSCookieOffset != NO_GS_COOKIE)
    {
        GSCookie = *(PUINT_PTR)(FramePointer + ScopeTable->GSCookieOffset);
        GSCookie ^= (UINT_PTR)(FramePointer + ScopeTable->GSCookieXOROffset);
#if defined(CRTDLL)
        _GUARD_CHECK_ICALL(CookieCheckFunction);
        (*CookieCheckFunction)(GSCookie);
#else
        __security_check_cookie(GSCookie);
#endif
    }

    EHCookie = *(PUINT_PTR)(FramePointer + ScopeTable->EHCookieOffset);
    EHCookie ^= (UINT_PTR)(FramePointer + ScopeTable->EHCookieXOROffset);
#if defined(CRTDLL)
    _GUARD_CHECK_ICALL(CookieCheckFunction);
    (*CookieCheckFunction)(EHCookie);
#else
    __security_check_cookie(EHCookie);
#endif
}

/***
*_except_handler4 - (non-CRT DLL only) SEH handler with security cookie checks
*
*_except_handler4_common - (CRT DLL only) actual SEH implementation called by
*                          _except_handler4 stub
*
*Purpose:
*   Implement structured exception handling for functions which have __try/
*   __except/__finally.  This version of SEH also performs security cookie
*   checks to detect buffer overruns which potentially corrupt the on-stack
*   exception handling data, terminating the process before such corruption
*   can be exploited.
*
*   Call exception and termination handlers as necessary, based on the current
*   execution point within the function.
*
*Entry:
*   CookiePointer - (CRT DLL only) pointer to the global security cookie to be
*       used to decode the scope table pointer in the exception registration
*       record
*   CookieCheckFunction - (CRT DLL only) pointer to __security_check_cookie in
*       the calling image
*   ExceptionRecord - pointer to the exception being dispatched
*   EstablisherFrame - pointer to the on-stack exception registration record
*       for this function
*   ContextRecord - pointer to a context record for the point of exception
*   DispatcherContext - pointer to the exception dispatcher or unwind
*       dispatcher context
*
*Return:
*   If the security checks fail, the process is terminated via a Watson dump.
*
*   If an exception is being dispatched and the exception is handled by an
*   __except filter for this exception frame, then this function does not
*   return.  Instead, it calls RtlUnwind and transfers control to the __except
*   block corresponding to the accepting __except filter.  Otherwise, an
*   exception disposition of continue execution or continue search is returned.
*
*   If an unwind is being dispatched, then each termination handler (__finally)
*   is called and a value of continue search is returned.
*
*******************************************************************************/

#if defined(_M_IX86) && !defined(CRTDLL) && !defined(_M_HYBRID)
// Filter incorrect x86 floating point exceptions, unless linkopt that provides an empty filter is available.
#pragma comment(linker, "/alternatename:__filter_x86_sse2_floating_point_exception=__filter_x86_sse2_floating_point_exception_default")
#endif

DECLSPEC_GUARD_SUPPRESS
EXCEPTION_DISPOSITION
#if !defined(CRTDLL)
_except_handler4(
#else
_except_handler4_common(
    IN PUINT_PTR                        CookiePointer,
    IN PCOOKIE_CHECK                    CookieCheckFunction,
#endif
    IN PEXCEPTION_RECORD                ExceptionRecord,
    IN PEXCEPTION_REGISTRATION_RECORD   EstablisherFrame,
    IN OUT PCONTEXT                     ContextRecord,
    IN OUT PVOID                        DispatcherContext
    )
{
    PEH4_EXCEPTION_REGISTRATION_RECORD  RegistrationNode;
    PCHAR                               FramePointer;
    PEH4_SCOPETABLE                     ScopeTable;
    ULONG                               TryLevel;
    ULONG                               EnclosingLevel;
    EXCEPTION_POINTERS                  ExceptionPointers;
    PEH4_SCOPETABLE_RECORD              ScopeTableRecord;
    PEXCEPTION_FILTER_X86               FilterFunc;
    LONG                                FilterResult;
    BOOLEAN                             Revalidate = FALSE;
    EXCEPTION_DISPOSITION               Disposition = ExceptionContinueSearch;

#ifdef _M_HYBRID_X86_ARM64

    //
    // Ensure that this function is implemented as guest code so that there are
    // no dynamic EH registration nodes added for push thunk frames.
    //

#if defined(CRTDLL)
    _HybridGenerateThunks((void *)_except_handler4_common, 1);
#else
    _HybridGenerateThunks((void *)_except_handler4, 1);
#endif
#endif

#if defined(_M_IX86) && !defined(CRTDLL) && !defined(_M_HYBRID)
    ExceptionRecord->ExceptionCode = _filter_x86_sse2_floating_point_exception(ExceptionRecord->ExceptionCode);
#endif

    //
    // We are passed a registration record which is a field offset from the
    // start of our true registration record.
    //

    RegistrationNode =
        (PEH4_EXCEPTION_REGISTRATION_RECORD)
        ( (PCHAR)EstablisherFrame -
          FIELD_OFFSET(EH4_EXCEPTION_REGISTRATION_RECORD, SubRecord) );

    //
    // The EBP frame pointer in the function corresponding to the registration
    // record will be immediately following the record.  If the function uses
    // FPO, this is a "virtual" frame pointer outside of exception handling,
    // but it's still the EBP value set when calling into the handlers or
    // filters.
    //

    FramePointer = (PCHAR)(RegistrationNode + 1);

    //
    // Retrieve the scope table pointer, which encodes where we find the local
    // security cookies within the function's frame, as well as how the guarded
    // blocks in the target function are laid out.  This pointer was XORed with
    // the image-local global security cookie when originally stored, to avoid
    // attacks which spoof the table to address valid local cookies elsewhere
    // on the stack.
    //

#if defined(CRTDLL)
    ScopeTable = (PEH4_SCOPETABLE)
                    (RegistrationNode->EncodedScopeTable ^ *CookiePointer);
#else
    ScopeTable = (PEH4_SCOPETABLE)
                    (RegistrationNode->EncodedScopeTable ^ __security_cookie);
#endif

    //
    // Perform the initial security cookie validation.
    //

    ValidateLocalCookies(
#if defined(CRTDLL)
        CookieCheckFunction,
#endif
        ScopeTable,
        FramePointer
        );

    __except_validate_context_record(ContextRecord);

    //
    // Security checks have passed, begin actual exception handling.
    //

    if (IS_DISPATCHING(ExceptionRecord->ExceptionFlags))
    {
        //
        // An exception dispatch is in progress.  First build the
        // EXCEPTION_POINTERS record queried by the _exception_info intrinsic
        // and save it in the exception registration record so the __except
        // filter can find it.
        //

        ExceptionPointers.ExceptionRecord = ExceptionRecord;
        ExceptionPointers.ContextRecord = ContextRecord;
        RegistrationNode->ExceptionPointers = &ExceptionPointers;

        //
        // Scan the scope table and call the appropriate __except filters until
        // we find one that accepts the exception.
        //

        for (TryLevel = RegistrationNode->TryLevel;
             TryLevel != TOPMOST_TRY_LEVEL;
             TryLevel = EnclosingLevel)
        {
            ScopeTableRecord = &ScopeTable->ScopeRecord[TryLevel];
            FilterFunc = ScopeTableRecord->FilterFunc;
            EnclosingLevel = ScopeTableRecord->EnclosingLevel;

            if (FilterFunc != NULL)
            {
                //
                // The current scope table record is for an __except.
                // Call the __except filter to see if we've found an
                // accepting handler.
                //

                FilterResult = _EH4_CallFilterFunc(FilterFunc, FramePointer);
                Revalidate = TRUE;

                //
                // If the __except filter returned a negative result, then
                // dismiss the exception.  If it returned a positive result,
                // unwind to the accepting exception handler.  Otherwise keep
                // searching for an exception filter.
                //

                if (FilterResult < 0)
                {
                    Disposition = ExceptionContinueExecution;
                    break;
                }
                else if (FilterResult > 0)
                {
#if !defined(_NTSUBSET_)
                    //
                    // If we're handling a thrown C++ exception, let the C++
                    // exception handler destruct the thrown object.  This call
                    // is through a function pointer to avoid linking to the
                    // C++ EH support unless it's already present.  Don't call
                    // the function pointer unless it's in read-only memory.
                    //

                    if (ExceptionRecord->ExceptionCode == EH_EXCEPTION_NUMBER &&
                        _pDestructExceptionObject != NULL &&
                        _IsNonwritableInCurrentImage((PBYTE)&_pDestructExceptionObject))
                    {
                        (*_pDestructExceptionObject)(ExceptionRecord, TRUE);
                    }
#endif  // !defined(_NTSUBSET_)



                    //
                    // Unwind all registration nodes below this one, then unwind
                    // the nested __try levels.
                    //
                    _EH4_GlobalUnwind2(
                        &RegistrationNode->SubRecord,
                        ExceptionRecord
                        );

                    if (RegistrationNode->TryLevel != TryLevel)
                    {
                        _EH4_LocalUnwind(
                            &RegistrationNode->SubRecord,
                            TryLevel,
                            FramePointer,
#if defined(CRTDLL)
                            CookiePointer
#else
                            &__security_cookie
#endif
                            );
                    }

                    //
                    // Set the __try level to the enclosing level, since it is
                    // the enclosing level, if any, that guards the __except
                    // handler.
                    //

                    RegistrationNode->TryLevel = EnclosingLevel;

                    //
                    // Redo the security checks, in case any __except filters
                    // or __finally handlers have caused overruns in the target
                    // frame.
                    //

                    ValidateLocalCookies(
#if defined(CRTDLL)
                        CookieCheckFunction,
#endif
                        ScopeTable,
                        FramePointer
                        );

                    //
                    // Call the __except handler.  This call will not return.
                    // The __except handler will reload ESP from the
                    // registration record upon entry.  The EBP frame pointer
                    // for the handler is directly after the registration node.
                    //

                    _EH4_TransferToHandler(
                        ScopeTableRecord->u.HandlerAddress,
                        FramePointer
                        );
                }
            }
        }
    }
    else
    {
        //
        // An exception unwind is in progress, and this isn't the target of the
        // unwind.  Unwind any active __try levels in this function, calling
        // the applicable __finally handlers.
        //

        if (RegistrationNode->TryLevel != TOPMOST_TRY_LEVEL)
        {
            _EH4_LocalUnwind(
                &RegistrationNode->SubRecord,
                TOPMOST_TRY_LEVEL,
                FramePointer,
#if defined(CRTDLL)
                CookiePointer
#else
                &__security_cookie
#endif
                );
            Revalidate = TRUE;
        }
    }

    //
    // If we called any __except filters or __finally handlers, then redo the
    // security checks, in case those funclets have caused overruns in the
    // target frame.
    //

    if (Revalidate)
    {
        ValidateLocalCookies(
#if defined(CRTDLL)
            CookieCheckFunction,
#endif
            ScopeTable,
            FramePointer
            );
    }

    //
    // Continue searching for exception or termination handlers in previous
    // registration records higher up the stack, or resume execution if we're
    // here because an __except filter returned a negative result.
    //

    return Disposition;
}
