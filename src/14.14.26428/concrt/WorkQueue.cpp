// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// workqueue.cpp
//
// Work stealing queues pair implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a new work queue.
    /// </summary>
    WorkQueue::WorkQueue()
        : m_structuredQueue(&m_lock)
        , m_detachmentState(QUEUE_ATTACHED)
        , m_pOwningContext(NULL)
        , m_unstructuredQueue(&m_lock)
    {
        m_detachment.m_listArrayIndex = 0;
        m_detachment.m_pObject = this;
        m_id = SchedulerBase::GetNewWorkQueueId();
    }

    /// <summary>
    ///     Steal a chore from the work stealing queue for unrealized parallelism.
    /// </summary>
    /// <param name="fForceStealLocalized">
    ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
    ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
    ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
    /// </param>
    _UnrealizedChore *WorkQueue::UnlockedSteal(bool fForceStealLocalized)
    {
        if (IsEmpty())
            return NULL;

        //
        // Make certain we do not steal from a canceled context.  The exception handling will properly unwind this.  This avoids a degree on infighting
        // on cancellation.  When a subtree which has stolen chores is canceled, the stealing threads are canceled, unwind, and immediately become available
        // for stealing.  They can easily pick up a region of the tree where exception handling has not unwound to the cancellation point and the exception
        // handlers wind up in a little battle with the stealing threads (the exception handlers shooting down contexts and the contexts stealing different
        // pieces of the tree, wash, rinse, repeat).  In order to avoid *some* of the contention, we simply will refuse to steal from any context which was
        // shot down as part of a subtree cancellation.
        //
        // Further, as an optimization, we do temporarily suspend stealing from a context which has an inflight cancellation -- even if the stealing happens
        // in a different piece of the subtree than being canceled.  This allows us to avoid yet more infighting during a cancellation.  It should be a rare
        // enough event that the suspension of stealing from that context during the in flight cancel shouldn't hurt performance.  Note that this is the difference
        // between the pCheckContext->HasInlineCancellation() and a pCheckContext->IsEntireContextCanceled() below.  The latter would only check fully canceled
        // contexts.  The former does the suspension on contexts with in flight cancellations.
        //
        // Allow stealing to occur from a blocked context as it could be relying on stolen chores to make further progress.
        //

        _UnrealizedChore *pResult = NULL;
        ContextBase *pOwningContext = m_pOwningContext;
        if (pOwningContext != NULL && pOwningContext->HasInlineCancellation() && !pOwningContext->IsSynchronouslyBlocked())
        {
            //
            // There is a scenario that we need to be extraordinarily careful of here.  Normally, we could just ignore the steal.  Unfortunately,
            // it's entirely possible that a detached queue was reused on a context (A) and then A became cancelled while we waited on a task
            // collection that had chores in the detached region.  This would lead to deadlock.  We need to allow stealing from the region of
            // the WSQ which was detached.
            //
            // m_unstructuredQueue may contains tasks from task collections that are not being canceled.  However, we cannot quickly detect the difference. Thus
            // before arbitrary stealing from m_unstructuredQueue, we need to finish cancelation of the task collections that need to be canceled, which is why
            // pOwningContext->HasInlineCancellation() is true.  Cancellation is over when pOwningContext->HasInlineCancellation() is false and then uninhibited
            // stealing from m_unstructuredQueue may occur again.  However, in the meantime, stealing threads may go idle because no work can be found and if
            // m_pOwningContext exits w/o waiting, there may be work left to execute, but all vprocs are idle.  This 'deadlock' is prevented by calling
            // NotifyWorkSkipped() that assures an associated vproc will not go idle without verifying there is no work left by executing this function again.
            //
            if (m_unstructuredQueue.MarkedForDetachment())
                pResult = (_UnrealizedChore *) m_unstructuredQueue.UnlockedSteal(fForceStealLocalized, true);
            if (pResult != NULL)
                pResult->_SetDetached(true);
            else
                static_cast<InternalContextBase *>(SchedulerBase::FastCurrentContext())->NotifyWorkSkipped();
        }
        else
        {
            pResult = (_UnrealizedChore *) m_structuredQueue.UnlockedSteal(fForceStealLocalized);

            //
            // Structured does not need to deal with detachment and cancellation references, simply return the value.
            //
            if (pResult != NULL)
                return pResult;

            pResult = (_UnrealizedChore *) m_unstructuredQueue.UnlockedSteal(fForceStealLocalized, false);

            //
            // If we stole and there is an owning context and it's not a detached steal (which always goes on the task collection list), keep the owning context
            // alive until the wrapper can add the task to the appropriate cancellation list. Since we're holding the workqueue lock, we can safely access m_pOwningContext.
            //  NOTE:  pResult is unstructured.
            //
            if (pResult != NULL)
            {
                if (m_pOwningContext != NULL
                    && reinterpret_cast <ContextBase *> (pResult->_OwningCollection()->_OwningContext()) == m_pOwningContext)
                {
                    m_pOwningContext->ReferenceForCancellation();
                    pResult->_SetDetached(false);
                }
                else
                {
                    pResult->_SetDetached(true);
                }
            }
        }

        return pResult;
    }

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
    _UnrealizedChore *WorkQueue::TryToSteal(bool fForceStealLocalized, bool& fSuccessfullyAcquiredLock)
    {
        _UnrealizedChore *pResult = NULL;
        if (m_lock._TryAcquire())
        {
            __try
            {
                pResult = UnlockedSteal(fForceStealLocalized);
            }
            __finally
            {
                m_lock._Release();
            }
            fSuccessfullyAcquiredLock = true;
        }
        else
            fSuccessfullyAcquiredLock = false;

        return pResult;
    }

    /// <summary>
    ///     Steal a chore from both work stealing queues.
    /// </summary>
    /// <param name="fForceStealLocalized">
    ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
    ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
    ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
    /// </param>
    _UnrealizedChore* WorkQueue::Steal(bool fForceStealLocalized)
    {
        _CriticalNonReentrantLock::_Scoped_lock lockHolder(m_lock);
        return UnlockedSteal(fForceStealLocalized);
    }

    /// <summary>
    ///     Sweeps the unstructured work stealing queue for items matching a predicate and potentially removes them
    ///     based on the result of a callback.
    /// </summary>
    /// <param name="pPredicate">
    ///     The predicate for things to call pSweepFn on.
    /// </param>
    /// <param name="pData">
    ///     The data for the predicate callback
    /// </param>
    /// <param name="pSweepFn">
    ///     The sweep function
    /// </param>
    void WorkQueue::SweepUnstructured(WorkStealingQueue<_UnrealizedChore>::SweepPredicate pPredicate,
                                      void *pData,
                                      WorkStealingQueue<_UnrealizedChore>::SweepFunction pSweepFn
                                      )
    {
        m_unstructuredQueue.Sweep(pPredicate, pData, pSweepFn);
    }

    /// <summary>
    ///     Causes a detached work queue to release its reference on the inpassed schedule group and remove itself from that schedule group's
    ///     list of work queues at the next available safe point.
    /// </summary>
    void WorkQueue::RetireAtSafePoint(ScheduleGroupSegmentBase *pSegment)
    {
        m_pDetachedSegment = pSegment;
        m_detachmentSafePoint.InvokeAtNextSafePoint(reinterpret_cast<SafePointInvocation::InvocationFunction>(&WorkQueue::StaticRetire),
                                                    this,
                                                    pSegment->GetGroup()->GetScheduler());
    }

    /// <summary>
    ///     Causes a detached work queue to redetach due to roll-back of retirement at the next available safe point.
    /// </summary>
    void WorkQueue::RedetachFromScheduleGroupAtSafePoint(ScheduleGroupSegmentBase *pSegment)
    {
        m_pDetachedSegment = pSegment;
        m_detachmentSafePoint.InvokeAtNextSafePoint(reinterpret_cast<SafePointInvocation::InvocationFunction>(&WorkQueue::StaticRedetachFromScheduleGroup),
                                                    this,
                                                    pSegment->GetGroup()->GetScheduler());
    }

    /// <summary>
    ///     Retires the detached work queue.
    /// </summary>
    void WorkQueue::StaticRetire(WorkQueue *pQueue)
    {
        pQueue->m_pDetachedSegment->RetireDetachedQueue(pQueue);
    }

    /// <summary>
    ///     Places the work queue back in a detached state on roll back.
    /// </summary>
    void WorkQueue::StaticRedetachFromScheduleGroup(WorkQueue *pQueue)
    {
        pQueue->m_pDetachedSegment->RedetachQueue(pQueue);
    }

    /// <summary>
    ///     Reinitialize a work queue pulled from a free pool
    /// </summary>
    void WorkQueue::Reinitialize()
    {
        // Grab steal locks, this will prevent other readers from grabbing this work
        // queue while we are reinitializing (h=t=0 is non-atomic, is it worth it to fix?)
        m_lock._Acquire();
        m_id = SchedulerBase::GetNewWorkQueueId();
        // Reinitialize the reused workqueue
        m_structuredQueue.Reinitialize();
        m_unstructuredQueue.Reinitialize();
        m_detachmentState = QUEUE_ATTACHED;
        m_pOwningContext = NULL;
        // Release the work queue locks
        m_lock._Release();
    }

} // namespace details
} // namespace Concurrency
