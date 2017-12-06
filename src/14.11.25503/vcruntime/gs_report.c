//
// gs_report.c
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of functions used to report a security check failure.  This file
// corresponds to gs_report.c in the Windows sources.
//
#include <vcruntime_internal.h>



#define STATUS_SECURITY_CHECK_FAILURE STATUS_STACK_BUFFER_OVERRUN

// Check if __fastfail is known to be available or if it should be checked.
// Simplifies subsequent ifdefs.
#if defined _CRT_APP || defined _M_ARM || defined _M_ARM64 || defined _SCRT_ENCLAVE_BUILD

#define PROCESSOR_FAST_FAIL_AVAILABLE

#elif defined _M_IX86 || defined _M_X64

// No extra define, lack of define from above signals this path.

#else

#error Unsupported architecture

#endif


extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;



// Use global memory for the exception and context records, and any other local
// state needed by __report_gsfailure and __report_securityfailure.  We're not
// worried about multithread issues here - we're going to die anyway, and it
// saves us from consuming a good chunk of stack, as well as potentially
// overwriting useful data in the stack memory dump.
static EXCEPTION_RECORD         GS_ExceptionRecord;
static CONTEXT                  GS_ContextRecord;
static EXCEPTION_POINTERS const GS_ExceptionPointers =
{
    &GS_ExceptionRecord,
    &GS_ContextRecord
};



#ifdef _VCRT_BUILD
    static BOOL DebuggerWasPresent;
#endif



#if !defined PROCESSOR_FAST_FAIL_AVAILABLE && defined _M_X64

    static void __cdecl capture_current_context(CONTEXT* pContextRecord)
    {
        ULONG64           ControlPc;
        ULONG64           EstablisherFrame;
        ULONG64           ImageBase;
        PRUNTIME_FUNCTION FunctionEntry;
        PVOID             HandlerData;

        RtlCaptureContext(pContextRecord);

        ControlPc     = pContextRecord->Rip;
        FunctionEntry = RtlLookupFunctionEntry(ControlPc, &ImageBase, NULL);

        if (FunctionEntry != NULL)
        {
            RtlVirtualUnwind(
                UNW_FLAG_NHANDLER,
                ImageBase,
                ControlPc,
                FunctionEntry,
                pContextRecord,
                &HandlerData,
                &EstablisherFrame,
                NULL);
        }
    }

    static void __cdecl capture_previous_context(CONTEXT* pContextRecord)
    {
        ULONG64           ControlPc;
        ULONG64           EstablisherFrame;
        ULONG64           ImageBase;
        PRUNTIME_FUNCTION FunctionEntry;
        PVOID             HandlerData;
        INT               frames;

        RtlCaptureContext(pContextRecord);

        ControlPc = pContextRecord->Rip;

        // Unwind "twice" to get the context of the caller to the "previous" caller:
        for (frames = 0; frames < 2; ++frames)
        {
            FunctionEntry = RtlLookupFunctionEntry(ControlPc, &ImageBase, NULL);

            if (FunctionEntry != NULL)
            {
                RtlVirtualUnwind(
                    UNW_FLAG_NHANDLER,
                    ImageBase,
                    ControlPc,
                    FunctionEntry,
                    pContextRecord,
                    &HandlerData,
                    &EstablisherFrame,
                    NULL);
            }
            else
            {
                break;
            }
        }
    }

#endif // !defined PROCESSOR_FAST_FAIL_AVAILABLE && defined _M_X64



// __raise_securityfailure() - Raises a security failure and terminates the process.
//
// Invokes the unhandled exception filter using the provided exception pointers and
// then terminates the process.
#if !defined PROCESSOR_FAST_FAIL_AVAILABLE

#if _CONTROL_FLOW_GUARD_SHADOW_STACK_SUPPORTED
    __declspec(guard(nosspro)) __declspec(guard(nossepi))
