// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSBackgroundPoller.cpp
//
// A background thread responsible for polling for contexts which had a failed ExecuteUmsThread and inject them on our
// abstraction of a completion list.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a new UMS background poller.
    /// </summary>
    /// <param name="pollingPeriod">
    ///     How often (in milliseconds) the background thread should poll for awakenings.
    /// </param>
    UMSBackgroundPoller::UMSBackgroundPoller() :
        m_hNotification(NULL),
        m_pollCount(0)
    {
        m_hNotification = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (m_hNotification == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    /// <summary>
    ///     Destructs a UMS background poller.
    /// </summary>
    UMSBackgroundPoller::~UMSBackgroundPoller()
    {
        CloseHandle(m_hNotification);
    }

    /// <summary>
    ///     Causes the background poller thread to poll for waking of pThreadProxy and insert it into the completion
    ///     list of pSchedulerProxy.
    /// </summary>
    /// <param name="pThreadProxy">
    ///     The thread proxy to poll for awakening of.
    /// </param>
    /// <param name="pSchedulerProxy">
    ///     The scheduler proxy to insert pThreadProxy into the completion list of when awake.
    /// </param>
    void UMSBackgroundPoller::InsertProxyInCompletionWhenAwake(UMSThreadProxy *pThreadProxy, UMSSchedulerProxy *pSchedulerProxy)
    {
        pThreadProxy->m_pPollInsertionProxy = pSchedulerProxy;
#if defined(_DEBUG)
        pThreadProxy->m_backgroundPollerEntry.m_pollStart = _ReadTimeStampCounter();
        pThreadProxy->m_backgroundPollerEntry.m_pollCount = 0;
#endif // _DEBUG
        m_pollProxies.AddTail(&(pThreadProxy->m_backgroundPollerEntry.m_link));
        if (InterlockedIncrement(&m_pollCount) == 1)
        {
            SetEvent(m_hNotification);
        }
    }

    /// <summary>
    ///     Wakes up and polls periodically the list of proxies that are required and inserts them into the appropriate transfer
    ///     lists if they are awake.
    /// </summary>
    bool UMSBackgroundPoller::DoPolling()
    {
        SafeRWList<ListEntry>::_Scoped_lock writeLock(m_pollProxies);
        ListEntry *pEntry = m_pollProxies.First();
        while (pEntry != NULL)
        {
            ListEntry *pNext = m_pollProxies.Next(pEntry);
            UMSBackgroundPollerEntry *pPollerEntry = CONTAINING_RECORD(pEntry, UMSBackgroundPollerEntry, m_link);
            UMSThreadProxy *pProxy = CONTAINING_RECORD(pPollerEntry, UMSThreadProxy, m_backgroundPollerEntry);
            if (!pProxy->IsSuspended())
            {
                m_pollProxies.UnlockedRemove(pEntry);
                InterlockedDecrement(&m_pollCount);
                pProxy->m_pPollInsertionProxy->PushPolledCompletion(pProxy);
            }
#if defined(_DEBUG)
            else
            {
                pPollerEntry->m_pollCount++;
            }
#endif // defined(_DEBUG)

            pEntry = pNext;
        }

        return (m_pollCount == 0);
    }
} // namespace details
} // namespace Concurrency
