//
// delete_scalar_size.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator delete, size_t overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>

////////////////////////////////////////////////////////////////
// delete() Fallback Ordering
//
// +-------------+
// |delete_scalar<----+-----------------------+
// +--^----------+    |                       |
//    |               |                       |
// +--+---------+  +--+---------------+  +----+----------------+
// |delete_array|  |delete_scalar_size|  |delete_scalar_nothrow|
// +--^----^----+  +------------------+  +---------------------+
//    |    |
//    |    +-------------------+
//    |                        |
// +--+--------------+  +------+-------------+
// |delete_array_size|  |delete_array_nothrow|
// +-----------------+  +--------------------+

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(void* const block, size_t const) noexcept
{
    operator delete(block);
}
