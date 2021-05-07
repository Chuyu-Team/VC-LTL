// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerBase.cpp
//
// Implementation file of the metaphor for a concrt scheduler
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <awint.hpp> // TRANSITION, shouldn't include this STL-build-internal header

namespace Concurrency
{
/// <summary>
///     Creates a scheduler that only manages internal contexts. Implicitly calls Reference.
///     If Attach is called, the scheduler is no longer anonymous because it is also managing the external
///     context where Attach was called.  To destroy an anonymous scheduler, Release needs to be called.
/// </summary>
/// <param name="policy">
///     [in] A const reference to the scheduler policy.
/// </param>
/// <returns>
///     A pointer to the new scheduler
/// </returns>
Scheduler* Scheduler::Create(_In_ const SchedulerPolicy& policy)
{
    ::Concurrency::details::SchedulerBase *pScheduler = ::Concurrency::details::SchedulerBase::Create(policy);
    pScheduler->Reference();
    return pScheduler;
}

/// <summary>
///     Allows a user defined policy to be used to create the default scheduler. It is only valid to call this API when no default
///     scheduler exists. Once a default policy is set, it remains in effect until the next valid callto the API.
/// </summary>
/// <param name="_Policy">
///     [in] The policy to be set as the default. The runtime will make a copy of the policy for its use, and the user
///     is responsible for the lifetime of the policy that is passed in.
/// </param>
void Scheduler::SetDefaultSchedulerPolicy(_In_ const SchedulerPolicy & _Policy)
{
    ::Concurrency::details::SchedulerBase::SetDefaultSchedulerPolicy(_Policy);
}

/// <summary>
///     Resets the default scheduler policy, and the next time a default scheduler is created, it will use the runtime's default policy settings.
/// </summary>
void Scheduler::ResetDefaultSchedulerPolicy()
{
    ::Concurrency::details::SchedulerBase::ResetDefaultSchedulerPolicy();
}

//
// Internal bit mask definitions for the shutdown gate.
//
#define SHUTDOWN_INITIATED_FLAG                 0x80000000
#define SUSPEND_GATE_FLAG                       0x40000000
#define SHUTDOWN_COMPLETED_FLAG                 0x20000000
#define GATE_COUNT_MASK                         0x1FFFFFFF
#define GATE_FLAGS_MASK                         0xE0000000

namespace details
{

    // The default scheduler lock protects access to both the default scheduler as well as the
    // default scheduler policy.
    SchedulerBase *SchedulerBase::s_pDefaultScheduler = NULL;
    SchedulerPolicy *SchedulerBase::s_pDefaultSchedulerPolicy = NULL;

    LONG SchedulerBase::s_initializedCount = 0;
    LONG SchedulerBase::s_oneShotInitializationState = ONESHOT_NOT_INITIALIZED;
    volatile LONG SchedulerBase::s_workQueueIdCounter = 0;
    DWORD SchedulerBase::t_dwContextIndex;

    // Number of suballocators for use by external contexts that are active in the process.
    volatile LONG SchedulerBase::s_numExternalAllocators = 0;

    // The max number of external contexts that could have suballocators at any given time.
    const int SchedulerBase::s_maxExternalAllocators = 32;

    // The maximum depth of the free pool of allocators.
    const int SchedulerBase::s_allocatorFreePoolLimit = 16;

    /// <summary>
    ///     Constructor for SchedulerBase.
    /// </summary>
    SchedulerBase::SchedulerBase(_In_ const ::Concurrency::SchedulerPolicy& policy)
        : m_policy(policy)
        , m_scheduleGroups(NULL, 256, 64)
        , m_externalThreadStatistics(NULL, 256, ListArray<ExternalStatistics>::DeletionThresholdInfinite)
        , m_contextIdCounter(-1)
        , m_scheduleGroupIdCounter(-1)
        , m_safePointDataVersion(0)
        , m_safePointCommitVersion(0)
        , m_safePointPendingVersion(0)
        , m_id(static_cast<unsigned int>(-1))
        , m_nextSchedulingRingIndex(0)
        , m_refCount(0)
        , m_attachCount(0)
        , m_internalContextCountPlusOne(1)
        , m_initialReference(0)
        , m_boundContextCount(0)
        , m_vprocShutdownGate(0)
        , m_fSweepWithoutActualWork(FALSE)
        , m_lastServiceScan(0)
        , m_pResourceManager(NULL)
        , m_activeVProcCount(0)
        , m_virtualProcessorsPendingThreadCreate(0)
        , m_enqueuedTaskCounter(0)
        , m_dequeuedTaskCounter(0)
        , m_enqueuedTaskCheckpoint(0)
        , m_dequeuedTaskCheckpoint(0)
        , m_lastThrottledCreateTime(0)
        , m_pendingDeferredCreates(0)
    {
        //
        // @TODO: Ugly...
        //
        m_scheduleGroups.SetScheduler(this);

        m_schedulerKind             = (::Concurrency::SchedulerType) policy.GetPolicyValue(::Concurrency::SchedulerKind);
        m_localContextCacheSize     = (unsigned short) policy.GetPolicyValue(::Concurrency::LocalContextCacheSize);
        m_schedulingProtocol        = (::Concurrency::SchedulingProtocolType) policy.GetPolicyValue(::Concurrency::SchedulingProtocol);

        //
        // This is a count before which we will **NOT** perform any throttling.  In the event of repeated latent blocking, we will reach
        // this number of threads rapidly.  By default, we choose this number to be 4x the number of cores.  Because MaxConcurrency is utilized
        // as a "preferred concurrency level", if a client has specified a ManConcurrency value that implies a greater number of vprocs than this,
        // we will adjust the throttling limit upwards to MaxConcurrency.  This may result in poorer overall throttling performance;
        // however -- one would expect that most clients aren't requesting > 4x oversubscription.
        //
        m_threadsBeforeThrottling   = max(::Concurrency::GetProcessorCount() * 4, policy.GetPolicyValue(::Concurrency::MaxConcurrency));

        // Allocate a TLS slot to track statistics for threads alien to this scheduler
        m_dwExternalStatisticsIndex = platform::__TlsAlloc(); // VSO#459907

#ifndef _UMS_DISABLED
        // Auto-reset event that is not signalled initially
        m_hThrottlingEvent = platform::__CreateAutoResetEvent(); // VSO#459907

        // Use a trampoline for UMS
        if (!RegisterWaitForSingleObject(&m_hThrottlingWait, m_hThrottlingEvent, SchedulerBase::ThrottlerTrampoline, this, INFINITE, WT_EXECUTEDEFAULT))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907
        }
#endif // _UMS_DISABLED

        if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
        {
            if ((m_hThrottlingTimer = RegisterAsyncTimerAndLoadLibrary(PSEUDOINFINITE, SchedulerBase::ThrottlerDispatchBridge, this, true)) == nullptr)
            {
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907
            }
        }
        else
        {
            if (!platform::__CreateTimerQueueTimer(&m_hThrottlingTimer,
                                       NULL,
                                       SchedulerBase::ThrottlerDispatchBridgeXP,
                                       this,
                                       PSEUDOINFINITE,
                                       PSEUDOINFINITE,
                                       WT_EXECUTEDEFAULT))
            {
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // VSO#459907
            }
        }
    }

    SchedulerBase::~SchedulerBase()
    {
        Cleanup();
    }

    void SchedulerBase::Cleanup()
    {
        for (int idx = 0; idx < m_nodeCount; ++idx)
            delete m_nodes[idx];

        for (int idx = 0; idx < m_nodeCount; ++idx)
            delete m_rings[idx];

        delete [] m_pCoreAffinityQuickCache;
        delete [] m_nodes;
        delete [] m_rings;
        delete [] m_numaInformation;

        // Cleanup a TLS slot and allow a reuse
        platform::__TlsFree(m_dwExternalStatisticsIndex);
        m_dwExternalStatisticsIndex = 0;

        m_pResourceManager->Release();
        SchedulerBase::StaticDestruction();

    }

    // race is fine -- only for inputting work
    SchedulingRing *SchedulerBase::GetNextSchedulingRing()
    {
        SchedulingRing *pRing = m_rings[m_nextSchedulingRingIndex];
        ASSERT(pRing != NULL);
        m_nextSchedulingRingIndex = GetNextValidSchedulingRingIndex(m_nextSchedulingRingIndex);
        return pRing;
    }

    int SchedulerBase::GetValidSchedulingRingIndex(int idx)
    {
        ASSERT(idx >= 0 && idx <= m_nodeCount);
        ASSERT(m_rings[idx] != NULL);
        if (!m_rings[idx]->IsActive())
            return GetNextValidSchedulingRingIndex(idx);
        return idx;
    }

    int SchedulerBase::GetNextValidSchedulingRingIndex(int idx)
    {
        ASSERT(idx >= 0 && idx <= m_nodeCount);
        do
        {
            idx = (idx+1) % m_nodeCount;
            ASSERT(m_rings[idx] != NULL);
        } while (!m_rings[idx]->IsActive());
        return idx;
    }

    SchedulingRing *SchedulerBase::GetNextSchedulingRing(const SchedulingRing *pOwningRing, SchedulingRing *pCurrentRing)
    {
        ASSERT(pCurrentRing != NULL);

        SchedulingRing *pRing = m_rings[GetNextValidSchedulingRingIndex(pCurrentRing->Id())];
        ASSERT(pRing != NULL);
        if (pRing == pOwningRing)
            pRing = NULL;
        return pRing;
    }

    void SchedulerBase::SetNextSchedulingRing(SchedulingRing *pRing)
    {
        ASSERT(pRing != NULL);

        if (m_schedulingProtocol == ::Concurrency::EnhanceForwardProgress)
            m_nextSchedulingRingIndex = GetNextValidSchedulingRingIndex(pRing->Id());
    }

    /// <summary>
    ///     Creates a scheduler instance
    /// </summary>
    /// <param name="policy">
    ///     [in] A const reference to the scheduler policy.
    /// </param>
    /// <returns>
    ///     A pointer to the new scheduler An exception is thrown if an error occurs.
    /// </returns>
    _Ret_writes_(1) SchedulerBase* SchedulerBase::Create(_In_ const SchedulerPolicy& policy)
    {
        SchedulerBase *pScheduler = CreateWithoutInitializing(policy);
        // Obtain hardware threads, initialize virtual processors, etc.
        pScheduler->Initialize();

        return pScheduler;
    }

    /// <summary>
    ///     Creates a scheduler instance
    /// </summary>
    /// <param name="policy">
    ///     [in] A const pointer to the scheduler policy.
    /// </param>
    /// <returns>
    ///     A pointer to the new scheduler An exception is thrown if an error occurs.
    /// </returns>
    _Ret_writes_(1) SchedulerBase* SchedulerBase::CreateWithoutInitializing(_In_ const SchedulerPolicy& policy)
    {
        policy._ValidateConcRTPolicy();
        CheckStaticConstruction();

        SchedulerBase *pScheduler = ThreadScheduler::Create(policy);
        return pScheduler;
    }

    /// <summary>
    ///     Generates a unique identifier for a context.
    /// </summary>
    unsigned int SchedulerBase::GetNewContextId()
    {
        return (unsigned int) InterlockedIncrement(&m_contextIdCounter);
    }

    /// <summary>
    ///     Generates a unique identifier for a schedule group.
    /// </summary>
    unsigned int SchedulerBase::GetNewScheduleGroupId()
    {
        return (unsigned int) InterlockedIncrement(&m_scheduleGroupIdCounter);
    }

    /// <summary>
    ///     Generates a unique identifier for a work queue (across scheduler instances in the process).
    /// </summary>
    unsigned int SchedulerBase::GetNewWorkQueueId()
    {
        return (unsigned int) InterlockedIncrement(&s_workQueueIdCounter);
    }

    /// <summary>
    ///     Anything which requires a one shot pattern of initialization with no destruction until termination goes here.
    /// </summary>
    void SchedulerBase::OneShotStaticConstruction()
    {
        _SpinCount::_Initialize();

        //
        // The TLS indicies must be one-shot as they are used outside the domain of guaranteed scheduler presence.  We cannot free them
        // until process-exit/CRT-unload or we'll have races with scheduler teardown/creation and outside APIs which require the TLS indicies.
        //

        t_dwContextIndex = platform::__TlsAlloc();

#ifndef _UMS_DISABLED
        UMSThreadScheduler::OneShotStaticConstruction();
#endif // _UMS_DISABLED
    }

    /// <summary>
    ///     Anything which requires a pattern of demand initialization upon first scheduler creation and destruction upon last
    ///     scheduler destruction goes here.
    /// </summary>
    void SchedulerBase::StaticConstruction()
    {
        //
        // Register ConcRT as an ETW trace provider
        //
        if (g_pEtw == NULL)
        {
            ::Concurrency::details::_RegisterConcRTEventTracing();
        }
    }

    /// <summary>
    ///     Called to ensure static construction is performed upon creation of a scheduler.
    /// </summary>
    void SchedulerBase::CheckStaticConstruction()
    {
        //
        // The reason a lock is taken here (rather than InterlockedIncrement of s_initializedCount) is that we want to ensure
        // the entire construction is complete from the 0->1 transition before any other schedulers proceed past this point.
        //
        _StaticLock::_Scoped_lock lockHolder(s_schedulerLock);

        if (++s_initializedCount == 1)
        {
            //
            // all static initialization here
            //
            StaticConstruction();

            if ((s_oneShotInitializationState & ONESHOT_INITIALIZED_FLAG) == 0)
            {
                OneShotStaticConstruction();

                //
                // This both guarantees a full fence and protects against simultaneous manipulation of the reference count stored within the lower
                // 31 bits of s_oneShotInitializationState.
                //
                InterlockedOr(&s_oneShotInitializationState, ONESHOT_INITIALIZED_FLAG);
            }
        }
    }

    /// <summary>
    ///     Performs one shot static destruction (at unload/process exit).
    /// </summary>
    void SchedulerBase::OneShotStaticDestruction()
    {
#ifndef _UMS_DISABLED
        UMSThreadScheduler::OneShotStaticDestruction();
#endif // _UMS_DISABLED
        platform::__TlsFree(t_dwContextIndex);
        t_dwContextIndex = 0;
    }

    /// <summary>
    ///     Called at unload/process exit to perform cleanup of one-shot initialization items.
    /// </summary>
    void SchedulerBase::CheckOneShotStaticDestruction()
    {
        //
        // This might happen at unload time and does not need to be governed by lock.  Further, at the time of calling in such circumstance,
        // all static and globals should already have destructed -- it would be bad form to touch s_schedulerLock even if it is presently
        // a wrapper around a POD type.  Note that a background thread might come through here but would never get past the InterlockedDecrement
        // unless we were at unload time.
        //
        LONG val = DereferenceStaticOneShot();
        if (val == ONESHOT_INITIALIZED_FLAG) // ref==0
        {
            //
            // Here, we are at unload time.
            //
            OneShotStaticDestruction();

            val = InterlockedAnd(&s_oneShotInitializationState, ~ONESHOT_INITIALIZED_FLAG);
            ASSERT(val == ONESHOT_INITIALIZED_FLAG);
        }
    }

    void SchedulerBase::StaticDestruction()
    {
        //
        // The reason a lock is taken here (rather than InterlockedDecrement of s_initializedCount) is that we want to ensure
        // the entire destruction is complete from the 1->0 transition before any other schedulers try construction.
        //
        _StaticLock::_Scoped_lock lockHolder(s_schedulerLock);

        if (--s_initializedCount == 0)
        {
            //
            // all static destruction here
            //

            //
            // Unregister ConcRT as an ETW trace provider
            //
            ::Concurrency::details::_UnregisterConcRTEventTracing();

            // We have exclusive access to the free pool, and therefore can use unsafe APIs.
            SubAllocator* pAllocator = s_subAllocatorFreePool.Pop();
            while (pAllocator != NULL)
            {
                delete pAllocator;
                pAllocator = s_subAllocatorFreePool.Pop();
            }
        }
    }

