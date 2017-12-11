// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessor.cpp
//
// Source file containing the VirtualProcessor implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a virtual processor.
    /// </summary>
    VirtualProcessor::VirtualProcessor()
        : m_localRunnableContexts(&m_lock)
    {
        // Derived classes should use Initialize(...) to init the virtual processor
    }

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
    void VirtualProcessor::Initialize(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot)
    {
        OMTRACE(MTRACE_EVT_INITIALIZED, this, NULL, NULL, NULL);

        m_lastServiceTime = 0;
        m_priorityServiceLink.m_boostState = BoostedObject::BoostStateUnboosted;
        m_priorityServiceLink.m_type = BoostedObject::BoostTypeVirtualProcessor;
        m_pPushContext = NULL;
        m_pOwningNode = pOwningNode;
        m_pOwningRing = pOwningNode->GetSchedulingRing();
        m_pOwningRoot = pOwningRoot;
        m_fMarkedForRetirement = false;
        m_fOversubscribed = false;
        m_availabilityType = AvailabilityClaimed;
        m_enqueuedTaskCounter = 0;
        m_dequeuedTaskCounter = 0;
        m_enqueuedTaskCheckpoint = 0;
        m_dequeuedTaskCheckpoint = 0;
        m_pExecutingContext = NULL;
        m_pOversubscribingContext = NULL;
        m_safePointMarker.Reset();
        m_pSubAllocator = NULL;
        m_fLocal = false;
        m_fAffine = false;

        //
        // When a VPROC first comes online, it **MUST** do one affine SFW before moving on with life.  This avoids a wake/arrive race with affine
        // work.
        //
        m_fShortAffine = true;

        SchedulerBase *pScheduler = m_pOwningNode->GetScheduler();

        // A virtual procesor has the same id as its associated virtual processor root. The roots have process unique ids.
        m_id = pOwningRoot->GetId();

        // Keep track of the abstract identifier for the underlying execution resource.
        m_resourceId = pOwningRoot->GetExecutionResourceId();

        // Determine our hash id and create our map.
        m_maskId = pScheduler->GetResourceMaskId(m_resourceId);
        m_resourceMask.Grow(pScheduler->GetMaskIdCount());
        m_resourceMask.Wipe();
        m_resourceMask.Set(m_maskId);

        if (pScheduler->GetSchedulingProtocol() == ::Concurrency::EnhanceScheduleGroupLocality)
            m_searchCtx.Reset(this, WorkSearchContext::AlgorithmCacheLocal);
        else
            m_searchCtx.Reset(this, WorkSearchContext::AlgorithmFair);

        m_location = location(location::_ExecutionResource, m_resourceId, m_pOwningNode->m_pScheduler->Id(), this);

        // Notify the scheduler that we are listening for events pertaining to affinity and locality.
        pScheduler->ListenAffinity(m_maskId);

        TraceVirtualProcessorEvent(CONCRT_EVENT_START, TRACE_LEVEL_INFORMATION, m_pOwningNode->m_pScheduler->Id(), m_id);
    }

    /// <summary>
    ///     Destroys a virtual processor
    /// </summary>
    VirtualProcessor::~VirtualProcessor()
    {
        ASSERT(m_localRunnableContexts.Count() == 0);

        if (m_pSubAllocator != NULL)
        {
            SchedulerBase::ReturnSubAllocator(m_pSubAllocator);
            m_pSubAllocator = NULL;
        }
    }

    /// <summary>
    ///     Activates a virtual processor with the context provided.
    /// </summary>
    void VirtualProcessor::Activate(IExecutionContext * pContext)
    {
        VMTRACE(MTRACE_EVT_ACTIVATE, ToInternalContext(pContext), this, SchedulerBase::FastCurrentContext());
#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_ACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, pContext);
        CMTRACE(MTRACE_EVT_ACTIVATED, ToInternalContext(pContext), this, pCurrentContext);
#endif // _UMSTRACE
        m_pOwningRoot->Activate(pContext);
    }

    /// <summary>
    ///     Temporarily deactivates a virtual processor.
    /// <summary>
    /// <returns>
    ///     An indication of which side the awakening occured from (true -- we activated it, false -- the RM awoke it).
    /// </returns>
    bool VirtualProcessor::Deactivate(IExecutionContext * pContext)
    {
        VMTRACE(MTRACE_EVT_DEACTIVATE, ToInternalContext(pContext), this, false);
#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_DEACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, pContext);
#endif // _UMSTRACE

        return m_pOwningRoot->Deactivate(pContext);
    }

    /// <summary>
    ///     Invokes the underlying virtual processor root to ensure all tasks are visible
    /// </summary>
    void VirtualProcessor::EnsureAllTasksVisible(IExecutionContext * pContext)
    {
        VMTRACE(MTRACE_EVT_DEACTIVATE, ToInternalContext(pContext), this, true);
#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_DEACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, pContext);
#endif // _UMSTRACE

        m_pOwningRoot->EnsureAllTasksVisible(pContext);
    }

    /// <summary>
    ///     Exercises a claim of ownership over a virtual processor and starts it up.
    /// </summary>
    bool VirtualProcessor::ExerciseClaim(VirtualProcessor::AvailabilityType type, ScheduleGroupSegmentBase *pSegment, InternalContextBase *pContext)
    {
        VMTRACE(MTRACE_EVT_TICKETEXERCISE, SchedulerBase::FastCurrentContext(), this, type);

        SchedulerBase *pScheduler = GetOwningNode()->GetScheduler();

        CONCRT_COREASSERT(type != AvailabilityClaimed);

        //
        // @TODO: Should we allow a generic exercise with AvailabilityInactivePendingThread or should this require an explicit exercise.
        //
        switch(type)
        {
            case AvailabilityInactive:
            case AvailabilityInactivePendingThread:
                if (!pScheduler->VirtualProcessorActive(true))
                {
                    //
                    // This happened during a shutdown/DRM race.  We do not need to notify the background thread of anything.  It will all work out
                    // as finalization proceeds.
                    //
                    if (pContext != NULL)
                    {
                        CONCRT_COREASSERT(!pContext->IsPrepared());

                        //
                        // Only a pre-bound context is passed into here.  If we cannot start up the vproc right now, the context needs
                        // to get retired.
                        //
                        pScheduler->ReleaseInternalContext(pContext, true);
                    }

                    MakeAvailable(type, false);
                    return false;
                }

                if (pSegment == NULL)
                    pSegment = pScheduler->GetAnonymousScheduleGroupSegment();

                return StartupWorkerContext(pSegment, pContext);

            case AvailabilityIdlePendingThread:
                //
                // This path is not generalized and is specific to UMS.
                //
                // Consideration might be given to allowing push to this virtual processor we just decided to wake up instead of allowing the
                // SFW.  The problem here is that the context might be doing its own SFW and the two contexts might collide.  If they do, there's
                // a question about what's reasonable to do.  For now, this is unsupported -- we only push to inactive vprocs.
                //
                CONCRT_COREASSERT(pContext == NULL);

                break;

            default:
                //
                // See above comments under IdlePendingThread.
                //
                CONCRT_COREASSERT(pContext == NULL);

                CONCRT_COREASSERT(m_pAvailableContext != NULL);

                //
                // Note: We cannot validate that m_pExecutingContext is running atop this.  On UMS, it's legal (although extremely discouraged) to UMS
                // block between MakeAvailable and Deactivate in the SFW path.  In that case, we pick up other runnables.  The context which is going
                // to be activated and subsequently swallow the activate with a corresponding deactivate is m_pAvailableContext.  m_pExecutingContext
                // may be the random thing we switched to.  If this happens, m_pAvailableContext may also have a NULL vproc.
                //
                // Note that this typically occasionally happens due to the FlushProcessWriteBuffers call before final deactivation.
                //
#if defined(_DEBUG)
                {
                    VirtualProcessor *pVProc = ToInternalContext(m_pAvailableContext) ? ToInternalContext(m_pAvailableContext)->m_pVirtualProcessor : NULL;
                    CONCRT_COREASSERT(pVProc == this || pVProc == NULL);
                }
#endif // defined(_DEBUG)
        }

        m_pOwningRoot->Activate(m_pAvailableContext);
        return true;
    }

    /// <summary>
    ///     Start a worker context executing on this.virtual processor.
    /// </summary>
    bool VirtualProcessor::StartupWorkerContext(ScheduleGroupSegmentBase *pSegment, InternalContextBase *pContext)
    {
        //
        // We need to spin until m_pExecutingContext is NULL so we can appropriately affinitize a new thread and not conflict.
        //
        if (m_pExecutingContext != NULL)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);
            while(m_pExecutingContext != NULL)
                spinWait._SpinOnce();
        }

        if (pContext != NULL)
        {
            if (!pContext->IsPrepared())
                pContext->PrepareForUse(pSegment, NULL, false);
        }
        else
        {
            pContext = pSegment->GetInternalContext();
        }

        //
        // It's entirely possible we could *NOT* get a thread to wake up this virtual processor.  In this case, we need to make it idle
        // again and tell the background thread to wake us up when there is a thread available.
        //
        if (pContext == NULL)
        {
            MakeAvailable(AvailabilityInactivePendingThread);
            GetOwningNode()->GetScheduler()->DeferredGetInternalContext();
            return false;
        }

        Affinitize(pContext);
        m_pOwningRoot->Activate(m_pExecutingContext);
        return true;
    }

    /// <summary>
    ///     Affinitizes an internal context to the virtual processor.
    /// </summary>
    /// <param name="pContext">
    ///     The internal context to affinitize.
    /// </param>
    void VirtualProcessor::Affinitize(InternalContextBase *pContext)
    {
        //
        // Wait until the context is firmly blocked, if it has started. This is essential to prevent vproc orphanage
        // if the context we're switching to is IN THE PROCESS of switching out to a different one. An example of how this
        // could happen:
        //
        // 1] ctxA is running on vp1. It is in the process of blocking, and wants to switch to ctxB. This means ctxA needs to
        // affintize ctxB to its own vproc, vp1.
        //
        // 2] At the exact same time, ctxA is unblocked by ctxY and put onto a runnables collection in its scheduler. Meanwhile, ctxZ
        // executing on vp2, has also decided to block. It picks ctxA off the runnables collection, and proceeds to switch to it.
        // This means that ctxZ needs to affinitize ctxA to ITS vproc vp2.
        //
        // 3] Now, if ctxZ affintizes ctxA to vp2 BEFORE ctxA has had a chance to affintize ctxB to vp1, ctxB gets mistakenly
        // affintized to vp2, and vp1 is orphaned.
        //
        // In order to prevent this, ctxZ MUST wait until AFTER ctxA has finished its affinitization. This is indicated via the
        // blocked flag. ctxA will set its blocked flag to 1, after it has finished affintizing ctxB to vp1, at which point it is
        // safe for ctxZ to modify ctxA's vproc and change it from vp1 to vp2.
        //
        // Note that it is possible that pContext is NULL in the case where we are going to SwitchOut a virtual processor due to a lack of
        // available threads.
        //
        if (pContext != NULL)
        {
            pContext->SpinUntilBlocked();
            pContext->PrepareToRun(this);

            VCMTRACE(MTRACE_EVT_AFFINITIZED, pContext, this, NULL);

#if defined(_DEBUG)
            pContext->ClearDebugBits();
            pContext->SetDebugBits(CTX_DEBUGBIT_AFFINITIZED);
#endif // _DEBUG
        }

        // Make sure there is a two-way mapping between a virual processor and the affinitized context attached to it.
        // The pContext-> side of this mapping was established in PrepareToRun.
        m_pExecutingContext = pContext;

        //
        // If we were unable to update the statistical information because internal context was not
        // affinitized to a virtual processor, then do it now when the affinitization is done.
        //
        if (pContext != NULL && pContext->m_fHasDequeuedTask)
        {
            m_dequeuedTaskCounter++;
            pContext->m_fHasDequeuedTask = false;
        }
    }

    /// <summary>
    ///     Marks the virtual processor such that it removes itself from the scheduler once the context it is executing
    ///     reaches a safe yield point. Alternatively, if the context has not started executing yet, it can be retired immediately.
    /// </summary>
    void VirtualProcessor::MarkForRetirement()
    {
        ClaimTicket ticket;
        if (ClaimExclusiveOwnership(ticket))
        {
            //
            // If there is a context attached to this virtual processor but we were able to claim it for
            // retirement then we have to unblock this context and send it for retirement. Otherwise, if
            // there was no context attached we can simply retire the virtual processor.
            //
            if (ticket.ExerciseWakesExisting())
            {
                m_fMarkedForRetirement = true;
                ticket.Exercise();
            }
            else
            {
                Retire();
            }
        }
        else
        {
            //
            // Instruct the virtual processor to exit at a yield point - when the context it is executing enters the scheduler
            // from user code.
            //
            m_fMarkedForRetirement = true;
        }
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
    bool VirtualProcessor::ClaimExclusiveOwnership(VirtualProcessor::ClaimTicket &ticket, ULONG type, bool updateCounters)
    {
        CONCRT_COREASSERT(type != AvailabilityClaimed);

        AvailabilityType curType = m_availabilityType;
        if ((curType & type) != 0)
        {
            AvailabilityType prevType = AvailabilityType();
            bool fClaimed = false;

            if (type == AvailabilityAny)
            {
                prevType = (AvailabilityType)(InterlockedExchange(reinterpret_cast<volatile long *>(&m_availabilityType), AvailabilityClaimed));
                fClaimed = (prevType != AvailabilityClaimed);
            }
            else
            {
                for(;;)
                {
                    if ((curType & type) == 0)
                        break;

                    prevType = (AvailabilityType)(InterlockedCompareExchange(reinterpret_cast<volatile long *>(&m_availabilityType), AvailabilityClaimed, curType));
                    if (prevType == curType)
                    {
                        fClaimed = true;
                        break;
                    }

                    curType = prevType;
                }
            }

            if (fClaimed)
            {
                CONCRT_COREASSERT(m_availabilityType == AvailabilityClaimed);

#if _UMSTRACE
                ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
                VCMTRACE(MTRACE_EVT_CLAIMEDOWNERSHIP, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, SchedulerBase::FastCurrentContext());
#endif // _UMSTRACE

                if (updateCounters)
                {
                    InterlockedDecrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorAvailableCount);
                    InterlockedDecrement(&m_pOwningNode->m_virtualProcessorAvailableCount);

                    //
                    // Keep track of the number of virtual processors pending thread creation.
                    //
                    if (prevType == AvailabilityInactivePendingThread || prevType == AvailabilityIdlePendingThread)
                    {
                        InterlockedDecrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorsPendingThreadCreate);
                        InterlockedDecrement(&m_pOwningNode->m_virtualProcessorsPendingThreadCreate);
                    }

                    OMTRACE(MTRACE_EVT_CLAIMEDOWNERSHIP, m_pOwningNode->m_pScheduler, pCurrentContext, this, pCurrentContext);
                }

                OMTRACE(MTRACE_EVT_AVAILABLEVPROCS, m_pOwningNode->m_pScheduler, pCurrentContext, this, m_pOwningNode->m_pScheduler->m_virtualProcessorAvailableCount);

                ticket.InitializeTicket(prevType, this);
                m_claimantType = prevType;

                return true;
            }
        }
        return false;
    }

    /// <summary>
    ///     Makes a virtual processor available for scheduling work.
    /// </summary>
    /// <param name="fCanChangeActiveState">
    ///     Indicates whether the routine should update active state for the vproc based upon type or not.
    /// </param>
    void VirtualProcessor::MakeAvailable(AvailabilityType type, bool fCanChangeActiveState)
    {
        ASSERT(m_availabilityType == AvailabilityClaimed);

        //
        // Keep track of the context which made the virtual processor available.  It will be the one that deactivates if there's a corresponding activate
        // from outside.  This is a spec requirement.  On UMS, we can temporarily execute another context if the scheduler blocks with a Win32 primitive
        // between the MakeAvailable and the Deactivate call.  This should be avoided if at all possible -- it is a performance hit!
        //
        m_pAvailableContext = m_pExecutingContext;

        if (fCanChangeActiveState && (type == AvailabilityInactive || type == AvailabilityInactivePendingThread))
            GetOwningNode()->GetScheduler()->VirtualProcessorActive(false);

#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        VCMTRACE(MTRACE_EVT_MADEAVAILABLE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, NULL);
#endif // _UMSTRACE

        InterlockedIncrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorAvailableCount);
        InterlockedIncrement(&m_pOwningNode->m_virtualProcessorAvailableCount);

        //
        // Keep track of the number of virtual processors pending thread creation (if any are).
        //
        if (type == AvailabilityInactivePendingThread || type == AvailabilityIdlePendingThread)
        {
            InterlockedIncrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorsPendingThreadCreate);
            InterlockedIncrement(&m_pOwningNode->m_virtualProcessorsPendingThreadCreate);
        }

        OMTRACE(MTRACE_EVT_MADEAVAILABLE, m_pOwningNode->m_pScheduler, pCurrentContext, this, NULL);
        OMTRACE(MTRACE_EVT_AVAILABLEVPROCS, m_pOwningNode->m_pScheduler, pCurrentContext, this, m_pOwningNode->m_pScheduler->m_virtualProcessorAvailableCount);
        InterlockedExchange(reinterpret_cast<volatile long *>(&m_availabilityType), type);
    }

    /// <summary>
    ///     Oversubscribes the virtual processor by creating a new virtual processor root affinitized to the same
    ///     execution resource as that of the current root
    /// </summary>
    /// <returns>
    ///     A virtual processor that oversubscribes this one.
    /// </returns>
    VirtualProcessor * VirtualProcessor::Oversubscribe()
    {
        IVirtualProcessorRoot *pOversubscriberRoot = GetOwningNode()->GetScheduler()->GetSchedulerProxy()->CreateOversubscriber(m_pOwningRoot);
        ASSERT(pOversubscriberRoot != NULL);

        return m_pOwningNode->AddVirtualProcessor(pOversubscriberRoot, true);
    }

    /// <summary>
    ///     Causes the virtual processor to remove itself from the scheduler. This is used either when oversubscription
    ///     ends or when the resource manager asks the vproc to retire.
    /// </summary>
    void VirtualProcessor::Retire()
    {
        VMTRACE(MTRACE_EVT_RETIRE, SchedulerBase::FastCurrentContext(), this, m_availabilityType);

        m_pOwningNode->m_pScheduler->RemovePrioritizedObject(&m_priorityServiceLink);

        // Virtual processor available counts are already decremented by this point. We need to decrement the total counts
        // on both the node and the scheduler. Oversubscribed vprocs do not contribute to the total vproc count on the scheduler.
        m_pOwningNode->m_pScheduler->DecrementActiveResourcesByMask(m_maskId);
        InterlockedDecrement(&m_pOwningNode->m_virtualProcessorCount);
        if (!m_fOversubscribed)
        {
            InterlockedDecrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorCount);
        }

        // Since virtual processor is going away we'd like to preserve its counts
        m_pOwningNode->GetScheduler()->SaveRetiredVirtualProcessorStatistics(this);

        // Make sure we're no longer flagged as listening for affinity messages.
        if (!m_fAffine)
            m_pOwningNode->GetScheduler()->IgnoreAffinity(m_maskId);

        // If this is a virtual processor currently associated with an executing context, it's important to assert there that
        // the scheduler is not shutting down. We want to make sure that all virtual processor root removals (for executing virtual
        // processors) occur before the scheduler shuts down. This will ensure that all IVirtualProcessorRoot::Remove calls
        // that can originate from a scheduler's internal contexts instance are received the RM before the ISchedulerProxy::Shutdown call,
        // which asks the RM to release all resources and destroy the remaining virtual processor roots allocated to the scheduler.
        // RM should not receive Remove calls for roots that are already destroyed.
        ASSERT(ClaimantWasInactive() || ToInternalContext(m_pExecutingContext) == SchedulerBase::FastCurrentContext());
        ASSERT(ClaimantWasInactive() || (!m_pOwningNode->GetScheduler()->InFinalizationSweep() && !m_pOwningNode->GetScheduler()->HasCompletedShutdown()));

        m_pExecutingContext = NULL;

        // Check if there are contexts in the Local Runnables Collection and put them into the collection of runnables in their
        // respective schedule groups.
        InternalContextBase *pContext = GetLocalRunnableContext();
        while (pContext != NULL)
        {
            ScheduleGroupSegmentBase *pSegment = pContext->GetScheduleGroupSegment();
            pSegment->AddRunnableContext(pContext, pSegment->GetAffinity());
            pContext = GetLocalRunnableContext();
        }

        // Send an IScheduler pointer into to Remove. Scheduler does derive from IScheduler, and therefore we need
        // an extra level of indirection.
        m_pOwningRoot->Remove(m_pOwningNode->GetScheduler()->GetIScheduler());
        m_pOwningRoot = NULL;

        TraceVirtualProcessorEvent(CONCRT_EVENT_END, TRACE_LEVEL_INFORMATION, m_pOwningNode->m_pScheduler->Id(), m_id);

        if (m_pSubAllocator != NULL)
        {
            SchedulerBase::ReturnSubAllocator(m_pSubAllocator);
            m_pSubAllocator = NULL;
        }

        // Removing this VirtualProcessor from the ListArray will move it to a pool for reuse
        // This must be done at the end of this function, otherwise, this virtual processor itself could be
        // pulled out of the list array for reuse and stomped over before retirement is complete.
        m_pOwningNode->m_virtualProcessors.Remove(this);
        // *DO NOT* touch 'this' after removing it from the list array.
    }

    /// <summary>
    ///     Returns a pointer to the suballocator for the virtual processor.
    /// </summary>
    SubAllocator * VirtualProcessor::GetCurrentSubAllocator()
    {
        if (m_pSubAllocator == NULL)
        {
            m_pSubAllocator = SchedulerBase::GetSubAllocator(false);
        }
        return m_pSubAllocator;
    }

    /// <summary>
    ///     Updates tracking on the virtual processor whether it is executing affine work and/or local work.
    /// </summary>
    /// <param name="fAffine">
    ///     An indication of whether the virtual processor is executing work which has affinity to it or not.
    /// </param>
    /// <param name="fLocal">
    ///     An indication of whether the virtual processor is executing work which is local to it or not.
    /// </param>
    void VirtualProcessor::UpdateWorkState(bool fAffine, bool fLocal)
    {
        SchedulerBase *pScheduler = m_pOwningNode->GetScheduler();

        //
        // Notify the scheduler if we need to listen for affinity events or not.
        //
        if (m_fAffine)
        {
            if (!fAffine)
            {
                VCMTRACE(MTRACE_EVT_CHANGEAFFINITYSTATE, m_pExecutingContext, this, 0);

                //
                // See CheckAffinityNotification comments.  We need to avoid a listen/arrive race to prevent ourselves getting stuck on non-affine
                // work.
                //
                m_fShortAffine = true;
                pScheduler->ListenAffinity(m_maskId);
            }
        }
        else
        {
            if (fAffine)
            {
                VCMTRACE(MTRACE_EVT_CHANGEAFFINITYSTATE, m_pExecutingContext, this, 1);
                pScheduler->IgnoreAffinity(m_maskId);
            }
        }

        m_fAffine = fAffine;
        m_fLocal = fLocal;
    }

    /// <summary>
    ///     Performs a quick check as to whether work which is affine to the virtual processor has arrived.  This allows short circuiting of
    ///     expensive searches for affine work in cases where a user does not use any location objects.
    /// </summary>
    /// <returns>
    ///     An indication of whether or not work affine to the virtual processor has arrived since UpdateWorkState was called to indicate that
    ///     the virtual processor began working on non-affine work.
    /// </returns>
    bool VirtualProcessor::CheckAffinityNotification()
    {
        //
        // Once we switch to executing non-affine work and flag that we are listening to messages, we will get affinity notifications and do not
        // need to SFW for affine work.  There is an inherent race in this, however.  If we are doing an affine search and have passed segment "S"
        // (affine to us), and a series of work comes into "S", and then we find non-affine work (because we already checked "S"), we will get no
        // message for that work that came into "S".  We must ensure that we do an affine SFW **ONCE** immediately after flagging ourselves
        // as a listener.  m_fShortAffine tracks this requirement.
        //
        if (m_fShortAffine)
        {
            VCMTRACE(MTRACE_EVT_ACKNOWLEDGEAFFINITYMESSAGE, m_pExecutingContext, this, 1);

            m_fShortAffine = false;
            return true;
        }
        else
        {
            bool fAck = GetOwningNode()->GetScheduler()->AcknowledgedAffinityMessage(m_maskId);
#if _UMSTRACE
            if (fAck)
            {
                VCMTRACE(MTRACE_EVT_ACKNOWLEDGEAFFINITYMESSAGE, m_pExecutingContext, this, 0);
            }
#endif // UMSTRACE

            return fAck;
        }
    }

    /// <summary>
    ///     Send a virtual processor ETW event.
    /// </summary>
    void VirtualProcessor::ThrowVirtualProcessorEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD vprocId)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Class.Type = (UCHAR)eventType;
            concrtHeader.header.Class.Level = level;
            concrtHeader.header.Guid = VirtualProcessorEventGuid;

            concrtHeader.SchedulerID = schedulerId;
            concrtHeader.VirtualProcessorID = vprocId;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }

