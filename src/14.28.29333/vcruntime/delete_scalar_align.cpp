//
// delete_scalar_align.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator delete, align_val_t overload.
//
#include <malloc.h>
#include <vcruntime_new.h>
#include <vcstartup_internal.h>

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

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(void* const block, std::align_val_t const) noexcept
{
    _aligned_free(block);
}
