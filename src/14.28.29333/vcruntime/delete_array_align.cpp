//
// delete_array_align.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator delete, align_val_t overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>

//////////////////////////////////////////////////////////////////////////////////
// Aligned delete() Fallback Ordering
//
// +-------------------+
// |delete_scalar_align<----+---------------------------+
// +--^----------------+    |                           |
//    |                     |                           |
// +--+---------------+  +--+---------------------+  +--+------------------------+
// |delete_array_align|  |delete_scalar_size_align|  |delete_scalar_align_nothrow|
// +--^-----^---------+  +------------------------+  +---------------------------+
//    |     |
//    |     +------------------------+
//    |                              |
// +--+--------------------+  +------+-------------------+
// |delete_array_size_align|  |delete_array_align_nothrow|
// +-----------------------+  +--------------------------+

void __CRTDECL operator delete[](void* const block, std::align_val_t const al) noexcept
{
    operator delete(block, al);
}