    /// <summary>
    ///     Initialize variables and request execution resources from the Resource Manager.
    /// </summary>
    void SchedulerBase::Initialize()
    {
        m_virtualProcessorAvailableCount = 0;
        m_virtualProcessorCount = 0;
        m_nodeCount = 0;

        m_pResourceManager = Concurrency::CreateResourceManager();
        m_id = Concurrency::GetSchedulerId();

        // Get the number of nodes on the machine so we can create a fixed array for scheduling nodes and
        // scheduling rings - obviating the need for locking these collections when we traverse them.
        m_nodeCount = GetProcessorNodeCount();

        ULONG highestNodeNumber = platform::__GetNumaHighestNodeNumber();
        m_numaCount = highestNodeNumber + 1;
        m_numaInformation = _concrt_new NumaInformation[m_numaCount];

        m_rings = _concrt_new SchedulingRing*[m_nodeCount];
        m_nodes = _concrt_new SchedulingNode*[m_nodeCount];
        memset(m_rings, 0, sizeof(SchedulingRing*) * m_nodeCount);
        memset(m_nodes, 0, sizeof(SchedulingNode*) * m_nodeCount);

        m_pAnonymousScheduleGroup = static_cast<ScheduleGroupBase *>(CreateScheduleGroup());
        m_pAnonymousScheduleGroup->m_kind |= ScheduleGroupBase::AnonymousScheduleGroup;

        // Assigns a bitmask id to each execution resource.
        unsigned int ridCounter = 0;

        //
        // Build a complete understanding of the system topology *WITHIN* the scheduler as well as maps for resource IDs, etc...
        //
        unsigned int procCount = GetProcessorCount();

        for (int index = 0; index < m_numaCount; ++index)
        {
            // Not all NUMA nodes may be present in the RM's topology. Initialize the bit sets here so all
            // operations work as expected.
            m_numaInformation[index].m_nodeMask.Grow(m_nodeCount);
            m_numaInformation[index].m_resourceMask.Grow(procCount);
        }

        ITopologyNode *pCurNode = m_pResourceManager->GetFirstNode();
        while (pCurNode != NULL)
        {
            QuickBitSet nodeMask(procCount);
            unsigned int nodeId = pCurNode->GetId();

            DWORD numaNodeNumber = pCurNode->GetNumaNode();
            m_numaInformation[numaNodeNumber].m_nodeMask.Set(nodeId);

            unsigned int ridBase = ridCounter;

            ITopologyExecutionResource *pFirstResource = pCurNode->GetFirstExecutionResource();
            ITopologyExecutionResource *pCurResource = pFirstResource;
            while (pCurResource != NULL)
            {
                //
                // The resource ID is arbitrary from the resource manager.  Assign a position in the bitmask for the resource ID and place
                // that in a hash table for quick lookups and masking with affinity.
                //
                unsigned int resourceId = pCurResource->GetId();
                m_resourceNodeMap.Insert(resourceId, nodeId);
                m_resourceBitMap.Insert(resourceId, ridCounter);
                nodeMask.Set(ridCounter);
                m_numaInformation[numaNodeNumber].m_resourceMask.Set(ridCounter);
                ++ridCounter;

                pCurResource = pCurResource->GetNext();
            }

            SchedulingRing *pRing = _concrt_new SchedulingRing(this, nodeId);
            SchedulingNode *pNode = _concrt_new SchedulingNode(nodeMask, numaNodeNumber, pRing);
            pRing->SetOwningNode(pNode);

            pCurResource = pFirstResource;
            while (pCurResource != NULL)
            {
                pNode->NotifyResource(pCurResource->GetId(), ridBase++);
                pCurResource = pCurResource->GetNext();
            }

            m_rings[nodeId] = pRing;
            m_nodes[nodeId] = pNode;

            pCurNode = pCurNode->GetNext();
        }

        ASSERT(ridCounter <= ::Concurrency::GetProcessorCount());

        m_idleSearch.Grow(ridCounter);
        m_nonAffineResourceListeners.Grow(ridCounter);
        m_affinityMessages.Grow(ridCounter);
        m_activeSet.Grow(ridCounter);

        m_pCoreAffinityQuickCache = _concrt_new ScheduleGroupSegmentBase* [ridCounter << QUICKCACHEPAD_SHIFT];
        for (unsigned int i = 0; i < ridCounter << QUICKCACHEPAD_SHIFT; ++i)
            m_pCoreAffinityQuickCache[i] = 0;

        // The RequestInitialVirtualProcessors API will invoke a scheduler callback to add new virtual processors to
        // the scheduler during the course of the API call. If this API succeeds, we can assume that scheduling
        // nodes have been populated with virtual processors representing resources allocated by the RM based on
        // values specified in the scheduler's policy.
        m_pSchedulerProxy = m_pResourceManager->RegisterScheduler(GetIScheduler(), CONCRT_RM_VERSION_1);
        m_pSchedulerProxy->RequestInitialVirtualProcessors(false);

        m_nextSchedulingRingIndex = GetValidSchedulingRingIndex(0);

        m_hSchedulerShutdownSync = CreateSemaphoreExW(NULL, 0, 0x7FFFFFFF, NULL, 0, SEMAPHORE_ALL_ACCESS);

        if (m_hSchedulerShutdownSync == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // the RM process should probably die here

        InitializeSchedulerEventHandlers();

        TraceSchedulerEvent(CONCRT_EVENT_START, TRACE_LEVEL_INFORMATION, m_id);
    }

    /// <summary>
    ///     Create a context from the default scheduler (possibly create the default too).
    /// </summary>
    ContextBase* SchedulerBase::CreateContextFromDefaultScheduler()
    {
        // If the context TLS value is NULL, the current thread is not attached to a scheduler. Find the
        // default scheduler and attach to it.

        SchedulerBase* pDefaultScheduler = GetDefaultScheduler();
        // Creating an external context on the current thread attaches the scheduler.
        ContextBase *pContext = pDefaultScheduler->AttachExternalContext(false);

        ASSERT((ContextBase*) platform::__TlsGetValue(t_dwContextIndex) == pContext);

        // GetDefaultScheduler takes a reference which is safe to release after the attach.
        pDefaultScheduler->Release();

        return pContext;
    }

    /// <summary>
    ///     Returns the ConcRT context attached to the current OS execution context. If one does not exist NULL is returned
    /// </summary>
    ContextBase *SchedulerBase::SafeFastCurrentContext()
    {
        return IsOneShotInitialized() ? (ContextBase*) platform::__TlsGetValue(t_dwContextIndex) : NULL;
    }

    /// <summary>
    ///     Returns the ConcRT context attached to the current OS execution context. If one does not exist NULL is returned
    ///     This is only callable if you know a-priori that all statics have been initialized.
    /// </summary>
    ContextBase *SchedulerBase::FastCurrentContext()
    {
        CONCRT_COREASSERT(IsOneShotInitialized());
        return (ContextBase*) platform::__TlsGetValue(t_dwContextIndex);
    }

    /// <summary>
    ///     Returns a pointer to the ConcRT scheduler attached to the current thread. If one does not exist, it creates
    ///     a context and attaches it to the default scheduler.
    /// </summary>
    SchedulerBase* SchedulerBase::CurrentScheduler()
    {
        return CurrentContext()->GetScheduler();
    }

    /// <summary>
    ///     Returns a pointer to the current scheduler, if the current thread is attached to a ConcRT scheduler, null otherwise.
    ///     This is only callable if you know a-priori that all statics have been initialized.
    /// </summary>
    SchedulerBase *SchedulerBase::FastCurrentScheduler()
    {
        ContextBase * pContext = FastCurrentContext();
        return (pContext != NULL) ? pContext->GetScheduler() : NULL;
    }

    /// <summary>
    ///     Returns a pointer to the current scheduler, if the current thread is attached to a ConcRT scheduler, null otherwise.
    /// </summary>
    SchedulerBase *SchedulerBase::SafeFastCurrentScheduler()
    {
        ContextBase * pContext = SafeFastCurrentContext();
        return (pContext != NULL) ? pContext->GetScheduler() : NULL;
    }

    /// <summary>
    ///     Returns a pointer to the default scheduler. Creates one if it doesn't exist and tries to make it the default.
    ///     NOTE: The API takes an reference on the scheduler which must be released by the caller appropriately.
    /// </summary>
    SchedulerBase *SchedulerBase::GetDefaultScheduler()
    {
        // Acquire the lock in order to take a safe reference on the default scheduler.
        _StaticLock::_Scoped_lock _lock(s_defaultSchedulerLock);

        // If the default scheduler is non-null, try to reference it safely. If the reference fails,
        // we've encountered a scheduler that is in the middle of finalization => the thread finalizing
        // the scheduler will attempt to clear the value under write mode.
        if ((s_pDefaultScheduler == NULL) || !s_pDefaultScheduler->SafeReference())
        {
            SchedulerPolicy policy(0);

            // Note that the default scheduler policy is protected by the default scheduler lock.
            SchedulerPolicy * pDefaultPolicy = s_pDefaultSchedulerPolicy;
            if (pDefaultPolicy != NULL)
            {
                policy = *pDefaultPolicy;
            }

            // Either the default scheduler was null, or we found a scheduler that was in the middle of being finalized.
            // Create a scheduler and set it as the default.
            s_pDefaultScheduler = SchedulerBase::CreateWithoutInitializing(policy);

            // Obtain hardware threads, initialize virtual processors, etc.
            s_pDefaultScheduler->Initialize();

            // Create returns a scheduler with a reference count of 0. We need to reference the scheduler before releasing the lock.
            // to prevent a different thread from assuming this scheduler is shutting down because the ref count is 0.
            // The caller is responsible for decrementing it after attaching to the scheduler.
            s_pDefaultScheduler->Reference();
        }

        // We're holding on to a reference, so it is safe to return this scheduler.
        ASSERT(s_pDefaultScheduler != NULL);
        return s_pDefaultScheduler;
    }

    /// <summary>
    ///     Allows a user defined policy to be used to create the default scheduler. It is only valid to call this API when no default
    ///     scheduler exists, unless the parameter is NULL. Once a default policy is set, it remains in effect until the next valid call
    ///     to the API.
    /// </summary>
    /// <param name="_Policy">
    ///     [in] A pointer to the policy to be set as the default. The runtime will make a copy of the policy
    ///     for its use, and the user is responsible for the lifetime of the policy that is passed in. An argument of NULL will reset
    ///     the default scheduler policy, and the next time a default scheduler is created, it will use the runtime's default policy settings.
    /// </param>
    void SchedulerBase::SetDefaultSchedulerPolicy(_In_ const SchedulerPolicy & _Policy)
    {
        _Policy._ValidateConcRTPolicy();

        bool fSetDefault = false;

        if (s_pDefaultScheduler == NULL)
        {
            // We can only set a non-null default policy if the default scheduler does not exist.

            _StaticLock::_Scoped_lock _lock(s_defaultSchedulerLock);

            // It's possible the default scheduler exists but is on its way out, i.e. its ref count is 0, and a different thread is about
            // acquire the write lock and set the value to null. We ignore this case, and allow the API to fail.
            if (s_pDefaultScheduler == NULL)
            {
                delete s_pDefaultSchedulerPolicy;
                s_pDefaultSchedulerPolicy = _concrt_new SchedulerPolicy(_Policy);
                fSetDefault  = true;
            }
        }

        if (!fSetDefault)
        {
            throw default_scheduler_exists();
        }
    }

    /// <summary>
    ///     Resets the default scheduler policy, and the next time a default scheduler is created, it will use the runtime's default policy settings.
    /// </summary>
    void SchedulerBase::ResetDefaultSchedulerPolicy()
    {
        _StaticLock::_Scoped_lock _lock(s_defaultSchedulerLock);

        if (s_pDefaultSchedulerPolicy != NULL)
        {
            delete s_pDefaultSchedulerPolicy;
            s_pDefaultSchedulerPolicy = NULL;
        }
    }

    /// <summary>
    ///     Increments the reference count to the scheduler but does not allow a 0 to 1 transition. This API should
    ///     be used to safely access a scheduler when the scheduler is not 'owned' by the caller.
    /// </summary>
    /// <returns>
    ///     True if the scheduler was referenced, false, if the reference count was 0.
    /// </returns>
    bool SchedulerBase::SafeReference()
    {
        return SafeInterlockedIncrement(&m_refCount);
    }

    /// <summary>
    ///     Start up a virtual processor in the scheduler, if one is found.  The virtual processor must have the specified availability
    ///     characteristics.
    /// </summary>
    bool SchedulerBase::StartupVirtualProcessor(ScheduleGroupSegmentBase *pSegment,
                                                location bias,
                                                ULONG type)
    {
        //
        // We **MUST** be in a hyper-critical region during this period.  There is an interesting scenario on UMS that makes this so:
        //
        //     - [VP A] can't find work and is in its search for work loop
        //     - [VP A] makes itself available
        //     - [VP B] running context alpha adds a new work item and does a StartupIdleVirtualProcessor
        //     - [VP B] does a FindAvailableVirtualProcessor and claims VP A
        //     - [VP B] page faults / blocks
        //     - [VP A] finds context alpha in its final SFW pass
        //     - [VP A] tries to claim ownership of its virtual processor
        //     - [VP A] can't claim exclusive ownership because context alpha already did
        //     - [VP A] calls deactivate to wait for the corresponding activation.
        //     - [VP A] deadlocks with context alpha.  Since it is about to execute alpha, no one else can grab it.  Similarly,
        //              it's waiting on an activate which will only come from context alpha.
        //
        // Since this code runs on the primary anyway during completion list moves, hyper-crit should be safe.  This does mean that
        // this code must be extraordinarily careful about what it calls / does.  There can be NO MEMORY ALLOCATION or other arbitrary
        // Win32 calls in the UMS variant of this path.
        //
        bool foundVProc = false;
        ContextBase::StaticEnterHyperCriticalRegion();

        //
        // The callers of this API MUST check that that the available virtual processor count in the scheduler
        // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
        // most of the time, and it saves the function call overhead on fast paths (chore push)
        //
        VirtualProcessor::ClaimTicket ticket;
        if (FoundAvailableVirtualProcessor(ticket, bias, type))
        {
            ticket.Exercise(pSegment);
            foundVProc = true;
        }

        ContextBase::StaticExitHyperCriticalRegion();
        return foundVProc;
    }

    /// <summary>
    ///     Looks for an available virtual processor in the scheduler, and returns it.
    /// </summary>
    bool SchedulerBase::FoundAvailableVirtualProcessor(VirtualProcessor::ClaimTicket& ticket,
                                                       location bias,
                                                       ULONG type)
    {
        // Direct or indirect callers of this API MUST check that that the available virtual processor count in the scheduler
        // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
        // most of the time, and it saves the function call overhead on fast paths (chore push)

        //
        // Bias first towards the given virtual processor, secondly to its node, and thirdly to everyone else in order.
        //
        switch(bias._GetType())
        {
            case location::_NumaNode:
            {
                NumaInformation *pNumaInfo = m_numaInformation + bias._GetId();
                for (int i = 0; i < m_nodeCount; ++i)
                {
                    if (pNumaInfo->m_nodeMask.IsSet((unsigned int)i))
                    {
                        if (m_nodes[i]->FoundAvailableVirtualProcessor(ticket, bias, type))
                            return true;
                    }
                }

                break;
            }
            case location::_SchedulingNode:
            case location::_ExecutionResource:
            {
                SchedulingNode *pBiasNode = FindNodeByLocation(&bias);
                if (pBiasNode && pBiasNode->FoundAvailableVirtualProcessor(ticket, bias, type))
                    return true;

                break;
            }

            default:
                break;
        }

        for (int idx = 0; idx < m_nodeCount; ++idx)
        {
            SchedulingNode *pNode = m_nodes[idx];
            if (pNode != NULL)
            {
                // Perform a quick check of the processor count to avoid the function call overhead
                // on some fast paths (chore push operations) on a system with many nodes.
                if (pNode->HasVirtualProcessorAvailable())
                {
                    if (pNode->FoundAvailableVirtualProcessor(ticket, location(), type))
                        return true;
                }
            }
        }

        return false;

    }

    /// <summary>
    ///     Attempts to push a runnable to an inactive virtual processor.  If successful, true is returned.
    /// </summary>
    bool SchedulerBase::PushRunnableToInactive(InternalContextBase *pRunnableContext, location bias)
    {
        bool fPushed = false;

        //
        // Affinitization requires a spin wait on the blocked flag.  We cannot push ourselves to another virtual processor for startup!
        //
        if (SchedulerBase::FastCurrentContext() != pRunnableContext)
        {
            ContextBase::StaticEnterHyperCriticalRegion();

            //
            // The callers of this API MUST check that that the available virtual processor count in the scheduler
            // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
            // most of the time, and it saves the function call overhead on fast paths (chore push)
            //
            VirtualProcessor::ClaimTicket ticket;
            if (FoundAvailableVirtualProcessor(ticket, bias, VirtualProcessor::AvailabilityInactive | VirtualProcessor::AvailabilityInactivePendingThread))
            {
                ticket.ExerciseWith(pRunnableContext);
                fPushed = true;
            }

            ContextBase::StaticExitHyperCriticalRegion();
        }

        return fPushed;
    }

    /// <summary>
    ///     Steals a local runnable contexts from nodes in the scheduler other than the skip node provided.
    /// </summary>
    InternalContextBase *SchedulerBase::StealForeignLocalRunnableContext(SchedulingNode *pSkipNode)
    {
        ASSERT(pSkipNode != NULL);

        for (int i = 0; i < m_nodeCount; ++i)
        {
            if (m_nodes[i] != NULL && m_nodes[i] != pSkipNode)
            {
                ASSERT(m_nodes[i]->m_id == i);
                InternalContextBase *pContext = m_nodes[i]->StealLocalRunnableContext();
                if (pContext != NULL)
                    return pContext;
            }
        }

        return NULL;
    }

    /// <summary>
    ///     Determines how long in milliseconds until the next set of threads is allowed to be created.
    /// </summary>
    ULONG SchedulerBase::ThrottlingTime(ULONG stepWidth)
    {
        ULONG boundContextCount = GetNumberOfBoundContexts();
        if (boundContextCount < m_threadsBeforeThrottling)
            return 0;

        ULONG overage = (boundContextCount - m_threadsBeforeThrottling);

        //
        // We can instantly shoot up to m_threadsBeforeThrottling.  For all below notes, K is the stair-step width.  Note that we are
        // hard limited to 8K threads on Win7 UMS currently.  This should have hefty slope to reach the thousands especially since this is per-scheduler
        // and we can have multiple schedulers in the process!
        //
        // After that, the next 100 threads will take approximately (1) seconds to create. // 100 threads
        //           , the next 200 threads will take approximately (8) seconds to create. // 300 threads
        //           , the next 300 threads will take approximately (20) seconds to create. // 600 threads
        //           , the next 900 threads will take approximately (6.5) minutes to create. // 1.5K threads (2.5m: 600-1000)
        //           , the next 1000 threads will take approximately (20) minutes to create. // 2.5K threads
        //           , the next 1500 threads will take approximately (1.5) hours to create. // 4K threads
        //           , the next 4000 threads will take approximately (12) hours to create. // 8K threads
        //           , we run out of resources.  Hopefully, we aren't repeatedly waiting on operations with multi-hour latency in a parallel loop.
        //
        ULONG delay = 0;

        if (overage < 100)
            delay = 5 + (overage / 10);
        else if (overage < 300)
            delay = 15 + 0 + (overage / 8);
        else if (overage < 600)
            delay = 53 + 7 + (overage / 5);
        else if (overage < 1500)
            delay = 180 + 0 + (overage / 4);
        else if (overage < 2500)
            delay = 555 + 0 + (overage / 3);
        else if (overage < 4000)
            delay = 1388 + 1112 + (overage / 3);
        else
            delay = 3833 + 4367 + (overage / 2);

        return (delay * stepWidth);
    }

    /// <summary>
    ///     Acquires a new internal context of the appropriate type and notifies the scheduler when it is available.  The scheduler can
    ///     choose what to do with said internal context.  This creation happens in a deferred manner subject to throttling constraints.
    /// </summary>
    void SchedulerBase::DeferredGetInternalContext()
    {
        OMTRACE(MTRACE_EVT_DEFERREDCONTEXT, this, NULL, NULL, NULL);

        //
        // This routine must be UMS safe and must schedule the deferred task in a UMS safe way.
        //
        if (InterlockedIncrement(&m_pendingDeferredCreates) == 1)
        {
#ifndef _UMS_DISABLED
            SetEvent(m_hThrottlingEvent);
#else
            // Directly invoke the throttle dispatch
            SchedulerBase::ThrottlerTrampoline(this, true);
#endif // _UMS_DISABLED
        }
    }

    /// <summary>
    ///     Changes the due time for dispatching new threads
    /// </summary>
    void SchedulerBase::ChangeThrottlingTimer(ULONG dueTime)
    {
        if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
        {
            // The ChangeTimerQueueTimer is a pre-Vista API
            FILETIME time = {0};
            // Convert 100 ns unit into 1 ms unit.
            // Negative here means FILETIME is a time span (instead of time point).
            reinterpret_cast<long long &>(time) = -static_cast<long long>(dueTime) * 10000;

            SetThreadpoolTimer(static_cast<PTP_TIMER>(m_hThrottlingTimer), &time, PSEUDOINFINITE, 0);
        }
        else
        {
            platform::__ChangeTimerQueueTimer(NULL, m_hThrottlingTimer, dueTime, PSEUDOINFINITE);
        }
    }

    /// <summary>
    ///     Acts as a trampoline between the event wait and the timer wait as we cannot queue the timer in DeferredGetInternalContext
    ///     due to limitations on what Win32 APIs can be called from a UMS primary.
    /// </summary>
    void SchedulerBase::ThrottlerTrampoline(PVOID pData, BOOLEAN)
    {
        SchedulerBase *pScheduler = reinterpret_cast<SchedulerBase *>(pData);

        ULONG delay = pScheduler->ThrottlingTime(1);
        ULONG delta = pScheduler->ThrottlingDelta();

        //
        // If we don't need a timer (or we decide that the due time is too "soon"), just invoke the dispatcher.  Otherwise, reschedule it for
        // a later time.  Minimize the number of background threads we're utilizing here.
        //
        if (delta < delay)
        {
            OMTRACE(MTRACE_EVT_SCHEDULEDTHROTTLER,  pScheduler,  SchedulerBase::FastCurrentContext(), NULL, delay - delta);

            pScheduler->ChangeThrottlingTimer(delay-delta);
        }
        else
            pScheduler->ThrottlerDispatch();
    }

    /// <summary>
    ///     Creates new contexts.
    /// </summary>
    void SchedulerBase::ThrottlerDispatch()
    {
        OMTRACE(MTRACE_EVT_THROTTLERDISPATCH, this, NULL, NULL, platform::__GetTickCount64());

        //
        // The throttler will be spuriously awakened at 49-day intervals due to a timer limitation on pre-Vista operating systems.  Just ignore
        // the spurious awakening in these cases.
        //
        if (m_pendingDeferredCreates > 0)
        {
            bool fAwokeVProc = false;
            bool fFailedThreadCreate = false;
            bool fReschedule = false;


            //
            // It is entirely possible that m_pendingDeferredCreates is a number much larger than is strictly speaking necessary.  A context
            // may go through SFW, require a thread, block, get awoken because of a context, switch to it, block, go through SFW, and require
            // a thread again all in between two throttling ticks.
            //
            for(;;)
            {
                InternalContextBase *pContext = GetInternalContext(false);
                if (pContext == NULL)
                {
                    fFailedThreadCreate = true;
                    break;
                }

                OMTRACE(MTRACE_EVT_CREATEDTHROTTLEDCONTEXT, this, pContext, NULL, platform::__GetTickCount64());

                fAwokeVProc |= NotifyThrottledContext(pContext);

                if (!HasVirtualProcessorPendingThreadCreate() || ThrottlingTime(1) > 0)
                {
                    break;
                }
            }

            //
            // The timer should be rescheduled under the following circumstances only:
            //  - If we are not in scheduler shutdown and either there are still virtual processors in need of threads *OR* our observation of pending
            //      requests changed.
            //  - If we are in scheduler shutdown, were unable to create a new thread and all available virtual processors are pending threads (i.e, we
            //      are unable to wake up an idle virtual processor).
            //

            if (!m_fSweepWithoutActualWork && (fFailedThreadCreate || HasVirtualProcessorPendingThreadCreate()))
            {
                fReschedule = true;
                InterlockedExchange(&m_pendingDeferredCreates, 1);
            }
            else
            {
                LONG pendingDeferredCreates = m_pendingDeferredCreates;
                ASSERT(m_pendingDeferredCreates >= pendingDeferredCreates);
                fReschedule = InterlockedExchangeAdd(&m_pendingDeferredCreates, -pendingDeferredCreates) != pendingDeferredCreates;

                if (!fReschedule && m_fSweepWithoutActualWork && !fAwokeVProc)
                {
                    //
                    // At least one vproc needs to be activated since pending requests blocks scheduler shutdown.  Activate a vproc after
                    // clearing the pending request count to restart scheduler shutdown. If such a vproc cannot be found because all virtual processors
                    // are pending thread create, we need to reschedule the timer.
                    //
                    fAwokeVProc = StartupIdleVirtualProcessor(GetAnonymousScheduleGroupSegment());
                    if (!fAwokeVProc)
                    {
                        fReschedule = true;
                        InterlockedExchange(&m_pendingDeferredCreates, 1);
                    }
                }
            }

            if (fReschedule)
            {
                ASSERT(m_pendingDeferredCreates > 0);

                //
                // There is absolutely no reason to trampoline here.  We are within an arbitrary thread context and not a primary.  Just reset
                // the timer.
                //
                ULONG delay = ThrottlingTime(1);
                ULONG delta = ThrottlingDelta();

                delay = (delta < delay) ? delay - delta : 0;
                OMTRACE(MTRACE_EVT_SCHEDULEDTHROTTLER,  this,  NULL, NULL, delay);

                //
                // If for some reason, the throttler could not get a thread (we hit the system-wide cap on threads) and the failure wasn't due to
                // some other exception, we push back throttling so that nothing will get tried again for at least another 500 mS)
                //
                if (fFailedThreadCreate && delay < 500)
                    delay = 500;
                ChangeThrottlingTimer(delay);
            }
        }
    }

    /// <summary>
    ///     Called to notify the scheduler that a context is available from the throttling manager / background creation.
    /// </summary>
    bool SchedulerBase::NotifyThrottledContext(InternalContextBase *pContext)
    {
        VirtualProcessor::ClaimTicket ticket;

        if (FoundAvailableVirtualProcessor(ticket,
                                           location(),
                                           VirtualProcessor::AvailabilityInactivePendingThread | VirtualProcessor::AvailabilityIdlePendingThread))
        {
            OMTRACE(MTRACE_EVT_NOTIFYTHROTTLEDCONTEXT, this, pContext, NULL, TRUE);

            if (!ticket.ExerciseWakesExisting())
                ticket.ExerciseWith(pContext);
            else
            {
                m_reservedContexts.Push(pContext);
                ticket.Exercise();
            }

            return true;
        }
        else
        {
            OMTRACE(MTRACE_EVT_NOTIFYTHROTTLEDCONTEXT, this, pContext, NULL, FALSE);
            m_reservedContexts.Push(pContext);
        }

        return false;
    }

    /// <summary>
    ///     Acquires a new internal context of the appropriate type and returns it.  This can come from either
    ///     a free list within the scheduler, or be one newly allocated from the heap.  This method may return NULL if the thread cannot be
    ///     created due to a resource limitation.
    /// </summary>
    /// <param name="fThrottled">
    ///     An indication as to whether the creation should be throttled.
    /// </param>
    InternalContextBase *SchedulerBase::GetInternalContext(bool fThrottled)
    {
        OMTRACE(MTRACE_EVT_GETINTERNALCONTEXT, this, SchedulerBase::FastCurrentContext(), NULL, m_boundContextCount);

        //
        // For some schedulers, the reserved context pool is just a list of unused contexts that are pooled due to races.  For others (e.g.: UMS),
        // it is a pool of contexts to be used in certain circumstances (e.g.: where contexts can't be created) and shouldn't be pulled from for
        // general fetch.
        //
        if (fThrottled && AllowGeneralFetchOfReservedContexts())
        {
            InternalContextBase *pContext = GetReservedContext();
            if (pContext != NULL)
                return pContext;
        }

        //
        // m_internalContextPool contains unbound contexts.  The act of binding a context is what, in particular, needs to be throttled.
        //
        if (fThrottled)
        {
            ULONG delay = ThrottlingTime(1);
            if (delay > 0)
            {
                if (ThrottlingDelta() < delay)
                {
                    //
                    // The caller is responsible for notifying the background thread.  After all -- there may be other runnable contexts
                    // that can be switched to before notification that we absolutely need a thread created at the point of throttling.
                    //
                    return NULL;
                }

                //
                // It has been sufficiently long since the last thread creation.  Let this one go through here.  Yes, it is possible that
                // the background thread decides this at the same time and both create a thread without the exact throttling ramp.  Since
                // this is largely heuristic, it doesn't matter.
                //
                fThrottled = false;
            }
        }

#if _UMSTRACE
        OMTRACE(MTRACE_EVT_GETINTERNALCONTEXTNOTHROTTLE, this, SchedulerBase::FastCurrentContext(), NULL, m_boundContextCount);
#endif // _UMSTRACE

        //
        // *** READ THIS ***:
        //
        // The increment of m_boundContextCount (which is what the throttler uses to make a determination of whether/how much to throttle)
        // must happen between the point of throttling decision and ** ANY LOCK ACQUISITION ** in the below code.  Otherwise, on the UMS scheduler,
        // N threads may come in here, decide not to throttle, block, switch to another N threads which also decide not to throttle since the count is not
        // incremented yet and block on the same lock.
        //
        // This can lead to a side-stepping of the throttler and exponential thread growth.
        //
        // Note: If we ever delay bind contexts, this needs to change.
        //
        InterlockedIncrement(&m_boundContextCount);

        if (!fThrottled)
        {
            //
            // This is unprotected and unfenced.  It is entirely possible this thread and the background thread compete and both set
            // this.  Throttling does not need to be exact.
            //
            StampThrottledCreate();
        }

        //
        // Thread creation failure due to the CreateThread* call failing is not considered to be a fatal exception.  Any other
        // resource failure (e.g.: out of memory) is still considered fatal to the runtime.
        //
        InternalContextBase *pContext = pContext = m_internalContextPool.Pop();

        if (pContext == NULL)
        {
            pContext = CreateInternalContext();
            AddContext(pContext);


            // The internal reference count on the scheduler *must* be incremented by the creator of the context.
            // The reference count will be released when the context is canceled. If the context is executing on
            // a thread proxy at the time it is canceled, it will decrement its own reference count before
            // leaving the dispatch loop for the final time. If it is on the idle pool - the thread/context
            // that cancels it is responsible for decrementing the reference count.
            IncrementInternalContextCount();
        }

        //
        // IMPORTANT NOTE: It is possible that there is a thread proxy still in the process of executing this
        // context's dispatch loop. This is because on going idle, contexts add themselves to the idle pool,
        // and proceed to leave their dispatch loops - they could be picked up and re-initialized before they
        // have actually left the routine.
        //
        // We must be careful *not* to re-initialize any variables the context uses after the point at which it
        // adds itself to the idle list, until the time it has left the dispatch routine.
        //
        CONCRT_COREASSERT(pContext != NULL);

        //
        // Note also that there are other fields which need to be touched until m_blockedState is set.  When we reuse a context, we must spin until
        // that bit is set.  Newly created contexts are started blocked.
        //
        pContext->SpinUntilBlocked();

        // Context shall be marked as idle.
        CONCRT_COREASSERT(pContext->IsIdle());

        // Bind a thread proxy to the context
        pContext->m_pThreadProxy = NULL;

        try
        {
            // NOTE: This is one of the places in the core scheduler where we catch an internal exception. This means that
            // the code from this point down, including code in the Resource Manager layer must be exception safe wrt the
            // scheduler_worker_creation_error exception being thrown.
            GetSchedulerProxy()->BindContext(pContext);
        }
        catch (const scheduler_worker_creation_error&)
        {
            ReleaseInternalContext(pContext);
            pContext = NULL;
        }
        //__faststorefence();

        if (pContext != NULL)
        {

            CMTRACE(MTRACE_EVT_CONTEXT_ACQUIRED, pContext, NULL, NULL);

#if defined(_DEBUG)
            pContext->NotifyAcquired();
#endif // _DEBUG

            pContext->ClearCriticalRegion();

        }

        return pContext;
    }

    /// <summary>
    ///     Enqueues a context into m_allContexts
    /// </summary>
    void SchedulerBase::AddContext(InternalContextBase * pContext)
    {
        ContextNode *pNode = _concrt_new ContextNode(pContext);
        m_allContexts.Push(pNode);
    }

    ///<summary>
    ///     Releases an internal context of the appropriate to the scheduler's idle pool.
    ///</summary>
    void SchedulerBase::ReleaseInternalContext(InternalContextBase *pContext, bool fUnbind)
    {
        if (fUnbind)
            m_pSchedulerProxy->UnbindContext(pContext);

#if defined(_DEBUG)
        pContext->m_fEverRecycled = true;
        pContext->SetDebugBits(CTX_DEBUGBIT_RELEASED);
#endif // _DEBUG

        InterlockedDecrement(&m_boundContextCount);

        // Context shall be marked as idle.
        CONCRT_COREASSERT(fUnbind || pContext->IsIdle());

        // We keep all contexts created by the scheduler. Deleting/canceling these contexts would required us to
        // remove it from the list of 'all contexts' (m_allContexts), which we use during finalization to detect
        // blocked contexts, and would require an additional level of synchronization there. Since idle contexts
        // do not have backing thread proxies, this is not a problem.
        m_internalContextPool.Push(pContext);
    }

    /// <summary>
    ///     Gets a realized chore from the idle pool, creating a new one if the idle pool is empty.
    /// </summary>
    RealizedChore * SchedulerBase::GetRealizedChore(TaskProc pFunction, void * pParameters)
    {
        RealizedChore * pChore = m_realizedChorePool.Pop();

        if (pChore == NULL)
        {
            pChore = _concrt_new RealizedChore(pFunction, pParameters);
        }
        else
        {
            pChore->Initialize(pFunction, pParameters);
        }
        return pChore;
    }

    ///<summary>
    ///     Releases an external context of the to the scheduler's idle pool, destroying it if the idle pool is full.
    ///</summary>
    void SchedulerBase::ReleaseRealizedChore(RealizedChore * pChore)
    {
        // Try to maintain the max size of the free pool somewhere close to num vprocs * 32. It is
        // not an exact upper limit.
        if (m_realizedChorePool.Count() < (m_virtualProcessorCount << 5))
        {
            m_realizedChorePool.Push(pChore);
        }
        else
        {
            delete pChore;
        }
    }

    /// <returns>
    ///     Returns a copy of the policy this scheduler is using.  No error state.
    /// </returns>
    SchedulerPolicy SchedulerBase::GetPolicy() const
    {
        return m_policy;
    }

    /// <summary>
    ///     Increments a reference count to this scheduler to manage lifetimes over composition.
    ///     This reference count is known as the scheduler reference count.
    /// </summary>
    /// <returns>
    ///     The resulting reference count is returned.  No error state.
    /// </returns>
    unsigned int SchedulerBase::Reference()
    {
        ASSERT(m_internalContextCountPlusOne > 0);
        LONG val = InterlockedIncrement(&m_refCount);
        if (val == 1)
        {
            //
            // This could be an initial reference upon the scheduler from a creation path or it could be
            // the case that an unblocked context from a scheduler decided to attach the scheduler somewhere.
            // In the second case, we need to resurrect the scheduler and halt the shutdown attempt.
            //
            if (m_initialReference > 0)
            {
                //
                // This should only come from an **INTERNAL** context on this scheduler; otherwise, the client is
                // being very bad and racing with shutdown for a nice big crash.
                //
                ContextBase* pCurrentContext = SchedulerBase::FastCurrentContext();

                if ((pCurrentContext == NULL ) || (pCurrentContext->IsExternal()) || (pCurrentContext->GetScheduler() != this))
                {
                    throw improper_scheduler_reference();
                }

                Resurrect();
            }
            else
            {
                InterlockedExchange(&m_initialReference, 1);
            }

        }
        return (unsigned int)val;
    }

    /// <summary>
    ///     Decrements this scheduler's reference count to manage lifetimes over composition.
    ///     A scheduler starts the shutdown protocol when the scheduler reference count goes to zero.
    /// <summary>
    unsigned int SchedulerBase::Release()
    {
        LONG val = InterlockedDecrement(&m_refCount);
        if (val == 0)
        {
            PhaseOneShutdown();
        }

        return (unsigned int)val;
    }

    /// <summary>
    ///     Causes the OS event object 'eventObject' to be set when the scheduler shuts down and destroys itself.
    /// </summary>
    /// <param name="eventObject">
    ///     [in] A valid event object.
    /// </param>
    void SchedulerBase::RegisterShutdownEvent(_In_ HANDLE eventObject)
    {
        if (eventObject == NULL || eventObject == INVALID_HANDLE_VALUE)
        {
            throw std::invalid_argument("eventObject");
        }

        HANDLE hEvent = NULL;

        if (!DuplicateHandle(GetCurrentProcess(),
                             eventObject,
                             GetCurrentProcess(),
                             &hEvent,
                             0,
                             FALSE,
                             DUPLICATE_SAME_ACCESS))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
        else
        {
            WaitNode *pNode = _concrt_new WaitNode;
            pNode->m_hEvent = hEvent;
            m_finalEvents.AddTail(pNode);
        }
    }

    /// <summary>
    ///     Attaches this scheduler to the calling thread. Implicitly calls Reference.  After this function is called,
    ///     the calling thread is then managed by the scheduler and the scheduler becomes the current scheduler.
    /// </summary>
    void SchedulerBase::Attach()
    {
        const SchedulerBase* pCurrentScheduler = FastCurrentScheduler();

        // A context is not allowed to re-attach to its current scheduler.
        if (pCurrentScheduler == this)
            throw improper_scheduler_attach();

        // Attaching to the scheduler involves incrementing reference counts on the scheduler and creating a context data structure
        // corresponding to the new scheduler for the current thread.
        ASSERT(m_internalContextCountPlusOne > 0);

        // External context creation will reference the scheduler, create the context data structure and store the context and
        // scheduler values in TLS.
        AttachExternalContext(true);

        TraceSchedulerEvent(CONCRT_EVENT_ATTACH, TRACE_LEVEL_INFORMATION, m_id);
    }

    ///<summary>
    ///     Detaches the current scheduler from the calling thread and restores the previously attached scheduler as the
    ///     current scheduler.  Implicitly calls Release().  After this function is called, the calling thread is then managed
    ///     by the scheduler that was previously activated via Create() or Attach().
    ///</summary>
    void SchedulerBase::Detach()
    {
        ContextBase* pContext = SchedulerBase::FastCurrentContext();

        ASSERT(pContext != NULL);
        if (!pContext->IsExternal())
        {
            throw improper_scheduler_detach();
        }

        ExternalContextBase* pExternalContext = static_cast<ExternalContextBase*>(pContext);

        if (!pExternalContext->WasExplicitlyAttached())
        {
            // Only contexts that attached explicitly either via the attach api, or the current scheduler creation api
            // are allowed to detach.

            throw improper_scheduler_detach();
        }

        unsigned int schedulerId = m_id;

        // External context detachment will release references on the scheduler, and remove the context from TLS on explicit detaches.
        DetachExternalContext(pExternalContext, true);

        // It is *UNSAFE* to touch 'this', after the DetachExternalContext call, since the caller does not have a reference on the
        // scheduler and it could get deleted in the meantime.
        TraceSchedulerEvent(CONCRT_EVENT_DETACH, TRACE_LEVEL_INFORMATION, schedulerId);
    }

    /// <summary>
    ///     Creates an external context and attaches it to the calling thread. Called when a thread attaches to a scheduler.
    /// </summary>
    ExternalContextBase * SchedulerBase::AttachExternalContext(bool explicitAttach)
    {
        ContextBase * pCurrentContext = SchedulerBase::FastCurrentContext();

        if (pCurrentContext != NULL)
        {
            ASSERT(explicitAttach);

            if (pCurrentContext->m_pScheduler == this)
            {
                // A context is not allowed to re-attach to its own scheduler.
                throw improper_scheduler_attach();
            }
            // Check if this is an internal context. If so, it is presumably consuming a virtual processor
            // on a different scheduler (the parent scheduler). Since a new ConcRT context will be
            // associated with the OS context during this API, the parent scheduler should get its virtual processor back.
            if (!pCurrentContext->IsExternal())
            {
                //
                // If the underlying previous context was a UMS thread, the LeaveScheduler call will result in a
                // SwitchTo(..., Nesting) to the RM which will immediately transmogrify the UMS thread into a "virtual"-thread.
                // From the perspective of the scheduler, it will behave identically to a thread.  There's more overhead in this,
                // but the functionality is identical.
                //
                static_cast<InternalContextBase *>(pCurrentContext)->LeaveScheduler();
            }
            // We also clear out the context/scheduler TLS values here. Since we've saved the current context,
            // we remember it as the parent context of the new context we're about to create. The reason for
            // clearing TLS, is that if code that executes between this point and the point at which the new
            // TLS values are setup, relies on the current context in TLS, it might behave undeterministically.
            // e.g. Creating an external context looks at the anyonymous schedule group - since the current
            // context is an internal context on the previous scheduler, we don't get what we expect..
            pCurrentContext->ClearContextTls();
        }
        // Take reference counts on the scheduler.
        ReferenceForAttach();

        ExternalContextBase * pContext = GetExternalContext(explicitAttach);

        // Save the new context into the TLS slot reserved for the same.
        pContext->PushContextToTls(pCurrentContext);

        // The thread continues to run. It does not need to wait to be scheduled on a virtual processor.
        return pContext;
    }

    /// <summary>
    ///     Detaches an external context from the scheduler. On implicit detach we assume
    ///     that this routine (and any function it calls) is short and non-blocking. See
    ///     ExternalContextBase::ImplicitDetachHandler for details
    /// </summary>
    /// <param name="pContext">
    ///     The external context being detached.
    /// </param>
    /// <param name="explicitDetach">
    ///     Whether this was the result of an explicit detach or the thread exiting.
    /// </param>
    void SchedulerBase::DetachExternalContext(ExternalContextBase * pContext, bool explicitDetach)
    {
        // External context destruction will remove the values from TLS and cleanup the context data structure
        ContextBase * pParentContext = NULL;

        if (explicitDetach)
            pParentContext = pContext->PopContextFromTls();

        ReleaseExternalContext(pContext);

        // Release reference counts on the scheduler.
        ReleaseForDetach();

        // The this pointer is *cannot be dereferenced* after the Release call.

        if ((pParentContext != NULL) && (!pParentContext->IsExternal()))
        {
            ASSERT(explicitDetach);
            // An internal parent context must be rescheduled on its scheduler.
            static_cast<InternalContextBase *>(pParentContext)->RejoinScheduler();
        }
    }

    /// <summary>
    ///     Gets an external context from the idle pool, creating a new one if the idle pool is empty
    /// </summary>
    ExternalContextBase * SchedulerBase::GetExternalContext(bool explicitAttach)
    {
        ExternalContextBase * pContext = m_externalContextPool.Pop();

        if (pContext == NULL)
        {
            pContext = _concrt_new ExternalContextBase(this, explicitAttach);
        }
        else
        {
            pContext->PrepareForUse(explicitAttach);
        }
        return pContext;
    }

    ///<summary>
    ///     Releases an external context of the to the scheduler's idle pool, destroying it if the idle pool is full.
    ///</summary>
    void SchedulerBase::ReleaseExternalContext(ExternalContextBase * pContext)
    {
        // Try to maintain the max size of the free pool somewhere close to m_virtualProcessorCount. It is
        // not an exact upper limit.
        if (m_externalContextPool.Count() < m_virtualProcessorCount)
        {
            pContext->RemoveFromUse();
            m_externalContextPool.Push(pContext);
        }
        else
        {
            _InternalDeleteHelper<ExternalContextBase>(pContext);
        }
    }

    /// <summary>
    ///     References a segment in the anonymous schedule group and returns a pointer to it.  The segment returned will be close to the specified
    ///     source location.
    /// </summary>
    /// <param name="pSrcLocation">
    ///     The returned segment will be close to this location.
    /// </param>
    /// <returns>
    ///     A segment in the anonymous schedule group located close to srcLocation.
    /// </returns>
    ScheduleGroupSegmentBase *SchedulerBase::GetAnonymousScheduleGroupSegment()
    {
        return GetNextSchedulingRing()->GetAnonymousScheduleGroupSegment();
    }

    /// <summary>
    ///     Create a schedule group within this scheduler.
    /// </summary>
    /// <param name="pPlacement">
    ///     A poointer to a location where tasks within the schedule group will be biased towards executing at.
    /// </param>
    /// <returns>
    ///     A reference to a new ScheduleGroup.
    /// </returns>
    ScheduleGroup* SchedulerBase::InternalCreateScheduleGroup(location* pPlacement)
    {
        ScheduleGroupBase *pGroup = m_scheduleGroups.PullFromFreePool();
        if (m_schedulingProtocol == ::Concurrency::EnhanceScheduleGroupLocality)
        {
            if (pGroup == NULL)
            {
                pGroup = _concrt_new CacheLocalScheduleGroup(this, pPlacement);
            }
            else
            {
                pGroup->Initialize(pPlacement);
            }
        }
        else
        {
            //
            // Fair schedule groups completely ignore any hint of placement.  We are directed to round robin through groups anyway!
            //
            location unbiased;
            if (pGroup == NULL)
            {
                pGroup = _concrt_new FairScheduleGroup(this, &unbiased);
            }
            else
            {
                pGroup->Initialize(&unbiased);
            }
        }
        m_scheduleGroups.Add(pGroup);
        ASSERT(pGroup->m_refCount >= 0);
        return pGroup;
    }

    /// <summary>
    ///     Removes an unreferenced schedule group from the scheduler's list of groups.
    /// </summary>
    void SchedulerBase::RemoveScheduleGroup(ScheduleGroupBase *pGroup)
    {
        //
        // Essentially m_refCount is the refcount of how many contexts reference this schedule group.
        // m_refCount is incremented whenever a schedule group is associated with a context, decremented
        // when a context is switched out to another context.  When workstealing happens the context that
        // steals from this schedule group increments m_refCount and decrements after it is done with this
        // schedule group, whether by stealing from another or by switching out and going back on the
        // freelist.  When m_refCount hits 0, then the schedule group is taken out of m_scheduleGroups and hence
        // is not searched for runnables or stolen chores.
        //
        ASSERT(pGroup != NULL && pGroup->m_refCount == 0);

        // It is still in the circular list, but other apis that are traversing the list will move over
        // it, since it will not have any runnable contexts or chores to steal.

        m_scheduleGroups.Remove(pGroup);
    }

    /// <summary>
    ///     Schedules a light-weight task within the scheduler. The light-weight task will be placed within a schedule group of
    ///     the runtime's choosing.
    /// </summary>
    /// <param name="proc">
    ///     A pointer to the main function of a task.
    /// </param>
    /// <param name="data">
    ///     A void pointer to the data that will be passed in to the task.
    /// </param>
    void SchedulerBase::ScheduleTask(TaskProc proc, void *data)
    {
        ScheduleGroupBase *pGroup = NULL;

        //
        // If we are currently executing in the context of a particular schedule group, the work should go there instead of in an anonymous schedule group.
        // We should not easily create cross-group dependencies.
        //
        // Note that it is entirely possible that the current context is on a DIFFERENT scheduler (e.g.: we're scheduling work on scheduler 2 within a thread
        // bound to scheduler 1).  In this case, we do **NOT** want to look at the current group -- we just pick an **appropriate** anonymous group within
        // scheduler 1.
        //
        ContextBase *pCurrentContext = FastCurrentContext();
        if (pCurrentContext != NULL && pCurrentContext->GetScheduler() == this)
        {
            pGroup = pCurrentContext->GetScheduleGroup();
            ASSERT(pGroup != NULL);
        }
        else
        {
            //
            // This task does not have an associated schedule group, assign it to the anonymous
            // schedule group for the scheduler
            //
            pGroup = GetAnonymousScheduleGroup();
        }
        pGroup->ScheduleTask(proc, data);
    }

    /// <summary>
    ///     Schedules a light-weight task within the scheduler. The light-weight task will be scheduled at the specified priority and placed within
    ///     a schedule group of the runtime's choosing. It will also be biased towards executing at the specified location.
    /// </summary>
    /// <param name="proc">
    ///     A pointer to the function to execute to perform the body of the light-weight task.
    /// </param>
    /// <param name="data">
    ///     A void pointer to the data that will be passed as a parameter to the body of the task.
    /// </param>
    /// <param name="placement">
    ///     A reference to a location where the light-weight task will be biased towards executing at.
    /// </param>
    void SchedulerBase::ScheduleTask(TaskProc proc, void * data, location& placement)
    {
        ScheduleGroupBase *pGroup = NULL;

        //
        // If we are currently executing in the context of a particular schedule group, the work should go there instead of in an anonymous schedule group.
        // We should not easily create cross-group dependencies.
        //
        // Note that it is entirely possible that the current context is on a DIFFERENT scheduler (e.g.: we're scheduling work on scheduler 2 within a thread
        // bound to scheduler 1).  In this case, we do **NOT** want to look at the current group -- we just pick an **appropriate** anonymous group within
        // scheduler 1.
        //
        ContextBase *pCurrentContext = FastCurrentContext();
        if (pCurrentContext != NULL && pCurrentContext->GetScheduler() == this)
        {
            pGroup = pCurrentContext->GetScheduleGroup();
            ASSERT(pGroup != NULL);
        }
        else
        {
            //
            // This task does not have an associated schedule group, assign it to the anonymous
            // schedule group for the scheduler
            //
            pGroup = GetAnonymousScheduleGroup();
        }
        pGroup->ScheduleTask(proc, data, placement);
    }

    /// <summary>
    ///     Determines whether a given location is available on the scheduler.
    /// </summary>
    /// <param name="_Placement">
    ///     A reference to the location to query the scheduler about.
    /// </param>
    /// <returns>
    ///     An indication of whether or not the location specified by the <paramref name="_Placement"/> argument is available on the scheduler.
    /// </returns>
    /// <remarks>
    ///     Note that the return value is an instantaneous sampling of whether the given location is available.  In the presence of multiple
    ///     schedulers, dynamic resource management may add or take away resources from schedulers at any point.  Should this happen, the given
    ///     location may change availability.
    /// </remarks>
    bool SchedulerBase::IsAvailableLocation(const location& _Placement) const
    {
        // @TODO: The const_cast shouldn't be here.  We need to propagate const to more places (here & collections, etc...)
        QuickBitSet set = const_cast<SchedulerBase *>(this)->GetBitSet(&_Placement);
        return set.Intersects(m_activeSet);
    }

    /// <summary>
    ///     Initialize event handlers or background threads
    /// </summary>
    void SchedulerBase::InitializeSchedulerEventHandlers()
    {
        //
        // The utilization of the thread-pool for background throttling needs reference on the scheduler as an "event-handler".  It will go away
        // at the time event handlers are destroyed.
        //
        IncrementInternalContextCount();

    }

    /// <summary>
    ///     Destroy event handlers or background threads
    /// </summary>
    void SchedulerBase::DestroySchedulerEventHandlers()
    {
#ifndef _UMS_DISABLED
        //
        // Ensure that background throttling processing is completed and will never be invoked again by the time this routine exits!
        //
        UnregisterWaitEx(m_hThrottlingWait, INVALID_HANDLE_VALUE);
#endif // _UMS_DISABLED

        if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
        {
            DeleteAsyncTimerAndUnloadLibrary(static_cast<PTP_TIMER>(m_hThrottlingTimer));
        }
        else
        {
            platform::__DeleteTimerQueueTimer(NULL, m_hThrottlingTimer, INVALID_HANDLE_VALUE);
        }

#ifndef _UMS_DISABLED
        CloseHandle(m_hThrottlingEvent);
#endif // _UMS_DISABLED

        OMTRACE(MTRACE_EVT_EVENTHANDLERSDESTROYED, this, NULL, NULL, NULL);

        //
        // The UnregisterWait and DeleteTimerQueueTimer should give us the guarantees we need to make this safe.
        //
        DecrementInternalContextCount();
    }

    //  Finalization:
    //
    // The active vproc count and idle vproc count are encoded into the vproc shutdown gate, in addition to the SHUTDOWN and SUSPEND flags.
    //
    //                 SHUTDOWN_INITIATED_FLAG (0x80000000)
    //                   |
    //                   |
    //                   |         <----- 29 bit gate count ------>
    //                 +---+---+---+------------------------------+
    //                 | i | g | c | encoded active/idle count    |
    //                 +---+---+---+------------------------------+
    //                       |   |
    //                       |   |
    //                       |  SHUTDOWN_COMPLETED_FLAG (0x20000000)
    //                       |
    //                    SUSPEND_GATE_FLAG (0x40000000)
    //
    // The gate count is 29 bits that encodes both the active and idle vproc counts. NOTE: If we have more than half a billion vprocs,
    // this needs to change.  Somehow, I'm doubting that, but it's a hard limit of the way this gate functions.
    //
    // Once the SHUTDOWN_INITIATED bit on the shutdown gate is set (all external references to the scheduler are released),
    // the scheduler proceeds into a 'sweep' phase iff all active virtual processors in the scheduler are idle (gate count is 0). The
    // sweep suspends all virtual processors and looks for blocked contexts in the scheduler. If any are found, finalization is rolled
    // back and virtual processors are unsuspended and allowed to proceed. In addition, virtual processor transitions from active to idle
    // and active to inactive, that bring the gate count to 0, also trigger a sweep and can lead to finalization.
    //
    // A virtual processor notifies the scheduler that it is ACTIVE when it is started up by the scheduler in response to incoming work.
    // From the ACTIVE state it could transition to IDLE and back to ACTIVE several times before either the scheduler shuts down or the
    // in the individual virtual processor is asked to retire. At that point it moves from ACTIVE to INACTIVE.
    //
    //                VirtualProcessorActive(true)              VirtualProcessorIdle(true)
    //                                     |                         |
    //                                     |                         |
    //                                  ------------>        ------------->
    //                         INACTIVE               ACTIVE                IDLE
    //                                  <------------        <-------------
    //                                     |                         |
    //                                     |                         |
    //               VirtualProcessorActive(false)              VirtualProcessorIdle(false)
    //
    //
    // The figure below shows how a single virtual processor affects the gate count value(GC) as it changes state.
    //
    //
    //                   VirtualProcessorActive(false)    VirtualProcessorActive(true)
    //                                         |             |
    //                                         |             |
    //                                       <--------  --------->
    //                                    GC-1        GC        GC+1
    //                                       <--------  --------->
    //                                         |              |
    //                                         |              |
    //                     VirtualProcessorIdle(true)       VirtualProcessorIdle(false)
    //
    // Virtual processors should never be able to produce a gate count of -1 if they transition through valid states.
    // Therefore the increment/decrement operations can be done simply using InterlockedIncrement/InterlockedDecrement
    // without fear of underflowing the 29 bit count into the flag area. (This is all assuming there are not > 1/2 billion
    // vprocs, of course).
    //
    // When the number of active vprocs is equal to the number of idle vprocs the gate count is 0.

    ///<summary>
    ///     Called to initiate shutdown of the scheduler.  This may directly proceed to phase two of shutdown (actively
    ///     shutting down internal contexts) or it may wait for additional events (e.g.: all work to complete) before
    ///     proceeding to phase two.
    ///</summary>
#pragma warning(disable: 4702) // unreachable code
    void SchedulerBase::PhaseOneShutdown()
    {
        OMTRACE(MTRACE_EVT_PHASEONESHUTDOWN, this, NULL, NULL, NULL);

        // If this scheduler is the default scheduler, it should be cleared here, since we do not want any external contexts attaching
        // to it via the CurrentScheduler interface. In the meantime, all calls to GetDefaultScheduler will perform a safe reference which
        // will fail.
        if (s_pDefaultScheduler == this)
        {
            _StaticLock::_Scoped_lock lockHolder(s_defaultSchedulerLock);

            if (s_pDefaultScheduler == this)
            {
                s_pDefaultScheduler = NULL;
            }
        }

        ASSERT(m_internalContextCountPlusOne >= 1);

        // Check if the internal reference count is greater than 1. This is an optimistic check - it could go down to 1 immediately
        // after the check - this just means we will attempt to shutdown virtual processors even though there may be none left by the
        // time we try. Note, that with our implementation today this will not happen, if virtual processors were started up previously,
        // the contexts that executed on them retain their internal references on the scheduler, keeping m_internalContextCountPlusOne > 1,
        // until a finalization sweep completes successfully. However, even if the implementation changes, this optimistic check, at worst,
        // causes us to do slightly more work here. Could it increase after the check if we found it to be 1? Not without a bug
        // in user code, and we're not hardening ourself against these. (The external facing reference count is 0 at this point, and we
        // have the only reference to the scheduler.)
        //
        // The reason we don't decrement the value m_internalContextCountPlusOne, and check if the result is non-zero, to detect that
        // there are contexts/virtual processors out there, is that the moment we do that, the scheduler could disappear out
        // from under us. Consider the case where we decremented the internal count from 5 to 4, then proceeded to set the SHUTDOWN_INITIATED_FLAG
        // bit, and noticed that the gate count was 0. Now there is nothing stopping a virtual processor from becoming active
        // while the SHUTDOWN_INITIATED_FLAG bit is set. If it quickly became inactive (bringing the Gate count to zero once again),
        // we could have two callers simultaneously executing AttemptSchedulerSweep. One would win, and go on to the sweep and finalize
        // the scheduler, the other could be left touching garbage memory. This is why decrementing the ref count m_internalContextCountPlusOne
        // is the last thing we should do in this function - this keeps the scheduler alive as long as we're executing PhaseOneShutdown.
        //

        if (m_internalContextCountPlusOne > 1)
        {
            //
            // Signal a start of shutdown.
            //
            // At this point, we cannot finalize the scheduler.  Instead, we need to wait for all work to complete
            // before performing such finalization.  This means any realized chores, blocked contexts, etc...  We do
            // this by waiting for all virtual processors to become idle (no more unblocked work) and then suspending
            // all the virtual processors (they're idle anyway) and sweeping the scheduler looking for blocked contexts.
            // If none are found, the scheduler can be finalized.
            //
            LONG oldVal = m_vprocShutdownGate;
            LONG xchgVal = 0;

            for(;;)
            {
                xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal | SHUTDOWN_INITIATED_FLAG, oldVal);
                if (xchgVal == oldVal)
                    break;

                oldVal = xchgVal;
            }

            ASSERT((oldVal & GATE_FLAGS_MASK) == 0);

            if ((oldVal & GATE_COUNT_MASK) == 0)
            {
                AttemptSchedulerSweep();
            }
        }

        // There is an 'extra' internal context count per scheduler that must be released by every call to PhaseOneShutdown on the scheduler.
        // Resurrecting a scheduler must resurrect this count as well.
        DecrementInternalContextCount();
    }
#pragma warning(default: 4702)

