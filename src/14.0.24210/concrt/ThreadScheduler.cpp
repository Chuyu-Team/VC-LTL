// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadScheduler.cpp
//
// Source file containing the implementation for a thread based concrt scheduler
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Creates a thread based scheduler
    /// </summary>
    ThreadScheduler::ThreadScheduler(_In_ const ::Concurrency::SchedulerPolicy& policy) :
        SchedulerBase(policy)
    {
    }

    /// <summary>
    ///     Creates a thread based scheduler
    /// </summary>
    ThreadScheduler* ThreadScheduler::Create(_In_ const ::Concurrency::SchedulerPolicy& policy)
    {
        return _concrt_new ThreadScheduler(policy);
    }

    /// <summary>
    ///     Creates a thread based virtual processor.
    /// </summary>
    VirtualProcessor* ThreadScheduler::CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot* pOwningRoot)
    {
        return _concrt_new ThreadVirtualProcessor(pOwningNode, pOwningRoot);
    }

    ///<summary>
    ///     Returns a newly created thread internal context to the base scheduler.
    ///</summary>
    InternalContextBase *ThreadScheduler::CreateInternalContext()
    {
        return _concrt_new ThreadInternalContext(this);
    }

    /// <summary>
    ///     Destroys a thread based scheduler
    /// </summary>
    ThreadScheduler::~ThreadScheduler()
    {
    }

} // namespace details
} // namespace Concurrency
