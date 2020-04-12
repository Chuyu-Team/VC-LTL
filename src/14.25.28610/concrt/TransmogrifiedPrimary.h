// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// TransmogrifiedPrimary.h
//
// A very special primary thread whose sole purpose is to "virtually" transmogrify a UMS thread into an NT thread
// for the purposes of scheduler nesting.  Note that this class is also reused in any circumstance where similar behavior
// is required.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A base interface for all UMS thread abstractions so that we could determine if the
    ///     current thread is a primary or a UT.
    /// </summary>
    class UMSBaseObject
    {
    public:

        virtual bool IsPrimary() = 0;
        virtual ~UMSBaseObject();
    };

    class TransmogrifiedPrimary : public UMSBaseObject
    {
    public:

        /// <summary>
        ///     Creates a new primary which transmogrifies a series of proxies (via ExecuteToCompletion/QueueToCompletion) into "virtual threads" until
        ///     they exit -- one at a time.
        /// </summary>
        TransmogrifiedPrimary();

        /// <summary>
        ///     Destroys the transmogrified primary.
        /// </summary>
        ~TransmogrifiedPrimary();

        /// <summary>
        ///     Queues a thread to execute to completion and asynchronously returns.  It is not safe to utilize this method and ExecuteToCompletion
        ///     simultaneously.
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy to queue to completion.
        /// </param>
        void QueueToCompletion(UMSThreadProxy *pProxy);

        /// <summary>
        ///     Shuts down a multiple binding transmogrified primary.
        /// </summary>
        void Shutdown()
        {
            SetEvent(m_hRetire);
        }

        /// <summary>
        ///     Indicates whether this is a primary or not.
        /// </summary>
        virtual bool IsPrimary()
        {
            return true;
        }

        UMSBackgroundPoller *GetUMSBackgroundPoller()
        {
            return &m_poller;
        }

    protected:

        /// <summary>
        ///     Called when a transmogrification is complete.
        /// </summary>
        virtual void CompletedTransmogrification()
        {
        }

    private:

        // The UMS thread that this primary transmogrifies to a "virtual"-thread.
        UMSThreadProxy *m_pBoundProxy;

        // The primary thread.
        HANDLE m_hPrimary;

        // UMS Completion list event
        HANDLE m_hCompletionListEvent;

        // The block handle (we are signaled when the old primary indicates that transmogrification is complete)
        HANDLE m_hBlock;

        // The retirement handle
        HANDLE m_hRetire;

        // The TID of the primary thread.
        DWORD m_primaryId;

        // The UMS completion list for this transmogrified primary.
        PUMS_COMPLETION_LIST m_pCompletionList;

        //
        // For multiple bindings.
        //
        SafeSQueue<UMSThreadProxy> m_queuedExecutions;
        volatile LONG m_queueCount;

        //
        // For poller
        //
        UMSBackgroundPoller m_poller;

        /// <summary>
        ///     Initializes the class.
        /// </summary>
        void Initialize();

        /// <summary>
        ///     Unblocks the transmogrified primary so that it can execute the UT it is transmogrifying.
        /// </summary>
        void Unblock()
        {
            SetEvent(m_hBlock);
        }

        /// <summary>
        ///     Search for new work
        /// </summary>
        UMSThreadProxy * SearchForWork();

        /// <summary>
        ///     Waits for new work
        /// </summary>
        UMSThreadProxy * WaitForWork();

        /// <summary>
        ///     Handle blocking of a UT
        /// </summary>
        UMSThreadProxy * HandleBlocking();

        /// <summary>
        ///     Handle yielding of a UT
        /// </summary>
        UMSThreadProxy * HandleYielding();

        /// <summary>
        ///     Execute the given proxy on this primary
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy to run
        /// </param>
        void Execute(UMSThreadProxy *pProxy);

        /// <summary>
        ///     Waits for the blocked proxy to be ready
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy that is blocked
        /// </param>
        UMSThreadProxy * WaitForBlockedThread(UMSThreadProxy * pProxy);

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
        static void NTAPI PrimaryInvocation(UMS_SCHEDULER_REASON reason, ULONG_PTR activationPayload, PVOID pData);

        /// <summary>
        ///     The primary thread.
        /// </summary>
        /// <param name="pContext">
        ///     The TransmogrifiedPrimary that the primary manages.
        /// </param>
        static DWORD CALLBACK PrimaryMain(LPVOID pContext);

    };

    /// <summary>
    ///     A transmogrified primary which places itself upon the transmogrificator's cache at the end of each transmogrification.
    /// </summary>
    class CachedTransmogrifiedPrimary : public TransmogrifiedPrimary
    {
    public:

        /// <summary>
        ///     Construct a new cached transmogrified primary.  The primary will be placed back upon the cache of the specified transmogrificator
        ///     at the end of each transmogrification.
        /// </summary>
        CachedTransmogrifiedPrimary(Transmogrificator *pTransmogrificator);

    protected:

        /// <summary>
        ///     Called when a transmogrification is complete.
        /// </summary>
        virtual void CompletedTransmogrification();

    private:

        friend class Transmogrificator;

        // An entry for this primary on the transmogrificator's cache.
        SLIST_ENTRY m_cacheEntry;

        Transmogrificator *m_pTransmogrificator;
    };

} // namespace details
} // namespace Concurrency
