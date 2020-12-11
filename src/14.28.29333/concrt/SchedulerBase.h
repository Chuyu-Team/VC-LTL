// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerBase.h
//
// Header file containing the metaphor for a concrt scheduler
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

//
// Defines how many (1 << x) pointers worth of padding there will be in between quick cache slots.
//
#ifdef _WIN64
//
// 64 bit:
// 1 << 4 == 8 pointers * 8 == 64 bytes (assumed cache pad)
//
#define QUICKCACHEPAD_SHIFT 4
#else // !_WIN64
//
// 32 bit:
// 1 << 5 == 16 pointers * 4 == 64 bytes (assumed cache pad)
//
#define QUICKCACHEPAD_SHIFT 5
#endif // _WIN64

namespace Concurrency
{
namespace details
{
    // The base class that implements a scheduler instance

    class SchedulerBase : public Scheduler
    {
    private:

        //
        // NESTED CLASSES:
        //

        /// <summary>
        ///     Represents information about the NUMA nodes on the machine.
        /// </summary>
        struct NumaInformation
        {
            QuickBitSet m_nodeMask;
            QuickBitSet m_resourceMask;
        };

        ///<summary>
        ///     An intrusive node type for context tracking outside of the normal placement of contexts upon
        ///     free/runnable lists.
        ///</summary>
        class ContextNode
        {
        public:

            ContextNode(InternalContextBase *pContext) : m_pContext(pContext)
            {
            }

            SLIST_ENTRY m_slNext;
            InternalContextBase *m_pContext;
        };

        ///<summary>
        ///     A node that tracks events needing to be signaled at finalization time.
        ///</summary>
        class WaitNode
        {
        public:

            WaitNode *m_pNext, *m_pPrev;
            HANDLE m_hEvent;
        };

        ///<summary>
        ///     A class that the scheduler uses to manage external context exit events for implicitly attached
        ///     external contexts.
        ///</summary>
        class ContextExitEventHandler
        {
        public:

            bool m_fCanceled;
            // Count of handles the event handler is waiting on at any time.
            unsigned short m_handleCount;
            // Modified to reflect the new handle count after adding handles to the wait array and before notifying the
            // watch dog of handle addition.
            unsigned short m_newHandleCount;
            // Event handle used to notify the event handler of certain events (new handle addition, shutdown).
            HANDLE m_hWakeEventHandler;
            SchedulerBase *m_pScheduler;
            // prev, next pointers for the list of all handlers in the scheduler.
            ContextExitEventHandler *m_pNext, *m_pPrev;
            // list entry for a list of handlers with available slots for context handles. The scheduler uses this
            // list when registering contexts.
            ListEntry m_availableChain;
            // The array of wait handles each thread waits on. Of these one is an event handle for notification
            // and the rest are handles to OS contexts.
            HANDLE m_waitHandleArray[MAXIMUM_WAIT_OBJECTS];
        };

    public:

        /// <summary>
        ///     Creates a scheduler that only manages internal contexts. Implicitly calls Reference.
        ///     If Attach is called, the scheduler is no longer anonymous because it is also managing the external
        ///     context where Attach was called.  To destroy an anonymous scheduler, Release needs to be called.
        /// </summary>
        /// <param name="policy">
        ///     [in] A const reference to the scheduler policy.
        /// </param>
        /// <returns>
        ///     A pointer to the new scheduler (never null)
        /// </returns>
        static _Ret_writes_(1) SchedulerBase* Create(_In_ const SchedulerPolicy& policy);
        static _Ret_writes_(1) SchedulerBase* CreateWithoutInitializing(_In_ const SchedulerPolicy& policy);

        // Constructor
        SchedulerBase(_In_ const ::Concurrency::SchedulerPolicy& policy);

        // dtor
        virtual ~SchedulerBase();

    public:  // Public Scheduler interface
        /// <returns>
        ///     Returns a unique identifier for this scheduler.  No error state.
        /// </returns>
        virtual unsigned int Id() const { return m_id; }

        /// <returns>
        ///     Returns a current number of virtual processors for this scheduler.  No error state.
        /// </returns>
        virtual unsigned int GetNumberOfVirtualProcessors() const { return m_virtualProcessorCount; };

        /// <returns>
        ///     Returns a copy of the policy this scheduler is using.  No error state.
        /// </returns>
        virtual _Ret_writes_(1) SchedulerPolicy GetPolicy() const;

        /// <summary>
        ///     Increments a reference count to this scheduler to manage lifetimes over composition.</summary>
        ///     This reference count is known as the scheduler reference count.
        /// </summary>
        /// <returns>
        ///     The resulting reference count is returned.  No error state.
        /// </returns>
        virtual unsigned int Reference();

        /// <summary>
        ///     Decrements this scheduler's reference count to manage lifetimes over composition.
        ///     A scheduler starts the shutdown protocol when the scheduler reference count goes to zero.
        /// <summary>
        /// <returns>
        ///     The resulting reference count is returned.  No error state.
        /// </returns>
        virtual unsigned int Release();

        /// <summary>
        ///     Causes the OS event object 'event' to be set when the scheduler shuts down and destroys itself.
        /// </summary>
        /// <param name="event">
        ///     [in] A handle to avalid event object
        /// </param>
        virtual void RegisterShutdownEvent(_In_ HANDLE event);

        /// <summary>
        ///     Attaches this scheduler to the calling thread. Implicitly calls Reference.
        ///     After this function is called, the calling thread is then managed by the scheduler and the scheduler becomes the current scheduler.
        ///     It is illegal for an internal context to call Attach on its current scheduler.
        /// </summary>
        virtual void Attach();

        /// <summary>
        ///     Allows a user defined policy to be used to create the default scheduler. It is only valid to call this API when no default
        ///     scheduler exists. Once a default policy is set, it remains in effect until the next time the API is called (in the absense
        ///     of a default scheduler).
        /// </summary>
        /// <param name="pPolicy">
        ///     [in] A pointer to the policy to be set as the default. The runtime will make a copy of the policy
        ///     for its use, and the user is responsible for the lifetime of the policy that is passed in.
        /// </param>
        static void SetDefaultSchedulerPolicy(_In_ const SchedulerPolicy & _Policy);

        /// <summary>
        ///     Resets the default scheduler policy, and the next time a default scheduler is created, it will use the runtime's default policy settings.
        /// </summary>
        static void ResetDefaultSchedulerPolicy();

        /// <summary>
        ///     Creates a new schedule group within the scheduler associated with the calling context.
        /// </summary>
        /// <returns>
        ///     A pointer to the newly created schedule group.  This <c>ScheduleGroup</c> object has an initial reference count placed on it.
        /// </returns>
        /// <remarks>
        ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
        ///     scheduler currently associated with the calling context.
        ///     <para>You must invoke the <see cref="ScheduleGroup::Release Method">Release</see> method on a schedule group when you are
        ///     done scheduling work to it. The scheduler will destroy the schedule group when all work queued to it has completed.</para>
        ///     <para>Note that if you explicitly created this scheduler, you must release all references to schedule groups within it, before
        ///     you release your reference on the scheduler, via detaching the current context from it.</para>
        /// </remarks>
        /// <seealso cref="ScheduleGroup Class"/>
        /// <seealso cref="ScheduleGroup::Release Method"/>
        /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
        virtual ScheduleGroup* CreateScheduleGroup()
        {
            location unbiased;
            return InternalCreateScheduleGroup(&unbiased);
        }

