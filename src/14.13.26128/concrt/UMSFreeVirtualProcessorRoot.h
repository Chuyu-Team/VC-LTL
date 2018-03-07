// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSFreeVirtualProcessorRoot.h
//
// Part of the ConcRT Resource Manager -- this header file contains the internal definition for the UMS free virtual
// processor root (represents a virtual processor as handed to a scheduler).
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{

    class UMSFreeVirtualProcessorRoot : public IThreadProxy, public UMSBaseObject, public VirtualProcessorRoot
    {
    public:

        // Private helper class
        class InitialThreadParam
        {
        public:
            InitialThreadParam(UMSFreeVirtualProcessorRoot * pRoot)
            {
                m_pRoot = pRoot;
                m_hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
                if (m_hEvent == NULL)
                {
                    throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
                }
            }

            ~InitialThreadParam()
            {
                if (m_hEvent != NULL)
                {
                    CloseHandle(m_hEvent);
                }
            }

            HANDLE m_hEvent;
            UMSFreeVirtualProcessorRoot * m_pRoot;
        };

        /// <summary>
        ///     Constructs a new free virtual processor root.
        /// </summary>
        /// <param name="pSchedulerProxy">
        ///     The scheduler proxy this root is created for. A scheduler proxy holds RM data associated with an instance of
        ///     a scheduler.
        /// </param>
        /// <param name="pNode">
        ///     The processor node that this root belongs to. The processor node is one among the nodes allocated to the
        ///     scheduler proxy.
        /// </param>
        /// <param name="coreIndex">
        ///     The index into the array of cores for the processor node specified.
        /// </param>
        UMSFreeVirtualProcessorRoot(UMSSchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex);

        /// <summary>
        ///     Destroys a free virtual processor root.
        /// </summary>
        virtual ~UMSFreeVirtualProcessorRoot();

        /// <summary>
        ///     Deletes the virtual processor.
        /// </summary>
        virtual void DeleteThis();

        /// <summary>
        ///     Determines whether the virtual processor is marked for deletion.
        /// </summary>
        bool IsDeleting() const
        {
            return m_fDelete;
        }

        // **************************************************
        // IVirtualProcessorRoot:
        // **************************************************

        /// <summary>
        ///     Causes the scheduler to start running a thread proxy on the specified virtual processor root which will execute
        ///     the Dispatch method of the context supplied by pContext.
        /// </summary>
        /// <param name="pContext">
        ///     The context which will be dispatched on a (potentially) new thread running atop this virtual processor root.
        /// </param>
        virtual void Activate(::Concurrency::IExecutionContext *pContext);

        /// <summary>
        ///     Causes the thread proxy running atop this virtual processor root to temporarily stop dispatching pContext.
        /// </summary>
        /// <param name="pContext">
        ///     The context which should temporarily stop being dispatched by the thread proxy running atop this virtual processor root.
        /// </param>
        virtual bool Deactivate(::Concurrency::IExecutionContext *pContext);

        /// <summary>
        ///     Forces all data in the memory heirarchy of one processor to be visible to all other processors.
        /// </summary>
        /// <param name="pContext">
        ///     The context which is currently being dispatched by this root.
        /// </param>
        virtual void EnsureAllTasksVisible(::Concurrency::IExecutionContext *pContext);

        /// <summary>
        ///     Notify the primary that a thread on which it is critically blocked (due to asynchronous interruption in a critical region)
        ///     is now runnable.
        /// </summary>
        void CriticalNotify()
        {
            SetEvent(m_hCriticalNotificationEvent);
        }

        // **************************************************
        // IThreadProxy (for the scheduling context):
        // **************************************************

        /// <summary>
        ///     Returns a process unique identifier for the thread proxy.
        /// </summary>
        /// <returns>
        ///     The IThreadProxy id.
        /// </returns>
        virtual unsigned int GetId() const;

        /// <summary>
        ///     Called in order to perform a cooperative context switch between one context and another.  After this call, pContext will
        ///     be running atop the virtual processor root and the context which was running will not.  What happens to the context that
        ///     was running depends on the value of the reason argument.
        /// </summary>
        /// <param name="pContext">
        ///     The context to cooperatively switch to.
        /// </param>
        /// <param name="switchState">
        ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
        ///     proxy and context.
        /// </param>
        virtual void SwitchTo(IExecutionContext *pContext, SwitchingProxyState switchState);

        /// <summary>
        ///     Called in order to disassociate the currently executing context from its virtual processor root, and reinitialize the root
        ///     for future use.
        /// </summary>
        /// <param name="switchState">
        ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
        ///     proxy and context.
        /// </param>
        virtual void SwitchOut(SwitchingProxyState switchState = Blocking);

        /// <summary>
        ///     Called in order to yield to the underlying operating system. This allows the operating system to schedule
        ///     other work in that time quantum.
        /// </summary>
        virtual void YieldToSystem();

        /// <summary>
        ///     Returns whether or not this object is the primary.
        /// </summary>
        virtual bool IsPrimary()
        {
            return true;
        }

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

        /// <summary>
        ///     Returns whether or not the given context is the scheduling context.
        /// </summary>
        bool IsSchedulingContext(IExecutionContext *pContext) const
        {
            return (pContext == m_pSchedulingContext);
        }

        /// <summary>
        ///     Returns our RM.
        /// </summary>
        ResourceManager *GetResourceManager();

        /// <summary>
        ///     Returns the currently executing proxy on this virtual processor root.
        /// </summary>
        UMSThreadProxy *GetExecutingProxy()
        {
            return m_pExecutingProxy;
        }

        /// <summary>
        ///     Called in order to reset this virtual processor root to a completely quiescent state (not running anything).
        /// </summary>
        void ResetOnIdle();

    protected:

        /// <summary>
        ///     Returns our scheduler proxy (cast appropriately to a UMSSchedulerProxy).
        /// </summary>
        UMSSchedulerProxy *SchedulerProxy()
        {
            return static_cast<UMSSchedulerProxy *>(GetSchedulerProxy());
        }

    private:

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

        // A handle to the primary thread.
        HANDLE m_hPrimary;

        // Blocking handle if the thread should block.
        HANDLE m_hBlock;

        // The critical notification event.  This event is waited upon in conjunction with the completion list event
        // when the primary gets stuck in a critical region (or other piece of critical code such as startup).  When signaled,
        // it informs the primary that there it can do something.
        HANDLE m_hCriticalNotificationEvent;

        // The unique scheduling context.  This is hard bound to this virtual processor and is actually invoked on the primary thread.
        IExecutionContext *m_pSchedulingContext;

        // The UT which is running atop this primary.
        UMSThreadProxy *m_pExecutingProxy;

        // The TID of the primary thread.
        DWORD m_primaryId;

        // Process wide unique identifier.
        unsigned int m_id;

        // Variable used to manage subscription level for UMS, in addition to the activation fence which is used for both threads and UMS.
        // Additional comments in UMSFreeVirtualProcessorRoot::Deactivate.
        bool m_fWokenByScheduler;

        // Set to true when the root is activated for the first time.
        bool m_fActivated;

        // Indicates deletion needs to take place.
        bool m_fDelete;

        // Indicates the primary is started.
        bool m_fStarted;

        /// <summary>
        ///     Called in order to invoke the scheduler's scheduling context.
        /// </summary>
        /// <param name="fAsynchronous">
        ///     If invocation of this context is due to previous context blocking, then was it due to an asynchronous event (e.g.: page fault).
        ///     Otherwise, false is passed in.
        /// </param>
        void InvokeSchedulingContext(bool fAsynchronous);

        /// <summary>
        ///     Executes the specified proxy.  This can only be called from the primary thread!
        /// </summary>
        /// <param name="pProxy">
        ///     The thread proxy to execute.
        /// </param>
        /// <param name="fromSchedulingContext">
        ///     Whether the switch is happening as a result of a SwitchTo from the scheduling context.  On failure, we do not recursively reinvoke
        ///     the scheduling context, we simply return -- indicating failure.
        /// </param>
        /// <param name="fCriticalBlockAndExecute">
        ///     An indication as to whether the execution was due to the result of a critical block and subsequent execute.
        /// </param>
        /// <returns>
        ///     This does *NOT* return if execution is successful.  Any return indicates failure.
        /// </returns>
        void Execute(UMSFreeThreadProxy *pProxy, bool fromSchedulingContext, bool fCriticalBlockAndExecute);

        /// <summary>
        ///     Creates the primary thread.
        /// </summary>
        void CreatePrimary();

        /// <summary>
        ///     Starts up the primary thread.
        /// </summary>
        void StartupPrimary()
        {
            CONCRT_COREASSERT(m_fStarted == false);
            m_fStarted = true;
            SetEvent(m_hBlock);
        }

        /// <summary>
        ///     Marks a particular UMS thread proxy as running atop this UMS virtual processor root.
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy which is to run atop this virtual processor root.
        /// </param>
        /// <param name="fCriticalReexecute">
        ///     Is the affinitization due to a critical execution happening on the same vproc.
        /// </param>
        void Affinitize(UMSFreeThreadProxy *pProxy, bool fCriticalReexecute = false);

        /// <summary>
        ///     Called in order to handle a UMS thread blocking.
        /// </summary>
        /// <param name="pBlockedProxy">
        ///     The thread that is blocking.
        /// </param>
        /// <param name="fAsynchronous">
        ///     An indication of whether the blocking was due to an asynchronous event (e.g.: page fault) or a synchronous one (e.g.: calling an API
        ///     which explicitly blocked.
        /// </param>
        void HandleBlocking(UMSFreeThreadProxy *pBlockedProxy, bool fAsynchronous);

        /// <summary>
        ///     Called in order to handle a UMS thread cooperative yielding.
        /// </summary>
        /// <param name="pProxy">
        ///     The thread that is yielding.
        /// </param>
        void HandleYielding(UMSFreeThreadProxy *pProxy);

        /// <summary>
        ///     Performs a critical blocking of the primary until a specific UT appears on the completion list.  The specified UT must
        ///     be in a critical region!
        /// </summary>
        void CriticalBlockAndExecute(UMSFreeThreadProxy *pProxy);

        /// <summary>
        ///     Returns whether or not we are executing on the primary thread for this virtual processor.
        /// </summary>
        bool OnPrimary()
        {
            return (GetCurrentThreadId() == m_primaryId);
        }

        /// <summary>
        ///     Performs a deactivation of the virtual processor.  This is always called on the primary.  A user thread which deactivates must defer the
        ///     call to the primary to perform the blocking action.
        /// </summary>
        bool InternalDeactivate();

        /// <summary>
        ///     The UMS primary function.  This is invoked when the virtual processor switches into UMS scheduling mode or whenever a given
        ///     context blocks or yields.
        /// </summary>
        /// <param name="reason">
        ///     The reason for the UMS invocation.
        /// </param>
        /// <param name="activationPayload">
        ///     The activation payload (depends on reason)
        /// </param>
        /// <param name="pData">
        ///     The context (the virtual processor pointer)
        /// </param>
        static void NTAPI PrimaryInvocation(UMS_SCHEDULER_REASON reason, ULONG_PTR activationPayload, PVOID pData);

        /// <summary>
        ///     The primary thread for this UMS virtual processor.
        /// </summary>
        /// <param name="pContext">
        ///     The UMSFreeVirtualProcessorRoot that the primary manages.
        /// </param>
        static DWORD CALLBACK PrimaryMain(LPVOID pContext);
    };
} // namespace details
} // namespace Concurrency
