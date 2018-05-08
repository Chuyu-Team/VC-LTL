//
// vcruntime_new.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Declarations and definitions of memory management functions in the VCRuntime.
//
#pragma once

#include <vcruntime.h>

#ifdef __cplusplus
extern "C++" {

#pragma pack(push, _CRT_PACKING)

#pragma warning(push)
#pragma warning(disable: 4985) // attributes not present on previous declaration

#pragma push_macro("new")
#undef new

// C++17 or greater only
#ifndef _HAS_ALIGNED_NEW
 #ifdef _MSVC_LANG
  #if _MSVC_LANG > 201402
   #define _HAS_ALIGNED_NEW	1
  #else
   #define _HAS_ALIGNED_NEW	0
  #endif
 #else
  #if __cplusplus > 201402
   #define _HAS_ALIGNED_NEW	1
  #else
   #define _HAS_ALIGNED_NEW	0
  #endif
 #endif
#endif

#if _HAS_ALIGNED_NEW
namespace std
{
    enum class align_val_t : size_t {};
}
#endif

#ifndef __NOTHROW_T_DEFINED
#define __NOTHROW_T_DEFINED
    namespace std
    {
        struct nothrow_t { };

        #ifdef _CRT_ENABLE_SELECTANY_NOTHROW
            extern __declspec(selectany) nothrow_t const nothrow;
        #else
            extern nothrow_t const nothrow;
        #endif
    }
#endif

_Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t _Size
    );

_Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t _Size,
    std::nothrow_t const&
    ) noexcept;

_Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t _Size
    );

_Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t _Size,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete(
    void* _Block
    ) noexcept;

void __CRTDECL operator delete(
    void* _Block,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete[](
    void* _Block
    ) noexcept;

void __CRTDECL operator delete[](
    void* _Block,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete(
    void*  _Block,
    size_t _Size
    ) noexcept;

void __CRTDECL operator delete[](
    void* _Block,
    size_t _Size
    ) noexcept;

#if _HAS_ALIGNED_NEW
_Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t           _Size,
    std::align_val_t _Al
    );

_Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t                _Size,
    std::align_val_t      _Al,
    std::nothrow_t const&
    ) noexcept;


_Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t           _Size,
    std::align_val_t _Al
    );

_Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t                _Size,
    std::align_val_t      _Al,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete(
    void*            _Block,
    std::align_val_t _Al
    ) noexcept;

void __CRTDECL operator delete(
    void*                 _Block,
    std::align_val_t      _Al,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete[](
    void*            _Block,
    std::align_val_t _Al
    ) noexcept;

void __CRTDECL operator delete[](
    void*                 _Block,
    std::align_val_t      _Al,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete(
    void*            _Block,
    size_t           _Size,
    std::align_val_t _Al
    ) noexcept;

void __CRTDECL operator delete[](
    void*            _Block,
    size_t           _Size,
    std::align_val_t _Al
    ) noexcept;
#endif

#pragma warning(push)
#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified
#ifndef __PLACEMENT_NEW_INLINE
    #define __PLACEMENT_NEW_INLINE
    _Ret_notnull_ _Post_writable_byte_size_(_Size) _Post_satisfies_(return == _Where)
    inline void* __CRTDECL operator new(size_t _Size, _Writable_bytes_(_Size) void* _Where) noexcept
    {
        (void)_Size;
        return _Where;
    }

    inline void __CRTDECL operator delete(void*, void*) noexcept
    {
        return;
    }
#endif

#ifndef __PLACEMENT_VEC_NEW_INLINE
    #define __PLACEMENT_VEC_NEW_INLINE
    _Ret_notnull_ _Post_writable_byte_size_(_Size) _Post_satisfies_(return == _Where)
    inline void* __CRTDECL operator new[](size_t _Size,
        _Writable_bytes_(_Size) void* _Where) noexcept
    {
        (void)_Size;
        return _Where;
    }

    inline void __CRTDECL operator delete[](void*, void*) noexcept
    {
    }
#endif
#pragma warning(pop)

#pragma pop_macro("new")

#pragma warning(pop)
#pragma pack(pop)

} // extern "C++"
#endif // __cplusplus
