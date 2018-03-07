// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerProxy.h
//
// RM proxy for a scheduler instance
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    #pragma warning(push)
    #pragma warning(disable: 4265) // non-virtual destructor in base class
    class SchedulerProxy : public ::Concurrency::ISchedulerProxy
    {
    public:
        /// <summary>
        ///     Constructs a scheduler proxy.
        /// </summary>
        SchedulerProxy(IScheduler * pScheduler, ResourceManager * pResourceManager, const SchedulerPolicy &policy);

        /// <summary>
        ///     Called in order to notify the resource manager that the given scheduler is shutting down.  This
        ///     will cause the resource manager to immediately reclaim all resources granted to the scheduler.
        /// </summary>
        virtual void Shutdown();

        /// <summary>
        ///     Called by a scheduler in order make an initial request for an allocation of virtual processors.  The request
        ///     is driven by policies within the scheduler queried via the IScheduler::GetPolicy method.  If the request
        ///     can be satisfied via the rules of allocation, it is communicated to the scheduler as a call to
        ///     IScheduler::AddVirtualProcessors.
        /// </summary>
        /// <param name="doSubscribeCurrentThread">
        ///     Whether to subscribe the current thread and account for it during resource allocation.
        /// </param>
        /// <returns>
        ///     The IExecutionResource instance representing current thread if doSubscribeCurrentThread was true; NULL otherwise.
        /// </returns>
        virtual IExecutionResource * RequestInitialVirtualProcessors(bool doSubscribeCurrentThread);

        /// <summary>
        ///     Ensures that a context is bound to a thread proxy.  This API should *NOT* be called in the vast majority of circumstances.
        ///     The IThreadProxy::SwitchTo will perform late binding to thread proxies as necessary.  There are, however, circumstances
        ///     where it is necessary to pre-bind a context to ensure that the SwitchTo operation switches to an already bound context.  This
        ///     is the case on a UMS scheduling context as it cannot call allocation APIs.
        /// </summary>
        /// <param name="pContext">
        ///     The context to bind.
        /// </param>
        virtual void BindContext(IExecutionContext * pContext);

        /// <summary>
        ///     Returns an **unstarted** thread proxy attached to pContext, to the thread proxy factory.
        ///     Such a thread proxy **must** be unstarted.
        ///     This API should *NOT* be called in the vast majority of circumstances.
        /// </summary>
        /// <param name="pContext">
        ///     The context to unbind.
        /// </param>
        virtual void UnbindContext(IExecutionContext * pContext);

        /// <summary>
        ///     This API registers the current thread with the resource manager associating it with this scheduler,
        ///     and returns an instance of IExecutionResource back to the scheduler, for bookkeeping and maintenance.
        /// </summary>
        /// <returns>
        ///     The IExecutionResource instance representing current thread in the runtime.
        /// </returns>
        virtual IExecutionResource * SubscribeCurrentThread();

        /// <summary>
        ///     The unique identifier of the scheduler this proxy represents.
        /// </summary>
        unsigned int GetId() const
        {
            return m_id;
        }

        /// <summary>
        ///     Causes the resource manager to create a new virtual processor root running atop the same hardware thread as this
        ///     execution resource. Typically, this is used when a scheduler wishes to oversubscribe a particular hardware thread
        ///     for a limited amount of time.
        /// </summary>
        /// <param name="pExecutionResource">
        ///     The execution resource abstraction on which to oversubscribe.
        /// </param>
        /// <returns>
        ///     A new virtual processor root running atop the same hardware thread as this execution resource.
        /// </returns>
        virtual IVirtualProcessorRoot * CreateOversubscriber(IExecutionResource * pExecutionResource);

        /// <summary>
        ///     Getters for the various policy elements.
        /// </summary>
        unsigned int MaxConcurrency() const
        {
            return m_maxConcurrency;
        }
        unsigned int MinConcurrency() const
        {
            return m_minConcurrency;
        }
        unsigned int TargetOversubscriptionFactor() const
        {
            return m_targetOversubscriptionFactor;
        }
        int ContextStackSize () const
        {
            return m_contextStackSize;
        }
        int ContextPriority () const
        {
            return m_contextPriority;
        }

        /// <summary>
        ///     Returns the minimum number of cores that must contain vprocs for this scheduler. These cores
        ///     may contain a subscribed thread in addition to virtual processors.
        /// </summary>
        unsigned int MinVprocHWThreads() const
        {
            // Compute number of cores used for virtual processors that are fixed
            ASSERT(m_numFixedCores >= m_numExternalThreadCores);
            unsigned int fixedVprocCores = m_numFixedCores - m_numExternalThreadCores;
            // Compute maximum(t1, minimum set by policy) which is minimum of virtual processor cores
            return max(fixedVprocCores, m_minimumHardwareThreads);
        }

        unsigned int MinHWThreads() const
        {
            // The minimum needed number of hardware threads (cores) is equal to:
            //  - minimum needed vproc cores + minimum needed external thread cores
            unsigned int minimumCores = MinVprocHWThreads() + m_numExternalThreadCores;

            ASSERT(minimumCores <= m_coreCount);
            return minimumCores;
        }

        unsigned int DesiredHWThreads() const
        {
            unsigned int desiredCores = min(m_coreCount, m_desiredHardwareThreads + m_numExternalThreadCores);

            ASSERT(m_numExternalThreads != 0 || desiredCores == m_desiredHardwareThreads);
            return desiredCores;
        }

        unsigned int ComputeMinHWThreadsWithExternalThread() const
        {
            unsigned int newMin = min(m_coreCount, MinHWThreads() + 1);
            return newMin;
        }

        unsigned int ComputeDesiredHWThreadsWithExternalThread() const
        {
            unsigned int newDesired = min(m_coreCount, DesiredHWThreads() + 1);
            return newDesired;
        }

        /// <summary>
        ///     Returns the number of external thread subscriptions
        /// </summary>
        unsigned int GetNumNestedThreadSubscriptions()
        {
            return m_threadSubscriptions.Count();
        }

        /// <summary>
        ///     Called to adjust the suggested allocation such that we do not exceed maxConcurrency.
        ///     This routine takes into account vprocs that are marked for removal but haven't yet been
        ///     retired by the scheduler. The suggested allocation would be decreased to account for such
        ///     vprocs.
        /// </summary>
        unsigned int AdjustAllocationIncrease(unsigned int suggestedAllocation) const;

        /// <summary>
        ///     Returns the number of cores allocated to the proxy at any time.
        /// </summary>
        unsigned int GetNumAllocatedCores() const
        {
            return m_numAllocatedCores;
        }

        /// <summary>
        ///     Returns the number of borrowed cores. These are cores that were oversubscribed and temporarily
        ///     assigned to this scheduler during dynamic core migration as they were found to be unused
        ///     by the other scheduler(s) they were assigned to. The reason these cores were oversubscribed
        ///     instead of migrated was that they contributed to the minimum number of cores on the other
        ///     scheduler(s) and hence couldn't be taken away.
        /// </summary>
        unsigned int GetNumBorrowedCores() const
        {
            return m_numBorrowedCores;
        }

        /// <summary>
        ///     Returns the number of owned cores. This is the total allocated cores minus the borrowed cores.
        /// </summary>
        unsigned int GetNumOwnedCores() const
        {
            return m_numAllocatedCores - m_numBorrowedCores;
        }

        /// <summary>
        ///     Returns the number of fixed cores - cores that have a subscribed thread on them. These cores may
        ///     also have vprocs belonging to this scheduler.
        /// </summary>
        unsigned int GetNumFixedCores() const
        {
            return m_numFixedCores;
        }

        /// <summary>
        ///     Toggles the state on a core from borrowed to owned (and vice versa), and updates necessary counts.
        /// </summary>
        void ToggleBorrowedState(SchedulerNode * pNode, unsigned int coreIndex);

        /// <summary>
        ///     Creates a new execution resource for the external thread and registers it with the scheduler proxy.
        /// </summary>
        ExecutionResource * CreateExternalThreadResource(SchedulerNode * pNode, unsigned int coreIndex);

        /// <summary>
        ///     Called by the RM when it is done reserving cores for the scheduler proxy. The scheduler proxy
        ///     allocates virtual processors or standalone execution resources based on the cores that were allocated
        ///     to it.
        /// </summary>
        ExecutionResource * GrantAllocation(unsigned int numberReserved, bool fInitialAllocation, bool fSubscribeCurrentThread);

        /// <summary>
        ///     Finds the core allocated by the RM on which a single subscribed external thread should run.
        /// </summary>
        ExecutionResource * GrantExternalThreadAllocation(bool doOversubscribeCore);

        /// <summary>
        ///     Returns a pointer to the copy of allocated nodes that were assigned to the proxy at
        ///     creation time.
        /// </summary>
        SchedulerNode * GetAllocatedNodes() const
        {
            return m_pAllocatedNodes;
        }

        /// <summary>
        ///     Sets the allocated nodes for the scheduler proxy to the nodes provided.
        /// </summary>
        void SetAllocatedNodes(SchedulerNode * pNodes)
        {
            ASSERT(m_pAllocatedNodes == NULL && pNodes != NULL);
            m_pAllocatedNodes = pNodes;
        }
        /// <summary>
        ///     Returns a pointer to the array that holds the sorted order for nodes. This is used by the
        ///     RM to sort nodes by whatever criteria it chooses.
        /// </summary>
        unsigned int * GetSortedNodeOrder() const
        {
            return m_pSortedNodeOrder;
        }

        /// <summary>
        ///     Returns a pointer to the scheduler associated with the sheduler proxy.
        /// </summary>
        IScheduler * Scheduler() const
        {
            return m_pScheduler;
        }

        /// <summary>
        ///     Returns a pointer to the resource manager associated with the scheduler proxy.
        /// </summary>
        ResourceManager * GetResourceManager() const
        {
            return m_pResourceManager;
        }

        /// <summary>
        ///     Returns a pointer to a data buffer that is used to store static allocation data. The data
        ///     is populated and manipulated by the RM, but stored in the scheduler proxy for convenience.
        /// </summary>
        StaticAllocationData * GetStaticAllocationData()
        {
            return &m_staticData;
        }

        /// <summary>
        ///     Returns a pointer to a data buffer that is used to store dynamic allocation data. The data
        ///     is populated and manipulated by the RM, but stored in the scheduler proxy for convenience.
        /// </summary>
        DynamicAllocationData * GetDynamicAllocationData()
        {
            return &m_dynamicData;
        }

        /// <summary>
        ///     Creates a virtual processor root and adds it to the scheduler proxys list of roots.
        /// </summary>
        virtual VirtualProcessorRoot * CreateVirtualProcessorRoot(SchedulerNode * pNode, unsigned int coreIndex);

        /// <summary>
        ///     Notifies the scheduler associated with this proxy to add the virtual processor roots provided.
        ///     Called by the RM during initial allocation and dynamic core migration.
        /// </summary>
        void AddVirtualProcessorRoots(IVirtualProcessorRoot ** vprocRoots, unsigned int count);

        /// <summary>
        ///     Adds an appropriate number of virtual processor roots to the scheduler associated with this proxy.
        ///     Called by the RM during core migration when the RM decides to give this scheduler an additional
        ///     core.
        /// </summary>
        void AddCore(SchedulerNode * pNode, unsigned int coreIndex, bool fBorrowed);

        /// <summary>
        ///     Notifies the scheduler associated with this proxy to remove the virtual processor roots associated
        ///     with the core provided. Called by the RM during core migration.
        /// </summary>
        void RemoveCore(SchedulerNode * pNode, unsigned int coreIndex);

        /// <summary>
        ///     Called by the RM to instruct this scheduler proxy to notify its scheduler that this core is now
        ///     externally busy or externally idle.
        /// </summary>
        void SendCoreNotification(SchedulerCore * pCore, bool isBusyNotification);

        /// <summary>
        ///     Removes a root from the scheduler proxy and destroys it. This API is called in response to a scheduler
        ///     informing the RM that it is done with a virtual processor root.
        /// </summary>
        void DestroyVirtualProcessorRoot(VirtualProcessorRoot * pRoot);

        /// <summary>
        ///     Removes an execution resource from the scheduler proxy and destroys it. This API is called in response to a scheduler
        ///     informing the RM that it is done with an execution resource.
        /// </summary>
        void DestroyExecutionResource(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Returns a hardware affinity for the given node.  Note that a scheduler proxy may only be assigned a subset
        ///     of cores within a node -> the mask in the affinity reflects this subset.
        /// </summary>
        /// <returns>
        ///     An abstraction of the hardware affinity which can be applied to Win32 objects.
        /// </returns>
        HardwareAffinity GetNodeAffinity(unsigned int nodeId)
        {
            ASSERT(nodeId < m_nodeCount);
            ASSERT(m_pAllocatedNodes[nodeId].m_id == nodeId);

            return HardwareAffinity(static_cast<USHORT>(m_pAllocatedNodes[nodeId].m_processorGroup), m_pAllocatedNodes[nodeId].m_nodeAffinity);
        }

        /// <summary>
        ///     Adds an execution resource to the list of resources that run on a particular core.
        /// </summary>
        void AddExecutionResource(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Adds the execution resource to the list of subscribed threads
        /// </summary>
        void AddThreadSubscription(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Removes the execution resource from the list of subscribed threads
        /// </summary>
        void RemoveThreadSubscription(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Creates or reuses an execution resource for the thread subscription
        /// </summary>
        ExecutionResource * GetResourceForNewSubscription(ExecutionResource * pParentExecutionResource);

        /// <summary>
        ///     This function retrieves the execution resource associated with this thread, if one exists,
        ///     and updates the reference count on it for better bookkeeping.
        /// </summary>
        /// <returns>
        ///     The ExecutionResource instance representing current thread in the runtime.
        /// </returns>
        ExecutionResource * ReferenceCurrentThreadExecutionResource();

        /// <summary>
        ///     This function retrieves the execution resource associated with this thread, if one exists.
        /// </summary>
        /// <returns>
        ///     The ExecutionResource instance representing current thread in the runtime.
        /// </returns>
        ExecutionResource * GetCurrentThreadExecutionResource();

        /// <summary>
        ///     Registers that a call to SubscribeCurrentThread has occured for this core, making this core immovable.
        /// </summary>
        void IncrementFixedCoreCount(unsigned int nodeId, unsigned int coreIndex, bool isExternalThread);

        /// <summary>
        ///     Registers that a call to IExecutionResource::Release has occured, potentially freeing this core.
        /// </summary>
        void DecrementFixedCoreCount(unsigned int nodeId, unsigned int coreIndex, bool isExternalThread);

        /// <summary>
        ///     Returns the number of external threads on this scheduler proxy.
        /// </summary>
        unsigned int GetNumExternalThreads()
        {
            return m_numExternalThreads;
        }

        /// <summary>
        ///     Decides whether this scheduler proxy should receive notifications when other
        ///     schedulers borrow its cores or return them back.
        /// </summary>
        bool ShouldReceiveNotifications()
        {
            return (m_minimumHardwareThreads == m_desiredHardwareThreads);
        }

        /// <summary>
        ///     A function that passes statistical information to the hill climbing instance. Based on these
        ///     statistics, hill climbing will make a recommendation on the number of resources the scheduler
        ///     should be allocated.
        /// </summary>
        /// <param name="currentCoreCount">
        ///     The number of resources used in this period of time.
        /// </param>
        /// <param name="completionRate">
        ///     The number of completed units or work in that period of time.
        /// </param>
        /// <param name="arrivalRate">
        ///     The number of incoming units or work in that period of time.
        /// </param>
        /// <param name="queueLength">
        ///     The total length of the work queue.
        /// </param>
        /// <returns>
        ///     The recommended allocation for the scheduler.
        /// </returns>
        unsigned int DoHillClimbing(unsigned int currentCoreCount, unsigned int completionRate, unsigned int arrivalRate, unsigned int queueLength)
        {
            return m_pHillClimbing->Update(currentCoreCount, completionRate, arrivalRate, queueLength);
        }

        /// <summary>
        ///     This function returns whether the scheduler has opted in to statistical rebalancing.
        /// </summary>
        /// <returns>
        ///     Whether hill climbing is enabled.
        /// </returns>
        bool IsHillClimbingEnabled()
        {
            return m_fDoHillClimbing;
        }

        /// <summary>
        ///     Gets the current length of the scheduler queue.
        /// </summary>
        /// <returns>
        ///     The queue length.
        /// </returns>
        unsigned int GetQueueLength()
        {
            return m_queueLength;
        }

        /// <summary>
        ///     Sets the current length of the scheduler queue.
        /// </summary>
        /// <param name="queueLength">
        ///     The length to be set.
        /// </param>
        void SetQueueLength(unsigned int queueLength)
        {
            m_queueLength = queueLength;
        }

        /// <summary>
        ///     Gets a new thread proxy from the factory.
        /// </summary>
        virtual IThreadProxy * GetNewThreadProxy(IExecutionContext * pContext);

        /// <summary>
        ///     Called to shutdown a scheduler proxy.  Derived classes can override shutdown behavior based on this.
        /// </summary>
        virtual void FinalShutdown();

        /// <summary>
        ///     Called to assist dynamic resourcemanagement in determining whether cores assigned to schedulers
        ///     are idle. An idle core is one whose subscription level is 0.
        /// </summary>
        void IncrementCoreSubscription(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Called to assist dynamic resourcemanagement in determining whether cores assigned to schedulers
        ///     are idle. An idle core is one whose subscription level is 0.
        /// </summary>
        void DecrementCoreSubscription(ExecutionResource * pExecutionResource);

#if defined(CONCRT_TRACING)
        /// <summary>
        ///     Captures the initial state of the scheduler map at the beginning of core migration, each cycle.
        /// </summary>
        void TraceInitialDRMState();
#endif

protected:

        /// <summary>
        ///     Deletes the scheduler proxy.
        /// </summary>
        virtual void DeleteThis()
        {
            delete this;
        }

        /// <summary>
        ///     Cleans up resources associated with the scheduler.
        /// </summary>
        void Cleanup();

        /// <summary>
        ///     Destructor.
        /// </summary>
        ~SchedulerProxy();

        // A cached pointer to a thread proxy factory of the appropriate type for this scheduler proxy.
        IThreadProxyFactory * m_pThreadProxyFactory;

    private:
        template <class T, class Counter> friend class List;

#if defined(CONCRT_TRACING)

        struct SchedulerCoreData
        {
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            bool m_fAllocated : 1;
            bool m_fFixed : 1;
            bool m_fBorrowed : 1;
            bool m_fIdle : 1;
        };

        // Captures the initial global allocation during the DRM phase.
        SchedulerCoreData * m_drmInitialState;
        unsigned int m_numTotalCores;

#endif
        IScheduler * m_pScheduler;

        // Pointer to the resource manager instance.
        ResourceManager * m_pResourceManager;

        // Local copy of allocation map for this scheduler proxy.
        SchedulerNode * m_pAllocatedNodes;

        // Helper array used to sort nodes, used by the RM during core migration.
        unsigned int * m_pSortedNodeOrder;

        // Links for a list.
        SchedulerProxy * m_pNext, * m_pPrev;

        // A lock that protects resource allocation and deallocation of roots within this proxy.
        _ReentrantBlockingLock m_lock;

        // Hill climbing instance.
        HillClimbing * m_pHillClimbing;

        // Static and dynamic allocation data is populated and manipulated by the RM, but
        // stored in the scheduler proxy for convenience.
        union
        {
            // Data used during static allocation.
            StaticAllocationData m_staticData;

            // Data used during dynamic allocation.
            DynamicAllocationData m_dynamicData;
        };

        // Scheduler queue length.
        unsigned int m_queueLength;

        // Unique identifier.
        unsigned int m_id;

        // Variables that store policy elements.
        unsigned int m_desiredHardwareThreads;
        unsigned int m_minimumHardwareThreads;
        unsigned int m_minConcurrency;
        unsigned int m_maxConcurrency;
        unsigned int m_targetOversubscriptionFactor;
        int m_contextStackSize;
        int m_contextPriority;

        // Current concurrency level (number of vproc roots). This includes vproc roots
        // that are marked for removal but has not yet been destroyed by the scheduler.
        // Protected by the scheduler proxy lock
        unsigned int m_currentConcurrency;

        // The number of cores allocated to this scheduler proxy.
        unsigned int m_numAllocatedCores;

        // At any time this has the number of additional cores that can be allocated with m_tof threads.
        // When this falls to 0, all remaining allocated cores will get m_tof - 1 threads, to ensure that
        // we don't go over max concurrency threads.
        unsigned int m_numFullySubscribedCores;

        // The number of allocated cores that are borrowed. An borrowed core is a core that is assigned to
        // one or more different schedulers, but was found to be idle. The RM temporarily assigns idle resources to
        // schedulers that need them.
        unsigned int m_numBorrowedCores;

        // The number of cores that have a subscribed thread on them. These cores are 'fixed' in that they cannot
        // be removed by static/dynamic allocations, as long as the subscribed thread is present on them.
        unsigned int m_numFixedCores;

        // The number of virtual processors (threads) that were added to the related scheduler via initial
        // allocation or core migration. Does not include oversubscribed virtual processors.
        unsigned int m_numAssignedThreads;

        // The number of external threads that were added to the related scheduler via external subscription calls.
        unsigned int m_numExternalThreads;

        // The number of cores that external threads occupy exclusively.
        unsigned int m_numExternalThreadCores;

        // The number of hardware threads available on this machine.
        unsigned int m_coreCount;

        // Number of nodes in the allocated nodes array.
        unsigned int m_nodeCount;

        // List of execution resources representing subscribed threads
        List<ExecutionResource, CollectionTypes::Count> m_threadSubscriptions;

        // Used to determine whether statistical rebalancing is used for this scheduler proxy.
        bool m_fDoHillClimbing;
    };

    #pragma warning(pop)
} // namespace details
} // namespace Concurrency
