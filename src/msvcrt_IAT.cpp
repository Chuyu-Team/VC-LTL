
#include <vcruntime.h>

#if defined _M_IX86
#if defined _M_HYBRID
#define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "#"
#else
#define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "_"
#endif
#define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA "_"
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64
#define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX ""
#define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA ""
#else
#error Unsupported architecture
#endif

// The _VCRT_DEFINE_IAT_SYMBOL macro provides an architecture-neutral way of
// defining IAT symbols (__imp_- or _imp__-prefixed symbols).
#ifdef _M_IX86
#define _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(_imp__, f)
#else
#define _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(__imp_, f)
#endif

#define _VCRT_DEFINE_IAT_SYMBOL(f)                                                          \
    extern "C" void __cdecl f();                                                            \
    extern "C" __declspec(selectany) void const* const _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) \
        = reinterpret_cast<void const*>(f)


#ifdef _M_X64

_VCRT_DEFINE_IAT_SYMBOL(_set_FMA3_enable);
_VCRT_DEFINE_IAT_SYMBOL(_get_FMA3_enable);

#endif

//_VCRT_DEFINE_IAT_SYMBOL(abort_advanced);
//_VCRT_DEFINE_IAT_SYMBOL(_set_abort_behavior_advanced);

_VCRT_DEFINE_IAT_SYMBOL(_fstat64i32_downlevel);
_VCRT_DEFINE_IAT_SYMBOL(_stat64i32_light);
_VCRT_DEFINE_IAT_SYMBOL(_wstat64i32_light);

_VCRT_DEFINE_IAT_SYMBOL(_stat32_advanced);
_VCRT_DEFINE_IAT_SYMBOL(_stat32i64_advanced);
_VCRT_DEFINE_IAT_SYMBOL(_stat64i32_advanced);
_VCRT_DEFINE_IAT_SYMBOL(_stat64_advanced);

_VCRT_DEFINE_IAT_SYMBOL(_wstat32_advanced);
_VCRT_DEFINE_IAT_SYMBOL(_wstat32i64_advanced);
_VCRT_DEFINE_IAT_SYMBOL(_wstat64i32_advanced);
_VCRT_DEFINE_IAT_SYMBOL(_wstat64_advanced);

_VCRT_DEFINE_IAT_SYMBOL(_purecall_advanced);

_VCRT_DEFINE_IAT_SYMBOL(_set_purecall_handler_advanced);

_VCRT_DEFINE_IAT_SYMBOL(_get_purecall_handler_advanced);


_VCRT_DEFINE_IAT_SYMBOL(_set_invalid_parameter_handler_downlevel);

_VCRT_DEFINE_IAT_SYMBOL(_get_invalid_parameter_handler_downlevel);

_VCRT_DEFINE_IAT_SYMBOL(_set_thread_local_invalid_parameter_handler_downlevel);

_VCRT_DEFINE_IAT_SYMBOL(_get_thread_local_invalid_parameter_handler_downlevel);

_VCRT_DEFINE_IAT_SYMBOL(_set_abort_behavior_downlevel);






