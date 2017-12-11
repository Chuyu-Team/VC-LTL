// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ExecutionResource.h
//
// Part of the ConcRT Resource Manager -- this header file contains the internal definition for the
// execution resource.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    #pragma warning(push)
    #pragma warning(disable: 4265) // non-virtual destructor in base class
    /// <summary>
    ///     An abstraction for an execution resource -- an entity on top of which a single thread of execution (of whatever
    ///     type) runs.
    /// </summary>
    class ExecutionResource final : public IExecutionResource
    {
    public:

        /// <summary>
        ///     Constructs a new execution resource.
        /// </summary>
        /// <param name="pSchedulerProxy">
        ///     The scheduler proxy this resource is created for. A scheduler proxy holds RM data associated with an instance of
        ///     a scheduler.
        /// </param>
        /// <param name="pNode">
        ///     The processor node that this resource belongs to. The processor node is one among the nodes allocated to the
        ///     scheduler proxy.
        /// </param>
        /// <param name="coreIndex">
        ///     The index into the array of cores for the processor node specified.
        /// </param>
        ExecutionResource(SchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex);

        /// <summary>
        ///     Constructs a new execution resource.
        /// </summary>
        /// <param name="pSchedulerProxy">
        ///     The scheduler proxy this resource is created for. A scheduler proxy holds RM data associated with an instance of
        ///     a scheduler.
        /// <param name="pParentExecutionResource">
        ///     The parent execution resource representing this thread
        ///     scheduler proxy.
        /// </param>
        ExecutionResource(SchedulerProxy * pSchedulerProxy, ExecutionResource * pParentExecutionResource);

        /// <summary>
        ///     Destroys an execution resource.
        /// </summary>
        ~ExecutionResource()
        {
            ASSERT(m_numThreadSubscriptions == 0);
        }

        /// <summary>
        ///     Returns a unique identifier for the node that the given execution resource belongs to.  The identifier returned
        ///     will fall in the range [0, nodeCount] where nodeCount is the value returned from Concurrency::GetProcessorNodeCount.
        /// </summary>
        virtual unsigned int GetNodeId() const
        {
            return m_nodeId;
        }

        /// <summary>
        ///     Returns a unique identifier for the execution resource that this execution resource runs atop.
        /// </summary>
        virtual unsigned int GetExecutionResourceId() const
        {
            return m_executionResourceId;
        }

        /// <summary>
        ///     Called to indicate that a scheduler is done with an execution resource and wishes to return it to the resource manager.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler making the request to remove this execution resource.
        /// </param>
        virtual void Remove(IScheduler *pScheduler);

        /// <summary>
        ///     Returns the subscription level on the core that this execution resource represents
        /// </summary>
        /// <returns>
        ///     A current subscription level of the underlying execution resource.
        /// </returns>
        virtual unsigned int CurrentSubscriptionLevel() const;

        // **************************************************
        // Internal
        // **************************************************

        /// <summary>
        ///     Returns a pointer to the scheduler proxy this execution resource was created by.
        /// </summary>
        SchedulerProxy * GetSchedulerProxy()
        {
            return m_pSchedulerProxy;
        }

        /// <summary>
        ///     Returns the core index into the array of cores, for the node that this execution resource is part of.
        /// </summary>
        unsigned int GetCoreIndex()
        {
            return m_coreIndex;
        }

        /// <summary>
        ///     Retrieves a virtual processor root that contains this execution resource, if any.
        /// </summary>
        VirtualProcessorRoot * GetVirtualProcessorRoot()
        {
            return m_pVirtualProcessorRoot;
        }

        /// <summary>
        ///     Set this execution resource as current on this thread
        /// </summary>
        void SetAsCurrent();

        /// <summary>
        ///     Clear the current execution resource on this thread.
        /// </summary>
        void ResetCurrent();

        /// <summary>
        ///     Initializes the execution resource as either standalone or belonging to virtual processor root.
        /// </summary>
        void MarkAsVirtualProcessorRoot(VirtualProcessorRoot * pVPRoot)
        {
            ASSERT(m_pVirtualProcessorRoot == NULL);
            m_pVirtualProcessorRoot = pVPRoot;
        }

        /// <summary>
        ///     Increments the number of external threads that run on this execution resource as well as
        ///     the number of fixed threads that are running on the underlying core.
        /// </summary>
        /// <remarks>
        ///     This information is used to validate matching SubscribeCurrentThread/Release calls, as well as
        ///     to mark a core on which this resource runs as fixed (not-movable).
        /// </remarks>
        void IncrementUseCounts();

        /// <summary>
        ///     Called to update the crucial counts, which must be done under the RM lock.
        /// </summary>
        void DecrementUseCounts();

    protected:

        // Guards critical regions of the Execution Resource
        _NonReentrantLock m_lock;

        // The previous affinity of the external thread
        HardwareAffinity m_oldAffinity;

        // The scheduler proxy associated with the scheduler for which
        // this resource was created.
        SchedulerProxy * m_pSchedulerProxy;

        // Parent execution resource in the case of a nested subscribe
        ExecutionResource * m_pParentExecutionResource;

        // Virtual processor root that this execution resource is a part of, if any
        VirtualProcessorRoot * m_pVirtualProcessorRoot;

        // The value to use when external resource subscription of a virtual processor is removed
        size_t m_tlsResetValue;

        // The node to which this execution resource belongs.
        unsigned int m_nodeId;

        // The core index within this node.
        unsigned int m_coreIndex;

        // The hardware thread upon which this execution resource executes.
        unsigned int m_executionResourceId;

        // Number of subscription requests that have been received for this execution resource.
        unsigned int m_numThreadSubscriptions;

    private:
        template <class T, class Counter> friend class List;

        // Intrusive links
        ExecutionResource * m_pPrev, * m_pNext;
    };

    #pragma warning(pop)
} // namespace details
} // namespace Concurrency
