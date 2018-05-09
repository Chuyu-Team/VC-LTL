//
// invalid_parameter.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The invalid parameter handlers and related functionality
//
#include <corecrt_internal.h>
#include <msvcrt_IAT.h>



static _invalid_parameter_handler __acrt_invalid_parameter_handler = nullptr;

static thread_local _invalid_parameter_handler _thread_local_iph=nullptr;


#if defined _M_X64 && !defined _UCRT_ENCLAVE_BUILD

    static void __cdecl capture_current_context(CONTEXT* const context_record) throw()
    {
        ULONG64 ControlPc;
        ULONG64 EstablisherFrame;
        ULONG64 ImageBase;
        PRUNTIME_FUNCTION FunctionEntry;
        PVOID HandlerData;

        RtlCaptureContext(context_record);

        ControlPc = context_record->Rip;
        FunctionEntry = RtlLookupFunctionEntry(ControlPc, &ImageBase, nullptr);

        if (FunctionEntry != nullptr)
        {
            RtlVirtualUnwind(
                UNW_FLAG_NHANDLER,
                ImageBase,
                ControlPc,
                FunctionEntry,
                context_record,
                &HandlerData,
                &EstablisherFrame,
                nullptr);
        }
    }

#endif // _M_X64 && !_UCRT_ENCLAVE_BUILD

#if defined _CRT_GLOBAL_STATE_ISOLATION

    // The legacy Windows CRT (msvcrt) does not terminate the process when an
    // invalid parameter is passed to a library function.  Turning on this
    // validation for Windows components would be a big app compat problem.
    //
    // For OS components the default behavior will be to ignore the invalid parameters
    // and that is accomplished by providing an empty IPH.
    static void __cdecl invalid_parameter_handler_continue(
        wchar_t const * const,    // pszExpression
        wchar_t const * const,    // pszFunction
        wchar_t const * const,    // pszFile
        unsigned int,              // nLine
        uintptr_t                   // pReserved
    ) throw()
    {
    }

#endif

/*extern "C" void __cdecl __acrt_initialize_invalid_parameter_handler(void* const encoded_null)
{
#if defined _CRT_GLOBAL_STATE_ISOLATION
    const _invalid_parameter_handler encoded_os_iph = __crt_fast_encode_pointer(invalid_parameter_handler_continue);
#endif
    const _invalid_parameter_handler iph[] =
    {
        reinterpret_cast<_invalid_parameter_handler>(encoded_null)
#if defined _CRT_GLOBAL_STATE_ISOLATION
        ,encoded_os_iph
#endif
    };

    __acrt_invalid_parameter_handler.initialize_from_array(iph);
}*/



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// _invalid_parameter
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" void __cdecl _invalid_parameter_advanced(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved
    )
{
    if (_thread_local_iph)
    {
        _thread_local_iph(expression, function_name, file_name, line_number, reserved);
        return;
    }

    auto& global_handler = __acrt_invalid_parameter_handler;
    if (global_handler)
    {
        global_handler(expression, function_name, file_name, line_number, reserved);
        return;
    }

    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

_LCRT_DEFINE_IAT_SYMBOL(_invalid_parameter_advanced);

EXTERN_C _ACRTIMP void __cdecl _invalid_parameter(
        _In_opt_z_ wchar_t const*,
        _In_opt_z_ wchar_t const*,
        _In_opt_z_ wchar_t const*,
        _In_       unsigned int,
        _In_       uintptr_t
        );

extern "C" void __cdecl _invalid_parameter_noinfo_downlevel()
{
    _invalid_parameter(nullptr, nullptr, nullptr, 0, 0);
}

_LCRT_DEFINE_IAT_SYMBOL(_invalid_parameter_noinfo_downlevel);


// This is used by inline code in the C++ Standard Library and the SafeInt
// library.  Because it is __declspec(noreturn), the compiler can better
// optimize use of the invalid parameter handler for inline code.
extern "C" __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn_downlevel()
{
    _invalid_parameter(nullptr, nullptr, nullptr, 0, 0);
    _invoke_watson    (nullptr, nullptr, nullptr, 0, 0);
}

_LCRT_DEFINE_IAT_SYMBOL(_invalid_parameter_noinfo_noreturn_downlevel);


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// _invoke_watson
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if (defined _M_IX86 || defined _M_X64) && !defined _UCRT_ENCLAVE_BUILD

    extern "C" void __cdecl __acrt_call_reportfault(
        int   const debugger_hook_code,
        DWORD const _exception_code,
        DWORD const _exception_flags
        )
    {
        // Notify the debugger if attached.
        if (debugger_hook_code != _CRT_DEBUGGER_IGNORE)
        {
            _CRT_DEBUGGER_HOOK(debugger_hook_code);
        }

        // Fake an exception to call report fault:
        EXCEPTION_RECORD   ExceptionRecord{};
        CONTEXT            ContextRecord{};
        EXCEPTION_POINTERS ExceptionPointers = {&ExceptionRecord, &ContextRecord};

        #ifdef _M_IX86

        __asm
        {
            mov dword ptr [ContextRecord.Eax  ], eax
            mov dword ptr [ContextRecord.Ecx  ], ecx
            mov dword ptr [ContextRecord.Edx  ], edx
            mov dword ptr [ContextRecord.Ebx  ], ebx
            mov dword ptr [ContextRecord.Esi  ], esi
            mov dword ptr [ContextRecord.Edi  ], edi
            mov word ptr  [ContextRecord.SegSs], ss
            mov word ptr  [ContextRecord.SegCs], cs
            mov word ptr  [ContextRecord.SegDs], ds
            mov word ptr  [ContextRecord.SegEs], es
            mov word ptr  [ContextRecord.SegFs], fs
            mov word ptr  [ContextRecord.SegGs], gs
            pushfd
            pop [ContextRecord.EFlags]
        }

        ContextRecord.ContextFlags = CONTEXT_CONTROL;

        ContextRecord.Eip = (ULONG)_ReturnAddress();
        ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
        ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);

        #else // ^^^ _M_IX86 ^^^ // vvv _M_X64 vvv //

        capture_current_context(&ContextRecord);
        ContextRecord.Rip = (ULONGLONG)_ReturnAddress();
        ContextRecord.Rsp = (ULONGLONG)_AddressOfReturnAddress()+8;

        #endif // _M_X64

        ExceptionRecord.ExceptionCode    = _exception_code;
        ExceptionRecord.ExceptionFlags   = _exception_flags;
        ExceptionRecord.ExceptionAddress = _ReturnAddress();

        BOOL const was_debugger_present = IsDebuggerPresent();

        // Raises an exception that bypasses all exception handlers:
#pragma warning(suppress:__WARNING_SETUNHANDLEDEXCEPTIONFILTER_USE) // 28725 Use Watson instead of this SetUnhandledExceptionFilter.
        SetUnhandledExceptionFilter(nullptr);
        DWORD const handler_result = UnhandledExceptionFilter(&ExceptionPointers);

        // If no handler was found and no debugger was previously attached, then
        // execution must stop into the debugger hook:
        if (handler_result == EXCEPTION_CONTINUE_SEARCH && !was_debugger_present && debugger_hook_code != _CRT_DEBUGGER_IGNORE)
        {
            _CRT_DEBUGGER_HOOK(debugger_hook_code);
        }
    }

    extern "C" __declspec(noreturn) void __cdecl _invoke_watson_downlevel(
        wchar_t const* const expression,
        wchar_t const* const function_name,
        wchar_t const* const file_name,
        unsigned int   const line_number,
        uintptr_t      const reserved
        )
    {
        UNREFERENCED_PARAMETER(expression   );
        UNREFERENCED_PARAMETER(function_name);
        UNREFERENCED_PARAMETER(file_name    );
        UNREFERENCED_PARAMETER(line_number  );
        UNREFERENCED_PARAMETER(reserved     );

        if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
        {
            __fastfail(FAST_FAIL_INVALID_ARG);
        }

        // Otherwise, raise a fast-fail exception and termintae the process:
        __acrt_call_reportfault(
            _CRT_DEBUGGER_INVALIDPARAMETER,
            STATUS_INVALID_CRUNTIME_PARAMETER,
            EXCEPTION_NONCONTINUABLE);

        TerminateProcess(GetCurrentProcess(), STATUS_INVALID_CRUNTIME_PARAMETER);
    }

