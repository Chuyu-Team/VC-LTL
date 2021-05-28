//
// utility_desktop.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// CRT startup and termination functionality specific to use of the CRT in a
// Desktop app.
//
#include <vcstartup_internal.h>
#include <eh.h>
#include <ehdata.h>
#include <intrin.h>
#include <trnsctrl.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Startup Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" WORD __cdecl __scrt_get_show_window_mode()
{
    STARTUPINFOW startup_info{};
    GetStartupInfoW(&startup_info);

    return startup_info.dwFlags & STARTF_USESHOWWINDOW
        ? startup_info.wShowWindow
        : SW_SHOWDEFAULT;
}

extern "C" bool __cdecl __scrt_is_managed_app()
{
    PIMAGE_DOS_HEADER const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(GetModuleHandleW(nullptr));
    if (dos_header == nullptr)
        return false;

    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    PIMAGE_NT_HEADERS const pe_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<BYTE*>(dos_header) + dos_header->e_lfanew);

    if (pe_header->Signature != IMAGE_NT_SIGNATURE)
        return false;

    if (pe_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
        return false;

    // prefast assumes we are overrunning __ImageBase
    #pragma warning(push)
    #pragma warning(disable: 26000)

    if (pe_header->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR)
        return false;

    #pragma warning(pop)

    if (pe_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress == 0)
        return false;

    return true;
}

extern "C" int __cdecl __scrt_initialize_winrt()
{
    // We never initialize WinRT in a Desktop app:
    return 0;
}

// If exe_initialize_mta.lib doesn't get linked in, then the "real" function won't be seen, so all we do is call the no-op stub.
extern "C" int __cdecl __scrt_exe_initialize_mta();

extern "C" int __cdecl __scrt_stub_for_initialize_mta()
{
    // By default, we don't initialize a COM MTA
    return 0;
}

_VCRT_DECLARE_ALTERNATE_NAME(__scrt_exe_initialize_mta, __scrt_stub_for_initialize_mta)

extern "C" int __cdecl __scrt_initialize_mta()
{
    return __scrt_exe_initialize_mta();
}

extern "C" LONG WINAPI __scrt_unhandled_exception_filter(LPEXCEPTION_POINTERS const pointers)
{
    auto const exception_record = reinterpret_cast<EHExceptionRecord*>(pointers->ExceptionRecord);
    if (PER_IS_MSVC_PURE_OR_NATIVE_EH(exception_record))
    {
        _pCurrentException = reinterpret_cast<EHExceptionRecord*>(exception_record);
        _pCurrentExContext = pointers->ContextRecord;
        terminate();
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

extern "C" void __cdecl __scrt_set_unhandled_exception_filter()
{
    SetUnhandledExceptionFilter(__scrt_unhandled_exception_filter);
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Fatal Error Reporting
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// There are parts of the runtime startup process that absolutely must succeed.
// If an error occurs during these parts of the startup process, we terminate
// the process.  In a Windows Store app, we can just invoke __fastfail.  In a
// Desktop app, we may be executing on a machine that does not support the
// __fastfail intrinsic.  If we cannot __fastfail, we issue a noncontinuable
// exception directly to the unhandled exception filter, which will result in
// the process being terminated.
//
// This functionality is equivalent to the fault-reporting logic in the AppCRT.
// We cannot make use of the AppCRT fault-reporting logic here, because we may
// encounter a fatal error before we have fully initialized the AppCRT.

#if defined(_M_IX86) || defined(_M_X64)

extern "C" int __scrt_debugger_hook_flag = 0;

extern "C" __declspec(noinline) void __cdecl _CRT_DEBUGGER_HOOK(int const reserved)
{
    UNREFERENCED_PARAMETER(reserved);

    // We assign zero to the debugger hook flag so that this function is not
    // folded when optimized.  The flag is not otherwise used.
    __scrt_debugger_hook_flag = 0;
}

#endif

#if (!defined _M_IX86 || defined _M_HYBRID) && (!defined _M_X64 || defined _M_ARM64EC)

    extern "C" void __cdecl __scrt_fastfail(unsigned const code)
    {
        // The __fastfail intrinsic is always available outside of x86 and x64
        __fastfail(code);
    }

#else // x86 or x64 (no ARM64 hybrid architectures)

    extern "C" void __cdecl __scrt_fastfail(unsigned const code)
    {
        // First see if __fastfail is available, and invoke it if it is.  This will
        // always be availbale on ARM and is always available on Windows 8 and above.
        if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
            __fastfail(code);

        // If __fastfail is not available, fall back to raising an exception that
        // bypasses all exception handlers (via a call to the unhandled exception
        // filter.

        // Notify the debugger if attached.
        _CRT_DEBUGGER_HOOK(_CRT_DEBUGGER_ABORT);

        CONTEXT context_record = { };

        #if defined _M_IX86

        __asm
        {
            mov dword ptr [context_record.Eax],   eax
            mov dword ptr [context_record.Ecx],   ecx
            mov dword ptr [context_record.Edx],   edx
            mov dword ptr [context_record.Ebx],   ebx
            mov dword ptr [context_record.Esi],   esi
            mov dword ptr [context_record.Edi],   edi
            mov word ptr  [context_record.SegSs], ss
            mov word ptr  [context_record.SegCs], cs
            mov word ptr  [context_record.SegDs], ds
            mov word ptr  [context_record.SegEs], es
            mov word ptr  [context_record.SegFs], fs
            mov word ptr  [context_record.SegGs], gs
            pushfd
            pop [context_record.EFlags]
        }

        context_record.ContextFlags = CONTEXT_CONTROL;
        context_record.Eip = reinterpret_cast<ULONG>(_ReturnAddress());
        context_record.Esp = reinterpret_cast<ULONG>(_AddressOfReturnAddress());
        context_record.Ebp = *(reinterpret_cast<ULONG*>(_AddressOfReturnAddress()) - 1);

        #elif defined _M_X64

        RtlCaptureContext(&context_record);

        ULONG64 const control_pc = context_record.Rip;

        ULONG64 image_base;
        PRUNTIME_FUNCTION const function_entry = RtlLookupFunctionEntry(control_pc, &image_base, nullptr);

        if (function_entry)
        {
            ULONG64 establisher_frame;
            PVOID   handler_data;
            RtlVirtualUnwind(
                UNW_FLAG_NHANDLER,
                image_base,
                control_pc,
                function_entry,
                &context_record,
                &handler_data,
                &establisher_frame,
                nullptr);
        }

        context_record.Rip = reinterpret_cast<ULONGLONG>(_ReturnAddress());
        context_record.Rsp = reinterpret_cast<ULONGLONG>(_AddressOfReturnAddress()) + 8;

        #endif

        EXCEPTION_RECORD exception_record = { };
        exception_record.ExceptionCode    = STATUS_FATAL_APP_EXIT;
        exception_record.ExceptionFlags   = EXCEPTION_NONCONTINUABLE;
        exception_record.ExceptionAddress = _ReturnAddress();

        bool const was_debugger_present = IsDebuggerPresent() == TRUE;

        // Raise an exception that bypasses all exception handlers:
        EXCEPTION_POINTERS exception_pointers = { &exception_record, &context_record };

        SetUnhandledExceptionFilter(nullptr);
        LONG const result = UnhandledExceptionFilter(&exception_pointers);

        // If no handler was found and no debugger was previously attached, then make
        // sure we notify the debugger.
        if (result == EXCEPTION_CONTINUE_SEARCH && !was_debugger_present)
            _CRT_DEBUGGER_HOOK(_CRT_DEBUGGER_ABORT);
    }

#endif // x86 or x64
