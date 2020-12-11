//
// locks.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Critical sections used for synchronization in the VCRuntime.
//
#include <vcruntime_internal.h>

extern "C" {

// This table holds the locks used by the VCRuntime.  It is indexed using the
// enumerators of the __vcrt_lock_id enumeration.
static CRITICAL_SECTION __vcrt_lock_table[__vcrt_lock_count];

// This variable stores the number of locks that have been successfully
// initialized.  Locks are initialized in order and are destroyed in reverse
// order.  The startup and exit code must ensure that initialization and
// destruction is synchronized:  these functions should never be executed
// concurrently.
static unsigned __vcrt_locks_initialized;


bool __cdecl __vcrt_initialize_locks()
{
    for (unsigned i = 0; i < __vcrt_lock_count; ++i)
    {
        if (!__vcrt_InitializeCriticalSectionEx(&__vcrt_lock_table[i], _VCRT_SPINCOUNT, 0))
        {
            __vcrt_uninitialize_locks();
            return false;
        }

        ++__vcrt_locks_initialized;
    }

    return true;
}

bool __cdecl __vcrt_uninitialize_locks()
{
    for (unsigned i = __vcrt_locks_initialized; i > 0; --i)
    {
        DeleteCriticalSection(&__vcrt_lock_table[i - 1]);
        --__vcrt_locks_initialized;
    }

    return true;
}

void __cdecl __vcrt_lock(_In_ __vcrt_lock_id _Lock)
{
    EnterCriticalSection(&__vcrt_lock_table[_Lock]);
}

void __cdecl __vcrt_unlock(_In_ __vcrt_lock_id _Lock)
{
    LeaveCriticalSection(&__vcrt_lock_table[_Lock]);
}

} // extern "C"
