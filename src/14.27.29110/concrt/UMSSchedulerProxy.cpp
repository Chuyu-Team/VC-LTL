// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSSchedulerProxy.cpp
//
// RM proxy for a UMS scheduler instance
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Constructs a new scheduler proxy for a UMS scheduler.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler in question.
    /// </param>
    /// <param name="pResourceManager">
    ///     The resource manager instance.
    /// </param>
    /// <param name="policy">
    ///     A copy of the scheduler's policy
    /// </param>
    UMSSchedulerProxy::UMSSchedulerProxy(IScheduler *pScheduler, ResourceManager *pResourceManager, const SchedulerPolicy &policy) :
        SchedulerProxy(pScheduler, pResourceManager, policy),
        m_pCompletionList(NULL),
        m_hCompletionListEvent(NULL),
        m_hTransferListEvent(NULL),
        m_pushedBackCount(0),
        m_refCount(1)
    {
        m_hTransferListEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (m_hTransferListEvent == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        if (!UMS::CreateUmsCompletionList(&m_pCompletionList))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        if (!UMS::GetUmsCompletionListEvent(m_pCompletionList, &m_hCompletionListEvent))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        InitializeSListHead(&m_transferList);

        (static_cast<IUMSScheduler *>(pScheduler))->SetCompletionList(this);
    }

    /// <summary>
    ///     Destroys a scheduler proxy for a UMS scheduler.
    /// </summary>
    UMSSchedulerProxy::~UMSSchedulerProxy()
    {
        UMSThreadProxy *pProxy = GetCompletionListItems();
        CONCRT_COREASSERT(pProxy == NULL);

        if (m_hTransferListEvent != NULL)
            CloseHandle(m_hTransferListEvent);

        if (m_hCompletionListEvent != NULL)
            CloseHandle(m_hCompletionListEvent);

        if (m_pCompletionList != NULL)
            UMS::DeleteUmsCompletionList(m_pCompletionList);
    }

    /// <summary>
    ///     Handles the completion of a UMS thread.
    /// </summary>
    /// <param name="pCompletion">
    ///     The thread which was noticed on the completion list
    /// </param>
    /// <returns>
    ///     An indication as to whether the thread moved to the transfer list (true).  If false is returned, the thread was special
    ///     (e.g.: a critically blocked thread) and was handled through a different path.
    /// </returns>
    bool UMSSchedulerProxy::HandleCompletion(UMSThreadProxy *pCompletion)
    {
        //
        // We need to make absolutely certain that we know *WHY* the context blocked so we can tell what to do when it comes off the completion list.
        // This is not known until the primary which was invoked sets appropriate flags and then notifies the proxy that it is blocked.  In order to
        // read those bits, we must spin until the proxy has set those flags.
        //
        UMSThreadProxy::BlockingType blockingType = pCompletion->SpinOnAndReturnBlockingType();

        //
        // We are allowing thread termination on the way out of the RM's main loop in order to retire virtual processors and threads simultaneously
        // (a necessary condition in order to work around a win7 issue).  This means that terminated threads can come back on the completion list.  We
        // do not want to pop this back for the scheduler -- the scheduler should already know (this is *NOT* TerminateThread friendly).
        //
        // Termination will take the same path as critical blocking.  We must ensure elsewhere in the scheduler that threads we allow to terminate in
        // this manner are in hyper-critical regions.
        //
        CONCRT_COREASSERT(!pCompletion->IsTerminated() || blockingType == UMSThreadProxy::BlockingCritical);

#if defined(_DEBUG)
        if (pCompletion->IsTerminated())
        {
            pCompletion->m_UMSDebugBits |= UMS_DEBUGBIT_COMPLETIONTERMINATED;
        }
#endif // _DEBUG

        RPMTRACE(MTRACE_EVT_PULLEDFROMCOMPLETION, pCompletion, NULL, blockingType);

#if defined(_DEBUG)
        CONCRT_COREASSERT(pCompletion->m_UMSDebugBits != UMS_DEBUGBIT_YIELDED);
#endif // _DEBUG

        if (blockingType == UMSThreadProxy::BlockingCritical)
        {
#if defined(_DEBUG)
            pCompletion->m_UMSDebugBits |= UMS_DEBUGBIT_CRITICALNOTIFY;
#endif // _DEBUG
            pCompletion->m_pLastRoot->CriticalNotify();
        }
        else if (!pCompletion->MessagedYield())
        {
#if defined(_DEBUG)
            pCompletion->m_UMSDebugBits |= UMS_DEBUGBIT_TRANSFERLIST;
#endif // _DEBUG

            //
            // Right now, just move the entry to the transfer list.
            //
            InterlockedPushEntrySList(&m_transferList, &(pCompletion->m_listEntry));

            //
            // Set the transferlist event that should wake up vprocs that are deactivated
            //
            if (InterlockedIncrement(&m_pushedBackCount) == 1)
            {
                SetEvent(m_hTransferListEvent);
            }

            return true;
        }
        else
        {
#if defined(_DEBUG)
            pCompletion->m_UMSDebugBits |= UMS_DEBUGBIT_SKIPPEDCOMPLETION;
#endif // _DEBUG
        }

        return false;
    }


    /// <summary>
    ///     Sweeps the completion list looking for critically blocked items or the sought item before moving everything to
    ///     the second stage transfer list.  If the sought item is found, true is returned and it is NOT enqueued to the
    ///     transfer list.  Any critically blocked item signals a critical notification event of the appropriate primary
    ///     and is NOT enqueued to the transfer list.
    /// </summary>
    /// <param name="pSought">
    ///     The thread proxy to sweep for.  If NULL, everything but critically blocked items are moved to the transfer list.
    /// </param>
    /// <param name="fWait">
    ///     An indication as to whether or not to wait for something to come onto the completion list.
    /// </param>
    /// <returns>
    ///     An indication as to whether the swept item was found.  The caller owns it if true was returned.  It is NOT moved
    ///     to the transfer list.
    /// </returns>
    bool UMSSchedulerProxy::SweepCompletionList(UMSThreadProxy *pSought, bool fWait)
    {
        PUMS_CONTEXT pFirstContext;
        PUMS_CONTEXT pContext;

        bool fFound = false;

        if (!UMS::DequeueUmsCompletionListItems(m_pCompletionList, fWait ? INFINITE : 0, &pFirstContext))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        pContext = pFirstContext;
        while (pContext != NULL)
        {
            UMSThreadProxy *pProxy = UMSThreadProxy::FromUMSContext(pContext);
            PUMS_CONTEXT pNext = UMS::GetNextUmsListItem(pContext);

#if defined(_DEBUG)
            CONCRT_COREASSERT((pProxy->m_UMSDebugBits & (UMS_DEBUGBIT_HANDEDTOPOLLER | UMS_DEBUGBIT_POLLERFOUNDCOMPLETION)) != UMS_DEBUGBIT_HANDEDTOPOLLER);
            pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_COMPLETION;
#endif // _DEBUG

            if (pProxy == pSought)
                fFound = true;
            else
                HandleCompletion(pProxy);

            pContext = pNext;

        }

        return fFound;
    }


    /// <summary>
    ///     Returns all items on the UMS completion list.  Note that this indirectly manipulates the transfer list.  This
    ///     will literally dequeue everything from BOTH lists and hand it to the caller.
    /// </summary>
    /// <returns>
    ///     The first UMSThreadProxy on the completion list.
    /// </returns>
    UMSThreadProxy *UMSSchedulerProxy::GetCompletionListItems()
    {
        SweepCompletionList();

        InterlockedExchange(&m_pushedBackCount, 0);

        //
        // Yep, there's a race where we could inadvertently set the event for an item which is pushed in this window from another thread
        // but popped right here.  The worst thing that comes out of said race is an extra awakening of a virtual processor to check
        // an empty completion list.
        //

        PSLIST_ENTRY pItem = InterlockedFlushSList(&m_transferList);
        UMSThreadProxy *pProxy = pItem == NULL ? NULL : CONTAINING_RECORD(pItem, UMSThreadProxy, m_listEntry);

#if defined(_DEBUG)
        UMSThreadProxy *pTrav = pProxy;
        while (pTrav != NULL)
        {
            pTrav->m_UMSDebugBits |= UMS_DEBUGBIT_PULLEDFROMTRANSFERLIST;
            pTrav = static_cast<UMSThreadProxy *>(pTrav->GetNextUnblockNotification());
        }
#endif // _DEBUG

        return pProxy;
    }

    /// <summary>
    ///     Create a virtual processor root and add it to the scheduler proxys list of roots.
    /// </summary>
    VirtualProcessorRoot* UMSSchedulerProxy::CreateVirtualProcessorRoot(SchedulerNode* pNode, unsigned int coreIndex)
    {
        // Callers of this API, should acquire the lock on the scheduler proxy if necessary. Currently, this API
        // is only used for initial allocation, and to add oversubscribed vprocs (this path takes a lock). When the
        // API is used to add cores during dynamic core migration, its caller will need to synchronize with a concurrent
        // call to ISchedulerProxy::Shutdown.

        VirtualProcessorRoot* pRoot = _concrt_new UMSFreeVirtualProcessorRoot(this, pNode, coreIndex);
        return pRoot;
    }

    /// <summary>
    ///     Called in order to push a polled completion onto the transfer list.  This should only be called from the background poller which
    ///     is quite intimate with the rest of the UMS data structures.
    /// </summary>
    /// <param name="pProxy">
    ///     The proxy which the background poller has noticed is now awake and should be on the "completion" list.
    /// </param>
    void UMSSchedulerProxy::PushPolledCompletion(UMSThreadProxy *pProxy)
    {
#if defined(_DEBUG)
        pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_POLLERFOUNDCOMPLETION;
#endif // _DEBUG

        //
        // Yes, the short circuit on HandleCompletion is the correct behavior here.  We only want to signal the transfer list event and wake
        // some vproc root if something was actually put upon the transfer list.
        //
        HandleCompletion(pProxy);
    }

    /// <summary>
    ///     Gets a new thread proxy from the factory.
    /// </summary>
    IThreadProxy * UMSSchedulerProxy::GetNewThreadProxy(IExecutionContext * pContext)
    {
        //
        // This cannot be called on the primary!
        //
        if (m_pThreadProxyFactory == NULL)
        {
            m_pThreadProxyFactory = GetResourceManager()->GetThreadProxyFactoryManager()->GetUMSFreeThreadProxyFactory();
        }

        UMSFreeThreadProxy * pProxy = static_cast<UMSFreeThreadProxy *>(m_pThreadProxyFactory->RequestProxy(ContextStackSize(), ContextPriority()));
        pProxy->AssociateExecutionContext(pContext);

        return pProxy;
    }

    /// <summary>
    ///     Ensures that a context is bound to a thread proxy.  This API should *NOT* be called in the vast majority of circumstances.
    ///     The IThreadProxy::SwitchTo will perform late binding to thread proxies as necessary.  There are, however, circumstances
    ///     where it is necessary to pre-bind a context to ensure that the SwitchTo operation switches to an already bound context.  This
    ///     is the case on a UMS scheduling context as it cannot call allocation APIs.
    /// </summary>
    /// <param name="pContext">
    ///     The context to bind.
    /// </param>
    void UMSSchedulerProxy::BindContext(IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        // Find out if this context already has a thread proxy, if not we have to request one from the factory.
        if (pContext->GetProxy() == NULL)
        {
            // Find a thread proxy from the pool.
            GetNewThreadProxy(pContext);
        }
    }

    /// <summary>
    ///     Returns an **unstarted** thread proxy attached to pContext, to the thread proxy factory.
    ///     Such a thread proxy **must** be unstarted.
    ///     This API should *NOT* be called in the vast majority of circumstances.
    /// </summary>
    /// <param name="pContext">
    ///     The context to unbind.
    /// </param>
    void UMSSchedulerProxy::UnbindContext(IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        UMSFreeThreadProxy * pProxy = static_cast<UMSFreeThreadProxy *> (pContext->GetProxy());

        CONCRT_COREASSERT(pProxy != NULL);
        RPMTRACE(MTRACE_EVT_CONTEXTUNBOUND, pProxy, NULL, pContext);
        pProxy->ReturnIdleProxy();
    }

} // namespace details
} // namespace Concurrency
