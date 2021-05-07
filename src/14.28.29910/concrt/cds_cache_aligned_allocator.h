/***
* ==++==
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
* Microsoft would like to acknowledge that this concurrency data structure implementation
* is based on Intel's implementation in its Threading Building Blocks ("Intel Material").
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* cds_cache_aligned_allocator.h
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

/*
    Intel Material Copyright 2005-2008 Intel Corporation.  All Rights Reserved.
*/

#pragma once

#include "concrtinternal.h"
#include <new>

namespace Concurrency
{

namespace details
{

    // Compile-time constant that is upper bound on cache line/sector size.
    /** It should be used only in situations where having a compile-time upper
        bound is more useful than a run-time exact answer. */
    const size_t NFS_MaxLineSize = 128;

    // Cache/sector line size.
    _CONCRTIMP size_t NFS_GetLineSize();

    // Allocate memory on cache/sector line boundary.
    _CONCRTIMP void* NFS_Allocate( size_t n_element, size_t element_size, void* hint );

    // Free memory allocated by NFS_Allocate.
    /** Freeing a NULL pointer is allowed, but has no effect. */
    _CONCRTIMP void NFS_Free( void* );

    // Meets "allocator" requirements of ISO C++ Standard, Section 20.1.5
    /** The members are ordered the same way they are in section 20.4.1
        of the ISO C++ standard. */
    template<typename _Ty>
    class cache_aligned_allocator
    {
    public:
        typedef _Ty* pointer;
        typedef const _Ty* const_pointer;
        typedef _Ty& reference;
        typedef const _Ty& const_reference;
        typedef _Ty value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        template<typename _U> struct rebind
        {
            typedef cache_aligned_allocator<_U> other;
        };

        cache_aligned_allocator() noexcept
        {
        }

        cache_aligned_allocator( const cache_aligned_allocator& ) noexcept
        {
        }

        template<typename _U> cache_aligned_allocator(const cache_aligned_allocator<_U>&) noexcept
        {
        }

        pointer address(reference x) const
        {
            return &x;
        }

        const_pointer address(const_reference x) const
        {
            return &x;
        }

        // Allocate space for n objects, starting on a cache/sector line.
        pointer allocate( size_type n, const void* hint=0 )
        {
            // The "hint" argument is always ignored in NFS_Allocate thus const_cast shouldn't hurt
            return pointer(details::NFS_Allocate( n, sizeof(_Ty), const_cast<void*>(hint) ));
        }

        // Free block of memory that starts on a cache line
        void deallocate( pointer p, size_type )
        {
            details::NFS_Free(p);
        }

        // Largest value for which method allocate might succeed.
        size_type max_size() const noexcept
        {
            return (~size_t(0)-details::NFS_MaxLineSize)/sizeof(_Ty);
        }

        // Copy-construct value at location pointed to by p.
        void construct( pointer p, const _Ty& value )
        {
            new(static_cast<void*>(p)) _Ty(value);
        }

        // Destroy value at location pointed to by p.
        void destroy( pointer p )
        {
            p->~_Ty();
        }
    };

    // Analogous to std::allocator<void>, as defined in ISO C++ Standard, Section 20.4.1
    template<>
    class cache_aligned_allocator<void> {
    public:
        typedef void* pointer;
        typedef const void* const_pointer;
        typedef void value_type;

        template<typename _U> struct rebind
        {
            typedef cache_aligned_allocator<_U> other;
        };
    };

    template<typename _Ty, typename _U>
    inline bool operator==( const cache_aligned_allocator<_Ty>&, const cache_aligned_allocator<_U>& )
    {
        return true;
    }

    template<typename _Ty, typename _U>
    inline bool operator!=( const cache_aligned_allocator<_Ty>&, const cache_aligned_allocator<_U>& )
    {
        return false;
    }

} // namespace details

} // namespace Concurrency
