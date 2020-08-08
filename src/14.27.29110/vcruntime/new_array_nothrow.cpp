//
// new_array_nothrow.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator new, nothrow overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>

////////////////////////////////////
// new() Fallback Ordering
//
// +----------+
// |new_scalar<---------------+
// +----^-----+               |
//      |                     |
// +----+-------------+  +----+----+
// |new_scalar_nothrow|  |new_array|
// +------------------+  +----^----+
//                            |
//               +------------+----+
//               |new_array_nothrow|
//               +-----------------+

void* __CRTDECL operator new[](size_t const size, std::nothrow_t const& x) noexcept
{
    return operator new(size, x);
}