    /// <summary>
    ///     Invoked when the Gate Count goes to zero as a result of virtual processor state transitions, while the
    ///     scheduler has been marked for shutdown. It proceeds to sweep the scheduler if it can set the suspend flag
    ///     on the shutdown gate while the gate count is still 0 and the scheduler is marked for shutdown.
    /// </summary>
    void SchedulerBase::AttemptSchedulerSweep()
    {
        //
        // If we're actively in a shutdown semantic, start a handshake to ensure that no virtual processor
        // comes out of idle until we've swept the scheduler.
        //
        LONG oldVal = SHUTDOWN_INITIATED_FLAG; // Gate Count was found to be 0 and suspend bit cannot be set.

        LONG xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal | SUSPEND_GATE_FLAG, oldVal);
        if (xchgVal == oldVal)
        {
            //
            // At this point no vprocs can become active even if they found work - they will stall on
            // the suspend gate when they make the transition from idle -> active.
            //
            SweepSchedulerForFinalize();
            //
            // NOTE: After this point, the *this* pointer is no longer valid and must not be dereferenced.
            //
        }
        else
        {
            //
            // One of three things could've happened here:
            //   - One of the idle vprocs became active before we could set the suspend gate flag
            //   - The scheduler was resurrected and vproc resurrecting the scheduler managed to become idle again.
            //   - There was a concurrent call to AttemptSchedulerSweep (see comments in PhaseOneShutdown) which
            //     succeeded in setting the suspend bit before we did, and probably completed finalization as well.
            //
            ASSERT(((xchgVal & GATE_COUNT_MASK) != 0) || ((xchgVal & GATE_FLAGS_MASK) == 0) ||
                   ((xchgVal & GATE_FLAGS_MASK) == (SHUTDOWN_INITIATED_FLAG | SUSPEND_GATE_FLAG)) ||
                   ((xchgVal & GATE_FLAGS_MASK) == (SHUTDOWN_INITIATED_FLAG | SHUTDOWN_COMPLETED_FLAG)));
        }
    }

    /// <summary>
    ///     Called during scheduler finalization, to check if any chores exist in the scheduler.
    /// </summary>
    /// <returns>
    ///     A boolean value indicating whether any unstarted chores (realized or unrealized) were found.
    /// </returns>
    bool SchedulerBase::FoundUnstartedChores()
    {
        ASSERT((m_vprocShutdownGate & SHUTDOWN_INITIATED_FLAG) != 0);
        ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) != 0);

        bool choresFound = false;

        for (int idx = 0; idx < m_nodeCount; ++idx)
        {
            SchedulingRing *pRing = m_rings[idx];
            // For each ring, go through all schedule groups

            if (pRing != NULL)
            {
                for (int i = 0; i < pRing->m_affineSegments.MaxIndex(); i++)
                {
                    ScheduleGroupSegmentBase *pSegment = pRing->m_affineSegments[i];
                    if (pSegment != NULL)
                    {
                        if (pSegment->HasRealizedChores() || pSegment->HasUnrealizedChores())
                        {
                            choresFound = true;
                            break;
                        }
                    }
                }

                if (choresFound)
                    break;

                for(int i = 0; i < pRing->m_nonAffineSegments.MaxIndex(); i++)
                {
                    ScheduleGroupSegmentBase *pSegment = pRing->m_nonAffineSegments[i];
                    if (pSegment != NULL)
                    {
                        if (pSegment->HasRealizedChores() || pSegment->HasUnrealizedChores())
                        {
                            choresFound = true;
                            break;
                        }
                    }
                }
            }
        }
        return choresFound;
    }

    /// <summary>
    ///     Called during scheduler finalization, to check if any blocked contexts exist in the scheduler.
    /// </summary>
    /// <returns>
    ///     A boolean value indicating whether any blocked contexts were found.
    /// </returns>
    bool SchedulerBase::FoundBlockedContexts()
    {
        ASSERT((m_vprocShutdownGate & SHUTDOWN_INITIATED_FLAG) != 0);
        ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) != 0);

        bool blockedContextsFound = false;

        //
        // No new contexts shall be added to the m_allContexts list at this point. Since
        // all the vprocs are guaranteed to be idle, there should be no one executing on
        // a vproc (even though there could be contexts that are blocked). Any background
        // thread in the scheduler that adds a context needs to synchronize with sweep to
        // ensure that contexts are not added while the sweep is in progress. For example,
        // the UT creation thread in UMS fails the sweep if there are pending requests to
        // add new reserved contexts.
        //
        ContextNode *pNode = m_allContexts.Unsafe_Top();
        while (pNode != NULL)
        {
            //
            // Work consists of active contexts that are blocked.
            //
            if ((!pNode->m_pContext->IsIdle()) && pNode->m_pContext->IsBlocked())
            {
                ASSERT(pNode->m_pContext->IsBlocked());
                blockedContextsFound = true;
                break;
            }

            pNode = LockFreePushStack<ContextNode>::Next(pNode);
        }

        return blockedContextsFound;
    }

    ///<summary>
    ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
    ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown.
    ///</summary>
    SchedulerBase::PendingWorkType SchedulerBase::TypeOfWorkPending()
    {
        if (FoundBlockedContexts() || FoundUnstartedChores())
            return UserWork;

        //
        // Deferred creates from the throttler count as work pending so that it does not race with finalization.
        //
        if (m_pendingDeferredCreates > 0)
            return OnlyAncillaryWork;

        return NoWork;
    }

    /// <summary>
    ///     Releases the list of reserved contexts to the idle pool.  The thread proxy
    ///     is released before returning the contexts to the idle pool.
    /// </summary>
    void SchedulerBase::ReleaseReservedContexts()
    {
        InternalContextBase *pContext = m_reservedContexts.Pop();

        while (pContext != NULL)
        {
            GetSchedulerProxy()->UnbindContext(pContext);
            ReleaseInternalContext(pContext);

            pContext = m_reservedContexts.Pop();
        }
    }

    ///<summary>
    ///     Cancel all the internal contexts.
    ///</summary>
    void SchedulerBase::CancelAllContexts()
    {
        //
        // We need to be in a hypercritical region (this code path shall not rely
        // on another UT to be scheduled).
        //
        ContextBase::StaticEnterHyperCriticalRegion();
        ReleaseReservedContexts();
        ContextBase::StaticExitHyperCriticalRegion();

        ContextNode *pNode = m_allContexts.Unsafe_Top();
        while ( pNode != NULL)
        {
            pNode->m_pContext->Cancel();
            pNode = LockFreePushStack<ContextNode>::Next(pNode);
        }
    }

    ///<summary>
    ///     Once all virtual processors are idle, the scheduler calls this routine which performs a full sweep through all
    ///     schedule groups looking for work.  If work is found (even a blocked context), the scheduler backs off finalization;
    ///     otherwise, it proceeds with phase two of shutdown, which cancels all contexts running in their dispatch loops,
    ///     as well as any background threads that exist.
    ///</summary>
    void SchedulerBase::SweepSchedulerForFinalize()
    {
        OMTRACE(MTRACE_EVT_SCHEDULERSWEEP, this, NULL, NULL, m_vprocShutdownGate);

        ASSERT((m_vprocShutdownGate & SHUTDOWN_INITIATED_FLAG) != 0);
        ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) != 0);

        ContextBase *pContext = FastCurrentContext();
        bool fExternal = true;

        if ((pContext != NULL) && (!pContext->IsExternal()))
        {
            fExternal = false;
            pContext->EnterCriticalRegion();
        }

        //
        // Once we get in the sweep, and the SUSPEND_GATE_FLAG is set, no virtual processor can start searching for work,
        // without being gated by us. At this point we expect all virtual processors (except this one, if this is a context
        // running on a virtual processor) to be deactivated via (vproot->Deactivate). Even if work was added and one of
        // these was activated again (via vproot->Activate), they would have to reduce the idle count (via VirtualProcessorIdle(false)),
        // before they actually searched the scheduler queues for work. If they did, they would be suspended on the shutdown
        // gate semaphore. We need to make one final pass for blocked contexts and chores before deciding whether the scheduler
        // can be finalized. If either is found, we need to roll back.
        //
        PendingWorkType workType = TypeOfWorkPending();
        if (workType == NoWork)
        {
            //
            // If there are no blocked contexts and no chores, we can safely proceed to PhaseTwoShutdown.
            //
            PhaseTwoShutdown();
        }
        else
        {
            if (workType == OnlyAncillaryWork && !m_fSweepWithoutActualWork)
            {
                //
                // Make sure the throttler doesn't dawdle forever by indicating the lack of actual work.
                //
                InterlockedExchange(&m_fSweepWithoutActualWork, TRUE);
            }

            //
            // There's work, we need to let everyone go and try again on the next active to idle transition, or
            // the next active to inactive transition.
            //
            LONG xchgVal = m_vprocShutdownGate;
            LONG oldVal = xchgVal;

            do
            {
                oldVal = xchgVal;
                xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal & ~SUSPEND_GATE_FLAG, oldVal);
            }
            while (xchgVal != oldVal);

            //
            // Some virtual processors may have tried to transition from idle to active and been suspended by the gate.
            // The number of these vprocs should be the value of the gate count at the time we clear the suspend flag.
            // The flag was set at a time the gate count was 0, and from there, it could only have transitioned to a
            // positive value. (+1 for each vproc that tried to go idle -> active and was suspended on the gate).
            // This is how many virtual processors we need to wake up from suspend.
            //
            ReleaseSuspendedVirtualProcessors(xchgVal & GATE_COUNT_MASK);
        }

        if (!fExternal) pContext->ExitCriticalRegion();
    }

    /// <summary>
    ///     Releases virtual processors that were suspended on the shutdown gate, while trying to go from IDLE to
    ///     ACTIVE when the finalization sweep was in progress.
    /// </summary>
    /// <param name="releaseCount">
    ///     Number of virtual processors that need to be released.
    /// </param>
    void SchedulerBase::ReleaseSuspendedVirtualProcessors(LONG releaseCount)
    {
        if (releaseCount > 0)
        {
            ReleaseSemaphore(m_hSchedulerShutdownSync, releaseCount, NULL);
        }
    }

    ///<summary>
    ///     Called when a virtual processor becomes active (before it does) or becomes inactive (before it does).
    ///</summary>
    ///<param value="fActive">
    ///     True if a virtual processor is going from INACTIVE to ACTIVE, and false if it is going from ACTIVE to INACTIVE.
    ///</param>
    ///<returns>
    ///     For activation, the function returns true if the virtual processor was successfully activated, and false
    ///     if it could not be activated because the scheduler was shutting down. For inactivation, it always returns true.
    ///</returns>
    bool SchedulerBase::VirtualProcessorActive(bool fActive)
    {
        OMTRACE(MTRACE_EVT_VPROCACTIVE, this, NULL, NULL, fActive);

        //
        // It is possible for a virtual processor to become active while the finalization sweep is in progress. A thread external
        // to the scheduler could unblock a scheduler context, and activate a virtual processor, if one is found to be available.
        //
        // However, it is not possible for a virtual processor to become inactive during the sweep. This is because the active
        // and idle vproc counts were equal at the time the bit was set (gate count was 0), and there are no external contexts
        // attached to the scheduler. To become inactive, a virtual processor would first have to go idle -> active, and would
        // block on the suspend gate.
        //

        //
        // A virtual processor increments the shutdown gate count on going from inactive to active, and decrements it
        // on going from active to inactive.
        // It also updates the active count - the gate count is used to synchronize with finalization, but the active count
        // is needed when the finalization code needs to suspend virtual processors.
        //
        if (fActive)
        {
            // We need to spin while the suspend bit is set, if the scheduler is in the sweep phase. If the scheduler finds a blocked
            // context, it will rollback finalization and allow us to proceed. If it enters phase two shutdown, it will set a bit
            // indicating that shutdown is complete and we should fail this call. We are guaranteed that the scheduler will not
            // be deleted, in that we are safe in accessing its data members during this call, since every path that calls this API
            // and does not already have a reference on the scheduler, employs a different means of synchronization to ensure the
            // scheduler stays around. An example of this is the AddRunnableContext API in CacheLocalScheduleGroup.
            //
            // We also need to increment the shutdown gate while ensuring that the suspend bit is not set, therefore we use a
            // compare exchange instead of an interlocked increment.
            LONG xchgVal = m_vprocShutdownGate;
            LONG oldVal = xchgVal;

            do
            {
                oldVal = xchgVal;

                if ((oldVal & SUSPEND_GATE_FLAG) != 0)
                {
                    // If the suspend bit was set in the meantime, we need to spin again until it is unset. Same logic as above applies;
                    // a blocked context exists, and the scheduler will roll back finalization.
                    oldVal = SpinUntilBitsReset(&m_vprocShutdownGate, SUSPEND_GATE_FLAG);
                }
                if ((oldVal & SHUTDOWN_COMPLETED_FLAG) != 0)
                {
                    // Scheduler has shutdown and we cannot activate virtual processors anymore.
                    OMTRACE(MTRACE_EVT_VPROCACTIVE, this, NULL, NULL, 2);
                    return false;
                }

                xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal + 1, oldVal);
            }
            while (xchgVal != oldVal);

            ASSERT((oldVal & SUSPEND_GATE_FLAG) == 0);
            ASSERT((oldVal & SHUTDOWN_COMPLETED_FLAG) == 0);

            LONG activeCount = InterlockedIncrement(&m_activeVProcCount);
            VirtualProcessorActiveNotification(fActive, activeCount);
        }
        else
        {
            ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) == 0);
            ASSERT((m_activeVProcCount != 0) && ((m_vprocShutdownGate & GATE_COUNT_MASK) != 0));

            LONG activeCount = InterlockedDecrement(&m_activeVProcCount);
            VirtualProcessorActiveNotification(fActive, activeCount);

            LONG val = InterlockedDecrement(&m_vprocShutdownGate);
            //
            // The act of going inactive could potentially make the active and idle vproc counts equal and should try initiate finalization
            // if the shutdown initiated flag is set.
            //
            if (((val & GATE_COUNT_MASK) == 0) && ((val & SHUTDOWN_INITIATED_FLAG) != 0))
            {
                // The suspend flag should not bet set.
                ASSERT(val == SHUTDOWN_INITIATED_FLAG);
                AttemptSchedulerSweep();
            }
        }

        return true;
    }

    ///<summary>
    ///     Internal contexts call the scheduler right before they deactivate their virtual processors and sleep indefinitely
    ///     until more work enters the scheduler, in order to allow things that happen on scheduler idle to happen (e.g.: sweeping
    ///     for phase two shutdown).
    ///
    ///     They also call the scheduler when they transition out of idle before starting to search the scheduler for work, if
    ///     they underlying virtual processor was re-activated as a result of new work entering. This may halt scheduler shutdown
    ///     or it may coordinate with scheduler shutdown depending on the current phase of shutdown.
    ///
    ///     This call *MUST* be made from a scheduler critical region.
    ///</summary>
    void SchedulerBase::VirtualProcessorIdle(bool fIdle)
    {
        OMTRACE(MTRACE_EVT_VPROCIDLE, this, NULL, NULL, fIdle);

        //
        // There shall be no blocking operations during SchedulerSweep. If they do and we are forced to make a scheduling decision (such as
        // in UMS), it is a recipe for deadlock. (Scheduling decision => activate idle virtual processor => wait for sweep to complete => deadlock).
        // We will enter a hypercritical region to enforce it. Any blocking operation that require another thread to be scheduled will immediately
        // deadlock.
        //
        ContextBase::StaticEnterHyperCriticalRegion();

        //
        // A virtual processor decrements the shutdown gate count when it goes from active to idle and increments it
        // when it goes from idle to active.
        //
        if (fIdle)
        {
            ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) == 0);
            ASSERT((m_vprocShutdownGate & GATE_COUNT_MASK) != 0);
            LONG val = InterlockedDecrement(&m_vprocShutdownGate);
            //
            // The act of going idle could potentially make the active and idle vproc counts equal and should try initiate finalization
            // if the shutdown in progres bit flag is set.
            //
            if (((val & GATE_COUNT_MASK) == 0) && ((val & SHUTDOWN_INITIATED_FLAG) != 0))
            {
                // The suspend flag should not bet set.
                ASSERT(val == SHUTDOWN_INITIATED_FLAG);
                AttemptSchedulerSweep();
            }
        }
        else
        {
            LONG val = InterlockedIncrement(&m_vprocShutdownGate);
            //
            // If a virtual processor is trying to go from idle to active while a scheduler sweep is in progress, it must be
            // forcefully suspended until the scheduler has decided whether it needs to rollback or continue with finalization.
            //
            if ((val & SUSPEND_GATE_FLAG) != 0)
            {
                //
                // For UMS, this will trigger return to primary.  We're in a critical section, so we won't
                // be able to observe it.
                //
                WaitForSingleObjectEx(m_hSchedulerShutdownSync, INFINITE, FALSE);
            }
        }

        ContextBase::StaticExitHyperCriticalRegion();
    }

    ///<summary>
    ///     Called to perform a resurrection of the scheduler.  When the scheduler reference count has fallen to zero,
    ///     it's possible there's still work on the scheduler and that one of those work items will perform an action
    ///     leading to additional reference.  Such bringing of the reference count from zero to non-zero is only legal
    ///     on an *INTERNAL* context and immediately halts shutdown.
    ///</summary>
    void SchedulerBase::Resurrect()
    {
        //
        // If we got here, someone is going to flag shutdown triggering a whole slew of stuff.  We need to ensure
        // that that guy progresses to the point where the shutdown_initiated_flag gets set and *THEN* clear it.  Hence,
        // spin for a while waiting for the thread which released to finish setting the flag.  Subsequently,
        // we clear it.  The original thread will not sweep (as there's a non idle vproc by definition if we
        // get here).
        //

        LONG val = SpinUntilBitsSet(&m_vprocShutdownGate, SHUTDOWN_INITIATED_FLAG);
        ASSERT((val & SHUTDOWN_INITIATED_FLAG) != 0);

        while(true)
        {
            LONG xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, val & ~SHUTDOWN_INITIATED_FLAG, val);
            if (xchgVal == val)
                break;

            val = xchgVal;
        }

        //
        // As this had to have happened from an internal context, that fact alone should guarantee that we weren't in the
        // middle of a sweep or moving forward finalization.  Further, it also makes this guaranteed safe -- our context
        // still holds one reference on the scheduler.  This cannot have dropped to zero yet.
        //
        IncrementInternalContextCount();
    }

    ///<summary>
    ///     Actively informs all internal contexts to exit and breaks them out of their dispatch loops.  When the last
    ///     internal context dies, finalization will occur and we move to SchedulerBase::Finalize().
    ///</summary>
    void SchedulerBase::PhaseTwoShutdown()
    {
        OMTRACE(MTRACE_EVT_PHASETWOSHUTDOWN, this, NULL, NULL, NULL);

        ContextBase * pContext = SchedulerBase::FastCurrentContext();
        bool fExternal = (pContext == NULL || pContext->IsExternal() || (pContext->GetScheduler() != this));

        // If this is not an internal context belonging to this scheduler, we need to take a reference here to be safe.
        // In the course of phase two shutdown, we may end up canceling all contexts, which may bring the reference
        // count to zero and finalize the scheduler while we're still releasing a lock or something.
        if (fExternal)
            IncrementInternalContextCount();

        //
        // Fully commit all safe points.
        //
        CommitToVersion(0);

        //
        // Cancel all contexts. For contexts that are running atop virtual processors, we must activate the virtual processors,
        // so that the contexts can exit their dispatch loops. For the remaining contexts, we must perform a certain amount of
        // cleanup, such as decrementing the reference counts they hold on the scheduler.
        //
        // Contexts *must* be canceled before the suspend bit is reset. This is so that any calls to VirtualProcessorIdle(false)
        // by contexts in their dispatch loops are blocked on the suspend gate until *after* the context is canceled. That way,
        // when they are un-suspended, they will immediately exit their dispatch loop, as we intend for them to. Calls to
        // VirtualProcessorIdle(false) could result as a race between adding a work item to the scheduler (and subsequently
        // trying to activate a virtual processor), and the scheduler shutting down due to a different virtual processor finding
        // and executing that work item and subsequently going idle.
        //
        CancelAllContexts();

        //
        // PhaseTwoShutdown is executed when the scheduler has confirmed that no work, in the form of blocked contexts, exists, and that
        // all active virtual processors are idle. However, it is possible that a foregin thread is trying to activate a virtual processor
        // in this scheduler. This could be due to a race while adding runnable contexts. We need to mark the scheduler such that the
        // vproc addition apis can fail gracefully. Atomically clear the suspend bit and set the shutdown completed bit.
        //
        // This needs to be done in a loop, since changes to the gate count from calls to VirtualProcessorIdle(false) are possible.
        //
        LONG xchgVal = m_vprocShutdownGate;
        LONG oldVal = 0;

        do
        {
            oldVal = xchgVal;
            ASSERT((oldVal & SHUTDOWN_INITIATED_FLAG) != 0);
            ASSERT((oldVal & SUSPEND_GATE_FLAG) != 0);

            xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, (oldVal | SHUTDOWN_COMPLETED_FLAG) & ~SUSPEND_GATE_FLAG, oldVal);
        }
        while (xchgVal != oldVal);

        //
        // Some virtual processors may have tried to transition from idle to active and been suspended by the gate.
        // The number of these vprocs should be the value of the gate count at the time we clear the suspend flag.
        // The flag was set at a time the gate count was 0, and from there, it could only have transitioned to a
        // positive value. (+1 for each vproc that tried to go idle -> active and was suspended on the gate).
        // This is how many virtual processors we need to wake up from suspend.
        //
        ReleaseSuspendedVirtualProcessors(xchgVal & GATE_COUNT_MASK);

        //
        // Cancel all background event handlers
        //
        DestroySchedulerEventHandlers();

        if (fExternal) DecrementInternalContextCount();
    }

    /// <summary>
    ///     Returns true if the scheduler has gone past a certain point in PhaseTwoShutdown (when it sets the shutdown completed flag).
    ///     This function is mainly used for debug asserts.
    /// </summary>
    bool SchedulerBase::HasCompletedShutdown()
    {
        LONG shutdownFlags = SHUTDOWN_INITIATED_FLAG | SHUTDOWN_COMPLETED_FLAG;
        return ((m_vprocShutdownGate & shutdownFlags) == shutdownFlags);
    }

    /// <summary>
    ///     Returns true if the scheduler is in the finalization sweep, i.e, the SUSPEND_GATE_FLAG is set.
    ///     This function is mainly used for debug asserts.
    /// </summary>
    bool SchedulerBase::InFinalizationSweep()
    {
        LONG shutdownFlags = SHUTDOWN_INITIATED_FLAG | SUSPEND_GATE_FLAG;
        return ((m_vprocShutdownGate & shutdownFlags) == shutdownFlags);
    }

    /// <summary>
    ///     Called to finalize the scheduler.
    /// </summary>
    void SchedulerBase::Finalize()
    {
        OMTRACE(MTRACE_EVT_FINALIZATION, this, NULL, NULL, NULL);

        // The scheduler resources should be given back to RM before setting the
        // shutdown events. Waiters should be able to create a new scheduler and
        // get the resources this scheduler released.
        //
        // Note that this should happen prior to deleting contexts.  We might be
        // on a UMS thread finalizing which might need to perform lock validation
        // of other UMS related things which require m_pContext not to be toasted
        // underneath the UMS thread.
        //
        m_pSchedulerProxy->Shutdown();
        CloseHandle(m_hSchedulerShutdownSync);

        // Delete all the internal contexts
        ContextNode *pNode = m_allContexts.Flush();
        while ( pNode != NULL)
        {
            ContextNode *pNextNode = LockFreePushStack<ContextNode>::Next(pNode);
            _InternalDeleteHelper<InternalContextBase>(pNode->m_pContext);
            delete pNode;
            pNode = pNextNode;
        }

        ExternalContextBase *pContext = m_externalContextPool.Flush();
        while (pContext != NULL)
        {
            ExternalContextBase *pNextContext = LockFreeStack<ExternalContextBase>::Next(pContext);
            _InternalDeleteHelper<ExternalContextBase>(pContext);
            pContext = pNextContext;
        }

        RealizedChore *pChore = m_realizedChorePool.Flush();
        while (pChore != NULL)
        {
            RealizedChore *pNextChore = LockFreeStack<RealizedChore>::Next(pChore);
            delete pChore;
            pChore = pNextChore;
        }

        // Trace event to signal scheduler shutdown
        TraceSchedulerEvent(CONCRT_EVENT_END, TRACE_LEVEL_INFORMATION, m_id);

        // Signal threads waiting on scheduler shutdown
        while ( !m_finalEvents.Empty())
        {
            WaitNode *pNode = m_finalEvents.RemoveHead();
            SetEvent(pNode->m_hEvent);
            CloseHandle(pNode->m_hEvent);
            delete pNode;
        }

        delete this;
    }

    ///<summary>
    ///     Increments the reference counts required by a scheduler attach.
    ///</summary>
    void SchedulerBase::ReferenceForAttach()
    {
        InterlockedIncrement(&m_attachCount);
        Reference();
    }

    ///<summary>
    ///     Decrements the reference counts incremented for scheduler attach.
    ///</summary>
    void SchedulerBase::ReleaseForDetach()
    {
        InterlockedDecrement(&m_attachCount);
        Release();
    }

    ///<summary>
    ///     Internal contexts call this when created and utilized inside this scheduler.
    ///</summary>
    void SchedulerBase::IncrementInternalContextCount()
    {
        InterlockedIncrement(&m_internalContextCountPlusOne);
    }

    ///<summary>
    ///     Internal contexts call this function in order to notify that they are out of dispatch.  The last internal context
    ///     to call this will trigger scheduler finalization.
    ///</summary>
    void SchedulerBase::DecrementInternalContextCount()
    {
        LONG val = InterlockedDecrement(&m_internalContextCountPlusOne);
        ASSERT(val >= 0);
        if (val == 0) Finalize();
    }

    /// <summary>
    ///     Send a scheduler ETW event.
    /// </summary>
    void SchedulerBase::ThrowSchedulerEvent(ConcRT_EventType eventType, UCHAR level, unsigned int schedulerId)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Class.Type = (UCHAR)eventType;
            concrtHeader.header.Class.Level = level;
            concrtHeader.header.Guid = SchedulerEventGuid;

            concrtHeader.SchedulerID = schedulerId;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }

    /// <summary>
    ///     Called when the resource manager is giving virtual processors to a particular scheduler.  The virtual processors are
    ///     identified by an array of IVirtualProcessorRoot interfaces. This call is made to grant virtual processor roots
    ///     at initial allocation during the course of ISchedulerProxy::RequestInitialVirtualProcessors, and during dynamic
    ///     core migration.
    /// </summary>
    /// <param name="pVirtualProcessorRoots">
    ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors being added to the scheduler.
    /// </param>
    /// <param name="count">
    ///     Number of IVirtualProcessorRoot interfaces in the array.
    /// </param>
    void SchedulerBase::AddVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count)
    {
        if (ppVirtualProcessorRoots == NULL)
            throw std::invalid_argument("ppVirtualProcessorRoots");

        if (count < 1)
            throw std::invalid_argument("count");

        for (unsigned int i = 0; i < count; ++i)
        {
            IVirtualProcessorRoot * pVProcRoot = ppVirtualProcessorRoots[i];

            // IMPORTANT: This API is called for each virtual processor added at the time of scheduler creation, and
            // later when dynamic RM adds cores to this scheduler. We do not need to synchronize between concurrent
            // invocations of this API, as the RM guarantees for now that only one thread is calling this API at a time.
            int nodeId = pVProcRoot->GetNodeId();

            ASSERT(nodeId >= 0 && nodeId < m_nodeCount && m_nodes[nodeId] != NULL && m_rings[nodeId] != NULL);
            if (!m_rings[nodeId]->IsActive())
            {
                m_rings[nodeId]->Activate();
            }

            m_nodes[nodeId]->AddVirtualProcessor(pVProcRoot);

            // This count should be incremented after adding the virtual processor. The SchedulingNode::AddVirtualProcessor
            // API called above tests if the count is 0, to infer that this virtual processor is the first one added to the
            // scheduler during the initial thread request.
            InterlockedIncrement(&m_virtualProcessorCount);

            // The total count on the scheduler is not incremented for oversubscribed virtual processors. (adding an oversubscribed
            // virtual processor doesn't go through this code path). Querying for the number of virtual processors assigned to a scheduler
            // does not take into account oversubscribed virtual processors, since technically these virtual processors are in place
            // to compensate for other virtual processors that may be blocked, and therefore are not available to perform work for
            // the scheduler.
        }
    }

    /// <summary>
    ///     Called when the resource manager is taking away virtual processors from a particular scheduler.  The scheduler should
    ///     mark the supplied virtual processors such that they are removed asynchronously and return immediately.  Note that
    ///     the scheduler should make every attempt to remove the virtual processors as quickly as possible as the resource manager
    ///     will reaffinitize threads executing upon them to other resources.  Delaying stopping the virtual processors may result
    ///     in unintentional oversubscription within the scheduler.
    /// </summary>
    /// <param name="pVirtualProcessorRoots">
    ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors which are to be removed.
    /// </param>
    /// <param name="count">
    ///     Number of IVirtualProcessorRoot interfaces in the array.
    /// </param>
    void SchedulerBase::RemoveVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count)
    {
        if (ppVirtualProcessorRoots == NULL)
            throw std::invalid_argument("ppVirtualProcessorRoots");

        if (count < 1)
            throw std::invalid_argument("count");

        for (unsigned int i = 0; i < count; ++i)
        {
            IVirtualProcessorRoot * pVProcRoot = ppVirtualProcessorRoots[i];
            VirtualProcessor * pVirtualProcessor = m_nodes[pVProcRoot->GetNodeId()]->FindMatchingVirtualProcessor(pVProcRoot);

            while (pVirtualProcessor == NULL)
            {
                // If the virtual processor was not found the first time around, it must because it is an oversubscribed virtual processor
                // and we are racing with the call to Oversubscribe(true). Once the virtual processor root has been created in the RM
                // (for the oversubscribed vproc), we can receive a RemoveVirtualProcessor call for that root at any time. Only the thread
                // scheduler creates oversubscribed vprocs.
                ASSERT(m_policy.GetPolicyValue(::Concurrency::SchedulerKind) == ::Concurrency::ThreadScheduler);

                _SpinWaitBackoffNone spinWait;
                while (spinWait._SpinOnce())
                {
                    // _YieldProcessor is called inside _SpinOnce
                }

                platform::__SwitchToThread();
                pVirtualProcessor = m_nodes[pVProcRoot->GetNodeId()]->FindMatchingVirtualProcessor(pVProcRoot);
                ASSERT(pVirtualProcessor == NULL || pVirtualProcessor->m_fOversubscribed);
            }

            if (pVirtualProcessor->m_fOversubscribed)
            {
                // We must sychronize with a potential RemoveVirtualProcessor for this virtual processor due to the RM taking the underlying
                // core away. The winner of the interlocked exchange gets to retire the virtual processor.
                ASSERT(pVirtualProcessor->m_pOversubscribingContext != NULL);
                pVirtualProcessor = pVirtualProcessor->m_pOversubscribingContext->GetAndResetOversubscribedVProc(pVirtualProcessor);

                ASSERT(pVirtualProcessor == NULL || pVirtualProcessor->GetOwningRoot() == pVProcRoot);
                // Even if we lose the race, we are safe to touch the virtual processor here, since the context retiring the virtual processor
                // is guaranteed to not get past the call to the RM (in VirtualProcessor::Retire), that removes the virtual processor.
            }

            if (pVirtualProcessor != NULL)
            {
                pVirtualProcessor->MarkForRetirement();
            }
        }
    }

    ///<summary>
    ///     Collect all the statistical information about this scheduler, which include arrival and completion
    ///     rates, from which the total number of tasks is deduced.
    ///</summary>
    void SchedulerBase::Statistics(unsigned int *pTaskCompletionRate, unsigned int *pTaskArrivalRate, unsigned int *pNumberOfTasksEnqueued)
    {
        //
        // Collect all the virtual processor statistics. All internal contexts own a virtual processor when they
        // run a task, so they also own the statistical information at that point and are free to update it without
        // racing with other internal contexts (it is done without interlocked operation). Now we simply collect that
        // information that aggregated on each virtual processor and add it to our total. We do it non-interlocked
        // fully aware that the numbers might be slightly off, for example due to store-buffer not being flushed.
        //

        for (int index = 0; index < m_nodeCount; index++)
        {
            SchedulingNode * pNode = m_nodes[index];

            if (pNode != NULL)
            {
                for (int i = 0; i < pNode->m_virtualProcessors.MaxIndex(); i++)
                {
                    VirtualProcessor * pVirtualProcessor = pNode->m_virtualProcessors[i];

                    //
                    // We collect the data and reset it so that next time around we would get the numbers as of
                    // last update. This allows us to get the rate of change and avoid overflow in most cases.
                    //
                    if (pVirtualProcessor != NULL)
                    {
                        unsigned int arrivalRate = pVirtualProcessor->GetEnqueuedTaskCount();
                        unsigned int completionRate = pVirtualProcessor->GetDequeuedTaskCount();

                        *pTaskArrivalRate += arrivalRate;
                        *pTaskCompletionRate += completionRate;
                        *pNumberOfTasksEnqueued += (arrivalRate - completionRate);
                    }
                }
            }
        }

        //
        // Collect data from the retired virtual processors, saved on the scheduler itself. Note that there is
        // a race here between virtual processor retiring and statistics being collected. Since we do not lock
        // any structure that we are reading from we can either count the statistics twice, or miss them completely.
        // This will cause a spike in statistics, but hopefully it will be rare enough that after collecting
        // several datapoints it can be discarded.
        //
        {
            unsigned int arrivalRate = GetEnqueuedTaskCount();
            unsigned int completionRate = GetDequeuedTaskCount();

            *pTaskArrivalRate += arrivalRate;
            *pTaskCompletionRate += completionRate;
            *pNumberOfTasksEnqueued += (arrivalRate - completionRate);
        }

        //
        // Collect all the external context and free thread statistics. All external contexts and alien threads (not
        // bound to our scheduler) are registered in a list array that is kept on a scheduler. They own a slot in the
        // list array of external statistics, and they update that particular external statistics. Because they own a unique
        // ExternalStatistics class there is no contention or races. Now we simply collect that information per external context
        // or free thread and add it to our total. Again, we are fully aware that numbers might not be fully accurate.
        //
        for (int i = 0; i < m_externalThreadStatistics.MaxIndex(); i++)
        {
            ExternalStatistics * externalStatistics = m_externalThreadStatistics[i];

            //
            // We collect the data and reset it so that next time around we would get the numbers as of
            // last update. This allows us to get the rate of change.
            //
            if (externalStatistics != NULL)
            {
                unsigned int arrivalRate = externalStatistics->GetEnqueuedTaskCount();
                unsigned int completionRate = externalStatistics->GetDequeuedTaskCount();

                *pTaskArrivalRate += arrivalRate;
                *pTaskCompletionRate += completionRate;
                *pNumberOfTasksEnqueued += (arrivalRate - completionRate);

                // If external statistics class is no longer useful, remove it. Note that we could have left the external
                // statistics alone because when scheduler finalizes it destroys the ListArray<ExternalStatistics>, which
                // will also destruct all external statistics stored in it. However, this way we allow for slot reuse in
                // the ListArray in case we have a huge amount of external context joining and leaving.
                if (!externalStatistics->IsActive())
                {
                    // We can safely remove this statistics from our list. To see why look at comments in IsActive().
                    m_externalThreadStatistics.Remove(externalStatistics, false);
                    delete externalStatistics;
                }
            }
        }
    }

    /// <summary>
    ///     Returns a suballocator from the pool of suballocators in the process, or creates a new one. The RM only allows
    ///     a fixed number of allocators for external contexts in the process, whereas every virtual processor that requests
    ///     an allocator will get one.
    /// </summary>
    /// <param name="fExternalAllocator">
    ///     Specifies whether the allocator is being requested for an external context. If this is 'true' the RM will return
    ///     NULL if it has reached its limit of suballocators for external contexts. If this is 'false', the caller is requesting
    ///     the suballocator for a virtual processor, and the RM *must* allocate one (resources permitting).
    /// </param>
    SubAllocator* SchedulerBase::GetSubAllocator(bool fExternalAllocator)
    {
        if (fExternalAllocator)
        {
            if (s_numExternalAllocators >= s_maxExternalAllocators)
            {
                return NULL;
            }
            InterlockedIncrement(&s_numExternalAllocators);
        }

        SubAllocator* pAllocator = s_subAllocatorFreePool.Pop();
        if (pAllocator == NULL)
        {
            pAllocator = _concrt_new SubAllocator();
        }

        ASSERT(pAllocator != NULL);
        pAllocator->SetExternalAllocatorFlag(fExternalAllocator);

        return pAllocator;
    }

    /// <summary>
    ///     Returns a suballocator back to the pool in the RM.
    ///     The RM caches a fixed number of suballocators and will destroy the rest.
    /// </summary>
    void SchedulerBase::ReturnSubAllocator(SubAllocator* pAllocator)
    {
        if (pAllocator->IsExternalAllocator())
        {
            LONG retVal = InterlockedDecrement(&s_numExternalAllocators);
            ASSERT(retVal >= 0);
        }

        if (s_subAllocatorFreePool.Count() < s_allocatorFreePoolLimit)
        {
            s_subAllocatorFreePool.Push(pAllocator);
        }
        else
        {
            delete pAllocator;
        }
    }

    /// <summary>
    ///     Called to perform a commit of safe-point registrations up to **AND INCLUDING** a particular version.
    /// </summary>
    /// <param name="commitVersion">
    ///     The data version that we commit to.  A version of zero indicates a full commit.
    /// </param>
    void SchedulerBase::CommitToVersion(ULONG commitVersion)
    {
        //
        // For UMS, this has to be lock free (more accurately, UMS trigger free -- meaning no blocking or yielding operations).
        // We store this as a pure-spin-lock (hyper critical) protected queue.  There should be very low contention on this lock.
        //

        SQueue<SafePointInvocation> pCommits;

        m_safePointInvocations.Acquire();
        for(;;)
        {
            SafePointInvocation *pCur = m_safePointInvocations.Current();

            //
            // We do not attempt to commit across the wrap-around boundary.  We commit up to the boundary and then recommit afterward.  This prevents
            // wrap-around issues.
            //
            if (pCur != NULL && (commitVersion == 0 || (IsVisibleVersion(pCur->m_safePointVersion) && pCur->m_safePointVersion <= commitVersion)))
            {
                pCur = m_safePointInvocations.SQueue<SafePointInvocation>::Dequeue();
                pCommits.Enqueue(pCur);
            }
            else
            {
                break;
            }
        }
        m_safePointInvocations.Release();

        //
        // Perform every safe point invocation.
        // These must be invoked in the order they were enqueued to m_safePointInvocations.  There is an ordering constraint because
        // ListArrays of workqueues are in ScheduleGroups, which are in ListArrays themselves.  Deleting a workqueue after deleting
        // its enclosing ScheduleGroup will cause an AV.
        //
        while (!pCommits.Empty())
        {
            SafePointInvocation *pCur = pCommits.Dequeue();
            pCur->Invoke();
        }
    }

    /// <summary>
    ///     Returns the commit version for safe points within the scheduler.
    /// </summary>
    ULONG SchedulerBase::ComputeSafePointCommitVersion()
    {
        bool fVersioned = false;
        ULONG commitVersion = 0;

        for (int index = 0; index < m_nodeCount; index++)
        {
            SchedulingNode *pNode = m_nodes[index];

            if (pNode != NULL)
            {
                for (int i = 0; i < pNode->m_virtualProcessors.MaxIndex(); i++)
                {
                    VirtualProcessor *pVirtualProcessor = pNode->m_virtualProcessors[i];
                    if (pVirtualProcessor != NULL)
                    {
                        ULONG localCommit = ObservedVersion(pVirtualProcessor->m_safePointMarker.m_lastObservedVersion);

                        if (fVersioned)
                        {
                            //
                            // We can only commit to the lowest version that every virtual processor has observed.
                            //
                            if (commitVersion > localCommit)
                                commitVersion = localCommit;
                        }
                        else
                        {
                            commitVersion = localCommit;
                            fVersioned = true;
                        }
                    }
                }
            }
        }

        return commitVersion;
    }

    /// <summary>
    ///     Updates and returns the pending version for safe point commits.
    ///     If there are no commits pending, 0 is returned.
    /// </summary>
    ULONG SchedulerBase::UpdatePendingVersion()
    {
        ULONG commitVersion = ComputeSafePointCommitVersion();

        if (commitVersion <= m_safePointPendingVersion)
        {
            // It has either been comitted or is pending in some vproc.
            return 0;
        }

        if (m_safePointPendingVersion == m_safePointCommitVersion)
        {
            // Update pending version. This routine is called with the lock
            // held. This compare and set operation needs to be atomic.
            m_safePointPendingVersion = commitVersion;
            return commitVersion;
        }
        else
        {
            // Just update the pending version. The vproc that snapped the list
            // will notice this update and resnap the new entries
            m_safePointPendingVersion = commitVersion;
            return 0;
        }
    }

    /// <summary>
    ///     Updates the commit version to the given version and returns
    ///     the pending commit version. If there are no commits pending, 0 is returned.
    /// </summary>
    /// <param name="commitVersion">
    ///     The version up to which safe points have been committed.
    /// </param>
    ULONG SchedulerBase::UpdateCommitVersion(ULONG commitVersion)
    {
        ASSERT(commitVersion != 0);

        if (commitVersion == ULONG_MAX)
        {
            // Handle overflow
            ASSERT(commitVersion == m_safePointPendingVersion);
            m_safePointCommitVersion = 0;

            // Handle safepoints above the wrap around point
            m_safePointPendingVersion = ComputeSafePointCommitVersion();
        }
        else
        {
            // Update the committed version
            m_safePointCommitVersion = commitVersion;
        }

        if (m_safePointCommitVersion != m_safePointPendingVersion)
        {
            // Found pending commits
            ASSERT(m_safePointPendingVersion > m_safePointCommitVersion);
            return m_safePointPendingVersion;
        }

        return 0;
    }

    /// <summary>
    ///     Called to make a determination of what version of data we can commit up to.  This is the minimum data version that all virtual
    ///     processors have observed.
    /// </summary>
    void SchedulerBase::CommitSafePoints()
    {
        ULONG commitVersion = 0;

        // Update the version we are about to commit.
        m_safePointInvocations.Acquire();
        commitVersion = UpdatePendingVersion();
        m_safePointInvocations.Release();


        // Note that a commitVersion of 0 here indicates that there are no safe points
        // to commit.
        while (commitVersion != 0)
        {
            CommitToVersion(commitVersion);

            // Publish the committed version and check for any pending commits
            m_safePointInvocations.Acquire();
            commitVersion = UpdateCommitVersion(commitVersion);
            m_safePointInvocations.Release();
        }
    }

    /// <summary>
    ///     Called when a particular virtual processor reaches a safe point.  This function does very little unless there has
    ///     been a change in the version number of the safe point.
    /// </summary>
    /// <param name="pMarker">
    ///     The safe point marker for a given virtual processor.  This is the virtual processor reaching a safe point.
    /// </param>
    /// <returns>
    ///     An indication of whether a commit should take place.  If this is true, the caller should call CommitSafePoints when possible.
    ///     Note that this is a return value so that things like UMS virtual processors can exit critical regions before performing
    ///     the commit (to avoid, for instance, heap locks in critical regions).
    /// </returns>
    bool SchedulerBase::MarkSafePoint(SafePointMarker *pMarker)
    {
        //
        // If there has been no change in observation version, there's nothing to mark off and nothing to worry about.  Only if there
        // has been a change need we go down the path of checking how far we can commit.
        //
        if (pMarker->m_lastObservedVersion != m_safePointDataVersion)
        {
            pMarker->m_lastObservedVersion = m_safePointDataVersion;
            return true;
        }

        return false;
    }

    /// <summary>
    ///     The routine is used to trigger a safe point commit on all the vprocs by
    ///     updating the data version. This routine shall not trigger synchronous UMS blocking
    /// </summary>
    void SchedulerBase::TriggerCommitSafePoints(SafePointMarker *pMarker)
    {
        m_safePointInvocations.Acquire();

        //
        // We check for a change with the lock held to avoid triggering
        // a commit check on all vprocs unnecessarily.
        //
        if (pMarker->m_lastObservedVersion != m_safePointDataVersion)
        {
            //
            // Publishing a new data version would cause all vprocs
            // to attempt a commit
            //
            PublishNewDataVersion();
        }

       m_safePointInvocations.Release();
    }

    /// <summary>
    ///     Called to register an object to invoke upon reaching the next safe point after this call.
    /// </summary>
    /// <param name="pInvocation">
    ///     The invocation object which contains information about what to call.
    /// </param>
    void SchedulerBase::InvokeOnSafePoint(SafePointInvocation *pInvocation)
    {
        //
        // *READ THIS CAREFULLY*:
        //
        // Due to the places in which this is likely to be invoked, this routine must not trigger UMS.  That means it must be lock-free (or at least
        // block/yield free).  Doing any UMS trigger operation will wreak havoc on the UMS scheduler.
        //
        // Note that in order to vastly simplify this code, I am utilizing a pure spin lock protected queue.  There should be low enough contention
        // on this that it should not matter and there are potential truly lock-free algorithms which might be used for more efficiency (though hugely
        // more complex).
        //

        //
        // Note that we assume that everything is enqueued in data version order (FIFO) in order to simplify the code and prevent having to always
        // scan the entire list.  In order to guarantee this, the increment and the list addition must be atomic with repsect to each other.  Right
        // now, this is guarded via the spinlock on this list.
        //
        m_safePointInvocations.Acquire();

        pInvocation->m_safePointVersion = PublishNewDataVersion();
        m_safePointInvocations.SQueue<SafePointInvocation>::Enqueue(pInvocation);

        m_safePointInvocations.Release();

        //
        // pInvocation is must not be dereferenced after this point:
        //

    }

    /// <summary>
    ///     Registers a particular function to be called with particular data when a given scheduler reaches the next safe point
    ///     after the call is made.  This is an intrusive invocation with the current SafePointInvocation class incuring no heap
    ///     allocations.
    /// </summary>
    /// <param name="pInvocationFunction">
    ///     The function which will be invoked at the next safe point
    /// </param>
    /// <param name="pData">
    ///     User specified data.
    /// </param>
    /// <param name="pScheduler">
    ///     The scheduler on which to wait for a safe point to invoke pInvocationFunction.
    /// </param>
    void SafePointInvocation::InvokeAtNextSafePoint(InvocationFunction pInvocationFunction, void *pData, SchedulerBase *pScheduler)
    {
        //
        // If the shutdown completed flag is set, all scheduler operations are serialized to one thread for finalization.  It is perfectly safe to
        // perform the action right now rather than defer it.  This is cleanup!
        //
        if (pScheduler->m_vprocShutdownGate & SHUTDOWN_COMPLETED_FLAG)
        {
            pInvocationFunction(pData);
        }
        else
        {
            m_pInvocation = pInvocationFunction;
            m_pData = pData;

            pScheduler->InvokeOnSafePoint(this);
        }
    }

    /// <summary>
    ///     Finds the node associated with the calling thread, if any. This method only returns a node if the current context is an internal context
    ///     in this scheduler.
    /// </summary>
    SchedulingNode *SchedulerBase::FindCurrentNode()
    {
        SchedulingNode * pCurrentNode = NULL;
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        if (pCurrentContext != NULL && pCurrentContext->m_pScheduler == this && !pCurrentContext->IsExternal())
        {
            InternalContextBase *pInteralContext = static_cast<InternalContextBase *>(pCurrentContext);
            pInteralContext->EnterCriticalRegion();
            pCurrentNode = pInteralContext->m_pVirtualProcessor->GetOwningNode();
            pInteralContext->ExitCriticalRegion();
        }

        return pCurrentNode;
    }

    /// <summary>
    ///     Returns the scheduling node which srcLocation is a member of.  Note that if srcLocation and this node's location do not intersect,
    ///     this will return NULL.
    /// </summary>
    SchedulingNode *SchedulerBase::FindNodeByLocation(location* pSrcLocation)
    {
        SchedulingNode *pNode = NULL;

        switch(pSrcLocation->_GetType())
        {
            case location::_ExecutionResource:
            {
                unsigned int nodeId;
                if (m_resourceNodeMap.Find(pSrcLocation->_GetId(), &nodeId))
                    pNode = m_nodes[nodeId];

                break;
            }

            case location::_SchedulingNode:
                pNode = m_nodes[pSrcLocation->_GetId()];
                break;

            case location::_NumaNode:
                //
                // This should be handled at a different level.  There is no single node which maps here.
                //
                ASSERT(false);

            default:
                break;
        }

        return pNode;
    }

    /// <summary>
    ///     Returns a bit set for a given location to perform quick masking.
    /// </summary>
    QuickBitSet SchedulerBase::GetBitSet(const location* pLoc)
    {
        QuickBitSet set(GetMaskIdCount());

        switch(pLoc->_GetType())
        {
            case location::_System:
            {
                set.Fill();
                break;
            }
            case location::_NumaNode:
            {
                set = m_numaInformation[pLoc->_GetId()].m_resourceMask;
                break;
            }
            case location::_SchedulingNode:
            {
                set = m_nodes[pLoc->_GetId()]->GetResourceSet();
                break;
            }
            case location::_ExecutionResource:
            {
                unsigned int maskId;
                Hash<unsigned int, unsigned int>::ListNode *pHashNode = m_resourceBitMap.Find(pLoc->_GetId(), &maskId);
                ASSERT(pHashNode != NULL);
                set.Set(maskId);
                break;
            }

            default:
                break;
        }

        return set;
    }

    //
    // @TODO: This needs to go with real priority.
    //
    BoostedObject *SchedulerBase::GetNextPriorityObject()
    {
        m_priorityObjects.AcquireWrite();
        BoostedObject *pEntry = m_priorityObjects.UnlockedRemoveHead();

        if (pEntry)
        {
            // m_boostState must be set under the lock
            ASSERT(pEntry->m_boostState == BoostedObject::BoostStateBoosted);
            pEntry->m_boostState = BoostedObject::BoostStateUnboosted;
        }
        m_priorityObjects.ReleaseWrite();

        return pEntry;
    }

    /// <summary>
    ///     Performs the scheduler service scan.
    /// </summary>
    void SchedulerBase::PerformServiceScan(ULONGLONG serviceTime)
    {
        //
        // Only one person should enter the service loop.  It should also only be entered every period defined by how often we want to
        // do priority boosting, etc...
        //
        for(;;)
        {
            ULONGLONG readServiceTime = m_lastServiceScan;

            if ((ULONGLONG)InterlockedCompareExchange64(reinterpret_cast<volatile LONGLONG *>(&m_lastServiceScan),
                                                  serviceTime,
                                                  readServiceTime) == readServiceTime)
            {

                OMTRACE(MTRACE_EVT_PERIODICSCAN, this, NULL, NULL, serviceTime);

                //
                // Take the lock and perform the scan.
                // @TODO: This part needs to change.
                //

                m_priorityObjects.AcquireWrite();

                for (int idx = 0; idx < m_nodeCount; ++idx)
                {
                    SchedulingRing *pRing = m_rings[idx];
                    SchedulingNode *pNode = m_nodes[idx];

                    int sIdx;
                    ScheduleGroupSegmentBase *pSegment = pRing->GetFirstAffineScheduleGroupSegment(&sIdx);
                    while (pSegment != NULL)
                    {
                        if (pSegment->TimeSinceServicing(serviceTime) > 2000 &&
                            pSegment->m_priorityServiceLink.m_boostState == BoostedObject::BoostStateUnboosted)
                        {
                            OMTRACE(MTRACE_EVT_PERIODICSCANNED, pSegment, NULL, NULL, 1);
                            OMTRACE(MTRACE_EVT_PRIORITYBOOST, pSegment, NULL, NULL, serviceTime);

                            pSegment->m_priorityServiceLink.m_boostState = BoostedObject::BoostStateBoosted;
                            m_priorityObjects.UnlockedAddHead(&pSegment->m_priorityServiceLink);
                        }
                        else
                        {
                            OMTRACE(MTRACE_EVT_PERIODICSCANNED, pSegment, NULL, NULL, 0);
                        }

                        pSegment = pRing->GetNextAffineScheduleGroupSegment(&sIdx);
                    }

                    pSegment = pRing->GetFirstNonAffineScheduleGroupSegment(&sIdx);
                    while (pSegment != NULL)
                    {
                        if (pSegment->TimeSinceServicing(serviceTime) > 2000 &&
                            pSegment->m_priorityServiceLink.m_boostState == BoostedObject::BoostStateUnboosted)
                        {
                            OMTRACE(MTRACE_EVT_PRIORITYBOOST, pSegment, NULL, NULL, serviceTime);

                            pSegment->m_priorityServiceLink.m_boostState = BoostedObject::BoostStateBoosted;
                            m_priorityObjects.UnlockedAddHead(&pSegment->m_priorityServiceLink);
                        }
                        else
                        {
                            OMTRACE(MTRACE_EVT_PERIODICSCANNED, pSegment, NULL, NULL, 0);
                        }

                        pSegment = pRing->GetNextNonAffineScheduleGroupSegment(&sIdx);
                    }

                    int vIdx;
                    VirtualProcessor *pVProc = pNode->GetFirstVirtualProcessor(&vIdx);
                    while (pVProc != NULL)
                    {
                        if (pVProc->TimeSinceServicing(serviceTime) > 2000 &&
                            pVProc->m_priorityServiceLink.m_boostState == BoostedObject::BoostStateUnboosted)
                        {
                            pVProc->m_priorityServiceLink.m_boostState = BoostedObject::BoostStateBoosted;
                            m_priorityObjects.UnlockedAddHead(&pVProc->m_priorityServiceLink);
                        }
                        pVProc = pNode->GetNextVirtualProcessor(&vIdx);
                    }
                }

                m_priorityObjects.ReleaseWrite();

                break;
            }
        }
    }

    void SchedulerBase::RemovePrioritizedObject(BoostedObject *pObject)
    {
        m_priorityObjects.AcquireWrite();
        if (pObject->m_boostState == BoostedObject::BoostStateBoosted)
        {
            m_priorityObjects.UnlockedRemove(pObject);
        }
        pObject->m_boostState = BoostedObject::BoostStateDisallowed;
        m_priorityObjects.ReleaseWrite();
    }

    unsigned int _Scheduler::_Reference()
    {
        return _M_pScheduler->Reference();
    }

    unsigned int _Scheduler::_Release()
    {
        return _M_pScheduler->Release();
    }
} // namespace details
} // namespace Concurrency
