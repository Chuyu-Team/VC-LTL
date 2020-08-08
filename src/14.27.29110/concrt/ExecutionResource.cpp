// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ExecutionResource.cpp
//
// Part of the ConcRT Resource Manager -- this file contains the internal implementation for the execution
// resource.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a new execution resource.
    /// </summary>
    /// <param name="pSchedulerProxy">
    ///     The scheduler proxy this resource is created for. A scheduler proxy holds RM data associated with an instance of
    ///     a scheduler.
    /// <param name="pNode">
    ///     The processor node that this resource belongs to. The processor node is one among the nodes allocated to the
    ///     scheduler proxy.
    /// </param>
    /// <param name="coreIndex">
    ///     The index into the array of cores for the processor node specified.
    /// </param>
    ExecutionResource::ExecutionResource(SchedulerProxy * pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex)
        : m_pSchedulerProxy(pSchedulerProxy)
        , m_pParentExecutionResource(NULL)
        , m_pVirtualProcessorRoot(NULL)
        , m_tlsResetValue(0)
        , m_nodeId(pNode->m_id)
        , m_coreIndex(coreIndex)
        , m_numThreadSubscriptions(0)
    {
        // Derive the execution resource id from the node and the core.
        m_executionResourceId = ((int)(pNode->m_processorGroup) << 8) + pNode->m_pCores[coreIndex].m_processorNumber;
    }

    /// <summary>
    ///     Constructs a new execution resource.
    /// </summary>
    /// <param name="pSchedulerProxy">
    ///     The scheduler proxy this resource is created for. A scheduler proxy holds RM data associated with an instance of
    ///     a scheduler.
    /// <param name="pParentExecutionResource">
    ///     The parent execution resource representing this thread. If there was already an execution resource on the
    ///     calling thread that was created in a different scheduler, it becomes the parent of this execution resource.
    /// </param>
    ExecutionResource::ExecutionResource(SchedulerProxy * pSchedulerProxy, ExecutionResource * pParentExecutionResource)
        : m_pSchedulerProxy(pSchedulerProxy)
        , m_pParentExecutionResource(pParentExecutionResource)
        , m_pVirtualProcessorRoot(NULL)
        , m_tlsResetValue(0)
        , m_nodeId(pParentExecutionResource->GetNodeId())
        , m_coreIndex(pParentExecutionResource->GetCoreIndex())
        , m_numThreadSubscriptions(0)
    {
        m_executionResourceId = pParentExecutionResource->GetExecutionResourceId();
    }


    /// <summary>
    ///     Called to indicate that a scheduler is done with an execution resource and wishes to return it to the resource manager.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler making the request to remove this execution resource.
    /// </param>
    void ExecutionResource::Remove(IScheduler *pScheduler)
    {
        if (pScheduler == NULL)
        {
            throw std::invalid_argument("pScheduler");
        }

        // Remove must be called on the same thread that called SubscribeCurrentThread.
        ExecutionResource * pExecutionResource = m_pSchedulerProxy->GetCurrentThreadExecutionResource();
        if (pExecutionResource != this)
        {
            throw invalid_operation();
        }

        // The scheduler proxy should match the scheduler calling remove.
        if (m_pSchedulerProxy->Scheduler() != pScheduler)
        {
            throw invalid_operation();
        }

        m_pSchedulerProxy->GetResourceManager()->RemoveExecutionResource(this);
    }

    /// <summary>
    ///     Set this execution resource as current on this thread
    /// </summary>
    void ExecutionResource::SetAsCurrent()
    {
        // Save the information about this execution resource in the TLS for nested SubscribeCurrentThread calls.
        DWORD tlsSlot = m_pSchedulerProxy->GetResourceManager()->GetExecutionResourceTls();
        m_tlsResetValue = (size_t) platform::__TlsGetValue(tlsSlot);
        ASSERT((void *) m_tlsResetValue != (void *)this);
        platform::__TlsSetValue(tlsSlot, this);
    }

    /// <summary>
    ///     Clear the current execution resource on this thread.
    /// </summary>
    void ExecutionResource::ResetCurrent()
    {
        DWORD tlsSlot = m_pSchedulerProxy->GetResourceManager()->GetExecutionResourceTls();
        platform::__TlsSetValue(tlsSlot, (void *) m_tlsResetValue);
        m_tlsResetValue = 0;
    }

    /// <summary>
    ///     Increments the number of external threads that run on this execution resource as well as
    ///     the number of fixed threads that are running on the underlying core.
    /// </summary>
    /// <remarks>
    ///     This information is used to validate matching SubscribeCurrentThread/Release calls, as well as
    ///     to mark a core on which this resource runs as fixed (not-movable).
    /// </remarks>
    void ExecutionResource::IncrementUseCounts()
    {
        // The RM LOCK needs to be held before calling this routine

        if (m_numThreadSubscriptions++ == 0)
        {
            // For an execution resources associated with a vproc, the threadsubscription count is expected to
            // go from 0 to 1 when a context running on that vproc subscribes a thread to the scheduler it is running
            // on or a different scheduler.

            // For an execution resource *not* associated with a vproc, the threadsubscription count is expected to
            // go from 0 to 1 when it is created.
            bool isVPRoot = (m_pVirtualProcessorRoot != NULL);

            if (m_pParentExecutionResource == NULL)
            {
                // Mark on the core that this execution resource has added a new reference
                m_pSchedulerProxy->IncrementFixedCoreCount(m_nodeId, m_coreIndex, !isVPRoot);

                if (!isVPRoot)
                {
                    // Save old affinity
                    HANDLE hThread = GetCurrentThread();
                    m_oldAffinity = HardwareAffinity(hThread);

                    // Affinitize this thread to a given node
                    HardwareAffinity newAffinity = m_pSchedulerProxy->GetNodeAffinity(m_nodeId);
                    newAffinity.ApplyTo(hThread);

                    m_pSchedulerProxy->IncrementCoreSubscription(this);
                    m_pSchedulerProxy->AddExecutionResource(this);
                }
            }
            else
            {
                ASSERT(!isVPRoot);
                m_pSchedulerProxy->AddThreadSubscription(this);
            }

            SetAsCurrent();
        }
    }

    /// <summary>
    ///     Called to update the proxy counts, which must be done under the RM lock.
    /// </summary>
    void ExecutionResource::DecrementUseCounts()
    {
        // The RM LOCK needs to be held before calling this routine
        ASSERT(m_numThreadSubscriptions > 0);

        // This particular call does not have to worry about the RM receiving a SchedulerShutdown for the scheduler proxy in question.
        if (--m_numThreadSubscriptions == 0)
        {
            bool isVPRoot = (m_pVirtualProcessorRoot != NULL);
            // Reset the TLS to the previous state.
            // The previous state could be either NULL (if this was not an external threads first subscription),
            // a pointer to a thread proxy (it if was originally a vproc), or a parent execution resource (if this
            // was a nested execution resource).
            ResetCurrent();

            if (m_pParentExecutionResource == NULL)
            {
                // Mark on the core that this execution resource has removed one of its references
                m_pSchedulerProxy->DecrementFixedCoreCount(m_nodeId, m_coreIndex, !isVPRoot);

                if (!isVPRoot)
                {
                    m_oldAffinity.ApplyTo(GetCurrentThread());
                    m_pSchedulerProxy->DecrementCoreSubscription(this);
                    m_pSchedulerProxy->DestroyExecutionResource(this);
                }
            }
            else
            {
                ASSERT(!isVPRoot);
                m_pParentExecutionResource->DecrementUseCounts();
                m_pSchedulerProxy->RemoveThreadSubscription(this);
            }
        }
    }

    /// <summary>
    ///     Returns the subscription level on the core that this execution resource represents
    /// </summary>
    /// <returns>
    ///     A current subscription level of the underlying execution resource.
    /// </returns>
    unsigned int ExecutionResource::CurrentSubscriptionLevel() const
    {
        return m_pSchedulerProxy->GetResourceManager()->CurrentSubscriptionLevel(m_nodeId, m_coreIndex);
    }
} // namespace details
} // namespace Concurrency
