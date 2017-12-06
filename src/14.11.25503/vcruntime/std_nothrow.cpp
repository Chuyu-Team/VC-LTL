//
// std_nothrow.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//      Derived from code Copyright (c) 1992-2001 by P.J. Plauger.
//
// Defines std::nothrow.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>

namespace std
{
    // Note:  We use aggregate initialization in order to avoid use of a dynamic
    // initializer.
    const nothrow_t nothrow = { };
}
