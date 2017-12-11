// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadInternalContext.cpp
//
// Implementation file containing the metaphor for a UMS thread based internal execution context/stack.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Construct an internal thread based context.
    /// </summary>
    UMSThreadInternalContext::UMSThreadInternalContext(SchedulerBase *pScheduler) :
        InternalContextBase(pScheduler),
        m_blockingType(BlockingNone)
    {

    }

    /// <summary>
    ///     Destroys an internal thread based context.
    /// </summary>
    UMSThreadInternalContext::~UMSThreadInternalContext()
    {
    }

    /// <summary>
    ///     Called before this executes on a given virtual processor.
    /// </summary>
    /// <param name="pVProc">
    ///     The virtual processor that we are about to run on.
    /// </param>
    void UMSThreadInternalContext::PrepareToRun(VirtualProcessor *pVProc)
    {
        //
        // m_pVirtualProcessor will be set in the base class.  We require knowledge of the previous virtual processor in order to implement
        // critical regions.
        //
        m_pLastVirtualProcessor = static_cast<UMSThreadVirtualProcessor *>(pVProc);
        m_blockingType = BlockingNone;
        InternalContextBase::PrepareToRun(pVProc);
    }

    /// <summary>
    ///     Notifies the context that it's blocked (with the given blocking type)
    /// </summary>
    /// <param name="fAsynchronous">
    ///     Is previously running context asynchronously blocked.
    /// </param>
    void UMSThreadInternalContext::NotifyBlocked(bool fAsynchronous)
    {
        UNSAFE_SetVirtualProcessor(NULL);
        InterlockedExchange(&m_blockedState, fAsynchronous ? CONTEXT_UMS_ASYNC_BLOCKED : CONTEXT_UMS_SYNC_BLOCKED);

        //
        // After this point, it might be running atop another vproc.  Remember that it may have come back on the completion list and been affinitized
        // prior to even getting into this code!
        //
    }

    /// <summary>
    ///     Causes the internal context to block yielding the virtual processor to a different internal context.
    /// </summary>
    void UMSThreadInternalContext::Block()
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
            // Only switch to the other context, if unblock has not been called since we last touched the
            // context switching fence. If there was an unblock since, the comparison below will fail.
            if ((InterlockedIncrement(&m_contextSwitchingFence) == 1)
                && (InterlockedCompareExchange(&m_contextSwitchingFence, 2, 1) == 1))
            {
                // *NOTE* After this point, we shall not block.  A racing ::Unblock call can put *US* on the runnables list and the scheduler
                // will get awfully confused if a UMS activation happens between now and the time we SwitchTo the context below.  Note that
                // page faults and suspensions are masked by the effect of being in a critical region.  It just means that we cannot call
                // *ANY* blocking API (including creating a new thread)
                // We will just switch to the primary.
                SwitchTo(NULL, Blocking);
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
    ///     The method that is called when a thread proxy starts executing a particular context.  The thread proxy which executes
    ///     the context is passed into this method and must be saved and returned on a call to the get_Proxy method.
    /// </summary>
    /// <param name="pDispatchState">
    ///     The state under which this IExecutionContext is being dispatched.
    /// </param>
    void UMSThreadInternalContext::Dispatch(DispatchState * pDispatchState)
    {
        CONCRT_COREASSERT(GetCriticalRegionType() == OutsideCriticalRegion);
        InternalContextBase::Dispatch(pDispatchState);
    }

    /// <summary>
    ///     Enters a critical region of the scheduler.  Calling this guarantees that the virtual processor on which this context lives
    ///     is guaranteed to be stable throughout the critical region.  For some context types, this is virtually a NOP.  For others
    ///     (UMS), this makes it appear that blocking on the context actually blocks the UMS thread instead of triggering return to
    ///     primary.  Note that critical regions suppress asynchronous blocking but not synchronous blocking.
    /// </summary>
    int UMSThreadInternalContext::EnterCriticalRegionHelper()
    {
        CONCRT_COREASSERT(m_pThreadProxy != NULL);

        int val = 0;

        while (true)
        {
            ContextBase::EnterCriticalRegionHelper();
            val = (static_cast<IUMSThreadProxy *>(m_pThreadProxy))->EnterCriticalRegion();

            if (m_criticalRegionCount == 1)
            {
                //
                // First entry must be specially handled.  It's possible that someone else was inside the critical region and we cannot enter.
                // This can happen with an ordinary critical region, but not a hyper critical one.
                //
                // Note that the pVProc == NULL case happens when we make an attempt to rejoin a scheduler on denesting.  The context is still transmogrified
                // until it does its SwitchOut but it is running.  There's no worry about conflicts of critical regions in this path.
                //
                UMSThreadVirtualProcessor *pVProc = static_cast<UMSThreadVirtualProcessor *>(m_pVirtualProcessor);
                if (pVProc == NULL || pVProc->CanCriticalEnter())
                {
                    // We are allowed to enter critical region
                    break;
                }

                // VProc is already in a critical region. Exit and retry.
                ExitCriticalRegionHelper();

                // We want to go back to the primary (allowing other vprocs to pick us up)
                Sleep(1);
            }
            else
            {
                // Already in a critical region
                break;
            }
        }

        return val;
    }

    /// <summary>
    ///     Exits a critical region of the scheduler.
    /// </summary>
    int UMSThreadInternalContext::ExitCriticalRegionHelper()
    {
        CONCRT_COREASSERT(m_pThreadProxy != NULL);

        ContextBase::ExitCriticalRegionHelper();
        return (static_cast<IUMSThreadProxy *>(m_pThreadProxy))->ExitCriticalRegion();
    }

    /// <summary>
    ///     Enters a hyper-critical region of the scheduler.  Calling this guarantees not only the conditions of a critical region but it
    ///     guarantees that synchronous blocking is suppressed as well.  This allows for lock sharing between the primary and hyper-critical
    ///     regions running on UTs.  No lock sharing can occur between the inside of this region type and the outside of this region type
    ///     on a UT.
    /// </summary>
    int UMSThreadInternalContext::EnterHyperCriticalRegionHelper()
    {
        CONCRT_COREASSERT(m_pThreadProxy != NULL);

        int val = 0;

        while (true)
        {
            ContextBase::EnterHyperCriticalRegionHelper();
            val = (static_cast<IUMSThreadProxy *>(m_pThreadProxy))->EnterHyperCriticalRegion();

            if (m_criticalRegionCount == 1)
            {
                //
                // First entry must be specially handled.  It's possible that someone else was inside the critical region and we cannot enter.
                // This can happen with an ordinary critical region, but not a hyper critical one.
                //
                // Note that the pVProc == NULL case happens when we make an attempt to rejoin a scheduler on denesting.  The context is still transmogrified
                // until it does its SwitchOut but it is running.  There's no worry about conflicts of critical regions in this path.
                //
                UMSThreadVirtualProcessor *pVProc = static_cast<UMSThreadVirtualProcessor *>(m_pVirtualProcessor);
                if (pVProc == NULL || pVProc->CanCriticalEnter())
                {
                    // We are allowed to enter critical region
                    break;
                }

                // VProc is already in a critical region. Exit and retry.
                ExitHyperCriticalRegionHelper();

                // We want to go back to the primary (allowing other vprocs to pick us up)
                Sleep(1);
            }
            else
            {
                // Alreay in a critical region
                break;
            }
        }

        return val;
    }

    /// <summary>
    ///     Exits a hyper-critical region of the scheduler.
    /// </summary>
    int UMSThreadInternalContext::ExitHyperCriticalRegionHelper()
    {
        CONCRT_COREASSERT(m_pThreadProxy != NULL);

        ContextBase::ExitHyperCriticalRegionHelper();
        return (static_cast<IUMSThreadProxy *>(m_pThreadProxy))->ExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Spins until we've registered what blocking type the last block operation was and returns the type.
    /// </summary>
    UMSThreadInternalContext::BlockingType UMSThreadInternalContext::SpinOnAndReturnBlockingType()
    {
        if (m_blockingType == BlockingNone)
        {
            // Spin wait for blocking type to be set
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();

            } while (m_blockingType == BlockingNone);
        }

        return m_blockingType;
    }

/// <summary>
///     Depending on the argument, causes the scheduler to add an extra virtual processor for the
///     duration of a block of code or remove a previously added one.
///
///         Oversubscribe(true);
///         // some slow kernel or I/O code, etc.
///         Oversubscribe(false);
///
///     UMS provides automatic oversubscription. This API is largely ignored in UMS except for enforcing
///     the matching Oversubscribe(false) API as per the spec.
/// </summary>
/// <param name="beginOversubscription">
///     A boolean value specifying whether oversubscription is to be turned on or off.
/// </param>
    void UMSThreadInternalContext::Oversubscribe(bool beginOversubscription)
    {
        //
        // The reason why someone would call Oversubscribe is because they feel they are going to do something long running and blocking
        // and don't want to lose the core while the blocking occurs.  UMS solves this problem and hence ignores the Oversubscribe call.
        // This is according to spec.
        //
        // Note that the only thing we do is match Oversubscribe(true) and ...(false) calls per-spec.
        //
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        if (beginOversubscription)
        {
            ++m_oversubscribeCount;
        }
        else
        {
            if (m_oversubscribeCount == 0)
            {
                throw invalid_oversubscribe_operation();
            }
            --m_oversubscribeCount;
        }
    }

} // namespace details
} // namespace Concurrency
