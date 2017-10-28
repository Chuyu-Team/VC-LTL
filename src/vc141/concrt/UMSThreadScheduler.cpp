// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadScheduler.cpp
//
// Source file containing the implementation for a UMS thread based concrt scheduler
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    DWORD UMSThreadScheduler::t_dwSchedulingContextIndex;

    /// <summary>
    ///     Creates a UMS thread based scheduler
    /// </summary>
    UMSThreadScheduler::UMSThreadScheduler(_In_ const ::Concurrency::SchedulerPolicy& policy) :
        SchedulerBase(policy),
        m_pCompletionList(NULL)
    {
    }

    /// <summary>
    ///     Creates a UMS thread based scheduler
    /// </summary>
    UMSThreadScheduler* UMSThreadScheduler::Create(_In_ const ::Concurrency::SchedulerPolicy& policy)
    {
        return _concrt_new UMSThreadScheduler(policy);
    }

    /// <summary>
    ///     Creates a UMS thread based virtual processor.
    /// </summary>
    VirtualProcessor* UMSThreadScheduler::CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot* pOwningRoot)
    {
        return _concrt_new UMSThreadVirtualProcessor(pOwningNode, pOwningRoot);
    }

    ///<summary>
    ///     Creates a new thread internal context and returns it to the base scheduler.
    ///</summary>
    InternalContextBase *UMSThreadScheduler::CreateInternalContext()
    {
        return _concrt_new UMSThreadInternalContext(this);
    }

    /// <summary>
    ///     Destroys a UMS thread based scheduler
    /// </summary>
    UMSThreadScheduler::~UMSThreadScheduler()
    {
    }

    ///<summary>
    ///     Notification after a virtual processor goes from INACTIVE to ACTIVE or ACTIVE to INACTIVE
    ///     For UMS we need to ensure that there is at least 1 active vproc.
    ///</summary>
    /// <param value="fActive">
    ///     True if a virtual processor is going from INACTIVE to ACTIVE, and false if it is going from ACTIVE to INACTIVE.
    /// </param>
    /// <param value="activeCount">
    ///     Active virtual processor count after the transition
    /// </param>
    void UMSThreadScheduler::VirtualProcessorActiveNotification(bool fActive, LONG activeCount)
    {
        // We need to maintain at least 1 active virtual processor as long
        // as there is work. Since we cannot easily determine if there are blocked UMS context,
        // do not allow active vproc count to go from 1 to 0.
        if (activeCount == 0)
        {
            // If we are the last active virtual processor, we should be in a hyper cricital region
            CONCRT_COREASSERT(!fActive);
            StartupIdleVirtualProcessor(GetNextSchedulingRing()->GetAnonymousScheduleGroupSegment());
        }
    }

    /// <summary>
    ///     Called in order to move the completion list to the runnables lists.
    /// </summary>
    /// <param name="bias">
    ///     Bias any awakening of virtual processors to this location.
    /// </param>
    /// <returns>
    ///     Whether there was anything on the completion list when queried.
    /// </returns>
    bool UMSThreadScheduler::MoveCompletionListToRunnables(location bias)
    {
        bool fHadItems = false;

        //
        // This *ABSOLUTELY* must be in a hyper-critical region.  Deadlock can ensue if not.  Anything outward from this
        // must follow the set of rules governing a hyper-critical region.
        //
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        if (pCurrentContext != NULL)
            pCurrentContext->EnterHyperCriticalRegion();

        IUMSUnblockNotification *pUnblock = m_pCompletionList->GetUnblockNotifications();
        while (pUnblock != NULL)
        {
            fHadItems = true;

            IUMSUnblockNotification *pNext = pUnblock->GetNextUnblockNotification();
            UMSThreadInternalContext *pContext = static_cast<UMSThreadInternalContext *> (pUnblock->GetContext());

            VCMTRACE(MTRACE_EVT_PULLEDFROMCOMPLETION, pContext, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext)->m_pVirtualProcessor : NULL, pCurrentContext);

#if defined(_DEBUG)
            pContext->SetDebugBits(CTX_DEBUGBIT_PULLEDFROMCOMPLETIONLIST);
#endif // _DEBUG

            //
            // In order to know what to do with this particular item, we need to know *why* it blocked.  If the primary hasn't gotten to telling us that,
            // we must spin.
            //
            UMSThreadInternalContext::BlockingType blockingType = pContext->SpinOnAndReturnBlockingType();
            CONCRT_COREASSERT(blockingType != UMSThreadInternalContext::BlockingNone);

            //
            // Make a determination of where this item goes.  There are several cases here:
            //
            // - It might have UMS blocked during a normal critical region (e.g.: the main dispatch loop blocked on the heap lock or some
            //   other similar object).  If the context was inside a critical region, we have special determinations to make.
            //
            // - It might just be a runnable.
            //
            switch(blockingType)
            {
                case UMSThreadInternalContext::BlockingCritical:
                    //
                    // This is the single special context allowed to be "inside a critical region" on the virtual processor.  Signal the virtual
                    // processor specially.
                    //
                    VCMTRACE(MTRACE_EVT_CRITICALNOTIFY, pContext, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext)->m_pVirtualProcessor : NULL, pCurrentContext);
#if defined(_DEBUG)
                    pContext->SetDebugBits(CTX_DEBUGBIT_CRITICALNOTIFY);
#endif // _DEBUG
                    pContext->m_pLastVirtualProcessor->CriticalNotify();
                    break;

                case UMSThreadInternalContext::BlockingNormal:
                    //
                    // If it's a normal runnable, it just goes on the runnables list.  We pass along the bias to indicate which virtual processor
                    // (or owning node) we prefer to awaken due to the addition of runnables.
                    //
                    pContext->AddToRunnables(bias);
                    break;
            }

            pUnblock = pNext;

        }

        if (pCurrentContext != NULL)
            pCurrentContext->ExitHyperCriticalRegion();

        return fHadItems;

    }

    /// <summary>
    ///     Static initialization common to UMS schedulers.
    /// </summary>
    void UMSThreadScheduler::OneShotStaticConstruction()
    {
        t_dwSchedulingContextIndex = TlsAlloc();
        if (t_dwSchedulingContextIndex == TLS_OUT_OF_INDEXES)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    /// <summary>
    ///     Static destruction common to UMS schedulers.
    /// </summary>
    void UMSThreadScheduler::OneShotStaticDestruction()
    {
        TlsFree(t_dwSchedulingContextIndex);
        t_dwSchedulingContextIndex = 0;
    }

} // namespace details
} // namespace Concurrency