        /// <summary>
        ///     Creates a new schedule group within the scheduler associated with the calling context.  Tasks scheduled within the newly created
        ///     schedule group will be biased towards executing at the specified location.
        /// </summary>
        /// <param name="_Placement">
        ///     A reference to a location where the tasks within the schedule group will biased towards executing at.
        /// </param>
        /// <returns>
        ///     A pointer to the newly created schedule group.  This <c>ScheduleGroup</c> object has an initial reference count placed on it.
        /// </returns>
        /// <remarks>
        ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
        ///     scheduler currently associated with the calling context.
        ///     <para>You must invoke the <see cref="ScheduleGroup::Release Method">Release</see> method on a schedule group when you are
        ///     done scheduling work to it. The scheduler will destroy the schedule group when all work queued to it has completed.</para>
        ///     <para>Note that if you explicitly created this scheduler, you must release all references to schedule groups within it, before
        ///     you release your reference on the scheduler, via detaching the current context from it.</para>
        /// </remarks>
        /// <seealso cref="ScheduleGroup Class"/>
        /// <seealso cref="ScheduleGroup::Release Method"/>
        /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
        /// <seealso cref="location Class"/>
        virtual ScheduleGroup * CreateScheduleGroup(location& _Placement)
        {
            return InternalCreateScheduleGroup(&_Placement);
        }

        /// <summary>
        ///     Schedules a light-weight task within the scheduler.  The light-weight task will be placed in a schedule group of the runtime's choosing.
        /// </summary>
        /// <param name="proc">
        ///     A pointer to the function to execute to perform the body of the light-weight task.
        /// </param>
        /// <param name="data">
        ///     A void pointer to the data that will be passed as a parameter to the body of the task.
        /// </param>
        /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
        /// <seealso cref="ScheduleGroup Class"/>
        virtual void ScheduleTask(TaskProc proc, void *data);

        /// <summary>
        ///     Schedules a light-weight task within the scheduler.  The light-weight task will be placed
        ///     within a schedule group of the runtime's choosing.  It will also be biased towards executing at the specified location.
        /// </summary>
        /// <param name="proc">
        ///     A pointer to the function to execute to perform the body of the light-weight task.
        /// </param>
        /// <param name="data">
        ///     A void pointer to the data that will be passed as a parameter to the body of the task.
        /// </param>
        /// <param name="placement">
        ///     A reference to a location where the light-weight task will be biased towards executing at.
        /// </param>
        /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
        /// <seealso cref="ScheduleGroup Class"/>
        /// <seealso cref="location Class"/>
        virtual void ScheduleTask(TaskProc proc, void * data, location& placement);

        /// <summary>
        ///     Determines whether a given location is available on the scheduler.
        /// </summary>
        /// <param name="_Placement">
        ///     A reference to the location to query the scheduler about.
        /// </param>
        /// <returns>
        ///     An indication of whether or not the location specified by the <paramref name="_Placement"/> argument is available on the scheduler.
        /// </returns>
        /// <remarks>
        ///     Note that the return value is an instantaneous sampling of whether the given location is available.  In the presence of multiple
        ///     schedulers, dynamic resource management may add or take away resources from schedulers at any point.  Should this happen, the given
        ///     location may change availability.
        /// </remarks>
        virtual bool IsAvailableLocation(const location& _Placement) const;

    public:  // Internal stuff

        enum
        {
            //
            // One shot starts with a single reference count placed implicitly by the module in which ConcRT is contained.
            //
            ONESHOT_NOT_INITIALIZED = 1,
            ONESHOT_INITIALIZED_FLAG = 0x80000000
        };

        /// <summary>
        ///     Returns whether or not the scheduler has performed one shot static construction.
        /// </summary>
        static bool IsOneShotInitialized() { return ((s_oneShotInitializationState & ONESHOT_INITIALIZED_FLAG) != 0); }

        /// <summary>
        ///     Detaches this scheduler from the current thread. It is required that the current scheduler on the thread be the same as 'this'
        /// </summary>
        void Detach();

        /// <summary>
        ///     Generates a unique identifier for a context.
        /// </summary>
        unsigned int GetNewContextId();

        /// <summary>
        ///     Generates a unique identifier for a schedule group.
        /// </summary>
        unsigned int GetNewScheduleGroupId();

        /// <summary>
        ///     Generates a unique identifier for a work queue across schedulers.
        /// </summary>
        static unsigned int GetNewWorkQueueId();

        /// <summary>
        ///     Gets a reserved context off the free list.  This is lock-free and safe to use at any point in the scheduler.  If a context
        ///     is returned, it is a pre-bound and unstarted context.
        /// </summary>
        InternalContextBase *GetReservedContext()
        {
            return m_reservedContexts.Pop();
        }

        /// <summary>
        ///     Releases the list of reserved contexts to the idle pool.  The thread proxy
        ///     is released before returning the contexts to the idle pool.
        /// </summary>
        void ReleaseReservedContexts();

        /// <summary>
        ///     Acquires a new internal context of the appropriate type and returns it.  This can come from either
        ///     a free list within the scheduler, or be one newly allocated from the heap.
        /// </summary>
        /// <param name="fThrottled">
        ///     An indication as to whether the creation should be throttled.
        /// </param>
        InternalContextBase *GetInternalContext(bool fThrottled = true);

        /// <summary>
        ///     Acquires a new internal context of the appropriate type and notifies the scheduler when it is available.  The scheduler can
        ///     choose what to do with said internal context.  This creation happens in a deferred manner subject to throttling constraints.
        /// </summary>
        void DeferredGetInternalContext();

        ///<summary>
        ///     Releases an internal context to the scheduler's idle pool.
        ///</summary>
        void ReleaseInternalContext(InternalContextBase *pContext, bool fUnbind = false);

        /// <summary>
        ///     Gets a realized chore from the idle pool, creating a new one if the idle pool is empty.
        /// </summary>
        RealizedChore *GetRealizedChore(TaskProc pFunction, void* pParameters);

        ///<summary>
        ///     Releases an external context of the to the scheduler's idle pool, destroying it if the idle pool is full.
        ///</summary>
        void ReleaseRealizedChore(RealizedChore *pChore);

        /// <summary>
        ///     References the anonymous schedule group, creating it if it doesn't exists, and returns a pointer to it.
        /// </summary>
        ScheduleGroupBase* GetAnonymousScheduleGroup()
        {
            return m_pAnonymousScheduleGroup;
        }

        /// <summary>
        ///     References a segment in the anonymous schedule group and returns a pointer to it.
        /// </summary>
        /// <returns>
        ///     A segment in the anonymous schedule group.
        /// </returns>
        ScheduleGroupSegmentBase *GetAnonymousScheduleGroupSegment();

        static SchedulerBase* CurrentScheduler();
        static SchedulerBase* FastCurrentScheduler();
        static SchedulerBase* SafeFastCurrentScheduler();
        static ContextBase* FastCurrentContext();
        static ContextBase* SafeFastCurrentContext();
        static ContextBase* CreateContextFromDefaultScheduler();
        static ContextBase* CurrentContext()
        {
            if ( !IsOneShotInitialized())
                return CreateContextFromDefaultScheduler();
            ContextBase *pContext = (ContextBase*) platform::__TlsGetValue(t_dwContextIndex);
            if (pContext == NULL)
                return CreateContextFromDefaultScheduler();
            return pContext;
        }

        /// <summary>
        ///     Gets an IScheduler pointer for use in communication with the resource manager.
        /// </summary>
        virtual IScheduler* GetIScheduler() = 0;

        bool IsUMSScheduler() const
        {
            return false;
        }

        /// <summary>
        ///     Gets an IResourceManager pointer for use in communication with the resource manager.
        /// </summary>
        IResourceManager *GetResourceManager() const
        {
            return m_pResourceManager;
        }

        /// <summary>
        ///     Gets an ISchedulerProxy pointer for use in communication with the resource manager.
        /// </summary>
        ISchedulerProxy *GetSchedulerProxy() const
        {
            return m_pSchedulerProxy;
        }

        /// <summary>
        ///     Find an available virtual processor in the scheduler.
        /// </summary>
        bool FoundAvailableVirtualProcessor(VirtualProcessor::ClaimTicket& ticket,
                                            location bias = location(),
                                            ULONG type = VirtualProcessor::AvailabilityAny);

