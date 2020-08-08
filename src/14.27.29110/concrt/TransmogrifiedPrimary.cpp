// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// TransmogrifiedPrimary.cpp
//
// A very special primary thread whose sole purpose is to "virtually" transmogrify a UMS thread into an NT thread
// for the purposes of scheduler nesting.
//
// Specifically, this thread servers the following functions:
// 1. Run thread proxies to their thread main
// 2. Retire thread proxies
// 3. Allow thread proxies to run external to vproc (in the case of nesting scheduler).
// 4. Poll for thread proxies that are suspended
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Destroy a UMSBaseObject
    /// </summary>
    UMSBaseObject::~UMSBaseObject()
    {
    }

    /// <summary>
    ///     Construct a transmogrified primary
    /// </summary>
    TransmogrifiedPrimary::TransmogrifiedPrimary() :
        m_pBoundProxy(NULL),
        m_hCompletionListEvent(NULL),
        m_hBlock(NULL),
        m_hRetire(NULL),
        m_queueCount(0)
    {
        Initialize();
    }

    /// <summary>
    ///     Initialize the transmogrified primary.
    /// </summary>
    void TransmogrifiedPrimary::Initialize()
    {
        if (!UMS::CreateUmsCompletionList(&m_pCompletionList))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        if (!UMS::GetUmsCompletionListEvent(m_pCompletionList, &m_hCompletionListEvent))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_hRetire = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (m_hRetire == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_hBlock = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (m_hBlock == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_hPrimary = LoadLibraryAndCreateThread(NULL,
                                  0,
                                  PrimaryMain,
                                  this,
                                  0,
                                  &m_primaryId);

        if (m_hPrimary == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }


    /// <summary>
    ///     Destroys the transmogrified primary.
    /// </summary>
    TransmogrifiedPrimary::~TransmogrifiedPrimary()
    {
        CloseHandle(m_hPrimary);

        if (m_hRetire != NULL)
        {
            CloseHandle(m_hRetire);
        }

        if (m_hBlock != NULL)
        {
            CloseHandle(m_hBlock);
        }

        if (m_hCompletionListEvent != NULL)
        {
            CloseHandle(m_hCompletionListEvent);
        }

        UMS::DeleteUmsCompletionList(m_pCompletionList);
    }

    /// <summary>
    ///     Queues a thread to execute to completion and asynchronously returns.
    /// </summary>
    /// <param name="pProxy">
    ///     The proxy to queue to completion.
    /// </param>
    void TransmogrifiedPrimary::QueueToCompletion(UMSThreadProxy *pProxy)
    {
        // We need to use a hypercritical lock here since
        // this is called from the primary as well.
        UMSThreadProxy *pCurrentProxy = UMSThreadProxy::GetCurrent();
        if (pCurrentProxy != NULL)
        {
            pCurrentProxy->EnterHyperCriticalRegion();
        }

        m_queuedExecutions.Enqueue(pProxy);

        if (pCurrentProxy != NULL)
        {
            pCurrentProxy->ExitHyperCriticalRegion();
        }

        if (InterlockedIncrement(&m_queueCount) == 1)
        {
            Unblock();
        }
    }

    /// <summary>
    ///     The UMS primary function.  This is invoked when the primary switches into UMS scheduling mode or whenever a given
    ///     context blocks or yields.
    /// </summary>
    /// <param name="reason">
    ///     The reason for the UMS invocation.
    /// </param>
    /// <param name="activationPayload">
    ///     The activation payload (depends on reason)
    /// </param>
    /// <param name="pData">
    ///     The context (the primary pointer)
    /// </param>
    void NTAPI TransmogrifiedPrimary::PrimaryInvocation(UMS_SCHEDULER_REASON reason, ULONG_PTR activationPayload, PVOID pData)
    {
        (activationPayload);

        TransmogrifiedPrimary *pRoot = NULL;
        PUMS_CONTEXT pPrimaryContext = UMS::GetCurrentUmsThread();

        if (reason != UmsSchedulerStartup)
        {
            //
            // activationPayload and pData might be NULL (blocking), so we're left with storing the TransmogrifiedPrimary in either
            // TLS or the UMS context (the primary does have one).  At present, it's in the UMS context.
            //
            if (!UMS::QueryUmsThreadInformation(pPrimaryContext, UmsThreadUserContext, &pRoot, sizeof(pRoot), NULL))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
        else
        {
            pRoot = reinterpret_cast<TransmogrifiedPrimary *>(pData);

            if (!UMS::SetUmsThreadInformation(pPrimaryContext, UmsThreadUserContext, &pRoot, sizeof(pRoot)))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        UMSThreadProxy *pProxy = NULL;

        switch(reason)
        {
            case UmsSchedulerStartup:
            {
                pProxy = pRoot->WaitForWork();

                if (pProxy == NULL)
                {
                    //
                    // No work was found. We are done
                    //
                    return;
                }

                pRoot->Execute(pProxy);

                CONCRT_COREASSERT(false);
                break;
            }

            case UmsSchedulerThreadBlocked:
            {
                pProxy = pRoot->HandleBlocking();

                if (pProxy == NULL)
                {
                    //
                    // No work was found. We are done
                    //
                    return;
                }

                pRoot->Execute(pProxy);

                CONCRT_COREASSERT(false);
                break;
            }
            case UmsSchedulerThreadYield:
            {
                pProxy = pRoot->HandleYielding();

                if (pProxy == NULL)
                {
                    //
                    // No work was found.  We are done.
                    //
                    return;
                }

                pRoot->Execute(pProxy);

                CONCRT_COREASSERT(false);
                break;
            }
            default:
                CONCRT_COREASSERT(false);
                break;
        }
    }

    /// <summary>
    ///     Handle blocking for a UT on this primary
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::HandleBlocking()
    {
        UMSThreadProxy *pProxy = m_pBoundProxy;
        CONCRT_COREASSERT(pProxy != NULL);

        //
        // Wait for the blocked thread to complete
        //
        WaitForBlockedThread(pProxy);

        //
        // If the thread terminated, either someone did an ExitThread or the thread we meant to run to completion did.  If so, block.
        //
        if (pProxy->IsTerminated())
        {
            //
            // This is the **FIRST** place it's safe to delete the proxy and move on.
            //
            delete pProxy;
            m_pBoundProxy = NULL;

            //
            // Search for new work
            //
            return SearchForWork();
        }
        else
        {
            //
            // proxy has not run to completion yet.
            //
            return pProxy;
        }
    }

    /// <summary>
    ///     Handle yielding for a UT on this primary
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::HandleYielding()
    {
        UMSThreadProxy *pProxy = m_pBoundProxy;
        CONCRT_COREASSERT(pProxy != NULL);

        switch(pProxy->m_yieldAction)
        {
            case UMSThreadProxy::ActionStartup:
            {
                //
                // UT startup
                //
                UMSFreeThreadProxy * pStartedProxy = static_cast<UMSFreeThreadProxy *>(pProxy);
                pStartedProxy->m_yieldAction = UMSThreadProxy::ActionNone;
                SetEvent(pStartedProxy->m_hBlock);
                break;
            }

            default:
            {
                //
                // When the thread explicity yields, it's blocked as far as we're concerned and someone else can run it.  This would be the case
                // on an exit from nesting.
                //
                pProxy->NotifyBlocked(false);
                break;
            }
        };

        m_pBoundProxy = NULL;
        return SearchForWork();
    }

    /// <summary>
    ///     Execute the given proxy on this primary
    /// </summary>
    /// <param name="pProxy">
    ///     The proxy to execute
    /// </param>
    void TransmogrifiedPrimary::Execute(UMSThreadProxy *pProxy)
    {
        CONCRT_COREASSERT(pProxy != NULL);

        m_pBoundProxy = pProxy;

        int retryCount = 0;
        for(;;)
        {
            UMS::ExecuteUmsThread(pProxy->GetUMSContext());
            CONCRT_COREASSERT(!pProxy->IsTerminated());
            Sleep(0);

            // Poll at regular intervals
            if (++retryCount == 100)
            {
                m_poller.DoPolling();
                retryCount = 0;
            }
        }
    }

    /// <summary>
    ///     Search for work queued in the case of multiple binding
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::SearchForWork()
    {
        CONCRT_COREASSERT(m_pBoundProxy == NULL);

        //
        // This decrement is for the PREVIOUSLY executed work item.
        //
        _InterlockedDecrement(&m_queueCount);

        m_pBoundProxy = m_queuedExecutions.Dequeue();

        if (m_pBoundProxy != NULL)
        {
            return m_pBoundProxy;
        }

        CompletedTransmogrification();

        //
        // Dequeue new work and bind it to the primary
        //
        return WaitForWork();
    }

    /// <summary>
    ///     Wait for a proxy to appear on the completion list
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::WaitForBlockedThread(UMSThreadProxy * pProxy)
    {
        //
        // While waiting on the completion list we need to poll proxies for execution, if any.
        // This is required because the current proxy could be blocked for a resource that is
        // held by a UT that is suspended (and needs to be polled for subsequent execution).
        //
        const int maxCount = 2;
        HANDLE hObjects[maxCount];
        int count = 0;
        hObjects[count++] = m_poller.GetEvent();
        hObjects[count++] = m_hCompletionListEvent;

        CONCRT_COREASSERT(count == maxCount);

        DWORD timeout = INFINITE;

        for(;;)
        {
            DWORD result = WaitForMultipleObjectsEx(count, hObjects, FALSE, timeout, FALSE);
            DWORD index = (result == WAIT_TIMEOUT) ? 0 : (result - WAIT_OBJECT_0);

            if (index == 0)
            {
                bool done = m_poller.DoPolling();

                //
                // Poll every interval
                //
                timeout = done ? INFINITE : UMSBackgroundPoller::PollInterval();
            }
            else
            {
                CONCRT_COREASSERT(index == 1);

                // Proxy came back on the completion list
                PUMS_CONTEXT pUMSContext = NULL;
                if (!UMS::DequeueUmsCompletionListItems(m_pCompletionList, 0, &pUMSContext))
                    throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

                //
                // The completed thread should be the one we are running
                //
                UMSThreadProxy *pCompletedProxy = UMSThreadProxy::FromUMSContext(pUMSContext);
                CONCRT_COREASSERT(pCompletedProxy == pProxy && UMS::GetNextUmsListItem(pUMSContext) == NULL);
                return pCompletedProxy;
            }
        }
    }

    /// <summary>
    ///     Wait for new work
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::WaitForWork()
    {
        //
        // There are 3 possibilities here
        // 1. A proxy needs to be polled for execution
        // 2. A proxy needs to be transmogrified/retired/run to thread main
        // 3. This background thread needs to be retired
        //
        const int maxCount = 3;
        HANDLE hObjects[maxCount];
        int count = 0;
        hObjects[count++] = m_poller.GetEvent();
        hObjects[count++] = m_hBlock;
        hObjects[count++] = m_hRetire;

        CONCRT_COREASSERT(count == maxCount);

        DWORD timeout = INFINITE;

        for(;;)
        {
            DWORD result = WaitForMultipleObjectsEx(count, hObjects, FALSE, timeout, FALSE);
            DWORD index = (result == WAIT_TIMEOUT) ? 0 : (result - WAIT_OBJECT_0);

            if (index == 0)
            {
                bool done = m_poller.DoPolling();

                //
                // Poll every interval
                //
                timeout = done ? INFINITE : UMSBackgroundPoller::PollInterval();
            }
            else if (index == 1)
            {
                //
                // Dequeue new work and bind it to the primary. It is possible
                // that we already picked up the entry that signalled the event.
                //
                m_pBoundProxy = m_queuedExecutions.Dequeue();
                if (m_pBoundProxy != NULL)
                {
                    return m_pBoundProxy;
                }
            }
            else
            {
                //
                // Canceled
                //
                CONCRT_COREASSERT(index == 2);

                CONCRT_COREASSERT(m_queueCount == 0);
                CONCRT_COREASSERT(timeout == INFINITE);
                return NULL;
            }
        }
    }

    /// <summary>
    ///     The primary thread for this transmogrified primary.
    /// </summary>
    /// <param name="pContext">
    ///     The TransmogrifiedPrimary that this thread manages.
    /// </param>
    DWORD CALLBACK TransmogrifiedPrimary::PrimaryMain(LPVOID pContext)
    {
        TransmogrifiedPrimary *pPrimary = reinterpret_cast<TransmogrifiedPrimary *>(pContext);
        UMS_SCHEDULER_STARTUP_INFO si;

        si.UmsVersion = UMS_VERSION;
        si.CompletionList = pPrimary->m_pCompletionList;
        si.SchedulerProc = (PUMS_SCHEDULER_ENTRY_POINT) &PrimaryInvocation;
        si.SchedulerParam = pContext;

        if (!UMS::EnterUmsSchedulingMode(&si))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        delete pPrimary;
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

    /// <summary>
    ///     Construct a new cached transmogrified primary.  The primary will be placed back upon the cache of the specified transmogrificator
    ///     at the end of each transmogrification.
    /// </summary>
    CachedTransmogrifiedPrimary::CachedTransmogrifiedPrimary(Transmogrificator *pTransmogrificator) :
        m_pTransmogrificator(pTransmogrificator)
    {
    }

    /// <summary>
    ///     Called when a transmogrification is complete.
    /// </summary>
    void CachedTransmogrifiedPrimary::CompletedTransmogrification()
    {
        m_pTransmogrificator->ReturnToCache(this);
    }


} // namespace details
} // namespace Concurrency
