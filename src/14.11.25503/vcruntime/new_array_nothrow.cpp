//
// new_array_nothrow.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator new, nothrow overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



void* __CRTDECL operator new[](size_t const size, std::nothrow_t const& x) noexcept
{
    return operator new(size, x);
}



/*
 * Copyright (c) 1992-2001 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.10:0009 */
