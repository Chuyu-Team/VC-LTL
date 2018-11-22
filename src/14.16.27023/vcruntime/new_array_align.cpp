//
// new_array_align.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator new, align_val_t overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>

////////////////////////////////////////////////
// Aligned new() Fallback Ordering
//
// +----------------+
// |new_scalar_align<--------------+
// +----^-----------+              |
//      |                          |
// +----+-------------------+  +---+-----------+
// |new_scalar_align_nothrow|  |new_array_align|
// +------------------------+  +---^-----------+
//                                 |
//                     +-----------+-----------+
//                     |new_array_align_nothrow|
//                     +-----------------------+
//

void* __CRTDECL operator new[](size_t const size, std::align_val_t const al)
{
    return operator new(size, al);
}
