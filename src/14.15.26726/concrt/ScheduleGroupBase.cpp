// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ScheduleGroupBase.cpp
//
// Implementation file for ScheduleGroupBase.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a schedule group with an initial reference count of 1.
    /// </summary>
    ScheduleGroupBase::ScheduleGroupBase(SchedulerBase *pScheduler, location* pGroupPlacement) :
        m_pScheduler(pScheduler),
        m_pAffineSegments(NULL),
        m_pNonAffineSegments(NULL),
        m_refCount(0)
    {
        Initialize(pGroupPlacement);
        m_id = m_pScheduler->GetNewScheduleGroupId();
    }

    /// <summary>
    ///     Performs initialization (or reinitialization) of a schedule group.
    /// </summary>
    void ScheduleGroupBase::Initialize(location* pGroupPlacement)
    {
        ASSERT(m_refCount == 0);
        m_refCount = 1;
        m_groupPlacement = *pGroupPlacement;

        OMTRACE(MTRACE_EVT_INITIALIZED, this, NULL, NULL, 0);
    }

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
    ScheduleGroupSegmentBase::ScheduleGroupSegmentBase(ScheduleGroupBase *pOwningGroup, SchedulingRing *pOwningRing, location* pSegmentAffinity) :
        m_mailedTasks(pOwningGroup->GetScheduler(), pOwningGroup->GetScheduler()->GetBitSet(&m_affinity)),
        m_workQueues(pOwningGroup->GetScheduler(), 256, 64),
        m_detachedWorkQueues(pOwningGroup->GetScheduler(), 256, ListArray< ListArrayInlineLink<WorkQueue> >::DeletionThresholdInfinite), // No deletion
        m_lastServiceTime(0)
    {
        Initialize(pOwningGroup, pOwningRing, pSegmentAffinity);
    }

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
    void ScheduleGroupSegmentBase::Initialize(ScheduleGroupBase *pOwningGroup, SchedulingRing *pOwningRing, location *pSegmentAffinity)
    {
        m_pOwningGroup = pOwningGroup;
        m_pRing = pOwningRing;
        m_affinity = *pSegmentAffinity;
        m_priorityServiceLink.m_type = BoostedObject::BoostTypeScheduleGroupSegment;
        m_priorityServiceLink.m_boostState = BoostedObject::BoostStateUnboosted;

        m_affinitySet = pOwningGroup->GetScheduler()->GetBitSet(pSegmentAffinity);
        if (pSegmentAffinity->_GetType() == location::_ExecutionResource)
        {
            m_maskIdIf = pOwningGroup->GetScheduler()->GetResourceMaskId(pSegmentAffinity->_GetId());
        }
        m_mailedTasks.Initialize(m_affinitySet);
    }

    /// <summary>
    ///     Creates a new segment with the specified affinity within the specified ring.
    /// </summary>
    /// <param name="pSegmentAffinity">
    ///     The affinity of the segment.
    /// </param>
    /// <param name="pOwningRing">
    ///     The ring into which the new segment will be placed.  Some aspect of pSegmentAffinity must overlap with the node to which this ring
    ///     belongs.
    /// </param>
    /// <returns>
    ///     A new segment with the specified affinity within the specified ring.
    /// </returns>
    ScheduleGroupSegmentBase *ScheduleGroupBase::CreateSegment(location* pSegmentAffinity, SchedulingRing *pOwningRing)
    {
        ScheduleGroupSegmentBase **pSegmentList = pSegmentAffinity->_Is_system() ? &m_pNonAffineSegments : &m_pAffineSegments;

        //
        // At the moment, there is no point in free listing segments -- they are bound to the lifetime of the group.  There should never be
        // anything on the free list except at destruct time.
        //
        ScheduleGroupSegmentBase *pSegment = NULL;

        if (pSegmentAffinity->_Is_system())
        {
            pSegment = pOwningRing->m_nonAffineSegments.PullFromFreePool();
        }
        else
        {
            pSegment = pOwningRing->m_affineSegments.PullFromFreePool();
        }

        if (pSegment == NULL)
        {
            pSegment = AllocateSegment(pOwningRing, pSegmentAffinity);
        }
        else
        {
            pSegment->Initialize(this, pOwningRing, pSegmentAffinity);
        }

        pSegment->m_pNext = *pSegmentList;
        *pSegmentList = pSegment;

        //
        // If this ring is not active yet, make it active.  This would happen for a ring which we have no virtual processors / nodes in but which
        // we've created a segment in.  We would do this if we knew a-priori that we were running on a thread / virtual processor affine to that node
        // and scheduled work to this scheduler from there despite this scheduler not having any virtual processors from that node.  There are two ways
        // in which this might happen:
        //
        // 1: We decide that location::current can return specific locations from an external thread.  Today this is not done.
        // 2: We might be running on scheduler A / node X and schedule work to scheduler B from there.  We're on node X even though scheduler B doesn't
        //    *currently* have any virtual processors there.
        //
        // In either of these cases, we'll still fork the group (create a new segment) within the node/ring even though we have no virtual processors there.
        // After all, we never know when dynamic RM will kick in and change that.
        //
        // Note that this does *NOT* necessarily mean that the work is strongly affine to a non-existent node/ring!
        //
        if (!pOwningRing->IsActive())
        {
            pOwningRing->Activate();
        }

        if (pSegmentAffinity->_Is_system())
        {
            pOwningRing->m_nonAffineSegments.Add(pSegment);
        }
        else
        {
            pOwningRing->m_affineSegments.Add(pSegment);
        }

        OMTRACE(MTRACE_EVT_CREATESEGMENT, this, NULL, NULL, pSegment);

        return pSegment;
    }

    /// <summary>
    ///     Internal routine which finds an appropriate segment for a task placement.
    /// </summary>
    /// <param name="pSegmentAffinity">
    ///     A segment with this affinity will be located.
    /// </param>
    /// <param name="pRing">
    ///     A segment with pSegmentAffinity within this ring will be found.  A given location may be split into multiple segments by node in order
    ///     to keep work local.
    /// </param>
    /// <returns>
    ///     A segment with the specified affinity close to the specified location.
    /// </returns>
    ScheduleGroupSegmentBase *ScheduleGroupBase::FindSegment(location* pSegmentAffinity, SchedulingRing *pRing)
    {
        ScheduleGroupSegmentBase **pSegmentList = pSegmentAffinity->_Is_system() ? &m_pNonAffineSegments : &m_pAffineSegments;
        ScheduleGroupSegmentBase *pSegment = *pSegmentList;

        location origin = pRing->GetOwningNode()->GetLocation();

        //
        // @TODO:
        //
        // At some point, it might be beneficial to hash segments within the group instead of looking them up.  There will be M * N segments
        // within a group where M is the number of different locations utilized and N is the number of nodes which those locations span.
        //
        while (pSegment != NULL)
        {
            if (pSegment->GetAffinity() == *pSegmentAffinity && pSegment->GetSchedulingRing() == pRing)
            {
                break;
            }

            pSegment = pSegment->m_pNext;
        }

        return pSegment;
    }

    /// <summary>
    ///     Locates a segment that is appropriate for scheduling a task within the schedule group given information about the task's placement
    ///     and the origin of the thread making the call.
    /// </summary>
    /// <param name="pSegmentAffinity">
    ///     A segment with affinity to this particular location will be located.
    /// </paaram>
    /// <param name="pOrigin">
    ///     A location representing the origin of the search.  The scheduler will tend to fork a given pSegmentAffinity into segments by node in order
    ///     to keep locally scheduled work with the same affinity local.
    /// </param>
    /// <param name="fCreateNew">
    ///     An indication as to whether the schedule group can create a new segment if an appropriate segment cannot be found.  If this parameter is
    ///     specified as true, NULL will never be returned from this method; otherwise, it can be if no matching segment can be found.
    /// </param>
    /// <returns>
    ///     A segment appropriate for scheduling work with affinity to pSegmentAffinity from code executing at pOrigin.  Note that NULL may be returned
    ///     if fCreateNew is specified as false and no appropriate segment yet exists for the group.
    /// </returns>
    ScheduleGroupSegmentBase *ScheduleGroupBase::LocateSegment(location* pSegmentAffinity, bool fCreateNew)
    {
        //
        // In general, we wish to find a segment local to our origin (the node where the current context is executing) that has a placement
        // of pSegmentAffinity, or create a new segment should one not yet exist within the group.
        //
        // In practice, a segment will only be created specific to the current ring, if the current ring's affinity is "within" pSegmentAffinity.
        // Otherwise, someone creating a group and saying something like:
        //
        // pGroup->ScheduleTask(..., N0);
        //                     (..., N1);
        //                     (..., N2);
        //                     (..., Nn);
        //
        // might wind up creating n^2 segments within the group for no good reason.
        //
        // This will also allow unbiased work to fork per node and work on local portions.  This is exactly the kind of separation we want.
        //
        // Note that it is possible that we cannot place the current thread (or the scheduler chooses not to) and FindCurrentNode will return NULL.  It's also
        // possible that the pSegmentAffinity to locate does not intersect our origin (even if the scheduler could place the current thread).  In either of these
        // cases, we revert back to previous behavior and round robin an appropriate ring.  The ring's affinity must intersect pSegmentAffinity in some way!

        //
        SchedulingNode * pNode = m_pScheduler->FindCurrentNode();
        SchedulingRing *pRing = (pNode != NULL) ? pNode->GetSchedulingRing() : m_pScheduler->GetNextSchedulingRing();

        //
        // Make sure pRing's affinity intersects pSegmentAffinity (or find a ring which does in round robin order).
        //
        location ringAffinity = pRing->GetOwningNode()->GetLocation();
        location unbiased;
        SchedulingRing * pFirstRing = pRing;

        while (!ringAffinity._FastNodeIntersects(*pSegmentAffinity))
        {
            pRing = m_pScheduler->GetNextSchedulingRing(NULL, pRing);
            ringAffinity = pRing->GetOwningNode()->GetLocation();

            // If we've looped through all the SchedulingRings and haven't found an intersection, back off
            // to an unbiased system-wide location.  This can occur on certain machines that have NUMA
            // nodes with no processors, thus they have valid node locations with no ring created by the
            // scheduler since they can do no work.
            if (pRing == pFirstRing)
            {
                ASSERT(pSegmentAffinity->_GetType() == location::_NumaNode);
                pSegmentAffinity = &unbiased;
            }
        }

        //
        // Do not hold a lock unless we need to create the segment.  This operation should be as inexpensive as possible in the majority case where
        // the appropriate segment already exists.
        //
        ScheduleGroupSegmentBase *pSegment = FindSegment(pSegmentAffinity, pRing);
        if (pSegment == NULL && fCreateNew)
        {
            m_segmentLock._Acquire();
            pSegment = FindSegment(pSegmentAffinity, pRing);
            if (pSegment == NULL)
            {
                pSegment = CreateSegment(pSegmentAffinity, pRing);
            }
            m_segmentLock._Release();
        }

        ASSERT(pSegment->GetSchedulingRing()->IsActive());
        return pSegment;
    }

    /// <summary>
    ///     Removes all schedule group segments from the group.
    /// </summary>
    void ScheduleGroupBase::RemoveSegments()
    {
        ScheduleGroupSegmentBase *pSegment = m_pNonAffineSegments;
        ScheduleGroupSegmentBase *pNext = NULL;

        while(pSegment)
        {
            pNext = pSegment->m_pNext;
            pSegment->Remove();
            pSegment = pNext;
        }

        pSegment = m_pAffineSegments;
        while(pSegment)
        {
            pNext = pSegment->m_pNext;
            pSegment->Remove();
            pSegment = pNext;
        }

        m_pNonAffineSegments = NULL;
        m_pAffineSegments = NULL;
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
    void ScheduleGroupBase::ScheduleTask(_In_ TaskProc proc, void* data)
    {
        ScheduleGroupSegmentBase *pSegment = LocateSegment(&m_groupPlacement, true);
        pSegment->ScheduleTask(proc, data);
    }

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
    void ScheduleGroupBase::ScheduleTask(TaskProc proc, void * data, location& placement)
    {
        ScheduleGroupSegmentBase *pSegment = LocateSegment(&placement, true);
        pSegment->ScheduleTask(proc, data);
    }

    /// <summary>
    ///     Adds runnable context to the schedule group. This is usually a previously blocked context that
    ///     was subsequently unblocked, but it could also be an internal context executing chores on behalf
    ///     of an external context.
    /// </summary>
    void ScheduleGroupSegmentBase::AddRunnableContext(InternalContextBase* pContext, location bias)
    {
        ASSERT(pContext->GetScheduleGroupSegment() == this);
        //
        // If the current context does not belong to this group, the caller is not guaranteed to have a reference to the
        // schedule group. We call CrossGroupRunnable() to make sure that scheduler and schedule group are kept around long
        // enough, that we can attempt to startup the virtual processor without fear of the scheduler being finalized, or the
        // schedule group being destroyed.
        //
        ContextBase* pCurrentContext = SchedulerBase::FastCurrentContext();

#if _UMSTRACE
        OMTRACE(MTRACE_EVT_ADDRUNNABLE, this, pCurrentContext, (!pCurrentContext || pCurrentContext->IsExternal()) ? NULL : static_cast<InternalContextBase *>(pCurrentContext)->DBG_GetCurrentVirtualProcessor(), pContext);
        OMTRACE(MTRACE_EVT_ADDRUNNABLE, this->m_pOwningGroup, pCurrentContext, (!pCurrentContext || pCurrentContext->IsExternal()) ? NULL : static_cast<InternalContextBase *>(pCurrentContext)->DBG_GetCurrentVirtualProcessor(), pContext);
#endif // _UMSTRACE

        if ((pCurrentContext == NULL) || (pCurrentContext->GetScheduleGroupSegment() != this))
        {
            // Set this flag to allow the calling thread to use 'this' safely once the context is pushed onto runnables.
            // Note that this call does not need a fence because it is fenced by push to the runnable contexts collection.
            pContext->CrossGroupRunnable(TRUE);
        }

        //
        // If there is an "inactive pending thread" virtual processor, this runnable should be shoved to it instead of going through the normal
        // wake path.  There is *NO REASON* to require an SFW context to immediately switch to this.
        //
        SchedulerBase *pScheduler = m_pOwningGroup->GetScheduler();
        if (!(pScheduler->HasVirtualProcessorPendingThreadCreate() && pScheduler->PushRunnableToInactive(pContext, bias)))
        {
            // Add it to the actual collection.
            AddToRunnablesCollection(pContext);

            if (!m_affinity._Is_system() && bias == m_affinity)
            {
                NotifyAffinitizedWork();
            }

            if (pScheduler->HasVirtualProcessorAvailable())
            {
                pScheduler->StartupIdleVirtualProcessor(this, bias);
            }
        }

        // Reset the flag, if it was set, since we're done with touching scheduler/context data.
        // This flag is not fenced. This means the reader could end up spinning a little longer until the data is
        // propagated by the cache coherency mechanism.
        pContext->CrossGroupRunnable(FALSE);
        // NOTE: It is not safe to touch 'this' after this point, if this was a cross group runnable.
    }

    /// <summary>
    ///     Steals an unrealized chore from a workqueue in the schedule group.
    /// </summary>
    /// <param name="fForceStealLocalized">
    ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
    ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
    ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
    /// </param>
    _UnrealizedChore* ScheduleGroupSegmentBase::StealUnrealizedChore(bool fForceStealLocalized)
    {
        //
        // When we fail to steal from a work queue that's detached, it's an indication that the work queue
        // is finally empty and can be retired.
        //

        _UnrealizedChore *pChore;

        bool killEmptyQueues = false;
        int maxIndex = m_workQueues.MaxIndex();
        if (maxIndex > 0)
        {
            int skippedCount = 0;
            const int maxSkippedCount = 16;
            int skippedState[maxSkippedCount];
            bool fEntered = false;

            for (int j = 0; j < maxIndex; j++)
            {
                WorkQueue *pQueue = m_workQueues[j];
                if (pQueue != NULL)
                {
                    if ( !pQueue->IsEmpty())
                    {
                        if ((pChore = pQueue->TryToSteal(fForceStealLocalized, fEntered)) != NULL)
                            return pChore;
                        else if ( !fEntered)
                        {
                            if (skippedCount < maxSkippedCount-1)
                            {
                                skippedState[skippedCount++] = j;
                                continue;
                            }
                            else if ((pChore = pQueue->Steal(fForceStealLocalized)) != NULL)
                                return pChore;
                        }

                        killEmptyQueues |= (pQueue->IsDetached() && pQueue->IsEmpty());
                    }
                    else
                        killEmptyQueues |= pQueue->IsDetached();
                }
            }

            if (skippedCount > 0)
            {
                for (int j = 0; j < skippedCount; j++)
                {
                    WorkQueue *pQueue = m_workQueues[skippedState[j]];
                    if (pQueue != NULL)
                    {
                        if ( !pQueue->IsEmpty() && (pChore = pQueue->Steal(fForceStealLocalized)) != NULL)
                            return pChore;
                        else
                            killEmptyQueues |= (pQueue->IsDetached() && pQueue->IsEmpty());
                    }
                }
            }
        }

        if (m_mailedTasks.Dequeue(&pChore))
        {
#if _UMSTRACE && _CHORETRACK
            InternalContextBase *pContext = static_cast<InternalContextBase *>(SchedulerBase::FastCurrentContext());
            OMTRACE(MTRACE_EVT_MAILDEQUEUE, this, pContext, pContext->DBG_GetCurrentVirtualProcessor(), pChore);
            OMTRACE(MTRACE_EVT_MAILDEQUEUE, this->m_pOwningGroup, pContext, pContext->DBG_GetCurrentVirtualProcessor(), pChore);
#endif // _UMSTRACE && _CHORETRACK

            // The chore may not be from a detached workqueue, but since it is dequeued from a mailbox, we set it as detached
            // which will add the stealing context to a list in the task collection instead of the owning contexts stealer collection.
            pChore->_SetDetached(true);
            return pChore;
        }

        int numDetachedArrays = m_detachedWorkQueues.MaxIndex();
        if (numDetachedArrays > 0 && killEmptyQueues)
        {
            for (int i = 0; i < m_workQueues.MaxIndex(); i++)
            {
                WorkQueue *pQueue = m_workQueues[i];
                if (pQueue != NULL)
                {
                    if (pQueue->IsDetached() && pQueue->IsUnstructuredEmpty())
                    {
                        SafelyDeleteDetachedWorkQueue(pQueue);
                    }
                }
            }
        }

        return NULL;
    }

    /// <summary>
    ///     Returns true if the group has any realized chores.
    ///     This is used during scheduler finalization when only one thread is active in the scheduler.
    ///     At any other time, this information is stale since new work could get added to the scheduler.
    /// </summary>
    bool ScheduleGroupSegmentBase::HasRealizedChores() const
    {
        return !m_realizedChores.Empty();
    }

    /// <summary>
    ///     Returns the first work queue in the schedule group that has unrealized chores.
    ///     This is only stable during scheduler finalization when only one thread is active in the scheduler.
    ///     At any other time, this information is stale since new work could get added to the scheduler.
    /// </summary>
    WorkQueue *ScheduleGroupSegmentBase::LocateUnrealizedChores()
    {
        for (int i = 0; i < m_workQueues.MaxIndex(); i++)
        {
            WorkQueue *pQueue = m_workQueues[i];
            if (pQueue != NULL)
            {
                if (!pQueue->IsStructuredEmpty() || !pQueue->IsUnstructuredEmpty())
                {
                    return pQueue;
                }
                else if (pQueue->IsDetached())
                {
                    SafelyDeleteDetachedWorkQueue(pQueue);
                }
            }
        }

        if (!m_mailedTasks.IsEmpty())
            return MAILBOX_LOCATION;

        return NULL;
    }

    /// <summary>
    ///     Returns true if any of the workqueues in the schedule group has unrealized chores.
    ///     This is only stable during scheduler finalization when only one thread is active in the scheduler.
    ///     At any other time, this information is stale since new work could get added to the scheduler.
    /// </summary>
    bool ScheduleGroupSegmentBase::HasUnrealizedChores()
    {
        return LocateUnrealizedChores() != NULL;
    }

    /// <summary>
    ///     Called to safely delete a detached work queue -- this is lock free and utilizes safe points to perform
    ///     the deletion and dereference.  It can be called during the normal SFW or during the finalization sweep
    ///     safely.
    /// </summary>
    bool ScheduleGroupSegmentBase::SafelyDeleteDetachedWorkQueue(WorkQueue *pQueue)
    {
        //
        // The way in which we resolve race conditions between this and queue reattachment is by who is able to remove the
        // element from the detached list array.  We cannot kill the work queue until it's gone out of that list array.
        //
        if (m_detachedWorkQueues.Remove(&pQueue->m_detachment, false))
        {
            //
            // There's always the possibility of a very subtle race where we check IsDetached and IsUnstructuredEmpty and then
            // are preempted, the queue is reattached, work is added, and it's detached again in the same spot with work.  We
            // cannot free the queue in such circumstance.  Only if it is empty AFTER removal from m_detachedWorkQueues are
            // we safe.
            //
            if (pQueue->IsUnstructuredEmpty())
            {
                //
                // Each detached work queue holds a reference on the group.  It is referenced
                // in ScheduleGroupBase::DetachActiveWorkQueue().  Since we are removing this
                // empty work queue, we need to release the reference.
                //
                // There's an unfortunate reality here -- this work queue might be the LAST thing holding reference onto
                // the schedule group.  It's entirely possible that someone just stole and hasn't yet gotten to the point
                // where a reference is added to the schedule group.  If we arbitrarily release this reference, we might delete
                // (or reuse) an active schedule group.  This could cause all sorts of problems.
                //
                // Instead of trying to release that reference here, we will wait until the next safe point to do so.  We
                // are guaranteed no one is in the middle of stealing from this schedule group at that time.
                //
                // Note that this means that the stealer **MUST** stay within a critical region until after the WorkItem::TransferReferences
                // call.
                //
                pQueue->RetireAtSafePoint(this);
                return true;
            }
            else
            {
                CONCRT_COREASSERT(!m_pOwningGroup->GetScheduler()->InFinalizationSweep());

                //
                // The queue is not empty and we need to roll back.  Since we never removed the queue from m_workQueues, the work will
                // still be found by the scheduler without undue futzing around sleep states.  The queue must, however, be placed
                // back in m_detachedWorkQueues in a detached state.
                //
                // There's an unfortunate reality here too -- the slot used for the queue within the detached queues list might already
                // be gone.  Adding back to the detached queues might trigger a heap allocation.  Given that this might be in SFW, a heap allocation
                // triggering UMS would be bad.  Hence -- if we need to roll back (unlikely), we must do this at a safe point.
                //
                pQueue->RedetachFromScheduleGroupAtSafePoint(this);
            }
        }

        return false;
    }

    /// <summary>
    ///     Creates a realized (non workstealing) chore in the schedule group. Used to schedule light-weight
    ///     tasks and agents.
    /// </summary>
    void ScheduleGroupSegmentBase::ScheduleTask(_In_ TaskProc proc, void* data)
    {
        if (proc == NULL)
        {
            throw std::invalid_argument("proc");
        }

        SchedulerBase *pScheduler = m_pOwningGroup->GetScheduler();
        RealizedChore *pChore = pScheduler->GetRealizedChore(proc, data);
        TRACE(TRACE_SCHEDULER, L"ScheduleGroupBase::ScheduleTask(sgroup=%d,ring=0x%p,chore=0x%p)\n", Id(), m_pRing, pChore);

        // Every task takes a reference on its schedule group. This is to ensure a schedule group has a ref count > 0 if
        // no contexts are working on it, but queued tasks are present. The reference count is transferred to the context
        // that eventually executes the task.
        m_pOwningGroup->InternalReference();

        m_realizedChores.Enqueue(pChore);

        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();

#if _UMSTRACE && _CHORETRACK
        OMTRACE(MTRACE_EVT_ADDLWT, this, pCurrentContext, (!pCurrentContext || pCurrentContext->IsExternal()) ? NULL : static_cast<InternalContextBase *>(pCurrentContext)->DBG_GetCurrentVirtualProcessor(), pChore);
        OMTRACE(MTRACE_EVT_ADDLWT, this->m_pOwningGroup, pCurrentContext, (!pCurrentContext || pCurrentContext->IsExternal()) ? NULL : static_cast<InternalContextBase *>(pCurrentContext)->DBG_GetCurrentVirtualProcessor(), pChore);
#endif // _UMSTRACE && CHORETRACK

        if (pCurrentContext == NULL || pCurrentContext->GetScheduler() != pScheduler)
        {
            //
            // This is a thread that is in no way tracked in ConcRT (no context assigned to it) or it is a context foreign to
            // this scheduler, so we cannot have statistics directly associated with its context. Instead, there is an entry in
            // the TLS section PER scheduler that points to the external statistics mapping. From that information, we can know
            // whether we have seen this thread before and whether it was ever scheduling tasks on the current scheduler.
            //
            ExternalStatistics * externalStatistics = (ExternalStatistics *) platform::__TlsGetValue(pScheduler->m_dwExternalStatisticsIndex);

            if (externalStatistics == NULL)
            {
                //
                // This is the first piece of statistical data for this thread on this scheduler, so
                // create a statistics class, add it to the list array of statistics on this scheduler and
                // save it in the TLS slot reserved for statistics on this scheduler.
                //
                externalStatistics = _concrt_new ExternalStatistics();
                pScheduler->AddExternalStatistics(externalStatistics);
                platform::__TlsSetValue(pScheduler->m_dwExternalStatisticsIndex, externalStatistics);
            }
            else
            {
                //
                // We already have some statistical data for this thread on this scheduler.
                //
                ASSERT(pScheduler->m_externalThreadStatistics.MaxIndex() > 0);
            }

            ASSERT(externalStatistics != NULL);
            externalStatistics->IncrementEnqueuedTaskCounter();
        }
        else if (pCurrentContext->IsExternal())
        {
            static_cast<ExternalContextBase *>(pCurrentContext)->IncrementEnqueuedTaskCounter();
        }
        else
        {
            static_cast<InternalContextBase *>(pCurrentContext)->IncrementEnqueuedTaskCounter();
        }

        //
        // If there is explicit affinity placed on this new task, make sure to tell the scheduler so that it can send messages to any virtual
        // processors as necessary to snap them back to affine work.
        //
        if (!m_affinity._Is_system())
        {
            NotifyAffinitizedWork();
        }

        // In most cases this if check will fail. To avoid the function call overhead in the common case, we check
        // for virtual processors beforehand.
        if (pScheduler->HasVirtualProcessorAvailableForNewWork())
        {
            pScheduler->StartupNewVirtualProcessor(this, m_affinity);
        }

    }


    /// <summary>
    ///     Places a work queue in the detached queue.  This will cause the work queue to remain eligible for stealing
    ///     while the queue can be detached from a context.  The work queue will be recycled and handed back to a
    ///     context executing within the schedule group that needs a queue.  If the queue is not recycled, it will be
    ///     abandoned and freed when it becomes empty (a steal on it while in detached mode fails).
    /// </summary>
    void ScheduleGroupSegmentBase::DetachActiveWorkQueue(WorkQueue *pWorkQueue)
    {
        m_pOwningGroup->InternalReference();

        //
        // Note: there is a distinct lack of relative atomicity between the flag set and the queue add.  The worst thing that
        //       happens here is that we ask the list array to remove an element at an invalid index.  It is prepared to handle
        //       that anyway.
        //
        pWorkQueue->SetDetached(true);
        m_detachedWorkQueues.Add(&pWorkQueue->m_detachment);
    }

    /// <summary>
    ///     Called by a work queue in order to roll back an attempted kill that could not be committed due to reuse.
    /// </summary>
    void ScheduleGroupSegmentBase::RedetachQueue(WorkQueue *pWorkQueue)
    {
        //
        // Roll back by reinserting into m_detachedWorkQueues.  We detect the error before setting detached state to false or releasing
        // reference, so this is the only operation which needs to happen.  It just cannot happen during the steal due to the fact that
        // there is a **SLIGHT** chance that the call will perform a heap allocation.
        //
        m_detachedWorkQueues.Add(&pWorkQueue->m_detachment);
    }

    /// <summary>
    ///     Attempts to acquire a detached work queue from the schedule group.  If such a work queue is found, it
    ///     is removed from detached queue and returned.  This allows recycling of work queues that are detached
    ///     yet still have unstructured work.
    ///</summary>
    WorkQueue *ScheduleGroupSegmentBase::GetDetachedWorkQueue()
    {
        int maxIdx = m_detachedWorkQueues.MaxIndex();
        for (int i = 0; i < maxIdx; i++)
        {
            ListArrayInlineLink<WorkQueue> *pLink = m_detachedWorkQueues[i];

            //
            // No code below this may dereference pLink unless it is removed from the list array.  There is no guarantee
            // of safety as this can be called from an external context or multiple internal contexts.
            //
            if (pLink != NULL && m_detachedWorkQueues.Remove(pLink, i, false))
            {
                WorkQueue *pWorkQueue = pLink->m_pObject;

                pWorkQueue->SetDetached(false);

                //
                // This removed detached work queue incremented the reference count
                // in ScheduleGroupBase::DetachActiveWorkQueue().  Release it now.
                //
                // This is safe because we are inside the schedule group getting a work queue.  This means that there is already
                // some context with a reference on the schedule group and it won't disappear out from underneath us by removing
                // the detach reference.
                //
                m_pOwningGroup->InternalRelease();

                return pWorkQueue;
            }
        }

        return NULL;
    }

    /// <summary>
    ///     Called by a work queue in order to retire itself at a safe point.
    /// </summary>
    void ScheduleGroupSegmentBase::RetireDetachedQueue(WorkQueue *pWorkQueue)
    {
        CONCRT_VERIFY(m_workQueues.Remove(pWorkQueue));

        //
        // This removed detached work queue incremented the reference count
        // in ScheduleGroupBase::DetachActiveWorkQueue().  Release it now.
        //
        m_pOwningGroup->InternalRelease();
    }

    RealizedChore * ScheduleGroupSegmentBase::GetRealizedChore()
    {
        if (m_realizedChores.Empty())
            return NULL;

        RealizedChore *pChore = m_realizedChores.Dequeue();
        TRACE(TRACE_SCHEDULER, L"ScheduleGroup::GetRealizedChore(sgroup=%d,ring=0x%p,chore=0x%p)\n", Id(), m_pRing, pChore);
        return pChore;
    }

    /// <summary>
    ///     Gets an internal context from either the idle pool or a newly allocated one and prepares it for
    ///     exection. A NULL return value from the routine is considered fatal (out of memory). This is the
    ///     API that should be used to obtain an internal context for exection. The context is associated
    //      with this schedule group.
    /// </summary>
    InternalContextBase * ScheduleGroupSegmentBase::GetInternalContext(_Chore *pChore, bool choreStolen)
    {
        // Get an internal context from the idle pool
        InternalContextBase* pContext = m_pOwningGroup->GetScheduler()->GetInternalContext();

        if (pContext != NULL)
        {
            // Associate it with this schedule group
            pContext->PrepareForUse(this, pChore, choreStolen);
        }

        return pContext;
    }

    /// <summary>
    ///     Releases an internal context after execution into the idle pool. If the idle pool
    ///     is full, it could be freed.
    /// </summary>
    void ScheduleGroupSegmentBase::ReleaseInternalContext(InternalContextBase *pContext)
    {
        pContext->RemoveFromUse();
        m_pOwningGroup->GetScheduler()->ReleaseInternalContext(pContext);
    }

    /// <summary>
    ///     Destroys a schedule group segment.
    /// </summary>
    ScheduleGroupSegmentBase::~ScheduleGroupSegmentBase()
    {
        //
        // Make CERTAIN that the quick cache is cleared if this segment is contained within it.
        //
        if (m_affinity._GetType() == location::_ExecutionResource)
        {
            m_pOwningGroup->GetScheduler()->ClearQuickCacheSlotIf(m_maskIdIf, this);
        }

        // There shall be no work queues  (detached or otherwise) when a schedule group segment
        // is deleted. This assumption is made in our safe point mechanism. If one
        // of the workqueues in a schedule group segment requests a safe point invocation after
        // the one for schedule group deletion, the workqueues would be deleted before
        // its callback is invoked.
        ASSERT(m_workQueues.IsEmptyAtSafePoint());
        ASSERT(m_detachedWorkQueues.IsEmptyAtSafePoint());
    }

    /// <summary>
    ///     Removes the segment.
    /// </summary>
    void ScheduleGroupSegmentBase::Remove()
    {
        OMTRACE(MTRACE_EVT_DESTROYSEGMENT, m_pOwningGroup, NULL, NULL, this);
        // The order of operations here is important. Removing from the list array should be the last operation we perform on
        // the segment.
        m_pOwningGroup->m_pScheduler->RemovePrioritizedObject(&m_priorityServiceLink);
        m_pRing->RemoveScheduleGroupSegment(this);
    }

} // namespace details
} // namespace Concurrency
