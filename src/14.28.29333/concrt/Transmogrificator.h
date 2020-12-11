// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Transmogrificator.h
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

namespace Concurrency
{
namespace details
{

//
// The maximum number of threads we will cache on the side to perform transmogrifications for nesting within a UMS scheduler.  These
// are *NOT* pre-created, but they are cached up to this depth once they are created and done with.
//
#define TRANSMOGRIFICATOR_CACHE_DEPTH 8

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class Transmogrificator
    {
    public:

        /// <summary>
        ///     Constructs a new Transmogrificator.
        /// </summary>
        Transmogrificator();

        /// <summary>
        ///     Destructs the Transmogrificator.
        /// </summary>
        ~Transmogrificator();

        /// <summary>
        ///     Performs a transmogrification of pProxy.  The Transmogrified primary which is created will not start until
        ///     UnblockTransmogrification is called.
        /// </summary>
        /// <param name="pProxy">
        ///     The thread proxy which is being transmogrified.
        /// </param>
        void PerformTransmogrification(UMSThreadProxy *pProxy);

        /// <summary>
        ///     Unblocks the transmogrification which was created in PerformTransmogrification.  Note that PerformTransmogrification
        ///     must be called first!
        /// </summary>
        /// <param name="pProxy">
        ///     The thread proxy whose transmogrification is being unblocked.
        /// </param>
        void UnblockTransmogrification(UMSThreadProxy *pProxy);


        /// <summary>
        ///     Called in order to return a cached transmogrified primary to the transmogrificator.
        /// </summary>
        void ReturnToCache(CachedTransmogrifiedPrimary *pTransmogrifiedPrimary);

    private:

        /// <summary>
        ///     The thread function which awakens when necessary to bind proxies which wish to be transmogrified with new
        ///     TransmogrifiedPrimary objects.
        /// </summary>
        void BeginTransmogrifying();

        /// <summary>
        ///     Callback handler for m_hUnblock
        /// </summary>
        static void CALLBACK TransmogrificationHandler(PVOID parameter, BOOLEAN is_timeout);

        // Handle to the transmogrificator thread.
        HANDLE m_hThread;

        // Handle to the wait in RegisterWaitForSingleObject
        HANDLE m_hWaitHandle;

        // Notification that there are pollers to poll.  The background poller stays asleep otherwise.
        HANDLE m_hUnblock;

        // The count of proxies queued for transmogrification.
        volatile LONG m_queuedProxyCount;

        // The list of proxies that haven't yet gotten a transmogrified primary.
        SafeRWList<ListEntry> m_queuedProxies;

        // The number of cached proxies and the lock-free list.
        volatile LONG m_cacheCount;
        SLIST_HEADER m_cachedProxies; // must be 16-byte aligned in x64

    };
#pragma warning(pop)

} // namespace details
} // namespace Concurrency