#endif
    __declspec(noreturn) void __cdecl __raise_securityfailure(
        PEXCEPTION_POINTERS const exception_pointers
        )
    {
        #ifdef _VCRT_BUILD
        DebuggerWasPresent = IsDebuggerPresent();
        _CRT_DEBUGGER_HOOK(_CRT_DEBUGGER_GSFAILURE);
        #endif // _VCRT_BUILD

        SetUnhandledExceptionFilter(NULL);
        UnhandledExceptionFilter(exception_pointers);

        #ifdef _VCRT_BUILD
        // If we make it back from Watson, then the user may have asked to debug
        // the app.  If we weren't under a debugger before invoking Watson,
        // re-signal the VS CRT debugger hook, so a newly attached debugger gets
        // a chance to break into the process.
        if (!DebuggerWasPresent)
        {
            _CRT_DEBUGGER_HOOK(_CRT_DEBUGGER_GSFAILURE);
        }
        #endif // _VCRT_BUILD

        TerminateProcess(GetCurrentProcess(), STATUS_SECURITY_CHECK_FAILURE);
    }

#endif // !defined PROCESSOR_FAST_FAIL_AVAILABLE



// Force an EBP frame on x86; no stack packing.  Forcing a frame applies to all
// of the functions below, including __report_gsfailure, __report_securityfailure,
// and __report_securityfailureEx
BEGIN_PRAGMA_OPTIMIZE_DISABLE("", DevDivVSO:162582, "Optimizations must be disabled to enable us to walk the stack")



// __report_gsfailure() - Report security error.
//
// A /GS security error has been detected.  We save the registers in a CONTEXT
// struct & call UnhandledExceptionFilter to display a message to the user
// (invoke Watson handling) and terminate the program.
//
// NOTE: __report_gsfailure should not be called directly.  Instead, it should
// be entered due to a failure detected by __security_check_cookie.   That's
// because __security_check_cookie may perform more checks than just a mismatch
// against the global security cookie, and because the context captured by
// __report_gsfailure is unwound assuming that __security_check_cookie was
// used to detect the failure.
//
// ULONGLONG StackCookie - the local cookie.  On x86, the local cookie is not
// passed in and is instead available in ECX, but since __report_gsfailure isn't
// __fastcall, it isn't a true argument, and we must flush ECX to the context
// record quickly.
#if defined _M_IX86
    #define GSFAILURE_PARAMETER
#elif defined _M_X64
    #define GSFAILURE_PARAMETER ULONGLONG stack_cookie
#elif defined _M_ARM || defined _M_ARM64
    #define GSFAILURE_PARAMETER uintptr_t stack_cookie
#else
    #error Unsupported architecture
#endif

#if defined PROCESSOR_FAST_FAIL_AVAILABLE

    #pragma warning(push)
    #pragma warning(disable: 4100) // unreferenced formal parameter
    __declspec(noreturn) void __cdecl __report_gsfailure(GSFAILURE_PARAMETER)
    {
        __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
    }
    #pragma warning(pop)

#else

#if _CONTROL_FLOW_GUARD_SHADOW_STACK_SUPPORTED
    __declspec(guard(nosspro)) __declspec(guard(nossepi))
