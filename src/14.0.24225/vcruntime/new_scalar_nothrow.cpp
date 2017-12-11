//
// new_scalar_nothrow.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator new, nothrow overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



void* __CRTDECL operator new(size_t const size, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new(size);
    }
    catch (...)
    {
        return nullptr;
    }
}



/*
 * Copyright (c) 1992-2001 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.10:0009 */
