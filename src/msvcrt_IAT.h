#pragma once

#include <vcruntime.h>

#if defined _M_IX86
#if defined _M_HYBRID
#define _LCRT_DECLARE_ALTERNATE_NAME_PREFIX "#"
#else
#define _LCRT_DECLARE_ALTERNATE_NAME_PREFIX "_"
#endif
#define _LCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA "_"
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64
#define _LCRT_DECLARE_ALTERNATE_NAME_PREFIX ""
#define _LCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA ""
#else
#error Unsupported architecture
#endif

// The _VCRT_DEFINE_IAT_SYMBOL macro provides an architecture-neutral way of
// defining IAT symbols (__imp_- or _imp__-prefixed symbols).
#ifdef _M_IX86
#define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(_imp__, f)
#else
#define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(__imp_, f)
#endif


#if !defined _LTL_Using_Dynamic_Lib || defined __Build_OBJ

#define _LCRT_DEFINE_IAT_SYMBOL(f)                                                          \
    extern "C" __declspec(selectany) void const* const _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) \
        = reinterpret_cast<void const*>(f)

#define _LCRT_DEFINE_IAT_SYMBOL_EXTERN(f)                                                   \
    extern "C" void __cdecl f();                                                            \
    _LCRT_DEFINE_IAT_SYMBOL(f)

#else

#define _LCRT_DEFINE_IAT_SYMBOL(f) __pragma(comment(linker,"/EXPORT:" _LCRT_DECLARE_ALTERNATE_NAME_PREFIX #f))

#define _LCRT_DEFINE_IAT_SYMBOL_EXTERN _LCRT_DEFINE_IAT_SYMBOL

#endif