        /// <summary>
        ///     Try to steal from foreign nodes.
        /// </summary>
        InternalContextBase *StealForeignLocalRunnableContext(SchedulingNode *pSkipNode);

        /// <summary>
        ///     Start up a virtual processor in the scheduler, if one is found.  The virtual processor must have the specified availability
        ///     characteristics.
        /// </summary>
        bool StartupVirtualProcessor(ScheduleGroupSegmentBase *pSegment,
                                     location bias = location(),
                                     ULONG type = VirtualProcessor::AvailabilityAny);

        /// <summary>
        ///     Start up an idle virtual processor in the scheduler.  This can be any virtual processor except one that is inactive due to
        ///     waiting for a thread creation.
        /// </summary>
        bool StartupIdleVirtualProcessor(ScheduleGroupSegmentBase *pSegment, location bias = location())
        {
            //
            // If the vproc is inactive pending thread -- there's no point in performing a general wake up.  The general wake up will require an SFW
            // context which will simply put it back to sleep and violate our concurrency constraints.  Either an incoming runnable must push to the
            // context or the throttler must wake it up.
            //
            return StartupVirtualProcessor(pSegment, bias, VirtualProcessor::AvailabilityAny & ~VirtualProcessor::AvailabilityInactivePendingThread);
        }

        /// <summary>
        ///     Start up an new virtual processor in the scheduler. New virtual processor refers
        ///     to any vproc that either has never been activated or has been deactivated due to lack
        ///     of work (wait for work).
        /// </summary>
        virtual void StartupNewVirtualProcessor(ScheduleGroupSegmentBase *pSegment, location bias = location())
        {
            StartupVirtualProcessor(pSegment, bias, (VirtualProcessor::AvailabilityType)(VirtualProcessor::AvailabilityIdle | VirtualProcessor::AvailabilityInactive));
        }

        /// <summary>
        ///     Attempts to push a runnable to an inactive virtual processor.  If successful, true is returned.
        /// </summary>
        virtual bool PushRunnableToInactive(InternalContextBase *pRunnableContext, location bias = location());

        /// <summary>
        ///     Called when a virtual processor becomes active (before it does) or becomes inactive (before it does).
        /// </summary>
        /// <param value="fActive">
        ///     True if a virtual processor is going from INACTIVE to ACTIVE, and false if it is going from ACTIVE to INACTIVE.
        /// </param>
        /// <returns>
        ///     For activation, the function returns true if the virtual processor was successfully activated, and false
        ///     if it could not be activated because the scheduler was shutting down. For inactivation, it always returns true.
        /// </returns>
        bool VirtualProcessorActive(bool fActive);

        /// <summary>
        ///     Internal contexts and background threads call this when created and used inside the scheduler.
        /// </summary>
        void IncrementInternalContextCount();

        /// <summary>
        ///     Internal contexts and background threads call this function in order to notify that they are about to exit.
        ///     The last caller will trigger scheduler finalization.
        /// </summary>
        void DecrementInternalContextCount();

        /// <summary>
        ///     Returns the scheduling protocol policy element value this scheduler was created with.
        /// </summary>
        ::Concurrency::SchedulingProtocolType GetSchedulingProtocol() { return m_schedulingProtocol; }

        /// <summary>
        ///     Returns a pointer to the 'next' scheduling ring in a round-robin manner
        /// </summary>
        SchedulingRing *GetNextSchedulingRing();

        // Specifying pOwningNode produces an order of scheduling rings, ordered by node distance.
        // pCurrentNode is the current poistion in said order.
        SchedulingRing *GetNextSchedulingRing(const SchedulingRing *pOwningRing, SchedulingRing *pCurrentRing);

        /// <summary>
        ///     Sets the 'next' scheduling ring in a round-robin manner
        /// </summary>
        void SetNextSchedulingRing(SchedulingRing *pRing);

        /// <summary>
        ///     Returns true if the scheduler has gone past a certain point in PhaseTwoShutdown (when it sets the shutdown completed flag).
        ///     This function is mainly used for debug asserts.
        /// </summary>
        bool HasCompletedShutdown();

        /// <summary>
        ///     Returns true if the scheduler is in the finalization sweep, i.e, the SUSPEND_GATE_FLAG is set.
        ///     This function is mainly used for debug asserts.
        /// </summary>
        bool InFinalizationSweep();

        /// <summary>
        ///     Internal contexts call the scheduler when they go idle for a specified amount of time in order to allow
        ///     things that happen on scheduler idle to happen (e.g.: sweeping for phase two shutdown).
        ///     They must also call the scheduler when they transition out of idle before executing a work item or performing
        ///     a context switch.  This may halt scheduler shutdown or it may coordinate with scheduler shutdown depending on
        ///     the current phase of shutdown.
        ///
        ///     This call *MUST* be made from a scheduler critical region.
        /// </summary>
        /// <param name="fIdle">
        ///     Specifies whether the proessor is going idle or non-idle.
        /// </param>
        void VirtualProcessorIdle(bool fIdle);

        /// <summary>
        ///     Adds a new statistics class to track.
        /// </summary>
        /// <param name="pStats">
        ///     The statistics we are adding to the scheduler's ListArray<ExternalStatistics> for tracking.
        /// </param>
        void AddExternalStatistics(ExternalStatistics * pStats)
        {
            m_externalThreadStatistics.Add(pStats);
        }

        /// <summary>
        ///     Saves the statistical information from the retiring virtual processor.
        /// </summary>
        /// <param name="pVProc">
        ///     The virtual processor that is retiring and whose statistics we are trying to preserve.
        /// </param>
        /// <remarks>
        ///     The reason we use interlocked operation here is because multiple virtual processors can
        ///     be retiring at the same time and the error can be much greater than on a simple increment.
        /// </remarks>
        void SaveRetiredVirtualProcessorStatistics(VirtualProcessor * pVProc)
        {
            InterlockedExchangeAdd((volatile long *) &m_enqueuedTaskCounter, pVProc->GetEnqueuedTaskCount());
            InterlockedExchangeAdd((volatile long *) &m_dequeuedTaskCounter, pVProc->GetDequeuedTaskCount());
        }

