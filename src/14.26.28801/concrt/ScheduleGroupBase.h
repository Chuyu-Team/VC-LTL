// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ScheduleGroupBase.h
//
// Header file containing ScheduleGroup related declarations.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#define MAILBOX_LOCATION (reinterpret_cast<WorkQueue *>(1))

namespace Concurrency
{
namespace details
{
    //
    // A ScheduleGroupBase* object represents a schedule group as defined in the public API set.  It is a container of work which is related
    // and benefits from temporally and spatially close scheduling.
    //
    // A ScheduleGroupSegmentBase* object represents a segment of a schedule group with affinity to a particular location -- in this case affinity
    // to a particular scheduling node.
    //

    class ScheduleGroupBase;

    /// <summary>
    ///     A piece of a schedule group which is uniquely assigned to a given scheduling node/ring.
    /// </summary>
    class ScheduleGroupSegmentBase
    {
    public:

        /// <summary>
        ///     Called by a work queue in order to retire itself at a safe point.
        /// </summary>
        void RetireDetachedQueue(WorkQueue *pWorkQueue);

        /// <summary>
        ///     Called by a work queue in order to roll back an attempted kill that could not be committed due to reuse.
        /// </summary>
        void RedetachQueue(WorkQueue *pWorkQueue);

        /// <summary>
        ///     Destroys a schedule group segment.
        /// </summary>
        virtual ~ScheduleGroupSegmentBase();

        /// <summary>
        ///     Returns the group to which this segment belongs.
        /// </summary>
        ScheduleGroupBase *GetGroup() const
        {
            return m_pOwningGroup;
        }

        /// <summary>
        ///     Returns the scheduling ring to which this segment belongs.
        /// </summary>
        SchedulingRing *GetSchedulingRing() const
        {
            return m_pRing;
        }

        /// <summary>
        ///     Schedules a realized (non workstealing) chore in the schedule group segment.  Used to schedule light-weight
        ///     tasks and agents.
        /// </summary>
        void ScheduleTask(TaskProc proc, void* data);

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

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
            {
                OMTRACE(MTRACE_EVT_MARK, this, NULL, NULL, markTime);
                m_lastServiceTime = markTime;
            }
        }

        /// <summary>
        ///    Returns the time delta between the last service time and the passed service time.
        /// </summary>
        ULONG TimeSinceServicing(ULONGLONG markTime)
        {
            return (ULONG) (markTime - m_lastServiceTime);
        }

        /// <summary>
        ///     Returns a segment from its internal list entry.
        /// </summary>
        static ScheduleGroupSegmentBase* FromBoostEntry(BoostedObject *pEntry)
        {
            return CONTAINING_RECORD(pEntry, ScheduleGroupSegmentBase, m_priorityServiceLink);
        }

        /// <summary>
        ///     Notifies virtual processors that work affinitized to them has become available in the schedule group segment.
        /// </summary>
        virtual void NotifyAffinitizedWork() =0;

    protected:

        //
        // Private methods
        //

        /// <summary>
        ///     Constructs a new schedule group segment with a specific affinity in the specified ring.
        /// </summary>
        /// <param name="pOwningGroup">
        ///     The group to which this segment belongs.
        /// </param>
        /// <param name="pOwningRing">
        ///     The ring in which this segment is contained.
        /// </param>
        /// <param name="pSegmentAffinity">
        ///     The affinity of this segment.
        /// </param>
        ScheduleGroupSegmentBase(ScheduleGroupBase *pOwningGroup, SchedulingRing *pOwningRing, location* pSegmentAffinity);

        /// <summary>
        ///     Initializes a schedule group segment.
        /// </summary>
        /// <param name="pOwningGroup">
        ///     The group to which this segment belongs.
        /// </param>
        /// <param name="pOwningRing">
        ///     The ring in which this segment is contained.
        /// </param>
        /// <param name="pSegmentAffinity">
        ///     The affinity of this segment.
        /// </param>
        void Initialize(ScheduleGroupBase *pOwningGroup, SchedulingRing *pOwningRing, location *pSegmentAffinity);

