//
// new_scalar_align.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator new, align_val_t overload.
//
#include <malloc.h>
#include <vcruntime_new.h>
#include <vcstartup_internal.h>

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

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new(size_t const size, std::align_val_t const al)
{
    for (;;)
    {
        if (void* const block = _aligned_malloc(size, static_cast<size_t>(al)))
        {
            return block;
        }

        if (_callnewh(size) == 0)
        {
            if (size == SIZE_MAX)
            {
                __scrt_throw_std_bad_array_new_length();
            }
            else
            {
                __scrt_throw_std_bad_alloc();
            }
        }

        // The new handler was successful; try to allocate again...
    }
}