#endif
    __declspec(noreturn) void __cdecl __report_gsfailure(GSFAILURE_PARAMETER)
    {
        if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
        {
            __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
        }

        volatile UINT_PTR cookie[2];

        // Set up a fake exception, and report it via UnhandledExceptionFilter.
        // We can't raise a true exception because the stack (and therefore
        // exception handling) can't be trusted after a buffer overrun.  The
        // exception should appear as if it originated after the call to
        // __security_check_cookie, so it is attributed to the function where the
        // buffer overrun was detected.

        #if defined _M_IX86
        // On x86, we reserve some extra stack which won't be used.  That is to
        // preserve as much of the call frame as possible when the function with
        // the buffer overrun entered __security_check_cookie with a JMP instead of
        // a CALL, after the calling frame has been released in the epilogue of
        // that function.
        ULONG volatile dw[(sizeof(CONTEXT) + sizeof(EXCEPTION_RECORD)) / sizeof(ULONG)];

        // Save the state in the context record immediately.  Hopefully, since
        // opts are disabled, this will happen without modifying ECX, which has
        // the local cookie which failed the check.
        __asm
        {
            mov dword ptr [GS_ContextRecord.Eax  ], eax
            mov dword ptr [GS_ContextRecord.Ecx  ], ecx
            mov dword ptr [GS_ContextRecord.Edx  ], edx
            mov dword ptr [GS_ContextRecord.Ebx  ], ebx
            mov dword ptr [GS_ContextRecord.Esi  ], esi
            mov dword ptr [GS_ContextRecord.Edi  ], edi
            mov word ptr  [GS_ContextRecord.SegSs], ss
            mov word ptr  [GS_ContextRecord.SegCs], cs
            mov word ptr  [GS_ContextRecord.SegDs], ds
            mov word ptr  [GS_ContextRecord.SegEs], es
            mov word ptr  [GS_ContextRecord.SegFs], fs
            mov word ptr  [GS_ContextRecord.SegGs], gs
            pushfd
            pop [GS_ContextRecord.EFlags]

            // Set the context EBP/EIP/ESP to the values which would be found
            // in the caller to __security_check_cookie.
            mov eax, [ebp]
            mov dword ptr [GS_ContextRecord.Ebp], eax
            mov eax, [ebp+4]
            mov dword ptr [GS_ContextRecord.Eip], eax
            lea eax, [ebp+8]
            mov dword ptr [GS_ContextRecord.Esp], eax

            // Make sure the dummy stack space looks referenced.
            mov eax, dword ptr dw
        }

        GS_ContextRecord.ContextFlags       = CONTEXT_CONTROL;
        GS_ExceptionRecord.ExceptionAddress = (PVOID)(ULONG_PTR)GS_ContextRecord.Eip;

        #else // ^^^ _M_IX86 ^^^ // vvv _M_X64 vvv //

        capture_previous_context(&GS_ContextRecord);
        GS_ContextRecord.Rip                = (ULONGLONG)_ReturnAddress();
        GS_ContextRecord.Rsp                = (ULONGLONG)_AddressOfReturnAddress()+8;
        GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Rip;
        GS_ContextRecord.Rcx                = stack_cookie;

        #endif // _M_X64

        GS_ExceptionRecord.ExceptionCode           = STATUS_SECURITY_CHECK_FAILURE;
        GS_ExceptionRecord.ExceptionFlags          = EXCEPTION_NONCONTINUABLE;
        GS_ExceptionRecord.NumberParameters        = 1;
        GS_ExceptionRecord.ExceptionInformation[0] = FAST_FAIL_STACK_COOKIE_CHECK_FAILURE;

        // Save the global cookie and cookie complement locally - using an array
        // to defeat any potential stack-packing.
        cookie[0] = __security_cookie;
        cookie[1] = __security_cookie_complement;

        // Raise the security failure by passing it to the unhandled exception
        // filter and then terminate the process.
        __raise_securityfailure((EXCEPTION_POINTERS*)&GS_ExceptionPointers);
    }

#endif



// Reports a specific security failure condition
//
// Reports a specific security failure condition.  The type of failure that
// occurred is embodied in the failure_code that is provided as a parameter.
// A specific failure condition can optionally specify additional parameters
// that should be recorded as part of the exception that is generated.
//
// NOTE: Unlike __report_gsfailure, __report_securityfailureEx assumes that
// it is called directly by the function in which the failure occurred.
#if defined PROCESSOR_FAST_FAIL_AVAILABLE

    __declspec(noreturn) void __cdecl __report_securityfailureEx(
        _In_                            ULONG  failure_code,
        _In_                            ULONG  parameter_count,
        _In_reads_opt_(parameter_count) void** parameters
        )
    {
        UNREFERENCED_PARAMETER(parameter_count);
        UNREFERENCED_PARAMETER(parameters);

        __fastfail(failure_code);
    }

#else

#if _CONTROL_FLOW_GUARD_SHADOW_STACK_SUPPORTED
    __declspec(guard(nosspro)) __declspec(guard(nossepi))
