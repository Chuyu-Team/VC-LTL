// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadProxy.cpp
//
// Proxy for a UMS thread.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a thread proxy.
    /// </summary>
    UMSThreadProxy::UMSThreadProxy(IThreadProxyFactory * pFactory, PUMS_COMPLETION_LIST pStartupList, unsigned int stackSize)
        : m_pFactory(pFactory)
        , m_pUMSContext(NULL)
        , m_hPhysicalContext(NULL)
        , m_hBlock(NULL)
        , m_blockingType(BlockingNone)
        , m_pRoot(NULL)
        , m_pLastRoot(NULL)
        , m_stackSize(stackSize)
        , m_threadPriority(THREAD_PRIORITY_NORMAL)
        , m_fIdlePooled(false)
        , m_yieldAction(ActionNone)
        , m_pTransmogrification(NULL)
        , m_activationCause(ActivationCauseNone)
        , m_fCanceled(FALSE)
        , m_criticalRegionCount(0)
        , m_hyperCriticalRegionCount(0)
#if defined(_DEBUG)
        , m_lastRunPrepareTimeStamp(0)
        , m_UMSDebugBits(0)
        , m_fShutdownValidations(false)
#endif // _DEBUG
    {
        // Since we treat thread proxy creation as non-fatal, this function must be exception safe with respect to
        // scheduler_worker_creation_error
        m_hBlock = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (m_hBlock == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_id = ResourceManager::GetThreadProxyId();

        UMS_CREATE_THREAD_ATTRIBUTES umsAttributes;
        PPROC_THREAD_ATTRIBUTE_LIST pAttributeList;
        SIZE_T sizeAttributeList;

        if (!UMS::CreateUmsThreadContext(&m_pUMSContext))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907

        UMSBaseObject *pThis = this;
        if (!UMS::SetUmsThreadInformation(m_pUMSContext, UmsThreadUserContext, &pThis, sizeof(pThis)))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907

        umsAttributes.UmsVersion = UMS_VERSION;
        umsAttributes.UmsContext = m_pUMSContext;
        umsAttributes.UmsCompletionList = pStartupList;

        UMS::InitializeProcThreadAttributeList(NULL, 1, 0, &sizeAttributeList);
        ASSERT(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

        pAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(_concrt_new char[sizeAttributeList]);
        if (!UMS::InitializeProcThreadAttributeList(pAttributeList, 1, 0, &sizeAttributeList))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907
        }

        if (!UMS::UpdateProcThreadAttribute(pAttributeList, 0, PROC_THREAD_ATTRIBUTE_UMS_THREAD, &umsAttributes, sizeof(UMS_CREATE_THREAD_ATTRIBUTES), NULL, NULL))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907
        }

        m_hPhysicalContext = UMS::CreateRemoteThreadEx(GetCurrentProcess(),
                                                       NULL,
                                                       stackSize*KB,
                                                       UMSThreadProxyMain,
                                                       this,
                                                       STACK_SIZE_PARAM_IS_A_RESERVATION,
                                                       pAttributeList,
                                                       &m_threadId);

        UMS::DeleteProcThreadAttributeList(pAttributeList);
        delete[] (reinterpret_cast<char *>(pAttributeList));

        if (m_hPhysicalContext == NULL)
        {
            // Cleanup everything we've allocated because this exception may be caught by a higher
            // layer to provide resiliency against thread creation failures during thread proxy construction.
            UMS::DeleteUmsThreadContext(m_pUMSContext);
            CloseHandle(m_hBlock);
            throw scheduler_worker_creation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    /// <summary>
    ///     Destroys a thread proxy.
    /// </summary>
    UMSThreadProxy::~UMSThreadProxy()
    {
        ASSERT(m_hBlock != NULL);
        CloseHandle(m_hBlock);

        ASSERT(m_hPhysicalContext != NULL);
        CloseHandle(m_hPhysicalContext);

        ASSERT(m_pUMSContext != NULL);
        UMS::DeleteUmsThreadContext(m_pUMSContext);
    }

    /// <summary>
    ///     Returns a process unique identifier for the thread proxy.
    /// </summary>
    unsigned int UMSThreadProxy::GetId() const
    {
        return m_id;
    }

    /// <summary>
    ///     Thread start routine for proxies.
    /// </summary>
    /// <param name="lpParameter">
    ///     Pointer to the thread proxy
    /// </param>
    DWORD CALLBACK UMSThreadProxy::UMSThreadProxyMain(LPVOID lpParameter)
    {
        UMSThreadProxy* pThreadProxy = reinterpret_cast<UMSThreadProxy*> (lpParameter);

        pThreadProxy->ReadyForDispatch();

        CONCRT_COREASSERT(pThreadProxy->GetCriticalRegionType() == OutsideCriticalRegion || pThreadProxy->m_pRoot->IsDeleting());
        pThreadProxy->Dispatch();

        ASSERT(pThreadProxy->m_fCanceled);

        //
        // The data structure needs to remain until we jump back to the primary executing it for the last time and the primary detects that
        // the thread has terminated.  Otherwise, the primary will read an invalid data structure to determine what just terminated!
        //
        return 0;
    }

    /// <summary>
    ///     Indicate that the thread proxy is ready for dispatch.
    /// </summary>
    void UMSThreadProxy::ReadyForDispatch()
    {
        // If the UT is started up on a dedicated primary then we need not be in a hypercritical
        // region. We enter a hypercritical region here to ensure that this would work if we decide
        // to move UT startup to vprocs.
        EnterHyperCriticalRegion();
        m_yieldAction = ActionStartup;
        UMS::UmsThreadYield(this);

       // Critical region count will be cleared on dispatch.
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
    UMSThreadProxy *UMSThreadProxy::FromUMSContext(PUMS_CONTEXT pUMSContext)
    {
        if (pUMSContext == NULL)
            return NULL;

        UMSBaseObject *pObj = NULL;
        UMSThreadProxy *pProxy = NULL;
        ULONG returnLength;

        if (!UMS::QueryUmsThreadInformation(pUMSContext, UmsThreadUserContext, reinterpret_cast <PVOID> (&pObj), sizeof(pObj), &returnLength))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        ASSERT(returnLength == sizeof(pObj));

        if (pObj != NULL && !pObj->IsPrimary())
            pProxy = static_cast<UMSThreadProxy *>(pObj);

        return pProxy;

    }

    /// <summary>
    ///     Returns our understanding of a UMS context (a UMS thread proxy) from the SLIST_ENTRY used to
    ///     place it on an SLIST (e.g.: the transfer list).
    /// </summary>
    /// <param name="pListEntry">
    ///     Pointer to the SList entry in the thread proxy
    /// </param>
    /// <returns>
    ///     UMS thread proxy
    /// </returns>
    UMSThreadProxy *UMSThreadProxy::FromListEntry(PSLIST_ENTRY pListEntry)
    {
        if (pListEntry == NULL) return NULL;
        return CONTAINING_RECORD(pListEntry, UMSThreadProxy, m_listEntry);
    }

    /// <summary>
    ///     Switch from the current thread proxy to pProxy.
    /// </summary>
    /// <param name="pProxy">
    ///     The thread proxy to switch to.
    /// </param>
    /// <param name="switchState">
    ///     The reason for the switch.
    /// </param>
    void UMSThreadProxy::InternalSwitchTo(UMSThreadProxy *pProxy, SwitchingProxyState switchState)
    {
        static const YieldAction stateActions[] = {
            ActionSwitchToAndRetire /* Idle */,
            ActionSwitchTo /* Blocking */,
            ActionTransmogrifyAndSwitch /* Nesting */
            };

        //
        // Manipulations of m_yieldAction need to be guarded under a hyper-critical region.  We cannot tolerate a page fault happening
        // which corrupts our state.
        //
        EnterHyperCriticalRegion();

        //
        // On nesting, we have to transmogrify the currently running UMS thread to a "virtual"-thread so that it can continue
        // to run as an external context of a nested scheduler without having to be scheduled atop a virtual processor (something
        // outside our model).
        //
        if (switchState == Nesting)
        {
            CONCRT_COREASSERT(m_pTransmogrification == NULL);

            //
            // We cannot perform an allocation or thread creation here, so we ask the transmogrificator to do this for us.  Oh fun.
            //
            RequestTransmogrification();
        }

        RVPMTRACE(switchState == Idle ? MTRACE_EVT_SWITCHTO_IDLE : (switchState == Blocking ? MTRACE_EVT_SWITCHTO_BLOCKING : MTRACE_EVT_SWITCHTO_NESTING), this, m_pRoot, pProxy);
        CONCRT_COREASSERT((int)switchState >= 0 && (int)switchState <= 2);
        m_yieldAction = stateActions[switchState];
        m_pNextProxy = pProxy;
        CONCRT_COREASSERT(m_yieldAction != ActionNone);
        CONCRT_COREASSERT(m_pRoot != NULL && m_pRoot == m_pLastRoot && m_pRoot->GetExecutingProxy() == this);
        UMS::UmsThreadYield(this);

        //
        // Idle threads have critical regions completely reset when they go on the idle pool (this is what allows us to have
        // "until the end of time" semantics on certain critical regions from the perspective of a thread going away).
        //
        if (switchState != Idle)
            ExitHyperCriticalRegion();
        else
        {
            //
            // Note that there is now a code path in the primary where-by it must allocate a new (or get a pooled) proxy to burn on the way out to avoid
            // a Win7 bug.  In this case, the primary must continue to execute the thread until thread exit which means it must be hyper-crit.  In this circumstance,
            // m_fCanceled should already be set on the way out!
            //
            CONCRT_COREASSERT(m_hyperCriticalRegionCount == 0 || m_fCanceled);
        }
    }

    /// <summary>
    ///     Called in order to perform a cooperative context switch out.  After this call, the context which was running will be blocked
    ///     until it is switched to or used to activate a virtual processor.
    /// </summary>
    void UMSThreadProxy::InternalSwitchOut(SwitchingProxyState switchState)
    {
        EnterHyperCriticalRegion();

        if (switchState == Nesting)
        {
            CONCRT_COREASSERT(m_pTransmogrification == NULL);

            //
            // We cannot perform an allocation or thread creation here, so we ask the transmogrificator to do this for us.  Oh fun.
            //
            RequestTransmogrification();

            //
            // Indicate to the primary that it should reset itself after unblocking the transmogrifier
            //
            m_yieldAction = ActionTransmogrifyAndReset;
            m_pNextProxy = NULL;
            UMS::UmsThreadYield(this);
        }
        else
        {
            CONCRT_COREASSERT(switchState == Blocking);
            //
            // If we are currently in a transmogrified state, undo the transmogrification.  Yielding back to the transmogrified primary will get rid of that
            // particular thread.
            //
            if (m_pTransmogrification != NULL)
            {
                m_pTransmogrification = NULL;
                UMS::UmsThreadYield(this);
            }
            else
            {
                m_yieldAction = ActionResetForSwitchOut;
                UMS::UmsThreadYield(this);
            }
        }
        ExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Yield to the underlying Operting system
    /// </summary>
    void UMSThreadProxy::InternalYieldToSystem()
    {
        // This needs to be called by the thread that is currently executing
        CONCRT_COREASSERT(UMSThreadProxy::GetCurrent() == this);

        EnterHyperCriticalRegion();

        m_yieldAction = ActionYieldToSystem;
        m_pNextProxy = NULL;
        CONCRT_COREASSERT(m_yieldAction != ActionNone);
        CONCRT_COREASSERT(m_pRoot != NULL && m_pRoot == m_pLastRoot && m_pRoot->GetExecutingProxy() == this);
        UMS::UmsThreadYield(this);

        ExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Deactivate the current thread proxy.  As this requires a message block set, it lives here
    ///     rather than in the VPROOT.
    /// </summary>
    /// <returns>
    ///     An indication of whether the awakening was due to an Activate call on the virtual processor root
    ///     (true) or an RM cause (e.g.: completion notification -- false).
    /// </returns>
    bool UMSThreadProxy::Deactivate()
    {
        CONCRT_COREASSERT(GetCriticalRegionType() != OutsideCriticalRegion);
        CONCRT_COREASSERT(m_pRoot != NULL);

        //
        // Manipulations of m_yieldAction need to be guarded under a hyper-critical region.  We cannot tolerate a page fault happening
        // which corrupts our state.
        //
        EnterHyperCriticalRegion();

        m_yieldAction = ActionDeactivate;
        m_pNextProxy = NULL;
        UMS::UmsThreadYield(this);

        CONCRT_COREASSERT(m_activationCause != ActivationCauseNone);
        bool fActivated = (m_activationCause == ActivationCauseActivate);
        m_activationCause = ActivationCauseNone;

        ExitHyperCriticalRegion();

        return fActivated;
    }

    /// <summary>
    ///     The caller has exited the dispatch loop.  Free the thread and deactivate.
    /// <summary>
    void UMSThreadProxy::FreeViaExit()
    {
        //
        // Manipulations of m_yieldAction need to be guarded under a hyper-critical region.  We cannot tolerate a page fault happening
        // which corrupts our state.
        //
        EnterHyperCriticalRegion();

        CONCRT_COREASSERT(!m_fIdlePooled);

        m_yieldAction = ActionFree;
        UMS::UmsThreadYield(this);

        //
        // We need not worry about releasing the critical region here.  When it goes on the free list, it gets completely reinitialized.  Either
        // that or we're on the way out (to get rid of the thread) and we only want to run this thread anyway.
        //
    }

    /// <summary>
    ///     Sets the priority of the underlying thread.
    /// </summary>
    /// <param name="priority">
    ///     The new priority value for the thread.
    /// </param>
    void UMSThreadProxy::SetPriority(int priority)
    {
        //
        // Right now (Win7), the priority set here only impacts running atop the real UT thread (not running the primary underneath).
        // This only happens on certain kernel transitions and on certain optimizations out of the kernel.  Really,
        // the primary should be the only one setting priority. However, we do this to ensure that BOTH the primary thread
        // and the kernel portion of the UT have the same priority setting.
        //
        m_threadPriority = priority;

        if (SetThreadPriority(m_hPhysicalContext, m_threadPriority) == 0)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    /// <summary>
    ///     Cancels the thread proxy causing the underlying thread to exit.
    /// </summary>
    void UMSThreadProxy::Cancel()
    {
        m_fCanceled = true;
    }

    /// <summary>
    ///     Spins until we've registered blocking.
    /// </summary>
    void UMSThreadProxy::SpinUntilBlocked()
    {
        if (m_pRoot != NULL)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();

            } while (m_pRoot != NULL);
        }
    }

    /// <summary>
    ///     Spins until we've registered what blocking type the last block operation was and returns the type.
    /// </summary>
    /// <returns>
    ///     Returns the blocking type.
    /// </returns>
    UMSThreadProxy::BlockingType UMSThreadProxy::SpinOnAndReturnBlockingType()
    {
        if (m_blockingType == BlockingNone)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();

            } while (m_blockingType == BlockingNone);
        }

        return m_blockingType;
    }

    /// <summary>
    ///     Request this proxy to be transmogrified.
    /// </summary>
    void UMSThreadProxy::RequestTransmogrification()
    {
        CONCRT_COREASSERT(m_pFactory != NULL);
        UMSFreeThreadProxyFactory * pFactory = static_cast<UMSFreeThreadProxyFactory *>(m_pFactory);
        Transmogrificator *pTransmogrificator = pFactory->GetTransmogrificator();

        CONCRT_COREASSERT(pTransmogrificator != NULL);
        pTransmogrificator->PerformTransmogrification(this);
    }

    /// <summary>
    ///     Notify that this proxy is ready to be transmogrified.
    /// </summary>
    void UMSThreadProxy::NotifyTransmogrification()
    {
        CONCRT_COREASSERT(m_pFactory != NULL);
        UMSFreeThreadProxyFactory * pFactory = static_cast<UMSFreeThreadProxyFactory *>(m_pFactory);
        Transmogrificator *pTransmogrificator = pFactory->GetTransmogrificator();

        CONCRT_COREASSERT(pTransmogrificator != NULL);
        pTransmogrificator->UnblockTransmogrification(this);
    }

    /// <summary>
    ///     Returns whether or not this thread is currently suspended.
    /// </summary>
    /// <returns>
    ///     Returns true if the thread is suspended
    /// </returns>
    bool UMSThreadProxy::IsSuspended()
    {
        BOOLEAN fSuspended = FALSE;
        UMS::QueryUmsThreadInformation(m_pUMSContext, UmsThreadIsSuspended, &fSuspended, sizeof(fSuspended), NULL);

        return !!fSuspended;
    }

    /// <summary>
    ///     Returns whether or not this thread is currently terminated.
    /// </summary>
    /// <returns>
    ///     Returns true if the thread is terminated
    /// </returns>
    bool UMSThreadProxy::IsTerminated()
    {
        BOOLEAN fTerminated = FALSE;
        UMS::QueryUmsThreadInformation(m_pUMSContext, UmsThreadIsTerminated, &fTerminated, sizeof(fTerminated), NULL);

        return !!fTerminated;
    }

    /// <summary>
    ///     Called in order to prepare this thread proxy to run atop a given virtual processor root.
    /// </summary>
    void UMSThreadProxy::PrepareToRun(UMSFreeVirtualProcessorRoot *pRoot)
    {
#if defined(_DEBUG)
        m_lastRunPrepareTimeStamp = _ReadTimeStampCounter();
#endif // _DEBUG

        m_blockingType = BlockingNone;
        m_pLastRoot = pRoot;
        m_pRoot = pRoot;
    }

} // namespace details
} // namespace Concurrency
