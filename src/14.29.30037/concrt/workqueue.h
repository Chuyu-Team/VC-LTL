// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// workqueue.h
//
// Work stealing queues pair implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
#define QUEUE_ATTACHED 0
#define QUEUE_DETACHED 1

    /// <summary>
    ///     The work queue is a pair of bound work stealing queues, one structured and one unstructured, that
    ///     can be associated with a context.
    /// </summary>
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class WorkQueue
    {
    public:

        /// <summary>
        ///     Constructs a new work queue.
        /// </summary>
        WorkQueue();

        //
        // Queue Reuse:
        //

        /// <summary>
        ///     Informs the WSQ what context it's attached to in a synchronized manner.
        /// </summary>
        void LockedSetOwningContext(ContextBase *pOwningContext)
        {
            m_lock._Acquire();
            m_pOwningContext = pOwningContext;
            m_lock._Release();
        }

        /// <summary>
        ///     Informs the WSQ what context it's attached to.
        /// </summary>
        void SetOwningContext(ContextBase *pOwningContext)
        {
            m_pOwningContext = pOwningContext;
        }

        //
        // Structured Parallelism:
        //

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the structured work stealing queue
        /// </param>
        /// <param name="affinitySlot">
        ///     The mailbox slot into which the specified chore has already been queued for explicit affinitization.  The slot can be an empty
        ///     slot if the chore was not queued anywhere.
        /// </param>
        void PushStructured(_UnrealizedChore *pChore, Mailbox<_UnrealizedChore>::Slot affinitySlot)
        {
            m_structuredQueue.Push(pChore, affinitySlot);
        }

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the structured work stealing queue
        /// </param>
        void PushStructured(_UnrealizedChore *pChore)
        {
            m_structuredQueue.Push(pChore);
        }

        /// <summary>
        ///     Pops the topmost chore from the work stealing queue for unstructured parallelism.  Failure
        ///     to pop typically indicates stealing.
        /// </summary>
        /// <returns>
        ///     An unrealized chore from the structured work stealing queue or NULL if none is present
        /// </returns>
        _UnrealizedChore* PopStructured()
        {
            return m_structuredQueue.Pop();
        }

        /// <summary>
        ///     Returns whether the structured work stealing queue is empty.
        /// </summary>
        bool IsStructuredEmpty() const
        {
            return m_structuredQueue.Empty();
        }

        //
        // Unstructured Parallelism:
        //

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.  The returned
        ///     value is a cookie which can be used in a call to TryPopUnstructured.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the unstructured work stealing queue
        /// </param>
        /// <param name="affinitySlot">
        ///     The mailbox slot into which the specified chore has already been queued for explicit affinitization.  The slot can be an empty
        ///     slot if the chore was not queued anywhere.
        /// </param>
        /// <returns>
        ///     A cookie which can be used to identify the chore for a later TryPopUnstructured call
        /// </returns>
        int PushUnstructured(_UnrealizedChore *pChore, Mailbox<_UnrealizedChore>::Slot affinitySlot)
        {
            return m_unstructuredQueue.Push(pChore, affinitySlot);
        }

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.  The returned
        ///     value is a cookie which can be used in a call to TryPopUnstructured.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the unstructured work stealing queue
        /// </param>
        /// <returns>
        ///     A cookie which can be used to identify the chore for a later TryPopUnstructured call
        /// </returns>
        int PushUnstructured(_UnrealizedChore *pChore)
        {
            return m_unstructuredQueue.Push(pChore);
        }

        /// <summary>
        ///     Attempts to pop the chore specified by a cookie value from the unstructured work stealing queue.  Failure
        ///     to pop typically indicates stealing.
        /// </summary>
        /// <param name="cookie">
        ///     A cookie returned from PushUnstructured indicating the chore to attempt to pop from
        ///     the unstructured work stealing queue
        /// </param>
        /// <returns>
        ///     The specified unrealized chore (as indicated by cookie) or NULL if it could not be popped from
        ///     the work stealing queue
        /// </returns>
        _UnrealizedChore *TryPopUnstructured(int cookie)
        {
            return m_unstructuredQueue.TryPop(cookie);
        }

        /// <summary>
        ///     Attempts to steal an unrealized chore from the unstructured work stealing queue.
        /// </summary>
        /// <param name="fForceStealLocalized">
        ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
        ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
        ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
        /// </param>
        /// <returns>
        ///     An unrealized chore stolen from the work stealing queues or NULL if no such chore can be stolen
        /// </returns>
        _UnrealizedChore *Steal(bool fForceStealLocalized);

        /// <summary>
        ///     Attempts to steal an unrealized chore from the unstructured work stealing queue.
        /// </summary>
        /// <param name="fForceStealLocalized">
        ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
        ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
        ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
        /// </param>
        /// <param name="fSuccessfullyAcquiredLock">
        ///     The try lock was successfully acquired.
        /// </param>
        /// <returns>
        ///     An unrealized chore stolen from the work stealing queues or NULL if no such chore can be stolen
        /// </returns>
        _UnrealizedChore *TryToSteal(bool fForceStealLocalized, bool& fSuccessfullyAcquiredLock);

        /// <summary>
        ///     Returns whether the unstructured work stealing queue is empty.
        /// </summary>
        bool IsUnstructuredEmpty() const
        {
            return m_unstructuredQueue.Empty();
        }

        /// <summary>
        ///     Sweeps the unstructured work stealing queue for items matching a predicate and potentially removes them
        /// based on the result of a callback.
        /// </summary>
        /// <param name="pPredicate">
        ///     The predicate for things to call pSweepFn on.
        /// </param>
        /// <param name="pData">
        ///     The data for the predicate and sweep callback
        /// </param>
        /// <param name="pSweepFn">
        ///     The sweep function
        /// </param>
        void SweepUnstructured(WorkStealingQueue<_UnrealizedChore>::SweepPredicate pPredicate,
                               void *pData,
                               WorkStealingQueue<_UnrealizedChore>::SweepFunction pSweepFn);

        /// <summary>
        ///     Called in order to mark this work queue as detached so that we know how far it's legal to steal up the work
        ///     queue should it become reattached to context with active cancellation.
        /// </summary>
        void MarkDetachment()
        {
            //
            // We only detach unstructured queues.
            //
            m_unstructuredQueue.MarkDetachment();
        }

        //
        // Both:
        //

        /// <summary>
        ///     Returns the id of the work queue.
        /// </summary>
        unsigned int Id() const { return m_id; }

        /// <summary>
        ///     Returns whether the both work stealing queues are empty.
        /// </summary>
        bool IsEmpty() const
        {
            return m_structuredQueue.Empty() &&  m_unstructuredQueue.Empty();
        }

        /// <summary>
        ///     Sets the queue to a detached state.
        /// </summary>
        void SetDetached(bool fDetached)
        {
            if (fDetached)
                MarkDetachment();

            InterlockedExchange(&m_detachmentState, fDetached ? QUEUE_DETACHED : QUEUE_ATTACHED);
        }

        /// <summary>
        ///     Queries whether the queue is detached.
        /// </summary>
        bool IsDetached() const
        {
            return (m_detachmentState == QUEUE_DETACHED);
        }

        /// <summary>
        ///     Causes a detached work queue to release its reference on the inpassed schedule group and remove itself from that schedule group's
        ///     list of work queues at the next available safe point.
        /// </summary>
        void RetireAtSafePoint(ScheduleGroupSegmentBase *pSegment);

        /// <summary>
        ///     Causes a detached work queue to redetach due to roll-back of retirement at the next available safe point.
        /// </summary>
        void RedetachFromScheduleGroupAtSafePoint(ScheduleGroupSegmentBase *pSegment);

        /// <summary>
        ///     Indicates whether the steal lock is held.
        /// </summary>
        bool IsLockHeld() const
        {
            return m_lock._IsLockHeld();
        }

    private:
        friend class ContextBase;
        friend class ScheduleGroupBase;
        friend class ScheduleGroupSegmentBase;
        template <class T> friend class ListArray;
        template<class T> friend void _InternalDeleteHelper(T*);

        // structured work stealing
        StructuredWorkStealingQueue<_UnrealizedChore, _CriticalNonReentrantLock> m_structuredQueue;

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

        // The safe point invocation which will perform a release of schedule group held by a detached WSQ.
        SafePointInvocation m_detachmentSafePoint;

        // Tracking for detachment
        ListArrayInlineLink<WorkQueue> m_detachment;
        volatile long m_detachmentState;
        ScheduleGroupSegmentBase *m_pDetachedSegment;

        // The unique identifier for the work queue.  This is the final level of binding between a task collection and a work queue.
        unsigned int m_id;

        // The index this workqueue appears at in its list array
        int m_listArrayIndex;

        // The context which owns the WSQ.  NOTE: Any utilization of this must be capture/use as it will change outside the scope
        // of the WSQ list lock.
        ContextBase *m_pOwningContext;

        // Unstructured work stealing
        WorkStealingQueue<_UnrealizedChore, _CriticalNonReentrantLock> m_unstructuredQueue;

        // External lock for unstructured work stealing
        _CriticalNonReentrantLock m_lock;

        // Reinitialize a work queue pulled from a free pool
        void Reinitialize();

        // steal helper
        _UnrealizedChore *UnlockedSteal(bool fForceStealLocalized);

        /// <summary>
        ///     Retires the detached work queue.
        /// </summary>
        static void StaticRetire(WorkQueue *pQueue);

        /// <summary>
        ///     Places the work queue back in a detached state on roll back.
        /// </summary>
        static void StaticRedetachFromScheduleGroup(WorkQueue *pQueue);
    };
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
