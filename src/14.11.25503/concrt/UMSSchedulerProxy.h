// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSSchedulerProxy.h
//
// RM proxy for a UMS scheduler instance
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    #pragma warning(push)
    #pragma warning(disable: 4265) // non-virtual destructor in base class
    class UMSSchedulerProxy : public IUMSCompletionList, public SchedulerProxy
    {
    public:

        /// <summary>
        ///     Constructs a new scheduler proxy for a UMS scheduler.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler in question.
        /// </param>
        /// <param name="pResourceManager">
        ///     The resource manager instance.
        /// </param>
        UMSSchedulerProxy(IScheduler *pScheduler, ResourceManager *pResourceManager, const SchedulerPolicy &policy);

        /// <summary>
        ///     Destroys a scheduler proxy for a UMS scheduler.
        /// </summary>
        ~UMSSchedulerProxy();

        /// <summary>
        ///     Returns the UMS completion list for this scheduler.
        /// </summary>
        /// <returns>
        ///     The UMS completion list for this scheduler.
        /// </returns>
        PUMS_COMPLETION_LIST GetCompletionList() const
        {
            return m_pCompletionList;
        }

        /// <summary>
        ///     Returns the UMS completion list event handle for this scheduler's completion list.
        /// </summary>
        /// <returns>
        ///     The UMS completion list event handle for this scheduler's completion list.
        /// </returns>
        HANDLE GetCompletionListEvent() const
        {
            return m_hCompletionListEvent;
        }

        /// <summary>
        ///     Returns the transfer list event handle for this scheduler's transfer list.  Note that this may *NOT* exist and
        ///     may return NULL.  Only if the RM is in a mode where it will push things back on the transfer list will this
        ///     exist.
        /// </summary>
        HANDLE GetTransferListEvent() const
        {
            return m_hTransferListEvent;
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
        bool SweepCompletionList(UMSThreadProxy *pSought = NULL, bool fWait = false);

        /// <summary>
        ///     Pops a single item off the transfer list.  This is *ONLY* allowed to be utilized from the primary.
        /// </summary>
        /// <returns>
        ///     A single item off the transfer list or NULL if no such item exists.
        /// </returns>
        UMSThreadProxy *PopTransferListItem()
        {
            return UMSThreadProxy::FromListEntry(InterlockedPopEntrySList(&m_transferList));
        }

        /// <summary>
        ///     Returns all items on the UMS completion list.  Note that this indirectly manipulates the transfer list.  This
        ///     will literally dequeue everything from BOTH lists and hand it to the caller.
        /// </summary>
        /// <returns>
        ///     The first UMSThreadProxy on the completion list.
        /// </returns>
        UMSThreadProxy *GetCompletionListItems();

        /// <summary>
        ///     Returns all the items on the transfer list.  This is the public (e.g.: scheduler facing) mechanism for querying
        ///     the items on the completion list.
        /// </summary>
        /// <returns>
        ///     A chain of IUMSUnblockNotification interfaces representing all unblocked contexts.
        /// </returns>
        virtual IUMSUnblockNotification *GetUnblockNotifications()
        {
            return GetCompletionListItems();
        }

        /// <summary>
        ///     Create a virtual processor root and add it to the scheduler proxys list of roots.
        /// </summary>
        virtual VirtualProcessorRoot* CreateVirtualProcessorRoot(SchedulerNode* pNode, unsigned int coreIndex);

        /// <summary>
        ///     Causes the UMS background poller to poll for pProxy's completion at its specified rate and insert it on the completion
        ///     list when it detects that it's runnable.  Note that it really goes on the transfer list, but that's really our abstraction
        ///     of completion anyway.
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy to poll for completion of.
        /// </param>
        void PollForCompletion(UMSThreadProxy *pProxy)
        {
            UMSFreeThreadProxyFactory * pFactory = static_cast<UMSFreeThreadProxyFactory *>(m_pThreadProxyFactory);
            UMSBackgroundPoller * pPoller = pFactory->GetUMSBackgroundPoller();

            CONCRT_COREASSERT(pPoller != NULL);
            pPoller->InsertProxyInCompletionWhenAwake(pProxy, this);
        }

        /// <summary>
        ///     Adds a reference to the scheduler proxy.
        /// </summary>
        LONG Reference()
        {
            return InterlockedIncrement(&m_refCount);
        }

        /// <summary>
        ///     Releases a reference on the scheduler proxy.
        /// </summary>
        LONG Release()
        {
            LONG refCount = InterlockedDecrement(&m_refCount);
            if (refCount == 0)
                delete this;
            return refCount;
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
        virtual void BindContext(IExecutionContext *pContext);

        /// <summary>
        ///     Returns an **unstarted** thread proxy attached to pContext, to the thread proxy factory.
        ///     Such a thread proxy **must** be unstarted.
        ///     This API should *NOT* be called in the vast majority of circumstances.
        /// </summary>
        /// <param name="pContext">
        ///     The context to unbind.
        /// </param>
        void UnbindContext(IExecutionContext *pContext);

        /// <summary>
        ///     Gets a new thread proxy from the factory.
        /// </summary>
        virtual IThreadProxy * GetNewThreadProxy(IExecutionContext * pContext);

    protected:

        /// <summary>
        ///     Deletes the scheduler proxy.
        /// </summary>
        virtual void DeleteThis()
        {
            Release();
        }

    private:

        friend class UMSBackgroundPoller;

        // The UMS completion list RM hosts for the scheduler.
        PUMS_COMPLETION_LIST m_pCompletionList;

        // The UMS completion list event for m_pCompletionList.
        HANDLE m_hCompletionListEvent;

        // The UMS transfer list event.
        HANDLE m_hTransferListEvent;

        // The transfer list.
        SLIST_HEADER m_transferList; // must be 16-bye aligned in x64

        // The count of items pushed back to the transfer list since the last grab.
        volatile LONG m_pushedBackCount;

        // The reference count on the scheduler proxy.  Unlike thread scheduler proxies, the UMS scheduler proxy must live until
        // all the primaries have exited (because completion and transfer lists are stored here and are necessary for the primaries
        // to run).  Hence, destruction and cleanup of the UMS side of the proxy is done by reference count.
        volatile LONG m_refCount;

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
        bool HandleCompletion(UMSThreadProxy *pCompletion);

        /// <summary>
        ///     Called in order to push a polled completion onto the transfer list.  This should only be called from the background poller which
        ///     is quite intimate with the rest of the UMS data structures.
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy which the background poller has noticed is now awake and should be on the "completion" list.
        /// </param>
        void PushPolledCompletion(UMSThreadProxy *pProxy);
    };

    #pragma warning(pop)
} // namespace details
} // namespace Concurrency
