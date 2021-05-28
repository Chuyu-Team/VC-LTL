// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Transmogrificator.cpp
//
// A background thread which has the purpose of transmogrifying UTs to make them appear as NT threads to the layer
// above them.  This is done via queued creation of a TransmogrifiedPrimary.  The reason there is a background thread
// that does this is that we can only make a determination of when to transmogrify at a SwitchTo(..., Nesting) or subsequent
// SwitchOut in the RM.  At this point, we **CANNOT** perform a heap allocation without hopelessly confusing the scheduler
// or deadlocking it.  But we **MUST** not allow the running thread to continue without performing a heap allocation.  The
// catch-22 is solved here by returning the virtual processor (allowing the SwitchTo to happen) and letting it run things
// which may hold the heap lock.  The original UT (which certainly isn't holding any blasted locks at this stage) isn't run again
// until this thread can perform the allocation and get it set up.
//
// Note that this thread may **VERY WELL** cache transmogrified primaries in order to avoid having to do the thread creation and
// heap allocations on each nesting.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a new Transmogrificator.
    /// </summary>
    Transmogrificator::Transmogrificator()
        : m_hWaitHandle(NULL)
        , m_hUnblock(NULL)
        , m_queuedProxyCount(0)
        , m_cacheCount(0)
    {
        m_hUnblock = CreateEventW(NULL, FALSE, FALSE, NULL); // VSO#459907
        if (m_hUnblock == NULL)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        InitializeSListHead(&m_cachedProxies);

        if (!RegisterWaitForSingleObject(&m_hWaitHandle,
                        m_hUnblock,
                        Transmogrificator::TransmogrificationHandler,
                        this, INFINITE, WT_EXECUTEDEFAULT))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
    }


    /// <summary>
    ///     Destructs the Transmogrificator.
    /// </summary>
    Transmogrificator::~Transmogrificator()
    {
        ASSERT(m_queuedProxyCount == 0);

        //
        // Get rid of everything in the cache.
        //
        PSLIST_ENTRY pLE = InterlockedFlushSList(&m_cachedProxies);
        while (pLE != NULL)
        {
            PSLIST_ENTRY pNext = pLE->Next;
            CachedTransmogrifiedPrimary *pCachedPrimary = CONTAINING_RECORD(pLE, CachedTransmogrifiedPrimary, m_cacheEntry);
            pCachedPrimary->Shutdown();
            pLE = pNext;
        }

        // Cancels the wait and ensure that all callbacks have indeed completed.

        if (m_hWaitHandle != NULL)
        {
            UnregisterWaitEx(m_hWaitHandle, INVALID_HANDLE_VALUE);
        }

        CloseHandle(m_hUnblock);
    }

    /// <summary>
    ///     Callback handler for m_hUnblock
    /// </summary>
    void CALLBACK Transmogrificator::TransmogrificationHandler(PVOID parameter, BOOLEAN)
    {
        Transmogrificator * pTransmogrificator = reinterpret_cast<Transmogrificator *>(parameter);
        pTransmogrificator->BeginTransmogrifying();
    }


    /// <summary>
    ///     Performs a transmogrification of pProxy.  The Transmogrified primary which is created will not start until
    ///     UnblockTransmogrification is called.
    /// </summary>
    /// <param name="pProxy">
    ///     The thread proxy which is being transmogrified.
    /// </param>
    void Transmogrificator::PerformTransmogrification(UMSThreadProxy *pProxy)
    {
        //
        // This **CANNOT** do anything that cannot be done in a HyperCritical region or on an arbitrary primary!  Try to grab one off the cache
        // before we defer to the transmogrificator's thread.
        //
        PSLIST_ENTRY pEntry = InterlockedPopEntrySList(&m_cachedProxies);
        if (pEntry != NULL)
        {
            InterlockedDecrement(&m_cacheCount);

            //
            // This does not need a fence as there's no race since we cannot unblock it until after this call returns.
            //
            pProxy->m_pTransmogrification = CONTAINING_RECORD(pEntry, CachedTransmogrifiedPrimary, m_cacheEntry);
        }
        else
        {
            //
            // There's nothing on the cache.  We cannot perform *ANY* allocation or creation of threads here.  We must go back to the transmogrificator's
            // thread.
            //
            m_queuedProxies.AddTail(&(pProxy->m_transmogrificatorPendingQueue));
            if (InterlockedIncrement(&m_queuedProxyCount) == 1)
                SetEvent(m_hUnblock);
        }
    }

    /// <summary>
    ///     Unblocks the transmogrification which was created in PerformTransmogrification.  Note that PerformTransmogrification
    ///     must be called first!
    /// </summary>
    /// <param name="pProxy">
    ///     The thread proxy whose transmogrification is being unblocked.
    /// </param>
    void Transmogrificator::UnblockTransmogrification(UMSThreadProxy *pProxy)
    {
        //
        // CAS in a magic constant indicating an immediate unblock.  If we pull out the primary, we unblock it.  If the other side pulls out
        // they unblock.
        //
        TransmogrifiedPrimary *pTransmogrifiedPrimary = reinterpret_cast<TransmogrifiedPrimary *>(
            InterlockedCompareExchangePointer((volatile PVOID *)&pProxy->m_pTransmogrification, (PVOID)TRANSMOGRIFICATION_UNBLOCKED, (PVOID)NULL)
            );

        if (pTransmogrifiedPrimary != NULL)
            pTransmogrifiedPrimary->QueueToCompletion(pProxy);
    }

    /// <summary>
    ///     The thread function which awakens when necessary to bind proxies which wish to be transmogrified with new
    ///     TransmogrifiedPrimary objects.
    /// </summary>
    void Transmogrificator::BeginTransmogrifying()
    {
        do
        {
            ListEntry *pLE = m_queuedProxies.RemoveHead();

            UMSThreadProxy *pProxy = CONTAINING_RECORD(pLE, UMSThreadProxy, m_transmogrificatorPendingQueue);
            TransmogrifiedPrimary *pTransmogrifiedPrimary = _concrt_new CachedTransmogrifiedPrimary(this);
            if ((PVOID)InterlockedExchangePointer((volatile PVOID *)&pProxy->m_pTransmogrification, pTransmogrifiedPrimary) == (PVOID)TRANSMOGRIFICATION_UNBLOCKED)
                pTransmogrifiedPrimary->QueueToCompletion(pProxy);

        } while (InterlockedDecrement(&m_queuedProxyCount) > 0);
    }

    /// <summary>
    ///     Called in order to return a cached transmogrified primary to the transmogrificator.
    /// </summary>
    void Transmogrificator::ReturnToCache(CachedTransmogrifiedPrimary *pTransmogrifiedPrimary)
    {
        if (m_cacheCount >= TRANSMOGRIFICATOR_CACHE_DEPTH)
            pTransmogrifiedPrimary->Shutdown();
        else
        {
            InterlockedIncrement(&m_cacheCount);
            InterlockedPushEntrySList(&m_cachedProxies, &(pTransmogrifiedPrimary->m_cacheEntry));
        }
    }

} // namespace details
} // namespace Concurrency
