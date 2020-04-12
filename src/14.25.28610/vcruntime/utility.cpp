//
// utility.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Common functionality shared by both the EXE and DLL startup code.
//
#include <vcstartup_internal.h>
#include <limits.h>



extern "C" unsigned int                __scrt_native_dllmain_reason         = NATIVE_DLLMAIN_REASON_NO_REASON;
extern "C" __scrt_native_startup_state __scrt_current_native_startup_state  = __scrt_native_startup_state::uninitialized;
extern "C" void*                       __scrt_native_startup_lock           = nullptr;



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// PE Image Utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// Tests whether a PE image is located at the given image base.  Returns true if
// the given image base potentially points to a loaded PE image; false otherwise.
static bool __cdecl is_potentially_valid_image_base(void* const image_base) noexcept
{
    if (!image_base)
    {
        return false;
    }

    auto const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return false;
    }

    auto const nt_header_address = reinterpret_cast<PBYTE>(dos_header) + dos_header->e_lfanew;
    auto const nt_header         = reinterpret_cast<PIMAGE_NT_HEADERS>(nt_header_address);
    if (nt_header->Signature != IMAGE_NT_SIGNATURE)
    {
        return false;
    }

    auto const optional_header = &nt_header->OptionalHeader;
    if (optional_header->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
    {
        return false;
    }

    return true;
}



// Given an RVA, finds the PE section in the pointed-to image that includes the
// RVA.  Returns null if no such section exists or the section is not found.
static PIMAGE_SECTION_HEADER __cdecl find_pe_section(unsigned char* const image_base, uintptr_t const rva) noexcept
{
    auto const dos_header        = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);
    auto const nt_header_address = reinterpret_cast<PBYTE>(dos_header) + dos_header->e_lfanew;
    auto const nt_header         = reinterpret_cast<PIMAGE_NT_HEADERS>(nt_header_address);

    // Find the section holding the RVA.  We make no assumptions here about the
    // sort order of the section descriptors, though they always appear to be
    // sorted by ascending section RVA.
    PIMAGE_SECTION_HEADER const first_section = IMAGE_FIRST_SECTION(nt_header);
    PIMAGE_SECTION_HEADER const last_section  = first_section + nt_header->FileHeader.NumberOfSections;
    for (auto it = first_section; it != last_section; ++it)
    {
        if (rva >= it->VirtualAddress && rva < it->VirtualAddress + it->Misc.VirtualSize)
        {
            return it;
        }
    }

    return nullptr;
}