#if _UMSTRACE
    void VirtualProcessor::TraceSearchedLocalRunnables()
    {
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_SEARCHEDLOCALRUNNABLES, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, NULL);
    }
#endif // UMSTRACE

    /// <summary>
    ///     Returns a type-cast of pContext to an InternalContextBase or NULL if it is not.
    /// </summary>
    InternalContextBase *VirtualProcessor::ToInternalContext(IExecutionContext *pContext)
    {
        return static_cast<InternalContextBase *>(pContext);
    }

    /// <summary>
    ///     Called when the context running atop this virtual processor has reached a safe point.
    /// </summary>
    /// <returns>
    ///     An indication of whether the caller should perform a commit.
    /// </returns>
    bool VirtualProcessor::SafePoint()
    {
        return GetOwningNode()->GetScheduler()->MarkSafePoint(&m_safePointMarker);
    }

    /// <summary>
    ///     Exercises the ticket with a particular context.  This is only valid if ExerciseWakesExisting() returns false.  Callers should have
    ///     sought virtual processors with specific claim rights.
    /// </summary>
    bool VirtualProcessor::ClaimTicket::ExerciseWith(InternalContextBase *pContext)
    {
        bool fResult = false;
        if (m_type != AvailabilityClaimed)
        {
            //
            // @TODO: It should be started on a specific group plumbed through from the point at which the vproc couldn't find a thread.
            // Right now, this info is not plumbed through so we pick up the anonymous group.
            //
            fResult = m_pVirtualProcessor->ExerciseClaim(m_type, m_pVirtualProcessor->GetOwningNode()->GetSchedulingRing()->GetAnonymousScheduleGroupSegment(), pContext);
            m_type = AvailabilityClaimed;
        }

        return fResult;
    }

} // namespace details
} // namespace Concurrency