        /// <summary>
        ///     Adds runnable context to the schedule group. This is usually a previously blocked context that
        ///     was subsequently unblocked, but it could also be an internal context executing chores on behalf
        ///     of an external context.
        /// </summary>
        void AddRunnableContext(InternalContextBase *pContext, location bias = location());

        /// <summary>
        ///     Puts a runnable context into the runnables collection in the schedule group.
        /// </summary>
        virtual void AddToRunnablesCollection(InternalContextBase *pContext) =0;

        virtual InternalContextBase *GetRunnableContext() = 0;

        /// <summary>
        ///     Returns true if the group has any realized chores.
        ///     This is used during scheduler finalization when only one thread is active in the scheduler.
        ///     At any other time, this information is stale since new work could get added to the scheduler.
        /// </summary>
        bool HasRealizedChores() const;

        /// <summary>
        ///     Returns the first work queue in the schedule group that has unrealized chores.
        ///     This is only stable during scheduler finalization when only one thread is active in the scheduler.
        ///     At any other time, this information is stale since new work could get added to the scheduler.
        /// </summary>
        /// <returns>
        ///     This method either returns a special constant MAILBOX_LOCATION if work was found in the mailbox or
        ///     a work queue in which an unrealized chore was found.
        /// </returns>
        WorkQueue *LocateUnrealizedChores();

        /// <summary>
        ///     Returns true if any of the workqueues in the schedule group has unrealized chores.
        ///     This is only stable during scheduler finalization when only one thread is active in the scheduler.
        ///     At any other time, this information is stale since new work could get added to the scheduler.
        /// </summary>
        bool HasUnrealizedChores();

        /// <summary>
        ///     Returns a realized chore if one exists in the queue.
        /// </summary>
        RealizedChore *GetRealizedChore();

        /// <summary>
        ///     Acquires an internal context for execution
        /// </summary>
        InternalContextBase* GetInternalContext(_Chore *pChore = NULL, bool choreStolen = false);

        /// <summary>
        ///     Releases an internal context after execution
        /// </summary>
        void ReleaseInternalContext(InternalContextBase *pContext);

        /// <summary>
        ///     Steals an unrealized chore from a workqueue in the schedule group.
        /// </summary>
        /// <param name="fForceStealLocalized">
        ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
        ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
        ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
        /// </param>
        _UnrealizedChore* StealUnrealizedChore(bool fForceStealLocalized = false);

        /// <summary>
        ///     Attempts to acquire a detached work queue from the schedule group.  If such a work queue is found, it
        ///     is removed from detached queue and returned.  This allows recycling of work queues that are detached
        ///     yet still have unstructured work.
        ///</summary>
        WorkQueue *GetDetachedWorkQueue();

        /// <summary>
        ///     Places a work queue in the detached queue.  This will cause the work queue to remain eligible for stealing
        ///     while the queue can be detached from a context.  The work queue will be recycled and handed back to a
        ///     context executing within the schedule group that needs
        ///     a queue.  If the queue is not recycled, it will be abandoned and freed when it becomes empty (a steal on it
        ///     while in detached mode fails).
        /// </summary>
        void DetachActiveWorkQueue(WorkQueue *pWorkQueue);

        /// <summary>
        ///     Called to safely delete a detached work queue -- this is lock free and utilizes safe points to perform
        ///     the deletion and dereference.  It can be called during the normal SFW or during the finalization sweep
        ///     safely.
        /// </summary>
        bool SafelyDeleteDetachedWorkQueue(WorkQueue *pQueue);

        /// <summary>
        ///     Returns a location representing the affinity of this segment.  Note that if the location returned is the system location,
        ///     the segment has no specific placement affinity.  It may still have a weaker natural affinity to a particular node by
        ///     nature of the fact that a segment is contained within a ring.
        /// </summary>
        const location& GetAffinity() const
        {
            return m_affinity;
        }

        /// <summary>
        ///     Returns our cached affinity set.
        /// </summary>
        const QuickBitSet& GetAffinitySet() const
        {
            return m_affinitySet;
        }

        /// <summary>
        ///     Removes the segment.
        /// </summary>
        void Remove();

        //
        // Private data
        //

        // Owning ring.
        SchedulingRing *m_pRing;

