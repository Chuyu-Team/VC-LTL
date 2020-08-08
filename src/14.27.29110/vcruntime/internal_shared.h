//
// internal_shared.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Header-only utilities used by both the VCRuntime and the CoreCRT.
//
#pragma once

#include <crtdbg.h>
#include <intrin.h>
#include <malloc.h>
#include <stdlib.h>
#include <vcruntime.h>
#include <Windows.h>

_CRT_BEGIN_C_HEADER



extern IMAGE_DOS_HEADER __ImageBase;



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Preprocessor Utilities and Awesome Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Attributes for managed declarations in the CRT
#ifdef _M_CEE
    #define _CRT_INTEROPSERVICES_DLLIMPORT(_DllName , _EntryPoint , _CallingConvention) \
        [System::Runtime::InteropServices::DllImport( \
            _DllName , EntryPoint = _EntryPoint, CallingConvention = _CallingConvention)]
    #define _CRT_SUPPRESS_UNMANAGED_CODE_SECURITY [System::Security::SuppressUnmanagedCodeSecurity]
    #define _CRT_CALLING_CONVENTION_CDECL System::Runtime::InteropServices::CallingConvention::Cdecl
    #define _CRT_CALLING_CONVENTION_WINAPI System::Runtime::InteropServices::CallingConvention::Winapi
    #define _CRT_RELIABILITY_CONTRACT \
        [System::Runtime::ConstrainedExecution::ReliabilityContract( \
            System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
            System::Runtime::ConstrainedExecution::Cer::Success)]
    #define _CRT_ASSERT_UNMANAGED_CODE_ATTRIBUTE \
        [System::Security::Permissions::SecurityPermissionAttribute( \
            System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
    #define _CRT_SECURITYCRITICAL_ATTRIBUTE [System::Security::SecurityCritical]
    #define _CRT_SECURITYSAFECRITICAL_ATTRIBUTE [System::Security::SecuritySafeCritical]
#else
    #define _CRT_INTEROPSERVICES_DLLIMPORT(_DllName , _EntryPoint , _CallingConvention)
    #define _CRT_SUPPRESS_UNMANAGED_CODE_SECURITY
    #define _CRT_CALLING_CONVENTION_CDECL
    #define _CRT_CALLING_CONVENTION_WINAPI
    #define _CRT_RELIABILITY_CONTRACT
    #define _CRT_ASSERT_UNMANAGED_CODE_ATTRIBUTE
    #define _CRT_SECURITYCRITICAL_ATTRIBUTE
    #define _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
#endif



// The pointer-width interlocked exchange and compare-exchange operations are
// not defined for x86.  We declare them ourselves so that we can use the same
// macros consistently across all architectures:
#ifdef _M_IX86

    #undef _InterlockedExchangePointer
    #undef _InterlockedCompareExchangePointer

    #define _InterlockedExchangePointer(target, value) \
        ((void*)_InterlockedExchange((long volatile*)(target), (long)(value)))

    #define _InterlockedCompareExchangePointer(target, exchange, comparand) \
        ((void*)_InterlockedCompareExchange((long volatile*)(target), (long)(exchange), (long)(comparand)))

#endif



#define _BEGIN_SECURE_CRT_DEPRECATION_DISABLE \
    __pragma(warning(push))                   \
    __pragma(warning(disable:4996))           \
    __pragma(warning(disable:25025))          \
    __pragma(warning(disable:28719))

#define _END_SECURE_CRT_DEPRECATION_DISABLE   \
    __pragma(warning(pop))



#define CRT_WARNING_DISABLE_PUSH(x,y) \
    __pragma(warning(push))           \
    __pragma(warning(disable: x))

#define CRT_WARNING_POP               \
    __pragma(warning(pop))



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Section Attributes
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#pragma section(".CRT$XCA",    long, read) // First C++ Initializer
#pragma section(".CRT$XCAA",   long, read) // Startup C++ Initializer
#pragma section(".CRT$XCZ",    long, read) // Last C++ Initializer

#pragma section(".CRT$XDA",    long, read) // First Dynamic TLS Initializer
#pragma section(".CRT$XDZ",    long, read) // Last Dynamic TLS Initializer

#pragma section(".CRT$XIA",    long, read) // First C Initializer
#pragma section(".CRT$XIAA",   long, read) // Startup C Initializer
#pragma section(".CRT$XIAB",   long, read) // PGO C Initializer
#pragma section(".CRT$XIAC",   long, read) // Post-PGO C Initializer
#pragma section(".CRT$XIC",    long, read) // CRT C Initializers
#pragma section(".CRT$XIYA",   long, read) // VCCorLib Threading Model Initializer
#pragma section(".CRT$XIYAA",  long, read) // XAML Designer Threading Model Override Initializer
#pragma section(".CRT$XIYB",   long, read) // VCCorLib Main Initializer
#pragma section(".CRT$XIZ",    long, read) // Last C Initializer

#pragma section(".CRT$XLA",    long, read) // First Loader TLS Callback
#pragma section(".CRT$XLC",    long, read) // CRT TLS Constructor
#pragma section(".CRT$XLD",    long, read) // CRT TLS Terminator
#pragma section(".CRT$XLZ",    long, read) // Last Loader TLS Callback

#pragma section(".CRT$XPA",    long, read) // First Pre-Terminator
#pragma section(".CRT$XPB",    long, read) // CRT ConcRT Pre-Terminator
#pragma section(".CRT$XPX",    long, read) // CRT Pre-Terminators
#pragma section(".CRT$XPXA",   long, read) // CRT stdio Pre-Terminator
#pragma section(".CRT$XPZ",    long, read) // Last Pre-Terminator

#pragma section(".CRT$XTA",    long, read) // First Terminator
#pragma section(".CRT$XTZ",    long, read) // Last Terminator

#pragma section(".CRTMA$XCA",  long, read) // First Managed C++ Initializer
#pragma section(".CRTMA$XCZ",  long, read) // Last Managed C++ Initializer

#pragma section(".CRTVT$XCA",  long, read) // First Managed VTable Initializer
#pragma section(".CRTVT$XCZ",  long, read) // Last Managed VTable Initializer

#pragma section(".rdata$T",    long, read)

#pragma section(".rtc$IAA",    long, read) // First RTC Initializer
#pragma section(".rtc$IZZ",    long, read) // Last RTC Initializer

#pragma section(".rtc$TAA",    long, read) // First RTC Terminator
#pragma section(".rtc$TZZ",    long, read) // Last RTC Terminator

#define _CRTALLOC(x) __declspec(allocate(x))



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// "Special" Data
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef _M_CEE
    typedef void (__cdecl* _PVFV)(void);
    typedef int  (__cdecl* _PIFV)(void);

    extern _CRTALLOC(".CRT$XIA") _PIFV __xi_a[]; // First C Initializer
    extern _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[]; // Last C Initializer
    extern _CRTALLOC(".CRT$XCA") _PVFV __xc_a[]; // First C++ Initializer
    extern _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[]; // Last C++ Initializer
    extern _CRTALLOC(".CRT$XPA") _PVFV __xp_a[]; // First Pre-Terminator
    extern _CRTALLOC(".CRT$XPZ") _PVFV __xp_z[]; // Last Pre-Terminator
    extern _CRTALLOC(".CRT$XTA") _PVFV __xt_a[]; // First Terminator
    extern _CRTALLOC(".CRT$XTZ") _PVFV __xt_z[]; // Last Terminator
#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// SEH Encapsulation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Whenever we acquire locks in the CRT, we use SEH __try/__finally blocks to
// ensure that we release the lock under all circumstances, even if an unwind
// operation unwinds through the CRT frames.  (Unexpected unwinding through the
// CRT is not a supported scenario, but we protect ourselves against this
// possibility anyway as a matter of policy.)
//
// These wrappers make it easier to mix SEH handling with RAII types in the CRT.
// The 'action' is called under a __try, and the 'cleanup' executes in the
// __finally block.  Note that we expect the return type of the 'action callable
// to be either 'void' or some trivial type.  At a minimum, the type must be
// value initializable.
#ifdef __cplusplus
extern "C++"
{
    template <typename Result>
    struct __crt_seh_guarded_call
    {
        template <typename Setup, typename Action, typename Cleanup>
        auto operator()(Setup&& setup, Action&& action, Cleanup&& cleanup)
            -> decltype(action())
        {
            decltype(action()) result{};

            setup();
            __try
            {
                result = action();
            }
            __finally
            {
                cleanup();
            }

            return result;
        }
    };

    template <>
    struct __crt_seh_guarded_call<void>
    {
        template <typename Setup, typename Action, typename Cleanup>
        void operator()(Setup&& setup, Action&& action, Cleanup&& cleanup)
        {
            setup();
            __try
            {
                action();
            }
            __finally
            {
                cleanup();
            }
        }
    };

    template <typename Action, typename Cleanup>
    auto __crt_call_and_cleanup(Action&& action, Cleanup&& cleanup)
        -> decltype(action())
    {
        return __crt_seh_guarded_call<decltype(action())>()([](){}, action, cleanup);
    }
}
#endif // __cplusplus



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CRT Memory Allocation and Management
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef _DEBUG

    // These must match the definitions in the CoreCRT's debug header.  They
    // are defined separately here to avoid unwanted CRT header dependencies.
    #define _NORMAL_BLOCK    1
    #define _CRT_BLOCK       2

    #define _calloc_crt(c, s)      (_calloc_dbg  (   c, s, _CRT_BLOCK, __FILE__, __LINE__))
    #define _free_crt(p)           (_free_dbg    (p,       _CRT_BLOCK                    ))
    #define _malloc_crt(s)         (_malloc_dbg  (      s, _CRT_BLOCK, __FILE__, __LINE__))
    #define _msize_crt(p)          (_msize_dbg   (p,       _CRT_BLOCK                    ))
    #define _recalloc_crt(p, c, s) (_recalloc_dbg(p, c, s, _CRT_BLOCK, __FILE__, __LINE__))
    #define _realloc_crt(p, s)     (_realloc_dbg (p,    s, _CRT_BLOCK, __FILE__, __LINE__))

    #define _malloca_crt(size)                                                            \
        __pragma(warning(suppress: 6255))                                                 \
        (_MallocaComputeSize(size) != 0                                                   \
            ? _MarkAllocaS(_malloc_crt(_MallocaComputeSize(size)), _ALLOCA_S_HEAP_MARKER) \
            : NULL)

#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv

    // The *_crt macros call the allocation function that vcruntime should use for
    // internal allocations. It changes based off of where it is being built.

    #ifdef _CRT_WINDOWS
    // When building for the UCRT, we want to use the internal allocation functions.
    // We need to ensure that users hooking the public allocation functions do not
    // interfere with the UCRT's allocations.
        #define _calloc_crt   _calloc_base
        #define _free_crt     _free_base
        #define _malloc_crt   _malloc_base
        #define _realloc_crt  _realloc_base
        #define _msize_crt    _msize_base
        #define _recalloc_crt _recalloc_base
    #else
    // When building for vcruntime*.dll, we are not a part of the UCRT or OS so we
    // should use the public allocation functions exported by the UCRT.
        #define _calloc_crt   calloc
        #define _free_crt     free
        #define _malloc_crt   malloc
        #define _realloc_crt  realloc
        #define _msize_crt    _msize
        #define _recalloc_crt _recalloc
    #endif

    #define _malloca_crt(size)                                                                 \
        __pragma(warning(suppress: 6255))                                                      \
        (_MallocaComputeSize(size) != 0                                                        \
            ? ((_MallocaComputeSize(size) <= _ALLOCA_S_THRESHOLD)                              \
                ? _MarkAllocaS(_alloca(_MallocaComputeSize(size)), _ALLOCA_S_STACK_MARKER)     \
                : _MarkAllocaS(_malloc_crt(_MallocaComputeSize(size)), _ALLOCA_S_HEAP_MARKER)) \
            : NULL)

#endif // !_DEBUG

#pragma warning(push)
#pragma warning(disable: 6014)
__inline void __CRTDECL _freea_crt(_Pre_maybenull_ _Post_invalid_ void* memory)
{
    if (memory)
    {
        unsigned int marker;

        memory = (char*)memory - _ALLOCA_S_MARKER_SIZE;
        marker = *(unsigned int*)memory;
        if (marker == _ALLOCA_S_HEAP_MARKER)
        {
            _free_crt(memory);
        }
        else if (marker != _ALLOCA_S_STACK_MARKER)
        {
            _ASSERTE(("Corrupted pointer passed to _freea", 0));
        }
    }
}
#pragma warning(pop)

#ifdef __cplusplus
extern "C++" {

    struct __crt_internal_free_policy
    {
        template <typename T>
        void operator()(T const* const p) const noexcept
        {
            _free_crt(const_cast<T*>(p));
        }
    };

    struct __crt_public_free_policy
    {
        template <typename T>
        void operator()(T const* const p) const noexcept
        {
            free(const_cast<T*>(p));
        }
    };

    template <typename T, typename Free = __crt_internal_free_policy>
    class __crt_unique_heap_ptr
    {
    public:

        explicit __crt_unique_heap_ptr(T* const p = nullptr) noexcept
            : _p(p)
        {
        }

        __crt_unique_heap_ptr(__crt_unique_heap_ptr const&) = delete;
        __crt_unique_heap_ptr& operator=(__crt_unique_heap_ptr const&) = delete;

        __crt_unique_heap_ptr(__crt_unique_heap_ptr&& other) noexcept
            : _p(other._p)
        {
            other._p = nullptr;
        }

        ~__crt_unique_heap_ptr() noexcept
        {
            release();
        }

        __crt_unique_heap_ptr& operator=(__crt_unique_heap_ptr&& other) noexcept
        {
            release();
            _p = other._p;
            other._p = nullptr;
            return *this;
        }

        T* detach() noexcept
        {
            T* const local_p{_p};
            _p = nullptr;
            return local_p;
        }

        void attach(T* const p) noexcept
        {
            release();
            _p = p;
        }

        void release() noexcept
        {
            Free()(_p);
            _p = nullptr;
        }

        bool is_valid() const noexcept
        {
            return _p != nullptr;
        }

        explicit operator bool() const noexcept
        {
            return is_valid();
        }

        T* get() const noexcept
        {
            return _p;
        }

        T** get_address_of() noexcept
        {
            return &_p;
        }

        T** release_and_get_address_of() noexcept
        {
            release();
            return &_p;
        }

    private:
        T* _p;
    };

    // An internal-use scoped smart pointer for memory allocated by _malloca_crt.
    template <typename T>
    struct __crt_scoped_stack_ptr_tag
    {
        __crt_scoped_stack_ptr_tag(T* const p) noexcept
            : _p(p)
        {
        }

        T* _p;
    };

    template <typename T>
    class __crt_scoped_stack_ptr
    {
    public:

        explicit __crt_scoped_stack_ptr(__crt_scoped_stack_ptr_tag<T> const p) noexcept
            : _p(p._p)
        {
        }

        __crt_scoped_stack_ptr(__crt_scoped_stack_ptr const&) = delete;
        __crt_scoped_stack_ptr& operator=(__crt_scoped_stack_ptr const&) = delete;

        ~__crt_scoped_stack_ptr() noexcept
        {
            _freea_crt(_p);
        }

        T* get() const noexcept { return _p; }

        // Note that we do not provide a release() because one would not be
        // useful:  a stack allocation is only useful in the scope in which it
        // was allocated.

        explicit operator bool() const noexcept
        {
            return _p != nullptr;
        }

    private:
        T* const _p;
    };

    // Note that no overflow checks are necessary for the multiplications here,
    // because each multiplication operation is an argument to a parameter that
    // is decorated __declspec(guard(overflow)).
    #define _calloc_crt_t(t, n)      (__crt_unique_heap_ptr     <t>(static_cast<t*>(_calloc_crt  (     (n),  sizeof(t)))))
    #define _malloc_crt_t(t, n)      (__crt_unique_heap_ptr     <t>(static_cast<t*>(_malloc_crt  (     (n) * sizeof(t)))))
    #define _recalloc_crt_t(t, p, n) (__crt_unique_heap_ptr     <t>(static_cast<t*>(_recalloc_crt((p), (n),  sizeof(t)))))
    #define _malloca_crt_t(t, n)     (__crt_scoped_stack_ptr_tag<t>(static_cast<t*>(_malloca_crt (     (n) * sizeof(t)))))



    enum : int
    {
        __crt_maximum_pointer_shift = sizeof(uintptr_t) * 8
    };

    inline unsigned int __crt_rotate_pointer_value(unsigned int const value,
        int const shift) noexcept
    {
        return RotateRight32(value, shift);
    }

    inline unsigned __int64 __crt_rotate_pointer_value(unsigned __int64 const value,
        int const shift) noexcept
    {
        return RotateRight64(value, shift);
    }

    // Fast alternatives to the encode/decode pointer functions that do not use
    // the EncodePointer and DecodePointer functions.
    template <typename T>
    T __crt_fast_decode_pointer(T const p) noexcept
    {
        return reinterpret_cast<T>(
            __crt_rotate_pointer_value(
                reinterpret_cast<uintptr_t>(p) ^ __security_cookie,
                __security_cookie % __crt_maximum_pointer_shift
            )
        );
    }

    template <typename T>
    T __crt_fast_encode_pointer(T const p) noexcept
    {
        return reinterpret_cast<T>(
            __crt_rotate_pointer_value(
                reinterpret_cast<uintptr_t>(p),
                __crt_maximum_pointer_shift - (__security_cookie % __crt_maximum_pointer_shift)
            ) ^ __security_cookie
        );
    }

    // The primary __crt_fast_encode_pointer template does not work properly
    // when it is called with the argument 'nullptr' because the encoded void*
    // pointer is casted back to nullptr_t, and nullptr_t can only represent a
    // single value:  the real, unencoded null pointer.  Therefore, we overload
    // the function for nullptr_t, and defer the cast until we know the actual
    // type that we need.
    struct __crt_fast_encoded_nullptr_t
    {
        template <typename T>
        operator T*() const noexcept
        {
            return __crt_fast_encode_pointer(static_cast<T*>(nullptr));
        }
    };

    inline __crt_fast_encoded_nullptr_t __crt_fast_encode_pointer(decltype(nullptr)) noexcept
    {
        return __crt_fast_encoded_nullptr_t();
    }



    template <typename T>
    T __crt_get_proc_address(HMODULE const m, char const* const f) noexcept
    {
        return reinterpret_cast<T>(::GetProcAddress(m, f));
    }

    template <typename T, typename V>
    T* __crt_interlocked_exchange_pointer(T* const volatile* target, V const value) noexcept
    {
        // This is required to silence a spurious unreferenced formal parameter
        // warning.
        UNREFERENCED_PARAMETER(value);

        return reinterpret_cast<T*>(_InterlockedExchangePointer((void**)target, (void*)value));
    }

    template <typename T, typename E, typename C>
    T __crt_interlocked_compare_exchange(T* const target, E const exchange,
        C const comparand) noexcept
    {
        UNREFERENCED_PARAMETER(exchange);  // These are required to silence spurious
        UNREFERENCED_PARAMETER(comparand); // unreferenced formal parameter warnings.

        static_assert(sizeof(T) == sizeof(LONG), "Type being compared must be same size as a LONG.");
        return static_cast<T>(_InterlockedCompareExchange(
            reinterpret_cast<LONG*>(target), (LONG)exchange, (LONG)comparand));
    }

    template <typename T, typename E, typename C>
    T* __crt_interlocked_compare_exchange_pointer(T* const volatile* target, E const exchange,
        C const comparand) noexcept
    {
        UNREFERENCED_PARAMETER(exchange);  // These are required to silence spurious
        UNREFERENCED_PARAMETER(comparand); // unreferenced formal parameter warnings.

        return reinterpret_cast<T*>(_InterlockedCompareExchangePointer(
            (void**)target, (void*)exchange, (void*)comparand));
    }

    #ifndef _M_CEE_PURE

        #if defined _M_ARM
            #define __crt_interlocked_memory_barrier() (__dmb(_ARM_BARRIER_ISH))
        #elif defined _M_ARM64
            #define __crt_interlocked_memory_barrier() (__dmb(_ARM64_BARRIER_ISH))
        #endif

        inline __int32 __crt_interlocked_read_32(__int32 const volatile* target) noexcept
        {
            #if defined _M_IX86 || defined _M_X64
            __int32 const result = *target;
            _ReadWriteBarrier();
            return result;
            #elif defined _M_ARM || defined _M_ARM64
            __int32 const result = __iso_volatile_load32(reinterpret_cast<int const volatile*>(target));
            __crt_interlocked_memory_barrier();
            return result;
            #else
            #error Unsupported architecture
            #endif
        }

        #if defined _WIN64
            inline __int64 __crt_interlocked_read_64(__int64 const volatile* target) noexcept
            {
                #if defined _M_X64
                __int64 const result = *target;
                _ReadWriteBarrier();
                return result;
                #elif defined _M_ARM64
                __int64 const result = __iso_volatile_load64(target);
                __crt_interlocked_memory_barrier();
                return result;
                #else
                #error Unsupported architecture
                #endif
            }
        #endif // _WIN64

        template <typename T>
        T __crt_interlocked_read(T const volatile* target) noexcept
        {
            static_assert(sizeof(T) == sizeof(__int32), "Type being read must be 32 bits in size.");
            return (T)__crt_interlocked_read_32((__int32*)target);
        }


        template <typename T>
        T* __crt_interlocked_read_pointer(T* const volatile* target) noexcept
        {
            #ifdef _WIN64
            return (T*)__crt_interlocked_read_64((__int64*)target);
            #else
            return (T*)__crt_interlocked_read_32((__int32*)target);
            #endif
        }

    #endif // _M_CEE_PURE

} // extern "C++"
#endif // __cplusplus



#define _CRT_DEBUGGER_IGNORE            -1
#define _CRT_DEBUGGER_GSFAILURE          1
#define _CRT_DEBUGGER_INVALIDPARAMETER   2
#define _CRT_DEBUGGER_ABORT              3

// Note:  These names are well-known to the debugger
#ifdef _M_IX86
    void __cdecl _crt_debugger_hook(int);
    #define _CRT_DEBUGGER_HOOK _crt_debugger_hook
#else
    void __cdecl __crt_debugger_hook(int);
    #define _CRT_DEBUGGER_HOOK __crt_debugger_hook
#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Precondition Validation Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef _DEBUG
    #define _INVALID_PARAMETER(expr) _invalid_parameter(expr, __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else
    #define _INVALID_PARAMETER(expr) _invalid_parameter_noinfo()
#endif

#define _VALIDATE_CLEAR_OSSERR_RETURN(expr, errorcode, retexpr)                \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            _doserrno = 0L;                                                    \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(expr, errorcode)                 \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            _doserrno = 0L;                                                    \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return errorcode;                                                  \
        }                                                                      \
    }

#define _VALIDATE_RETURN(expr, errorcode, retexpr)                             \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_RETURN_ERRCODE(expr, errorcode)                              \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return errorcode;                                                  \
        }                                                                      \
    }

#define _VALIDATE_RETURN_ERRCODE_NOEXC(expr, errorcode)                        \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            errno = errorcode;                                                 \
            return errorcode;                                                  \
        }                                                                      \
    }

#define _VALIDATE_RETURN_NOERRNO(expr, retexpr)                                \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_RETURN_NOEXC(expr, errorcode, retexpr)                       \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            errno = errorcode;                                                 \
            return retexpr;                                                    \
        }                                                                      \
    }

#define _VALIDATE_RETURN_VOID(expr, errorcode)                                 \
    {                                                                          \
        int _Expr_val=!!(expr);                                                \
        _ASSERT_EXPR((_Expr_val), _CRT_WIDE(#expr));                           \
        if (!(_Expr_val))                                                      \
        {                                                                      \
            errno = errorcode;                                                 \
            _INVALID_PARAMETER(_CRT_WIDE(#expr));                              \
            return;                                                            \
        }                                                                      \
    }



_CRT_END_C_HEADER