        /// <summary>
        ///     Resets the count of work coming in.
        /// </summary>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetEnqueuedTaskCount()
        {
            ULONG currentValue = m_enqueuedTaskCounter;
            unsigned int retVal = (unsigned int) (currentValue - m_enqueuedTaskCheckpoint);

            // Update the checkpoint value with the current value
            m_enqueuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Resets the count of work being done.
        /// </summary>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetDequeuedTaskCount()
        {
            ULONG currentValue = m_dequeuedTaskCounter;
            unsigned int retVal = (unsigned int) (currentValue - m_dequeuedTaskCheckpoint);

            // Update the checkpoint value with the current value
            m_dequeuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Returns a suballocator from the pool of suballocators in the process, or creates a new one. The RM only allows
        ///     a fixed number of allocators for external contexts in the process, whereas every virtual processor that requests
        ///     an allocator will get one.
        /// </summary>
        /// <param name="fExternalAllocator">
        ///     Specifies whether the allocator is being requested for an external context. If this is 'true' the RM will return
        ///     NULL if it has reached its limit of suballocators for external contexts. If this is 'false', the caller is requesting
        ///     the suballocator for a virtual processor, and the RM *must* allocate one (resources permitting).
        /// </param>
        static SubAllocator* GetSubAllocator(bool fExternalAllocator);

        /// <summary>
        ///     Returns a suballocator back to the pool in the RM. The RM caches a fixed number of suballocators and will destroy the
        ///     rest.
        /// </summary>
        static void ReturnSubAllocator(SubAllocator* pAllocator);

        /// <summary>
        ///     Enqueues a context into m_allContexts
        /// </summary>
        void AddContext(InternalContextBase * pContext);

        /// <summary>
        ///     Returns the first scheduling node.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned scheduling node will be placed here.  This can only be
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextSchedulingNode.
        /// </param>
        SchedulingNode *GetFirstSchedulingNode(int *pIdx)
        {
            *pIdx = 0;
            return GetNextSchedulingNode(pIdx, -1);
        }

        /// <summary>
        ///     Returns the next scheduling node in an iteration.
        /// </summary>
        SchedulingNode *GetNextSchedulingNode(int *pIdx, int idxStart = 0)
        {
            int base = *pIdx + (idxStart == -1 ? 0 : 1);
            int size = m_nodeCount;
            for (int i = 0; i < size; i++)
            {
                int index = (i + base) % size;
                if (index == idxStart)
                    return NULL;

                SchedulingNode *pNode = m_nodes[index];
                if (pNode != NULL)
                {
                    *pIdx = index;
                    return pNode;
                }
            }

            return NULL;
        }

        /// <summary>
        ///     Performs a reference on one shot static items.  The caller should CheckOneShotStaticDestruction to remove
        ///     the reference count.
        /// </summary>
        static LONG ReferenceStaticOneShot()
        {
            return InterlockedIncrement(&s_oneShotInitializationState);
        }

        /// <summary>
        ///     Removes a previous reference on one shot static items.
        /// </summary>
        static LONG DereferenceStaticOneShot()
        {
            return InterlockedDecrement(&s_oneShotInitializationState);
        }

        /// <summary>
        ///     Called at unload/process exit to perform cleanup of one-shot initialization items.
        /// </summary>
        static void CheckOneShotStaticDestruction();

        /// <summary>
        ///     Called when a particular virtual processor reaches a safe point.  This function does very little unless there has
        ///     been a change in the version number of the safe point.
        /// </summary>
        /// <param name="pMarker">
        ///     The safe point marker for a given virtual processor.  This is the virtual processor reaching a safe point.
        /// </param>
        /// <returns>
        ///     An indication of whether a commit should take place.  If this is true, the caller should call CommitSafePoints when possible.
        ///     Note that this is a return value so that things like UMS virtual processors can exit critical regions before performing
        ///     the commit (to avoid, for instance, heap locks in critical regions).
        /// </returns>
        bool MarkSafePoint(SafePointMarker *pMarker);

        /// <summary>
        ///     Called to make a determination of what version of data we can commit up to.  This is the minimum data version that all virtual
        ///     processors have observed.
        /// </summary>
        void CommitSafePoints();

        /// <summary>
        ///     The routine is used to trigger a safe point commit on all the vprocs by
        ///     updating the data version.
        /// </summary>
        void TriggerCommitSafePoints(SafePointMarker *pMarker);

        /// <summary>
        ///     Determines how long in milliseconds until the next set of threads is allowed to be created.
        /// </summary>
        ULONG ThrottlingTime(ULONG stepWidth);

        /// <summary>
        ///     Returns the delay before the next thread can be created.
        /// </summary>
        ULONG ThrottlingDelta()
        {
            ULONGLONG curTime = platform::__GetTickCount64();
            ULONG delta = (ULONG)(curTime - m_lastThrottledCreateTime);

            return delta;
        }

        /// <summary>
        ///     Puts a timestamp on the last time a throttled thread was created.
        /// </summary>
        void StampThrottledCreate()
        {
            m_lastThrottledCreateTime = platform::__GetTickCount64();
        }

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

        /// <summary>
        ///     Returns whether a virtual processor is available.
        /// </summary>
        bool HasVirtualProcessorAvailable() const
        {
            return m_virtualProcessorAvailableCount > 0;
        }

        /// <summary>
        ///     Returns whether a virtual processor is waiting for throttling.
        /// </summary>
        bool HasVirtualProcessorPendingThreadCreate() const
        {
            return m_virtualProcessorsPendingThreadCreate > 0;
        }

        /// <summary>
        ///     Returns whether a virtual processor is available to execute new work.
        /// </summary>
        bool HasVirtualProcessorAvailableForNewWork() const
        {
            //
            // The observational race (lack of atomicity between the two reads) should not matter.  If it does in some obscure
            // case, a new atomic counter can be added.
            //
            return (m_virtualProcessorAvailableCount - m_virtualProcessorsPendingThreadCreate) > 0;
        }

        /// <summary>
        ///     Removes an unreferenced schedule group from the scheduler's list of groups.
        /// </summary>
        void RemoveScheduleGroup(ScheduleGroupBase *pGroup);

        /// <summary>
        ///     Returns the scheduling node associated with the calling thread, if any. This method only returns a node if the current
        ///     context is an internal context.
        /// </summary>
        SchedulingNode * FindCurrentNode();

        /// <summary>
        ///     Returns the scheduling node which pSrcLocation is a member of.  Note that if srcLocation and this node's location do not intersect,
        ///     this will return NULL.
        /// </summary>
        SchedulingNode * FindNodeByLocation(location* pSrcLocation);

        /// <summary>
        ///     Returns whether or not a location has a tight binding to an object on this scheduler.
        /// </summary>
        bool IsLocationBound(const location* pLoc) const
        {
            return (pLoc->_GetBindingId() == m_id);
        }

        /// <summary>
        ///     Returns a bit set for a given location to perform quick masking.
        /// </summary>
        QuickBitSet GetBitSet(const location* pLoc);

        /// <summary>
        ///     Notifies the scheduler that a given virtual processor is listening for affinity events pertaining to its underlying
        ///     resource.  Note that this is a reference counted API.
        /// </summary>
        /// <param name="maskId">
        ///     The mask id assigned for a given resource.
        /// </param>
        void ListenAffinity(unsigned int maskId)
        {
            m_nonAffineResourceListeners.InterlockedSet(maskId);
            OMTRACE(MTRACE_EVT_LISTENINGTRUE, this, NULL, NULL, maskId);
            ClearQuickCacheSlot(maskId);
        }

        /// <summary>
        ///     Notifies the scheduler that a given virtual processor is ignoring messages for affinity events pertaining to its underlying
        ///     resource.  Note that this is a reference counted API.
        /// </summary>
        /// <param name="maskId">
        ///     The mask id assigned for a given resource.
        /// </param>
        void IgnoreAffinity(unsigned int maskId)
        {
            m_nonAffineResourceListeners.InterlockedClear(maskId);
            OMTRACE(MTRACE_EVT_LISTENINGFALSE, this, NULL, NULL, maskId);
        }

        /// <summary>
        ///     Called when affine work comes into the scheduler, this posts any required notifications to virtual processors which are executing
        ///     non-affine work that they need to stop working on their current group and search for affine work again.
        /// </summary>
        void PostAffinityMessage(const QuickBitSet& srcMask)
        {
            if (srcMask.Intersects(m_nonAffineResourceListeners))
            {
                OMTRACE(MTRACE_EVT_POSTAFFINITYMESSAGE, this, NULL, NULL, srcMask.DbgAcquireBits(0));
                m_affinityMessages.InterlockedSet(srcMask & m_nonAffineResourceListeners);
            }
        }

        /// <summary>
        ///     Returns whether a given resource id has a message for affinity and, if so, acknowledges it.
        /// </summary>
        bool AcknowledgedAffinityMessage(unsigned int maskId)
        {
            bool hasMessage = m_affinityMessages.IsSet(maskId);
            if (hasMessage)
                m_affinityMessages.InterlockedClear(maskId);

            return hasMessage;
        }

        /// <summary>
        ///     Returns the mask id for a given resource id.
        /// </summary>
        unsigned int GetResourceMaskId(unsigned int resourceId)
        {
            unsigned int val;
            Hash<unsigned int, unsigned int>::ListNode *pNode = m_resourceBitMap.Find(resourceId, &val);
            ASSERT(pNode != NULL);
            return val;
        }

        /// <summary>
        ///     Returns the number of mask ids associated with the scheduler.
        /// </summary>
        unsigned int GetMaskIdCount() const
        {
            return ::Concurrency::GetProcessorCount();
        }

        /// <summary>
        ///     Acquires the quick cache slot.
        /// </summary>
        ScheduleGroupSegmentBase *AcquireQuickCacheSlot(unsigned int maskId)
        {
            //
            // Make **SURE** this is short, sweet, and inlines.
            //
            if (m_pCoreAffinityQuickCache[maskId << QUICKCACHEPAD_SHIFT] > reinterpret_cast<ScheduleGroupSegmentBase *>(1))
            {
                return ActualGetQuickCacheSlot(maskId);
            }

            return NULL;
        }

        /// <summary>
        ///     Clears the quick cache slot.
        /// </summary>
        void ClearQuickCacheSlot(unsigned int maskId)
        {
            if (m_pCoreAffinityQuickCache[maskId << QUICKCACHEPAD_SHIFT] == reinterpret_cast<ScheduleGroupSegmentBase *>(1))
            {
                InterlockedCompareExchangePointer(reinterpret_cast <void * volatile *>(m_pCoreAffinityQuickCache + (maskId << QUICKCACHEPAD_SHIFT)),
                                                  reinterpret_cast <void *> (NULL),
                                                  reinterpret_cast <void *> (1));
            }
        }

        /// <summary>
        ///     Clears a given quick cache slot if the slot contains a specific value.
        /// </summary>
        void ClearQuickCacheSlotIf(unsigned int maskId, ScheduleGroupSegmentBase *pSegment)
        {
            if (m_pCoreAffinityQuickCache[maskId << QUICKCACHEPAD_SHIFT] == pSegment)
            {
                InterlockedCompareExchangePointer(reinterpret_cast <void * volatile *>(m_pCoreAffinityQuickCache + (maskId << QUICKCACHEPAD_SHIFT)),
                                                  reinterpret_cast <void *> (NULL),
                                                  reinterpret_cast <void *> (pSegment));
            }
        }

        /// <summary>
        ///     Sets a given quick cache slot.  Each execution resource (by mask id) gets a quick cache slot.  When a work item arrives that is specifically
        ///     affinitized to a given execution resource, the segment containing that work item is stashed in the quick cache slot for the corresponding
        ///     execution resource.  This is a fast check which is made repeatedly during search-for-work.  This allows a virtual processor which is idle
        ///     searching for work or which is executing non-affine work to quickly snap back to an affine segment without the need for a search.  This allows
        ///     more rapid virtual processor spin-up for certain affinity scenarios.
        /// </summary>
        void SetQuickCacheSlot(unsigned int maskId, ScheduleGroupSegmentBase *pSegment)
        {
            if (m_pCoreAffinityQuickCache[maskId << QUICKCACHEPAD_SHIFT] == NULL)
            {
                InterlockedCompareExchangePointer(reinterpret_cast <void * volatile *>(m_pCoreAffinityQuickCache + (maskId << QUICKCACHEPAD_SHIFT)),
                                                  reinterpret_cast <void *> (pSegment),
                                                  reinterpret_cast <void *> (NULL));
            }
        }

        /// <summary>
        ///     Notifies the scheduler that a thread serving a virtual processor with the given mask id is actively searching for work.  This
        ///     will prevent other virtual processors from picking up work which is affine to maskId but not affine to the other virtual processor.
        /// </summary>
        void NotifySearching(unsigned int maskId, bool fSearching)
        {
            if (fSearching)
            {
                m_idleSearch.InterlockedSet(maskId);
                OMTRACE(MTRACE_EVT_SEARCHINGTRUE, this, NULL, NULL, maskId);
                ClearQuickCacheSlot(maskId);
            }
            else
            {
                m_idleSearch.InterlockedClear(maskId);
                OMTRACE(MTRACE_EVT_SEARCHINGFALSE, this, NULL, NULL, maskId);

            }
        }

        /// <summary>
        ///     Returns whether or not any of the set of virtual processors represented by bitSet is searching for work.
        /// </summary>
        bool HasSearchers(const QuickBitSet& bitSet) const
        {
            return m_idleSearch.Intersects(bitSet);
        }

        /// <summary>
        ///     Checks whether a periodic scan is necessary, and if so, performs it.
        /// </summary>
        void PeriodicScan(ULONGLONG serviceTime)
        {
            //
            // Right now, we only perform livelock service scan every 2 seconds.
            //
            if (serviceTime - m_lastServiceScan > 2000)
                PerformServiceScan(serviceTime);
        }

        /// <summary>
        ///     Increments the count of active resources by a given resource's mask id.
        /// </summary>
        void IncrementActiveResourcesByMask(unsigned int maskId)
        {
            m_activeSet.InterlockedSet(maskId);
        }

        /// <summary>
        ///     Decrements the count of active resources by a given resource's mask id.
        /// </summary>
        void DecrementActiveResourcesByMask(unsigned int maskId)
        {
            m_activeSet.InterlockedClear(maskId);
        }

        //**************************************************
        //
        // TRANSITION: This is temporary until such time as we can hook into priority to solve livelock issues.
        //

        bool HasPriorityObjects() const
        {
            return !m_priorityObjects.Empty();
        }

        BoostedObject *GetNextPriorityObject();

        void RemovePrioritizedObject(BoostedObject *pEntry);

        //
        // TRANSITION: End of temporary section
        //
        //**************************************************


    protected:

        SchedulerPolicy m_policy;

        // scheduler policy fields
        ::Concurrency::SchedulerType m_schedulerKind;
        ::Concurrency::SchedulingProtocolType m_schedulingProtocol;
        unsigned short m_localContextCacheSize;

        // The total number of virtual processors in the scheduler, not including oversubscribed virtual processors.
        // This number is adjusted as dynamic RM adds and removes cores.
        volatile LONG m_virtualProcessorCount;

        // The default scheduler
        static SchedulerBase* s_pDefaultScheduler;
        static _StaticLock s_defaultSchedulerLock;

        // The default scheduler policy
        static SchedulerPolicy* s_pDefaultSchedulerPolicy;

        // TLS data
        static DWORD t_dwContextIndex;
        DWORD m_dwExternalStatisticsIndex;

        //
        // NOTE:  Must cleanup up m_nodes before m_rings
        //
        NumaInformation* m_numaInformation;
        SchedulingNode** m_nodes;
        SchedulingRing** m_rings;
        int m_numaCount;
        int m_nodeCount;

        //
        // The active set of virtual processors on this scheduler.
        //
        ReferenceCountedQuickBitSet m_activeSet;

        //
        // Tracking for virtual processors which need messages of notification for affine work scheduling, etc...
        //
        ReferenceCountedQuickBitSet m_idleSearch;
        ReferenceCountedQuickBitSet m_nonAffineResourceListeners;
        QuickBitSet m_affinityMessages;

        //
        // Quick cache for core affine tasks.
        //
        ScheduleGroupSegmentBase* volatile * m_pCoreAffinityQuickCache;

        // The list of schedule groups within the scheduler.  Note that while groups are owned by the scheduler, a group is merely
        // a collection of segments where the individual segments are owned by scheduling rings.  This allows groups with affinity applied
        // as well as separation of work within a group by which node scheduled it.
        ListArray<ScheduleGroupBase> m_scheduleGroups;

        // The single anonymous schedule group for the scheduler.  The anonymous schedule group will have one segment per ring.
        ScheduleGroupBase *m_pAnonymousScheduleGroup;

        // Lock free list of all internal contexts in the scheduler
        LockFreePushStack<ContextNode> m_allContexts;

        SafeRWList<WaitNode> m_finalEvents;

        // A list array that keeps statistical information for all non-internal contexts
        ListArray<ExternalStatistics> m_externalThreadStatistics;

        // Lock that guards the data structures for tracking context exit events.
        _NonReentrantBlockingLock m_listArrayDeletionLock;

        /// <summary>
        ///     Activate the given virtual processor
        /// </summary>
        void ActivateVirtualProcessor(VirtualProcessor *pVirtualProcessor, ScheduleGroupBase *pGroup);

        /// <summary>
        ///     Returns a newly constructed internal context appropriate to the given type of scheduler.
        /// </summary>
        virtual InternalContextBase *CreateInternalContext() =0;

        /// <summary>
        ///     Increments the reference counts required by a scheduler attach.
        /// </summary>
        void ReferenceForAttach();

        /// <summary>
        ///     Decrements the reference counts incremented for scheduler attach.
        /// </summary>
        void ReleaseForDetach();

        /// <summary>
        ///     Returns a current number of active virtual processors for this scheduler
        /// </summary>
        /// <returns>
        ///     Returns a current number of active virtual processors for this scheduler.  No error state.
        /// </returns>
        unsigned int GetNumberOfActiveVirtualProcessors() const { return m_activeVProcCount; };

        ///<summary>
        ///     Notification after a virtual processor goes from INACTIVE to ACTIVE or ACTIVE to INACTIVE
        ///</summary>
        /// <param value="fActive">
        ///     True if a virtual processor is going from INACTIVE to ACTIVE, and false if it is going from ACTIVE to INACTIVE.
        /// </param>
        /// <param value="activeCount">
        ///     Active virtual processor count after the transition
        /// </param>
        virtual void VirtualProcessorActiveNotification(bool fActive, LONG activeCount)
        {
            (fActive); (activeCount);
        }

        /// <summary>
        ///     Indicates the type of work which exists within the scheduler.
        /// </summary>
        enum PendingWorkType
        {
            /// <summary>
            ///     No work exists within the scheduler.
            /// </summary>
            NoWork,

            /// <summary>
            ///     There is user work within the scheduler (chores, tasks, blocked contexts, etc...).  There may or may not
            ///     be ancillary work.
            /// </summary>
            UserWork,

            /// <summary>
            ///     There is ancillary work related to the scheduler (e.g.: queued timers for throttling, etc...)
            /// </summary>
            OnlyAncillaryWork
        };

        /// <summary>
        ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
        ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown.
        /// </summary>
        virtual PendingWorkType TypeOfWorkPending();

        /// <summary>
        ///     Initialize scheduler event handlers/background threads
        /// </summary>
        virtual void InitializeSchedulerEventHandlers();

        /// <summary>
        ///     Destroy scheduler event handlers/background threads
        /// </summary>
        virtual void DestroySchedulerEventHandlers();

        /// <summary>
        ///     Cancel all the inernal contexts.
        /// </summary>
        virtual void CancelAllContexts();

        /// <summary>
        ///     Returns the count of bound contexts on the scheduler.
        /// </summary>
        ULONG GetNumberOfBoundContexts() const
        {
            return (ULONG)m_boundContextCount;
        }

        // Implementation for IScheduler interface APIs that is shared between to all derived classes.

        /// <summary>
        ///     Called by the resource manager in order to gather statistics for a given scheduler.  The statistics gathered here
        ///     will be used to drive dynamic feedback with the scheduler to determine when it is appropriate to assign more resources
        ///     or take resources away.  Note that these counts can be optimistic and do not necessarily have to reflect the current
        ///     count with 100% synchronized accuracy.
        /// </summary>
        /// <param name="pTaskCompletionRate">
        ///     The number of tasks which have been completed by the scheduler since the last call to the Statistics method.
        /// </param>
        /// <param name="pTaskArrivalRate">
        ///     The number of tasks that have arrived in the scheduler since the last call to the Statistics method.
        /// </param>
        /// <param name="pNumberOfTasksEnqueued">
        ///     The total number of tasks in all scheduler queues.
        /// </param>
        void Statistics(unsigned int *pTaskCompletionRate, unsigned int *pTaskArrivalRate, unsigned int *pNumberOfTasksEnqueued);

        /// <summary>
        ///     Called when the resource manager is giving virtual processors to a particular scheduler.  The virtual processors are
        ///     identified by an array of IVirtualProcessorRoot interfaces. This call is made to grant virtual processor roots
        ///     at initial allocation during the course of ISchedulerProxy::RequestInitialVirtualProcessors, and during dynamic
        ///     core migration.
        /// </summary>
        /// <param name="pVirtualProcessorRoots">
        ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors being added to the scheduler.
        /// </param>
        /// <param name="count">
        ///     Number of IVirtualProcessorRoot interfaces in the array.
        /// </param>
        void AddVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count);

        /// <summary>
        ///     Called when the resource manager is taking away virtual processors from a particular scheduler.  The scheduler should
        ///     mark the supplied virtual processors such that they are removed asynchronously and return immediately.  Note that
        ///     the scheduler should make every attempt to remove the virtual processors as quickly as possible as the resource manager
        ///     will reaffinitize threads executing upon them to other resources.  Delaying stopping the virtual processors may result
        ///     in unintentional oversubscription within the scheduler.
        /// </summary>
        /// <param name="pVirtualProcessorRoots">
        ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors which are to be removed.
        /// </param>
        /// <param name="count">
        ///     Number of IVirtualProcessorRoot interfaces in the array.
        /// </param>
        void RemoveVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count);

