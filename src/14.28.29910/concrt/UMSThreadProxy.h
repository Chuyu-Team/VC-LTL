// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadProxy.h
//
// Proxy for a UMS thread.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#if defined(_DEBUG)
#define UMS_DEBUGBIT_CRITICALBLOCK 0x00000001
#define UMS_DEBUGBIT_BLOCKED 0x00000002
#define UMS_DEBUGBIT_COMPLETION 0x00000004
#define UMS_DEBUGBIT_TRANSFERLIST 0x00000008
#define UMS_DEBUGBIT_CRITICALNOTIFY 0x00000010
#define UMS_DEBUGBIT_SKIPPEDCOMPLETION 0x00000020
#define UMS_DEBUGBIT_HANDEDTOPOLLER 0x00000040
#define UMS_DEBUGBIT_PULLEDFROMTRANSFERLIST 0x00000080
#define UMS_DEBUGBIT_POLLERFOUNDCOMPLETION 0x00000100
#define UMS_DEBUGBIT_YIELDED 0x00000200
#define UMS_DEBUGBIT_FREELIST 0x00000400
#define UMS_DEBUGBIT_CRITICALAWAKENING 0x00000800
#define UMS_DEBUGBIT_DEACTIVATEDPROXY 0x00001000
#define UMS_DEBUGBIT_ACTIVATEDPROXY 0x00002000
#define UMS_DEBUGBIT_COMPLETIONTERMINATED 0x00004000
#define UMS_DEBUGBIT_EXECUTEFAILURESUSPENDED 0x80000000
#define UMS_DEBUGBIT_EXECUTEFAILURETERMINATED 0x40000000
#endif // _DEBUG

// A magic value for m_pTransmogrification that indicates it should be immediately unblocked when defer created.
#define TRANSMOGRIFICATION_UNBLOCKED ((PVOID)1)

