//
// delete_array.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator delete.
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
void __CRTDECL operator delete[](void* const block) noexcept
{
    operator delete(block);
}