        /// <summary>
        ///     Invoked when the Gate Count goes to zero as a result of virtual processor state transitions, while the
        ///     scheduler has been marked for shutdown. It proceeds to sweep the scheduler if it can set the suspend flag
        ///     on the shutdown gate while the gate count is still 0 and the scheduler is marked for shutdown.
        /// </summary>
        void AttemptSchedulerSweep();

        /// <summary>
        ///     Returns whether the reserved context pool can be utilized to fetch contexts to bypass throttling.
        /// </summary>
        virtual bool AllowGeneralFetchOfReservedContexts()
        {
            return true;
        }

    private:

        friend class ContextBase;
        friend class ::Concurrency::CurrentScheduler;
        friend class ScheduleGroupBase;
        friend class ScheduleGroupSegmentBase;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class InternalContextBase;
        friend class ExternalContextBase;
        friend class VirtualProcessor;
        friend class SchedulingRing;
        friend class SchedulingNode;
        friend class SafePointInvocation;

        //
        // TRANSITION: This is a temporary patch for livelock prevention until we can hook into priority.
        // TRANSITION: This **MUST** have a hyper lock on it.
        //
        SafeRWList<BoostedObject, CollectionTypes::NoCount, _ReaderWriterLock> m_priorityObjects;

        // The list of invocations for safe point registrations.
        SafeSQueue<SafePointInvocation, _HyperNonReentrantLock> m_safePointInvocations;

