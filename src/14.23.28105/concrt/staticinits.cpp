// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// staticinits.cpp
//
// A separate module for static fields and globals that need to be initialized in
// a special, compiler segment. This is done to ensure that these objects are initialized
// before any user code or third-party library code.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
#pragma warning(push)
#pragma warning(disable:4074)

#pragma init_seg(compiler)

    // There shall be no dependency between the objects being initialized here.
    // If they do we should have a StaticInitialize() method that invokes the
    // initializers in the appropriate order.

    // From utils.cpp
    volatile long Security::s_initialized = 0;
    ULONG_PTR Security::s_cookie = Security::InitializeCookie();

    // From SchedulerBase.cpp
    // Define statics
    _StaticLock SchedulerBase::s_schedulerLock;
    _StaticLock SchedulerBase::s_defaultSchedulerLock;

    // A stack that holds free suballocators.
    LockFreeStack<SubAllocator> SchedulerBase::s_subAllocatorFreePool;

    // From ResourceManager.cpp
    _StaticLock ResourceManager::s_lock;

    // From Trace.cpp
    _StaticLock Etw::s_lock;

#pragma warning(pop)

} // namespace details
} // namespace Concurrency
