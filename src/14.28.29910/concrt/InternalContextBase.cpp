// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// InternalContextBase.cpp
//
// Source file containing the implementation for an internal execution context.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    unsigned int GetProcessorMaskId(InternalContextBase* pContext)
    {
        return pContext->m_pVirtualProcessor->GetMaskId();
    }

#if defined(_DEBUG)
    void SetContextDebugBits(InternalContextBase *pContext, DWORD bits)
    {
        if (pContext != NULL)
            pContext->SetDebugBits(bits);
    }
#endif

    /// <summary>
    ///     Constructs the base class object for an internal context.
    /// </summary>
    InternalContextBase::InternalContextBase(SchedulerBase *pScheduler) :
        ContextBase(pScheduler, false),
        m_pThreadProxy(NULL),
#ifdef _DEBUG
        _m_pVirtualProcessor(NULL),
#else
        m_pVirtualProcessor(NULL),
#endif // _DEBUG
        m_pOversubscribedVProc(NULL),
        m_pAssociatedChore(NULL),
        m_searchCount(0),
        m_fCanceled(false),
        m_fIsVisibleVirtualProcessor(false),
        m_fHasDequeuedTask(false),
        m_fWorkSkipped(false),
#ifdef _DEBUG
        m_fEverRecycled(false),
        m_workStartTimeStamp(0),
        m_lastRunPrepareTimeStamp(0),
        m_prepareCount(0),
        m_ctxDebugBits(0),
        m_lastDispatchedTid(0),
        m_lastAcquiredTid(0),
        m_lastAffinitizedTid(0),
        m_pAssignedThreadProxy(NULL),
        m_pLastAssignedThreadProxy(NULL),
