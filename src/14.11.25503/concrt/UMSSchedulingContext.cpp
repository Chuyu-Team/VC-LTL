// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSSchedulingContext.cpp
//
// Implementation of the UMS scheduling context.  This is a special context whose sole purpose is to determine
// which context to run next at initial startup of a virtual processor and whenever a UMS thread running on the virtual
// processor blocks.  The RM will invoke this scheduling context whenever a return to primary happens.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Creates a new UMS scheduling context that is bound to a particular virtual processor.  Once the context
    ///     is created, it is always bound to the virtual processor used at creation time.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler to which this virtual processor belongs.
    /// </param>
    /// <param name="pBoundVProc">
    ///     The virtual processor to which this context is bound.
    /// </param>
    UMSSchedulingContext::UMSSchedulingContext(UMSThreadScheduler *pScheduler, UMSThreadVirtualProcessor *pBoundVProc) :
        m_pScheduler(pScheduler),
        m_pBoundVProc(pBoundVProc),
        m_pThreadProxy(NULL)
    {
        m_id = m_pScheduler->GetNewContextId();
    }

    /// <summary>
    ///     Returns a scheduler unique identifier for the context.
    /// </summary>
    /// <returns>
    ///     The context Id.
    /// </returns>
    unsigned int UMSSchedulingContext::GetId() const
    {
        return m_id;
    }

    /// <summary>
    ///     Returns the scheduler to which this context belongs.
    /// </summary>
    /// <returns>
    ///     The owning scheduler.
    /// </returns>
    IScheduler * UMSSchedulingContext::GetScheduler()
    {
        return m_pScheduler->GetIScheduler();
    }

    /// <summary>
    ///     Returns the thread proxy which is executing this context.  Until the SetProxy method has been called on the given
    ///     context, this will return NULL.  Once the SetProxy method has been called, this returns the IThreadProxy which
    ///     was passed into the SetProxy method.
    /// </summary>
    /// <returns>
    ///     The thread proxy which dispatched this particular context.
    /// </returns>
    IThreadProxy * UMSSchedulingContext::GetProxy()
    {
        return m_pThreadProxy;
    }

    /// <summary>
    ///     Sets the thread proxy which is executing this context.  The caller must save this and return it upon a call to the GetProxy method.
    ///     Note that the resource manager guarantees stability of the thread proxy while inside the Dispatch method.
    /// </summary>
    /// <param name="pThreadProxy">
    ///     The thread proxy which dispatched this particular context.
    /// </param>
    void UMSSchedulingContext::SetProxy(IThreadProxy *pThreadProxy)
    {
        if (pThreadProxy == NULL)
            throw std::invalid_argument("pThreadProxy");

        m_pThreadProxy = pThreadProxy;
    }

    /// <summary>
    ///     Sets all blocked status on a given context.
    /// </summary>
    /// <param name="pPreviousContext">
    ///     The previously running context.
    /// </param>
    /// <param name="fAsynchronous">
    ///     Is previously running context asynchronously blocked.
    /// </param>
    void UMSSchedulingContext::SetUMSBlocked(UMSThreadInternalContext *pPreviousContext, bool fAsynchronous)
    {
#if defined(_DEBUG)
        //
        // If this assertion fires, someone has called a blocking API between a ReleaseInternalContext and the time we switch off it.  Doing this
        // will corrupt state within the scheduler.
        //
        CONCRT_COREASSERT((pPreviousContext->GetDebugBits() & CTX_DEBUGBIT_RELEASED) == 0);
        pPreviousContext->ClearDebugBits(CTX_DEBUGBIT_AFFINITIZED);
        pPreviousContext->SetDebugBits(CTX_DEBUGBIT_UMSBLOCKED);
#endif // _DEBUG

        CONCRT_COREASSERT(pPreviousContext->m_pThreadProxy != NULL);

        pPreviousContext->NotifyBlocked(fAsynchronous);

        //
        // After this point, it might be running atop another vproc.  Remember that it may have come back on the completion list and been affinitized
        // prior to even getting into this code!
        //
    }

    /// <summary>
    ///     Claims the bound virtual processor and swallows an activation.
    /// </summary>
    void UMSSchedulingContext::ClaimBoundProcessorAndSwallowActivation()
    {
        VirtualProcessor::ClaimTicket ticket;
        if (!m_pBoundVProc->ClaimExclusiveOwnership(ticket))
        {
            //
            // Someone else activated us.  Swallow the event set.  The event isn't set until true is returned -- hence the loop.
            //
            while (!m_pBoundVProc->Deactivate(this));
        }
    }

    /// <summary>
    ///     The method that is called when a thread proxy starts executing a particular context.  The thread proxy which executes
    ///     the context is set in SetProxy before entering the dispatch loop and must be saved and returned on a call to the GetProxy method.
    /// </summary>
    /// <param name="pDispatchState">
    ///     The state under which this IExecutionContext is being dispatched.
    /// </param>
    void UMSSchedulingContext::Dispatch(DispatchState * pDispatchState)
    {
        const int PASS_COUNT_BEFORE_SLEEP_NORMAL = 1;
        const int PASS_COUNT_BEFORE_SLEEP_ORIGINALLY_ACTIVATED = 5;

        CONCRT_COREASSERT(m_pThreadProxy != NULL);
        SetAsCurrentTls();

#if defined(_DEBUG)
        DWORD fromBits = 0;
#endif // _DEBUG

        for(;;)
        {
            int pass = 0;
            UMSThreadInternalContext *pPreviousContext = static_cast<UMSThreadInternalContext *> (m_pBoundVProc->GetExecutingContext());
            ScheduleGroupSegmentBase *pSegment = (pPreviousContext == NULL ? m_pBoundVProc->m_pStartingSegment : pPreviousContext->m_pSegment);

            // **************************************************
            // READ THIS:
            //
            // Yet another incredibly subtle point about where we get suspended..  There are times in the scheduling context's
            // dispatch loop where we can't find work (the critical context is blocked, etc...) and we want to run through a
            // Deactivate pass in order to put the vproc to sleep much as we do with an ordinary search for work in the dispatch loop.  The unfortunate thing
            // is that there's another context which thinks it this is its exclusive purview.  We aren't going to try to maintain a complex state machine to
            // be able to restore his expected state, so we spin if that's the case.
            //
            // Ordinarily, you might think that we can simply check m_pBoundVProc->IsAvailable, however, there might be a race on that such as what follows:
            //
            // - Context 1 on vproc A makes the vproc available and then blocks
            // - Context 2 on vproc B claims exclusive ownership of the virtual processor (it suspends, takes a while, take your pick)
            // - We get in here and see the virtual processor as not available so we think we're safe to make it available
            // - We make the context available
            // - Context 3 on vproc C claims exclusive ownership of the virtual processor (now 2 contexts think they have exclusive ownership)
            //
            // There are other potential races as well.  What we really need to know is if there IS a context in the dispatch loop that has made the virtual
            // processor available.  It doesn't necessarily need to be pPreviousContext because the original context might have critically blocked in that region
            // and we might be running someone else.  Hence the rule -- you **MUST** stay in a critical region between the call to MakeAvailable and the call to Deactivate
            // without exception.  No other MakeAvailable is permitted.  Once we know what the critical context is, we can check it to see if IT thinks IT has flagged
            // the virtual processor.  That check must come **BEFORE** the call to MakeAvailable and must be fenced by the time m_fAvailable is set to true.
            // **************************************************

            bool fOriginallyAvailable = false;
            bool fMadeAvailable = false;

            int passes = fOriginallyAvailable ? PASS_COUNT_BEFORE_SLEEP_ORIGINALLY_ACTIVATED : PASS_COUNT_BEFORE_SLEEP_NORMAL;

            UMSThreadInternalContext::BlockingType blockingType = UMSThreadInternalContext::BlockingNormal;
            CriticalRegionType type = OutsideCriticalRegion;

            //
            // If someone explicitly switched back to the primary, don't do the UMS blocked bit.  Instead, just conduct the search from
            // the primary for runnables or invoke the reserved context as appropriate. This is accomplished by the fact that affinitize would clear
            // the executing proxy.
            //
            if (pPreviousContext != NULL)
            {
                VCMTRACE(MTRACE_EVT_UMSBLOCKED, pPreviousContext, m_pBoundVProc, NULL);

                CONCRT_COREASSERT(pPreviousContext->UNSAFE_CurrentVirtualProcessor() == m_pBoundVProc);
                CONCRT_COREASSERT(!pPreviousContext->IsBlocked());
                CONCRT_COREASSERT(pPreviousContext->m_pThreadProxy != NULL);
#if defined(_DEBUG)
                //
                // If the context UMS blocks while it's holding a UMS blocked context prior to the switch, we can deadlock in a variety of ways.
                // Assert this instead of relying on stress to ferret this out.
                //
                CONCRT_COREASSERT((pPreviousContext->GetDebugBits() & CTX_DEBUGBIT_HOLDINGUMSBLOCKEDCONTEXT) == 0);
#endif // _DEBUG
                type = pPreviousContext->GetCriticalRegionType();

            }

            CONCRT_COREASSERT(type != InsideHyperCriticalRegion);

            if (m_pBoundVProc->m_pCriticalContext != NULL)
            {
                //
                // Only 1 context can be inside the critical region at a time
                //
                CONCRT_COREASSERT(pPreviousContext->GetCriticalRegionType() == OutsideCriticalRegion);
            }
            else if (type != OutsideCriticalRegion)
            {
                //
                // A thread/context inside a critical region blocked
                //
                CONCRT_COREASSERT(m_pBoundVProc->m_pCriticalContext == NULL);
                VCMTRACE(MTRACE_EVT_CRITICALBLOCK, pPreviousContext, m_pBoundVProc, NULL);
                m_pBoundVProc->m_pCriticalContext = pPreviousContext;
                blockingType = UMSThreadInternalContext::BlockingCritical;
            }

            bool fCritical = (m_pBoundVProc->m_pCriticalContext != NULL);

            //
            // Any context which made a virtual processor available darn well better be in a critical region until they claim it again.
            //
            UMSThreadInternalContext *pCriticalContext = m_pBoundVProc->m_pCriticalContext;
            CONCRT_COREASSERT(!fOriginallyAvailable || pCriticalContext != NULL);

            if (pCriticalContext != NULL && pCriticalContext->m_fIsVisibleVirtualProcessor)
            {
                fOriginallyAvailable = true;
            }

            //
            // pSegment might be NULL because we've looped around,  because someone blocked during a context recycling
            // after we've already NULL'd the group out.  In any of these cases, we go to the anonymous schedule group to start the search.
            //
            if (pSegment == NULL)
            {
                pSegment = m_pBoundVProc->GetOwningRing()->GetAnonymousScheduleGroupSegment();
            }

            if (pPreviousContext != NULL)
            {
                pPreviousContext->SetBlockingType(blockingType);
            }

            //
            // The push context comes first (it is what the vproc was started with).  We do *NOT* push to idle vprocs -- only to inactive ones.
            //
            InternalContextBase *pContext = m_pBoundVProc->m_pPushContext;
            m_pBoundVProc->m_pPushContext = NULL;

            while (pContext == NULL)
            {
                if (m_pBoundVProc->m_pCriticalContext != NULL)
                {
                    //
                    // Sweep the completion list if we are waiting for a critical context.
                    // Otherwise the search for runnable would do the sweep.
                    //
                    m_pScheduler->MoveCompletionListToRunnables();

                    //
                    // The critical context is **ALWAYS** first priority -- no matter what!  Since we are the only thread that picks up critical contexts
                    // due to SFW happening in a critical region, there's no CAS.  We simply can clear the flag when appropriate.
                    //
                    if (m_pBoundVProc->m_fCriticalIsReady)
                    {
                        pContext = m_pBoundVProc->m_pCriticalContext;
                        m_pBoundVProc->m_fCriticalIsReady = FALSE;
                        m_pBoundVProc->m_pCriticalContext = NULL;

#if defined(_DEBUG)
                        fromBits = CTX_DEBUGBIT_PRIMARYAFFINITIZEFROMCRITICAL;
#endif // _DEBUG
                        CONCRT_COREASSERT(pContext != NULL);
                    }
                }
                else
                {
                    CONCRT_COREASSERT(!m_pBoundVProc->m_fCriticalIsReady);
                }

                //
                // Next priority is searching for contexts to run.
                //
                if (pContext == NULL)
                {
                    //
                    // We need to do a full search for runnables.  This means all scheduling rings, nodes, LRCs, etc...  The reason for this is subtle.  Normally,
                    // if we can't quickly find something to run, we switch to the reserved context which is a real search context and everyone is happy (we keep the virtual
                    // processor active).  The only time we'll put the virtual processor to sleep HERE is when there's a critical context blocked or there are no reserved
                    // contexts.
                    // You might think we're okay to do that because the wakings there explicitly notify us.  Unfortunately, those special contexts might be blocked
                    // on a lock held by an ARBITRARY context.  That ARBITRARY context might have been moved to a runnables list in a different scheduling ring/node by
                    // the MoveCompletionListToRunnables above.  Therefore, we must do a FULL search for runnables here across all rings.
                    //
                    WorkItem work;
                    if (m_pBoundVProc->SearchForWork(&work, pSegment, false, WorkItem::WorkItemTypeContext))
                    {
                        pContext = work.GetContext();
#if defined(_DEBUG)
                        CMTRACE(MTRACE_EVT_SFW_FOUNDBY, pContext, m_pBoundVProc, NULL);

                        fromBits = CTX_DEBUGBIT_PRIMARYAFFINITIZEFROMSEARCH;
#endif // _DEBUG
                    }

                }

                //
                // If we could not find anyone to run by this point, we're stuck having to create a new SFW context.  This should only happen
                // if we're **NOT** critically blocked.
                //
                if (!fCritical && pContext == NULL)
                {
                    pContext = m_pScheduler->GetReservedContext();
                    if (pContext == NULL)
                        m_pScheduler->DeferredGetInternalContext();
                    else
                        pContext->PrepareForUse(m_pScheduler->GetAnonymousScheduleGroupSegment(), NULL, false);

#if defined(_DEBUG)
                    fromBits = CTX_DEBUGBIT_PRIMARYRESERVEDCONTEXT;
#endif // _DEBUG
                }

                if (pPreviousContext != NULL)
                {
                    //
                    // After one time through the search loop from the source, let go of the previous context.  This means we can no longer originate
                    // a search from the source group.  We cannot place a reference here because removing it might entail a deletion from the ListArray
                    // which cannot happen on the primary.  Just search outward from the anonymous schedule group if we cannot find anything the first time
                    // through
                    //
                    if (pContext == NULL)
                    {
                        pSegment = m_pBoundVProc->GetOwningRing()->GetAnonymousScheduleGroupSegment();
                    }

                    SetUMSBlocked(pPreviousContext, pDispatchState->m_fIsPreviousContextAsynchronouslyBlocked);
                    pPreviousContext = NULL;
                }

                if (pContext == NULL)
                {
                    //
                    // Make a series of passes through the "special SFW" above and then put the virtual processor to sleep.
                    //
                    pass++;
                    if (pass == passes)
                    {
                        //
                        // Make the virtual processor available and perform a flush.  We need to make one more loop to "search for work"
                        // as it's entirely possible we raced with a wake notification on the critical context or reserved context list event.
                        //
                        // It's also entirely possible that a context in its last SFW loop after making the virtual processor available UMS triggered and got us
                        // back here.  In that case, we need to remember this because special handling is required.  Instead of having a horribly complex state
                        // machine to manage this particular race, we simply don't Deactivate here and instead, we poll.  Much safer.
                        //
                        if (!fOriginallyAvailable)
                        {
                            fMadeAvailable = true;
                            m_pBoundVProc->MakeAvailableFromSchedulingContext();
                        }

                        //
                        // Currently safe because this is simply a flush that doesn't restore any state or wait on any events.
                        //
                        m_pBoundVProc->EnsureAllTasksVisible(this);
                    }
                    else if (pass > passes)
                    {
                        //
                        // Because we're not running on a context, we cannot participate in finalization and yet we are putting this virtual processor
                        // to sleep.  In order to do that safely, we must have a guarantee that something will wake *US* up.  That basically means that
                        // we have a special context blocked -- either a critically blocked context or waiting on reserved context event.

                        //
                        // Put the virtual processor to sleep for real.  If we wake up for *ANY* reason (doesn't matter if it's the completion notification
                        // or not), loop back up and perform another SFW.
                        //
                        if (!fOriginallyAvailable)
                        {
                            if (!m_pBoundVProc->Deactivate(this))
                            {
                                //
                                // This indicates that something came back on the completion list.  We really do want to do a FULL SFW here.  We need to claim
                                // ownership of the VProc.
                                //
                                ClaimBoundProcessorAndSwallowActivation();
                            }

                            CONCRT_COREASSERT(!m_pBoundVProc->IsAvailable());

                            fMadeAvailable = false;
                        }
                        else
                        {
                            //
                            // In order to avoid horrible race conditions with the context which made this virtual processor available, we simply sleep, loop back
                            // up and check again.
                            //
                            // MINIMIZE blocking between MakeAvailable and Deactivate within the dispatch loop.  This path has a big performance penalty.
                            // Also -- NEVER  release the critical region between those paths (see above).
                            //
                            Sleep(100);
                        }

                        pass = 0;
                    }
                }
            }

            //
            // If we made the virtual processor available, we need to make it not so right now -- we're going to execute a context.
            //
            if (fMadeAvailable)
            {
                ClaimBoundProcessorAndSwallowActivation();
            }

            CONCRT_COREASSERT(!m_pBoundVProc->IsAvailable());

            m_pBoundVProc->Affinitize(pContext);
#if defined(_DEBUG)
            pContext->SetDebugBits(fromBits);
#endif // _DEBUG
            m_pThreadProxy->SwitchTo(pContext, Blocking);

            //
            // If we get here, it indicates that the SwitchTo failed as a result of the underlying thread blocking asynchronously (e.g.: it was suspended or
            // had a kernel APC running atop it when we tried to SwitchTo it).  In this case, just go back up and pick another runnable.  There's one absolutely
            // critical thing here.  We affinitized the vproc to pContext.  It isn't executing pContext and never was.  The execute failed because of a thread
            // suspension, kernel APC, etc...  After looping back, we *CANNOT* rely on vproc relative fields.  We simply pick another context on the basis of
            // information we already know and switch.
            //
            // On success, SwitchTo will snap out our stack (such is the way of the world on the UMS primary).
            //
#if defined(_DEBUG)
            pContext->SetDebugBits(CTX_DEBUGBIT_PRIMARYSWITCHTOFAILED);
#endif // _DEBUG

        }

        return;

    }

    /// <summary>
    ///     Returns whether we are on a primary thread.
    /// </summary>
    bool UMSSchedulingContext::OnPrimary()
    {
        return (UMSThreadScheduler::FastCurrentSchedulingContext() != NULL);
    }

} // namespace details
} // namespace Concurrency