        // A location representing the affinity of this segment.
        location m_affinity;

        // The bitset representing m_affinity for quick masking.
        QuickBitSet m_affinitySet;

        // Quickly stashed maskId if the location for this segment is a core.
        unsigned int m_maskIdIf;

        // Each schedule group has three stores of work. It has a collection of runnable contexts (in the derived classes),
        // a FIFO queue of realized chores and a list of workqueues that hold unrealized chores.

        // A queue of realized chores.
        SafeSQueue<RealizedChore, _HyperNonReentrantLock> m_realizedChores;

        // The list of tasks which were mailed to this segment.
        Mailbox<_UnrealizedChore> m_mailedTasks;

        // A list array of all unrealized chore queues that are owned by contexts in this schedule group,
        // protected by a r/w lock.
        ListArray<WorkQueue> m_workQueues;

        // A list array of work queues which still contain work within the schedule group but have become detached
        // from their parent context (e.g.: a chore queues unstrucured work and does not wait upon it before
        // exiting).  This is the first level "free list".  Any context needing a work queue can grab one from
        // here assuming it's executing the same schedule group.
        ListArray< ListArrayInlineLink<WorkQueue> > m_detachedWorkQueues;

        // The index that this schedule group is at in its containing ListArray
        int m_listArrayIndex;

        // Unique identifier
        unsigned int m_id;

        // The group to which this segment belongs
        ScheduleGroupBase *m_pOwningGroup;

        // The next segment within the group
        ScheduleGroupSegmentBase * volatile m_pNext;

        // The last time this segment was serviced.
        ULONGLONG m_lastServiceTime;

        //
        // TRANSITION: This is a temporary patch on livelock until we can tie into priority for livelock prevention.
        //
        BoostedObject m_priorityServiceLink;

    private:

