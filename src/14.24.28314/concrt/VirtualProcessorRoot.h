// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessorRoot.h
//
// Part of the ConcRT Resource Manager -- this header file contains the internal definition for the base virtual
// processor root.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     An abstraction for a virtual processor -- an entity on top of which a single thread of execution (of whatever
    ///     type) runs.  Note that there are specific derived classes for free and bound virtual processor roots -- necessary
    ///     so as to easily hand out different types of thread proxies and message thread proxies upon Activate for bound
    ///     schedulers, etc...
    /// </summary>
    class VirtualProcessorRoot : public IVirtualProcessorRoot
    {
    public:

        /// <summary>
        ///     Constructs a new virtual processor root.
        /// </summary>
        /// <param name="pSchedulerProxy">
        ///     The scheduler proxy this root is created for. A scheduler proxy holds RM data associated with an instance of
        ///     a scheduler.
        /// </param>
        /// <param name="pNode">
        ///     The processor node that this root belongs to. The processor node is one among the nodes allocated to the
        ///     scheduler proxy.
        /// </param>
        /// <param name="coreIndex">
        ///     The index into the array of cores for the processor node specified.
        /// </param>
        VirtualProcessorRoot(SchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex);

        /// <summary>
        ///     Destroys a virtual processor root.
        /// </summary>
        virtual ~VirtualProcessorRoot()
        {
        }

        /// <summary>
        ///     Returns a unique identifier for the virtual processor root.
        /// </summary>
        virtual unsigned int GetId() const
        {
            return m_id;
        }

        /// <summary>
        ///     Returns a unique identifier for the node that the given virtual processor root belongs to.  The identifier returned
        ///     will fall in the range [0, nodeCount] where nodeCount is the value returned from Concurrency::GetProcessorNodeCount.
        /// </summary>
        virtual unsigned int GetNodeId() const
        {
            return m_executionResource.GetNodeId();
        }

        /// <summary>
        ///     Returns a unique identifier for the execution resource that this virtual processor root runs atop.
        /// </summary>
        virtual unsigned int GetExecutionResourceId() const
        {
            return m_executionResource.GetExecutionResourceId();
        }

        /// <summary>
        ///     Causes the scheduler to start running a thread proxy on the specified virtual processor root which will execute
        ///     the Dispatch method of the context supplied by pContext.
        /// </summary>
        /// <param name="pContext">
        ///     The context which will be dispatched on a (potentially) new thread running atop this virtual processor root.
        /// </param>
        virtual void Activate(::Concurrency::IExecutionContext *pContext) =0;

        /// <summary>
        ///     Causes the thread proxy running atop this virtual processor root to temporarily stop dispatching pContext.
        /// </summary>
        /// <param name="pContext">
        ///     The context which should temporarily stop being dispatched by the thread proxy running atop this virtual processor root.
        /// </param>
        virtual bool Deactivate(::Concurrency::IExecutionContext *pContext) =0;

        /// <summary>
        ///     Forces all data in the memory heirarchy of one processor to be visible to all other processors.
        /// </summary>
        /// <param name="pContext">
        ///     The context which is currently being dispatched by this root.
        /// </param>
        virtual void EnsureAllTasksVisible(::Concurrency::IExecutionContext *pContext) =0;

        /// <summary>
        ///     Called to indicate that a scheduler is done with a virtual processor root and wishes to return it to the resource manager.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler making the request to remove this virtual processor root.
        /// </param>
        virtual void Remove(IScheduler *pScheduler);

        /// <summary>
        ///     Returns a number of active virtual processors and external threads running on top of the execution resource
        ///     associated with this virtual processor root.
        /// </summary>
        /// <returns>
        ///     A current subscription level of the underlying execution resource.
        /// </returns>
        virtual unsigned int CurrentSubscriptionLevel() const
        {
            return m_executionResource.CurrentSubscriptionLevel();
        }

        // **************************************************
        // Internal
        // **************************************************

        /// <summary>
        ///     Returns a pointer to the scheduler proxy this virtual processor root was created by.
        /// </summary>
        SchedulerProxy * GetSchedulerProxy()
        {
            return m_executionResource.GetSchedulerProxy();
        }

        /// <summary>
        ///     Returns the core index into the array of cores, for the node that this virtual processor root is part of.
        /// </summary>
        int GetCoreIndex()
        {
            return m_executionResource.GetCoreIndex();
        }

        /// <summary>
        ///     Retrieves an underlying execution resource.
        /// </summary>
        ExecutionResource * GetExecutionResource()
        {
            return &m_executionResource;
        }

        /// <summary>
        ///     Helpers to prevent from removing a virtual processor root twice. We remove the root from the list in the allocated
        ///     nodes when the corresponding vprocroot::Remove call is made.
        /// </summary>
        bool IsRootRemoved() { return m_fRemovedFromScheduler; }
        void MarkRootRemoved() { m_fRemovedFromScheduler = true; }

        /// <summary>
        ///     Deletes the virtual processor.
        /// </summary>
        virtual void DeleteThis() =0;

        /// <summary>
        ///     Notifies the underlying RM that the core is subscribed by this root *if* necessary.
        /// </summary>
        void Subscribe();

        /// <summary>
        ///    Notifies the underlying RM that the core is not subscribed by this root *if* necessary.
        /// </summary>
        void Unsubscribe();

        /// <summary>
        ///     This API is called when a virtual processor root is being destroyed. It removes the effect of this virtual processor root
        ///     on the subscription level for the underlying core.
        /// <summary>
        void ResetSubscriptionLevel();

        /// <summary>
        ///     Sets the activated context to the specified value.
        /// </summary>
        void SetActivatedContext(::Concurrency::IExecutionContext *pContext)
        {
            //
            // The below need not be atomic but must have at least write release semantics.
            //
            InterlockedExchangePointer(reinterpret_cast<void *volatile*>(&m_pActivatedContext), pContext);
        }

        /// <summary>
        ///     Gets the activated context and simultaneously sets it to NULL.
        /// </summary>
        ::Concurrency::IExecutionContext *AcquireActivatedContext()
        {
            //
            // Once this is set, we are the only entity that will ever acquire/reset.  We simply need to wait for the set which may
            // be a few instructions behind.
            //
            _SpinWaitBackoffNone spinWait(_Sleep0);
            while (m_pActivatedContext == NULL)
            {
                spinWait._SpinOnce();
            }

            //
            // The only entity which will subsequently set this won't come until after this has happened and we woke up another thread.  This
            // need not be atomic or have barrier semantics.
            //
            ::Concurrency::IExecutionContext *pActivatedContext = m_pActivatedContext;
            m_pActivatedContext = NULL;

            return pActivatedContext;
        }

        /// <summary>
        ///     This API is called by the RM to indicate that this vproc does not contribute towards concurrency limits
        ///     set by the user
        /// <summary>
        void MarkAsOversubscribed()
        {
            m_fOversubscribed = true;
        }

        /// <summary>
        ///     Returns true if this is an oversubscribed vproc.
        /// <summary>
        bool IsOversubscribed()
        {
            return m_fOversubscribed;
        }

    protected:

        // Internal context that was last activated.  This is used to resolve activate/deactivate,idle races.
        IExecutionContext * volatile m_pActivatedContext;

        // Execution resource underneath this virtual processor root
        ExecutionResource m_executionResource;

        // Set to true when a RemoveVirtualProcessors call has been made on the corresponding scheduler interface for this
        // virtual processor root.
        bool m_fRemovedFromScheduler;

        // Flag to distinguish vprocs that were created through CreateOversubscribe() call.
        bool m_fOversubscribed;

        // The process unique identifier assigned to this virtual processor root.
        unsigned int m_id;

        // Fence used to avoid kernel transitions with activation/deactivation races.
        volatile LONG m_activationFence;

    private:

        // The current unique identifier being handed out to created virtual processor roots.
        static long s_currentId;
    };
} // namespace details
} // namespace Concurrency