        // Counter used to assign unique identifiers to contexts.
        volatile LONG m_contextIdCounter;

        // Counter used to assign unique identifiers to schedule groups.
        volatile LONG m_scheduleGroupIdCounter;

        // Counter used to assign unique identifiers to work queues.
        static volatile LONG s_workQueueIdCounter;

        // The current safe point version for data.  This indicates the newest data requiring observation by all virtual processors
        volatile ULONG m_safePointDataVersion;

        // The current safe point commit version.  This indicates the newest data that has been observed by all virtual processors
        volatile ULONG m_safePointCommitVersion;

        // The pending version that is being committed by one of the vprocs.
        volatile ULONG m_safePointPendingVersion;

        // Hash tables for conversion
        Hash<unsigned int, unsigned int> m_resourceNodeMap;
        Hash<unsigned int, unsigned int> m_resourceBitMap;

        // scheduler id
        unsigned int m_id;

        // Round-robin index for scheduling ring.
        unsigned int m_nextSchedulingRingIndex;

        // Handle to a semaphore used to synchronize during scheduler finalization.
        HANDLE m_hSchedulerShutdownSync;

        //
        // Reference counts:
        //
        // m_refCount -- The externally visible reference count on the scheduler.  Incremented for attachment
        //               and for explicit calls to Reference.  When this reference count falls to zero, the
        //               scheduler initiates shutdown.  When m_internalContextCount falls to zero, the
        //               scheduler finalizes.
        //
        // m_attachCount -- The count of external contexts to which this scheduler is attached.  This is primarily
        //                  present for debugging purposes.
        //
        // m_internalContextCountPlusOne -- The count of internal contexts on the scheduler plus one.  Note that
        //                                  it's +1 to explicitly handle any possibility of scheduler shutdown
        //                                  before internal contexts are created.
        //
        // m_boundContextCount -- The count of internal contexts which are currently bound.  This affects how the scheduler
        //                        throttles thread creation.
        //
        volatile LONG m_refCount;
        volatile LONG m_attachCount;
        volatile LONG m_internalContextCountPlusOne;
        volatile LONG m_initialReference;
        volatile LONG m_boundContextCount;

