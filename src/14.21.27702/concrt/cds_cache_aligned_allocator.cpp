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
* cds_cache_aligned_allocator.cpp
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

/*
    Intel Material Copyright 2005-2008 Intel Corporation.  All Rights Reserved.
*/


#include "cds_cache_aligned_allocator.h"
#include <cstdlib>
#include <crtdbg.h>
#include <stdio.h>
#include <Windows.h>

using namespace std;

namespace Concurrency
{

namespace details
{

    static size_t NFS_LineSize = 64;

    _CONCRTIMP size_t NFS_GetLineSize()
    {
        return NFS_LineSize;
    }

    #pragma warning( push )
    // unary minus operator applied to unsigned type, result still unsigned
    #pragma warning( disable: 4146 4706 )

    _CONCRTIMP void* NFS_Allocate( size_t n, size_t element_size, void*)
    {
        size_t m = NFS_LineSize;
        _CONCRT_ASSERT( m<=NFS_MaxLineSize ); // illegal value for NFS_LineSize
        _CONCRT_ASSERT( (m & m-1)==0 ); // must be power of two
        size_t bytes = n*element_size;
        unsigned char* base;
        if( bytes<n || bytes+m<bytes || !(base=(unsigned char*)(malloc(m+bytes))) )
        {
            // Overflow
            throw bad_alloc();
        }
        // Round up to next line
        unsigned char* result = (unsigned char*)((size_t)(base+m)&-m);
        // Record where block actually starts.
        ((size_t*)result)[-1] = size_t(base);
        _CONCRT_ASSERT( ((size_t)result&(m-1)) == 0 );  // The address returned isn't aligned to cache line size
        return result;
    }

    _CONCRTIMP void NFS_Free( void* p ) {
        if( p )
        {
            // Recover where block actually starts
            unsigned char* base = ((unsigned char**)p)[-1];
            _CONCRT_ASSERT( (void*)((size_t)(base+NFS_LineSize)&-NFS_LineSize)==p ); // not allocated by NFS_Allocate?
            free(base);
        }
    }

    #pragma warning( pop )

} // namespace details

} // namespace Concurrency