namespace Concurrency
{
namespace details
{
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class UMSThreadProxy : public UMSBaseObject, public ::Concurrency::IUMSThreadProxy, public ::Concurrency::IUMSUnblockNotification
    {
    public:
        /// <summary>
        ///     Constructs a thread proxy.
        /// </summary>
        /// <param name="pFactory">
        ///     The thread proxy factory that created this thread proxy, and maintains the idle pool of thread proxies.
        /// </param>
        /// <param name="pStartupList">
        ///     The initial UMS completion list that this UMS thread should be put on upon creation.  A primary cannot schedule
        ///     the thread until it has appeared on the specified completion list.
        /// </param>
        /// <param name="stackSize">
        ///     The stack size of the created thread.
        /// </param>
        UMSThreadProxy(IThreadProxyFactory * pFactory, PUMS_COMPLETION_LIST pStartupList, unsigned int stackSize);

        /// <summary>
        ///     Destroys a thread proxy.
        /// </summary>
        virtual ~UMSThreadProxy();

        /// <summary>
        ///     Retrieves a process unique id for the thread proxy.
        /// </summary>
        unsigned int GetId() const;

        /// <summary>
        ///     Gets the stack size of the thread proxy. Multiply by 1 KB to get actual stack size in bytes.
        /// </summary>
        unsigned int GetStackSize()
        {
            return m_stackSize;
        }

        /// <summary>
        ///     Cancels the thread proxy causing the underlying thread to exit.
        /// </summary>
        void Cancel();

        /// <summary>
        ///     Returns the virtual processor root the thread proxy is running on.
        /// </summary>
        UMSFreeVirtualProcessorRoot *GetVirtualProcessorRoot()
        {
            return m_pRoot;
        }

        /// <summary>
        ///     Returns the previous virtual processor root the thread proxy is running on.  If it is running, the current one is returned.
        /// </summary>
        UMSFreeVirtualProcessorRoot *GetLastVirtualProcessorRoot()
        {
            return m_pLastRoot;
        }

        /// <summary>
        ///     Returns a Win32 handle to the thread that is backing this proxy.
        /// </summary>
        HANDLE GetThreadHandle()
        {
            return m_hPhysicalContext;
        }

        /// <summary>
        ///     Returns our understanding of a UMS context (a UMS thread proxy) from the UMS system's understanding (a UMS_CONTEXT)
        /// </summary>
        /// <param name="pUMSContext">
        ///     UMS context
        /// </param>
        /// <returns>
        ///     UMS thread proxy
        /// </returns>
        static UMSThreadProxy *FromUMSContext(PUMS_CONTEXT pUMSContext);

        /// <summary>
        ///     Returns our understanding of a UMS context (a UMS thread proxy) from the SLIST_ENTRY used to place it on an SLIST (e.g.: the transfer list).
        /// </summary>
        /// <param name="pListEntry">
        ///     Pointer to the SList entry in the thread proxy
        /// </param>
        /// <returns>
        ///     UMS thread proxy
        /// </returns>
        static UMSThreadProxy *FromListEntry(PSLIST_ENTRY pListEntry);

        /// <summary>
        ///     Returns the current UMS context.
        /// </summary>
        /// <returns>
        ///     Current UMS thread proxy
        /// </returns>
        static UMSThreadProxy *GetCurrent()
        {
            return UMSThreadProxy::FromUMSContext(UMS::GetCurrentUmsThread());
        }

        /// <summary>
        ///     Returns the PUMS_CONTEXT associated with this UMS thread.
        /// </summary>
        /// <returns>
        ///     UMS context for this thread
        /// </returns>
        PUMS_CONTEXT GetUMSContext() const
        {
            return m_pUMSContext;
        }

        /// <summary>
        ///     Returns whether the action that this thread took to get itself "unscheduled" was a message/yield pair.  This could be an explicit
        ///     SwitchTo, a Deactivate, a pooling, etc...
        /// </summary>
        /// <returns>
        ///     Returns true if the thread last yielded to the primary
        /// </returns>
        bool MessagedYield()
        {
            return (m_yieldAction != ActionNone);
        }

        /// <summary>
        ///     Enters a critical region within the resource manager.  This definition is constrained to the resource manager's definition
        ///     of a critical region.  While inside a critical region, asynchronous suspensions (page faults, etc...) are hidden from view
        ///     of the underlying scheduling code (within the RM and the scheduler).
        /// </summary>
        /// <returns>
        ///     Returns the current critical region count
        /// </returns>
        virtual int EnterCriticalRegion()
        {
            // Entering a critical region should be done on the same thread
            CONCRT_COREASSERT(UMSThreadProxy::GetCurrent() == this);

            return(++m_criticalRegionCount);
        }

        /// <summary>
        ///     Exits a critical region within the resource manager.   This definition is constrained to the resource manager's definition
        ///     of a critical region.  While inside a critical region, asynchronous suspensions (page faults, etc...) are hidden from view
        ///     of the underlying scheduling code (within the RM and the scheduler).
        /// </summary>
        /// <returns>
        ///     Returns the current critical region count
        /// </returns>
        virtual int ExitCriticalRegion()
        {
            CONCRT_COREASSERT(m_criticalRegionCount > 0);
            return(--m_criticalRegionCount);
        }

        /// <summary>
        ///     Identical to EnterHyperCriticalRegion -- this is a private version for utilization within the RM that does not follow the
        ///     "on-the-owning" thread rule.
        /// </summary>
        /// <returns>
        ///     Returns the current hypercritical region count
        /// </returns>
        int ForceEnterHyperCriticalRegion()
        {
            m_criticalRegionCount++;
            return(++m_hyperCriticalRegionCount);
        }

        /// <summary>
        ///     Enters a hyper-critical region within the resource manager.  This definition is constrained to the resource manager's definition
        ///     of a hyper-critical region.  When inside a hyper-critical region, any suspension is hidden from view of the underlying scheduling
        ///     code.  This requires EXTRA care with regards to the locks which are taken inside the region.  No lock taken inside the region
        ///     can be shared with code outside such a region!
        /// </summary>
        /// <returns>
        ///     Returns the current hypercritical region count
        /// </returns>
        virtual int EnterHyperCriticalRegion()
        {
            // Entering a critical region should be done on the same thread
            CONCRT_COREASSERT(UMSThreadProxy::GetCurrent() == this);

            return ForceEnterHyperCriticalRegion();
        }

        /// <summary>
        ///     Exit a hyper-critical region within the resource manager.  This definition is constrained to the resource manager's definition
        ///     of a hyper-critical region.  When inside a hyper-critical region, any suspension is hidden from view of the underlying scheduling
        ///     code.  This requires EXTRA care with regards to the locks which are taken inside the region.  No lock taken inside the region
        ///     can be shared with code outside such a region!
        /// </summary>
        /// <returns>
        ///     Returns the current hypercritical region count
        /// </returns>
        virtual int ExitHyperCriticalRegion()
        {
            CONCRT_COREASSERT(m_hyperCriticalRegionCount > 0);
            CONCRT_COREASSERT(m_criticalRegionCount > 0);
            int val = --m_hyperCriticalRegionCount;
            m_criticalRegionCount--;

            return val;
        }

        /// <summary>
        ///     Returns whether or not this UMS thread proxy is within a critical region (either inside the RM or inside the scheduler).
        ///     Note that the scheduler's definition of a critical region may be stronger than ours.
        /// </summary>
        /// <returns>
        ///     Whether or not this UMS thread proxy is within a critical region.
        /// </returns>
        virtual CriticalRegionType GetCriticalRegionType() const
        {
            if (m_hyperCriticalRegionCount > 0)
                return InsideHyperCriticalRegion;
            if (m_criticalRegionCount > 0)
                return InsideCriticalRegion;

            return OutsideCriticalRegion;
        }

        /// <summary>
        ///     Clears the critical region bits.  This should be done before reusing the context or placing it on the idle pool as certain operations
        ///     require a "until the end of time" critical region.
        /// </summary>
        void ClearCriticalRegion()
        {
            m_criticalRegionCount = m_hyperCriticalRegionCount = 0;
        }

        /// <summary>
        ///     Gets the priority of the thread proxy.
        /// </summary>
        /// <returns>
        ///     The priority of the thread proxy
        /// </returns>
        int GetPriority()
        {
            return m_threadPriority;
        }

        /// <summary>
        ///     Sets the priority of the underlying thread.
        /// </summary>
        /// <param name="priority">
        ///     The new priority value for the thread.
        /// </param>
        void SetPriority(int priority);

        /// <summary>
        ///     Notifies the thread proxy that it is blocked (due to UMS activation of the primary).
        /// </summary>
        /// <param name="fCritically">
        ///     Indication that this thread is critically blocked (inside critical region)
        /// </param>
        void NotifyBlocked(bool fCritically)
        {
            CONCRT_COREASSERT(m_pRoot != NULL);
#if defined(_DEBUG)
            CONCRT_COREASSERT((m_UMSDebugBits & (UMS_DEBUGBIT_DEACTIVATEDPROXY | UMS_DEBUGBIT_ACTIVATEDPROXY)) != UMS_DEBUGBIT_DEACTIVATEDPROXY);
#endif // _DEBUG

            m_blockingType = fCritically ? BlockingCritical : BlockingNormal;
            if (!fCritically)
                InterlockedExchangePointer((volatile PVOID*)&m_pRoot, (PVOID)NULL);
        }

        /// <summary>
        ///     Notify that this proxy is ready to be transmogrified.
        /// </summary>
        void NotifyTransmogrification();

        /// <summary>
        ///     Returns whether or not this thread is currently suspended.
        /// </summary>
        /// <returns>
        ///     Returns true if the thread is suspended
        /// </returns>
        bool IsSuspended();

        /// <summary>
        ///     Returns whether or not this thread is currently terminated.
        /// </summary>
        /// <returns>
        ///     Returns true if the thread is terminated
        /// </returns>
        bool IsTerminated();

        /// <summary>
        ///     Returns the next context which was pulled off the UMS completion list.  This call can only be made until one of these objects is
        ///     rescheduled.  After that, the results of the call become undefined.
        /// </summary>
        /// <returns>
        ///     Pointer to the unblock notification (context).
        /// </returns>
        IUMSUnblockNotification *GetNextUnblockNotification()
        {
            PSLIST_ENTRY psle = m_listEntry.Next;
            UMSThreadProxy *pProxy = psle == NULL ? NULL : CONTAINING_RECORD(psle, UMSThreadProxy, m_listEntry);
            return pProxy;
        }

        /// <summary>
        ///     Indicates whether this is a primary or not.
        /// </summary>
        /// <returns>
        ///     True if this is the primary thread, false otherwise
        /// </returns>
        virtual bool IsPrimary()
        {
            return false;
        }

#if defined(_DEBUG)

        /// <summary>
        ///     Tells the proxy it's no longer shutting down a virtual processor root and normal lock validations apply.
        /// </summary>
        void ClearShutdownValidations()
        {
            m_fShutdownValidations = false;
        }

        /// <summary>
        ///     Tells the proxy it's shutting down a virtual processor root and normal lock validations don't apply.
        /// </summary>
        void SetShutdownValidations()
        {
            m_fShutdownValidations = true;
        }

        /// <summary>
        ///     Returns whether or not the proxy is in a "shutting down a virtual processor root" mode where normal lock validations don't apply.
        /// </summary>
        bool IsShutdownValidations() const
        {
            return m_fShutdownValidations;
        }
#endif // _DEBUG

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

        /// <summary>
        ///     Returns whether this proxy is transmogrified (e.g.: running on its own dedicated primary to simulate being an NT thread).
        ///     This call is only valid on the *CURRENT PROXY* otherwise the answer may be stale at the moment of return.
        /// </summary>
        /// <returns>
        ///     Ture if this proxy is transmogrified.
        /// </returns>
        bool IsTransmogrified() const
        {
            return m_pTransmogrification != NULL;
        }

        /// <summary>
        ///     A scoped critical region.
        /// </summary>
        class ScopedCriticalRegion
        {
        public:

            ScopedCriticalRegion()
            {
                m_pProxy = UMSThreadProxy::GetCurrent();
                if (m_pProxy != NULL)
                    m_pProxy->EnterCriticalRegion();
            }

            ScopedCriticalRegion(UMSThreadProxy *pProxy) :
                m_pProxy(pProxy)
            {
                if (m_pProxy != NULL)
                    m_pProxy->EnterCriticalRegion();
            }

            ~ScopedCriticalRegion()
            {
                if (m_pProxy != NULL)
                    m_pProxy->ExitCriticalRegion();
            }

        private:

            UMSThreadProxy *m_pProxy;
        };

        /// <summary>
        ///     A scoped hyper-critical region.
        /// </summary>
        class ScopedHyperCriticalRegion
        {
        public:

            ScopedHyperCriticalRegion()
            {
                m_pProxy = UMSThreadProxy::GetCurrent();
                if (m_pProxy != NULL)
                    m_pProxy->EnterHyperCriticalRegion();
            }

            ScopedHyperCriticalRegion(UMSThreadProxy *pProxy) :
                m_pProxy(pProxy)
            {
                if (m_pProxy != NULL)
                    m_pProxy->EnterHyperCriticalRegion();
            }

            ~ScopedHyperCriticalRegion()
            {
                if (m_pProxy != NULL)
                    m_pProxy->ExitHyperCriticalRegion();
            }

        private:

            UMSThreadProxy *m_pProxy;

        };

    protected:

        // The thread proxy factory that created this thread proxy, and maintains the idle pool of thread proxies.
        IThreadProxyFactory * m_pFactory;

        // The UMS context for the underlying UT.
        PUMS_CONTEXT m_pUMSContext;

        // The OS handle for the underlying UT.
        HANDLE m_hPhysicalContext;

        // The blocking handle.
        HANDLE m_hBlock;

        enum BlockingType
        {
            // No blocking registered.
            BlockingNone,

            // Normal blocking
            BlockingNormal,

            // Critical blocking (e.g.: masking a page fault)
            BlockingCritical
        };

        // How we last blocked.
        volatile BlockingType m_blockingType;

        // The virtual processor root on which this thread proxy is executing.
        UMSFreeVirtualProcessorRoot * volatile m_pRoot;

        // The last virtual processor root on which this thread proxy was executing.
        UMSFreeVirtualProcessorRoot *m_pLastRoot;

        // Stores the stack size of the thread proxy. Multiply by 1 KB to get actual stack size in bytes.
        unsigned int m_stackSize;

        // Stores the last priority value that was set on the thread. Initial value is normal priority.
        int m_threadPriority;

        // An indication as to whether this thread proxy was idle pooled.
        bool m_fIdlePooled;

        // **************************************************
        // Messaging Block:
        //
        // Everything between these markers is a messaging block between the UT and the primary.  These fields may *ONLY* be touched under
        // guardianship of a hyper-critical region.
        // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

        // *************************
        // Input to the primary:

        // The action that the primary is to take.
        enum YieldAction
        {
            // No message
            ActionNone = 0,

            // Switch to a new context.  The old one is still in use.
            ActionSwitchTo,

            // Switch to a new context and retire the old one to a free pool.
            ActionSwitchToAndRetire,

            // Perform a transmogrification of the context (start it on the transmogrified primary) and switch to a provided context or the scheduling context
            ActionTransmogrifyAndSwitch,

            // Perform a transmogrification of the context (start it on the transmogrified primary) and rest the primary.
            ActionTransmogrifyAndReset,

            // Deactivate the primary
            ActionDeactivate,

            // Get rid of the thread
            ActionFree,

            // Reset the primary on a SwitchOut
            ActionResetForSwitchOut,

            // UT Startup
            ActionStartup,

            // Yield to the operating system (SwitchToThread)
            ActionYieldToSystem

        } m_yieldAction;

        // The next thread proxy that should be executed.
        UMSThreadProxy *m_pNextProxy;

        // The primary which will transmogrify this UMS thread into a "virtual"-thread.
        TransmogrifiedPrimary * volatile m_pTransmogrification;

        // *************************
        // Output from the primary:

        // Why Deactivate returned.
        enum ActivationCause
        {
            ActivationCauseNone = 0,
            ActivationCauseActivate,
            ActivationCauseCompletionNotification
        };

        // Results of messaged calls to the primary are marshaled back to the UT here.
        union
        {
            // Indicates why Deactivate returned.
            ActivationCause m_activationCause;
        };

        // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        // End Messaging Block:
        // **************************************************

        bool m_fSuspended;
        volatile LONG m_fCanceled;

        /// <summary>
        ///     Called in order to prepare this thread proxy to run atop a given virtual processor root.
        /// </summary>
        void PrepareToRun(UMSFreeVirtualProcessorRoot *pRoot);

        /// <summary>
        ///     Spins until we've registered blocking.
        /// </summary>
        void SpinUntilBlocked();

        /// <summary>
        ///     Spins until we've registered what blocking type the last block operation was and returns the type.
        /// </summary>
        BlockingType SpinOnAndReturnBlockingType();

        /// <summary>
        ///     Switch from the current thread proxy to pProxy.
        /// </summary>
        /// <param name="pProxy">
        ///     The thread proxy to switch to.
        /// </param>
        /// <param name="switchState">
        ///     The reason for the switch.
        /// </param>
        void InternalSwitchTo(UMSThreadProxy *pProxy, SwitchingProxyState switchState);

        /// <summary>
        ///     Switch out the current thread proxy.
        /// </summary>
        /// <param name="switchState">
        ///     The reason for the switch.
        /// </param>
        void InternalSwitchOut(SwitchingProxyState switchState);

        /// <summary>
        ///     Yield to the underlying Operating system
        /// </summary>
        void InternalYieldToSystem();

        /// <summary>
        ///     Deactivate the current thread proxy.  As this requires a message block set, it lives here
        ///     rather than in the VPROOT.
        /// </summary>
        /// <returns>
        ///     An indication of whether the awakening was due to an Activate call on the virtual processor root
        ///     (true) or an RM cause (e.g.: completion notification -- false).
        /// </returns>
        bool Deactivate();

        /// <summary>
        ///     The caller has exited the dispatch loop.  Free the thread and deactivate.
        /// <summary>
        void FreeViaExit();

        /// <summary>
        ///     Request this proxy to be transmogrified.
        /// </summary>
        void RequestTransmogrification();

    private:
        friend class UMSFreeVirtualProcessorRoot;
        friend class UMSSchedulerProxy;
        friend class UMSBackgroundPoller;
        friend class Transmogrificator;
        friend class TransmogrifiedPrimary;

        friend class UMSFreeThreadProxyFactory;
        template<class T> friend class SQueue;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

        // Intrusive Link Field
        UMSThreadProxy *m_pNext;

        // Process wide unique identifier.
        unsigned int m_id;

        // Tracks the depth of critical region entry within the RM.
        DWORD m_criticalRegionCount;

        // Tracks the depth of hyper-critical region entry within the RM.
        DWORD m_hyperCriticalRegionCount;

        // Thread id.
        DWORD m_threadId;

        // Link chain for the transfer list.
        SLIST_ENTRY m_listEntry;

        // The background poller list chain.
        UMSBackgroundPollerEntry m_backgroundPollerEntry;

        // The chain entries for transmogrification.
        ListEntry m_transmogrificatorPendingQueue;

        // The background poller notification proxy.
        UMSSchedulerProxy *m_pPollInsertionProxy;

        // BEGIN - USED IN DEBUG ONLY

        __int64 m_lastRunPrepareTimeStamp;
        DWORD m_UMSDebugBits;
        DWORD m_UMSLastExecuteError;

        bool m_fShutdownValidations;

        // END - USED IN DEBUG ONLY

        /// <summary>
        /// Indicate that the thread proxy is ready for dispatch.
        /// </summary>
        void ReadyForDispatch();

        /// <summary>
        ///     Dispatch routine for thread proxies.
        /// </summary>
        virtual void Dispatch() = 0;

        /// <summary>
        ///     Thread start routine for proxies.
        /// </summary>
        static DWORD CALLBACK UMSThreadProxyMain(LPVOID lpParameter);

    };
#pragma warning(pop)

} // namespace details
} // namespace Concurrency