        //
        // The virtual processor shutdown gate.  This is used to implement scheduler shutdown, by ensuring a handshake
        // when all virtual processors go idle.  When such happens, no virtual processor may go active again without
        // handshaking.  During the period between handshakes, the scheduler is free to sweep schedule groups
        // to detect whether finalization is yet appropriate.
        //
        // Layout:
        //    31 - SHUTDOWN_INITIATED_FLAG   -- indicates that the external reference count on the scheduler has fallen to zero,
        //                                      and the scheduler should be able to finalize when all work queued to it has
        //                                      completed. This flag may be reset at a later point if an internal context
        //                                      ends up resurrecting the scheduler.
        //    30 - SUSPEND_GATE_FLAG         -- indicates a suspend phase while the scheduler is trying to evaluate whether
        //                                      it is ready to finalize. A scheduler may find blocked contexts during this
        //                                      phase and back off from finalization, resetting the flag. No contexts are allowed
        //                                      to execute work during this phase, and no new virtual processors may be added
        //                                      to the scheduler while this bit is set.
        //    29 - SHUTDOWN_COMPLETED_FLAG   -- indicates that the scheduler has completed shutdown. This is the point of no
        //                                      return, for this scheduler. At this point no work should exist in the scheduler,
        //                                      and attempts to add any new virtual processors will fail, since the scheduler
        //                                      is about to be destroyed.
        //
        volatile LONG m_vprocShutdownGate;

        // An indication of whether we have done a sweep without actual work.
        volatile LONG m_fSweepWithoutActualWork;

        // An indication of how long it has been since the last sweep for livelocked segments.
        volatile ULONGLONG m_lastServiceScan;

        static _StaticLock s_schedulerLock;
        static LONG s_initializedCount;

        //
        // The one shot initialization state has two parts, a reference count occupying the lower 31 bits and a flag indicating whether
        // one shot initialization was performed in the top bit.
        //
        static LONG s_oneShotInitializationState;

        IResourceManager *m_pResourceManager;
        ISchedulerProxy *m_pSchedulerProxy;

        // The count of virtual processors active in the scheduler.
        volatile LONG m_activeVProcCount;

        // The number of virtual processors available to schedule more work.
        // This does *NOT* take into account those virtual processors which are *inactive pending thread*
        volatile LONG m_virtualProcessorAvailableCount;

        // The number of virtual processors available pending a thread creation.
        volatile LONG m_virtualProcessorsPendingThreadCreate;

        // Statistics data counters
        volatile ULONG m_enqueuedTaskCounter;
        volatile ULONG m_dequeuedTaskCounter;

        // Statistics data checkpoints
        ULONG m_enqueuedTaskCheckpoint;
        ULONG m_dequeuedTaskCheckpoint;

        //
        // Throttling information:
        //
        ULONG m_threadsBeforeThrottling;
        ULONGLONG m_lastThrottledCreateTime;

#ifndef _UMS_DISABLED
        HANDLE m_hThrottlingEvent;
        HANDLE m_hThrottlingWait;
#endif //_UMS_DISABLED

        HANDLE m_hThrottlingTimer;
        volatile LONG m_pendingDeferredCreates;

        // Free list of internal contexts.
        LockFreeStack<InternalContextBase> m_internalContextPool;

        // Free list of external contexts.
        LockFreeStack<ExternalContextBase> m_externalContextPool;

        // Free list of realized chores.
        LockFreeStack<RealizedChore> m_realizedChorePool;

        // List of reserved contexts
        LockFreeStack<InternalContextBase> m_reservedContexts;

        // A stack that holds free suballocators.
        static LockFreeStack<SubAllocator> s_subAllocatorFreePool;

        // Number of suballocators for use by external contexts that are active in the process.
        static volatile LONG s_numExternalAllocators;

        // The max number of external contexts that could have suballocators at any given time.
        static const int s_maxExternalAllocators;

        // The maximum depth of the free pool of allocators.
        static const int s_allocatorFreePoolLimit;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

        static void CheckStaticConstruction();
        static void StaticConstruction();
        static void StaticDestruction();
        static void OneShotStaticConstruction();
        static void OneShotStaticDestruction();

        void Initialize();
        void Cleanup();

        int GetValidSchedulingRingIndex(int idx);
        int GetNextValidSchedulingRingIndex(int idx);

        /// <summary>
        ///     Creates the correct type of virtual processor.
        /// </summary>
        virtual VirtualProcessor *CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot) = 0;

        /// <summary>
        ///     Creates an external context and attaches it to the calling thread. Called when a thread attaches to a scheduler.
        /// </summary>
        ExternalContextBase *AttachExternalContext(bool explicitAttach);

        /// <summary>
        ///     Detaches an external context from the scheduler it is attached to. Called when an external context actively detaches
        ///     from a scheduler, or when the underlying thread for an implicitly attached external context exits.
        /// </summary>
        /// <param name="pContext">
        ///     The external context being detached.
        /// </param>
        /// <param name="explicitDetach">
        ///     Whether this was the result of an explicit detach or the thread exiting.
        /// </param>
        void DetachExternalContext(ExternalContextBase* pContext, bool explicitDetach);

        /// <summary>
        ///     Gets an external context from the idle pool, creating a new one if the idle pool is empty.
        /// </summary>
        ExternalContextBase *GetExternalContext(bool explicitAttach);

        ///<summary>
        ///     Releases an external context of the to the scheduler's idle pool, destroying it if the idle pool is full.
        ///</summary>
        void ReleaseExternalContext(ExternalContextBase *pContext);

        /// <summary>
        ///     Increments the reference count to the scheduler but does not allow a 0 to 1 transition. This API should
        ///     be used to safely access a scheduler when the scheduler is not 'owned' by the caller.
        /// </summary>
        /// <returns>
        ///     True if the scheduler was referenced, false, if the reference count was 0.
        /// </returns>
        bool SafeReference();

        /// <summary>
        ///     Returns the default scheduler creating one if it doesn't exist.
        /// </summary>
        /// <returns>
        ///     A pointer to the default scheduler
        /// </returns>
        static SchedulerBase* GetDefaultScheduler();

        //
        // Finalization:
        //

        /// <summary>
        ///     Called to initiate shutdown of the scheduler.  This may direclty proceed to phase two of shutdown (actively
        ///     shutting down internal contexts) or it may wait for additional events (e.g.: all work to complete) before
        ///     proceeding to phase two.
        /// </summary>
        void PhaseOneShutdown();

        /// <summary>
        ///     Actively informs all internal contexts to exit and breaks them out of their dispatch loops.  When the last
        ///     internal context dies, finalization will occur and we move to SchedulerBase::Finalize().
        /// </summary>
        void PhaseTwoShutdown();

        /// <summary>
        ///     Performs finalization of the scheduler deleting all structures, etc...  This will also notify any listeners
        ///     that the scheduler has actively shut down.
        /// </summary>
        void Finalize();

        /// <summary>
        ///     Once all virtual processors are idle, the scheduler calls this routine which performs a full sweep through all
        ///     schedule groups looking for work.  If work is found (even a blocked context), the scheduler backs off finalization;
        ///     otherwise, it proceeds by asking all virtual processors for final check-in.
        /// </summary>
        void SweepSchedulerForFinalize();

