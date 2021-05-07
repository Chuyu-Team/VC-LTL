// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessor.h
//
// Source file containing the VirtualProcessor declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    //
    // TRANSITION: This is temporary until priority based livelock prevention is real.
    //
    struct BoostedObject
    {
        enum BoostType
        {
            BoostTypeScheduleGroupSegment,
            BoostTypeVirtualProcessor

        };

        enum BoostState
        {
            BoostStateDisallowed,
            BoostStateUnboosted,
            BoostStateBoosted
        };

        BoostType m_type;
        BoostedObject *m_pNext;
        BoostedObject *m_pPrev;
        LONG m_boostState;

        bool IsScheduleGroupSegment() const
        {
            return m_type == BoostTypeScheduleGroupSegment;
        }

        bool IsVirtualProcessor() const
        {
            return m_type == BoostTypeVirtualProcessor;
        }
    };

    //
    // virtualized hardware thread
    //
    /// <summary>
    ///     A virtual processor is an abstraction of a hardware thread.  However, there may very well be more than one
    ///     virtual processor per hardware thread.  The <c>SchedulerPolicy</c> key <c>TargetOversubscriptionFactor</c> determines
    ///     the number of virtual processor per hardware thread, scheduler wide.
    /// </summary>
    /// <remarks>
    ///     Virtual processors may be created and destroyed at any time, since resource management (RM) may give or take
    ///     away hardware threads.  But as such batches of <c>TargetOversubscriptionFactor</c> virtual processors are created or
    ///     destroyed simultaneously.
    /// </remarks>
    class VirtualProcessor
    {
    public:

        /// <summary>
        ///     Indicates the type of availability of a virtual processor.
        /// </summary>
        enum AvailabilityType
        {
            // The virtual processor is claimed and running -- it is not available for claiming
            AvailabilityClaimed = 0x0,

            // The virtual processor was marked available and is/will go inactive with nothing running atop it
            AvailabilityInactive = 0x01,

            // The virtual processor is available with an idle/sleeping (or soon to be) thread running atop it.
            AvailabilityIdle = 0x2,

            // The virtual processor was marked available and is/will deactivate; however -- it is pending a thread.  It should
            // not be awoken unless a thread is ready for it (e.g.: not for LWTs / WSQ items)
            AvailabilityInactivePendingThread = 0x4,

            // The virtual processor is available with an idle/sleeping (or soon to be) thread running atop it; however -- it is pending a thread.
            // This is a state exclusive to a scheduler with a specific scheduling context (e.g.: UMS)
            AvailabilityIdlePendingThread = 0x8,

            // Mask for any type of availability (e.g.: if trying to find/claim a vproc)
            AvailabilityAny = 0xf
        };

        /// <summary>
        ///     Represents a claim of exclusive ownership on a virtual processor.
        /// </summary>
        class ClaimTicket
        {
        public:

            /// <summary>
            ///     Constructs a new empty ticket.
            /// </summary>
            ClaimTicket() :
                m_type(AvailabilityClaimed)
            {
            }

            /// <summary>
            ///     Exercises the ticket.  Starts up a new context on the virtual processor or activates the idle one as appropriate.
            /// </sumamry>
            bool Exercise(ScheduleGroupSegmentBase *pSegment = NULL)
            {
                ASSERT(m_type != AvailabilityClaimed);

                bool fResult = m_pVirtualProcessor->ExerciseClaim(m_type, pSegment);

                //
                // Ensure that a second call to exercise the same ticket asserts.
                //
                m_type = AvailabilityClaimed;

                return fResult;
            }

            /// <summary>
            ///     Exercises the ticket with a particular context.  This is only valid if ExerciseWakesExisting() returns false.  Callers should have
            ///     sought virtual processors with specific claim rights.
            /// </summary>
            bool ExerciseWith(InternalContextBase *pContext);

            /// <summary>
            ///     Informs the caller whether the claim simply wakes an existing thread.
            /// </summary>
            bool ExerciseWakesExisting() const
            {
                return m_type == AvailabilityIdle || m_type == AvailabilityIdlePendingThread;
            }


        private:

            friend class VirtualProcessor;

            ClaimTicket(AvailabilityType type, VirtualProcessor *pVirtualProcessor)
            {
                InitializeTicket(type, pVirtualProcessor);
            }

            void InitializeTicket(AvailabilityType type, VirtualProcessor *pVirtualProcessor)
            {
                m_type = type;
                m_pVirtualProcessor = pVirtualProcessor;
            }

            AvailabilityType m_type;
            VirtualProcessor *m_pVirtualProcessor;

        };

        /// <summary>
        ///     Constructs a virtual processor.
        /// </summary>
        VirtualProcessor();

        /// <summary>
        ///     Destroys a virtual processor
        /// </summary>
        virtual ~VirtualProcessor();

        /// <summary>
        ///     Returns a scheduler unique identifier for the virtual processor.
        /// </summary>
        unsigned int GetId() const { return m_id; }

        /// <summary>
        ///     Returns the execution resource id associated with this virtual processor.
        /// </summary>
        unsigned int GetExecutionResourceId() const { return m_resourceId; }

        /// <summary>
        ///     Returns the mask id for this virtual processor.
        /// </summary>
        unsigned int GetMaskId() const { return m_maskId; }

        /// <summary>
        ///     Updates tracking on the virtual processor whether it is executing affine work and/or local work.
        /// </summary>
        /// <param name="fAffine">
        ///     An indication of whether the virtual processor is executing work which has affinity to it or not.
        /// </param>
        /// <param name="fLocal">
        ///     An indication of whether the virtual processor is executing work which is local to it or not.
        /// </param>
        void UpdateWorkState(bool fAffine, bool fLocal);

        /// <summary>
        ///     Performs a quick check as to whether work which is affine to the virtual processor has arrived.  This allows short circuiting of
        ///     expensive searches for affine work in cases where a user does not use any location objects.
        /// </summary>
        /// <returns>
        ///     An indication of whether or not work affine to the virtual processor has arrived since UpdateWorkState was called to indicate that
        ///     the virtual processor began working on non-affine work.
        /// </returns>
        bool CheckAffinityNotification();

        /// <summary>
        ///     Returns an indication of whether the virtual processor is (or was last) executing affine work.
        /// </summary>
        /// <returns>
        ///     An indication of whether the virtual processor is (or was last) executing affine work.
        /// </returns>
        bool ExecutingAffine()
        {
            return m_fAffine;
        }

        /// <summary>
        ///     Attempts to claim exclusive ownership of the virtual processor by resetting the available flag.
        /// </summary>
        /// <param name="ticket">
        ///     If true is returned, this will contain the claim ticket used to activate the virtual processor.
        /// </param>
        /// <param name="type">
        ///     If specified, indicates the type of availability that we can claim.  If the caller is only interested in inactive virtual processors,
        ///     for instance, AvailabilityInactive can be passed.
        /// </param>
        /// <returns>
        ///     True if it was able to claim the virtual processor, false otherwise.
        /// </returns>
        bool ClaimExclusiveOwnership(ClaimTicket &ticket, ULONG type = AvailabilityAny, bool updateCounters = true /* GET RID OF THIS */);

        /// <summary>
        ///     Returns the state of the virtual processor's availability prior to the last claim.
        /// </summary>
        AvailabilityType ClaimantType() const
        {
            ASSERT(m_availabilityType == AvailabilityClaimed);
            return m_claimantType;
        }

        /// <summary>
        ///     Returns whether the last claimant claimed us while inactive.
        /// </summary>
        bool ClaimantWasInactive() const
        {
            return ((ClaimantType() & (AvailabilityInactive | AvailabilityInactivePendingThread)) != 0);
        }

        /// <summary>
        ///     Makes a virtual processor available for scheduling work.
        /// </summary>
        void MakeAvailableForIdle()
        {
            MakeAvailable(AvailabilityIdle);
        }

        /// <summary>
        ///     Makes a virtual processor available for scheduling work.
        /// </summary>
        void MakeAvailableForInactive()
        {
            MakeAvailable(AvailabilityInactive);
        }

        /// <summary>
        ///     Makes a virtual processor available pending a thread.
        /// </summary>
        void MakeAvailablePendingThread()
        {
            MakeAvailable(AvailabilityInactivePendingThread);
        }

        /// <summary>
        ///     Returns a pointer to the internal context that is executing on this virtual processor.
        /// </summary>
        IExecutionContext * GetExecutingContext() { return m_pExecutingContext; }

        /// <summary>
        ///     Returns a pointer to the owning node for the virtual processor.
        /// </summary>
        SchedulingNode * GetOwningNode() { return m_pOwningNode; }

        /// <summary>
        ///     Returns a pointer to the owning ring for the virtual processor.
        /// </summary>
        SchedulingRing * GetOwningRing() { return m_pOwningRing; }

        /// <summary>
        ///     Returns a pointer to the owning root for the virtual processor.
        /// </summary>
        IVirtualProcessorRoot * GetOwningRoot() { return m_pOwningRoot; }

        /// <summary>
        ///     Returns a pointer to the suballocator for the virtual processor.
        /// </summary>
        SubAllocator * GetCurrentSubAllocator();

        /// <summary>
        ///     Returns true if the virtual processor is marked as available, false otherwise.
        /// </summary>
        bool IsAvailable() { return (m_availabilityType != AvailabilityClaimed); }

        /// <summary>
        ///     Returns the type of availability of the virtual processor.
        /// </summary>
        AvailabilityType TypeOfAvailability() { return m_availabilityType; }

        /// <summary>
        ///     Returns true if the virtual processor is marked for retirement, false otherwise.
        /// </summary>
        bool IsMarkedForRetirement() { return m_fMarkedForRetirement; }

        /// <summary>
        ///     Activates a virtual processor with the context provided.
        /// </summary>
        void Activate(IExecutionContext *pContext);

        /// <summary>
        ///     Temporarily deactivates a virtual processor.
        /// <summary>
        /// <returns>
        ///     An indication of which side the awakening occured from (true -- we activated it, false -- the RM awoke it).
        /// </returns>
        bool Deactivate(IExecutionContext *pContext);

        /// <summary>
        ///     Invokes the underlying virtual processor root to ensure all tasks are visible.
        /// </summary>
        void EnsureAllTasksVisible(IExecutionContext * pContext);

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

        /// <summary>
        ///     Returns the default destination of a SwitchTo(NULL).  There is none on a default virtual processor.
        /// </summary>
        virtual IExecutionContext *GetDefaultDestination()
        {
            return NULL;
        }

        /// <summary>
        ///     Performs a search for work for the given virtual processor.
        /// </summary>
        bool SearchForWork(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass)
        {
            return m_searchCtx.Search(pWorkItem, pOriginSegment, fLastPass);
        }

        /// <summary>
        ///     Performs a search for work for the given virtual processor only allowing certain types of work to be found.
        /// </summary>
        bool SearchForWork(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes)
        {
            return m_searchCtx.Search(pWorkItem, pOriginSegment, fLastPass, allowableTypes);
        }

        /// <summary>
        ///     Performs a yielding search for work for the given virtual processor.
        /// </summary>
        bool SearchForWorkInYield(WorkItem *pWork, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass)
        {
            return m_searchCtx.YieldingSearch(pWork, pOriginSegment, fLastPass);
        }

        /// <summary>
        ///     Performs a yielding search for work for the given virtual processor only allowing certain types of work to be found.
        /// </summary>
        bool SearchForWorkInYield(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes)
        {
            return m_searchCtx.YieldingSearch(pWorkItem, pOriginSegment, fLastPass, allowableTypes);
        }

        /// <summary>
        ///     Stub called in SFW before we search for runnable contexts.
        /// </summary>
        /// <returns>
        ///     A context which should be run.
        /// </returns>
        virtual InternalContextBase *PreRunnableSearch()
        {
            return NULL;
        }

        /// <summary>
        ///     Called when the context running atop this virtual processor has reached a safe point.
        /// </summary>
        /// <returns>
        ///     An indication of whether the caller should perform a commit.
        /// </returns>
        bool SafePoint();

        /// <summary>
        ///     Gets a location object which represents the virtual processor.
        /// </summary>
        const location& GetLocation() const
        {
            return m_location;
        }

        //
        // TRANSITION: This goes with real priority.
        //
        void MarkGrabbedPriority()
        {
            m_fShortAffine = true;
        }

        /// <summary>
        ///     Marks the segment as serviced at a particular time mark.
        /// </summary>
        void ServiceMark(ULONGLONG markTime)
        {
            //
            // Avoid cache contention on this by only writing the service time every so often.  We only care about this on granularities of something
            // like 1/2 seconds anyway -- it's effectively the priority boost time granularity that we care about.
            //
            if (TimeSinceServicing(markTime) > 100)
                m_lastServiceTime = markTime;
        }

        /// <summary>
        ///    Returns the time delta between the last service time and the passed service time.
        /// </summary>
        ULONG TimeSinceServicing(ULONGLONG markTime)
        {
            return (ULONG)(markTime - m_lastServiceTime);
        }

        /// <summary>
        ///     Returns a segment from its internal list entry.
        /// </summary>
        static VirtualProcessor* FromBoostEntry(BoostedObject *pEntry)
        {
            return CONTAINING_RECORD(pEntry, VirtualProcessor, m_priorityServiceLink);
        }

    protected:
        //
        // protected data
        //

        // Indicates whether vproc is available to perform work.
        volatile AvailabilityType m_availabilityType;

        // Indicates how the current claimant acquired the vproc.
        AvailabilityType m_claimantType;

        // Local caching of realized chores/contexts
        StructuredWorkStealingQueue<InternalContextBase, _HyperNonReentrantLock> m_localRunnableContexts;

        // The search context which keeps track of where this virtual processor is in a search-for-work regardless of algorithm.
        WorkSearchContext m_searchCtx;

        // Owning scheduling node
        SchedulingNode *m_pOwningNode;

        // Owning ring - ring associated with the owning node
        SchedulingRing *m_pOwningRing;

        // Owning virtual processor root
        IVirtualProcessorRoot *m_pOwningRoot;

        // Sub allocator
        SubAllocator *m_pSubAllocator;

        // Tracks the types of work that the virtual processor is currently operating on.
        bool m_fAffine;
        bool m_fLocal;

        // Once m_fAffine is clear, we need to post ONE notification to make sure that an affine search happens AFTER
        // m_fAffine is set.  This tracks the need for the ONE search.
        bool m_fShortAffine;

        // The index that this Virtual Processor is at in its list array
        int m_listArrayIndex;

        // Statistics data counters
        unsigned int m_enqueuedTaskCounter;
        unsigned int m_dequeuedTaskCounter;

        // Statistics data checkpoints
        unsigned int m_enqueuedTaskCheckpoint;
        unsigned int m_dequeuedTaskCheckpoint;

        // The context pushed to startup this virtual processor.
        InternalContextBase *m_pPushContext;

        // Internal context that is affinitized to and executing on this virtual processor
        // This is always an InternalContextBase except on UMS in very special circumstances.
        IExecutionContext * volatile m_pExecutingContext;

        // The context which made the virtual processor available.
        IExecutionContext * m_pAvailableContext;

        _HyperNonReentrantLock m_lock;

        // Unique identifier for vprocs within a scheduler.
        unsigned int m_id;

        // The execution resource id for this virtual processor.
        unsigned int m_resourceId;

        // The mask id assigned by the scheduler to this virtual processor.
        unsigned int m_maskId;

        // Stashed location for this virtual processor.
        location m_location;

        // The resource mask for this virtual processor.
        QuickBitSet m_resourceMask;

        // Flag specifying whether this is a virtual processor created as a result of a call to Oversubscribe.
        bool m_fOversubscribed;

        // Flag that is set when the virtual processor should remove itself from the scheduler at a yield point,
        // i.e, either when the context executing on it calls Block or Yield, or when it is in the dispatch loop
        // looking for work.
        bool m_fMarkedForRetirement;

        // Whether this virtual processor has reached a safe point in the code
        // Used to demark when all virtual processors have reached safe points and a list array deletion
        // can occur
        bool m_fReachedSafePoint;

        /// <summary>
        ///     Causes the virtual processor to remove itself from the scheduler. This is used either when oversubscription
        ///     ends or when the resource manager asks the vproc to retire.
        /// </summary>
        virtual void Retire();

        // The internal context that caused this virtual processor to be created, if this is an oversubscribed vproc.
        InternalContextBase * m_pOversubscribingContext;

        /// <summary>
        ///     Affinitizes an internal context to the virtual processor.
        /// </summary>
        /// <param name="pContext">
        ///     The internal context to affinitize.
        /// </param>
        virtual void Affinitize(InternalContextBase *pContext);

        /// <summary>
        ///     Returns a type-cast of pContext to an InternalContextBase or NULL if it is not.
        /// </summary>
        virtual InternalContextBase *ToInternalContext(IExecutionContext *pContext);

        /// <summary>
        ///     Initializes the virtual processor. This API is called by the constructor, and when a virtual processor is to
        ///     be re-initialized, when it is pulled of the free pool in the list array.
        /// </summary>
        /// <param name="pOwningNode">
        ///     The owning schedule node for this virtual processor
        /// </param>
        /// <param name="pOwningRoot">
        ///     The owning IVirtualProcessorRoot
        /// </param>
        virtual void Initialize(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

        /// <summary>
        ///     Makes a virtual processor available for scheduling work.
        /// </summary>
        /// <param name="fCanChangeActiveState">
        ///     Indicates whether the routine should update active state for the vproc based upon type or not.
        /// </param>
        void MakeAvailable(AvailabilityType type, bool fCanChangeActiveState = true);

    private:
        friend class SchedulerBase;
        friend class ContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class UMSThreadInternalContext;
        friend class UMSSchedulingContext;
        friend class ScheduleGroup;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class SchedulingNode;
        friend class WorkSearchContext;
        friend class ClaimTicket;
        template <class T> friend class ListArray;
        template<class T, class Counter> friend class List;

        // Links for throttling
        VirtualProcessor *m_pNext;
        VirtualProcessor *m_pPrev;

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

        // The safe point marker.
        SafePointMarker m_safePointMarker;

        // The last time this virtual processor's LRC was serviced.
        ULONGLONG m_lastServiceTime;

        //
        // TRANSITION: This is a temporary patch on livelock until we can tie into priority for livelock prevention.
        //
        bool m_prioritized; // Under m_priorityServiceLink's lock.
        BoostedObject m_priorityServiceLink;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

        /// <summary>
        ///     Exercises a claim over the virtual processor.
        /// </summary>
        bool ExerciseClaim(AvailabilityType type, ScheduleGroupSegmentBase *pSegment, InternalContextBase *pContext = NULL);

        /// <summary>
        ///     Start a worker context executing on this.virtual processor.
        /// </summary>
        virtual bool StartupWorkerContext(ScheduleGroupSegmentBase* pSegment, InternalContextBase *pContext = NULL);

        /// <summary>
        ///     Oversubscribes the virtual processor by creating a new virtual processor root affinitized to the same
        ///     execution resource as that of the current root
        /// </summary>
        /// <returns>
        ///     A virtual processor that oversubscribes this one.
        /// </returns>
        virtual VirtualProcessor * Oversubscribe();

        /// <summary>
        ///     Marks the virtual processor such that it removes itself from the scheduler, once the context it is executing
        ///     reaches a safe yield point. Alternatively, if the context has not started executing yet, it can be retired immediately.
        /// </summary>
        void MarkForRetirement();

#if _UMSTRACE
        void TraceSearchedLocalRunnables();
#endif // _UMSTRACE

        /// <summary>
        ///     Steals a context from the local runnables queue of this virtual processor.
        /// </summary>
        InternalContextBase *StealLocalRunnableContext()
        {
#if _UMSTRACE
            TraceSearchedLocalRunnables();
#endif // _UMSTRACE

            InternalContextBase *pContext = NULL;
            if (!m_localRunnableContexts.Empty())
            {
                pContext = m_localRunnableContexts.Steal();
            }

            if (pContext != NULL)
            {
#if defined(_DEBUG)
                ::Concurrency::details::SetContextDebugBits(pContext, CTX_DEBUGBIT_STOLENFROMLOCALRUNNABLECONTEXTS);
#endif // _DEBUG
            }

            return pContext;
        }

        /// <summary>
        ///     Pops a runnable context from the local runnables queue of the vproc, if it can find one.
        /// </summary>
        InternalContextBase *GetLocalRunnableContext()
        {
#if _UMSTRACE
            TraceSearchedLocalRunnables();
#endif // _UMSTRACE

            if (m_localRunnableContexts.Count() > 0) // Is this check worthwhile?  Yes, I believe.  We'd take a fence to check otherwise.
            {
                InternalContextBase *pContext = m_localRunnableContexts.Pop();
#if defined(_DEBUG)
                ::Concurrency::details::SetContextDebugBits(pContext, CTX_DEBUGBIT_POPPEDFROMLOCALRUNNABLECONTEXTS);
#endif // _DEBUG
                return pContext;
            }
            return NULL;
        }

        /// <summary>
        ///     Resets the count of work coming in.
        /// </summary>
        /// <remarks>
        ///     This function is using modulo 2 behavior of unsigned ints to avoid overflow and
        ///     reset problems. For more detail look at ExternalStatistics class in externalcontextbase.h.
        /// </remarks>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetEnqueuedTaskCount()
        {
            unsigned int currentValue = m_enqueuedTaskCounter;
            unsigned int retVal = currentValue - m_enqueuedTaskCheckpoint;

            // Update the checkpoint value with the current value
            m_enqueuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Resets the count of work being done.
        /// </summary>
        /// <remarks>
        ///     This function is using modulo 2 behavior of unsigned ints to avoid overflow and
        ///     reset problems. For more detail look at the ExternalStatistics class in externalcontextbase.h.
        /// </remarks>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetDequeuedTaskCount()
        {
            unsigned int currentValue = m_dequeuedTaskCounter;
            unsigned int retVal = currentValue - m_dequeuedTaskCheckpoint;

            // Update the checkpoint value with the current value
            m_dequeuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Send a virtual processor ETW event
        /// </summary>
        void TraceVirtualProcessorEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD vprocId)
        {
            if (g_TraceInfo._IsEnabled(level, VirtualProcessorEventFlag))
                ThrowVirtualProcessorEvent(eventType, level, schedulerId, vprocId);
        }

        /// <summary>
        ///     Send a virtual processor ETW event
        /// </summary>
        static void ThrowVirtualProcessorEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD vprocId);
    };
} // namespace details
} // namespace Concurrency
