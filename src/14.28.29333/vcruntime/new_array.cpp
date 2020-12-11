//
// new_array.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator new.
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

void* __CRTDECL operator new[](size_t const size)
{
    return operator new(size);
}