        /// <summary>
        ///     Releases virtual processors that were suspended on the shutdown gate, while trying to go from IDLE to
        ///     ACTIVE when the finalization sweep was in progress.
        /// </summary>
        /// <param name="releaseCount">
        ///     Number of virtual processors that need to be released.
        /// </param>
        void ReleaseSuspendedVirtualProcessors(LONG releaseCount);

        /// <summary>
        ///     Called during scheduler finalization, after all virtual processors are suspended to check if any chores still
        ///     exist in the scheduler. The calling thread is the only thread active in the scheduler at the time the function
        ///     is called.
        /// </summary>
        /// <returns>
        ///     A boolean value indicating whether any unstarted chores (realized or unrealized) were found.
        /// </returns>
        bool FoundUnstartedChores();

        /// <summary>
        ///     Called during scheduler finalization, before all virtual processors are suspended to check if any blocked
        ///     contexts exist in the scheduler.
        /// </summary>
        /// <returns>
        ///     A boolean value indicating whether any blocked contexts were found.
        /// </returns>
        bool FoundBlockedContexts();

        ///<summary>
        ///     Called to perform a resurrection of the scheduler.  When the scheduler reference count has fallen to zero,
        ///     it's possible there's still work on the scheduler and that one of those work items will perform an action
        ///     leading to additional reference.  Such bringing of the reference count from zero to non-zero is only legal
        ///     on an *INTERNAL* context and immediately halts shutdown.
        ///</summary>
        void Resurrect();

        /// <summary>
        ///     Called to perform a commit of safe-point registrations up to **AND INCLUDING** a particular version.
        /// </summary>
        /// <param name="commitVersion">
        ///     The data version that we commit to.  A version of zero indicates a full commit.
        /// </param>
        void CommitToVersion(ULONG commitVersion);

        /// <summary>
        ///     Returns the commit version for safe points within the scheduler.
        /// </summary>
        ULONG ComputeSafePointCommitVersion();

        /// <summary>
        ///     Updates and returns the pending version for safe point commits.
        ///     If there are no commits pending, 0 is returned.
        /// </summary>
        ULONG UpdatePendingVersion();

        /// <summary>
        ///     Updates the commit version to the given version and returns
        ///     the pending commit version. If there are no commits pending, 0 is returned.
        /// </summary>
        /// <param name="commitVersion">
        ///     The version up to which safe points have been committed.
        /// </param>
        ULONG UpdateCommitVersion(ULONG commitVersion);

        /// <summary>
        ///     Returns whether a particular version number is visible to us yet.  Versions at the wrap-around point
        ///     are not visible until we commit the wrap.
        /// </summary>
        bool IsVisibleVersion(ULONG version)
        {
            return (version >= m_safePointCommitVersion);
        }

        /// <summary>
        ///     Returns the version we are allowed to see from an observation.  This handles wrap around.
        /// </summary>
        ULONG ObservedVersion(ULONG version)
        {
            return (IsVisibleVersion(version) ? version : ULONG_MAX);
        }

        /// <summary>
        ///     Publishes a new data version and returns the version number.
        /// </summary>
        ULONG PublishNewDataVersion()
        {
            ULONG dataVersion = InterlockedIncrement(reinterpret_cast<volatile LONG *>(&m_safePointDataVersion));

            //
            // Zero and ULONG_MAX are special keys used to handle wrap-around in the version counters.  The commit counter may never be either of these values due
            // to a data version being them.
            //
            while (dataVersion == 0 || dataVersion == ULONG_MAX)
                dataVersion = InterlockedIncrement(reinterpret_cast<volatile LONG *>(&m_safePointDataVersion));

            return dataVersion;
        }

        /// <summary>
        ///     Registers a callback at the next safe point after this function call.  This should never be directly used by clients.
        ///     SafePointInvocation::Register(...) should be used instead.
        /// </summary>
        /// <param name="pInvocation">
        ///     The invocation object which is being registered.
        /// </param>
        void InvokeOnSafePoint(SafePointInvocation *pInvocation);

        /// <summary>
        ///     Send a scheduler ETW event
        /// </summary>
        void TraceSchedulerEvent(ConcRT_EventType eventType, UCHAR level, unsigned int schedulerId)
        {
            if (g_TraceInfo._IsEnabled(level, SchedulerEventFlag))
                ThrowSchedulerEvent(eventType, level, schedulerId);
        }

        /// <summary>
        ///     Changes the due time for dispatching new threads
        /// </summary>
        void ChangeThrottlingTimer(ULONG dueTime);

        /// <summary>
        ///     Acts as a trampoline between the event wait and the timer wait as we cannot queue the timer in DeferredGetInternalContext
        ///     due to limitations on what Win32 APIs can be called from a UMS primary.
        /// </summary>
        static void CALLBACK ThrottlerTrampoline(PVOID pData, BOOLEAN waitOrTimerFired);

        /// <summary>
        ///     Creates new contexts.
        /// </summary>
        void ThrottlerDispatch();

        /// <summary>
        ///     Called to notify the scheduler that a context is available from the throttling manager / background creation.
        /// </summary>
        /// <returns>
        ///     An indication of whether a virtual processor was awoken due to the context being utilized.
        /// </returns>
        bool NotifyThrottledContext(InternalContextBase *pContext);

        /// <summary>
        ///     Create a schedule group within this scheduler.
        /// </summary>
        /// <param name="pPlacement">
        ///     A pointer to a location where tasks within the schedule group will be biased towards executing at.
        /// </param>
        /// <returns>
        ///     A pointer to a newly created schedule group.
        /// </returns>
        ScheduleGroup* InternalCreateScheduleGroup(location* pPlacement);

        /// <summary>
        ///    Internal claim of a quick cache slot.
        /// </summary>
        ScheduleGroupSegmentBase *ActualGetQuickCacheSlot(unsigned int maskId)
        {
            ScheduleGroupSegmentBase *pSegment = m_pCoreAffinityQuickCache[maskId << QUICKCACHEPAD_SHIFT];

            if (pSegment > reinterpret_cast<ScheduleGroupSegmentBase *>(1))
            {
                ScheduleGroupSegmentBase *pXchgSegment = reinterpret_cast <ScheduleGroupSegmentBase *> (
                    InterlockedCompareExchangePointer(reinterpret_cast <void * volatile *> (m_pCoreAffinityQuickCache + (maskId << QUICKCACHEPAD_SHIFT)),
                                                      reinterpret_cast <void *> (1),
                                                      reinterpret_cast <void *> (pSegment))
                    );

                if (pSegment == pXchgSegment)
                    return pSegment;
            }

            return NULL;
        }

        /// <summary>
        ///     Performs the scheduler service scan.
        /// </summary>
        void PerformServiceScan(ULONGLONG serviceTime);

        /// <summary>
        ///     A simple bridge to ThrottlerDispatch. This bridge is used for Vista and up (except MSDK)
        /// </summary>
        static void CALLBACK ThrottlerDispatchBridge(PTP_CALLBACK_INSTANCE, void * pContext, PTP_TIMER)
        {
            ThrottlerDispatchBridgeXP(pContext, true);
        }

        /// <summary>
        ///     A simple bridge to ThrottlerDispatch. This bridge is used for XP and MSDK
        /// </summary>
        static void CALLBACK ThrottlerDispatchBridgeXP(PVOID pScheduler, BOOLEAN)
        {
            reinterpret_cast<SchedulerBase *>(pScheduler)->ThrottlerDispatch();
        }

        static void ThrowSchedulerEvent(ConcRT_EventType eventType, UCHAR level, unsigned int schedulerId);

        // Hide assignment operator and copy constructor
        SchedulerBase const &operator =(SchedulerBase const &);
        SchedulerBase(SchedulerBase const &);
    };
} // namespace details
} // namespace Concurrency
