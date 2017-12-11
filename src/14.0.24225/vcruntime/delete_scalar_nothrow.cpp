//
// delete_scalar_nothrow.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator delete, nothrow_t overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



void __CRTDECL operator delete(void* block, std::nothrow_t const&) noexcept
{
    operator delete(block);
}



/*
* Copyright (c) 1992-2001 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.10:0009 */