#else // ^^^ (_M_IX86 || _M_X64) && !_UCRT_ENCLAVE_BUILD ^^^ // vvv Newer Architectures vvv //

    extern "C" __declspec(noreturn) void __cdecl _invoke_watson_downlevel(
        wchar_t const* const expression,
        wchar_t const* const function_name,
        wchar_t const* const file_name,
        unsigned int   const line_number,
        uintptr_t      const reserved
        )
    {
        UNREFERENCED_PARAMETER(expression   );
        UNREFERENCED_PARAMETER(function_name);
        UNREFERENCED_PARAMETER(file_name    );
        UNREFERENCED_PARAMETER(line_number  );
        UNREFERENCED_PARAMETER(reserved     );

        __fastfail(FAST_FAIL_INVALID_ARG);
    }

#endif

	_LCRT_DEFINE_IAT_SYMBOL(_invoke_watson_downlevel);


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Handler Accessors
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" _invalid_parameter_handler __cdecl _set_invalid_parameter_handler_downlevel(_invalid_parameter_handler const new_handler)
{
	return __crt_interlocked_exchange_pointer(&__acrt_invalid_parameter_handler, new_handler);
}

_LCRT_DEFINE_IAT_SYMBOL(_set_invalid_parameter_handler_downlevel);

extern "C" _invalid_parameter_handler __cdecl _get_invalid_parameter_handler_downlevel()
{
    return __acrt_invalid_parameter_handler;
}

_LCRT_DEFINE_IAT_SYMBOL(_get_invalid_parameter_handler_downlevel);


extern "C" _invalid_parameter_handler __cdecl _set_thread_local_invalid_parameter_handler_downlevel(_invalid_parameter_handler const new_handler)
{
    _invalid_parameter_handler const old_handler = _thread_local_iph;
    _thread_local_iph = new_handler;
    return old_handler;
}

_LCRT_DEFINE_IAT_SYMBOL(_set_thread_local_invalid_parameter_handler_downlevel);


extern "C" _invalid_parameter_handler __cdecl _get_thread_local_invalid_parameter_handler_downlevel()
{
    return _thread_local_iph;
}

_LCRT_DEFINE_IAT_SYMBOL(_get_thread_local_invalid_parameter_handler_downlevel);