#endif
    __declspec(noreturn) void __cdecl __report_securityfailureEx(
        _In_                            ULONG  failure_code,
        _In_                            ULONG  parameter_count,
        _In_reads_opt_(parameter_count) void** parameters
        )
    {
        if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
        {
            __fastfail(failure_code);
        }

        // Set up a fake exception, and report it via UnhandledExceptionFilter.
        // We can't raise a true exception because the stack (and therefore
        // exception handling) can't be trusted.  The exception should appear as
        // if it originated after the call to __report_securityfailureEx, so it
        // is attributed to the function where the violation occurred.
        //
        // We assume that the immediate caller of __report_securityfailureEx is
        // the function where the security violation occurred.  Note that the
        // compiler may elect to emit a jump to this routine instead of a call,
        // in which case we will not be able to blame the correct function.
        #ifdef _M_IX86

        // On x86, we reserve some extra stack which won't be used.  That is to
        // preserve as much of the call frame as possible when the function with
        // the buffer overrun entered __security_check_cookie with a JMP instead
        // of a CALL, after the calling frame has been released in the epilogue
        // of that function.
        ULONG volatile dw[(sizeof(CONTEXT) + sizeof(EXCEPTION_RECORD)) / sizeof(ULONG)];

        // Save the state in the context record immediately.  Hopefully, since
        // opts are disabled, this will happen without modifying ECX, which has
        // the local cookie which failed the check.
        __asm
        {
            mov dword ptr [GS_ContextRecord.Eax  ], eax
            mov dword ptr [GS_ContextRecord.Ecx  ], ecx
            mov dword ptr [GS_ContextRecord.Edx  ], edx
            mov dword ptr [GS_ContextRecord.Ebx  ], ebx
            mov dword ptr [GS_ContextRecord.Esi  ], esi
            mov dword ptr [GS_ContextRecord.Edi  ], edi
            mov word ptr  [GS_ContextRecord.SegSs], ss
            mov word ptr  [GS_ContextRecord.SegCs], cs
            mov word ptr  [GS_ContextRecord.SegDs], ds
            mov word ptr  [GS_ContextRecord.SegEs], es
            mov word ptr  [GS_ContextRecord.SegFs], fs
            mov word ptr  [GS_ContextRecord.SegGs], gs
            pushfd
            pop [GS_ContextRecord.EFlags]

            // Set the context EBP/EIP/ESP to the values which would be found
            // in the caller to __security_check_cookie.
            mov eax, [ebp]
            mov dword ptr [GS_ContextRecord.Ebp], eax
            mov eax, [ebp + 4]
            mov dword ptr [GS_ContextRecord.Eip], eax
            lea eax, [ebp + 8]
            mov dword ptr [GS_ContextRecord.Esp], eax

            // Make sure the dummy stack space looks referenced.
            mov eax, dword ptr dw
        }

        GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Eip;

        #else // ^^^ _M_IX86 ^^^ // vvv _M_X64 vvv //

        capture_current_context(&GS_ContextRecord);
        GS_ContextRecord.Rip = (ULONGLONG) _ReturnAddress();
        GS_ContextRecord.Rsp = (ULONGLONG) _AddressOfReturnAddress()+8;
        GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Rip;

        #endif // _M_X64

        GS_ExceptionRecord.ExceptionCode  = STATUS_SECURITY_CHECK_FAILURE;
        GS_ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

        if (parameter_count > 0 && parameters == NULL)
        {
            parameter_count = 0;
        }

        if (parameter_count > EXCEPTION_MAXIMUM_PARAMETERS - 1)
        {
            parameter_count--;
        }

        GS_ExceptionRecord.NumberParameters        = parameter_count + 1;
        GS_ExceptionRecord.ExceptionInformation[0] = failure_code;

        for (ULONG i = 0; i < parameter_count; i++)
        {
            GS_ExceptionRecord.ExceptionInformation[i + 1] = (ULONG_PTR)parameters[i];
        }

        // Raise the security failure by passing it to the unhandled exception
        // filter and then terminate the process.
        __raise_securityfailure((EXCEPTION_POINTERS*)&GS_ExceptionPointers);
    }

#endif



// Reports a specific security failure condition.  The type of failure that
// occurred is embodied in the failure_code that is provided as a parameter.
// If a failure condition needs to specify additional parameters then it
// should call __report_securityfailureEx instead.
//
// NOTE: Unlike __report_gsfailure, __report_securityfailure assumes that it is
// called directly by the function in which the failure occurred.   This also
// means that __security_reportfailure doesn't simply call
//
//     __report_securityfailureEx(failure_code, 0, 0);
//
// as that would alter the call stack.
#if defined PROCESSOR_FAST_FAIL_AVAILABLE

    __declspec(noinline) void __cdecl __report_securityfailure(ULONG failure_code)
    {
        __fastfail(failure_code);
    }