// Tests whether a target address is located within the current PE image (the
// image located at __ImageBase), that the target address is located in a proper
// section of the image, and that the section in which it is located is not
// writable.
extern "C" bool __cdecl __scrt_is_nonwritable_in_current_image(void const* const target)
{
    auto const target_address = reinterpret_cast<unsigned char const*>(target);
    auto const image_base     = reinterpret_cast<unsigned char*>(&__ImageBase);

    __try
    {
        // Make sure __ImageBase is the address of a valid PE image.  This is
        // likely an unnecessary check, since we should be executing in a
        // normal image, but it is fast, this routine is rarely called, and the
        // normal call is for security purposes.  If we don't have a PE image,
        // return failure:
        if (!is_potentially_valid_image_base(image_base))
        {
            return false;
        }

        // Convert the target address to an RVA within the image and find the
        // corresponding PE section.  Return failure if the target address is
        // not found within the current image:
        uintptr_t const rva_target = target_address - image_base;
        PIMAGE_SECTION_HEADER const section_header = find_pe_section(image_base, rva_target);
        if (!section_header)
        {
            return false;
        }

        // Check the section characteristics to see if the target address is
        // located within a writable section, returning a failure if it is:
        if (section_header->Characteristics & IMAGE_SCN_MEM_WRITE)
        {
            return false;
        }

        return true;
    }
    __except (GetExceptionCode() == STATUS_ACCESS_VIOLATION)
    {
        // If any of the above operations failed, assume that we do not have a
        // valid nonwritable address in the current image:
        return false;
    }
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Startup Synchronization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" bool __cdecl __scrt_acquire_startup_lock()
{
    if (!__scrt_is_ucrt_dll_in_use())
    {
        // No synchronization is required when the static CRT is in use.
        return false;
    }

    void* const this_fiber = reinterpret_cast<PNT_TIB>(NtCurrentTeb())->StackBase;

    while (void* const owning_fiber = _InterlockedCompareExchangePointer(&__scrt_native_startup_lock, this_fiber, nullptr))
    {
        if (this_fiber == owning_fiber)
        {
            return true;
        }
    }

    return false;
}

extern "C" void __cdecl __scrt_release_startup_lock(bool const is_nested)
{
    if (!__scrt_is_ucrt_dll_in_use())
    {
        // No synchronization is required when the static CRT is in use.
        return;
    }

    if (is_nested)
    {
        return;
    }

    _InterlockedExchangePointer(&__scrt_native_startup_lock, nullptr);
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CRT Initialization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
static bool is_initialized_as_dll;

extern "C" bool __cdecl __scrt_initialize_crt(__scrt_module_type const module_type)
{
    if (module_type == __scrt_module_type::dll)
    {
        is_initialized_as_dll = true;
    }

    __isa_available_init();

    // Notify the CRT components of the process attach, bottom-to-top:
    if (!__vcrt_initialize())
    {
        return false;
    }

    if (!__acrt_initialize())
    {
        __vcrt_uninitialize(false);
        return false;
    }

    return true;
}

extern "C" bool __cdecl __scrt_uninitialize_crt(bool const is_terminating, bool const from_exit)
{
    // If the CRT is statically linked into a DLL and that DLL calls exit(), we
    // do not uninitialize the statically-linked CRT.  Instead, we wait for the
    // DLL_PROCESS_DETACH notification that we will receive after ExitProcess()
    // is called by exit().
    if (is_initialized_as_dll && from_exit)
    {
        return true;
    }

    // Notify the CRT components of the process detach, top-to-bottom:
    __acrt_uninitialize(is_terminating);
    __vcrt_uninitialize(is_terminating);

    return true;
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// On-Exit Table
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// When a module uses the Universal CRT DLL, the behavior of _onexit(), atexit(),
// and at_quick_exit() is different depending on whether the module is an EXE or
// a DLL.  If it is an EXE, then calls to these functions are passed to the
// Universal CRT DLL and the callbacks are registered with its atexit function
// tables.  This way, functions registered by the EXE are called whenever one of
// the exit() functions is called.
//
// If the module is a DLL, it has its own table of registered functions.  This
// table is executed when the DLL is unloaded (during DLL_PROCESS_DETACH).
//
// Here, we define the module-local function tables used by DLLs that use the
// Universal CRT DLL.  If this module is an EXE or if the Universal CRT is
// statically linked into this module, then these tables will have pointers
// initialized to -1.
//
// The user-callable _onexit(), atexit(), and at_quick_exit() functions then
// either use the module-local function tables or will forward calls to the
// Universal CRT DLL, as appropriate.
static _onexit_table_t module_local_atexit_table{};
static _onexit_table_t module_local_at_quick_exit_table{};
static bool module_local_atexit_table_initialized = false;

extern "C" _onexit_t __cdecl _onexit(_onexit_t const function)
{
    _PVFV* const onexit_first = module_local_atexit_table._first;

    if (onexit_first == reinterpret_cast<_PVFV*>(-1))
    {
        return _crt_atexit(reinterpret_cast<_PVFV>(function)) == 0
            ? function
            : nullptr;
    }
    else
    {
        return _register_onexit_function(&module_local_atexit_table, function) == 0
            ? function
            : nullptr;
    }
}

extern "C" int __cdecl atexit(_PVFV const function)
{
    return _onexit(reinterpret_cast<_onexit_t>(function)) != nullptr
        ? 0
        : -1;
}

extern "C" int __cdecl at_quick_exit(_PVFV const function)
{
    _PVFV* const onexit_first = module_local_at_quick_exit_table._first;

    if (onexit_first == reinterpret_cast<_PVFV*>(-1))
    {
        return _crt_at_quick_exit(function);
    }
    else
    {
        _onexit_t const onexit_function = reinterpret_cast<_onexit_t>(function);
        return _register_onexit_function(&module_local_at_quick_exit_table, onexit_function);
    }
}

extern "C" bool __cdecl __scrt_initialize_onexit_tables(__scrt_module_type const module_type)
{
    if (module_local_atexit_table_initialized)
    {
        return true;
    }

    if (module_type != __scrt_module_type::dll && module_type != __scrt_module_type::exe)
    {
        __scrt_fastfail(FAST_FAIL_INVALID_ARG);
    }

    if (__scrt_is_ucrt_dll_in_use() && module_type == __scrt_module_type::dll)
    {
        // If this module is a DLL, we initialize our module-local onexit table:
        if (_initialize_onexit_table(&module_local_atexit_table) != 0)
        {
            return false;
        }

        if (_initialize_onexit_table(&module_local_at_quick_exit_table) != 0)
        {
            return false;
        }
    }
    else
    {
        // Initialize the onexit table pointers to indicate that either [1] this
        // module has statically-linked the Universal CRT or [2] this module is
        // a DLL that uses the Universal CRT DLL.
        _PVFV* const invalid = reinterpret_cast<_PVFV*>(-1);

        module_local_atexit_table        = { invalid, invalid, invalid };
        module_local_at_quick_exit_table = { invalid, invalid, invalid };
    }

    module_local_atexit_table_initialized = true;
    return true;
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// DLL Functionality
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" int __cdecl __scrt_dllmain_exception_filter(
    HINSTANCE           const instance,
    DWORD               const reason,
    LPVOID              const reserved,
    __scrt_dllmain_type const crt_dllmain,
    unsigned long       const exception_code_,
    PEXCEPTION_POINTERS const exception_info_
    )
{
    if (!__scrt_is_ucrt_dll_in_use() && reason == DLL_PROCESS_ATTACH)
    {
        // The client DllMain routine failed with an unhandled exception, so the
        // in-module static CRT needs to be cleaned up.  We do this by calling
        // the CRT DllMain equivalent with DLL_PROCESS_DETACH.  Note that we do
        // not call the client DllMain again because the DLL has not completed
        // loading, and we do not know what its state is due to the unhandled
        // exception.
        crt_dllmain(instance, DLL_PROCESS_DETACH, reserved);
    }

    return _seh_filter_dll(exception_code_, exception_info_);
}

extern "C" bool __cdecl __scrt_dllmain_before_initialize_c()
{
    if (!__scrt_initialize_onexit_tables(__scrt_module_type::dll))
    {
        return false;
    }

    return true;
}

extern "C" bool __cdecl __scrt_dllmain_after_initialize_c()
{
    if (__scrt_is_ucrt_dll_in_use())
    {
        __isa_available_init();
    }
    else
    {
        if (__scrt_dll_argv_policy::configure_argv() != 0)
        {
            return false;
        }

        __scrt_dll_environment_policy::initialize_environment();
    }

    return true;
}

extern "C" void __cdecl __scrt_dllmain_uninitialize_c()
{
    if (__scrt_is_ucrt_dll_in_use())
    {
        _execute_onexit_table(&module_local_atexit_table);
    }
    else
    {
        if (!_is_c_termination_complete())
        {
            _cexit();
        }
    }
}

extern "C" void __cdecl __scrt_dllmain_uninitialize_critical()
{
    // The critical uninitialization code should be unnecessary.  When an
    // exception escapes from DllMain during DLL_PROCESS_DETACH, the OS
    // should terminate the process.  Unfortunately, through Windows 7, the
    // exception is silently swallowed.
    //
    // Therefore, we uninitialize the PTD during phase two unwind.  This
    // ensures that our FLS callback is unregistered (we must unregister it
    // so that it is not left pointing to code in the CRT module, which is
    // about to be unloaded).
    //
    // Note that the CRT is in one of three possible states here:
    // (1) Uninitialization completed successfully, so the PTD has already
    //     been uninitialized.  In this case, the call here has no effect.
    // (2) Uninitialization failed, but after the PTD was uninitialized.
    //     Again, in this case, the call here has no effect.
    // (3) Uninitialization failed, before the PTD was uninitialized.  In
    //     this case, the call here will uninitialize the PTD and remove
    //     the FLS callback.
    __acrt_uninitialize_critical(false);
    __vcrt_uninitialize_critical();
}

extern "C" bool __cdecl __scrt_dllmain_crt_thread_attach()
{
    // Notify the CRT components of the thread attach, bottom-to-top:
    if (!__vcrt_thread_attach())
    {
        return false;
    }

    if (!__acrt_thread_attach())
    {
        __vcrt_thread_detach();
        return false;
    }

    return true;
}

extern "C" bool __cdecl __scrt_dllmain_crt_thread_detach()
{
    // Notify the CRT components of the thread detach, top-to-bottom:
    __acrt_thread_detach();
    __vcrt_thread_detach();
    return true;
}
