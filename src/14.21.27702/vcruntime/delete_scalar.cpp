//
// delete_scalar.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator delete.
//
#include <crtdbg.h>
#include <malloc.h>
#include <vcruntime_new.h>
#include <vcstartup_internal.h>

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
void __CRTDECL operator delete(void* const block) noexcept
{
    #ifdef _DEBUG
    _free_dbg(block, _UNKNOWN_BLOCK);
    #else
    free(block);
    #endif
}