#else

#if _CONTROL_FLOW_GUARD_SHADOW_STACK_SUPPORTED
    __declspec(guard(nosspro)) __declspec(guard(nossepi))
#endif
    __declspec(noinline) void __cdecl __report_securityfailure(ULONG failure_code)
    {
        if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
        {
            __fastfail(failure_code);
        }

        // Set up a fake exception, and report it via UnhandledExceptionFilter.
        // We can't raise a true exception because the stack (and therefore
        // exception handling) can't be trusted.  The exception should appear as
        // if it originated after the call to __report_securityfailure, so it
        // is attributed to the function where the violation occurred.
        //
        // We assume that the immediate caller of __report_securityfailure is
        // the function where the security violation occurred.  Note that the
        // compiler may elect to emit a jump to this routine instead of a call,
        // in which case we will not be able to blame the correct function.
        #if defined _M_IX86

        // On x86, we reserve some extra stack which won't be used.  That is to
        // preserve as much of the call frame as possible when the function with
        // the buffer overrun entered __security_check_cookie with a JMP instead
        // of a CALL, after the calling frame has been released in the epilogue
        // of that function.
        volatile ULONG dw[(sizeof(CONTEXT) + sizeof(EXCEPTION_RECORD))/sizeof(ULONG)];

        // Save the state in the context record immediately.  Hopefully, since
        // opts are disabled, this will happen without modifying ECX, which has
        // the local cookie which failed the check.
        __asm
        {
            mov dword ptr [GS_ContextRecord.Eax  ], eax
            mov dword ptr [GS_ContextRecord.Ecx  ], ecx
            mov dword ptr [GS_ContextRecord.Edx  ], edx
            mov dword ptr [GS_ContextRecord.Ebx  ], ebx
            mov dword ptr [GS_ContextRecord.Esi  ], esi
            mov dword ptr [GS_ContextRecord.Edi  ], edi
            mov word ptr  [GS_ContextRecord.SegSs], ss
            mov word ptr  [GS_ContextRecord.SegCs], cs
            mov word ptr  [GS_ContextRecord.SegDs], ds
            mov word ptr  [GS_ContextRecord.SegEs], es
            mov word ptr  [GS_ContextRecord.SegFs], fs
            mov word ptr  [GS_ContextRecord.SegGs], gs
            pushfd
            pop [GS_ContextRecord.EFlags]

            // Set the context EBP/EIP/ESP to the values which would be found
            // in the caller to __security_check_cookie.
            mov eax, [ebp]
            mov dword ptr [GS_ContextRecord.Ebp], eax
            mov eax, [ebp + 4]
            mov dword ptr [GS_ContextRecord.Eip], eax
            lea eax, [ebp + 8]
            mov dword ptr [GS_ContextRecord.Esp], eax

            // Make sure the dummy stack space looks referenced.
            mov eax, dword ptr dw
        }

        GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Eip;

        #else // ^^^ _M_IX86 ^^^ // vvv _M_X64 vvv //

        capture_current_context(&GS_ContextRecord);
        GS_ContextRecord.Rip = (ULONGLONG) _ReturnAddress();
        GS_ContextRecord.Rsp = (ULONGLONG) _AddressOfReturnAddress()+8;
        GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Rip;

        #endif // _M_X64

        GS_ExceptionRecord.ExceptionCode  = STATUS_SECURITY_CHECK_FAILURE;
        GS_ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

        GS_ExceptionRecord.NumberParameters = 1;
        GS_ExceptionRecord.ExceptionInformation[0] = failure_code;

        /*
         * Raise the security failure by passing it to the unhandled exception
         * filter and then terminate the process.
         */

        __raise_securityfailure((EXCEPTION_POINTERS*)&GS_ExceptionPointers);
    }

#endif



// Declare stub for rangecheckfailure, since these occur often enough that the
// code bloat of setting up the parameters hurts performance
#if _CONTROL_FLOW_GUARD_SHADOW_STACK_SUPPORTED
__declspec(guard(nosspro)) __declspec(guard(nossepi))
#endif
__declspec(noreturn) void __cdecl __report_rangecheckfailure()
{
    __report_securityfailure(FAST_FAIL_RANGE_CHECK_FAILURE);
}