#endif // _DEBUG
        m_fCrossGroupRunnable(FALSE),
        m_fIdle(true)
    {
        // Initialize base class members.
        m_pSegment = NULL;
    }

    /// <summary>
    ///     Called to find work to switch to when the current context needs to block or nest a different scheduler.
    ///     The function may return NULL if no work was found and thread creation was disallowed by the thread
    ///     throttler.
    /// </summary>
    InternalContextBase* InternalContextBase::FindWorkForBlockingOrNesting(bool& fSFWContext, bool& fBoundUnrealized)
    {
        InternalContextBase *pContext = NULL;
        WorkItem work;
        //
        // Either search for some context or a placement token letting us know that we need to allocate a thread for it.  If
        // we are throttled, said thread creation will not happen.
        //
        if (m_pVirtualProcessor->SearchForWork(&work, m_pSegment, false, WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChoreToken | WorkItem::WorkItemTypeUnrealizedChoreToken))
        {
            if (!work.IsContext())
            {
                //
                // Make sure we can get a thread.  Further, make sure all of this happens outside a critical region to avoid the huge cost of
                // allocation within one.
                //
                ExitCriticalRegion();
                CONCRT_COREASSERT(GetCriticalRegionType() == OutsideCriticalRegion);

                pContext = m_pScheduler->GetInternalContext();
                EnterCriticalRegion();

                if (pContext != NULL)
                {
                    if (work.ResolveToken())
                    {
                        fBoundUnrealized = work.GetType() == WorkItem::WorkItemTypeUnrealizedChore;
                        work.BindTo(pContext);
                    }
                    else
                    {
                        if (m_pVirtualProcessor->SearchForWork(&work, m_pSegment, false, WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChore | WorkItem::WorkItemTypeUnrealizedChore))
                        {
                            if (!work.IsContext())
                            {
                                fBoundUnrealized = work.GetType() == WorkItem::WorkItemTypeUnrealizedChore;
                                work.BindTo(pContext);
                            }
                            else
                            {
                                // Oops -- we have an extra thread due to a race.  Do what's always done and pop it back on the free list.
                                m_pScheduler->ReleaseInternalContext(pContext, true);
                                pContext = work.Bind();
                            }
                        }
                        else
                        {
                            // If we did not find anything in this SFW, the context we just grabbed will be utilized as the SFW context which
                            // deactivates the vproc.
                            //
                            // If this method was called during a Block operation, the context will not get prepared until we fully commit the block
                            // (it will get released in the case of a block/unblock race)
                            fSFWContext = true;
                        }
                    }
                }
                else
                {
                    if (m_pVirtualProcessor->SearchForWork(&work, m_pSegment, false, WorkItem::WorkItemTypeContext))
                        pContext = work.Bind();
                }
            }
            else
            {
                pContext = work.GetContext();
            }
        }
        else
        {
            //
            // Get an SFW context ready.
            //
            pContext = m_pScheduler->GetInternalContext();
            fSFWContext = (pContext != NULL);
        }
        return pContext;
    }

    /// <summary>
    ///     Causes the internal context to block yielding the virtual processor to a different internal context.
    /// </summary>
    void InternalContextBase::Block()
    {
        EnterCriticalRegion();
        ASSERT(this == SchedulerBase::FastCurrentContext());
        ASSERT(m_pVirtualProcessor != NULL);

        TraceContextEvent(CONCRT_EVENT_BLOCK, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

        if (m_pVirtualProcessor->IsMarkedForRetirement())
        {
            // The virtual processor has been marked for retirement. The context needs to switch out rather
            // than switching to a different context.

            // The context switching fence needs to be modified in two steps to maintain parity
            // with the regular block/unblock sequence. Else, we could get into a situation where
            // it has an invalid value.
            if ((InterlockedIncrement(&m_contextSwitchingFence) == 1) && (InterlockedCompareExchange(&m_contextSwitchingFence, 2, 1) == 1))
            {
                TRACE(TRACE_SCHEDULER, L"InternalContextBase::Block->switching out");
                SwitchOut(Blocking);
            }
            else
            {
                // Even if the unblock is skipped, we should not continue running this context since the
                // virtual processor needs to be retired. It should be put on the runnables list and
                // the context should block (which is the same series of steps as when yielding).
                TRACE(TRACE_SCHEDULER, L"InternalContextBase::Block->Unblock was skipped, switching out");
                SwitchOut(Yielding);
            }
        }
        else
        {
            // Execute a different context on the underlying virtual processor.
            if (InterlockedIncrement(&m_contextSwitchingFence) == 1)
            {
                bool fSFWContext = false;
                bool fBoundUnrealized = false;
                InternalContextBase *pContext = FindWorkForBlockingOrNesting(fSFWContext, fBoundUnrealized);

#if defined(_DEBUG)
                CONCRT_COREASSERT(this != pContext);
                if (pContext != NULL)
                {
                    CMTRACE(MTRACE_EVT_SFW_FOUND, this, m_pVirtualProcessor, pContext);
                    CMTRACE(MTRACE_EVT_SFW_FOUNDBY, pContext, m_pVirtualProcessor, this);
                }
#endif
                // Only switch to the other context if unblock has not been called since we last touched the
                // context switching fence. If there was an unblock since, the comparison below will fail.
                if (InterlockedCompareExchange(&m_contextSwitchingFence, 2, 1) == 1)
                {
                    //
                    // *NOTE* After this point, we dare not block.  A racing ::Unblock call can put *US* on the runnables list and the scheduler
                    // will get awfully confused if a UMS activation happens between now and the time we SwitchTo the context below.  Note that
                    // page faults and suspensions are masked by the effect of being in a critical region.  It just means that we cannot call
                    // *ANY* blocking API (including creating a new thread).
                    //
                    if (fSFWContext)
                    {
                        ASSERT(pContext != NULL);
                        pContext->PrepareForUse(m_pSegment, NULL, false);
                    }
                    //
                    // If pContext is NULL, we are stuck.  This virtual processor is blocked and we cannot create a thread.  We must make the virtual
                    // processor available and remove anything from running atop it.  SwitchTo(NULL) implies this behavior.
                    //
                    SwitchTo(pContext, Blocking);
                }
                else
                {
                    // A matching unblock was detected. Skip the block. If a runnable context was found, it needs to be
                    // put back into the runnables collection.

                    // NOTE -- don't look at pContext after pContext->AddToRunnables; it might be gone
                    if (pContext != NULL)
                    {
                        TRACE(TRACE_SCHEDULER, L"InternalContextBase::Block->innerskipblock(ctx=%d,grp=%d)", pContext->GetId(), pContext->GetScheduleGroupId());
                        VCMTRACE(MTRACE_EVT_BLOCKUNBLOCKRACE, pContext, m_pVirtualProcessor, this);

#if defined(_DEBUG)
                        //
                        // For a recycled context, this allows other assertions elsewhere in the codebase to be valid and continue to catch
                        // issues around recycling.
                        //
                        pContext->ClearDebugBits(CTX_DEBUGBIT_RELEASED);
#endif // _DEBUG

                        if (fSFWContext)
                            m_pScheduler->ReleaseInternalContext(pContext, true);
                        else
                        {
                            //
                            // There is a reference count which is handed from the stealer to the chore wrapper which will block a thread from exiting.
                            // Placing a stolen chore bound to a context directly back on the runnables list has a risk of deadlock on 1 vproc unless we
                            // deal with this: transfer the reference count to the stealers list which would normally be done by the chore wrapper.
                            // This act will take a contended lock, so we exit critical region before doing it.
                            //
                            //
                            if (fBoundUnrealized)
                            {
                                CONCRT_COREASSERT(pContext->m_pAssociatedChore != NULL);
                                _UnrealizedChore *pChore = static_cast<_UnrealizedChore *>(pContext->m_pAssociatedChore);

                                ExitCriticalRegion();
                                pChore->_PrepareSteal(pContext);
                                EnterCriticalRegion();
                            }

                            pContext->AddToRunnables(pContext->GetScheduleGroupSegment()->GetAffinity());
                        }
                    }
                }
            }
            else
            {
                // Skip the block
                TRACE(TRACE_SCHEDULER, L"InternalContextBase::Block->outerskipblock(ctx=%d,grp=%d)", GetId(), GetScheduleGroupId());
            }
        }
        ExitCriticalRegion();
    }

    /// <summary>
    ///     Unblocks the internal context putting it on the runnables collection in its schedule group.
    /// </summary>
    void InternalContextBase::Unblock()
    {
        if (this != SchedulerBase::FastCurrentContext())
        {
            LONG newValue = 0;

            newValue = InterlockedDecrement(&m_contextSwitchingFence);

            TraceContextEvent(CONCRT_EVENT_UNBLOCK, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

            if (newValue == 1)
            {
                // Weak assign is ok.  Any other 'LOCK' interaction with m_contextSwitchingFence will
                // flush the correct value through.
                m_contextSwitchingFence = 0;

                // Wait until this context is blocked.
                //
                // SpinUntilBlocked is essential here. Consider the case where the context being unblocked is currently executing the Block
                // API on virtual processor VP1. It is at a point very close to SwitchTo, (after the second interlocked operation), which implies a
                // different context is about to affinitized VP1, to take its place before, before it is switched out.

                // If Unblock puts the 'this' context on a runnables list, it could be pulled off by a different context running on VP2 and get
                // affinitized to VP2. Then SwitchTo in Block is called and the new context is affinitized to VP2 instead of VP1 and VP1 is orphaned.

                // The wait until blocked ensures that the affinitize step in the Block takes place before the context is put onto runnables, by which
                // the correct affinity is set for the new context by the blocking context.

                SpinUntilBlocked();

                TRACE(TRACE_SCHEDULER, L"InternalContextBase::Unblock->runnables(ctx=%d,grp=%d)", GetId(), GetScheduleGroupId());
                AddToRunnables(m_pSegment->GetAffinity());
            }
            else
            {
                if ((newValue < -1) || (newValue > 0))
                {
                    // Should not be able to get m_contextSwitchingFence above 0.
                    ASSERT(newValue < -1);

                    // Too many unblocks without intervening blocks. Block/unblock calls need to balance.
                    TRACE(TRACE_SCHEDULER, L"InternalContextBase::Unblock->unbalanced(ctx=%d,grp=%d)", GetId(), GetScheduleGroupId());

                    throw context_unblock_unbalanced();
                }
                else
                {
                    TRACE(TRACE_SCHEDULER, L"InternalContextBase::Unblock->skipunblock(ctx=%d)", GetId());
                }
            }
        }
        else
        {
            // A context is not allowed to unblock itself.
            TRACE(TRACE_SCHEDULER, L"InternalContextBase::Unblock->selfunblock(ctx=%d,grp=%d)", GetId(), GetScheduleGroupId());
            throw context_self_unblock();
        }
    }

    /// <summary>
    ///     Yields the virtual processor to a different runnable internal context if one is found.
    /// </summary>
    void InternalContextBase::Yield()
    {
        bool bSwitchToThread = false;

        EnterCriticalRegion();
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(m_pVirtualProcessor != NULL);

        TraceContextEvent(CONCRT_EVENT_YIELD, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

        if (m_pVirtualProcessor->IsMarkedForRetirement())
        {
            // The virtual processor has been marked for retirement. The context needs to switch out rather
            // than switching to a different context or continuing to run.
            SwitchOut(Yielding);
        }
        else
        {
            InternalContextBase *pContext = NULL;

            WorkItem work;
            if (m_pVirtualProcessor->SearchForWorkInYield(&work, m_pSegment, false, WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChoreToken))
            {
                if (!work.IsContext())
                {
                    //
                    // Make sure we can get a thread.  Further, make sure all of this happens outside a critical region to avoid the huge cost of
                    // allocation within one.
                    //
                    ExitCriticalRegion();
                    CONCRT_COREASSERT(GetCriticalRegionType() == OutsideCriticalRegion);

                    pContext = m_pScheduler->GetInternalContext();
                    EnterCriticalRegion();

                    if (pContext != NULL)
                    {
                        if (work.ResolveToken())
                        {
                            work.BindTo(pContext);
                        }
                        else
                        {
                            if (m_pVirtualProcessor->SearchForWorkInYield(&work, m_pSegment, false, WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChore))
                            {
                                if (!work.IsContext())
                                {
                                    work.BindTo(pContext);
                                }
                                else
                                {
                                    //
                                    // Oops -- we have an extra thread due to a race (we couldn't claim the token and instead found a runnable that requires
                                    // no thread).  Do what's always done and pop it back on the free list.
                                    //
                                    m_pScheduler->ReleaseInternalContext(pContext, true);
                                    pContext = work.GetContext();
                                }
                            }
                            else
                            {
                                //
                                // Oops -- we have an extra thread due to a race (we couldn't claim the token and could not find anything to run).
                                // Do what's always done and pop it back on the free list.
                                //
                                m_pScheduler->ReleaseInternalContext(pContext, true);
                                pContext = NULL;
                            }
                        }
                    }
                    else
                    {
                        if (m_pVirtualProcessor->SearchForWorkInYield(&work, m_pSegment, false, WorkItem::WorkItemTypeContext))
                            pContext = work.Bind();
                    }
                }
                else
                    pContext = work.GetContext();
            }

            if (pContext != NULL)
            {
                CMTRACE(MTRACE_EVT_SFW_FOUND, this, m_pVirtualProcessor, pContext);
                CMTRACE(MTRACE_EVT_SFW_FOUNDBY, pContext, m_pVirtualProcessor, this);

                ASSERT(pContext != this);

                SwitchTo(pContext, Yielding);
            }
            else
            {
                //
                // No need to cooperatively yield - there's no other runnable context to execute.
                // However, it is wise to check if the OS has any other threads available to run on the hardware thread.
                // On UMS, SwitchToThread will cause a transition to primary. We want to minimize such context
                // switches within critical region. Exit the critical region and then SwitchToThread.
                //
                bSwitchToThread = true;
            }
        }
        ExitCriticalRegion();

        if (bSwitchToThread)
        {
            m_pThreadProxy->YieldToSystem();
        }
    }

    /// <summary>
    ///     Yields the virtual processor to a different runnable internal context if one is found.
    ///
    ///     This is intended for spin loops.
    /// </summary>
    void InternalContextBase::SpinYield()
    {
        bool bSwitchToThread = false;

        EnterCriticalRegion();
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(m_pVirtualProcessor != NULL);

        TraceContextEvent(CONCRT_EVENT_YIELD, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

        if (m_pVirtualProcessor->IsMarkedForRetirement())
        {
            // The virtual processor has been marked for retirement. The context needs to switch out rather
            // than switching to a different context or continuing to run.
            SwitchOut(Yielding);
        }
        else
        {
            WorkItem work;
            if (m_pVirtualProcessor->SearchForWork(&work, m_pSegment, false, WorkItem::WorkItemTypeContext))
            {
                CMTRACE(MTRACE_EVT_SFW_FOUND, this, m_pVirtualProcessor, work.GetContext());
                CMTRACE(MTRACE_EVT_SFW_FOUNDBY, work.GetContext(), m_pVirtualProcessor, this);

                ASSERT(work.GetContext() != NULL && work.GetContext() != this);

                SwitchTo(work.GetContext(), Yielding);
            }
            else
            {
                //
                // No need to cooperatively yield - there's no other runnable context to execute.
                // However, it is wise to check if the OS has any other threads available to run on the hardware thread.
                // On UMS, SwitchToThread will cause a transition to primary. We want to minimize such context
                // switches within critical region. Exit the critical region and then SwitchToThread.
                //
                bSwitchToThread = true;
            }
        }
        ExitCriticalRegion();

        if (bSwitchToThread)
        {
            m_pThreadProxy->YieldToSystem();
        }
    }

    /// <summary>
    ///     See comments for Concurrency::Context::Oversubscribe.
    /// </summary>
    void InternalContextBase::Oversubscribe(bool beginOversubscription)
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        if (beginOversubscription)
        {
            // Increment the context over-subscription counter and only create an additional virtual processor
            // if the count goes from 0 to 1.

            if (++m_oversubscribeCount == 1)
            {
                ASSERT(m_pOversubscribedVProc == NULL);

                // Oversubscribe the hardware thread virtual processor by injecting a virtual processor into the current virtual processors
                // group in the scheduling node.
                EnterCriticalRegion();
                // Oversubscribe invokes a callback to stamp the value of the oversubscribed virtual processor onto the context. The reason
                // for this is that we have to ensure that the vproc <-> context mapping is in place before the virtual processor is added
                // to the collection of vprocs in the scheduler. This is in order to synchronize with RemoveVirtualProcessor, which assumes
                // the virtual processor is fully initialized if it can find it in the collection.
                m_pVirtualProcessor->Oversubscribe();
                ExitCriticalRegion();
            }
        }
        else
        {
            // Decrement the context over-subscription counter and retire the oversubscribed virtual processor
            // if the count goes from 1 to 0.
            if (m_oversubscribeCount == 0)
            {
                throw invalid_oversubscribe_operation();
            }

            if (--m_oversubscribeCount == 0)
            {
                VirtualProcessor * pExpectedVProc = m_pOversubscribedVProc;

                // Note that pExpectedVProc could be null if the RM has already snapped this vproc for removal.
                VirtualProcessor * pVProc = GetAndResetOversubscribedVProc(pExpectedVProc);
                ASSERT(pVProc == NULL || pVProc == pExpectedVProc);

                // We must sychronize with a potential RemoveVirtualProcessor for this virtual processor due to the RM taking the underlying
                // core away. The winner of the interlocked exchange gets to retire the virtual processor.
                if (pVProc != NULL)
                {
                    pVProc->MarkForRetirement();
                }
            }
        }
    }

    /// <summary>
    ///     Called to retreive the oversubscribed vproc and reset it to null.
    /// </summary>
    VirtualProcessor * InternalContextBase::GetAndResetOversubscribedVProc(VirtualProcessor * pExpectedVirtualProcessor)
    {
        // Can be called concurrently by oversubscribing context and the RM. When called by the RM, the argument is
        // non-NULL and represents what the RM thinks this context has as its oversubscribed vproc. The RM could
        // have stale information and so if the virtual processor argument doesn't match what is on the context,
        // we return NULL, informing the RM that the virtual processor it was looking for was already marked for
        // retirement by this context previously.
        VirtualProcessor * pVirtualProcessor = NULL;

        if ((pExpectedVirtualProcessor != NULL) && (pExpectedVirtualProcessor == m_pOversubscribedVProc) &&
            (InterlockedCompareExchangePointer((volatile PVOID *)(&m_pOversubscribedVProc), (void*) 0, pExpectedVirtualProcessor) == pExpectedVirtualProcessor))
        {
                pVirtualProcessor = pExpectedVirtualProcessor;
        }

        return pVirtualProcessor;
    }

    /// <summary>
    ///     Returns an identifier to the virtual processor the context is currently executing on, if any.
    /// </summary>
    unsigned int InternalContextBase::GetVirtualProcessorId() const
    {
        //
        // We really aren't changing anything, so cast away constness to enter the critical reigon.  The critical region is necessary
        // to guard volatility on UMS reentrancy due to PF when accessing m_pVirtualProcessor.
        //
        (const_cast<InternalContextBase *>(this))->EnterCriticalRegion();
        unsigned int id = (m_pVirtualProcessor != NULL) ? m_pVirtualProcessor->GetId() : UINT_MAX;
        (const_cast<InternalContextBase *>(this))->ExitCriticalRegion();

        return id;
    }

    /// <summary>
    ///     Adds the context to a runnables collection, either on the virtual processor, or the schedule group
    /// </summary>
    /// <param name="bias">
    ///     A location specifying where to bias the awakening of virtual processors to.
    /// </param>
    void InternalContextBase::AddToRunnables(location bias)
    {
        ASSERT(m_pSegment != NULL);
        ASSERT(m_pThreadProxy != NULL);

        TRACE(TRACE_SCHEDULER, L"InternalContextBase::AddRunnable(ctx=%d,grp=%d,grpRef=%d)", GetId(), GetScheduleGroupId(), ScheduleGroupRefCount());

        ContextBase* pCurrentContext = SchedulerBase::FastCurrentContext();

        CMTRACE(MTRACE_EVT_ADDEDTORUNNABLES, this, NULL, pCurrentContext);
        CMTRACE(MTRACE_EVT_INVERTED_ADDEDTORUNNABLES, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, NULL, this);

        //
        // If there is an "inactive pending thread" virtual processor, this runnable should be shoved to it instead of going through the normal
        // wake path.  There is *NO REASON* to require an SFW context to immediately switch to this.
        //
        if (m_pScheduler->HasVirtualProcessorPendingThreadCreate() && m_pScheduler->PushRunnableToInactive(this, bias))
            return;

        //
        // First see if there is room to place 'this' on the cache of local realized chores
        // for the ambient context.  This attempts to maintain cache locality when Block/Unblock
        // is called in quick succession and the unblocking current context subsequently blocks.
        //
        if (pCurrentContext != NULL && !pCurrentContext->IsExternal() && (m_pScheduler == pCurrentContext->GetScheduler()))
        {
            InternalContextBase* pContext = static_cast<InternalContextBase*>(pCurrentContext);
            int count;
            //
            // The current virtual processor is only safely accessed within a critical region
            //
            pContext->EnterCriticalRegion();

            //
            // We will only push to the LRC of this virtual processor if this virtual processor is within the affinity set of the segment
            // to which the context belongs.
            //
            if (!m_pSegment->GetGroup()->IsFairScheduleGroup()
                        &&
                 m_pSegment->GetAffinitySet().IsSet(pContext->m_pVirtualProcessor->GetMaskId())
                        &&
                ((count = pContext->m_pVirtualProcessor->m_localRunnableContexts.Count()) < m_pScheduler->m_localContextCacheSize))
            {
                //
                // If the current context does not belong to the same group, the caller is not guaranteed to have a reference to the
                // schedule group. We call CrossGroupRunnable() to make sure that scheduler and schedule group are kept around long
                // enough, that we can attempt to startup the virtual processor without fear of the scheduler being finalized, or the
                // schedule group being destroyed.
                // If the current context DOES belong to same group as 'this', it is possible for it to be recycled to the idle pool
                // once we add it to runnables collection. Since the m_pSegment field is reset to NULL when the context is recycled,
                // we cache it up front.
                //
                ScheduleGroupSegmentBase * pSegment = m_pSegment;
                if (pContext->GetScheduleGroup() != pSegment->GetGroup())
                {
                    // Set this flag to allow the calling thread to use m_pSegment safely once the context is pushed onto runnables.
                    // Note that this call does not need a fence. The addition of the context to the vproc LRC queue, which is a work-stealing
                    // queue, is unfenced, but since both, setting the flag, and adding to the queue, result in volatile writes, other processors
                    // will see the stores in the same order. That means that when this context is visible to a stealer, the stealer will also
                    // see the cross group runnable bit set.
                    CrossGroupRunnable(TRUE);
                }

#if defined(_DEBUG)
                SetDebugBits(CTX_DEBUGBIT_ADDEDTOLOCALRUNNABLECONTEXTS);
                if (m_pScheduler->HasVirtualProcessorAvailable())
                    SetDebugBits(CTX_DEBUGBIT_LIKELYTOSTARTUPIDLEVPROCONOTHERCONTEXT);
#endif // _DEBUG
                pContext->m_pVirtualProcessor->m_localRunnableContexts.Push(this);
                // IMPORTANT NOTE: 'this' could be recycled and reused by this point, unless the cross group runnables flag is set. (If the
                // flag IS set, we are guaranteed that the context's group will not be set to NULL/destroyed, and that the context will not
                // be recycled until we set the flag to false below).
                // We can, however, access m_pScheduler for a recycled context, since it retains the same value until the context is destroyed,
                // and contexts are only destroyed during scheduler shutdown.
                CMTRACE(MTRACE_EVT_AVAILABLEVPROCS, this, pContext->m_pVirtualProcessor, m_pScheduler->m_virtualProcessorAvailableCount);

                if (m_pScheduler->HasVirtualProcessorAvailable())
                {
#if defined(_DEBUG)
                    pContext->SetDebugBits(CTX_DEBUGBIT_STARTUPIDLEVPROCONADD);
#endif // _DEBUG
                    m_pScheduler->StartupIdleVirtualProcessor(pSegment, bias);
                }

                if (pContext->GetScheduleGroup() != pSegment->GetGroup())
                {
                    // Reset the flag, if it was set, since we're done with touching scheduler/context data.
                    // This flag is not fenced. This means the reader could end up spinning a little longer until the data is
                    // propagated by the cache coherency mechanism.
                    CrossGroupRunnable(FALSE);
                    // NOTE: It is not safe to touch 'this' after this point, if this was a cross group runnable.
                }

                pContext->ExitCriticalRegion();
                return;
            }
            pContext->ExitCriticalRegion();
        }

#if defined(_DEBUG)
        SetDebugBits(CTX_DEBUGBIT_ADDEDTORUNNABLES);
#endif // _DEBUG

        m_pSegment->AddRunnableContext(this, bias);
    }

    /// <summary>
    ///     Spins until the 'this' context is in a firmly blocked state.
    /// </summary>
    /// <remarks>
    ///     This implements a sort of barrier. At certain points during execution, it is essential to wait until a context
    ///     has set the flag inidicating it is blocked, in order to preserve correct behavior.
    ///     One example is if there is a race between block and unblock for the same context, i.e. if a context is trying to
    ///     block at the same time a different context is trying to unblock it.
    /// </remarks>
    void InternalContextBase::SpinUntilBlocked()
    {
        ASSERT(SchedulerBase::FastCurrentContext() != this);

        if (!IsBlocked())
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();

            } while (!IsBlocked());
        }
        ASSERT(IsBlocked());
    }

    /// <summary>
    ///     Swaps the existing schedule group with the one supplied. This function should be called when the context already
    ///     has a schedule group. It decrements the existing group reference count, and references the new one if the caller
    ///     indicates so.
    /// </summary>
    /// <param name="pNewSegment">
    ///     The new segment to assign to the context. This may be NULL.
    /// </param>
    /// <param name="referenceNewGroup">
    ///     Whether the context should reference the new group. In some cases, there may be an existing reference
    ///     transferred to the context, in which case this parameter is false.
    /// </param>
    void InternalContextBase::SwapScheduleGroupSegment(ScheduleGroupSegmentBase* pNewSegment, bool referenceNewGroup)
    {
        if (m_pSegment == NULL)
        {
            ASSERT(pNewSegment == NULL);
            return;
        }

        // We expect that a context modifies its non-null schedule group only when it is running.
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT((pNewSegment != NULL) || (!referenceNewGroup));

        // Before releasing the reference count on the schedule group, which could end up destroying the schedule group if the ref
        // count falls to zero, check if the m_fCrossGroupRunnable flag is set. If it is, it means a different thread that previously added
        // this context to a runnables collection, is relying on the group being alive. Also, since the current call is executing within
        // some context's dispatch loop, and every running dispatch loop has a reference on the scheduler, we are guaranteed that scheduler
        // finalization will not proceed while this flag is set on any context inside a scheduler.
        SpinUntilValueEquals(&m_fCrossGroupRunnable, FALSE);

        // Segments are bound to the lifetime of their group.
#if _UMSTRACE
        OMTRACE(MTRACE_EVT_GROUPSWAP, m_pSegment, this, m_pVirtualProcessor, pNewSegment);
        OMTRACE(MTRACE_EVT_GROUPSWAP, m_pSegment->GetGroup(), this, m_pVirtualProcessor, pNewSegment);
#endif // _UMSTRACE
        m_pSegment->GetGroup()->InternalRelease();
        if (referenceNewGroup)
        {
            pNewSegment->GetGroup()->InternalReference();
        }
        m_pSegment = pNewSegment;
    }

    /// <summary>
    ///     Switches from one internal context to another.
    /// </summary>
    /// <param name="pNextContext">
    ///     The context to switch to.  If this is NULL, we switch to the default destination if one exists.  If no default destination exists,
    ///     the virtual processor is turned inactive as determined by reason.  On the UMS scheduler, the default destination is the primary.
    ///     On the thread scheduler, there is no default destination.
    /// </param>
    /// <param name="reason">
    ///     Specifies the reason the switch is occurring.
    /// </param>
    void InternalContextBase::SwitchTo(InternalContextBase* pNextContext, ReasonForSwitch reason)
    {
        CMTRACE(MTRACE_EVT_SWITCHTO, this, m_pVirtualProcessor, pNextContext);

        SwitchingProxyState switchState = ::Concurrency::Blocking;

        // **************************************************
        //
        // There is a dangerous zone between the call to Affinitize and the end of pThreadProxy->SwitchTo.  If we trigger a UMS block for
        // any reason, we can corrupt the virtual processor state as we reschedule someone else, come back, and don't properly have pNextContext
        // affinitized.
        //
        // If we call any BLOCKING APIs (including utilization of our own locks), there are potential issues as something else might
        // be rescheduled on this virtual processor from the scheduling context.
        //
        // **************************************************

        //
        // Various state manipulations which may take locks or make arbitrary blocking calls happen here.  This must be done outside the inclusive
        // region of [Affinitize, pThreadProxy->SwitchTo].  Otherwise, our state can become corrupted if a page fault or blocking operation triggers
        // UMS activation in that region.
        //
        switch (reason)
        {
        case GoingIdle:
            CONCRT_COREASSERT(m_pAssociatedChore == NULL);
            VCMTRACE(MTRACE_EVT_SWITCHTO_IDLE, this, m_pVirtualProcessor, pNextContext);

            //
            // The scheduler has an idle pool of contexts, however, before putting a context on this pool, we must
            // disassociate it from its thread proxy - so that if it is picked up off the free list by a different
            // caller, that caller will associate a new thread proxy with it. The reason for this disassociation is,
            // that we want to pool thread proxies in the RM, and not the scheduler.
            //
            // The state of the context cannot be cleared until context reaches the blocked state. It's possible we
            // block/page fault somewhere lower and require the information until m_blockedState is set to blocked.
            //
            TraceContextEvent(CONCRT_EVENT_IDLE, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

            // It makes no sense to SwitchTo with GoingIdle as a parameter if there is no context to switch to. This is true
            // for thread and UMS schedulers.
            CONCRT_COREASSERT(pNextContext != NULL);
            TRACE(TRACE_SCHEDULER, L"InternalContextBase::SwitchTo(dispatch:pNextContext->(ctx=%d,grp=%d))", pNextContext->Id(), pNextContext->ScheduleGroupId());

            m_pSegment->ReleaseInternalContext(this);

            // **************************************************
            // Read this extraordinarily carefully:
            //
            // This context is on the free list.  Meaning someone can grab and switch to it.  Unfortunately, this means
            // we might page fault or block here.  That operation would instantly set m_blockedState, which would release
            // the guy spinning and suddenly we have two virtual processors in-fighting over the same context.
            //
            // Because we are inside a critical region, no page faults are observable to the scheduler code.  This does
            // mean that you cannot call *ANY BLOCKING* API between this marker and the EnterHyperCriticalRegion below.
            // API between this marker and the EnterHyperCriticalRegion below.  If you do, you will see random behavior
            // or the primary will assert at you.
            // **************************************************

            switchState = ::Concurrency::Idle;
            break;

        case Yielding:
            //
            // Add this to the runnables collection in the schedule group.
            //
            VCMTRACE(MTRACE_EVT_SWITCHTO_YIELDING, this, m_pVirtualProcessor, pNextContext);

            if (pNextContext != NULL)
            {
                TRACE(TRACE_SCHEDULER, L"InternalContextBase::SwitchTo(yield:pNextContext->(ctx=%d,grp=%d))", pNextContext->Id(), pNextContext->ScheduleGroupId());
            }

            CONCRT_COREASSERT(switchState == ::Concurrency::Blocking);
            m_pSegment->AddRunnableContext(this, m_pSegment->GetAffinity());
            break;

        case Blocking:
            VCMTRACE(MTRACE_EVT_SWITCHTO_BLOCKING, this, m_pVirtualProcessor, pNextContext);

            if (pNextContext != NULL)
            {
                TRACE(TRACE_SCHEDULER, L"InternalContextBase::SwitchTo(block:pNextContext->(ctx=%d,grp=%d))", pNextContext->Id(), pNextContext->ScheduleGroupId());
            }

            CONCRT_COREASSERT(switchState == ::Concurrency::Blocking);
            break;

        case Nesting:
            VCMTRACE(MTRACE_EVT_SWITCHTO_NESTING, this, m_pVirtualProcessor, pNextContext);

            if (pNextContext != NULL)
            {
                TRACE(TRACE_SCHEDULER, L"InternalContextBase::SwitchTo(nest:pNextContext->(ctx=%d,grp=%d))", pNextContext->Id(), pNextContext->ScheduleGroupId());
            }

            switchState = ::Concurrency::Nesting;
            break;
        }

        EnterHyperCriticalRegion();

        //
        // No one can reuse the context until we set the blocked flag.  It can come off the idle list, but the thread pulling it off the idle list will
        // immediately spin until blocked inside the acquisition.  It is entirely possible, however, that the moment we flip the blocked flag, the spinner
        // gets released and the proxy fields, etc...  are overwritten.  We still own the thread proxy from the RM's perspective and the RM will sort out
        // races in its own way.  We must, however, cache the thread proxy before we set the blocked flag and not rely on *ANY* fields maintained by the *this*
        // pointer after the flag set.
        //
        VirtualProcessor *pVirtualProcessor = m_pVirtualProcessor;
        SchedulerBase *pScheduler = m_pScheduler;
        m_pVirtualProcessor = NULL;

        CONCRT_COREASSERT(!IsBlocked());

#if defined(_DEBUG)
        ClearDebugBits(CTX_DEBUGBIT_AFFINITIZED);

        if (reason != GoingIdle)
            SetDebugBits(CTX_DEBUGBIT_COOPERATIVEBLOCKED);
#endif // _DEBUG

        CONCRT_COREASSERT(m_pThreadProxy != NULL);
        IThreadProxy *pThreadProxy = m_pThreadProxy;

        IExecutionContext *pDestination = (IExecutionContext *)pNextContext;
        if (pDestination == NULL)
        {
            //
            // A SwitchTo(, NULL) may have different semantic meaning depending on the scheduler.  It may go to a default destination (e.g.: the UMS
            // primary) or simply cause nothing to be run (e.g.: the thread scheduler).
            //
            pDestination = pVirtualProcessor->GetDefaultDestination();
            CONCRT_COREASSERT(pDestination != NULL || m_pScheduler->GetPolicy().GetPolicyValue(SchedulerKind) == ::Concurrency::ThreadScheduler);
        }

        ASSERT(pDestination != NULL || reason != GoingIdle);
        //
        // The blocked flag needs to be set on the context to prevent the block-unblock race as described in
        // VirtualProcessor::Affinitize. In addition, it is used during finalization to determine whether
        // work exists in the scheduler.
        //
        InterlockedExchange(&m_blockedState, CONTEXT_BLOCKED);

        // **************************************************
        // At this point, it unsafe to touch the *this* pointer.  You cannot touch it, debug with it, rely on it.  It may be reused if
        // reason == GoingIdle and represent another thread.
        // **************************************************

        // The 'next' context must be affinitized to a copy of the 'this' context's vproc that was snapped, BEFORE
        // the blocked flag was set. Not doing this could result in vproc orphanage. See VirtualProcessor::Affinitize
        // for details. We cache the vproc pointer in a local variable before setting m_blockedState. Thus re-affinitizing
        // the 'this' context would not affect the vproc that the 'next' context is going to get affinitized to.
        // With UMS, if the pNextContext is NULL, the vproc affinitizes the scheduling Context.
        pVirtualProcessor->Affinitize(pNextContext);

        CONCRT_COREASSERT(pNextContext == NULL || pNextContext->m_pThreadProxy != NULL);

#if defined(_DEBUG)
        if (pNextContext != NULL && pNextContext->m_pAssociatedChore != NULL)
            pNextContext->SetDebugBits(CTX_DEBUGBIT_SWITCHTOWITHASSOCIATEDCHORE);
#endif // _DEBUG

        //
        // If there is no default place to switch to (e.g.: the UMS primary), we simply switch out and do not run anything atop the virtual processor.
        // The caller has responsibility to do something intelligent with the virtual processor to mark it as available for whatever purpose they see.
        //
        if (pDestination == NULL)
        {
            //
            // *** READ THIS: ***
            //
            // It is monumentally important that the below calls to make a virtual processor available in some form never finalize the scheduler.
            // Though they may in certain cases lead to a sweep, they can never finalize.  Our context is marked as blocked and is *NOT* on the free list.
            // The sweep will find *this* and roll back.  Granted, it is possible that another vproc grabs *this* and starts trying to execute it because
            // its blocked flag is already set to true, but we have not yet executed the context switch from the RM's perspective and we rely on the RM to
            // resolve that particular race.
            //
            switch(reason)
            {
                case Blocking:
                case Nesting:
                    pVirtualProcessor->MakeAvailablePendingThread();
                    pScheduler->DeferredGetInternalContext();
                    break;

                default:
                    ASSERT(false);
                    break;
            }

            pThreadProxy->SwitchOut(switchState);
        }
        else
        {
            // If this assert fires, you're executing on the UMS Scheduler. A bugfix was made to this function to allow InternalContextBase::SwitchTo(NULL, Nesting)
            // to work for the thread scheduler. This fix should be evaluated for UMS it it becomes necessary.
            CONCRT_COREASSERT(pDestination == pNextContext || reason != Nesting);

            pThreadProxy->SwitchTo(pDestination, switchState);
        }

        //
        // The m_blockedState is cleared in Affinitize() when someone tries to re-execute this context.
        //
        if (reason != GoingIdle)
            ExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Switches out the internal context. Useful, when the virtual processor is to be retired.
    ///     Is also used when un-nesting a scheduler and the context is returning to its original scheduler.
    /// </summary>
    /// <param name="reason>
    ///     Specifies the reason the context is switching out.
    /// </param>
    /// <returns>
    ///     True if the context is canceled. This would happen only when reason == GoingIdle
    /// </returns>
    bool InternalContextBase::SwitchOut(ReasonForSwitch reason)
    {
        // If this context is about to be added to the idle pool, it could get picked up for reuse and reinitialized in
        // a call to GetInternalContext(). It will *NOT* get re-affinitized or reinitialized until we set the blocked flag
        // below. Save away the thread proxy before setting the blocked flag

        IThreadProxy * pThreadProxy = m_pThreadProxy;
        bool isCanceled = false;

        if (m_pVirtualProcessor != NULL)
        {
            // If this internal context is in the last stage of looking for work when it was asked
            // to switch out (retire), then we need to make sure that no one grabs it to do work.
            // That's why we make sure it has exclusive access to virtual processor.
            ReclaimVirtualProcessor();

            // The context is switching out due to the underlying virtual processor being marked for retirement.
            CONCRT_COREASSERT(!m_pVirtualProcessor->IsAvailable());

            // The vproc can safely be removed from lists within the scheduling node, etc. The finalization sweep that suspends
            // virtual processors and waits for them to check in cannot be executing at this time since the counts of idle
            // and active vprocs are guaranteed to be unequal.
            CONCRT_COREASSERT(!m_pScheduler->InFinalizationSweep() && !m_pScheduler->HasCompletedShutdown());

            // Virtual processor retirement needs to be in a hypercritical region. Since the vproc is being
            // retired it is safe to assume that we are not responsible for scheduling other work on this vproc.

#if defined(_DEBUG)
            SetShutdownValidations();
#endif // _DEBUG

            // Make a copy of the safepoint marker so that we could trigger commit later
            SafePointMarker safePointMarker = m_pVirtualProcessor->m_safePointMarker;

            EnterHyperCriticalRegion();
            m_pVirtualProcessor->Retire();
            m_pVirtualProcessor = NULL;

            if (reason != GoingIdle)
            {
                ASSERT(reason == Blocking || reason == Yielding);
                // For the cases where we are switching out while blocking, we need to exit the hypercritical region,
                // as the context could be unblocked later and run user code (chore). If we're going Idle,
                // the hypercritical region will be exited when this context is reinitialized, after being picked up
                // off of the free pool.
#if defined(_DEBUG)
                ClearShutdownValidations();
#endif // _DEBUG
                ExitHyperCriticalRegion();

                //
                // For blocking and yielding contexts, the context should be marked as blocked *before* calling
                // VirtualProcessorActive(false), so that finalization will roll back if the idle and active vproc counts become
                // equal (gate count is 0). Essentially, the moment this virtual processor decrements the gate count in
                // VirtualProcessorActive(false), it is not part of the scheduler anymore, and unless the blocked flag is set
                // here, the scheduler may finalize without resuming this context when it is ready to run.
                //
                // In addition, this should be done *after* all accesses to m_virtualProcessor. If this is a 'Block' operation,
                // an unblock could put this context on a runnables collection, and it could be taken off and re-affinitized,
                // changing the value of m_virtualProcessor out from under us. Moreover, setting m_virtualProcessor to null here,
                // ensures that we will quickly catch future bugs where it is accessed after this point and before the context
                // waits on the block event.
                //
                CONCRT_COREASSERT(!IsBlocked());
                InterlockedExchange(&m_blockedState, CONTEXT_BLOCKED);
            }
            else
            {
                // For the GoingIdle case, the sequence of events strictly needs to be as follows:
                //        1.  add 'this' to the idle pool
                //        2.  invoke VirtualProcessorActive(false) - making the virtual processor inactive
                //        3.  all other accesses to 'this'
                //        4.  set the blocked flag.
                // After the blocked flag is set while the context is on the idle pool, it is unsafe to touch *this*. The context
                // could be repurposed, or even deleted if the scheduler shuts down. It is important to note that the context is
                // inside a hypercritical region here. Therefore, in the UMS scheduler, all blocking operations are hidden from us.

                //
                // If the context is going idle, it should *not* be marked as blocked until after it is put on the idle queue.
                // During finalization, the scheduler ignores all contexts marked as blocked that are also on the idle queue for
                // the purpose of determining if any work is remaining to be done. If this context is marked as blocked before
                // it is on the idle queue, and the scheduler is simultaneously sweeping for finalize, it may incorrectly assume
                // that this is a blocked context that will become runnable in the future. This could hang finalization.
                //

                // Since we're going idle on a switch out operation, once we pass the VirtualProcessorActive(false) call below, we're no
                // longer considered part of the scheduler, and we need to worry about the scheduler shutting down simultaneously.
                // If we're blocking or yielding, there's no problem, because a scheduler cannot shutdown while there is a blocked
                // context. However, since we're going idle, a different thread (either a virtual processor or an external thread)
                // could initiate a sweep for finalize (if the conditions are met). It is unsafe for us to add this context to
                // the idle pool WHILE a sweep is concurrently going on. The sweep code goes through and checks to see if any contexts
                // not on the free list (not marked idle) have their blocked flag set. If we're racing with the sweep, we could add this
                // context to the free list and set its blocked flag between the time the sweep checks the idle and blocked state.
                // It is possible to hang finalization in this case, since the sweeping thread will believe it has found a 'blocked context'
                // and roll back finalization. Therefore we MUST add this context to the idle pool BEFORE making the VirtualProcessorActive
                // call.

                // Return to the idle pool. This first puts the context into the idle pool of the scheduler instance.
                // If the idle pool is full, the scheduler will return the context to the resource manager.
                TraceContextEvent(CONCRT_EVENT_IDLE, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);
                TRACE(TRACE_SCHEDULER, L"ThreadInternalContext::SwitchOut(idle)");
                m_pSegment->ReleaseInternalContext(this);
            }

            //
            // If the reason is "blocking", the context could now appear on the runnables list. As a result we shall not make
            // any synchronous UMS blocking calls such as attempting to acquire heap lock etc from this point on. If we do and
            // are blocked on a lock that is held by a UT, the remaining vproc might not be able to run the UT as it could be
            // spinning on this context.
            //

            //
            // In the event that this virtual processor hadn't yet observed safe points, we need to make sure that its removal commits
            // all data observations that are okay with other virtual processors. Since safe point invocations could take arbitrary
            // locks and block, we trigger safe points on all the virtual processors (we have removed ourselves from that list).
            //
            m_pScheduler->TriggerCommitSafePoints(&safePointMarker);

            // Reducing the active vproc count could potentially lead to finalization if we're in a shutdown semantic.
            // If that happens to be the case (it can only happen if the context switch reason is GoingIdle), we will exit the
            // dispatch loop and return the thread proxy to the RM - the virtual processor has been retired which means the
            // underlying virtual processor root has been destroyed. We have already removed the virtual processor from the
            // lists in the scheduler, so the underlying thread proxy will not get 'woken up' via a subsequent call to Activate
            // on the underlying vproc root.
            m_pScheduler->VirtualProcessorActive(false);
            CONCRT_COREASSERT(!m_fCanceled || (m_pScheduler->HasCompletedShutdown() && (reason == GoingIdle)));

            // Make a local copy of m_fCanceled before we set m_blockedState. On sheduler shutdown,
            // m_fCanceled is set to true.  In this case, we need to do cleanup. The field need
            // to be cached since another vproc could pick this context up and set the m_fCanceled flag
            // before we do the check again to invoke cleanup.
            isCanceled = m_fCanceled;

            if (reason == GoingIdle)
            {
                // After VirtualProcessorActive(false) and all accesses to 'this' it is safe to set m_blockedState while going idle.
                CONCRT_COREASSERT(!IsBlocked());
                InterlockedExchange(&m_blockedState, CONTEXT_BLOCKED);
            }
        }
        else
        {
            // This is a nested context returning to its parent scheduler.
            CONCRT_COREASSERT(reason == Nesting);
            CONCRT_COREASSERT(IsBlocked());
        }

        if (reason == Yielding || reason == Nesting)
        {
            // Add this to the runnables collection in the schedule group.
            TRACE(TRACE_SCHEDULER, L"ThreadInternalContext::SwitchOut(yield/nest)");
            m_pSegment->AddRunnableContext(this, m_pSegment->GetAffinity());
        }

        // If we're going idle there is no need to execute a 'switch'. The underlying virtual processor root needs to be reset,
        // since no one is going to run on it, and this will be done when the context returns from the dispatch loop in the
        // thread proxy dispatch routine.
        if (reason != GoingIdle)
        {
            // For both yielding and nesting, the intended outcome is to have the thread proxy block in the RM. Therefore we
            // set the switch state to Blocking.
            pThreadProxy->SwitchOut(::Concurrency::Blocking);
        }
        //
        // m_blockedState will be reset when we affinitize the context to re-execute it.
        //
        if (isCanceled)
        {
            // We could be canceled only if we are going idle.
            CONCRT_COREASSERT(reason == GoingIdle);
        }

        return isCanceled;
    }

    /// <summary>
    ///     Called when a context is nesting a scheduler. If nesting takes place on what is an internal context in
    ///     the 'parent' scheduler, the context must return the virtual processor to the parent scheduler
    /// </summary>
    void InternalContextBase::LeaveScheduler()
    {
        EnterCriticalRegion();

        // Find a context to take over the underlying virtual processor and switch to it. When a context switches to a
        // different context with the reason 'Nesting', the SwitchTo API will affinitize the context we found to
        // the virtual processor 'this' context is running on, and return - allowing the underlying thread proxy to
        // join a nested scheduler as an external context.
        bool fSFWContext = false;
        bool fBoundUnrealized = false;
        InternalContextBase *pContext = FindWorkForBlockingOrNesting(fSFWContext, fBoundUnrealized);
        ASSERT(this != pContext);

        if (fSFWContext)
        {
            ASSERT(pContext != NULL);
            pContext->PrepareForUse(m_pSegment, NULL, false);
        }
        // If pContext is NULL due to thread throttling, the virtual processor will be made available and the throttler
        // will be notified that this vproc is pending a thread.
        SwitchTo(pContext, Nesting);

        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(m_pVirtualProcessor == NULL);
        ASSERT(m_pSegment != NULL);
        ASSERT(IsBlocked());

        ExitCriticalRegion();
    }

    /// <summary>
    ///     Called when a internal context detaches from a nested scheduler. The context must find a virtual processor
    ///     on a previous context before it may run.
    /// </summary>
    void InternalContextBase::RejoinScheduler()
    {
        EnterCriticalRegion();

        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(m_pVirtualProcessor == NULL);
        ASSERT(m_pSegment != NULL);
        ASSERT(IsBlocked());

        // Switch out - this will take care of putting this context on a runnables queue and waking up a virtual processor
        // if one is available.
        SwitchOut(Nesting);
        ExitCriticalRegion();
    }

    /// <summary>
    ///     Wait for work algorithm:
    ///
    ///     We search numSearches times through the loop looking for work and then we allow other threads to claim this virtual processor
    ///     while we deactivate the virtual processor root, asking to be woken up when all processor write buffers are flushed
    ///     (signified by the m_fIsVisibleVirtualProcessor flag). After that we do *one* more full search for work.
    ///
    ///     Scenario #1: If the final sweep (after the flush) does not yield any work, we set the virtual processor to Idle
    ///     deactivate the virtual processor root. We will be woken up when work comes in, when we are canceled
    ///     due to the scheduler shutting down, or when the virtual processor we're running on was marked for retirement.
    ///     If we are not canceled, we simply reset and start over. If we are, we cleanup and exit the dispatch loop.
    ///
    ///     Scenario #2: If the final sweep does find work and this virtual processor is still available, we claim it and
    ///     do work.
    ///
    ///     Scenario #3: If the final sweep does find work and this virtual processor is not available, then either someone has activated
    ///     the underlying root while adding more work (via StartupIdleVirtualProcessor), which takes it from available list
    ///     (pVProc->IsAvailable()), or the virtual processor we're running on was retired due to core migration (via RemoveVirtualProcessors).
    ///     In this we execute a Deactivate to consume the activation. Note that even if we were marked for retirement, we will execute
    ///     the chore we just picked up, which will delay retirement a bit.
    /// </summary>
    void InternalContextBase::WaitForWork()
    {
        const unsigned int numSearches = 256;

        //
        // Once we've completed one pass of SFW, notify the scheduler that we're actively searching so anything that pops up affine to us isn't ripped
        // out from underneath us.
        //
        if (++m_searchCount == 1)
        {
            m_pScheduler->NotifySearching(m_pVirtualProcessor->GetMaskId(), true);
        }

        CMTRACE(MTRACE_EVT_SFW_NEXTLOOP, this, m_pVirtualProcessor, m_searchCount);

        if (m_searchCount < numSearches)
        {
            // Yield thread helps perf for oversubscribed vprocs
            m_pThreadProxy->YieldToSystem();

            CONCRT_COREASSERT(!m_fIsVisibleVirtualProcessor);
            // Do another search for the work within the loop.
        }
        else if (m_searchCount == numSearches)
        {
            // At this point virtual processor has to be un-available for everyone but this
            // internal context.
            CONCRT_COREASSERT(!m_pVirtualProcessor->IsAvailable());

            // At this point, we've made the virtual processor 'visible' to the rest of the scheduler. This means that anyone
            // adding work to the scheduler is able to grab this virtual processor and assume that it will find the work that
            // was added.
            m_fIsVisibleVirtualProcessor = true;

            // Make this virtual processor available and force all tasks to be visible. The idea is that any work queued *before*
            // the point at which we made the virtual processor available is visible after the API call, and we should be able to
            // make one single pass through the scheduler to find any such work. Work queued *after* the point at which we made
            // the virtual processor available should be able to wake up the virtual processor.
            m_pVirtualProcessor->MakeAvailableForIdle();

            m_pVirtualProcessor->EnsureAllTasksVisible(this);
            // If we find work during our final search, we will reclaim the virtual processor and reset the search count.

            // Context could not have been canceled since this vproc is not 'idle'.
            CONCRT_COREASSERT(m_fCanceled == 0);
        }
        else
        {
            CONCRT_COREASSERT(m_searchCount == numSearches + 1);
            CONCRT_COREASSERT(m_fIsVisibleVirtualProcessor);

            if (m_fWorkSkipped)
            {
                m_searchCount--;

                //
                // Account for the cases where we fail to check some of the work-stealing queues due to task collection cancellation in progress.
                // If we skip work and deactivate the vproc, the work left in the queue could never be picked up. An example would be a
                // chore that blocks on a win32 event that is to be signaled by a queued chore which essentially requires stealing. If the
                // only other vproc goes idle incorrectly (because it wasn't able to steal from this queue), then the application would hang.
                //

                //
                // Go back and search again.  Hopefully the owning context of the wsq that was skipped has finished canceling.
                //
                m_pThreadProxy->YieldToSystem();
            }
            else
            {
                // Notify the scheduler that this virtual processor is idle, right before going into a sleep state.
                m_pScheduler->VirtualProcessorIdle(true);

                // Note that the previous call to VirtualProcessorIdle could well be the one that takes the scheduler into PhaseTwoShutdown,
                // if this happens to be the last active vproc in the scheduler to go idle, AND no external references to the scheduler exist,
                // AND no work remains. In this case we expect that this context is canceled.
                CONCRT_COREASSERT( !m_fCanceled || m_pScheduler->InFinalizationSweep() || m_pScheduler->HasCompletedShutdown());

                // Deactivate the virtual processor for real this time. We will return out of deactivate for one of the following reasons:
                //      1] Someone adds work and wakes us up.
                //      2] The scheduler hass shutdown and this context was canceled (and woken up)
                //      3] The RM is in the process of removing virtual processors dut do core migration, and marks the underlying vproc for retirement.
                // Even if we were canceled during the call to virtual processor idle, we must call deactivate to consume the
                // signal sent to the thread proxy (via Activate, when the virtual processor was canceled).
#if defined(_DEBUG)
                bool fRMAwaken = false;
#endif // _DEBUG

                CONCRT_COREASSERT(!IsBlocked());
                while ( !m_pVirtualProcessor->Deactivate(this))
                {
                    //
                    // The resource manager has woken us up because of a completion notification from the completion list.  It's entirely possible
                    // that another thread was running, pulled it, and went idle.  Since we are in idle the VirtualProcessorIdle call above, that other virtual
                    // processor could have triggered finalization and we could be *IN* phase two shutdown right now.  Racing with phase two shutdown
                    // and VirtualProcessorIdle() / ClaimExclusiveOwnership() is *NOT* healthy for correct finalization.  Instead, we simply pretend that this
                    // virtual processor is still idle and simply *borrow* the *thread* to move completion list items.  The movement will translate into
                    // AddToRunnables calls which will activate virtual processors if there was something blocked.  If that was the case, we couldn't be in phase
                    // two finalization.
                    //
#if defined(_DEBUG)
                    fRMAwaken = true;
#endif // _DEBUG
                    RMAwaken();
                    //
                    // At this point, if things have moved from the completion list to runnables, the virtual processor might have been activated.  The RM will handle
                    // the activate/deactivate race and the looping around will swallow the activate and let us continue running in *PROPER FORM*.
                    //
                }

                CMTRACE(MTRACE_EVT_WOKEAFTERDEACTIVATE, this, m_pVirtualProcessor, NULL);
#if defined(_DEBUG)
                if (fRMAwaken)
                {
                    SetDebugBits(CTX_DEBUGBIT_ACTIVATEDAFTERRMAWAKEN);
                }
#endif // _DEBUG

                CONCRT_COREASSERT( !m_fCanceled || m_pScheduler->InFinalizationSweep() || m_pScheduler->HasCompletedShutdown());

                // We were woken up for one of the 3 reasons above. It is important to tell the scheduler we are not idle before
                // proceeding. If we were woken up due to the addition of work we need to ensure that the work is visible to the scheduler
                // until after we've reported that we are !IDLE. The scheduler makes one pass looking for work and blocked contexts
                // once all virtual processors have reported that they are idle, and if it doesn't find any it will finalize the scheduler.
                // Therefore, we must register as ACTIVE, via VirtualProcessorIdle(false), *before* removing any work from the scheduler queues.
                // In addition, the call allows us to synchronize with finalization. If we were woken up for any reason, and finalization is in
                // progress at the same time, we need to ensure the context does not continue to execute its dispatch loop while the scheduler
                // is finalizing.
                m_pScheduler->VirtualProcessorIdle(false);

                // It is possible that the previous call to VirtualProcessorIdle(false) was suspended on the shutdown gate if
                // the scheduler is in the middle of shutdown, in which case the context could be canceled.
                // It is ok for the context to canceled right after its virtual processor was marked for retirement. We simply exit
                // the dispatch loop, and the virtual processor root in question is destroyed when the scheduler invokes
                // ISchedulerProxy::Shutdown, instead of IVirtualProcessorRoot::Remove, which is the normal path for
                // retired virtual processors.
                CONCRT_COREASSERT(!m_fCanceled || m_pScheduler->InFinalizationSweep() || m_pScheduler->HasCompletedShutdown());

                CONCRT_COREASSERT(!m_pVirtualProcessor->IsAvailable());
                CONCRT_COREASSERT(m_pVirtualProcessor->GetExecutingContext() == this);

                m_fIsVisibleVirtualProcessor = false;

                if (m_searchCount > 0)
                    m_pScheduler->NotifySearching(m_pVirtualProcessor->GetMaskId(), false);

                m_searchCount = 0;
            }
        }
    }

    /// <summary>
    ///     This function is called to execute the associated chore if one is available. The chore can be a stolen unrealized
    ///     chore or realized chore.
    /// </summary>
    /// <returns>
    ///     Returns true if an associated chore was executed, false otherwise.
    /// </returns>
    bool InternalContextBase::ExecutedAssociatedChore()
    {
        if (m_pAssociatedChore != NULL)
        {

#if defined(_DEBUG)
            m_workStartTimeStamp = _ReadTimeStampCounter();
            m_prepareCount = 0;
#endif // _DEBUG

            ExitCriticalRegion();
            if (m_fAssociatedChoreStolen)
            {

#if (_CHORETRACK && _UMSTRACE)
                OMTRACE(MTRACE_EVT_EXECUTEUNREALIZED, this, this, m_pVirtualProcessor, static_cast<_UnrealizedChore *>(m_pAssociatedChore));
                OMTRACE(MTRACE_EVT_EXECUTEUNREALIZED, m_pScheduler, this, m_pVirtualProcessor, static_cast<_UnrealizedChore *>(m_pAssociatedChore));
#endif // _CHORETRACK && _UMSTRACE

                static_cast<_UnrealizedChore*> (m_pAssociatedChore)->_Invoke();
                m_pAssociatedChore = NULL;
            }
            else
            {
                RealizedChore * pRealizedChore = static_cast<RealizedChore*> (m_pAssociatedChore);
                pRealizedChore->Invoke();
                // Set the associated chore to NULL before releasing it (which may cause it to be deleted). The associated chore is used by the parallel
                // debugger to tell which task a ConcRT thread is executing, and if the chore is freed to the heap, the user may see invalid data.
                m_pAssociatedChore = NULL;
                m_pScheduler->ReleaseRealizedChore(pRealizedChore);
            }
            EnterCriticalRegion();
            ReleaseWorkQueue();

            return true;
        }
        return false;
    }

    /// <summary>
    ///     Performs the necessary cleanup for a canceled context in its dispatch routine.
    /// <summary>
    void InternalContextBase::CleanupDispatchedContextOnCancel()
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(m_fCanceled);

#if defined(_DEBUG)
        //
        // At this point, we're shutting down this vproc/thread and we do not want to perform lock/heap validations that are no longer
        // true.
        //
        SetShutdownValidations();
#endif // _DEBUG

        // This indicates that the vproc is going away. From now until the end of time, this vproc is in a hyper-critical region.
        // We are no longer responsible for scheduling anything, so this is "perfectly" safe -- we cannot deadlock
        // between ourselves and some arbitrary piece of code we are responsible for scheduling.
        EnterHyperCriticalRegion();

        // The cleanup call *must* occur before the context releases its reference count on the scheduler.
        // Part of cleanup involves releasing a reference on the context's schedule group, and in some cases
        // we may need to spin until is safe to do so, keeping both the group and the scheduler alive.
        Cleanup();

        // NOTE: This call to DecrementInternalContextCount may well be the call that deletes the scheduler. The *this* pointer
        // should not be touched after this point!
        m_pScheduler->DecrementInternalContextCount();
    }

    /// <summary>
    ///     Called in the dispatch loop to check if the virtual processor the context is running on is marked for retirement,
    ///     and retires the virtual processor if it is.
    /// <summary>
    /// <returns>
    ///     True if the virtual processor was retired, false otherwise.
    /// </returns>
    bool InternalContextBase::IsVirtualProcessorRetired()
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        //
        // It is not safe to retire a virtual processor that has been made visible. Once it has been made visible, it
        // may have been activated to do some work. We cannot retire in this case, or else we risk missing the work it
        // was activated to execute.
        //
        if (!m_fIsVisibleVirtualProcessor && m_pVirtualProcessor->IsMarkedForRetirement())
        {
            //
            // If we notified the scheduler that we were in SFW, make sure to notify it that we are no longer doing this.
            //
            if (m_searchCount > 0)
                m_pScheduler->NotifySearching(m_pVirtualProcessor->GetMaskId(), false);

            m_searchCount = 0;

            bool isCanceled = SwitchOut(GoingIdle);

            // This is one of the two places we can find the context canceled due to scheduler shutdown. (the other please is
            // on returning from WaitForWork). Perform the necessary required for a canceled context that is in its dispatch loop.
            if (isCanceled)
            {
                CleanupDispatchedContextOnCancel();
            }
            return true;
        }
        return false;
    }

    /// <summary>
    ///     Searches for work using the search algorithm specified by the scheduler's policy. Also prepares the context to execute
    ///     work by reclaiming the virtual processor if necessary.
    /// </summary>
    /// <param name=pWork>
    ///     A pointer to a work item which is filled in if work was found.
    /// </param>
    /// <returns>
    ///     True if work was found, false otherwise.
    /// </returns>
    bool InternalContextBase::WorkWasFound(WorkItem * pWork)
    {
        //
        // If the virtual processor is visible, this is a last pass SFW before we go to sleep.  Inform the search algorithm of this so it does not skip
        // certain kinds of work (e.g.: affine to other).  Doing so in certain cases may lead to deadlock if we race just right and go to sleep violating
        // the client's expectation of maintained concurrency level.
        //
        if (m_pVirtualProcessor->SearchForWork(pWork, m_pSegment, m_fIsVisibleVirtualProcessor))
        {
            ReclaimVirtualProcessor();

            //
            // We found work - reset the search counter and make sure to notify the scheduler that we are no longer searching for work if if had
            // been previously notified that we were.
            //
            if (m_searchCount != 0)
                m_pScheduler->NotifySearching(m_pVirtualProcessor->GetMaskId(), false);

            m_searchCount = 0;
            return true;
        }
        return false;
    }

    /// <summary>
    ///     Switches to the runnable context represented by the work item.
    /// </summary>
    /// <param name=pWork>
    ///     A pointer to a work item to be executed.
    /// </param>
    void InternalContextBase::SwitchToRunnableContext(WorkItem * pWork)
    {
        ASSERT(pWork->IsContext());

        InternalContextBase *pContext = pWork->GetContext();
#if defined(_DEBUG)
        //
        // We need to perform extra validation here in the UMS case.  If we've just picked up a context which is UMS blocked,
        // we cannot block on any arbitrary lock -- doing so can leave us in a deadlock situation.  This facilitates an assertion
        // to catch this instead of relying on random stress hits.
        //
        if (pContext->GetDebugBits() & CTX_DEBUGBIT_UMSBLOCKED)
        {
            pContext->SetDebugBits(CTX_DEBUGBIT_HOLDINGUMSBLOCKEDCONTEXT);
        }
#endif // _DEBUG
        CMTRACE(MTRACE_EVT_SFW_FOUND, this, m_pVirtualProcessor, pContext);
        CMTRACE(MTRACE_EVT_SFW_FOUNDBY, pContext, m_pVirtualProcessor, this);

        SwitchTo(pContext, GoingIdle);
        //
        // Ensure we do not touch anything refering to the *this* pointer.  Return early out of the dispatch loop and let
        // the RM do its thing.  At this point, there may be another thread inside this loop!
        //
    }

    /// <summary>
    ///     Executes the chore (realized or unrealized) specified by the work item.
    /// </summary>
    /// <param name=pWork>
    ///     A pointer to a work item that represents a realized or unrealized chore.
    /// </param>
    void InternalContextBase::ExecuteChoreInline(WorkItem * pWork)
    {
        ASSERT(!pWork->IsContext());

#if defined(_DEBUG)
        m_workStartTimeStamp = _ReadTimeStampCounter();
        m_prepareCount = 0;
#endif // _DEBUG

        //
        // Adjust the current group and perform any reference transfers necessary to inline the chore on this context.
        //
        pWork->TransferReferences(this);
        //
        // No client invocation can happen inside a critical region.
        //
        IncrementDequeuedTaskCounter();
        ExitCriticalRegion();
        CONCRT_COREASSERT(GetCriticalRegionType() == OutsideCriticalRegion);
        pWork->Invoke();
        EnterCriticalRegion();
        ReleaseWorkQueue();
    }

    /// <summary>
    ///     The method that is called when a thread proxy starts executing a particular context.  The thread proxy which executes
    ///     the context is passed into this method and must be saved and returned on a call to the get_Proxy method.
    /// </summary>
    /// <param name="pDispatchState">
    ///     The state under which this IExecutionContext is being dispatched.
    /// </param>
    void InternalContextBase::Dispatch(DispatchState * pDispatchState)
    {
        (pDispatchState);
        bool fWinRTInitialized = false;

        m_threadId = GetCurrentThreadId();

#if defined(_DEBUG)
        m_lastDispatchedTid = m_threadId;
#endif // _DEBUG

        //
        // This dispatch context is live, set TLS on the current thread proxy.  This must happen before any critical region is entered
        // on this context.
        //
        SetAsCurrentTls();

        if (m_pScheduler->GetPolicy().GetPolicyValue(WinRTInitialization) == ::Concurrency::InitializeWinRTAsMTA &&
            ::Concurrency::GetOSVersion() == ::Concurrency::IResourceManager::Win8OrLater)
        {
            fWinRTInitialized = true;
            WinRT::RoInitialize(RO_INIT_MULTITHREADED);
        }

        EnterCriticalRegion();

        CONCRT_COREASSERT(m_pThreadProxy != NULL);
        CONCRT_COREASSERT(!IsBlocked());
        CONCRT_COREASSERT(!m_fIsVisibleVirtualProcessor);
        CONCRT_COREASSERT(!m_fCanceled);

        TRACE(TRACE_SCHEDULER, L"InternalContextBase::Dispatch: Start dispatch loop");

        m_searchCount = 0;
        bool fDoneSearchingForWork = false;

        //
        // First try to execute an associated chore if there is one available.
        //
        if (ExecutedAssociatedChore())
        {
            // Check for virtual processor retirement since we've just finished executing a root chore.
            fDoneSearchingForWork = IsVirtualProcessorRetired();
        }

        while (!fDoneSearchingForWork)
        {
            WorkItem work;
            //
            // Indicate that no work is skipped at the start of the search loop.
            //
            m_fWorkSkipped = false;
            //
            // If the virtual processor is null, this could be an external context from a nested scheduler that neglected to
            // invoke Detach.
            //
            if (m_pVirtualProcessor == NULL)
            {
                CONCRT_COREASSERT((SchedulerBase::FastCurrentContext() != this) && SchedulerBase::FastCurrentContext()->IsExternal());
                CONCRT_COREASSERT(IsInsideCriticalRegion());

                ExitCriticalRegion();
                throw nested_scheduler_missing_detach();
            }

            //
            // This virtual processor has reached a safe point.  We are guaranteed to have made observations of data structures
            // in the scheduler and have no information cached anywhere.  Inform the virtual processor and if there is a new
            // data revision that needs committed, do so.  Any commits should happen outside a critical region so that the
            // possible heap frees there do not trigger badly performing UMS behavior.
            //
            // If this virtual processor is visible, we're in-between marking ourselves available and going to sleep.  Exiting
            // the critical region to perform a commit is illegal in this region.
            //
            if (!m_fIsVisibleVirtualProcessor && m_pVirtualProcessor->SafePoint())
            {
                ExitCriticalRegion();
                m_pScheduler->CommitSafePoints();
                EnterCriticalRegion();
            }

            // Search for work among the queues in the scheduler.
            if (WorkWasFound(&work))
            {
                if (work.IsContext())
                {
                    SwitchToRunnableContext(&work);
                    // This is now an idle context and should return from the dispatch loop immediately.
                    fDoneSearchingForWork = true;
                }
                else
                {

#if (_CHORETRACK && _UMSTRACE)
                    if (work.GetType() == WorkItem::WorkItemTypeUnrealizedChore)
                    {
                        OMTRACE(MTRACE_EVT_EXECUTEUNREALIZED, this, this, m_pVirtualProcessor, work.GetUnrealizedChore());
                        OMTRACE(MTRACE_EVT_EXECUTEUNREALIZED, m_pScheduler, this, m_pVirtualProcessor, work.GetUnrealizedChore());
                    }
#endif // _CHORETRACK && _UMSTRACE

                    ExecuteChoreInline(&work);

                    // Check for virtual processor retirement since we've just finished executing a root chore.
                    fDoneSearchingForWork = IsVirtualProcessorRetired();
                }
            }
            else if (IsVirtualProcessorRetired())
            {
                // Check for virtual processor retirement since we've made a full search through the scheduler without
                // finding any work.
                fDoneSearchingForWork = true;
            }
            else
            {
                WaitForWork();

                // This is one of the two places we can find the context canceled due to scheduler shutdown. (the other is right after
                // retiring a virtual processor). Perform the necessary required for a canceled context that is in its dispatch loop.
                if (m_fCanceled)
                {
                    CleanupDispatchedContextOnCancel();
                    fDoneSearchingForWork = true;
                }
            }
        } // end of while (!fDoneSearchingForWork)

        if (fWinRTInitialized)
        {
            WinRT::RoUninitialize();
        }

        // Clear the TLS as soon as possible for the debugger
        ClearContextTls();
    }

    /// <summary>
    ///     If internal context does not own this virtual processor then claim it back. This might require
    ///     waiting until it becomes available.
    /// </summary>
    void InternalContextBase::ReclaimVirtualProcessor()
    {
        // If we were in the process of releasing (relinquishing) this virtual processor and we found work
        // in the last search pass, then we have two options:
        //
        //    1) Virtual processor is still marked as available so we can reclaim it safely
        //    2) Someone is in the process of adding work (calling StartupIdleVirtualProcessor) in which case
        //       we make sure that they see our context and we simply block until they signal.
        if (m_fIsVisibleVirtualProcessor)
        {
            VirtualProcessor::ClaimTicket ticket;
            if ( !m_pVirtualProcessor->ClaimExclusiveOwnership(ticket))
            {
                // Someone has claimed this virtual processor exclusively for the purpose of activating it. We need to
                // deactivate to consume the activation.
                CONCRT_COREASSERT(m_pVirtualProcessor->GetExecutingContext() == this);
                while (!m_pVirtualProcessor->Deactivate(this))
                {
                    RMAwaken();
                }

                CMTRACE(MTRACE_EVT_WOKEAFTERDEACTIVATE, this, m_pVirtualProcessor, NULL);
            }

            CONCRT_COREASSERT(!m_pVirtualProcessor->IsAvailable());
            m_fIsVisibleVirtualProcessor = false;
        }
        else
        {
            // If this context was not releasing its virtual processor, it should still have it.
            CONCRT_COREASSERT(!m_pVirtualProcessor->IsAvailable());
            CONCRT_COREASSERT(m_pVirtualProcessor->GetExecutingContext() == this);
        }
    }

    /// <summary>
    ///     Performs cleanup of the internal context
    /// </summary>
    void InternalContextBase::Cleanup()
    {
        ContextBase::Cleanup();

        // Set the schedule group to null ensuring that no foreign threads/contexts are relying on it being alive.
        SwapScheduleGroupSegment(NULL);
    }

    /// <summary>
    ///     Cancels the context, causing it to exit the dispatch loop if it is running on a virtual processor.
    /// </summary>
    void InternalContextBase::Cancel()
    {
        ASSERT( !m_fCanceled);
        ASSERT(m_pScheduler->InFinalizationSweep());

        // This API must synchronize with scheduler finalization. The scheduler is kicked into finalization when no
        // external references exist AND when all active vprocs are idle. Therefore the triggers for finalization are:
        //    1] A thread decrements the last external reference count on the scheduler while idle and active counts
        //       are equal.
        //    2] An internal context in the dispatch loop calls VirtualProcessorIdle(true) -> it raises the idle count
        //       and makes it equal to the active count. This context has a valid virtual processor.
        //    3] An internal context in the dispatch loop calls SwitchOut since its virtual processor was marked for
        //       retirement, which in turn calls VirtualProcessorActive(false) -> it lowers the active count and makes
        //       it equal to the idle count. This context does NOT have a valid virtual processor since it has just
        //       retired it.

        // For each context in the list of 'all contexts' in the scheduler, we detect it if is executing on a virtual
        // processor by checking if it has a non-null m_pVirtualProcessor. The current thread may very well be one of
        // these contexts in the process of executing VirtualProcessorIdle(true), as described in 2] above.. We mark
        // these contexts as canceled, and wake them up by attempting to activate their virtual processor. If their virtual
        // processor was already activated, due to a race with StartupIdleVirtualProcessor (addition of work into the scheduler),
        // or a race with RemoveVirtualProcessors (core migration), they will be suspended on the gate and woken up before
        // PhaseTwoShutdown completes. When the contexts wake up, they will exit the dispatch loop, and since they are canceled,
        // they will release their internal reference on the scheduler.

        // If the context does NOT have a valid virtual processor, we *must* cleanup and release the internal reference on
        // the scheduler on its behalf. We need to be careful here, and ensure that the context has left its dispatch loop
        // else we're in danger of having all references on the scheduler released and the scheduler being deleted while
        // a context is in the process of leaving its dispatch loop. We DO NOT mark these contexts as canceled EXCEPT if
        // the current thread is one such context executing VirtualProcessorActive(false) as described in 3] above. We can
        // detect that a context without a virtual processor has left the dispatch loop by checking the m_fInDispatch member
        // variable.

        if (m_pVirtualProcessor != NULL)
        {
            // Mark the context as canceled, so it will break out of its dispatch loop when it is resumed.
            m_fCanceled = true;

            ASSERT(m_pVirtualProcessor->GetExecutingContext() == this);

            // We must synchronize with a potential external activation here. Virtual processors that were previously running
            // and got deactivated could've been activated due to a race during adding the last work item, during the time at
            // which the scheduler is shutting down. The context activating the virtual processor employs different meant of
            // synchronization to ensure that that the actual deletion of scheduler data structures is delayed until it is done
            // with trying to startup a virtual processor (an example of this is in ScheduleGroup::AddRunnableContext). However,
            // the virtual processor root must only be activated once. For that reason, we must reclaim try to reclaim the virtual
            // processor and if we fail, we can rely on the caller who beat us to it to activate the virtual processor root.

            // Claiming exclusive ownership also synchronizes with virtual processor retirement, as a result of core migration.

            // Since the suspend bit is set, the context is not allowed to get to a point where it can reset this flag.
            ASSERT(m_fIsVisibleVirtualProcessor);

            VirtualProcessor::ClaimTicket ticket;
            if (m_pVirtualProcessor->ClaimExclusiveOwnership(ticket))
            {
                //
                // We've succeeded in gaining ownership of this virtual processor, now we should activate it, since it has either
                // already executed, or is about to execute, a call to Deactivate.
                //
                ticket.Exercise();
            }
            else
            {
                ASSERT(m_pVirtualProcessor->GetExecutingContext() == this);
                //
                // Either someone added work and activated this virtual processor, after it executed VirtualProcessorIdle(true)
                // or the virtual processor was activated after it was marked for retirement due to core migration.
                //
                // We do nothing here.
            }
        }
        else
        {
            // DO NOT mark the context as canceled here unless it is the current context.(see case 3 above). With contexts that
            // are not associated with virtual processors, we're not  certain if they're left the dispatch loop for sure - we want
            // to make sure they don't execute the cleanup code below, or we may have over-dereference errors.
            if (SchedulerBase::FastCurrentContext() == this)
            {
                // If this is the current context executing PhaseTwoShutdown, it is in a VirtualProcessorActive(false) call,
                // inside SwitchOut in its dispatch loop, we need this context to cleanup after itself (it cannot exit its
                // dispatch routine until we are done with PhaseTwoShutdown), so we set its canceled flag to true here.
                m_fCanceled = true;
                return;
            }

            // It is a possible that a context without a virtual processor is in SwitchOut but has not finished executing
            // the function. Wait until its blocked flag is set, so we don't end up deleting the scheduler/context while
            // the thread is still accessing them in its dispatch loop.
            SpinUntilBlocked();

            // The cleanup call *must* occur before the context releases its reference count on the scheduler.
            // Part of cleanup involves releasing a reference on the context's schedule group, and in some cases
            // we may need to spin until is safe to do so, keeping both the group and the scheduler alive.
            Cleanup();
            m_pScheduler->DecrementInternalContextCount();
        }
    }

    /// <summary>
    ///     Destroys the base class object for an internal context.
    /// </summary>
    InternalContextBase::~InternalContextBase()
    {
        Cleanup();
    }

    /// <summary>
    ///     Prepare a context for execution by associating a scheduler group/chore with it. Scheduler
    //      shall call this routine before executing an internal context
    /// </summary>
    void InternalContextBase::PrepareForUse(ScheduleGroupSegmentBase* pSegment, _Chore *pChore, bool choreStolen)
    {
        ASSERT(m_pSegment == NULL);
        ASSERT(m_pAssociatedChore == NULL);
        ASSERT(m_pWorkQueue == NULL);
        ASSERT(m_pParentContext == NULL);

        // The context is no longer considered idle
        ASSERT(m_fIdle);
        m_fIdle = false;

        // Associate with a schedule group
        m_pSegment = pSegment;
        if (pChore == NULL)
        {
            // Reference the group since the context is now working on it.
            pSegment->GetGroup()->InternalReference();
        }
        else
        {
            // Realized chores already have a reference to their schedule group. This reference is transferred to
            // the new context. If the chore is a stolen chore, the schedule group must be referenced.
            if (choreStolen)
            {
                pSegment->GetGroup()->InternalReference();
                m_fAssociatedChoreStolen = true;
            }
            else
            {
                m_fAssociatedChoreStolen = false;
            }
            m_pAssociatedChore = pChore;
        }
    }

    /// <summary>
    ///     Remove a context from execution by dis-associating it from any scheduler group/chore.
    /// </summary>
    void InternalContextBase::RemoveFromUse()
    {
        //
        // Due to the way in which recycling and reusing contexts happen, contexts on the free list must be inside critical
        // regions until they are redispatched.
        //
        CONCRT_COREASSERT(IsInsideCriticalRegion());
        CMTRACE(MTRACE_EVT_CONTEXT_RELEASED, this, NULL, m_pVirtualProcessor);

        //
        // Send this context back to the free list. If the free list is full, the context will be canceled,
        // which will cause it to break out of the dispatch loop, cleanup and dereference the scheduler.
        // If not, the context may be reused by the scheduler to schedule agents/steal chores.
        //
        ASSERT(m_pSegment != NULL && ScheduleGroupRefCount()> 0);
        ASSERT(m_pAssociatedChore == NULL);

        ReleaseWorkQueue();

        ASSERT(m_pParentContext == NULL);

        // Set the schedule group to null ensuring that no foreign threads/contexts are relying on it being alive.
        SwapScheduleGroupSegment(NULL);

        // Mark the context as idle.
        ASSERT(!m_fIdle);
        m_fIdle = true;

        // For visualization purposes, the context is detached.
        m_threadId = 0;
    }

    /// <summary>
    ///     Returns a scheduler unique identifier for the context.
    /// </summary>
    /// <returns>
    ///     The context Id.
    /// </returns>
    unsigned int InternalContextBase::GetId() const
    {
        return m_id;
    }

    /// <summary>
    ///     Returns the scheduler to which this context belongs.
    /// </summary>
    /// <returns>
    ///     The owning scheduler.
    /// </returns>
    IScheduler * InternalContextBase::GetScheduler()
    {
        return m_pScheduler->GetIScheduler();
    }

    /// <summary>
    ///     Returns the thread proxy which is executing this context.  Until the Dispatch method has been called on the given
    ///     context, this will return NULL.  Once the Dispatch method has been called, this returns the IThreadProxy which
    ///     was passed into the Dispatch method.
    /// </summary>
    /// <returns>
    ///     The thread proxy which dispatched this particular context, otherwise NULL.
    /// </returns>
    IThreadProxy * InternalContextBase::GetProxy()
    {
        return m_pThreadProxy;
    }

#if _DEBUG
    // _DEBUG helper
    DWORD InternalContextBase::GetThreadId() const
    {
        if (m_pScheduler->IsUMSScheduler())
            return static_cast<DWORD>(-1);
        else
            return ((const ThreadProxy*) m_pThreadProxy)->GetThreadId();
    }
#endif

    /// <summary>
    ///     Sets the thread proxy which is executing this context.  The caller must save this and return it upon a call to the GetProxy method.
    ///     Note that the resource manager guarantees stability of the thread proxy while inside the Dispatch method.
    /// </summary>
    /// <param name="pThreadProxy">
    ///     The thread proxy which dispatched this particular context.
    /// </param>
    /// <returns>
    ///     An indication of success.
    /// </returns>
    void InternalContextBase::SetProxy(IThreadProxy *pThreadProxy)
    {
        if (pThreadProxy == NULL)
        {
            throw std::invalid_argument("pThreadProxy");
        }

        m_pThreadProxy = pThreadProxy;
#if defined(_DEBUG)
        m_pLastAssignedThreadProxy = m_pAssignedThreadProxy;
        m_pAssignedThreadProxy = m_pThreadProxy;
#endif // _DEBUG
    }

    /// <summary>
    ///     Allocates a block of memory of the size specified.
    /// </summary>
    /// <param name="numBytes">
    ///     Number of bytes to allocate.
    /// </param>
    /// <returns>
    ///     A pointer to newly allocated memory.
    /// </returns>
    void* InternalContextBase::Alloc(size_t numBytes)
    {
        void* pAllocation = NULL;
        ASSERT(SchedulerBase::FastCurrentContext() == this);

        // The alloc can throw an exception, we need to make sure we exit the critical region on this context before
        // leaving the function.

        {
            ContextBase::ScopedCriticalRegion cs(this);
            SubAllocator * pAllocator = m_pVirtualProcessor->GetCurrentSubAllocator();
            ASSERT(pAllocator != NULL);

            pAllocation = pAllocator->Alloc(numBytes);
        }

        return pAllocation;
    }

    /// <summary>
    ///     Frees a block of memory previously allocated by the Alloc API.
    /// </summary>
    /// <param name="pAllocation">
    ///     A pointer to an allocation previously allocated by Alloc.
    /// </param>
    void InternalContextBase::Free(void* pAllocation)
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(pAllocation != NULL);

        EnterCriticalRegion();

        SubAllocator * pAllocator = m_pVirtualProcessor->GetCurrentSubAllocator();
        ASSERT(pAllocator != NULL);

        pAllocator->Free(pAllocation);

        ExitCriticalRegion();
    }

    /// <summary>
    ///     Increments the count of work coming in.
    /// </summary>
    void InternalContextBase::IncrementEnqueuedTaskCounterHelper()
    {
        EnterCriticalRegion();

        ASSERT(m_pVirtualProcessor != NULL);
        ASSERT(SchedulerBase::FastCurrentContext() == this);

        m_pVirtualProcessor->m_enqueuedTaskCounter++;

        ExitCriticalRegion();
    }

    /// <summary>
    ///     Increments the count of work being done.
    /// </summary>
    void InternalContextBase::IncrementDequeuedTaskCounterHelper(unsigned int count)
    {
        EnterCriticalRegion();

        ASSERT(m_pVirtualProcessor != NULL);
        ASSERT(SchedulerBase::FastCurrentContext() == this);

        m_pVirtualProcessor->m_dequeuedTaskCounter += count;

        ExitCriticalRegion();
    }

} // namespace details
} // namespace Concurrency