        friend class SchedulerBase;
        friend class ScheduleGroupBase;
        friend class ContextBase;
        friend class ExternalContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class SchedulingNode;
        friend class SchedulingRing;
        friend class VirtualProcessor;
        friend class UMSSchedulingContext;
        friend class WorkItem;
        friend class WorkSearchContext;
        template <class T> friend class ListArray;

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

    };

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class ScheduleGroupBase : public ScheduleGroup
    {
    public:
        //
        // Public Methods
        //

        /// <summary>
        ///     Constructs a schedule group.
        /// </summary>
        ScheduleGroupBase(SchedulerBase *pScheduler, location* pGroupPlacement);

        /// <summary>
        ///     Virtual destructor
        /// </summary>
        virtual ~ScheduleGroupBase()
        {
        }

        /// <summary>
        ///     Performs initialization (or reinitialization) of a schedule group.
        /// </summary>
        void Initialize(location* pGroupPlacement);

        /// <summary>
        ///     Locates a segment that is appropriate for scheduling a task within the schedule group given information about the task's placement
        ///     and the origin of the thread making the call.
        /// </summary>
        /// <param name="pSegmentAffinity">
        ///     A segment with affinity to this particular location will be located.
        /// </paaram>
        /// <param name="fCreateNew">
        ///     An indication as to whether the schedule group can create a new segment if an appropriate segment cannot be found.  If this parameter is
        ///     specified as true, NULL will never be returned from this method; otherwise, it can be if no matching segment can be found.
        /// </param>
        /// <returns>
        ///     A segment appropriate for scheduling work with affinity to segmentAffinity from code executing at origin.  Note that NULL may be returned
        ///     if fCreateNew is specified as false and no appropriate segment yet exists for the group.
        /// </returns>
        virtual ScheduleGroupSegmentBase *LocateSegment(location* pSegmentAffinity, bool fCreateNew);

        /// <summary>
        ///     Returns a pointer to the scheduler this group belongs to.
        /// </summary>
        SchedulerBase * GetScheduler() { return m_pScheduler; }

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

        // ***************************************************************************
        //
        // Public Interface Derivations:
        //

        /// <summary>
        ///     Returns a unique identifier to the schedule group.
        /// </summary>
        unsigned int Id() const
        {
            return m_id;
        }

        /// <summary>
        ///     Increments the reference count of a schedule group.  A reference count is held for
        ///       - every unstarted or incomplete realized chore that is part of the schedule group
        ///       - every context that is executing a chore that was stolen from an unrealized chore queue
        ///         within the schedule group
        ///       - every external context attached to the scheduler instance, IFF this is the anonymous
        ///         schedule group for the scheduler instance,
        ///       - an external caller, IFF this schedule group was created using one of the public task
        ///         creation APIs.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Reference()
        {
            return (unsigned int)InternalReference();
        }

        /// <summary>
        ///     Decrements the reference count of a schedule group.  Used for composition.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Release()
        {
            return (unsigned int)InternalRelease();
        }

        /// <summary>
        ///     Schedules a light-weight task within the schedule group.
        /// </summary>
        /// <param name="proc">
        ///     A pointer to the function to execute to perform the body of the light-weight task.
        /// </param>
        /// <param name="data">
        ///     A void pointer to the data that will be passed as a parameter to the body of the task.
        /// </param>
        /// <remarks>
        ///     Calling the <c>ScheduleTask</c> method implicitly places a reference count on the schedule group which is removed by the runtime
        ///     at an appropriate time after the task executes.
        /// </remarks>
        /// <seealso cref="ScheduleGroup::Reference Method"/>
        virtual void ScheduleTask(TaskProc proc, void* data);
        //
        // End of Public Interface Derivations:
        //
        // ***************************************************************************

        /// <summary>
        ///     Schedules a light-weight task within the schedule group.  The light-weight task will also be biased towards executing at the specified location.
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
        /// <remarks>
        ///     Calling the <c>ScheduleTask</c> method implicitly places a reference count on the schedule group which is removed by the runtime
        ///     at an appropriate time after the task executes.
        /// </remarks>
        /// <seealso cref="ScheduleGroup::Reference Method"/>
        /// <seealso cref="location Class"/>
        void ScheduleTask(TaskProc proc, void * data, location& placement);

        /// <summary>
        ///     Places a chore in a mailbox associated with the schedule group which is biased towards tasks being picked up from the specified
        ///     location.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to mail.
        /// </param>
        /// <param name="pPlacement">
        ///     A pointer to a location where the chore will be mailed.
        /// </param>
        /// <returns>
        ///     The mailbox slot into which the chore was placed.
        /// </returns>
        /// <remarks>
        ///     A mailed chore should also be placed on its regular work stealing queue.  The mailing must come first and once mailed, the chore body
        ///     cannot be referenced until the slot is successfully claimed via a call to the ClaimSlot method.
        /// </remarks>
        virtual Mailbox<_UnrealizedChore>::Slot MailChore(_UnrealizedChore * pChore,
                                                          location * pPlacement,
                                                          ScheduleGroupSegmentBase ** ppDestinationSegment) =0;

        /// <summary>
        ///     Gets the first schedule group segment within the group that is either affine or non-affine as specified by fAffine.
        /// </summary>
        ScheduleGroupSegmentBase *GetFirstScheduleGroupSegment(bool fAffine)
        {
            return fAffine ? m_pAffineSegments : m_pNonAffineSegments;
        }

        /// <summary>
        ///     Gets the next schedule group segment within the group.  This will return only affine or non-affine segments depending
        ///     on how GetFirstScheduleGroupSegment was called.
        /// </summary>
        ScheduleGroupSegmentBase *GetNextScheduleGroupSegment(ScheduleGroupSegmentBase *pSegment)
        {
            return pSegment->m_pNext;
        }

    protected:
        friend class ScheduleGroupSegmentBase;

        friend class SchedulerBase;
        friend class ContextBase;
        friend class ExternalContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class SchedulingNode;
        friend class SchedulingRing;
        friend class VirtualProcessor;
        friend class UMSSchedulingContext;
        friend class WorkItem;
        friend class WorkSearchContext;
        template <class T> friend class ListArray;

        enum {
            CacheLocalScheduling = 1,
            FairScheduling = 2,
            AnonymousScheduleGroup = 4
        };

        //
        // Private data
        //

        // Owning scheduler
        SchedulerBase *m_pScheduler;

        // The lock which guards creation of segments.
        _NonReentrantLock m_segmentLock;

        // A linked list of explicitly affine segments within this group
        ScheduleGroupSegmentBase *m_pAffineSegments;

        // A linked list of segments which are not explicitly affine.
        ScheduleGroupSegmentBase *m_pNonAffineSegments;

        // Reference count for the schedule group
        volatile long m_refCount;

        // The index that this schedule group is at in its containing ListArray
        int m_listArrayIndex;

        // Unique identifier
        unsigned int m_id;

        // The location that the group schedules to by default.  A non-biased group will contain the system location.
        location m_groupPlacement;

        // flag indicating schedule group kind
        BYTE m_kind;

        //
        // Private methods
        //

        /// <summary>
        ///     Removes all schedule group segments from the group.
        /// </summary>
        virtual void RemoveSegments();

        /// <summary>
        ///     Non-virtual function that increments the reference count of a schedule group.
        /// </summary>
        LONG InternalReference()
        {
            if ((m_kind & AnonymousScheduleGroup) == 0)
            {
                ASSERT(m_refCount >= 0);
                TRACE(TRACE_SCHEDULER, L"ScheduleGroupBase::InternalReference(rc=%d)\n", m_refCount+1);
                LONG val = InterlockedIncrement(&m_refCount);

                OMTRACE(MTRACE_EVT_REFERENCE, this, NULL, NULL, val);

                return val;
            }
            return 0;
        }

        /// <summary>
        ///     Non-virtual function that decrements the reference count of a schedule group.
        /// </summary>
        LONG InternalRelease()
        {
            if ((m_kind & AnonymousScheduleGroup) == 0)
            {
                ASSERT(m_refCount > 0);
                TRACE(TRACE_SCHEDULER, L"ScheduleGroupBase::InternalRelease(rc=%d)\n", m_refCount-1);
                LONG newValue = InterlockedDecrement(&m_refCount);

                OMTRACE(MTRACE_EVT_DEREFERENCE, this, NULL, NULL, newValue);

                if (newValue == 0)
                {
                    RemoveSegments();
                    m_pScheduler->RemoveScheduleGroup(this);
                }
                return newValue;
            }
            return 0;
        }

        bool IsFairScheduleGroup() const { return (m_kind & FairScheduling) != 0; }

        /// <summary>
        ///     Allocates a new cache local schedule group segment within the specified group and ring with the specified affinity.
        /// </summary>
        /// <param name="pSegmentAffinity">
        ///     The affinity for the segment.
        /// </param>
        /// <param name="pOwningRing">
        ///     The scheduling ring to which the newly allocated segment will belong.
        /// </param>
        /// <returns>
        ///     A new cache local schedule group within the specified group and ring with the specified affinity.
        /// </returns>
        virtual ScheduleGroupSegmentBase* AllocateSegment(SchedulingRing *pOwningRing, location* pSegmentAffinity) = 0;

        /// <summary>
        ///     Creates a new segment with the specified affinity within the specified ring.
        /// </summary>
        /// <param name="pSegmentAffinity">
        ///     The affinity of the segment.
        /// </param>
        /// <param name="pOwningRing">
        ///     The ring into which the new segment will be placed.  Some aspect of segmentAffinity must overlap with the node to which this ring
        ///     belongs.
        /// </param>
        /// <returns>
        ///     A new segment with the specified affinity within the specified ring.
        /// </returns>
        ScheduleGroupSegmentBase *CreateSegment(location* pSegmentAffinity, SchedulingRing *pOwningRing);

        /// <summary>
        ///     Internal routine which finds an appropriate segment for a task placement.
        /// </summary>
        /// <param name="pSegmentAffinity">
        ///     A segment with this affinity will be located.
        /// </param>
        /// <param name="pRing">
        ///     A segment with segmentAffinity within this ring will be found.  A given location may be split into multiple segments by node in order
        ///     to keep work local.
        /// </param>
        /// <returns>
        ///     A segment with the specified affinity close to the specified location.
        /// </returns>
        virtual ScheduleGroupSegmentBase *FindSegment(location* pSegmentAffinity, SchedulingRing *pRing);

    private:

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE
    };
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
