// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSBackgroundPoller.h
//
// A background thread responsible for polling for contexts which had a failed ExecuteUmsThread and inject them on our
// abstraction of a completion list.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     An entry in the UMS background poller.  Anything wanting to be inserted must have an intrusive copy of this
    ///     data structure.
    /// </summary>
    struct UMSBackgroundPollerEntry
    {
        // The link within
        ListEntry m_link;

        // A timestamp at the start of polling.
        __int64 m_pollStart;

        // The number of times that the poller
        LONG m_pollCount;
    };

    /// <summary>
    ///     The object responsible for polling for completions of threads which when executed were suspended or otherwise occupied.
    ///     The UMS system will never return these to the completion list and threads such as these must be polled.  The RM polls
    ///     in a central location to isolate these types of issues from the scheduler.  This is that central location.
    /// </summary>
    class UMSBackgroundPoller
    {
    public:

        /// <summary>
        ///     Constructs a new UMS background poller.
        /// </summary>
        /// <param name="pollingPeriod">
        ///     How often (in milliseconds) the background thread should poll for awakenings.
        /// </param>
        UMSBackgroundPoller();

        /// <summary>
        ///     Destructs a UMS background poller.
        /// </summary>
        ~UMSBackgroundPoller();

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
        void InsertProxyInCompletionWhenAwake(UMSThreadProxy *pThreadProxy, UMSSchedulerProxy *pSchedulerProxy);

        /// <summary>
        ///     Poll the list of proxies that failed to execute and inserts them into the appropriate transfer
        ///     lists if they are resumed/ready to run.
        /// </summary>
        bool DoPolling();

        /// <summary>
        ///     Get the event that is signaled when a new proxy is added for polling
        /// </summary>
        HANDLE GetEvent()
        {
            return m_hNotification;
        }

        /// <summary>
        ///     Polling interval
        /// </summary>
        static int PollInterval()
        {
            // 100 ms
            return 100;
        }

    private:

        // Notification that there are pollers to poll.  The background poller stays asleep otherwise.
        HANDLE m_hNotification;

        // The count of pollers.
        volatile LONG m_pollCount;

        // The thread proxies to poll
        SafeRWList<ListEntry> m_pollProxies;
    };

} // namespace details
} // namespace Concurrency
