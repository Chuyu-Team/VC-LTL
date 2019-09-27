//
// winapi_downlevel.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of wrappers for Windows API functions that cannot be called
// directly because they are not available on all supported operating systems.
//
#include <vcruntime_internal.h>

#if _VCRT_WIN32_WINNT < _WIN32_WINNT_VISTA

    #define APPLY_TO_LATE_BOUND_MODULES(APPLY)                                \
        APPLY(api_ms_win_core_fibers_l1_1_1, "api-ms-win-core-fibers-l1-1-1") \
        APPLY(api_ms_win_core_synch_l1_2_0,  "api-ms-win-core-synch-l1-2-0" ) \
        APPLY(kernel32,                      "kernel32"                     )



    #define APPLY_TO_LATE_BOUND_FUNCTIONS(APPLY)                                          \
        APPLY(FlsAlloc,                    ({ api_ms_win_core_fibers_l1_1_1, kernel32 })) \
        APPLY(FlsFree,                     ({ api_ms_win_core_fibers_l1_1_1, kernel32 })) \
        APPLY(FlsGetValue,                 ({ api_ms_win_core_fibers_l1_1_1, kernel32 })) \
        APPLY(FlsSetValue,                 ({ api_ms_win_core_fibers_l1_1_1, kernel32 })) \
        APPLY(InitializeCriticalSectionEx, ({ api_ms_win_core_synch_l1_2_0,  kernel32 }))


    namespace
    {
        // Generate enumerators for each of the modules:
        enum module_id : unsigned
        {
            #define APPLY(_SYMBOL, _NAME) _SYMBOL,
            APPLY_TO_LATE_BOUND_MODULES(APPLY)
            #undef APPLY

            module_id_count
        };

        // Generate a table of module names that can be indexed by the module_id
        // enumerators:
        static wchar_t const* const module_names[module_id_count] =
        {
            #define APPLY(_SYMBOL, _NAME) _CRT_WIDE(_NAME),
            APPLY_TO_LATE_BOUND_MODULES(APPLY)
            #undef APPLY
        };

        // Generate enumerators for each of the functions:
        enum function_id : unsigned
        {
            #define APPLY(_FUNCTION, _MODULES) _CRT_CONCATENATE(_FUNCTION, _id),
            APPLY_TO_LATE_BOUND_FUNCTIONS(APPLY)
            #undef APPLY

            function_id_count
        };

        // Generate a typedef for each function of the form function_pft.
        #define APPLY(_FUNCTION, _MODULES) \
            using _CRT_CONCATENATE(_FUNCTION, _pft) = decltype(_FUNCTION)*;
        APPLY_TO_LATE_BOUND_FUNCTIONS(APPLY)
        #undef APPLY
    }

    // This table stores the module handles that we have obtained via LoadLibrary. If a handle is null, we have not yet
    // attempted to load that module. If a handle is -1 (INVALID_HANDLE_VALUE), we have attempted to load the module but the
    // attempt failed.
    //
    // These handles are leaked on purpose if a module is unloaded.
    static HMODULE module_handles[module_id_count];

    // This table stores the function pointers that we have loaded dynamically. If a function pointer is a null pointer, we
    // have not yet attempted to get that function pointer. If a function pointer is a -1, we have attempted to get that
    // function pointer but the attempt failed.
    static void *function_pointers[function_id_count];

    static __forceinline void* __cdecl invalid_function_sentinel() noexcept
    {
        return reinterpret_cast<void*>(static_cast<uintptr_t>(-1));
    }



    static HMODULE __cdecl try_load_library_from_system_directory(
        wchar_t const* const name) noexcept
    {
        HMODULE const handle = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (handle)
        {
            return handle;
        }

        // LOAD_LIBRARY_SEARCH_SYSTEM32 is only supported by Windows 7 and above; if the OS does not support this flag, try
        // again without it.  On these OSes, all APISets will be forwarders.  To prevent DLL hijacking, do not attempt to
        // load the APISet forwarders dynamically.  This will cause our caller to fall back to the real DLL (e.g. kernel32).
        // All of those are known DLLs.
        if (GetLastError() == ERROR_INVALID_PARAMETER &&
            wcsncmp(name, L"api-ms-", 7) != 0)
        {
            return LoadLibraryExW(name, nullptr, 0);
        }

        return nullptr;
    }



    static HMODULE __cdecl try_get_module(module_id const id) noexcept
    {
        // First check to see if we've cached the module handle:
        if (HMODULE const cached_handle = __crt_interlocked_read_pointer(module_handles + id))
        {
            if (cached_handle == INVALID_HANDLE_VALUE)
            {
                return nullptr;
            }

            return cached_handle;
        }

        // If we haven't yet cached the module handle, try to load the library.  If
        // this fails, cache the sentinel handle value INVALID_HANDLE_VALUE so that
        // we don't attempt to load the module again:
        HMODULE const new_handle = try_load_library_from_system_directory(module_names[id]);
        if (!new_handle)
        {
            if (HMODULE const cached_handle = __crt_interlocked_exchange_pointer(module_handles + id, INVALID_HANDLE_VALUE))
            {
                _ASSERTE(cached_handle == INVALID_HANDLE_VALUE);
            }

            return nullptr;
        }

        // Swap the new handle into the cache.  If the cache no longer contained a
        // null handle, then some other thread loaded the module and cached the
        // handle while we were doing the same.  In that case, we free the handle
        // once to maintain the reference count:
        if (HMODULE const cached_handle = __crt_interlocked_exchange_pointer(module_handles + id, new_handle))
        {
            _ASSERTE(cached_handle == new_handle);
            FreeLibrary(new_handle);
        }

        return new_handle;
    }



    static HMODULE __cdecl try_get_first_available_module(
        module_id const* const first,
        module_id const* const last
        ) noexcept
    {
        for (module_id const* it = first; it != last; ++it)
        {
            HMODULE const handle = try_get_module(*it);
            if (handle)
            {
                return handle;
            }
        }

        return nullptr;
    }



    static __forceinline void* __cdecl try_get_proc_address_from_first_available_module(
        char      const* const name,
        module_id const* const first_module_id,
        module_id const* const last_module_id
        ) noexcept
    {
        HMODULE const module_handle = try_get_first_available_module(first_module_id, last_module_id);
        if (!module_handle)
        {
            return nullptr;
        }

        return reinterpret_cast<void*>(GetProcAddress(module_handle, name));
    }



    static void* __cdecl try_get_function(
        function_id      const id,
        char      const* const name,
        module_id const* const first_module_id,
        module_id const* const last_module_id
        ) noexcept
    {
        // First check to see if we've cached the function pointer:
        {
            void *const cached_fp = __crt_interlocked_read_pointer(function_pointers + id);

            if (cached_fp == invalid_function_sentinel())
            {
                return nullptr;
            }

            if (cached_fp)
            {
                return cached_fp;
            }
        }

        // If we haven't yet cached the function pointer, try to import it from any of the modules in which it might be
        // defined.  If this fails, cache the sentinel pointer so that we don't attempt to load this function again:
        void *const new_fp = try_get_proc_address_from_first_available_module(name, first_module_id, last_module_id);
        if (!new_fp)
        {
            void *const cached_fp = __crt_interlocked_exchange_pointer(function_pointers + id, invalid_function_sentinel());

            if (cached_fp)
            {
                _ASSERTE(cached_fp == invalid_function_sentinel());
            }

            return nullptr;
        }

        // Swap the newly obtained function pointer into the cache. The cache may no longer contain a null pointer if
        // another thread obtained the function address while we were doing the same (both threads should have gotten the
        // same function pointer):
        {
            void *const cached_fp = __crt_interlocked_exchange_pointer(function_pointers + id, new_fp);
            if (cached_fp)
            {
                _ASSERTE(cached_fp == new_fp);
            }
        }

        return new_fp;
    }



    // Generate accessors that wrap the general try_get_function for each function,
    // passing the correct set of candidate modules and returning a function pointer
    // of the correct type:
    #define APPLY(_FUNCTION, _MODULES)                                                                    \
        static _CRT_CONCATENATE(_FUNCTION, _pft) __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept \
        {                                                                                                 \
            static module_id const candidate_modules[] = _CRT_UNPARENTHESIZE(_MODULES);                   \
                                                                                                          \
            return reinterpret_cast<_CRT_CONCATENATE(_FUNCTION, _pft)>(try_get_function(                  \
                _CRT_CONCATENATE(_FUNCTION, _id),                                                         \
                _CRT_STRINGIZE(_FUNCTION),                                                                \
                candidate_modules,                                                                        \
                candidate_modules + __crt_countof(candidate_modules)));                                   \
        }
    APPLY_TO_LATE_BOUND_FUNCTIONS(APPLY)
    #undef APPLY



    extern "C" DWORD __cdecl __vcrt_FlsAlloc(PFLS_CALLBACK_FUNCTION const callback)
    {
        if (auto const fls_alloc = try_get_FlsAlloc())
        {
            return fls_alloc(callback);
        }

        return TlsAlloc();
    }

    extern "C" BOOL __cdecl __vcrt_FlsFree(DWORD const fls_index)
    {
        if (auto const fls_free = try_get_FlsFree())
        {
            return fls_free(fls_index);
        }

        return TlsFree(fls_index);
    }

    extern "C" PVOID __cdecl __vcrt_FlsGetValue(DWORD const fls_index)
    {
        if (auto const fls_get_value = try_get_FlsGetValue())
        {
            return fls_get_value(fls_index);
        }

        return TlsGetValue(fls_index);
    }

    extern "C" BOOL __cdecl __vcrt_FlsSetValue(DWORD const fls_index, PVOID const fls_data)
    {
        if (auto const fls_set_value = try_get_FlsSetValue())
        {
            return fls_set_value(fls_index, fls_data);
        }

        return TlsSetValue(fls_index, fls_data);
    }

    extern "C" BOOL __cdecl __vcrt_InitializeCriticalSectionEx(
        LPCRITICAL_SECTION const critical_section,
        DWORD              const spin_count,
        DWORD              const flags
        )
    {
        if (auto const initialize_critical_section_ex = try_get_InitializeCriticalSectionEx())
        {
            return initialize_critical_section_ex(critical_section, spin_count, flags);
        }

        return InitializeCriticalSectionAndSpinCount(critical_section, spin_count);
    }

#endif // _VCRT_WIN32_WINNT < _WIN32_WINNT_VISTA
