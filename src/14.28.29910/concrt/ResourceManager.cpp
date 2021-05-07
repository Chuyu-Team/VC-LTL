// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ResourceManager.cpp
//
// Implementation of IResourceManager.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <float.h>
#include <algorithm>
#include <awint.hpp> // TRANSITION, shouldn't include this STL-build-internal header

#pragma warning(push)
#pragma warning(disable : 4702)
#pragma warning(disable : 4100)

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
#define _CONCRT_RESOURCE_USE_PLATFORM_DETECTION 1
#else
#define _CONCRT_RESOURCE_USE_PLATFORM_DETECTION 0
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)

namespace Concurrency
{
    /// <summary>
    ///     Returns the OS version.
    /// </summary>
    _CONCRTIMP IResourceManager::OSVersion __cdecl GetOSVersion()
    {
        return ::Concurrency::details::ResourceManager::Version();
    }

    /// <summary>
    ///     Returns an interface that represents the singleton resource manager instance. The API references the
    ///     resource manager, and must be matched by a call to IResourceManager::Release(), when the caller is done.
    /// </summary>
    _CONCRTIMP IResourceManager* __cdecl CreateResourceManager()
    {
        return ::Concurrency::details::ResourceManager::CreateSingleton();
    }

    /// <summary>
    ///     The number of hardware threads on the underlying machine is returned here.
    /// </summary>
    _CONCRTIMP unsigned int __cdecl GetProcessorCount()
    {
        return ::Concurrency::details::ResourceManager::GetCoreCount();
    }

    /// <summary>
    ///     The number of NUMA nodes or processor packages on the underlying machine is returned here.
    ///     If the processor contains more NUMA nodes than processor packages, the number of NUMA nodes
    ///     is returned, otherwise the number of packages is returned.
    /// </summary>
    _CONCRTIMP unsigned int __cdecl GetProcessorNodeCount()
    {
        return ::Concurrency::details::ResourceManager::GetNodeCount();
    }

    /// <summary>
    ///     Returns an identifier for a scheduler. Before calling an API with an IScheduler interface as a parameter,
    ///     an identifier must be obtained for the scheduler using this API.
    /// </summary>
    _CONCRTIMP unsigned int __cdecl GetSchedulerId()
    {
        return ::Concurrency::details::ResourceManager::GetSchedulerId();
    }

    /// <summary>
    ///     Returns an identifier for an execution context. Before calling an API with an IExecutionContext interface as a parameter,
    ///     an identifier must be obtained for the execution context using this API.
    /// </summary>
    _CONCRTIMP unsigned int __cdecl GetExecutionContextId()
    {
        return ::Concurrency::details::ResourceManager::GetExecutionContextId();
    }

    /// <summary>
    ///     Restricts the execution resources used by the Concurrency Runtime's internal worker threads to the affinity set specified.
    ///     <para> It is only valid to call this method before the Resource Manager has been created, or between two Resource Manager lifetimes.
    ///     It may be invoked multiple times as long as the Resource Manager does not exist at the time of invocation. Once an affinity limit
    ///     has been set, it remains in effect until the next valid call to the <c>set_task_execution_resources</c> method.</para>
    ///     <para>The affinity mask provided need not be a subset of the process affinity mask. The process affinity will be updated if necessary.</para>
    /// <summary>
    /// <param name="_ProcessAffinityMask">
    ///     The affinity mask that the Concurrency Runtime's worker threads are to be restricted to. Use this method on a system with greater than 64
    ///     hardware threads only if you want to limit the Concurrency Runtime to a subset of the current processor group. In general, you should
    ///     use the version of the method that accepts an array of group affinities as a parameter to restrict affinity on machines with greater
    ///     than 64 hardware threads.
    /// </param>
    /// <remarks>
    ///     The method will throw an <see cref="invalid_operation Class">invalid_operation </see> exception if a Resource Manager is present at
    ///     the time it is invoked, and an <see cref="invalid_argument">invalid_argument </see> exception if the affinity specified results in an empty set of resources.
    ///     <para>The version of the method that takes an array of group affinities as a parameter should only be used on operating systems with version
    ///     Windows 7 or higher. Otherwise, an <see cref="invalid_operation Class">invalid_operation </see> exception is thrown.</para>
    ///     <para>Programatically modifying the process affinity after this method has been invoked will not cause the Resource Manager to re-evaluate
    ///     the affinity it is restricted to. Therefore, all changes to process affinity should be made before calling this method.</para>
    /// </remarks>
    /**/
    _CONCRTIMP void __cdecl set_task_execution_resources(DWORD_PTR dwProcessAffinityMask)
    {
        // This API should not be called by modern APPs
        ENSURE_NOT_APP();
        ::Concurrency::details::ResourceManager::SetTaskExecutionResources(dwProcessAffinityMask);
    }

    /// <summary>
    ///     Restricts the execution resources used by the Concurrency Runtime's internal worker threads to the affinity set specified.
    ///     <para> It is only valid to call this method before the Resource Manager has been created, or between two Resource Manager lifetimes.
    ///     It may be invoked multiple times as long as the Resource Manager does not exist at the time of invocation. Once an affinity limit
    ///     has been set, it remains in effect until the next valid call to the <c>set_task_execution_resources</c> method.</para>
    ///     <para>The affinity mask provided need not be a subset of the process affinity mask. The process affinity will be updated if necessary.</para>
    /// <summary>
    /// <param name="_Count">
    ///    The number of <c>GROUP_AFFINITY</c> entries in the array specified by the parameter <paramref name=_PGroupAffinity"/>.
    /// </param>
    /// <param name="_PGroupAffinity">
    ///     An array of <c>GROUP_AFFINITY</c> entries.
    /// </param>
    /// <remarks>
    ///     The method will throw an <see cref="invalid_operation Class">invalid_operation </see> exception if a Resource Manager is present at
    ///     the time it is invoked, and an <see cref="invalid_argument">invalid_argument </see> exception if the affinity specified results in an empty set of resources.
    ///     <para>The version of the method that takes an array of group affinities as a parameter should only be used on operating systems with version
    ///     Windows 7 or higher. Otherwise, an <see cref="invalid_operation Class">invalid_operation </see> exception is thrown.</para>
    ///     <para>Programatically modifying the process affinity after this method has been invoked will not cause the Resource Manager to re-evaluate
    ///     the affinity it is restricted to. Therefore, all changes to process affinity should be made before calling this method.</para>
    /// </remarks>
    /**/
    _CONCRTIMP void __cdecl set_task_execution_resources(USHORT count, PGROUP_AFFINITY pGroupAffinity)
    {
        // This API should not be called by modern APPs
        ENSURE_NOT_APP();
        ::Concurrency::details::ResourceManager::SetTaskExecutionResources(count, pGroupAffinity);
    }
}

namespace Concurrency
{
namespace details
{
    // Static counters to generate unique identifiers.
    volatile LONG ResourceManager::s_schedulerIdCount = -1L;
    volatile LONG ResourceManager::s_executionContextIdCount = -1L;
    volatile LONG ResourceManager::s_threadProxyIdCount = -1L;

    // Operating system characteristics.
    unsigned int ResourceManager::s_coreCount = 0;
    unsigned int ResourceManager::s_nodeCount = 0;
    unsigned int ResourceManager::s_numaNodeCount = 0;
    bool ResourceManager::s_countProcessorPackages = false;

    DWORD_PTR ResourceManager::s_processAffinityMask = 0;
    DWORD_PTR ResourceManager::s_systemAffinityMask = 0;

    ResourceManager::AffinityRestriction * ResourceManager::s_pUserAffinityRestriction = NULL;
    ResourceManager::AffinityRestriction * ResourceManager::s_pProcessAffinityRestriction = NULL;

    IResourceManager::OSVersion ResourceManager::s_version = IResourceManager::UnsupportedOS;
    ResourceManager *ResourceManager::s_pResourceManager = NULL;

    // Variables used to obtain information about the topology of the system.
    DWORD ResourceManager::s_logicalProcessorInformationLength = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ResourceManager::s_pSysInfo = NULL;

    /// <summary>
    ///     Constructs a hardware affinity from a given thread.
    /// </summary>
    HardwareAffinity::HardwareAffinity(HANDLE hThread)
    {
        memset(&m_affinity, 0, sizeof(m_affinity));
        platform::__GetThreadGroupAffinity(hThread, &m_affinity);
    }

    /// <summary>
    ///     Applies this hardware affinity to a thread.
    /// </summary>
    /// <param name="hThread">
    ///     The thread handle to which to apply this affinity.
    /// </param>
    void HardwareAffinity::ApplyTo(HANDLE hThread)
    {
        platform::__SetThreadGroupAffinity(hThread, &m_affinity);
    }

    /// <summary>
    ///     Returns the OS version.
    /// </summary>
    IResourceManager::OSVersion ResourceManager::Version()
    {
        if (s_version == ::Concurrency::IResourceManager::UnsupportedOS)
        {
            { // begin locked region
                _StaticLock::_Scoped_lock lock(s_lock);
                if (s_version == ::Concurrency::IResourceManager::UnsupportedOS)
                {
                    RetrieveSystemVersionInformation();
                }
            } // end locked region
        }
        return s_version;
    }

    /// <summary>
    ///     Returns the number of nodes (processor packages or NUMA nodes, whichever is greater)
    /// </summary>
    unsigned int ResourceManager::GetNodeCount()
    {
        if (s_nodeCount == 0)
        {
            { // begin locked region
                _StaticLock::_Scoped_lock lock(s_lock);
                if (s_nodeCount == 0)
                {
                    InitializeSystemInformation();
                }
            } // end locked region
        }
        return s_nodeCount;
    }

    /// <summary>
    ///     Returns the number of cores.
    /// </summary>
    unsigned int ResourceManager::GetCoreCount()
    {
        if (s_coreCount == 0)
        {
            { // begin locked region
                _StaticLock::_Scoped_lock lock(s_lock);
                if (s_coreCount == 0)
                {
                    InitializeSystemInformation();
                }
            } // end locked region
        }
        return s_coreCount;
    }

    /// <summary>
    ///     Returns the current thread's node id and core id (relative to that node).
    /// </summary>
    unsigned int ResourceManager::GetCurrentNodeAndCore(unsigned int * pCore)
    {
#if defined(_ONECORE)
        // MSDK does not provide an API to determine the current node and core information
        // For MSDK we assume a single node
        if (pCore != NULL)
        {
            *pCore = 0;
        }
        return 0;
#else // ^^^ defined(_ONECORE) ^^^ // vvv !defined(_ONECORE) vvv
#if _CONCRT_RESOURCE_USE_PLATFORM_DETECTION
        const auto osVersion = ::Concurrency::GetOSVersion();
        if (osVersion >= ::Concurrency::IResourceManager::Win7OrLater)
#endif // _CONCRT_RESOURCE_USE_PLATFORM_DETECTION
        {
            // For Win7 or later, we can use a simple function
            PROCESSOR_NUMBER procNum;
            platform::__GetCurrentProcessorNumberEx(&procNum);
            DWORD processorNumber = procNum.Number;
#ifndef _WIN64
            processorNumber &= 31; // Map CPUs too high to reference on WOW64 down
#endif // _WIN64

            ULONG_PTR processorAffinity = static_cast<ULONG_PTR>(1) << processorNumber;

            for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; nodeIndex++)
            {
                GlobalNode * pNode = &m_pGlobalNodes[nodeIndex];
                if ((pNode->m_processorGroup == procNum.Group) && ((pNode->m_nodeAffinity & processorAffinity) != 0))
                {
                    for (unsigned int coreIndex = 0; coreIndex < pNode->m_coreCount; coreIndex++)
                    {
                        if (pNode->m_pCores[coreIndex].m_processorNumber == processorNumber)
                        {
                            if (pCore != nullptr)
                            {
                                *pCore = coreIndex;
                            }
                            return nodeIndex;
                        }
                    }
                }
            }

            ASSERT(UNREACHED);
            return 0;
        }
#if _CONCRT_RESOURCE_USE_PLATFORM_DETECTION
        else if(osVersion == ::Concurrency::IResourceManager::Win2k3 ||
                osVersion == ::Concurrency::IResourceManager::Vista)
        {
            // For Win2k3 or Vista, we need to compute the node id and core id from the absolute processor id
            DWORD processorNumber = GetCurrentProcessorNumber();
#ifndef _WIN64
            processorNumber &= 31; // Map CPUs too high to reference on WOW64 down
#endif // _WIN64

            ULONG_PTR processorAffinity = static_cast<ULONG_PTR>(1) << processorNumber;

            for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; nodeIndex++)
            {
                GlobalNode * pNode = &m_pGlobalNodes[nodeIndex];
                if ((pNode->m_nodeAffinity & processorAffinity) != 0)
                {
                    for (unsigned int coreIndex = 0; coreIndex < pNode->m_coreCount; coreIndex++)
                    {
                        if (pNode->m_pCores[coreIndex].m_processorNumber == processorNumber)
                        {
                            if (pCore != NULL)
                            {
                                *pCore = coreIndex;
                            }
                            return nodeIndex;
                        }
                    }
                }
            }

            ASSERT(UNREACHED);
            return 0;
        }
        else if (s_version == ::Concurrency::IResourceManager::XP)
        {
            // For XP, we assume everything is on one node, so affinity plays no role.
            if (pCore != NULL)
            {
                *pCore = 0;
            }
            return 0;
        }
        else
        {
            throw ::Concurrency::unsupported_os();
        }
#endif // _CONCRT_RESOURCE_USE_PLATFORM_DETECTION
#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Creates the static singleton instance of the Resource Manager.
    /// </summary>
    ResourceManager* ResourceManager::CreateSingleton()
    {
        ResourceManager *pRM = NULL;

        { // begin locked region
            _StaticLock::_Scoped_lock lock(s_lock);
            if (s_pResourceManager == NULL)
            {
                pRM = _concrt_new ResourceManager();
                pRM->Reference();
                s_pResourceManager = (ResourceManager*) Security::EncodePointer(pRM);
            }
            else
            {
                pRM = (ResourceManager*) Security::DecodePointer(s_pResourceManager);
                if ( !pRM->SafeReference())
                {
                    // pRM has refcnt=0 and will be deleted as soon as the lock is released
                    pRM = _concrt_new ResourceManager();
                    pRM->Reference();
                    s_pResourceManager = (ResourceManager*) Security::EncodePointer(pRM);
                }
            }
        } // end locked region

        return pRM;
    }

    /// <summary>
    ///     Constructs the resource manager.
    /// </summary>
    ResourceManager::ResourceManager()
        : m_referenceCount(0L)
        , m_numSchedulers(0)
        , m_maxSchedulers(16)
        , m_numSchedulersNeedingNotifications(0)
        , m_allocationRound(0)
        , m_dynamicRMWorkerState(Standby)
        , m_pGlobalNumaNodes(NULL)
        , m_pGlobalNodes(NULL)
        , m_hDynamicRMThreadHandle(NULL)
        , m_hDynamicRMEvent(NULL)
        , m_ppProxyData(NULL)
        , m_ppGivingProxies(NULL)
        , m_ppReceivingProxies(NULL)
    {
        // Initialize static information about the system asking for the topology information to be saved
        // so we can use it right after.
        InitializeSystemInformation(true);
        DetermineTopology();

        if (s_version >= ::Concurrency::IResourceManager::Vista)
        {
            m_pPageVirtualProtect = NULL;
        }
#if !defined(_ONECORE)
        else
        {
            // On an OS with version < 6000 we need to use a different mechanism to enable write buffer flushing.

            // Start by allocating a commited region of memory the size of a page.
            m_pPageVirtualProtect = (char *) VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (m_pPageVirtualProtect == NULL)
            {
                throw std::bad_alloc();
            }

            // We expect the OS to give us an allocation starting at a page boundary.
            ASSERT(((ULONG_PTR)m_pPageVirtualProtect & 0xFFF) == 0);

            *m_pPageVirtualProtect = 1;
        }
#endif // !defined(_ONECORE)

        // The dynamic RM thread is not created up front. It is created when the number of schedulers go from 1 to 2
        // In addition, once it is created, it is placed on standby if the number of schedulers goes down to 1 again.
        // The event is created here, since it could be signaled even before the thread is created.
        // Auto reset event that is not signalled initially
        m_hDynamicRMEvent = platform::__CreateAutoResetEvent();

        // Allocate common memory used for static and dynamic allocation. Buffers specific to dynamic core migration
        // are only allocated when the DRM thread is started up.
        m_ppProxyData = _concrt_new AllocationData * [m_maxSchedulers];

#if defined(CONCRT_TRACING)
        // Assumes a m x n allocation.
        m_numTotalCores = m_nodeCount * m_pGlobalNodes[0].m_coreCount;
        m_drmInitialState = _concrt_new GlobalCoreData[m_numTotalCores];
        memset(m_drmInitialState, 0, sizeof(GlobalCoreData) * m_numTotalCores);

        // Maintains a trace for every core removed during preprocessing.
        memset(m_preProcessTraces, 0, sizeof(PreProcessingTraceData) * 100);
        m_preProcessTraceIndex = 0;

        // Maintains a trace for each core allocation or assignment.
        memset(m_dynAllocationTraces, 0, sizeof(DynamicAllocationTraceData) * 100);
        m_dynAllocationTraceIndex = 0;
#endif
    }

    /// <summary>
    ///     This API is called by the dynamic RM worker thread when it starts up, and right after its state changed to
    ///     LoadBalance after being on Standby for a while. We need to find the existing schedulers, and discard the
    ///     statistics they have collected so far if any. Either we've never collected statistics for this scheduler before,
    ///     or too much/too little time has passed since we last collected statistics, and this information cannot be trusted.
    /// </summary>
    void ResourceManager::DiscardExistingSchedulerStatistics()
    {
        // NOTE: This routine must be called while holding m_lock.
        ASSERT(m_numSchedulers > 1);
        ASSERT(m_dynamicRMWorkerState == LoadBalance);

        SchedulerProxy * pSchedulerProxy = NULL;

        for (pSchedulerProxy = m_schedulers.First(); pSchedulerProxy != NULL; pSchedulerProxy = m_schedulers.Next(pSchedulerProxy))
        {
            // Initialize variables needed for statistics.
            unsigned int taskCompletionRate = 0, taskArrivalRate = 0;

            // Get the stored scheduler queue length.
            unsigned int numberOfTasksEnqueued = pSchedulerProxy->GetQueueLength();

            // Collect statistical information about this scheduler.
            pSchedulerProxy->Scheduler()->Statistics(&taskCompletionRate, &taskArrivalRate, &numberOfTasksEnqueued);

            // Update the queue length using the number computed by the statistics.
            pSchedulerProxy->SetQueueLength(numberOfTasksEnqueued);
        }
    }

    /// <summary>
    ///     Restricts the execution resources used by the Concurrency Runtime's internal worker threads to the affinity set specified.
    /// <summary>
    /// <param name="_ProcessAffinityMask">
    ///     The affinity mask that the Concurrency Runtime's worker threads are to be restricted to. Use this method on a system with greater than 64
    ///     hardware threads only if you want to limit the Concurrency Runtime to a subset of the current processor group. In general, you should
    ///     use the version of the method that accepts an array of group affinities as a parameter to restrict affinity on machines with greater
    ///     than 64 hardware threads.
    /// </param>
    /// <remarks>
    ///     The method will throw an <see cref="invalid_operation Class">invalid_operation </see> exception if a Resource Manager is present at
    ///     the time it is invoked, and an <see cref="invalid_argument">invalid_argument </see> exception if the affinity specified results in an empty set of resources.
    ///     <para>The version of the method that takes an array of group affinities as a parameter should only be used on operating systems with version
    ///     Windows 7 or higher. Otherwise, an <see cref="invalid_operation Class">invalid_operation </see> exception is thrown.</para>
    ///     <para>Programatically modifying the process affinity after this method has been invoked will not cause the Resource Manager to re-evaluate
    ///     the affinity it is restricted to. Therefore, all changes to process affinity should be made before calling this method.</para>
    /// </remarks>
    /**/
    void ResourceManager::SetTaskExecutionResources(DWORD_PTR dwAffinityMask)
    {
#if defined(_ONECORE)
        // This API should not be called by modern APPs
        ENSURE_NOT_APP();
#else
        // This API can be invoked on all operating systems supported by ConcRT. On a multigroup machine, we will take
        // the affinity to be the affinity of the process for the current group.
        { // begin locked region
            _StaticLock::_Scoped_lock lock(s_lock);
            if (s_pResourceManager != NULL)
            {
                // It is invalid to call the API if the RM is already in existence.
                throw invalid_operation();
            }

            if (s_version == ::Concurrency::IResourceManager::UnsupportedOS)
            {
                RetrieveSystemVersionInformation();
            }

            // Get the current thread affinity which will populate the group correctly on a machine with multiple groups.
            HardwareAffinity currentThreadAffinity(GetCurrentThread());

            // Use the correct group number and the provided user affinity to construct an affinity that the RM should restrict itself
            // to. Note that on operating systems that do not support multiple groups, the group is set to 0 by default.
            HardwareAffinity * pAffinity = _concrt_new HardwareAffinity(currentThreadAffinity.GetGroup(), dwAffinityMask);

            CaptureProcessAffinity();

            pAffinity->IntersectWith(s_systemAffinityMask);

            if (pAffinity->GetMask() == (KAFFINITY)0)
            {
                throw std::invalid_argument("dwAffinityMask");
            }
            // Check if the provided affinity is outside the process affinity mask. If so, adjust the process affinity.
            if ((pAffinity->GetMask() & (~s_processAffinityMask)) != 0)
            {
                SetProcessAffinityMask(GetCurrentProcess(), (pAffinity->GetMask() | s_processAffinityMask));
            }

            // We have a valid affinity restriction. Delete any existing restriction structure if it is present.
            delete s_pUserAffinityRestriction;
            s_pUserAffinityRestriction = _concrt_new AffinityRestriction(1, pAffinity);

            // We don't need the process affinity anymore.
            delete s_pProcessAffinityRestriction;
            s_pProcessAffinityRestriction = NULL;
        } // end locked region

#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Restricts the execution resources used by the Concurrency Runtime's internal worker threads to the affinity set specified.
    /// <summary>
    /// <param name="_Count">
    ///    The number of <c>GROUP_AFFINITY</c> entries in the array specified by the parameter <paramref name=_PGroupAffinity"/>.
    /// </param>
    /// <param name="_PGroupAffinity">
    ///     An array of <c>GROUP_AFFINITY</c> entries.
    /// </param>
    /// <remarks>
    ///     The method will throw an <see cref="invalid_operation Class">invalid_operation </see> exception if a Resource Manager is present at
    ///     the time it is invoked, and an <see cref="invalid_argument">invalid_argument </see> exception if the affinity specified results in an empty set of resources.
    ///     <para>The version of the method that takes an array of group affinities as a parameter should only be used on operating systems with version
    ///     Windows 7 or higher. Otherwise, an <see cref="invalid_operation Class">invalid_operation </see> exception is thrown.</para>
    ///     <para>Programatically modifying the process affinity after this method has been invoked will not cause the Resource Manager to re-evaluate
    ///     the affinity it is restricted to. Therefore, all changes to process affinity should be made before calling this method.</para>
    /// </remarks>
    void ResourceManager::SetTaskExecutionResources(USHORT count, PGROUP_AFFINITY pGroupAffinity)
    {
#if defined(_ONECORE)
        // This API should not be called by modern APPs
        ENSURE_NOT_APP();
#else
        { // begin locked region
            _StaticLock::_Scoped_lock lock(s_lock);
            if (s_pResourceManager != NULL)
            {
                throw invalid_operation();
            }

            if (s_version == ::Concurrency::IResourceManager::UnsupportedOS)
            {
                RetrieveSystemVersionInformation();
            }

            // This API can be invoked on Win7 and higher, since operating systems below that version do not support processor groups.
            if (s_version < ::Concurrency::IResourceManager::Win7OrLater)
            {
                throw invalid_operation();
            }

            if (count == 0)
            {
                throw std::invalid_argument("count");
            }

            if (pGroupAffinity == NULL)
            {
                throw std::invalid_argument("pGroupAffinity");
            }

            HardwareAffinity * pAffinity = _concrt_new HardwareAffinity[count];
            for (int i = 0; i < count; ++i)
            {
                pAffinity[i] = HardwareAffinity(pGroupAffinity[i].Group, pGroupAffinity[i].Mask);
            }

            // Sort by the group number -> lowest first selection sort. Duplicates are invalid.
            for (unsigned int i = 0; i < count; ++i)
            {
                unsigned int minIndex = i;
                for (unsigned int j = i + 1; j < count; ++j)
                {
                    if (pAffinity[j].GetGroup() == pAffinity[minIndex].GetGroup())
                    {
                        throw std::invalid_argument("pGroupAffinity");
                    }
                    else if (pAffinity[j].GetGroup() < pAffinity[minIndex].GetGroup())
                    {
                        minIndex = j;
                    }
                }
                if (i != minIndex)
                {
                    HardwareAffinity tempAffinity = pAffinity[i];
                    pAffinity[i] = pAffinity[minIndex];
                    pAffinity[minIndex] = tempAffinity;
                }
            }

            // There is no need to capture process affinity, since we will override process affinity, if necessary. There is no need to
            // modify the process affinity if the user supplied affinity doesn't intersect with it, as we do in the case of Vista, since
            // the APIs we use to set thread affinity on Win7 and higher are able to override process affinity themselves.
            // Ensure that the passed in affinity is a subset of what is available on the system.
            GetTopologyInformation(RelationGroup);
            // Cast this buffer as a PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, which is needed for GetLogicalProcessorInformationEx
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pSysInfoEx = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) s_pSysInfo;

            ASSERT(pSysInfoEx->Relationship == RelationGroup);
            PGROUP_RELATIONSHIP pGroupRelationship = (PGROUP_RELATIONSHIP) &(pSysInfoEx->Group);
            if (count > pGroupRelationship->ActiveGroupCount)
            {
                throw std::invalid_argument("count");
            }

            // This variable is used to check if the overall affinity is 0 and we throw an error if it is.
            KAFFINITY mergedAffinity = 0;

            // Go through the groups listed and ensure that the affinity requested by the user is a subset of the affinity offered by the system.
            // 'i' iterates through the array of groups provided by the user whereas 'j' iterates through the array of groups we obtained from the system.
            for (unsigned short i = 0, j = 0; i < count; ++i)
            {
                // Increment through the system group array until we find a group that matches the next one in the user provided array.
                while (j < pAffinity[i].GetGroup() && j < pGroupRelationship->ActiveGroupCount)
                {
                    ++j;
                }
                if (j == pGroupRelationship->ActiveGroupCount)
                {
                    // The user has specified an invalid group number.
                    throw std::invalid_argument("pGroupAffinity");
                }
                ASSERT(j == pAffinity[i].GetGroup());
                pAffinity[i].IntersectWith(pGroupRelationship->GroupInfo[j].ActiveProcessorMask);
                mergedAffinity |= pAffinity[i].GetMask();
            }

            if (mergedAffinity == 0)
            {
                // The user has a mask that results in 0 processors being assigned to the RM.
                throw std::invalid_argument("pGroupAffinity");
            }

            CleanupTopologyInformation();

            // We have a valid affinity restriction. Delete any existing restriction structure if it is present.
            delete s_pUserAffinityRestriction;
            s_pUserAffinityRestriction = _concrt_new AffinityRestriction(count, pAffinity);

        } // end locked region

#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Creates the dynamic RM worker thread and allocates memory for its use. The worker thread wakes up at
    ///     fixed intervals and load balances resources among schedulers, until it it put on standby.
    /// </summary>
    void ResourceManager::CreateDynamicRMWorker()
    {
        // MSDK compliant applications are not allowed to create multiple schedulers.
        // We do not need the dynamic RM worker for a single scheduler.
        ENSURE_NOT_APP();

        // NOTE: This routine is called *without* holding m_lock.
        // Set up a background thread for dynamic RM.
        m_hDynamicRMThreadHandle = LoadLibraryAndCreateThread(NULL,
                                           DEFAULTCONTEXTSTACKSIZE,
                                           DynamicRMThreadProc,
                                           this,
                                           0,
                                           NULL);

        if (m_hDynamicRMThreadHandle == NULL)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Make sure the background thread is running at the highest priority.
        platform::__SetThreadPriority(m_hDynamicRMThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
    }

    /// <summary>
    ///     Initializes static variables related to the operating system version.
    /// </summary>
    void ResourceManager::RetrieveSystemVersionInformation()
    {
        // Determine the OS version
#if     defined(_ONECORE)
        // MSDK support is in Windows 8 (or later)
        s_version = ::Concurrency::IResourceManager::Win8OrLater;
#elif   (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        OSVERSIONINFOW osvi;
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

// Fix for warnings as errors when building against WinBlue build 9444.0.130614-1739
// warning C4996: 'GetVersionExW': was declared deprecated
// externalapis\windows\8.1\sdk\inc\sysinfoapi.h(442)
// Deprecated. Use VerifyVersionInfo* or IsWindows* macros from VersionHelpers.
#pragma warning( disable : 4996 )
        if( !GetVersionExW(&osvi))
        {
            throw ::Concurrency::unsupported_os();
        }
#pragma warning( default : 4996 )

        switch (osvi.dwMajorVersion)
        {
        case 5:
            switch (osvi.dwMinorVersion)
            {
            case 2: // Win2k3 or XP-64
                s_version = ::Concurrency::IResourceManager::Win2k3;
                break;
            case 1: // XP
                s_version = ::Concurrency::IResourceManager::XP;
                break;
            case 0: // Win2k -- unsupported
                s_version = ::Concurrency::IResourceManager::UnsupportedOS;
                throw ::Concurrency::unsupported_os();
                break;
            }
            break;
        case 6: // Vista
            switch (osvi.dwMinorVersion)
            {
            case 0:
                s_version = ::Concurrency::IResourceManager::Vista;
                break;
            case 1:
                s_version = ::Concurrency::IResourceManager::Win7OrLater;
                break;
            default:
                s_version = ::Concurrency::IResourceManager::Win8OrLater;
                break;
            }
            break;

        default:
            s_version = ::Concurrency::IResourceManager::Win8OrLater;
            break;
        }
#else   // !(defined(_M_IX86) || defined(_M_X64)) || defined(_M_HYBRID)
        s_version = ::Concurrency::IResourceManager::Win8OrLater;
#endif  // !(defined(_M_IX86) || defined(_M_X64)) || defined(_M_HYBRID)

        // Initialize other information based on the OS version
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        if (s_version >= ::Concurrency::IResourceManager::Win7OrLater)
        {
            // Initialize function pointers to APIs that are only present on Windows 7 or later
            platform::InitializeSystemFunctionPointers();

#ifdef  _M_X64
#ifndef _UMS_DISABLED
            UMS::Initialize();
#endif // _UMS_DISABLED
#endif  // _M_X64

            if (s_version >= ::Concurrency::IResourceManager::Win8OrLater)
            {
                WinRT::Initialize();
            }
        }
#else   // !(defined(_M_IX86) || defined(_M_X64)) || defined(_M_HYBRID)
        WinRT::Initialize();
#endif  // !(defined(_M_IX86) || defined(_M_X64)) || defined(_M_HYBRID)
    }

    /// <summary>
    ///     Captures the process affinity if it is not equal to the system affinity.
    /// </summary>
    void ResourceManager::CaptureProcessAffinity()
    {
#if defined(_ONECORE)
        ENSURE_NOT_APP();
#else
        if (!GetProcessAffinityMask(GetCurrentProcess(), &s_processAffinityMask, &s_systemAffinityMask))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Check if the system affinity is different from the process affinity.
        if (s_processAffinityMask != s_systemAffinityMask)
        {
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
            if (s_version >= ::Concurrency::IResourceManager::Win7OrLater)
#endif
            {
                // On a multigroup machine, a process is assigned to one group by default, and the system and process affinity reflect the
                // affinity of the system/process within that group. If the process affinity is different from the system affinity for a
                // multigroup machine, we will limit this process to the process affinity in the current group.
                HardwareAffinity currentThreadAffinity(GetCurrentThread());
                HardwareAffinity * pCurrentProcessAffinity = _concrt_new HardwareAffinity(currentThreadAffinity.GetGroup(), s_processAffinityMask);
                ASSERT(s_pProcessAffinityRestriction == NULL);
                s_pProcessAffinityRestriction = _concrt_new AffinityRestriction(1, pCurrentProcessAffinity);
            }
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
            else
            {
                ASSERT(s_pProcessAffinityRestriction == NULL);
                s_pProcessAffinityRestriction = _concrt_new AffinityRestriction(1, _concrt_new HardwareAffinity(s_processAffinityMask));
            }
#endif
        }
#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Modify the passed in affinity based on any user or process affinity restrictions.
    /// </summary>
    void ResourceManager::ApplyAffinityRestrictions(PGROUP_AFFINITY pGroupAffinity)
    {
#if defined(_ONECORE)
        ENSURE_NOT_APP();
#else
        ASSERT(s_pProcessAffinityRestriction == NULL || s_pUserAffinityRestriction == NULL);
        if (pGroupAffinity->Mask != 0)
        {
            if (s_pProcessAffinityRestriction != NULL)
            {
                s_pProcessAffinityRestriction->ApplyAffinityLimits(pGroupAffinity);
            }
            else if (s_pUserAffinityRestriction != NULL)
            {
                s_pUserAffinityRestriction->ApplyAffinityLimits(pGroupAffinity);
            }
        }
#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Modify the passed in affinity based on any user or process affinity restrictions.
    /// </summary>
    void ResourceManager::ApplyAffinityRestrictions(PULONG_PTR pProcessorMask)
    {
#if defined(_ONECORE)
        ENSURE_NOT_APP();
#else
        GROUP_AFFINITY groupAffinity = {0};
        groupAffinity.Group = 0;
        groupAffinity.Mask = *pProcessorMask;

        ApplyAffinityRestrictions(&groupAffinity);

        *pProcessorMask = groupAffinity.Mask;
#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Retrieves a buffer from the operating system that contains topology information.
    /// </summary>
    void ResourceManager::GetTopologyInformation(LOGICAL_PROCESSOR_RELATIONSHIP relationship)
    {
#if defined(_ONECORE)
        ENSURE_NOT_APP();
#else
        ASSERT(s_version != ::Concurrency::IResourceManager::UnsupportedOS);
        ASSERT(s_version != ::Concurrency::IResourceManager::XP &&
               s_version != ::Concurrency::IResourceManager::Win2k3);

#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        if (s_version >= ::Concurrency::IResourceManager::Win7OrLater)
#endif
        {
            s_pSysInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) platform::__GetLogicalProcessorInformationEx(relationship, &s_logicalProcessorInformationLength);
        }
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        else
        {
            s_pSysInfo = platform::__GetLogicalProcessorInformation(&s_logicalProcessorInformationLength);
        }
#endif
#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Cleans up the variables that store operating system topology information.
    /// </summary>
    void ResourceManager::CleanupTopologyInformation()
    {
#if defined(_ONECORE)
        ENSURE_NOT_APP();
#else
        free(s_pSysInfo);
        s_pSysInfo = NULL;
        s_logicalProcessorInformationLength = 0;

#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Initializes static information related to the operating system and machine topology.
    /// </summary>
    void ResourceManager::InitializeSystemInformation(bool fSaveTopologyInfo)
    {
        if (s_version == ::Concurrency::IResourceManager::UnsupportedOS)
        {
            RetrieveSystemVersionInformation();
        }

        ASSERT(s_version != ::Concurrency::IResourceManager::UnsupportedOS);
        ASSERT(s_pSysInfo == NULL);

#if defined(_ONECORE)
        SYSTEM_INFO sysInfo;
        GetNativeSystemInfo(&sysInfo);

        // These static variables should be updated exactly once, so that concurrent reads do not see intermediate values.
        s_countProcessorPackages = false;
        s_nodeCount = 1;
        s_coreCount = sysInfo.dwNumberOfProcessors;
        s_numaNodeCount = 1;
#else
        // If the user specified an affinity restriction, use it. If not, we should ensure that
        // the RM will adhere to the process affinity.
        if (s_pUserAffinityRestriction == NULL)
        {
            CaptureProcessAffinity();
        }
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        if (s_version >= ::Concurrency::IResourceManager::Win7OrLater)
#endif
        {
            // Retrieve topology related information. This populates s_pSysInfo and s_logicalProcessorInformationLength, using the correct
            // API, based on the operating system version.
            GetTopologyInformation(RelationAll);

            unsigned int processorPackageCount = 0;
            unsigned int coreCount = 0;
            unsigned int numaNodeCount = 0;

            // Traverse the processor information buffer to find s_nodeCount and s_coreCount. It's possible for a package
            // to contain multiple NUMA nodes.  In the case were we have more NUMA nodes than packages, we will create one
            // scheduling node per NUMA node.  Otherwise, we create one scheduling node per package.

            // There are cases where the number of processor packages is greater than the number of NUMA nodes, but one or more
            // processor packages are split between two groups. A scheduling node will not span groups, therefore the node count
            // will be incremented for every such processor package.

            // In addition to traversing the buffer and calculating the number of nodes, modify the buffer to reflect the affinity
            // limitations either due to process affinity restrictions or because the user has specified a limited affinity.
            ASSERT(s_logicalProcessorInformationLength > 0);
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pSysInfoEx;
            for (auto raw = reinterpret_cast<char*>(s_pSysInfo), end = raw + s_logicalProcessorInformationLength;
                raw < end; raw += pSysInfoEx->Size)
            {
                pSysInfoEx = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(raw);
                switch (pSysInfoEx->Relationship)
                {
                case RelationProcessorPackage:
                    {
                        bool packageHasAddressableCores = false;
                        for (WORD index = 0; index < pSysInfoEx->Processor.GroupCount; ++index)
                        {
                            // It is possible for a processor package to span multiple groups.
                            PGROUP_AFFINITY pGroupAffinity = &pSysInfoEx->Processor.GroupMask[index];
                            ApplyAffinityRestrictions(pGroupAffinity);
                            packageHasAddressableCores |= pGroupAffinity->Mask != 0;
                        }

                        processorPackageCount += packageHasAddressableCores;
                    }
                    break;
                case RelationNumaNode:
                    {
                        PGROUP_AFFINITY pGroupAffinity = &pSysInfoEx->NumaNode.GroupMask;
                        ApplyAffinityRestrictions(pGroupAffinity);
                        numaNodeCount += pSysInfoEx->NumaNode.GroupMask.Mask != 0;
                    }
                    break;
                case RelationProcessorCore:
                    {
                        PGROUP_AFFINITY pGroupAffinity = &pSysInfoEx->Processor.GroupMask[0];
                        ApplyAffinityRestrictions(pGroupAffinity);
                        coreCount += NumberOfBitsSet(pGroupAffinity->Mask);
                    }
                    break;
                }
            }

            ASSERT(processorPackageCount > 0 && numaNodeCount > 0 && coreCount >= 0);

            // These static variables should be updated exactly once, so that concurrent reads do not see intermediate values.
            s_countProcessorPackages = (processorPackageCount > numaNodeCount);
            s_nodeCount = s_countProcessorPackages ? processorPackageCount : numaNodeCount;
            s_coreCount = coreCount;
            s_numaNodeCount = numaNodeCount;

            if (!fSaveTopologyInfo)
            {
                CleanupTopologyInformation();
            }
        }
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        else if(s_version == ::Concurrency::IResourceManager::Vista)
        {
            // Retrieve topology related information. This populates s_pSysInfo and s_logicalProcessorInformationLength, using the correct
            // API, based on the operating system version.
            GetTopologyInformation(RelationAll);

            unsigned int processorPackageCount = 0;
            unsigned int coreCount = 0;
            unsigned int numaNodeCount = 0;

            // Traverse the processor information buffer to find s_nodeCount. It's possible for a package to contain multiple
            // NUMA nodes.  In the case were we have more NUMA nodes than packages, we will create one scheduling node per NUMA node.
            // Otherwise, we create one scheduling node per package.
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pSysInfo = s_pSysInfo;
            ASSERT(s_logicalProcessorInformationLength > 0);
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pEnd =
                pSysInfo + s_logicalProcessorInformationLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            for(; pSysInfo < pEnd; ++pSysInfo)
            {
                switch (pSysInfo->Relationship)
                {
                case RelationProcessorPackage:
                    {
                        ApplyAffinityRestrictions(&pSysInfo->ProcessorMask);
                        if (pSysInfo->ProcessorMask != 0)
                        {
                            ++processorPackageCount;
                        }
                    }
                    break;
                case RelationNumaNode:
                    {
                        ApplyAffinityRestrictions(&pSysInfo->ProcessorMask);
                        if (pSysInfo->ProcessorMask != 0)
                        {
                            ++numaNodeCount;
                        }
                    }
                    break;
                case RelationProcessorCore:
                    ApplyAffinityRestrictions(&pSysInfo->ProcessorMask);
                    coreCount += NumberOfBitsSet(pSysInfo->ProcessorMask);
                    break;
                }
            }

            ASSERT(processorPackageCount > 0 && numaNodeCount > 0 && coreCount > 0);

            // These static variables should be updated exactly once, so that concurrent reads do not see intermediate values.
            s_countProcessorPackages = (processorPackageCount > numaNodeCount);
            s_nodeCount = s_countProcessorPackages ? processorPackageCount : numaNodeCount;
            s_coreCount = coreCount;
            s_numaNodeCount = numaNodeCount;

            if (!fSaveTopologyInfo)
            {
                CleanupTopologyInformation();
            }
        }
        else
        {
            s_countProcessorPackages = false;
            s_nodeCount = 1;

            DWORD_PTR affinityMask;
            if( s_pUserAffinityRestriction == NULL )
            {
                affinityMask = s_processAffinityMask;
            }
            else
            {
                CaptureProcessAffinity();
                HardwareAffinity * hw_affinity = s_pUserAffinityRestriction->FindGroupAffinity(0);
                ASSERT(hw_affinity != NULL);
                affinityMask = s_processAffinityMask & hw_affinity->GetMask();
            }
            s_coreCount = NumberOfBitsSet(affinityMask);

            s_numaNodeCount = 1;
        }
#endif

        delete s_pProcessAffinityRestriction;
        s_pProcessAffinityRestriction = NULL;

#endif // defined(_ONECORE)

        ASSERT(s_coreCount > 0 && s_coreCount <= USHORT_MAX);
    }

    /// <summary>
    ///     Creates a structure of nodes and cores based on the machine topology.
    /// </summary>
    void ResourceManager::DetermineTopology()
    {
        ASSERT(m_pGlobalNodes == NULL && m_pGlobalNumaNodes == NULL);
        ASSERT(s_nodeCount > 0 && s_nodeCount <= INT_MAX);

        m_numaNodeCount = s_numaNodeCount;
        m_nodeCount = s_nodeCount;
        m_coreCount = s_coreCount;

        m_pGlobalNumaNodes = _concrt_new GlobalNumaNode[m_numaNodeCount];
        memset(m_pGlobalNumaNodes, 0, m_numaNodeCount * sizeof(GlobalNumaNode));

        m_pGlobalNodes = _concrt_new GlobalNode[m_nodeCount];
        memset(m_pGlobalNodes, 0, m_nodeCount * sizeof(GlobalNode));

#if defined(_ONECORE)
        SYSTEM_INFO sysInfo;
        GetNativeSystemInfo(&sysInfo);
        ULONG_PTR affinityMask = (s_pUserAffinityRestriction != NULL) ? s_pUserAffinityRestriction->FindGroupAffinity(0)->GetMask() : sysInfo.dwActiveProcessorMask;

        ASSERT(m_coreCount == sysInfo.dwNumberOfProcessors);
        m_pGlobalNodes[0].Initialize(this, 0, 0, affinityMask);
        m_pGlobalNumaNodes[0].Initialize(0, 0, 0);
#else

#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        // Win7 or higher has a PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX structure to support > 64 cores
        if (s_version >= ::Concurrency::IResourceManager::Win7OrLater)
#endif
        {
            // Traverse the processor information buffer for a second time to populate the node structures.
            DWORD byteOffset = 0;
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pSysInfoEx = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) s_pSysInfo;
            unsigned int nodeIndex = 0;
            unsigned int numaIndex = 0;

            while (byteOffset < s_logicalProcessorInformationLength)
            {
                switch (pSysInfoEx->Relationship)
                {
                    case RelationProcessorPackage:
                        {
                            // If this machine had more packages than NUMA nodes, we should create one scheduling node per package.
                            if (s_countProcessorPackages)

                            {
                                ASSERT(pSysInfoEx->Processor.GroupCount >= 1);
                                int group = 0;
                                do
                                {
                                    if (pSysInfoEx->Processor.GroupMask[group].Mask != 0)
                                    {
                                        // Note that the group affinity mask has already been modified to reflect affinity restrictions placed by process affinity or the user.
                                        m_pGlobalNodes[nodeIndex].Initialize(this, static_cast<USHORT>(nodeIndex), pSysInfoEx->Processor.GroupMask[group].Group,
                                                                             pSysInfoEx->Processor.GroupMask[group].Mask);
                                        ++nodeIndex;
                                    }
                                    ++group;
                                } while(group < pSysInfoEx->Processor.GroupCount);
                            }
                        }
                        break;
                    case RelationNumaNode:
                        {
                            // If this machine had more NUMA nodes than packages, we should create one scheduling node per NUMA node.
                            if (!s_countProcessorPackages)
                            {
                                if (pSysInfoEx->NumaNode.GroupMask.Mask != 0)
                                {
                                    // Note that the group affinity mask has already been modified to reflect affinity restrictions placed by process affinity or the user.
                                    m_pGlobalNodes[nodeIndex].Initialize(this, static_cast<USHORT>(nodeIndex), pSysInfoEx->NumaNode.GroupMask.Group, pSysInfoEx->NumaNode.GroupMask.Mask);
                                    m_pGlobalNodes[nodeIndex].m_numaNodeNumber = pSysInfoEx->NumaNode.NodeNumber;

                                    ++nodeIndex;
                                }
                            }

                            // We also need to track NUMA topology and association with processor packages.

                            if (pSysInfoEx->NumaNode.GroupMask.Mask != 0)
                            {
                                // Note that the group affinity mask has already been modified to reflect affinity restrictions placed by process affinity or the user.
                                m_pGlobalNumaNodes[numaIndex].Initialize(pSysInfoEx->NumaNode.NodeNumber,
                                                                         pSysInfoEx->NumaNode.GroupMask.Group,
                                                                         pSysInfoEx->NumaNode.GroupMask.Mask);

                                ++numaIndex;
                            }
                        }
                        break;
                    default:
                        break;
                }

                byteOffset += pSysInfoEx->Size;
                pSysInfoEx = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) ((PBYTE)pSysInfoEx + pSysInfoEx->Size);
            }

            ASSERT(nodeIndex == m_nodeCount);
            ASSERT(numaIndex == m_numaNodeCount);

            CleanupTopologyInformation();
        }
#if     (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
        else
        {
            if (s_pSysInfo == NULL)
            {
                ASSERT(s_version == ::Concurrency::IResourceManager::XP || s_version == ::Concurrency::IResourceManager::Win2k3);
                ASSERT(m_nodeCount == 1);

                ULONG_PTR affinityMask = (s_pUserAffinityRestriction != NULL) ? s_pUserAffinityRestriction->FindGroupAffinity(0)->GetMask() : s_processAffinityMask;
                m_pGlobalNodes->Initialize(this, 0, 0, affinityMask);
                m_pGlobalNumaNodes->m_numaAffinity = affinityMask;
            }
            else
            {
                ASSERT(s_version == ::Concurrency::IResourceManager::Vista);

                // Traverse the processor information buffer for a second time to populate the node structures.
                DWORD byteOffset = 0;
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pSysInfo = s_pSysInfo;
                unsigned int nodeIndex = 0;
                unsigned int numaIndex = 0;

                while (byteOffset < s_logicalProcessorInformationLength)
                {
                    switch (pSysInfo->Relationship)
                    {
                        case RelationProcessorCore:
                            break;

                        case RelationProcessorPackage:
                            {
                                // If this machine had more packages than NUMA nodes, we should create one scheduling node per package.
                                if (s_countProcessorPackages)
                                {
                                    if (pSysInfo->ProcessorMask != 0)
                                    {
                                        // Note that the affinity mask has already been modified to reflect affinity restrictions placed by process affinity or the user.
                                        m_pGlobalNodes[nodeIndex].Initialize(this, static_cast<USHORT>(nodeIndex), 0, pSysInfo->ProcessorMask);
                                        ++nodeIndex;
                                    }
                                }
                            }
                            break;

                        case RelationNumaNode:
                            {
                                // If this machine had more NUMA nodes than packages, we should create one scheduling node per NUMA node.
                                if (!s_countProcessorPackages)
                                {
                                    if (pSysInfo->ProcessorMask != 0)
                                    {
                                        // Note that the affinity mask has already been modified to reflect affinity restrictions placed by process affinity or the user.
                                        m_pGlobalNodes[nodeIndex].Initialize(this, static_cast<USHORT>(nodeIndex), 0, pSysInfo->ProcessorMask);
                                        m_pGlobalNodes[nodeIndex].m_numaNodeNumber = pSysInfo->NumaNode.NodeNumber;
                                        ++nodeIndex;
                                    }
                                }

                                // We also need to track NUMA topology and association with processor packages.
                                if (pSysInfo->ProcessorMask != 0)
                                {
                                    // Note that the affinity mask has already been modified to reflect affinity restrictions placed by process affinity or the user.
                                    m_pGlobalNumaNodes[numaIndex].Initialize(pSysInfo->NumaNode.NodeNumber, 0, pSysInfo->ProcessorMask);
                                    ++numaIndex;

                                }
                            }
                            break;
                    }

                    byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
                    ++pSysInfo;
                }

                ASSERT(nodeIndex == m_nodeCount);
                ASSERT(numaIndex == m_numaNodeCount);

                CleanupTopologyInformation();
            }
        }
#endif  // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)

        //
        // If we count processor packages, go back and map those processor packages to the NUMA information.
        //
        if (s_countProcessorPackages)
        {
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                bool fDiscovered = false;
                for (unsigned int j = 0; j < m_numaNodeCount; ++j)
                {
                    if (m_pGlobalNodes[i].m_processorGroup == m_pGlobalNumaNodes[j].m_processorGroup &&
                        (m_pGlobalNodes[i].m_nodeAffinity & m_pGlobalNumaNodes[j].m_numaAffinity))
                    {
                        //
                        // The node should be fully contained within the NUMA node.
                        //
                        ASSERT((m_pGlobalNodes[i].m_nodeAffinity & m_pGlobalNumaNodes[j].m_numaAffinity) == m_pGlobalNodes[i].m_nodeAffinity);
                        m_pGlobalNodes[i].m_numaNodeNumber = m_pGlobalNumaNodes[j].m_numaNodeNumber;
                        fDiscovered = true;
                        break;
                    }
                }

                ASSERT(fDiscovered);
            }
        }

#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Increments the reference count of a resource manager.
    /// </summary>
    /// <returns>
    ///     Returns the resulting reference count.
    /// </returns>
    unsigned int ResourceManager::Reference()
    {
        return (unsigned int) InterlockedIncrement(&m_referenceCount);
    }

    /// <summary>
    ///     Increments the reference count to RM but does not allow a 0 to 1 transition.
    /// </summary>
    /// <returns>
    ///     True if RM was referenced, false, if the reference count was 0.
    /// </returns>
    bool ResourceManager::SafeReference()
    {
        return SafeInterlockedIncrement(&m_referenceCount);
    }

    /// <summary>
    ///     Decrements the reference count of a resource manager.
    /// </summary>
    unsigned int ResourceManager::Release()
    {
        long rc = InterlockedDecrement(&m_referenceCount);
        if (rc == 0)
        {
            { // begin locked region
                _StaticLock::_Scoped_lock lock(s_lock);
                if (this == (ResourceManager*) Security::DecodePointer(s_pResourceManager))
                {
                    // A new s_pRM could be created in CreateSingleton, we can only set the static pointer to null
                    // if it is the same as 'this'.
                    s_pResourceManager = NULL;
                }
            } // end locked region

            if (m_hDynamicRMThreadHandle != NULL)
            {
                { // begin locked region
                    _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

                    ASSERT(m_hDynamicRMThreadHandle != (HANDLE)1);
                    // Cause the dynamic RM background thread to Exit and wait for it to finish executing
                    ASSERT(m_dynamicRMWorkerState == Standby);
                    m_dynamicRMWorkerState = Exit;
                } // end locked region

                WakeupDynamicRMWorker();
                platform::__WaitForThread(m_hDynamicRMThreadHandle, INFINITE);
            }

            delete this;
        }
        return (unsigned int) rc;
    }

    /// <summary>
    ///     Debug CRT test hook to create artificial topologies. With the retail CRT, this API simply returns.
    /// </summary>
    void ResourceManager::CreateNodeTopology(unsigned int nodeCount, unsigned int *pCoreCount, unsigned int **pNodeDistance, unsigned int *pProcessorGroups)
    {
#if defined(_ONECORE)
        (nodeCount); (pCoreCount); (pNodeDistance); (pProcessorGroups);
        ENSURE_NOT_APP();
#else
#if defined(_DEBUG)

        if (pCoreCount == NULL)
        {
            throw std::invalid_argument("pCoreCount");
        }

        if (nodeCount < 1)
        {
            throw std::invalid_argument("nodeCount");
        }

        { // begin locked region
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);
            if ( !m_schedulers.Empty())
            {
                throw invalid_operation();
            }

            // Destroy the existing node structure.
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                delete [] m_pGlobalNodes[i].m_pCores;
            }
            delete [] m_pGlobalNodes;
#if defined(CONCRT_TRACING)
        delete [] m_drmInitialState;
#endif
            s_nodeCount = m_nodeCount = nodeCount;
            unsigned int coreCount = 0;

            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                coreCount += pCoreCount[i];
            }
            m_coreCount = s_coreCount = coreCount;

            m_pGlobalNodes = _concrt_new GlobalNode[m_nodeCount];
            memset(m_pGlobalNodes, 0, sizeof(GlobalNode) * m_nodeCount);

            //
            // This is a patch for the test hook to allow schedulers to actually be created with the "fake" underlying
            // topology as long as the group numbers are valid for the machine.
            //
            ULONG_PTR processAffinityMask = 0;
            ULONG_PTR systemAffinityMask = 0;

            BOOL retVal = GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask);
            ASSERT(retVal);

            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                USHORT processorGroup = 0;
                unsigned int baseProcNum = 0;

                if (pProcessorGroups != NULL)
                {
                    if (i > 0 && (pProcessorGroups[i] != pProcessorGroups[i - 1]))
                    {
                        // Reset the proc number to zero since we've encountered a new group.
                        baseProcNum = 0;
                    }
                    processorGroup = static_cast<USHORT>(pProcessorGroups[i]);
                }

                m_pGlobalNodes[i].Initialize(this, static_cast<USHORT>(i), processorGroup, processAffinityMask, (unsigned int)pCoreCount[i], baseProcNum);
            }
#if defined(CONCRT_TRACING)
            // Assumes a m x n allocation.
            m_numTotalCores = m_nodeCount * m_pGlobalNodes[0].m_coreCount;
            m_drmInitialState = _concrt_new GlobalCoreData[m_numTotalCores];
            memset(m_drmInitialState, 0, sizeof(GlobalCoreData) * m_numTotalCores);
#endif
        } // end locked region
#endif // if defined(_DEBUG)

#endif // defined(_ONECORE)
    }

    /// <summary>
    ///     Associate an IScheduler with the ISchedulerProxy that represents that part
    //      of IResourceManager associated with the IScheduler.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler be associated.
    /// </param>
    /// <param name="version">
    ///     The version of the RM<->Scheduler communication channel that is being utilized.
    /// </param>
    ISchedulerProxy *ResourceManager::RegisterScheduler(IScheduler *pScheduler, unsigned int version)
    {
        if (pScheduler == NULL)
            throw std::invalid_argument("pScheduler");

        if (version != CONCRT_RM_VERSION_1)
            throw std::invalid_argument("version");

        return CreateSchedulerProxy(pScheduler);
    }

    /// <summary>
    ///     Allocates and initializes the data structure that will represent the allocated nodes for a scheduler proxy.
    ///     This function is called the first time a scheduler proxy requests an allocation.
    /// </summary>
    SchedulerNode * ResourceManager::CreateAllocatedNodeData()
    {
        SchedulerNode * pNodes = _concrt_new SchedulerNode[m_nodeCount];
        memset(pNodes, 0, m_nodeCount * sizeof(SchedulerNode));

        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode * pNode = &pNodes[nodeIndex];
            GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];

            // Copy the base class portion of the node, which is shared.
            memcpy(pNode, pGlobalNode, sizeof(ProcessorNode));
            ASSERT(pNode->m_availableCores == 0);

            pNode->m_pCores = _concrt_new SchedulerCore[pNode->m_coreCount];
            memset(pNode->m_pCores, 0, pNode->m_coreCount * sizeof(SchedulerCore));

            for (unsigned int coreIndex = 0; coreIndex < pNode->m_coreCount; ++coreIndex)
            {
                // Copy the base class portion of the core.
                memcpy(&pNode->m_pCores[coreIndex], &pGlobalNode->m_pCores[coreIndex], sizeof(ProcessorCore));
                // All cores in the local map start out 'unassigned' to the scheduler proxy.
                pNode->m_pCores[coreIndex].m_coreState = ProcessorCore::Unassigned;

                // Each scheduler core has a pointer to the global use count for that core.
                pNode->m_pCores[coreIndex].m_pGlobalUseCountPtr = &pGlobalNode->m_pCores[coreIndex].m_useCount;
            }
        }
        return pNodes;
    }

    /// <summary>
    ///     Destroys the data structures representing nodes/cores allocated to a scheduler proxy when the proxy has
    ///     shutdown.
    /// </summary>
    void ResourceManager::DestroyAllocatedNodeData(SchedulerNode * pAllocatedNodes)
    {
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            delete [] pAllocatedNodes[i].m_pCores;
        }
        delete [] pAllocatedNodes;
    }

    /// <summary>
    ///     Called by a scheduler in order make an initial request for an allocation of virtual processors.  The request
    ///     is driven by policies within the scheduler queried via the IScheduler::GetPolicy method.  If the request
    ///     can be satisfied via the rules of allocation, it is communicated to the scheduler as a call to
    ///     IScheduler::AddVirtualProcessors.
    /// </summary>
    /// <param name="pProxy">
    ///     The scheduler proxy that is making the allocation request.
    /// </param>
    /// <param name="doSubscribeCurrentThread">
    ///     Whether to subscribe the current thread and account for it during resource allocation.
    /// </param>
    /// <returns>
    ///     The IExecutionResource instance representing current thread if doSubscribeCurrentThread was true; NULL otherwise.
    /// </returns>
    IExecutionResource * ResourceManager::RequestInitialVirtualProcessors(SchedulerProxy *pProxy, bool doSubscribeCurrentThread)
    {
        bool createWorkerThread = false;
        ExecutionResource * pExecutionResource = NULL;
        bool doExternalThreadAllocation = false;

        { // begin locked region
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

            ASSERT(pProxy->GetNumExternalThreads() == 0);

            if (doSubscribeCurrentThread)
            {
                // If the current thread is either already subscribed, or represents a virtual processor root, we return the
                // existing execution resource.
                pExecutionResource = pProxy->ReferenceCurrentThreadExecutionResource();

                if (pExecutionResource == NULL)
                {
                    doExternalThreadAllocation = true;
                }
            }

            // Increment this count before performaing the allocation. If the new scheduler activates vprocs at the time
            // they are added, we use this information to decide whether core busy/idle notifications need to be sent to other schedulers.
            if (pProxy->ShouldReceiveNotifications())
            {
                ++m_numSchedulersNeedingNotifications;
            }

            ++m_numSchedulers;
            m_schedulers.AddTail(pProxy);

            // Based on the policy of the scheduler proxy, and the load on the system, allocate cores to this proxy.
            // The API will invoke a scheduler proxy callback (GrantAllocation) before it returns.
            ExecutionResource * pNewExecutionResource = PerformAllocation(pProxy, true, doExternalThreadAllocation);

            // If this external thread did not exist in the RM already, get it from PerformAllocation.
            if (pExecutionResource == NULL)
            {
                pExecutionResource = pNewExecutionResource;
            }
            else
            {
                ASSERT(pNewExecutionResource == NULL);
            }

            if (pProxy->ShouldReceiveNotifications())
            {
                SendResourceNotifications(pProxy);
            }

            if (m_numSchedulers != 2)
            {
                return pExecutionResource;
            }

            // We've just added the second scheduler. We need to either create or wake up the dynamic RM worker thread.
            ASSERT(m_dynamicRMWorkerState == Standby);
            m_dynamicRMWorkerState = LoadBalance;

            if (m_hDynamicRMThreadHandle == NULL)
            {
                // Store a temporary value before releasing the lock and proceeding to allocate memory/create the thread.
                // This is to prevent a duplicate allocation if the refcount goes from 2->1 and back to 2 after the lock is released.
                m_hDynamicRMThreadHandle = (HANDLE)1;

                // Initialize the memory used for DRM under the lock, since these variables are touched in the static RM path as well
                ASSERT(m_ppProxyData != NULL);
                m_ppGivingProxies = _concrt_new DynamicAllocationData * [m_maxSchedulers];
                m_ppReceivingProxies = _concrt_new DynamicAllocationData * [m_maxSchedulers];

                createWorkerThread = true;
            }
        } // end locked region

        // Set the event outside the lock to prevent the high priority thread from having to block immediately upon starting up.
        WakeupDynamicRMWorker();

        // Create the thread/data or set the dynamic RM event after releasing the lock to prevent the high priority thread
        // from having to block immediately upon starting up.
        if (createWorkerThread)
        {
            CreateDynamicRMWorker();
        }

        return pExecutionResource;
    }

    /// <summary>
    ///     This API registers the current thread with the resource manager associating it with this scheduler proxy,
    ///     and returns an instance of IExecutionResource back to the scheduler, for bookkeeping and maintenance.
    /// </summary>
    /// <returns>
    ///     The IExecutionResource instance representing current thread in the runtime.
    /// </returns>
    ExecutionResource * ResourceManager::SubscribeCurrentThread(SchedulerProxy *pSchedulerProxy)
    {
        ExecutionResource * pExecutionResource = NULL;

        { // begin locked region
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

            pExecutionResource = pSchedulerProxy->ReferenceCurrentThreadExecutionResource();

            // Create an execution resources if the current thread does not already have one.
            if (pExecutionResource == NULL)
            {
                pExecutionResource = PerformAllocation(pSchedulerProxy, false, true);
            }

        } // end locked region

        return pExecutionResource;
    }

    /// <summary>
    ///     Removes an execution resource that was created for an external thread.
    /// </summary>
    void ResourceManager::RemoveExecutionResource(ExecutionResource * pExecutionResource)
    {
        bool signalDRM = false;

        { // begin locked region
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);
            SchedulerProxy * pSchedulerProxy = pExecutionResource->GetSchedulerProxy();
            pExecutionResource->DecrementUseCounts();

            // We have to manually redistribute available cores in the case where the DRM thread is not running.
            if ((pSchedulerProxy->GetNumAllocatedCores() < pSchedulerProxy->DesiredHWThreads()) && m_numSchedulers == 1)
            {
                ASSERT(m_dynamicRMWorkerState == Standby);
                if (!DistributeCoresToSurvivingScheduler())
                {
                    // Retry from the background thread
                    signalDRM = true;
                }
            }
        } // end locked region

        if (signalDRM)
        {
            WakeupDynamicRMWorker();
        }
    }

    /// <summary>
    ///     Called in order to notify the resource manager that the given scheduler is shutting down.  This
    ///     will cause the resource manager to immediately reclaim all resources granted to the scheduler.
    /// </summary>
    void ResourceManager::Shutdown(SchedulerProxy *pProxy)
    {
        bool signalDRM = false;

        { // begin locked region
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

            m_schedulers.Remove(pProxy);

            SchedulerNode *pAllocatedNodes = pProxy->GetAllocatedNodes();
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                SchedulerNode *pAllocatedNode = &pAllocatedNodes[i];
                if (pAllocatedNode->m_allocatedCores > 0)
                {
                    for (unsigned int j = 0; j < pAllocatedNode->m_coreCount; ++j)
                    {
                        if (pAllocatedNode->m_pCores[j].m_coreState == ProcessorCore::Allocated)
                        {
                            SchedulerCore *pAllocatedCore = &pAllocatedNode->m_pCores[j];

                            ASSERT(*pAllocatedCore->m_pGlobalUseCountPtr > 0);
                            --(*pAllocatedCore->m_pGlobalUseCountPtr);
                        }
                    }
                }
            }

            if (pProxy->ShouldReceiveNotifications())
            {
                --m_numSchedulersNeedingNotifications;
            }
            if (--m_numSchedulers == 1)
            {
                // Put the dynamic RM worker thread on standby.
                ASSERT(m_dynamicRMWorkerState == LoadBalance);
                m_dynamicRMWorkerState = Standby;
                signalDRM = true;
            }
        } // end locked region

        if (signalDRM)
        {
            // Set the event outside the lock to prevent the high priority thread from having to block immediately upon starting up.
            WakeupDynamicRMWorker();
        }
        pProxy->FinalShutdown();
    }


    /// <summary>
    ///     When the number of schedulers in the RM goes from 2 to 1, this routine is invoked to make sure the remaining scheduler
    ///     has its desired number of cores, before putting the dynamic RM worker on standby. It is also called when there is just
    ///     one scheduler with external subscribed threads that it removes -> there is a chance that this move may allow us to allocate
    ///     more vprocs.
    /// </summary>
    bool ResourceManager::DistributeCoresToSurvivingScheduler()
    {
        // NOTE: This routine must be called while m_lock is held.
        ASSERT(m_numSchedulers <= 1);

        if (!m_schedulers.Empty())
        {
            SchedulerProxy * pSchedulerProxy = m_schedulers.First();

            ASSERT(pSchedulerProxy != NULL);
            ASSERT(pSchedulerProxy->GetNumAllocatedCores() <= pSchedulerProxy->DesiredHWThreads());
            ASSERT(pSchedulerProxy->GetNumBorrowedCores() <= (pSchedulerProxy->DesiredHWThreads() - pSchedulerProxy->MinHWThreads()));

            // Since this is the only scheduler in the RM, we should able to satisfy its MaxConcurrency.
            if (pSchedulerProxy->GetNumAllocatedCores() < pSchedulerProxy->DesiredHWThreads() ||
                pSchedulerProxy->GetNumBorrowedCores() > 0)
            {
                unsigned int suggestedAllocation = pSchedulerProxy->AdjustAllocationIncrease(pSchedulerProxy->DesiredHWThreads());
                unsigned int remainingCores = suggestedAllocation - pSchedulerProxy->GetNumAllocatedCores();
                SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
                unsigned int * pSortedNodeOrder = pSchedulerProxy->GetSortedNodeOrder();

                // Sort the array of nodes in the proxy by number of allocated cores, largest first, if we're allocating
                // to it less cores than the total available. This is so that we pack nodes as tightly as possible.
                bool sortNodes = pSchedulerProxy->DesiredHWThreads() != m_coreCount;

                for (unsigned int i = 0; i < m_nodeCount; ++i)
                {
                    // No need to sort nodes the next time around, if there are no more cores to add.
                    sortNodes &= remainingCores > 0;

                    if (sortNodes)
                    {
                        unsigned int maxAllocationIndex = i;
                        SchedulerNode *pMaxNode = &pAllocatedNodes[pSortedNodeOrder[maxAllocationIndex]];

                        for (unsigned int j = i + 1; j < m_nodeCount; ++j)
                        {
                            SchedulerNode * pNode = &pAllocatedNodes[pSortedNodeOrder[j]];
                            if (pNode->m_allocatedCores > pMaxNode->m_allocatedCores)
                            {
                                maxAllocationIndex = j;
                                pMaxNode = pNode;
                            }
                        }

                        if (i != maxAllocationIndex)
                        {
                            // Swap the index at 'maxAllocationIndex' with the index at 'i'. The next iteration will traverse nodes starting at
                            // m_pSortedNodeOrder[i + i].
                            unsigned int tempIndex = pSortedNodeOrder[i];
                            pSortedNodeOrder[i] = pSortedNodeOrder[maxAllocationIndex];
                            pSortedNodeOrder[maxAllocationIndex] = tempIndex;
                        }
                    }

                    // Assign cores until the desired number of cores is reached. In addition, check if there are
                    // any borrowed cores and switch them to owned.
                    SchedulerNode * pCurrentNode = &pAllocatedNodes[pSortedNodeOrder[i]];
                    for (unsigned int coreIndex = 0; coreIndex < pCurrentNode->m_coreCount; ++coreIndex)
                    {
                        SchedulerCore * pCore = &pCurrentNode->m_pCores[coreIndex];
                        if (pCore->m_coreState == ProcessorCore::Unassigned)
                        {
                            if (remainingCores > 0)
                            {
                                ASSERT(*pCore->m_pGlobalUseCountPtr == 0);

                                ++(*pCore->m_pGlobalUseCountPtr);

                                pSchedulerProxy->AddCore(pCurrentNode, coreIndex, false);
                                --remainingCores;
                            }
                        }
                        else
                        {
                            ASSERT(pCore->m_coreState == ProcessorCore::Allocated);
                            if (pCore->IsBorrowed())
                            {
                                ASSERT(*pCore->m_pGlobalUseCountPtr == 1);
                                pSchedulerProxy->ToggleBorrowedState(pCurrentNode, coreIndex);
                            }
                        }
                    }
                }
            }

            if (pSchedulerProxy->ShouldReceiveNotifications())
            {
                SendResourceNotifications();
            }

#if defined(CONCRT_TRACING)
            if (pSchedulerProxy->GetNumAllocatedCores() != pSchedulerProxy->DesiredHWThreads())
            {
                TRACE(CONCRT_TRACE_DYNAMIC_RM, L"Surviving Scheduler %d: Allocated: %d, Desired: %d",
                    pSchedulerProxy->GetId(), pSchedulerProxy->GetNumAllocatedCores(), pSchedulerProxy->DesiredHWThreads());
            }
#endif
            return (pSchedulerProxy->GetNumAllocatedCores() == pSchedulerProxy->DesiredHWThreads());
        }

        return true;
    }

    /// <summary>
    ///     Denote the doubles in the input array AllocationData[*].m_scaledAllocation by: r[1],..., r[n].
    ///     Split r[j] into b[j] and fract[j] where b[j] is the integral floor of r[j] and fract[j] is the fraction truncated.
    ///     Sort the set { r[j] | j = 1,...,n } from largest fract[j] to smallest.
    ///     For each j = 0, 1, 2,...  if fract[j] > 0, then set b[j] += 1 and pay for the cost of 1-fract[j] by rounding
    ///     fract[j0] -> 0 from the end (j0 = n-1, n-2,...) -- stop before j > j0. b[j] is stored in AllocationData[*].m_allocation.
    ///     totalAllocated is the sum of all AllocationData[*].m_scaledAllocation upon entry, which after the function call is over will
    ///     necessarily be equal to the sum of all AllocationData[*].m_allocation.
    /// </summary>
    void ResourceManager::RoundUpScaledAllocations(AllocationData **ppData, unsigned int count, unsigned int totalAllocated)
    {
        ASSERT(count > 1 && ppData != NULL);
        double epsilon = 1e-07; // epsilon allows forgiveness of reasonable round-off errors.

#if defined(_DEBUG)
        double sumScaledAllocation = 0.0;
        for (unsigned int i = 0; i < count; ++i)
        {
            sumScaledAllocation += ppData[i]->m_scaledAllocation;
        }
        ASSERT(sumScaledAllocation <= totalAllocated + epsilon && sumScaledAllocation >= totalAllocated - epsilon);
#endif

        double fraction = 0.0;

        for (unsigned int i = 0; i < count; ++i)
        {
            ppData[i]->m_allocation = (unsigned int) ppData[i]->m_scaledAllocation;
            ppData[i]->m_scaledAllocation -= ppData[i]->m_allocation;
        }

        // Sort by scaledAllocation, highest first selection sort.
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned int maxIndex = i;
            for (unsigned int j = i + 1; j < count; ++j)
            {
                if (ppData[j]->m_scaledAllocation > ppData[maxIndex]->m_scaledAllocation + epsilon)
                {
                    maxIndex = j;
                }
            }
            if (i != maxIndex)
            {
                AllocationData * pTemp = ppData[i];
                ppData[i] = ppData[maxIndex];
                ppData[maxIndex] = pTemp;
            }
        }

        // Round up those with the largest truncation, stealing the fraction from those with the least.
        for (unsigned int i = 0, j = count - 1; i < count; ++i)
        {
            while (fraction > epsilon)
            {
                if (ppData[j]->m_scaledAllocation > epsilon)
                {
                    do
                    {
                        ASSERT(j < count);
                        fraction -= ppData[j]->m_scaledAllocation;
                        ppData[j]->m_scaledAllocation = 0.0;
                        --j;
                    }
                    while (fraction > epsilon);
                    ASSERT(i <= j+1);
                }
                else
                {
                    --j;
                    ASSERT(i <= j && j < count);
                }
            }

            if (i <= j)
            {
                ASSERT(j < count);
                if (ppData[i]->m_scaledAllocation > epsilon)
                {
                    fraction += (1.0 - ppData[i]->m_scaledAllocation);
                    ppData[i]->m_scaledAllocation = 0.0;
                    ppData[i]->m_allocation += 1;
                }
            }
            else
                break;
        }

        ASSERT(fraction <= epsilon && fraction >= -epsilon);

#if defined(_DEBUG)
        unsigned int sumAllocation = 0;
        for (unsigned int i = 0; i < count; ++i)
        {
            sumAllocation += ppData[i]->m_allocation;
        }
        ASSERT(sumAllocation == totalAllocated);
#endif

        // Sort by index, lowest first selection sort.
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned int minIndex = i;
            for (unsigned int j = i + 1; j < count; ++j)
            {
                if (ppData[j]->m_index < ppData[minIndex]->m_index)
                {
                    minIndex = j;
                }
            }
            if (i != minIndex)
            {
                AllocationData * pTemp = ppData[i];
                ppData[i] = ppData[minIndex];
                ppData[minIndex] = pTemp;
            }
        }
    }

    /// <summary>
    ///     Tries to redistribute cores allocated to all schedulers proportional to each schedulers value for 'DesiredHardwareThreads',
    ///     and reserve any freed cores for the new scheduler.
    /// </summary>
    unsigned int ResourceManager::RedistributeCoresAmongAll(SchedulerProxy* pSchedulerProxy, unsigned int allocated, unsigned int minimum, unsigned int desired)
    {
        // The argument 'allocated' is the sum of cores that have been previously allocated, and cores that were reserved during this allocation attempt.
        unsigned int reservation = 0;
        ASSERT(m_numSchedulers > 0 && m_ppProxyData[0]->m_pProxy == pSchedulerProxy);

        // Try to proportionally allocate cores to all schedulers w/o oversubscription. The proportions used will be
        // 'desired' for each scheduler, except that no existing scheduler will be forced to increase the current allocation.
        if (m_numSchedulers > 1)
        {
            unsigned int totalMinimum = minimum;
            unsigned int totalAllocated = allocated;
            unsigned int numSchedulers = 1; // includes the current scheduler

            // Let totalAllocated be the number of cores we have allocated to the new scheduler so far, plus the number of 'owned' cores
            // allocated to all existing schedulers.  Let s1,...sn be the currently allocated schedulers with 'desired' des[1],
            // ...,des[n] and 'minimum' min[1],...,min[n]. The new scheduler requesting an allocation is s0 with desired des[0] and
            // minimum min[0].
            for (unsigned int i = 1; i < m_numSchedulers; ++i)
            {
                SchedulerProxy * pProxy =  m_ppProxyData[i]->m_pProxy;
                ASSERT(pSchedulerProxy != pProxy);
                // Only take into account existing schedulers that have > Min. We work with the number of 'owned' cores here instead of
                // the number of 'allocated' cores (which includes borrowed cores). The borrowed cores should already have been released,
                // but they are accounted for in the total allocated count, until the release is confirmed.
                if (pProxy->GetNumOwnedCores() > pProxy->MinHWThreads())
                {
                    ++numSchedulers;

                    totalMinimum += pProxy->MinHWThreads();
                    totalAllocated += pProxy->GetNumOwnedCores();
                }
            }

            if (numSchedulers > 1 && totalMinimum <= totalAllocated)
            {
                // We have found schedulers with cores greater than min. Moreover, the sum of all cores already allocated to
                // existing schedulers can at least satisfy all mins (including the min requirement of the current scheduler).

                double totalDesired = 0.0;
                double scaling = 0.0;
#if defined(_DEBUG)
                double epsilon = 1e-07; // epsilon allows forgiveness of reasonable round-off errors
#endif
                // For the purpose of rounding up scaled allocation, we need an array of pointers to AllocationData.
                StaticAllocationData ** ppProxies = _concrt_new StaticAllocationData* [numSchedulers];

                ppProxies[0] = static_cast<StaticAllocationData *>(m_ppProxyData[0]);
                ASSERT(ppProxies[0]->m_index == 0);
                // 'desired' may not be the same as DesiredHWThreads, but it is the number desired for this allocation attempt.
                ppProxies[0]->m_adjustedDesired = desired;
                totalDesired += ppProxies[0]->m_adjustedDesired;

                unsigned int index = 1;
                for (unsigned int i = 1; i < m_numSchedulers; ++i)
                {
                    SchedulerProxy * pProxy =  m_ppProxyData[i]->m_pProxy;
                    ASSERT(pSchedulerProxy != pProxy);

                    if (pProxy->GetNumOwnedCores() > pProxy->MinHWThreads())
                    {
                        ppProxies[index] = pProxy->GetStaticAllocationData();
                        ASSERT(ppProxies[index]->m_adjustedDesired == pProxy->DesiredHWThreads());
                        totalDesired += ppProxies[index]->m_adjustedDesired;
                        ++index;
                    }
                }

                ASSERT(index == numSchedulers);

                while (true)
                {
                    // We're trying to pick a scaling factor r such that r * (Sum { des[j] | j = 0,...,n }) = totalAllocated.
                    scaling = totalAllocated/totalDesired;

                    // Multiply the scaling factor by each schedulers 'desired'.
                    for (index = 0; index < numSchedulers; ++index)
                    {
                        ppProxies[index]->m_scaledAllocation = ppProxies[index]->m_adjustedDesired * scaling;
                    }

                    // Convert the floating point scaled allocations into integer allocations, using the algorithm below.
                    // Denote the n+1 scaled allocations by:
                    //      r[0],..., r[n]
                    // Split r[j] into b[j] and fract[j] where b[j] is the integral floor of r[j] and fract[j] is the fraction truncated.
                    //
                    // Sort the set { r[j] | j = 0,...,n } from largest fract[j] to smallest.
                    //
                    // For each j = 0, 1, 2,...  if fract[j] > 0, then set b[j] += 1 and pay for the cost of 1-fract[j] by
                    // rounding fract[j0] -> 0 from the end (j0=n, n-1, n-2,...) -- stop before j > j0.
                    //
                    // The new allocations for schedulers s0,...,sn and s is b[0],...,b[n] -- where the original order is preserved.
                    //
                    //      { 1.6, 1.5, 1.7, 1.3, 1.8, 1.2, 1.3, 1.1, 1.4, 1.2, 1.9 }
                    //          --> { 1.9, 1.8, 1.7, 1.6, 1.5, 1.4, 1.3, 1.3, 1.2, 1.2, 1.1 } // sort
                    //          --> { 2, 1.8, 1.7, 1.6, 1.5, 1.4, 1.3, 1.3, 1.2, 1.2, 1 }
                    //          --> { 2, 2, 1.7, 1.6, 1.5, 1.4, 1.3, 1.3, 1.2, 1, 1 }
                    //          --> { 2, 2, 2, 1.6, 1.5, 1.4, 1.3, 1.2, 1, 1, 1 }
                    //          --> { 2, 2, 2, 2, 1.5, 1.4, 1.1, 1, 1, 1, 1 }
                    //          --> { 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1 }
                    //          --> { 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2 } // put back in original order
                    //
                    // Since all changes are properly accounted for, the sum will not change.
                    //
                    RoundUpScaledAllocations((AllocationData **)ppProxies, numSchedulers, totalAllocated);

                    bool fReCalculate = false;
                    for (unsigned int i = 1; i < numSchedulers; ++i)
                    {
                        // Keep recursing until previous allocations do not increase (excluding the current scheduler).
                        SchedulerProxy *pProxy = ppProxies[i]->m_pProxy;
                        if (ppProxies[i]->m_allocation > pProxy->GetNumOwnedCores())
                        {
                            double modifier = pProxy->GetNumOwnedCores()/(double)ppProxies[i]->m_allocation;

                            // Reduce adjustedDesired by multiplying it with 'modifier', to try to bias allocation to the original size or less.
                            totalDesired -= ppProxies[i]->m_adjustedDesired * (1.0 - modifier);
                            ppProxies[i]->m_adjustedDesired = modifier * ppProxies[i]->m_adjustedDesired;

                            fReCalculate = true;
                        }
                    }

                    if (fReCalculate)
                    {
#if defined(_DEBUG)
                        double sumDesired = 0.0;
                        for (unsigned int i = 0; i < numSchedulers; ++i)
                        {
                            sumDesired += ppProxies[i]->m_adjustedDesired;
                        }
                        ASSERT(totalDesired <= sumDesired + epsilon && totalDesired >= sumDesired - epsilon);
#endif
                        continue;
                    }

                    for (unsigned int i = 0; i < numSchedulers; ++i)
                    {
                        // Keep recursing until all allocations are no greater than desired (including the current scheduler).
                        SchedulerProxy *pProxy = ppProxies[i]->m_pProxy;
                        if (ppProxies[i]->m_allocation > pProxy->DesiredHWThreads())
                        {
                            double modifier = pProxy->DesiredHWThreads()/(double)ppProxies[i]->m_allocation;

                            // Reduce adjustedDesired by multiplying with it 'modifier', to try to bias allocation to desired or less.
                            totalDesired -= ppProxies[i]->m_adjustedDesired * (1.0 - modifier);
                            ppProxies[i]->m_adjustedDesired = modifier*ppProxies[i]->m_adjustedDesired;

                            fReCalculate = true;
                        }
                    }

                    if (fReCalculate)
                    {
#if defined(_DEBUG)
                        double sumDesired = 0.0;
                        for (unsigned int i = 0; i < numSchedulers; ++i)
                            sumDesired += ppProxies[i]->m_adjustedDesired;
                        ASSERT(totalDesired <= sumDesired + epsilon && totalDesired >= sumDesired - epsilon);
#endif
                        continue;
                    }

                    for (unsigned int i = 0; i < numSchedulers; ++i)
                    {
                        // Keep recursing until all allocations are at least minimum (including the current scheduler).
                        SchedulerProxy *pProxy = ppProxies[i]->m_pProxy;
                        if (pProxy->MinHWThreads() > ppProxies[i]->m_allocation)
                        {
                            double newDesired = pProxy->MinHWThreads()/scaling;

                            // Bias desired to get allocation closer to min.
                            totalDesired += newDesired - ppProxies[i]->m_adjustedDesired;
                            ppProxies[i]->m_adjustedDesired = newDesired;

                            fReCalculate = true;
                        }
                    }

                    if (fReCalculate)
                    {
#if defined(_DEBUG)
                        double sumDesired = 0.0;
                        for (unsigned int i = 0; i < numSchedulers; ++i)
                            sumDesired += ppProxies[i]->m_adjustedDesired;
                        ASSERT(totalDesired <= sumDesired + epsilon && totalDesired >= sumDesired - epsilon);
#endif
                        continue;
                    }
#if defined(_DEBUG)
                    for (unsigned int i = 1; i < numSchedulers; ++i)
                    {
                        ASSERT(ppProxies[i]->m_pProxy->MinHWThreads() <= ppProxies[i]->m_allocation && ppProxies[i]->m_allocation <= ppProxies[i]->m_pProxy->GetNumOwnedCores());
                    }
                    ASSERT(ppProxies[0]->m_pProxy->MinHWThreads() <= ppProxies[0]->m_allocation);
                    ASSERT(ppProxies[0]->m_pProxy == pSchedulerProxy);
#endif
                    break;
                } // end of while (true)

                if (ppProxies[0]->m_allocation > allocated)
                {
                    for (unsigned int i = 1; i < numSchedulers; ++i)
                    {
                        unsigned int reduceBy = ppProxies[i]->m_pProxy->GetNumOwnedCores() - ppProxies[i]->m_allocation;
                        if (reduceBy > 0)
                        {
                            ReleaseSchedulerResources(pSchedulerProxy, ppProxies[i]->m_pProxy, reduceBy);
                        }
                    }

                    // Reserve out of the cores we just freed.
                    reservation = ReserveCores(pSchedulerProxy, ppProxies[0]->m_allocation - allocated, 0);
                }

                delete [] ppProxies;
            }
        }
        return reservation;
    }

    /// <summary>
    ///     Instructs existing schedulers to release cores. Then tries to reserve available cores for the new scheduler.
    ///     The parameter numberToFree can be one of the two special values:
    ///         ReleaseCoresDownToMin: used to release cores until they are at min, or
    ////        ReleaseOnlyBorrowedCores: only relase borrowed cores.
    /// </summary>
    unsigned int ResourceManager::ReleaseCoresOnExistingSchedulers(SchedulerProxy * pNewProxy, unsigned int request, unsigned int numberToFree)
    {
        ASSERT(m_numSchedulers > 0 && m_ppProxyData[0]->m_pProxy == pNewProxy);
        ASSERT(numberToFree == ReleaseCoresDownToMin || numberToFree == ReleaseOnlyBorrowedCores);

        // Ask previously allocated schedulers to release surplus cores, until either the request is satisfied, or we're out of schedulers.
        bool releasedCores = false;

        for (unsigned int index = 1; index < m_numSchedulers; ++index)
        {
            ASSERT(pNewProxy != m_ppProxyData[index]->m_pProxy);
            if (ReleaseSchedulerResources(pNewProxy, m_ppProxyData[index]->m_pProxy, numberToFree))
            {
                releasedCores = true;
            }
        }
        unsigned int reservation;
        if (releasedCores)
        {
            reservation = ReserveCores(pNewProxy, request, 0);
        }
        else
        {
            reservation = 0;
        }
        return reservation;
    }

    /// <summary>
    ///     Reserves cores for the new scheduler at higher use counts - this is use only to satisify MinHWThreads.
    /// </summary>
    unsigned int ResourceManager::ReserveAtHigherUseCounts(SchedulerProxy* pSchedulerProxy, unsigned int request)
    {
        unsigned int reuseCount = 0;
        unsigned int reservation = 0;

        while (reservation < request)
        {
            reservation += ReserveCores(pSchedulerProxy, request - reservation, ++reuseCount);
        }

        return reservation;
    }

    /// <summary>
    ///     The main allocation routine that allocates cores to a scheduler proxy.
    /// </summary>
    ExecutionResource * ResourceManager::PerformAllocation(SchedulerProxy *pSchedulerProxy, bool fInitialAllocation, bool fSubscribeCurrentThread)
    {
        ASSERT(pSchedulerProxy != NULL && m_pGlobalNodes != NULL);
        ASSERT(fInitialAllocation || pSchedulerProxy->GetAllocatedNodes());
        ASSERT(fInitialAllocation || fSubscribeCurrentThread);
        ASSERT(!fInitialAllocation || pSchedulerProxy->GetNumAllocatedCores() == 0);

        if (fInitialAllocation)
        {
            pSchedulerProxy->SetAllocatedNodes(CreateAllocatedNodeData());
        }

        // Calculate the number of cores to attempt to allocate to the scheduler proxy. Note, that the only incremental allocation request
        // currently supported (fInitialAllocation == false), is the request for a single thread subscription.
        unsigned int minimum = 0;
        unsigned int desired = 0;
        unsigned int allocated = pSchedulerProxy->GetNumAllocatedCores();
        unsigned int reserved = 0;

        unsigned int request = 0;
        unsigned int minRequest = 0; // The request we have to make to satisfy the minimum for the scheduler proxy.
        if (fSubscribeCurrentThread)
        {
            minimum = pSchedulerProxy->ComputeMinHWThreadsWithExternalThread();
            desired = pSchedulerProxy->ComputeDesiredHWThreadsWithExternalThread();
            if (fInitialAllocation)
            {
                request = desired;
                minRequest = minimum;
            }
            else
            {
                // This is a subsequent allocation. Currently only external thread subscriptions are supported via this path.
                unsigned int currentMinimum = pSchedulerProxy->MinHWThreads();
                ASSERT(currentMinimum <= m_coreCount && currentMinimum <= allocated);
                ASSERT(currentMinimum < m_coreCount || (desired == m_coreCount && minimum == m_coreCount && allocated == m_coreCount));

                // We will look for 1 more core for the scheduler unless all cores are already allocated to the scheduler.
                // If we cannot find any cores at a use count of 0, the subscribed thread will either oversusbcribe an existing core
                // or replace an existing core allocated to the scheduler if it can afford to give up vprocs.
                request = (currentMinimum < m_coreCount) ? 1 : 0;
                // Note that 'minimum' takes into account the external thread we're currently allocating a core for.
                minRequest = (allocated < minimum) ? 1 : 0;
            }
        }
        else
        {
            ASSERT(allocated == 0);
            minimum = minRequest = pSchedulerProxy->MinHWThreads();
            desired = request = pSchedulerProxy->DesiredHWThreads();
        }
        ASSERT(request >= minRequest);

        bool coresStolen = false;
        if (reserved < request)
        {
            // Capture data needed for static allocation for all existing schedulers.
            SetupStaticAllocationData(pSchedulerProxy, fSubscribeCurrentThread);

            // Handle a subset of idle and borrowed cores up front.
            PreProcessStaticAllocationData();

            // Try for an initial reservation of cores with a useCount = 0 (viz, no sharing)
            reserved = ReserveCores(pSchedulerProxy, request, 0);

            if (reserved < request && (fInitialAllocation || minRequest > 0))
            {
                // For subsequent requests for thread subscription, we only proceed if we need a core to satisfy the new minimum.
                // If not, we should be able to use an already allocated core to satisfy the new request, by removing the virtual
                // processors from that core.

                // At this point we will start stealing cores from other schedulers, so we set a flag to true to remember to
                // commit any changes that are made to the allocations of these schedulers.
                coresStolen = true;

                // Have schedulers give up all borrowed cores first. These are cores that were temporarily borrowed and
                // assigned to a scheduler by dynamic RM when it was noticed that other schedulers that had that core
                // were not using it. While this does not necessarily free up cores at 0 use count, it's possible that
                // a combination of this and the attempt to redistribute cores, yields some free cores.
                reserved += ReleaseCoresOnExistingSchedulers(pSchedulerProxy, request - reserved, ReleaseOnlyBorrowedCores);

                if (reserved < request)
                {
                    // Next try to divide cores among schedulers, proportional to each scheduler's Desired value.
                    reserved += RedistributeCoresAmongAll(pSchedulerProxy, allocated + reserved, minimum, allocated + request);

                    if (reserved < minRequest)
                    {
                        // Finally force schedulers to reduce down to min, but this time only to satisfy the minimum requirement
                        // for the requesting scheduler.
                        reserved += ReleaseCoresOnExistingSchedulers(pSchedulerProxy, minRequest - reserved, ReleaseCoresDownToMin);

                        // If we still haven't satisfied the minimum request we have no choice but to share cores with other schedulers.
                        if (reserved < minRequest)
                        {
                            reserved += ReserveAtHigherUseCounts(pSchedulerProxy, minRequest - reserved);
                        }
                    }
                }

                ASSERT(reserved >= minRequest && reserved + allocated >= minimum);
            }

            // Revert the changes made to global state while setting up data for allocation.
            ResetGlobalAllocationData();
        }

        ExecutionResource * pExecutionResource = pSchedulerProxy->GrantAllocation(reserved, fInitialAllocation, fSubscribeCurrentThread);

        if (coresStolen)
        {
            // We have potentially stolen cores from other schedulers. CommitStolenCores will either inform
            // previously allocated schedulers that certain cores have been taken away (if those cores were used to
            // satisfy the allocation for the current scheduler), or will reclaim them for the schedulers they were
            // stolen from.
            // This needs to be done after the allocation is granted, since the subscription level for the newly allocated cores
            // could've increased, and we take that into account during the restore.
            CommitStolenCores(pSchedulerProxy);
        }

#if defined(_DEBUG)
        // Ensure that allocations are between min and max for all schedulers after a new scheduler receives its initial allocation.
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            SchedulerProxy * pSchedulerProxy = m_ppProxyData[index]->m_pProxy;
            ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());
            ASSERT(pSchedulerProxy->GetNumAllocatedCores() <= pSchedulerProxy->DesiredHWThreads());
        }
#endif

        return pExecutionResource;
    }

    /// <summary>
    ///     Worker routine that does actual core reservation, using the supplied use count. It tries to
    ///     pack reserved cores onto nodes by preferring nodes where more free cores are available.
    /// </summary>
    unsigned int ResourceManager::ReserveCores(SchedulerProxy * pSchedulerProxy, unsigned int request, unsigned int useCount)
    {
        unsigned int currentNodeIndex = (unsigned int) -1;

        //
        // If this is an external thread allocation, we need to look at which node the external thread is already
        // affinitized to in order to bias the reservation to that node.
        //
        StaticAllocationData * pStaticData = pSchedulerProxy->GetStaticAllocationData();
        if (pStaticData->m_fNeedsExternalThreadAllocation)
        {
            currentNodeIndex = GetCurrentNodeAndCore(NULL);
        }

        SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
        unsigned int * pSortedNodeOrder = pSchedulerProxy->GetSortedNodeOrder();

        //
        // GlobalCore::m_useCount which is the same as *SchedulerCore::m_pGlobalUseCountPtr is the number of schedulers utilizing
        // this core. The reservation routine works by looking at cores with m_useCount=0, grabs all it can, then looks at m_useCount=1,
        // then m_useCount=2, etc. At a given use count, say m_useCount = count, the number of cores available for a possible allocation
        // in a node is SchedulerNode::m_availableCores. The number of cores reserved at m_useCount < count, is SchedulerNode::m_reservedCores.
        // Previously allocated cores are represented by SchedulerNode::m_allocatedCores (these cores already have virtual processors or
        // execution resources associated with them).
        //
        bool fAvailableCores = false;

        ASSERT(request > 0);

        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode *pAllocatedNode = &pAllocatedNodes[nodeIndex];
            for (unsigned int coreIndex = 0; coreIndex < pAllocatedNode->m_coreCount; ++coreIndex)
            {
                SchedulerCore * pAllocatedCore = &(pAllocatedNode->m_pCores[coreIndex]);
                if (pAllocatedCore->m_coreState == ProcessorCore::Unassigned)
                {
                    if (useCount == *pAllocatedCore->m_pGlobalUseCountPtr)
                    {
                        fAvailableCores = true;

                        // Temporarily mark the core as available, for a possible reservation below.
                        pAllocatedCore->m_coreState = ProcessorCore::Available;

                        // m_availableCores is the number of cores that satisfy request at the current useCount.
                        ++pAllocatedNode->m_availableCores;
                    }
                }
                else
                {
                    ASSERT(pAllocatedCore->m_coreState == ProcessorCore::Allocated || pAllocatedCore->m_coreState == ProcessorCore::Reserved);
                }
            }
        }

        unsigned int reserved = 0;

        if (fAvailableCores)
        {
            // Now that we've found available cores at the current use count, reserve upto 'request' of them for the new scheduler.
            // Order the search for such cores by first looking at nodes that will have the most cores allocated from it.
            // Even after we satisfy 'request', walk through the remaining nodes, marking the cores marked Available as Unassigned.

            // As we go through the reservation, we want to satisfy the request by picking nodes with the largest possible allocation
            // first. However we still want to maintain the order of nodes in the array we give the scheduler proxy,
            // such that a node with a nodeId 'm', is at location 'm' in the allocated nodes array.
            //
            // What we do here is sort the array without moving items around. We store information in a different array of
            // indices where the indices are sorted such that they point to nodes with decreasing allocation when we're done.

            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                unsigned int maxAllocationIndex = i;

                // We use the array of sorted indices to 'sort' nodes, instead of swapping the nodes themselves.
                SchedulerNode *pMaxNode = &pAllocatedNodes[pSortedNodeOrder[maxAllocationIndex]];

                unsigned int remainingRequest = request - reserved;

                // If we've satisfied the request on previous iterations of the loop, don't bother comparing this node
                // with the nodes further down.
                if (remainingRequest > 0)
                {
                    // Lower the availability for reasons described below.
                    if (pMaxNode->m_availableCores > remainingRequest)
                    {
                        pMaxNode->m_availableCores = remainingRequest;
                    }

                    for (unsigned int j = i + 1; j < m_nodeCount; ++j)
                    {
                        SchedulerNode *pNode = &pAllocatedNodes[pSortedNodeOrder[j]];

                        if (pNode->m_availableCores > remainingRequest)
                        {
                            pNode->m_availableCores = remainingRequest;
                        }

                        // Q. Why did we just lower the number of available cores on this node down to request, if it was greater?
                        // A. The primary reason is to ensure that we pack nodes as tightly as possible while still taking into account
                        // use counts. Take a look at the example below. Let's say the system has 2 nodes 4 cores/node for
                        // a total of 8 cores. The scheduler has requested 6 cores. We were able to satisfy 5 of these six at a lower
                        // use count of 1, 3 belong to NodeA and 2 belong to NodeB. The remaining three cores have the same use count
                        // of 2. At this point NodeA.m_reservedCores = 3 and NodeA.m_availableCores = 1 whereas NodeB.m_reservedCores = 2 and
                        // NodeB.m_availableCores = 2. It would be beneficial for us to collocate the 6th core with the 3 on node A and
                        // leave the 2 cores (with use count 2) on node B unallocated for this scheduler.
                        //
                        //           ---------------              ---------------
                        //          |    -     -    |            |    -     -    |
                        //          |   |1|   |1|   |            |   |1|   |1|   |
                        //          |    -     -    |            |    -     -    |
                        //          |               |            |               |
                        //          |    -          |            |               |
                        //          |   |1|    2    |            |    2     2    |
                        //          |    -          |            |               |
                        //           ---------------              ---------------
                        //                Node A                       Node B
                        //
                        // Lowering the number of available cores on NodeB to the value of request allows us to preferentially allocate as many cores as
                        // possible on the same node (NodeA) to take advantage of, for instance, a node common L3 and use the same node TLB.
                        // Moreover, with the NUMA architecture of all current nodes, it is a much higher priority to collocate scheduler
                        // core allocation on a minimum number of nodes and pack each a full as possible.

                        if (pMaxNode->m_availableCores + pMaxNode->m_reservedCores + pMaxNode->m_allocatedCores <
                                pNode->m_availableCores + pNode->m_reservedCores + pNode->m_allocatedCores)
                        {
                            // m_reservedCores is the number of cores reserved on previous passes for a lower useCount, and m_allocatedCores is
                            // the number allocated on a previous allocation attempt. So this will set pMaxNode to be the one where the greatest
                            // number of cores are or will be allocated from.
                            maxAllocationIndex = j;
                            pMaxNode = &pAllocatedNodes[pSortedNodeOrder[maxAllocationIndex]];
                        }
                        else if (pMaxNode->m_availableCores + pMaxNode->m_reservedCores + pMaxNode->m_allocatedCores ==
                                    pNode->m_availableCores + pNode->m_reservedCores + pNode->m_allocatedCores)
                        {
                            // If all things are equal from the core-packing perspective, bias towards this node if we need an external
                            // thread allocation and this node is the one the external thread is affinitized to.
                            if (pStaticData->m_fNeedsExternalThreadAllocation && pSortedNodeOrder[j] == currentNodeIndex)
                            {
                                maxAllocationIndex = j;
                                pMaxNode = &pAllocatedNodes[pSortedNodeOrder[maxAllocationIndex]];
                            }
                        }
                    }
                } // end of if (remainingRequest > 0)

                ASSERT(pMaxNode->m_availableCores <= INT_MAX);

                if (pMaxNode->m_availableCores > 0)
                {
                    for (unsigned int k = 0; k < pMaxNode->m_coreCount; ++k)
                    {
                        SchedulerCore *pCore = &pMaxNode->m_pCores[k];
                        if (pCore->m_coreState == ProcessorCore::Available)
                        {
                            if (reserved < request)
                            {
                                // Reserve this core for the scheduler proxy.
                                pCore->m_coreState = ProcessorCore::Reserved;
                                ++(*pCore->m_pGlobalUseCountPtr);
                                ++pMaxNode->m_reservedCores;

                                // If we needed an external thread allocation and the allocation to this node satisfies that
                                // external allocation, clear the flag so no more biasing will be needed.
                                if (pStaticData->m_fNeedsExternalThreadAllocation && pSortedNodeOrder[maxAllocationIndex] == currentNodeIndex)
                                {
                                     pStaticData->m_fNeedsExternalThreadAllocation = false;
                                }
                                ++reserved;
                            }
                            else
                            {
                                // This is an 'extra' available core. Set its state back to Unassigned.
                                pCore->m_coreState = ProcessorCore::Unassigned;
                            }
                        }
                    }
                    pMaxNode->m_availableCores = 0;
                }

                if (i != maxAllocationIndex)
                {
                    // Swap the index at 'maxAllocationIndex' with the index at 'i'. The next iteration will traverse nodes starting at
                    // m_pSortedNodeOrder[i + i].
                    unsigned int tempIndex = pSortedNodeOrder[i];
                    pSortedNodeOrder[i] = pSortedNodeOrder[maxAllocationIndex];
                    pSortedNodeOrder[maxAllocationIndex] = tempIndex;
                }
            }
        }

        return reserved;
    }
#pragma warning(disable:26017)  // bogus overflow warning
#pragma warning(disable:26011)  // bogus overflow warning

    /// <summary>
    ///     Instruct a scheduler proxy to free up a fixed number of resources. This is only a temporary release of resources. The
    ///     use count on the global core is decremented and the scheduler proxy remembers the core as temporarily released. At a later
    ///     point, the release is either confirmed or rolled back, depending on whether the released core was used to satisfy the
    ///     receiving scheduler's allocation.
    /// </summary>
    /// <param name="pReceivingProxy">
    ///     The scheduler proxy for which the cores are being stolen - this is the proxy that is being currently allocated to.
    /// </param>
    /// <param name="pGivingProxy">
    ///     The scheduler proxy that needs to free up resources.
    /// </param>
    /// <param name="numberToFree">
    ///     The number of resources to free. This parameter can have a couple of special values:
    ///         ReleaseCoresDownToMin - scheduler should release all cores above its minimum. Preference is giving to releasing borrowed cores.
    ///         ReleaseOnlyBorrowedCores - scheduler should release all borrowed cores.
    ///     If the parameter is not a special value, a call should have previously been made for this scheduler with the value ReleaseOnlyBorrowedCores.
    ///     i.e., the scheduler should not have any borrowed cores to release.
    /// </param>
    bool ResourceManager::ReleaseSchedulerResources(SchedulerProxy * pReceivingProxy, SchedulerProxy *pGivingProxy, unsigned int numberToFree)
    {
        ASSERT(pReceivingProxy != NULL && pGivingProxy != NULL);

        unsigned int numBorrowedCores = 0;
        unsigned int numOwnedCores = 0;
        StaticAllocationData * pStaticData = pGivingProxy->GetStaticAllocationData();

        if (numberToFree == ReleaseOnlyBorrowedCores)
        {
            // We should only get one request to release borrowed cores - there should be no cores already stolen at this time.
            ASSERT(pStaticData->m_numCoresStolen == 0);

            numberToFree = numBorrowedCores = pGivingProxy->GetNumBorrowedCores();
        }
        else if (numberToFree == ReleaseCoresDownToMin)
        {
            ASSERT(pGivingProxy->GetNumBorrowedCores() == 0 || pStaticData->m_numCoresStolen >= pGivingProxy->GetNumBorrowedCores());
            ASSERT(pGivingProxy->GetNumOwnedCores() >= pGivingProxy->MinHWThreads());

            // Number to stolen includes all borrowed cores, if any, and possibly some owned cores.
            numberToFree = (pGivingProxy->GetNumOwnedCores() - pGivingProxy->MinHWThreads()) -
                           (pStaticData->m_numCoresStolen - pGivingProxy->GetNumBorrowedCores());
            numBorrowedCores = 0;
        }
        else
        {
            // If we're asked to release a specific number of cores, borrowed cores should already have been released, and we should
            // not encounter any borrowed cores during our search.
            ASSERT(pStaticData->m_numCoresStolen == pGivingProxy->GetNumBorrowedCores());
            ASSERT(pGivingProxy->GetNumOwnedCores() >= pGivingProxy->MinHWThreads());
            ASSERT(numberToFree > 0 && numberToFree <= pGivingProxy->GetNumOwnedCores() - pGivingProxy->MinHWThreads());
            numBorrowedCores = 0;
        }

        // We are only allowed to free numOwnedCores owned cores.
        ASSERT(numberToFree >= numBorrowedCores && numberToFree <= INT_MAX);
        numOwnedCores = numberToFree - numBorrowedCores;

        if (numberToFree > 0)
        {
            SchedulerNode *pGivingNodes = pGivingProxy->GetAllocatedNodes();
            SchedulerNode *pReceivingNodes = pReceivingProxy->GetAllocatedNodes();
            unsigned int * pReceivingNodeOrder = pReceivingProxy->GetSortedNodeOrder();

            // Walk through the sorted indices array, and try to release cores in nodes that appear earlier in the array. That way
            // we increase the possibility of giving up cores on a node the new scheduler already has some cores on.
            for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
            {
                SchedulerNode *pReceivingNode = &pReceivingNodes[pReceivingNodeOrder[nodeIndex]];

                // Even if all the cores in this node are already allocated or reserved for the new scheduler, we need to traverse the releasing
                // scheduler proxy's node, to accurately maintain counts.
                SchedulerNode *pGivingNode = &pGivingNodes[pReceivingNodeOrder[nodeIndex]];

                ASSERT(pGivingNode->m_id == pReceivingNode->m_id);
                ASSERT(pGivingNode->m_coreCount == pReceivingNode->m_coreCount);

                // Only traverse the cores if the giving scheduler proxy was allocated cores from this node.
                if (pGivingNode->GetNumMigratableCores() > 0)
                {
                    for (unsigned int coreIndex = 0; coreIndex < pGivingNode->m_coreCount; ++coreIndex)
                    {
                        SchedulerCore * pAllocatedCore = &pGivingNode->m_pCores[coreIndex];
                        if ((pAllocatedCore->m_coreState == ProcessorCore::Allocated) && !pAllocatedCore->IsFixed())
                        {
                            ASSERT(numBorrowedCores > 0 || !pAllocatedCore->IsBorrowed());

                            // We may have to skip over some owned cores so that we can release the required number of borrowed cores.
                            if (pAllocatedCore->IsBorrowed() || numOwnedCores > 0)
                            {
                                ASSERT(!pAllocatedCore->IsBorrowed() || *pAllocatedCore->m_pGlobalUseCountPtr > 1);

                                pAllocatedCore->m_coreState = ProcessorCore::Stolen;
                                ++pStaticData->m_numCoresStolen;

                                // Only the global use count is decremented here. The number allocated to this proxy is updated once the
                                // allocation is finished (in CommitStolenCores), if this core was allocated to the new scheduler.

                                --(*pAllocatedCore->m_pGlobalUseCountPtr);
                                if (!pAllocatedCore->IsBorrowed())
                                {
                                    ASSERT(numOwnedCores > 0);
                                    --numOwnedCores;
                                }

                                ASSERT(numberToFree > 0 && numberToFree <= INT_MAX);
                                if (--numberToFree == 0)
                                {
                                    ValidateStaticSchedulerState(pGivingProxy);
                                    return true;
                                }
                            }
                        }
                    }
                }
            }

            ASSERT(numberToFree == 0);
        }
        ValidateStaticSchedulerState(pGivingProxy);
        // The scheduler proxy does not have any cores available to free.
        return false;
    }

    /// <summary>
    ///     Called to claim back any previously released cores that were not allocated to a different scheduler. If released
    ///     cores were allocated (stolen), the proxy needs to notify its scheduler to give up the related virtual processor
    ///     roots.
    /// <summary>
    void ResourceManager::CommitStolenCores(SchedulerProxy * pNewSchedulerProxy)
    {
        ASSERT(pNewSchedulerProxy == m_ppProxyData[0]->m_pProxy);
        SchedulerNode * pNewNodes = pNewSchedulerProxy->GetAllocatedNodes();

        // Go through one core at a time, and check all schedulers.
        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode * pNewNode = &pNewNodes[nodeIndex];
            for (unsigned int coreIndex = 0; coreIndex < pNewNode->m_coreCount; ++coreIndex)
            {
                SchedulerCore * pNewCore = &pNewNode->m_pCores[coreIndex];
                unsigned int fAlreadyBorrowed = false;
                for (unsigned int index = 1; index < m_numSchedulers; ++index)
                {
                    SchedulerProxy * pSchedulerProxy = m_ppProxyData[index]->m_pProxy;
                    StaticAllocationData * pStaticData = pSchedulerProxy->GetStaticAllocationData();
                    if (pStaticData->m_numCoresStolen > 0)
                    {
                        SchedulerNode * pAllocatedNodes = m_ppProxyData[index]->m_pProxy->GetAllocatedNodes();
                        SchedulerNode * pAllocatedNode = &pAllocatedNodes[nodeIndex];
                        SchedulerCore * pAllocatedCore = &pAllocatedNode->m_pCores[coreIndex];
                        if (pAllocatedCore->m_coreState == ProcessorCore::Stolen)
                        {
                            if (pNewCore->m_coreState == ProcessorCore::Allocated)
                            {
                                // The core was allocated to the new scheduler - we need to decide what to do with
                                // the stolen core on the original scheduler.
                                if (pNewCore->m_subscriptionLevel == 0)
                                {
                                    // The new scheduler is not using the core yet. Check if the original scheduler
                                    // is using the core.
                                    if (pAllocatedCore->IsIdle() || fAlreadyBorrowed)
                                    {
                                        // Either the original scheduler is not using the core, or if it is, we lent it to
                                        // a different scheduler earlier in the iteration. We cannot have two schedulers borrowing
                                        // a core at the same time, so we have to remove it here.
                                        pSchedulerProxy->RemoveCore(pAllocatedNode, coreIndex);
                                    }
                                    else
                                    {
                                        // The original scheduler is still using the core. We mark the core as borrowed if it
                                        // is not already marked as borrowed.
                                        ++(*pAllocatedCore->m_pGlobalUseCountPtr);
                                        pAllocatedCore->m_coreState = ProcessorCore::Allocated;
                                        if (!pAllocatedCore->IsBorrowed())
                                        {
                                            pSchedulerProxy->ToggleBorrowedState(pAllocatedNode, coreIndex);
                                        }
                                        fAlreadyBorrowed = true;
                                    }
                                }
                                else
                                {
                                    // The core is in use by the new scheduler. We need to remove it from the
                                    // scheduler we stole it from. This is expected if the core was allocated for a
                                    // subscribed thread, or if the scheduler activated it at the time it was
                                    // added.
                                    pSchedulerProxy->RemoveCore(pAllocatedNode, coreIndex);
                                }
                            }
                            else
                            {
                                ASSERT(pNewCore->m_coreState == ProcessorCore::Unassigned);
                                // If the was core not allocated to the new scheduler, this scheduler can claim it back.
                                ++(*pAllocatedCore->m_pGlobalUseCountPtr);
                                pAllocatedNode->m_pCores[coreIndex].m_coreState = ProcessorCore::Allocated;
                            }
                        }
                    }
                }
            }
        }

        // Ensure that we did not end up lending a core to more than scheduler.
        ValidateBorrowedCores();
    }

    /// <summary>
    ///     Creates a scheduler proxy for an IScheduler that registers with the RM.
    /// </summary>
    SchedulerProxy* ResourceManager::CreateSchedulerProxy(IScheduler *pScheduler)
    {
        SchedulerPolicy policy = pScheduler->GetPolicy();
        return _concrt_new SchedulerProxy(pScheduler, this, policy);
    }

#pragma warning(default:26017)
#pragma warning(default:26011)

    //                                         DYNAMIC RM OVERVIEW
    // A high priority background thread is responsible for waking up at fixed intervals and rebalancing cores among
    // schedulers. The RM first gathers statistical information based on information about task completion and queue length
    // from each scheduler, then feeds it into a hill climbing instance, which uses history to generate a number between
    // MinHWThreads and MaxHWThreads for that scheduler. This number is the suggested new allocation for the scheduler.
    // At the same time, RM also finds the number of 'idle cores' on the machine (cores such that every scheduler that was
    // allocated on it is not using it). Idle cores are game for temporary oversubscription, if unused cores and cores the RM
    // can take from schedulers are not enough to satisfy the schedulers that need cores.
    //
    // The RM then starts to assign cores to schedulers that need cores to satisfy their new suggested allocation. It
    // takes away cores from schedulers that have more cores than their suggested allocation and looks for any unused
    // cores from schedulers that have finished execution and shutdown. After all unused cores and cores taken from other
    // schedulers have been assigned, it starts assigning idle cores to schedulers in need, oversubscribing them with
    // the original core owners (these cores are shared rather than transferred because they contribute to the minimum
    // requirement for the schedulers they belong to, and cannot be taken away). These cores are termed as borrowed cores,
    // and may be taken away if the original scheduler(s) the core was assigned to start using them).
    //
    // Once the RM has a list of schedulers willing to give up cores (termed as givers) and schedulers that need cores
    // (termed as receivers), it tries to maximize locality while taking/assigning cores. It does this in two phases.
    // For each receiver , it first tries to see if it can find an available core to assign to a partially
    // allocated node on the receiver. It cycles through the receivers assigning one core per receiver if possible,
    // until all partially allocated nodes are filled, or no cores are available on those nodes.
    //
    // If there are receivers that still need cores, it attempts to pick an empty node on each receiver in turn, which will
    // give the best fit allocation (from among available cores on that node), and proceeds to allocate to that node first.
    // It continues this process, picking a single node on a receiver per iteration until all request are satisfied.

#if defined(CONCRT_TRACING)

    /// <summary>
    ///     Captures the initial state of the global map at the beginning of core migration, each cycle.
    /// </summary>
    void ResourceManager::TraceInitialDRMState()
    {
        // Capture the initial state after calculating idle core information.
        int traceCoreIndex = 0;

        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];
            for (unsigned int coreIndex = 0; coreIndex < pGlobalNode->m_coreCount; ++coreIndex)
            {
                GlobalCore * pGlobalCore = &pGlobalNode->m_pCores[coreIndex];
                GlobalCoreData * pCoreData = &m_drmInitialState[traceCoreIndex++];
                pCoreData->m_nodeIndex = (unsigned char)nodeIndex;
                pCoreData->m_coreIndex = (unsigned char)coreIndex;
                pCoreData->m_useCount = (unsigned char)pGlobalCore->m_useCount;
                pCoreData->m_idleSchedulers = (unsigned char)pGlobalCore->m_idleSchedulers;
            }
        }
        ASSERT(traceCoreIndex == m_numTotalCores);

        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            m_ppProxyData[index]->m_pProxy->TraceInitialDRMState();
        }
    }

    /// <summary>
    ///     Captures data relating to an action during DRM preprocessing.
    /// </summary>
    void ResourceManager::TracePreProcessingAction(SchedulerProxy * pProxy, unsigned int nodeIndex, unsigned int coreIndex,
                                                   bool fMarkedAsOwned, bool fBorrowedCoreRemoved, bool fSharedCoreRemoved,
                                                   bool fIdleCore)
    {
        PreProcessingTraceData * pTraceData = &m_preProcessTraces[m_preProcessTraceIndex++];
        ASSERT(m_preProcessTraceIndex <= 100);

        pTraceData->m_pProxy = pProxy;
        pTraceData->m_nodeIndex = (unsigned char)nodeIndex;
        pTraceData->m_coreIndex = (unsigned char)coreIndex;
        pTraceData->m_fMarkedAsOwned = fMarkedAsOwned;
        pTraceData->m_fBorrowedCoreRemoved = fBorrowedCoreRemoved;
        pTraceData->m_fSharedCoreRemoved = fSharedCoreRemoved;
        pTraceData->m_fIdleCore = fIdleCore;

        if (fMarkedAsOwned)
        {
            TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Scheduler %d[min=%d, max=%d]: Preprocessing: Marking borrowed core [%d, %d] as owned (%ls))\n",
                pProxy->GetId(), pProxy->MinHWThreads(), pProxy->DesiredHWThreads(), nodeIndex, coreIndex, fIdleCore ? L"idle" : L"");
        }
        else if (fBorrowedCoreRemoved)
        {
            TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Scheduler %d[min=%d, max=%d]: Preprocessing: Removing borrowed core [%d, %d] since it is used by owner(s)(%ls)\n",
                    pProxy->GetId(), pProxy->MinHWThreads(), pProxy->DesiredHWThreads(), nodeIndex, coreIndex, fIdleCore ? L"idle" : L"");
        }
        else if (fSharedCoreRemoved)
        {
            TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Scheduler %d[min=%d, max=%d]: Preprocessing: Removing shared core [%d, %d] (%ls)\n",
                    pProxy->GetId(), pProxy->MinHWThreads(), pProxy->DesiredHWThreads(), nodeIndex, coreIndex, fIdleCore ? L"idle" : L"");
        }
    }

    /// <summary>
    ///     Captures data relating to an action during DRM core migration.
    /// </summary>
    void ResourceManager::TraceCoreMigrationAction(SchedulerProxy * pGiver, SchedulerProxy * pReceiver, unsigned int round, unsigned int nodeIndex,
                                                   unsigned int coreIndex, bool fUnusedCoreMigration, bool fIdleCoreSharing, bool fBorrowedByGiver,
                                                   bool fIdleOnGiver)
    {
        DynamicAllocationTraceData * pTraceData = &m_dynAllocationTraces[m_dynAllocationTraceIndex++];
        ASSERT(m_dynAllocationTraceIndex <= 100);

        pTraceData->m_pGiver = pGiver;
        pTraceData->m_pReceiver = pReceiver,
        pTraceData->m_round = (unsigned char)round;
        pTraceData->m_nodeIndex = (unsigned char)nodeIndex;
        pTraceData->m_coreIndex = (unsigned char)coreIndex;
        pTraceData->m_fUnusedCoreMigration = fUnusedCoreMigration;
        pTraceData->m_fIdleCoreSharing = fIdleCoreSharing;
        pTraceData->m_fBorrowedByGiver = fBorrowedByGiver;
        pTraceData->m_fIdleOnGiver = fIdleOnGiver;

        if (fUnusedCoreMigration)
        {
             TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Assigned unused core [%d, %d] to scheduler %d[min=%d, max=%d]\n",
                    nodeIndex, coreIndex, pReceiver->GetId(), pReceiver->MinHWThreads(), pReceiver->DesiredHWThreads());
        }
        else if (fIdleCoreSharing)
        {
             TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Assigned idle core [%d, %d] to scheduler %d[min=%d, max=%d]\n",
                    nodeIndex, coreIndex, pReceiver->GetId(), pReceiver->MinHWThreads(), pReceiver->DesiredHWThreads());
        }
        else
        {
             TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Migrated core [%d, %d] from scheduler %d[min=%d, max=%d] to scheduler %d[min=%d, max=%d] (%ls%ls%ls)\n",
                    nodeIndex, coreIndex, pGiver->GetId(), pGiver->MinHWThreads(), pGiver->DesiredHWThreads(),
                    pReceiver->GetId(), pReceiver->MinHWThreads(), pReceiver->DesiredHWThreads(), fBorrowedByGiver ? L"borrowed" : L"",
                    fBorrowedByGiver && fIdleOnGiver ? L", " : L"", fIdleOnGiver ? L"idle" : L"" );
        }
    }

#endif

    /// <summary>
    ///     Performs state validations during static allocation.
    /// </summary>
    void ResourceManager::ValidateStaticSchedulerState(SchedulerProxy * pSchedulerProxy)
    {
#if defined (_DEBUG)
        SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
        unsigned int numAllocated = 0;

        // For each core, go through every scheduler and find out if more than 1 scheduler has that core borrowed
        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode *pAllocatedNode = &pAllocatedNodes[nodeIndex];

            for (unsigned int coreIndex = 0; coreIndex < pAllocatedNode->m_coreCount; ++coreIndex)
            {
                SchedulerCore * pAllocatedCore = &pAllocatedNode->m_pCores[coreIndex];
                if (pAllocatedCore->m_coreState == ProcessorCore::Allocated)
                {
                    numAllocated++;
                }
            }
        }

        ASSERT(numAllocated >= pSchedulerProxy->MinVprocHWThreads());
#endif
    }

    /// <summary>
    ///     Performs state validations during dynamic core migration.
    /// </summary>
    void ResourceManager::ValidateDRMSchedulerState()
    {
#if defined (_DEBUG)
        // No schedulers should have any borrowed idle cores left
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            ASSERT(m_ppProxyData[index]->m_numBorrowedIdleCores == 0);
        }
        ValidateBorrowedCores();
#endif
    }

    /// <summary>
    ///     Performs borrowed core validation. A core can be borrowed by only one scheduler at a time.
    /// </summary>
    void ResourceManager::ValidateBorrowedCores()
    {
#if defined (_DEBUG)
        // For each core, go through every scheduler and find out if more than 1 scheduler has that core borrowed.
        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            for (unsigned int coreIndex = 0; coreIndex < m_pGlobalNodes[nodeIndex].m_coreCount; ++coreIndex)
            {
                unsigned int numBorrowingSchedulers = 0;
                for (unsigned int index = 0; index < m_numSchedulers; ++index)
                {
                    SchedulerNode * pAllocatedNodes = m_ppProxyData[index]->m_pProxy->GetAllocatedNodes();
                    SchedulerCore * pAllocatedCore = &pAllocatedNodes[nodeIndex].m_pCores[coreIndex];
                    if (pAllocatedCore->m_coreState == ProcessorCore::Allocated && pAllocatedCore->IsBorrowed())
                    {
                        ++numBorrowingSchedulers;
                        ASSERT(numBorrowingSchedulers < 2);
                    }
                }
            }
        }
#endif
    }

    /// <summary>
    ///     Ensures that the memory buffers needed for dynamic RM are of the right size, and initializes them.
    /// </summary>
    void ResourceManager::InitializeRMBuffers()
    {
        if (m_maxSchedulers < m_numSchedulers)
        {
            // Resize the buffers.
            while (m_maxSchedulers < m_numSchedulers)
            {
                m_maxSchedulers *= 2;
            }

            delete [] m_ppProxyData;
            delete [] m_ppGivingProxies;
            delete [] m_ppReceivingProxies;

            m_ppProxyData = _concrt_new AllocationData *[m_maxSchedulers];
            if (m_ppGivingProxies != NULL)
            {
                ASSERT(m_ppReceivingProxies != NULL);
                m_ppGivingProxies = _concrt_new DynamicAllocationData *[m_maxSchedulers];
                m_ppReceivingProxies = _concrt_new DynamicAllocationData *[m_maxSchedulers];
            }
        }

        memset(m_ppProxyData, 0, sizeof(AllocationData *) * m_numSchedulers);
        if (m_ppGivingProxies != NULL)
        {
            ASSERT(m_ppReceivingProxies != NULL);
            memset(m_ppGivingProxies, 0, sizeof(DynamicAllocationData *) * m_numSchedulers);
            memset(m_ppReceivingProxies, 0, sizeof(DynamicAllocationData *) * m_numSchedulers);
        }

#if defined(CONCRT_TRACING)
        memset(m_preProcessTraces, 0, sizeof(PreProcessingTraceData) * m_preProcessTraceIndex);
        memset(m_dynAllocationTraces, 0, sizeof(DynamicAllocationTraceData) * m_dynAllocationTraceIndex);
        m_preProcessTraceIndex = m_dynAllocationTraceIndex = 0;
#endif
    }

    /// <summary>
    ///     Toggles the idle state on a core during the dynamic RM phase and updates tracking counts.
    /// </summary>
    void ResourceManager::ToggleRMIdleState(SchedulerNode * pAllocatedNode, SchedulerCore * pAllocatedCore,
                                             GlobalNode * pGlobalNode, GlobalCore * pGlobalCore, AllocationData * pDRMData)
    {
        if (pAllocatedCore->IsIdle())
        {
            pAllocatedCore->m_fIdleDuringDRM = false;

            --pDRMData->m_numIdleCores;
            --pAllocatedNode->m_numDRMIdle;

            if (pAllocatedCore->IsBorrowed())
            {
                --pDRMData->m_numBorrowedIdleCores;
                --pAllocatedNode->m_numDRMBorrowedIdle;
            }

            if (pGlobalCore->m_coreState == ProcessorCore::Idle)
            {
                pGlobalCore->m_coreState = ProcessorCore::Unknown;
                --pGlobalNode->m_idleCores;
                --m_dynamicIdleCoresAvailable;
            }
            --pGlobalCore->m_idleSchedulers;
        }
        else
        {
            pAllocatedCore->m_fIdleDuringDRM = true;

            ++pDRMData->m_numIdleCores;
            ++pAllocatedNode->m_numDRMIdle;

            if (pAllocatedCore->IsBorrowed())
            {
                ++pDRMData->m_numBorrowedIdleCores;
                ++pAllocatedNode->m_numDRMBorrowedIdle;
            }

            ASSERT(pGlobalCore->m_coreState != ProcessorCore::Idle);

            ++pGlobalCore->m_idleSchedulers;
            ASSERT(pGlobalCore->m_idleSchedulers <= pGlobalCore->m_useCount);
        }
    }

    /// <summary>
    ///     Populates data needed for allocation (static or dynamic).
    /// </summary>
    void ResourceManager::PopulateCommonAllocationData(unsigned int index, SchedulerProxy * pSchedulerProxy, AllocationData * pAllocationData)
    {
        pAllocationData->m_index = index;
        pAllocationData->m_scaledAllocation = 0.0;
        pAllocationData->m_pProxy = pSchedulerProxy;

        // We need to find the number of inactive cores on this scheduler proxy. Note that since cores can be activated and deactivated at any
        // time, any number we use for number of idle cores could be stale the moment we compute it. However, that is the nature of the problem.
        // We work from data at a particular snapshot in time.
        SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
        if (pAllocatedNodes != NULL) // this could be the scheduler that is just being created, in which case the field is NULL.
        {
            for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
            {
                SchedulerNode * pAllocatedNode = &pAllocatedNodes[nodeIndex];
                // Set these to zero before we start counting up idle cores we find.
                pAllocatedNode->m_numDRMIdle = 0;
                pAllocatedNode->m_numDRMBorrowedIdle = 0;
                if (pAllocatedNode->m_allocatedCores > 0)
                {
                    for (unsigned int coreIndex = 0; coreIndex < pAllocatedNode->m_coreCount; ++coreIndex)
                    {
                        SchedulerCore * pAllocatedCore = &pAllocatedNode->m_pCores[coreIndex];
                        pAllocatedCore->m_fIdleDuringDRM = false;

                        if (pAllocatedCore->m_coreState == ProcessorCore::Allocated)
                        {
                            // Subscription level can change out from under us.
                            if (pAllocatedCore->m_subscriptionLevel == 0)
                            {
                                GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];
                                GlobalCore * pGlobalCore = &pGlobalNode->m_pCores[coreIndex];
                                // If the subscription level is 0 this core is idle in the current scheduler proxy. Increment the count of idle schedulers
                                // on the global core - this represents the number of schedulers the core has been assigned to, that are not currently
                                // using the core.

                                // Note that a fixed core can end up with a subscription level of 0, if it was fixed because code running on a virtual
                                // processor subscribed to a scheduler, and the virtual processor switched to a different context for whatever reason.
                                // If this core is idle, it cannot be taken away because it is fixed, and the fact that it contribues to min will take
                                // care of that. It can however be lent to a different scheduler.
                                ToggleRMIdleState(pAllocatedNode, pAllocatedCore, pGlobalNode, pGlobalCore, pAllocationData);

                                // As noted above, this information could become stale soon after, but for the purpose of this iteration of core migration,
                                // we consider the core to be idle.
                            }
                        }
                    }
                }
            }
        }
    }

    /// <summary>
    ///     Captures data needed for static allocation, for all existing schedulers. This includes determining which
    ///     cores on a scheduler are idle.
    /// </summary>
    void ResourceManager::SetupStaticAllocationData(SchedulerProxy * pNewSchedulerProxy, bool fSubscribeCurrentThread)
    {
        InitializeRMBuffers();

        // The scheduler we're allocating cores to, goes at index 0 in the array for convenience.
        StaticAllocationData * pStaticData = pNewSchedulerProxy->GetStaticAllocationData();
        memset(pStaticData, 0, sizeof(StaticAllocationData));
        PopulateCommonAllocationData(0, pNewSchedulerProxy, pStaticData);

        // Initialize the static allocation specific fields.
        pStaticData->m_adjustedDesired = pNewSchedulerProxy->DesiredHWThreads(); // for the new scheduler this value could be reset later
        pStaticData->m_fNeedsExternalThreadAllocation = fSubscribeCurrentThread;
        m_ppProxyData[0] = pStaticData;

        // Start the index at 1 for the remaining schedulers.
        unsigned int index = 1;
        SchedulerProxy * pSchedulerProxy = NULL;

        for (pSchedulerProxy = m_schedulers.First(); pSchedulerProxy != NULL; pSchedulerProxy = m_schedulers.Next(pSchedulerProxy))
        {
            if (pSchedulerProxy != pNewSchedulerProxy)
            {
                StaticAllocationData * pStaticData = pSchedulerProxy->GetStaticAllocationData();
                memset(pStaticData, 0, sizeof(StaticAllocationData));

                PopulateCommonAllocationData(index, pSchedulerProxy, pStaticData);

                // Initialize the static allocation specific fields.
                pStaticData->m_adjustedDesired = pSchedulerProxy->DesiredHWThreads();
                m_ppProxyData[index] = pStaticData;
                ++index;
            }
        }
        ASSERT(index == m_numSchedulers);
    }

    /// <summary>
    ///     Captures data needed for dynamic allocation for all existing schedulers. This includes gathering statistics
    ///     and invoking a per scheduler hill climbing instance to get a suggested future allocation. Also determines how many
    ///     idle cores a scheduler has.
    /// </summary>
    void ResourceManager::PopulateDynamicAllocationData()
    {
        unsigned int index = 0;
        SchedulerProxy * pSchedulerProxy = NULL;

        InitializeRMBuffers();

        for (pSchedulerProxy = m_schedulers.First(); pSchedulerProxy != NULL; pSchedulerProxy = m_schedulers.Next(pSchedulerProxy))
        {
            DynamicAllocationData * pDynamicData = pSchedulerProxy->GetDynamicAllocationData();
            memset(pDynamicData, 0, sizeof(DynamicAllocationData));

            PopulateCommonAllocationData(index, pSchedulerProxy, pDynamicData);

            // Initialize the dynamic allocation specific fields.
            if (pSchedulerProxy->IsHillClimbingEnabled())
            {
                // Initialize variables needed for statistics
                unsigned int taskCompletionRate = 0, taskArrivalRate = 0;

                // Get the stored scheduler queue length
                unsigned int numberOfTasksEnqueued = pSchedulerProxy->GetQueueLength();

                // Get the current number of cores allocated to this scheduler
                unsigned int numCoresAllocated = pSchedulerProxy->GetNumAllocatedCores();

                // Collect statistical information about this scheduler
                pSchedulerProxy->Scheduler()->Statistics(&taskCompletionRate, &taskArrivalRate, &numberOfTasksEnqueued);

                // Let hill climbing decide on the future allocation of cores for this scheduler.
                pDynamicData->m_suggestedAllocation = pSchedulerProxy->DoHillClimbing(numCoresAllocated,
                                                                                      taskCompletionRate,
                                                                                      taskArrivalRate,
                                                                                      numberOfTasksEnqueued);

                // Ensure that the new allocation does not exceed maxconcurrency. Take in-use cores into account
                if (pDynamicData->m_suggestedAllocation > pSchedulerProxy->GetNumAllocatedCores())
                {
                    pDynamicData->m_suggestedAllocation = pSchedulerProxy->AdjustAllocationIncrease(pDynamicData->m_suggestedAllocation);
                }

#if defined(CONCRT_TRACING)
                pDynamicData->m_originalSuggestedAllocation = pDynamicData->m_suggestedAllocation;
#endif

                // Update the queue length using the number computed by the statistics
                pSchedulerProxy->SetQueueLength(numberOfTasksEnqueued);

                ASSERT(pDynamicData->m_suggestedAllocation >= pSchedulerProxy->MinHWThreads()
                        && pDynamicData->m_suggestedAllocation <= pSchedulerProxy->DesiredHWThreads());
            }
            else
            {
                pDynamicData->m_suggestedAllocation = pSchedulerProxy->GetNumAllocatedCores();
            }

            // Fully loaded is used to mark schedulers that:
            //  1) Have a non-zero number of cores (or nested thread subscriptions), but no idle cores.
            //  2) Have a suggested allocation greater than or equal to what they currenty have.
            //  3) Have less cores than they desire.
            // If we have extra cores to give away or share, these schedulers could benefit from extra cores.
            if (pSchedulerProxy->GetNumAllocatedCores() > 0)
            {
                pDynamicData->m_fFullyLoaded = (pDynamicData->m_numIdleCores == 0 &&
                                                pSchedulerProxy->GetNumAllocatedCores() <= pDynamicData->m_suggestedAllocation &&
                                                pSchedulerProxy->GetNumAllocatedCores() < pSchedulerProxy->DesiredHWThreads());
            }
            else
            {
                // Account for external thread subscriptions on a nested scheduler with min = 0
                ASSERT(pSchedulerProxy->GetNumAllocatedCores() == 0);
                ASSERT(pDynamicData->m_numIdleCores == 0);
                pDynamicData->m_fFullyLoaded = (pSchedulerProxy->GetNumNestedThreadSubscriptions() > 0 &&
                                                pSchedulerProxy->GetNumAllocatedCores() <= pDynamicData->m_suggestedAllocation &&
                                                pSchedulerProxy->GetNumAllocatedCores() < pSchedulerProxy->DesiredHWThreads());
            }

            TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Scheduler %d[min=%d, max=%d]: Initial values - Allocated: %d, Suggested: %d, Idle: %d, Borrowed: %d\n",
                pSchedulerProxy->GetId(), pSchedulerProxy->MinHWThreads(), pSchedulerProxy->DesiredHWThreads(), pSchedulerProxy->GetNumAllocatedCores(),
                pDynamicData->m_suggestedAllocation, pDynamicData->m_numIdleCores, pSchedulerProxy->GetNumBorrowedCores());

            m_ppProxyData[index] = pDynamicData;
            ++index;
        }

#if defined(CONCRT_TRACING)
        TraceInitialDRMState();
#endif
        ASSERT(index == m_numSchedulers);
    }

    /// <summary>
    ///     Undo global state that was initialized to perform static allocation or dynamic core migration.
    /// </summary>
    void ResourceManager::ResetGlobalAllocationData()
    {
        // Clear changes we've potentially made to the global nodes.
        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];
            pGlobalNode->m_availableCores = 0;
            pGlobalNode->m_idleCores = 0;

            for (unsigned int coreIndex = 0; coreIndex < pGlobalNode->m_coreCount; ++coreIndex)
            {
                GlobalCore * pGlobalCore = &pGlobalNode->m_pCores[coreIndex];

                pGlobalCore->m_coreState = ProcessorCore::Unknown;
                pGlobalCore->m_idleSchedulers = 0;
            }
        }
    }

    /// <summary>
    ///     Preprocessing steps for borrowed cores - both static and dynamic allocation start out with a call to this API.
    ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
    ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed anymore.
    /// </summary>
    void ResourceManager::HandleBorrowedCores(SchedulerProxy * pSchedulerProxy, AllocationData * pAllocationData)
    {
        ASSERT(pSchedulerProxy->GetNumBorrowedCores() > 0);

        SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode * pAllocatedNode = &pAllocatedNodes[nodeIndex];
            if (pAllocatedNode->m_numBorrowedCores > 0)
            {
                ASSERT(pAllocatedNode->m_allocatedCores >= pAllocatedNode->m_numBorrowedCores);
                for (unsigned int coreIndex = 0; coreIndex < pAllocatedNode->m_coreCount; ++coreIndex)
                {
                    SchedulerCore * pAllocatedCore = &pAllocatedNode->m_pCores[coreIndex];
                    if ((pAllocatedCore->m_coreState == ProcessorCore::Allocated) && pAllocatedCore->IsBorrowed())
                    {
                        ASSERT(!pAllocatedCore->IsFixed());
                        GlobalCore * pGlobalCore = &(m_pGlobalNodes[nodeIndex].m_pCores[coreIndex]);
                        if (pGlobalCore->m_useCount == 1)
                        {
#if defined(CONCRT_TRACING)
                            TracePreProcessingAction(pSchedulerProxy,
                                                     nodeIndex,
                                                     coreIndex,
                                                     true,  /* borrowed core marked as owned */
                                                     false, /* borrowed core removed */
                                                     false, /* shared core removed */
                                                     pAllocatedCore->IsIdle());
#endif

                            // This scheduler is the only one this core is assigned to. This could happen if
                            // the scheduler(s) the core was being shared with were shutdown.
                            ASSERT(pGlobalCore->m_idleSchedulers == 0 || (pAllocatedCore->IsIdle() && pGlobalCore->m_idleSchedulers == 1));
                            ASSERT(pAllocatedCore->IsBorrowed());
                            pSchedulerProxy->ToggleBorrowedState(pAllocatedNode, coreIndex);
                            if (pAllocatedCore->IsIdle())
                            {
                                --pAllocatedNode->m_numDRMBorrowedIdle;
                                --pAllocationData->m_numBorrowedIdleCores;
                            }
                        }
                        else if ((pGlobalCore->m_useCount - pGlobalCore->m_idleSchedulers) > (unsigned int)(pAllocatedCore->IsIdle() ? 0 : 1))
                        {
#if defined(CONCRT_TRACING)
                            TracePreProcessingAction(pSchedulerProxy,
                                                     nodeIndex,
                                                     coreIndex,
                                                     false, /* borrowed core marked as owned */
                                                     true,  /* borrowed core removed */
                                                     false, /* shared core removed */
                                                     pAllocatedCore->IsIdle());
#endif

                            // One or more of the other scheduler(s) this core is assigned to are using it. Since this was a borrowed core,
                            // we take it away here.
                            --pGlobalCore->m_useCount;
                            // Update the dynamic allocation data if this core is idle.
                            if(pAllocatedCore->IsIdle())
                            {
                                ToggleRMIdleState(pAllocatedNode, pAllocatedCore, &m_pGlobalNodes[nodeIndex], pGlobalCore, pAllocationData);
                            }
                            pSchedulerProxy->RemoveCore(pAllocatedNode, coreIndex);
                        }
                    }
                }
            }
        }
    }

    /// <summary>
    ///     Preprocessing steps for shared cores - this is used during dynamic core migration.
    ///     - If the suggested allocation is less than the current allocation for a scheduler that has shared cores (cores oversubscribed
    ///         with a different scheduler), those cores are taken away here, since we want to minimize sharing.
    /// </summary>
    void ResourceManager::HandleSharedCores(SchedulerProxy * pSchedulerProxy, DynamicAllocationData * pAllocationData)
    {
        ASSERT(pAllocationData->m_numBorrowedIdleCores <= pSchedulerProxy->GetNumAllocatedCores() - pAllocationData->m_suggestedAllocation);
        unsigned int maxCoresToRemove = min(pSchedulerProxy->GetNumAllocatedCores() - pAllocationData->m_suggestedAllocation - pAllocationData->m_numBorrowedIdleCores,
                                            pSchedulerProxy->GetNumOwnedCores() - pSchedulerProxy->MinHWThreads());

        SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
        for (unsigned int nodeIndex = 0; maxCoresToRemove > 0 && nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode * pAllocatedNode = &pAllocatedNodes[nodeIndex];
            if (pAllocatedNode->GetNumMigratableCores() > 0)
            {
                ASSERT(pAllocatedNode->m_allocatedCores >= pAllocatedNode->m_numBorrowedCores);
                for (unsigned int coreIndex = 0; maxCoresToRemove > 0 && coreIndex < pAllocatedNode->m_coreCount; ++coreIndex)
                {
                    SchedulerCore * pAllocatedCore = &pAllocatedNode->m_pCores[coreIndex];
                    if (pAllocatedCore->m_coreState == ProcessorCore::Allocated &&
                        !pAllocatedCore->IsFixed() && !pAllocatedCore->IsBorrowed())
                    {
                        GlobalCore * pGlobalCore = &(m_pGlobalNodes[nodeIndex].m_pCores[coreIndex]);
                        ASSERT(pGlobalCore->m_useCount > 0);
                        if (pGlobalCore->m_useCount > 1)
                        {
#if defined(CONCRT_TRACING)
                            TracePreProcessingAction(pSchedulerProxy,
                                                     nodeIndex,
                                                     coreIndex,
                                                     false, /* borrowed core marked as owned */
                                                     false, /* borrowed core removed */
                                                     true,  /* shared core removed */
                                                     pAllocatedCore->IsIdle());
#endif

                            --pGlobalCore->m_useCount;

                            // Update the dynamic allocation data if this core is idle.
                            if(pAllocatedCore->IsIdle())
                            {
                                ToggleRMIdleState(pAllocatedNode, pAllocatedCore, &m_pGlobalNodes[nodeIndex], pGlobalCore, pAllocationData);
                            }

                            pSchedulerProxy->RemoveCore(pAllocatedNode, coreIndex);
                            --maxCoresToRemove;
                        }
                    }
                }
            }
        }
        ASSERT(pAllocationData->m_suggestedAllocation <= pSchedulerProxy->GetNumAllocatedCores());
        ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());
        ASSERT(pAllocationData->m_numBorrowedIdleCores <= pSchedulerProxy->GetNumAllocatedCores() - pAllocationData->m_suggestedAllocation);
    }

    /// <summary>
    ///     A number of preprocessing steps are performed before we are ready to allocate cores. They include handling of borrowed and idle
    ///     cores, as follows:
    ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
    ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed anymore.
    /// </summary>
    void ResourceManager::PreProcessStaticAllocationData()
    {
        // Borrowed cores must be handled only after ALL schedulers have gone through populating allocation data.
        // We need global information about idle cores when we deal with borrowed cores below.
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            SchedulerProxy * pSchedulerProxy = m_ppProxyData[index]->m_pProxy;
            if (pSchedulerProxy->GetNumBorrowedCores() > 0)
            {
                ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());
                HandleBorrowedCores(pSchedulerProxy, m_ppProxyData[index]);
            }
        }
    }

    /// <summary>
    ///     A number of preprocessing steps are preformed before we are ready to migrate cores. They include handling of borrowed, idle,
    ///     and shared cores, as follows:
    ///
    ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
    ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed anymore.
    ///     - If hill climbing has suggested an allocation increase for a scheduler that has idle cores, or an allocation decrease that
    ///         does not take away all its idle cores, the RM overrides it, setting the suggested allocation for that scheduler to
    ///         max(minCores, allocatedCores - idleCores).
    ///
    ///       The new value of suggested allocation is used for the following:
    ///     - If the suggested allocation is less than the current allocation for a scheduler that has shared cores (cores oversubscribed
    ///         with a different scheduler), those cores are taken away here, since we want to minimize sharing.
    /// </summary>
    void ResourceManager::PreProcessDynamicAllocationData()
    {
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            DynamicAllocationData * pDynamicData = static_cast<DynamicAllocationData *>(m_ppProxyData[index]);
            SchedulerProxy * pSchedulerProxy = pDynamicData->m_pProxy;
            ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());

            if (pSchedulerProxy->GetNumBorrowedCores() > 0)
            {
                HandleBorrowedCores(pSchedulerProxy, pDynamicData);
            }

            ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());
            ASSERT(pDynamicData->m_numIdleCores <= pSchedulerProxy->GetNumAllocatedCores());

            // If hill climbing has suggested an allocation increase for a scheduler with idle cores, or an allocation decrease that does not
            // take away all its idle cores over the minimum, we override the suggested allocation here.
            if (pDynamicData->m_numIdleCores > 0 &&
                pDynamicData->m_suggestedAllocation > pSchedulerProxy->GetNumAllocatedCores() - pDynamicData->m_numIdleCores)
            {
                pDynamicData->m_suggestedAllocation = max(pSchedulerProxy->MinHWThreads(), pSchedulerProxy->GetNumAllocatedCores() - pDynamicData->m_numIdleCores);
                ASSERT(pDynamicData->m_fFullyLoaded == false);
            }

            // Make another pass, since the loop above could change the state of some cores, as well as the borrowed and allocated counts.
            // Check if we can take away any owned shared cores from this scheduler. We don't want to migrate these cores, so we can minimize
            // sharing if possible. While taking away cores, we must ensure that there are enough owned cores to satisfy MinHWThreads().

            // Since we must migrate all borrowed idle cores, (we don't want to lend the underlying core to a different scheduler as part of the
            // distribute idle cores phase), we need to take that into account while deciding how many shared owned cores we can give up, if any.

            if (pDynamicData->m_suggestedAllocation < pSchedulerProxy->GetNumAllocatedCores() &&
                pSchedulerProxy->GetNumOwnedCores() > pSchedulerProxy->MinHWThreads())
            {
                HandleSharedCores(pSchedulerProxy, pDynamicData);
                ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());
            }

            // Fix for Intel - Prevent current concurrency from going over max concurrency.
            if (!pSchedulerProxy->IsHillClimbingEnabled() && (pDynamicData->m_suggestedAllocation > pSchedulerProxy->GetNumAllocatedCores()))
            {
                pDynamicData->m_suggestedAllocation = pSchedulerProxy->GetNumAllocatedCores();
            }
        }
    }

    /// <summary>
    ///     This routine increases the suggested allocation to desired, for schedulers with the following characteristics:
    ///     1) Hill climbing has *not* recommended an allocation decrease.
    ///     2) They are using all the cores allocated to them (no idle cores).
    ///     In the second round of core migration, we try to satisfy these schedulers' desired allocation.
    /// </summary>
    void ResourceManager::IncreaseFullyLoadedSchedulerAllocations()
    {
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            DynamicAllocationData * pDynamicData = static_cast<DynamicAllocationData *>(m_ppProxyData[index]);
            if (pDynamicData->m_fFullyLoaded == true)
            {
                SchedulerProxy * pSchedulerProxy = pDynamicData->m_pProxy;
                ASSERT(pDynamicData->m_suggestedAllocation >= pSchedulerProxy->GetNumAllocatedCores());
                ASSERT(pDynamicData->m_numIdleCores == 0);
                // Increase the suggested allocation to it's max. Note that this will ONLY be satisfied if cores can be taken away from other schedulers,
                // or if there are idle/unused cores available.
                pDynamicData->m_suggestedAllocation = pSchedulerProxy->AdjustAllocationIncrease(pSchedulerProxy->DesiredHWThreads());
            }
        }
    }

    /// <summary>
    ///     Decides on the number of additional cores to give a set of schedulers given what the schedulers need and what is available.
    ///     If the sum of needed cores is less than the sum of all available cores, it reduces the amount of cores it will give each
    ///     scheduler. The additional allocation is proportional to the difference between the suggested allocation and what the
    ///     scheduler already has allocated to it.
    /// </summary>
    unsigned int ResourceManager::AdjustDynamicAllocation(unsigned int coresAvailable, unsigned int coresNeeded, unsigned int numReceivers)
    {
        unsigned int coresTransferred = 0;
        if (coresAvailable < coresNeeded)
        {
            // We cannot satsify the requirement with what we have available. We take the cores available and distribute them
            // proportional to the requirement of the schedulers that need cores.
            if (numReceivers == 1)
            {
                // If there is just one receiver, we can simply assign the available cores to it.
                m_ppReceivingProxies[0]->m_allocation = coresAvailable;
            }
            else
            {
                // Go through the receivers and reduce their new allocation proportional to the difference between
                // what they already have and the suggested new allocation, so we can satisfy it.
                double scaling = (double)coresAvailable/(double)coresNeeded;

                for (unsigned int rec = 0; rec < numReceivers; ++rec)
                {
                    // Scale the difference between the new allocation and what the proxy already has. After rounding up,
                    // we get an integer that represents the additional number of cores to give to this scheduler out of
                    // what we have available.
                    m_ppReceivingProxies[rec]->m_scaledAllocation = scaling * (m_ppReceivingProxies[rec]->m_suggestedAllocation -
                                                                                m_ppReceivingProxies[rec]->m_pProxy->GetNumAllocatedCores());
                }

                // RoundUpScaledAllocations populates m_ppReceivingProxies[rec]->m_allocation, such that the sum of all allocations
                // is the number of cores available.
                RoundUpScaledAllocations((AllocationData **)m_ppReceivingProxies, numReceivers, coresAvailable);
            }
            coresTransferred = coresAvailable;
        }
        else
        {
            // We can satisfy all receivers with the number available.
            for (unsigned int rec = 0; rec < numReceivers; ++rec)
            {
                m_ppReceivingProxies[rec]->m_allocation = m_ppReceivingProxies[rec]->m_suggestedAllocation -
                                                            m_ppReceivingProxies[rec]->m_pProxy->GetNumAllocatedCores();
            }
            coresTransferred = coresNeeded;
        }
        return coresTransferred;
    }

    /// <summary>
    ///     Initializes receiving proxy data in preparation for core transfer. Calcuates the number of partially filled nodes
    ///     for schedulers that are receiving cores, and sorts the receiving proxy data in increasing order of partial nodes.
    /// </summary>
    /// <returns>
    ///     Number of recivers that still need cores (allocation field of the receiving proxy data > 0).
    /// </returns>
    unsigned int ResourceManager::PrepareReceiversForCoreTransfer(unsigned int numReceivers)
    {
        // For all receiving proxies with an allocation > 0 sort them by the number of partially filled nodes
        // (nodes which have some but not cores on it allocated to the proxy), lowest first. We only care about
        // proxies with non-zero partial nodes.
        for (unsigned int i = 0; i < numReceivers; ++i)
        {
            while ((i < numReceivers) && (m_ppReceivingProxies[i]->m_allocation == 0))
            {
                // Swap this element into the end of the array - we are not interested in it for the sort.
                --numReceivers;
                DynamicAllocationData * pTemp = m_ppReceivingProxies[i];
                m_ppReceivingProxies[i] = m_ppReceivingProxies[numReceivers];
                m_ppReceivingProxies[numReceivers] = pTemp;
            }

            // Initialize variables that are used during core transfer;
            m_ppReceivingProxies[i]->m_numPartiallyFilledNodes = 0;
            m_ppReceivingProxies[i]->m_startingNodeIndex = 0;
            m_ppReceivingProxies[i]->m_fExactFitAllocation = true;
        }

        // At this point numReceivers is the number of receivers with a non-zero suggested allocation increase.
        ASSERT(numReceivers > 0);
        unsigned int remainingReceivers = numReceivers;

        for (unsigned int i = 0; i < numReceivers; ++i)
        {
            while ((i < numReceivers) && (m_ppReceivingProxies[i]->m_numPartiallyFilledNodes == 0))
            {
                // Calculate the number of partially filled nodes for this receiver.
                SchedulerNode * pAllocatedNodes = m_ppReceivingProxies[i]->m_pProxy->GetAllocatedNodes();
                for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
                {
                    if ((pAllocatedNodes[nodeIndex].m_allocatedCores > 0) &&
                        (pAllocatedNodes[nodeIndex].m_allocatedCores < pAllocatedNodes[nodeIndex].m_coreCount))
                    {
                        ++m_ppReceivingProxies[i]->m_numPartiallyFilledNodes;
                    }
                }

                // If this proxy has 0 partially filled nodes - swap it into the end of the array - we are not interested in
                // it for the purposes of sorting.
                if (m_ppReceivingProxies[i]->m_numPartiallyFilledNodes == 0)
                {
                    --numReceivers;
                    DynamicAllocationData * pTemp = m_ppReceivingProxies[i];
                    m_ppReceivingProxies[i] = m_ppReceivingProxies[numReceivers];
                    m_ppReceivingProxies[numReceivers] = pTemp;
                }
            }
        }

        // The elements in the array with indices [0, numReceivers) has proxies with non-zero partially filled nodes.
        // Perform a lowest first selection sort.
        for (unsigned int i = 0; i < numReceivers; ++i)
        {
            unsigned int minIndex = i;
            for (unsigned int j = i + 1; j < numReceivers; ++j)
            {
                if (m_ppReceivingProxies[j]->m_numPartiallyFilledNodes < m_ppReceivingProxies[minIndex]->m_numPartiallyFilledNodes)
                {
                    minIndex = j;
                }
            }
            if (i != minIndex)
            {
                DynamicAllocationData * pTemp = m_ppReceivingProxies[i];
                m_ppReceivingProxies[i] = m_ppReceivingProxies[minIndex];
                m_ppReceivingProxies[minIndex] = pTemp;
            }

            // Now, for the scheduler proxy at location i, sort the nodes, using the sorted indices array.
            // Place the partial nodes at the beginning of the array, and among partially filled nodes,
            // sort by number of unallocated cores (lowest first).
            SchedulerNode * pAllocatedNodes = m_ppReceivingProxies[i]->m_pProxy->GetAllocatedNodes();
            unsigned int * pSortedNodeOrder = m_ppReceivingProxies[i]->m_pProxy->GetSortedNodeOrder();

            // The outer loop only needs to make as many iterations as there are partially filled nodes in this
            // scheduler proxy.
            for (unsigned int m = 0; m < m_ppReceivingProxies[i]->m_numPartiallyFilledNodes; ++m)
            {
                unsigned int minIndex = m;
                // We use the array 'sorted node order' to sort nodes, instead of swapping the nodes themselves
                SchedulerNode * pMinNode = &pAllocatedNodes[pSortedNodeOrder[minIndex]];

                // The inner node must make as many iterations as there are nodes.
                for (unsigned int n = m + 1; n < m_nodeCount; ++n)
                {
                    SchedulerNode * pNode = &pAllocatedNodes[pSortedNodeOrder[n]];

                    // Check if the current node is a partially filled node, and set it to be min, either if min
                    // is not a partially filled node itself, or if the current node is more tightly packed than the min node.
                    if ((pNode->m_allocatedCores > 0 && pNode->m_allocatedCores < pNode->m_coreCount) &&
                        (!(pMinNode->m_allocatedCores > 0 && pMinNode->m_allocatedCores < pMinNode->m_coreCount) ||
                          (pNode->m_allocatedCores > pMinNode->m_allocatedCores)))
                    {
                        minIndex = n;
                        pMinNode = &pAllocatedNodes[pSortedNodeOrder[minIndex]];
                    }
                }

                if (m != minIndex)
                {
                    // Swap the index at 'minIndex' with the index at 'm'. The next iteration will traverse nodes starting at
                    // m_pSortedNodeOrder[m + 1].
                    unsigned int tempIndex = pSortedNodeOrder[m];
                    pSortedNodeOrder[m] = pSortedNodeOrder[minIndex];
                    pSortedNodeOrder[minIndex] = tempIndex;
                }
            }
        }

        return remainingReceivers;
    }

    /// <summary>
    ///     Assigns a fixed number of unused cores to a scheduler.
    /// </summary>
    void ResourceManager::DynamicAssignCores(SchedulerProxy * pReceivingProxy, unsigned int nodeIndex, unsigned int numCoresToAssign, bool fIdle)
    {
        GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];
        ASSERT(numCoresToAssign > 0);
        ASSERT ((!fIdle && pGlobalNode->m_availableCores >= numCoresToAssign) ||
                (fIdle && pGlobalNode->m_idleCores >= numCoresToAssign));

        for (unsigned int coreIndex = 0; coreIndex < pGlobalNode->m_coreCount; ++coreIndex)
        {
            GlobalCore * pGlobalCore = &pGlobalNode->m_pCores[coreIndex];
            // We claim reserved cores when fIdle is false, and idle cores when fIdle is true.
            if ((pGlobalCore->m_coreState == ProcessorCore::Available && !fIdle)
                || (pGlobalCore->m_coreState == ProcessorCore::Idle && fIdle))
            {
#if defined(CONCRT_TRACING)
                TraceCoreMigrationAction(NULL,   /* no giving proxy */
                                         pReceivingProxy,
                                         m_allocationRound,
                                         nodeIndex,
                                         coreIndex,
                                         !fIdle, /* unused core assignment */
                                         fIdle,  /* idle core lending */
                                         false,  /* borrowed by giving proxy (no giving proxy) */
                                         false); /* idle on giving proxy (no giving proxy) */

#endif
                // Update the global core to reflect state for this scheduler proxy.
                ++pGlobalCore->m_useCount;

                pGlobalCore->m_coreState = ProcessorCore::Unknown;
                if (fIdle)
                {
                    ASSERT(pGlobalNode->m_idleCores != 0);
                    --pGlobalNode->m_idleCores;
                }
                else
                {
                    ASSERT(pGlobalNode->m_availableCores != 0);
                    --pGlobalNode->m_availableCores;
                }

                // Update the allocation map for the scheduler proxy.
                pReceivingProxy->AddCore(&pReceivingProxy->GetAllocatedNodes()[nodeIndex], coreIndex, fIdle);
                if (--numCoresToAssign == 0)
                {
                    return;
                }
            }
        }
        ASSERT(false);
    }

    /// <summary>
    ///     Transfers a fixed number of cores from one scheduler to another.
    /// </summary>
    void ResourceManager::DynamicMigrateCores(DynamicAllocationData * pGivingProxyData, SchedulerProxy * pReceivingProxy, unsigned int nodeIndex, unsigned int numCoresToMigrate)
    {
        SchedulerProxy * pGivingProxy = pGivingProxyData->m_pProxy;
        SchedulerNode * pGivingNodes = pGivingProxy->GetAllocatedNodes();
        SchedulerNode * pGivingNode = &pGivingNodes[nodeIndex];

        ASSERT (numCoresToMigrate > 0 && pGivingNode->GetNumMigratableCores() >= numCoresToMigrate);
        // If there are borrowed cores, we prefer to take those away first. In addition, among borrowed, or owned cores,
        // we prefer to take away idle core if we can find them.
        ASSERT(pGivingNode->m_numBorrowedCores >= pGivingNode->m_numDRMBorrowedIdle);
        ASSERT(pGivingNode->m_numDRMIdle >= pGivingNode->m_numDRMBorrowedIdle);

        // Calculate the number of each type of core we need to migrate.
        unsigned int numBorrowedIdle = min(numCoresToMigrate, pGivingNode->m_numDRMBorrowedIdle);
        unsigned int numBorrowedInUse = min (numCoresToMigrate - numBorrowedIdle, pGivingNode->m_numBorrowedCores - pGivingNode->m_numDRMBorrowedIdle);

        unsigned int remainingCores = numCoresToMigrate - numBorrowedIdle - numBorrowedInUse;
        ASSERT(remainingCores <= pGivingNode->GetNumMigratableCores() - pGivingNode->GetNumBorrowedCores());

        unsigned int numOwnedIdle = min(remainingCores, pGivingNode->m_numDRMIdle - pGivingNode->m_numDRMBorrowedIdle);
        unsigned int numOwnedInUse = remainingCores - numOwnedIdle;

        ASSERT(numOwnedInUse <= (pGivingNode->m_allocatedCores - pGivingNode->m_numBorrowedCores) - (pGivingNode->m_numDRMIdle - pGivingNode->m_numDRMBorrowedIdle));
        ASSERT(numBorrowedIdle + numBorrowedInUse + numOwnedIdle + numOwnedInUse == numCoresToMigrate);

        for (unsigned int coreIndex = 0; coreIndex < pGivingNode->m_coreCount; ++coreIndex)
        {
            bool fMigrateCore = false;
            SchedulerCore * pGivenCore = &pGivingNode->m_pCores[coreIndex];
            if (pGivenCore->m_coreState == ProcessorCore::Allocated && !pGivenCore->IsFixed())
            {
                if (pGivenCore->IsBorrowed())
                {
                    if (pGivenCore->IsIdle() && numBorrowedIdle > 0)
                    {
                        --numBorrowedIdle;
                        fMigrateCore = true;
                    }
                    else if (numBorrowedInUse > 0)
                    {
                        --numBorrowedInUse;
                        fMigrateCore = true;
                    }
                }
                else if (pGivenCore->IsIdle() && numOwnedIdle > 0)
                {
                    --numOwnedIdle;
                    fMigrateCore = true;
                }
                else if (numOwnedInUse > 0)
                {
                    --numOwnedInUse;
                    fMigrateCore = true;
                }

                if (fMigrateCore)
                {
#if defined(CONCRT_TRACING)
                    TraceCoreMigrationAction(pGivingProxy,
                                             pReceivingProxy,
                                             m_allocationRound,
                                             nodeIndex,
                                             coreIndex,
                                             false, /* unused core assignment */
                                             false, /* idle core lending */
                                             pGivenCore->IsBorrowed(), /* borrowed by giving proxy */
                                             pGivenCore->IsIdle()); /* idle on giving proxy */

#endif
                    GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];
                    GlobalCore * pGlobalCore = &(m_pGlobalNodes[nodeIndex].m_pCores[coreIndex]);

                    // The use count on the global core remains unchanged, since we're just migrating a core.
                    bool isIdleCore = pGivenCore->IsIdle();
                    if (isIdleCore)
                    {
                        ToggleRMIdleState(pGivingNode, pGivenCore, pGlobalNode, pGlobalCore, pGivingProxyData);
                    }

                    bool isBorrowedCore = false;
                    if (pGivenCore->IsBorrowed())
                    {
                        isBorrowedCore = true;
                        if (isIdleCore)
                        {
                            ASSERT(pGivingProxyData->m_borrowedIdleCoresToMigrate > 0);
                            --pGivingProxyData->m_borrowedIdleCoresToMigrate;
                        }
                        else
                        {
                            ASSERT(pGivingProxyData->m_borrowedInUseCoresToMigrate > 0);
                            --pGivingProxyData->m_borrowedInUseCoresToMigrate;
                        }
                    }
                    else
                    {
                        ASSERT(pGivingProxyData->m_ownedCoresToMigrate > 0);
                        --pGivingProxyData->m_ownedCoresToMigrate;
                    }

                    // Update the allocation map for the giving scheduler proxy.
                    pGivingProxy->RemoveCore(pGivingNode, coreIndex);

                    // Update the allocation map for the receiving scheduler proxy. If the core we are migrating is borrowed, that state
                    // is transferred to the receiving proxy's core.
                    pReceivingProxy->AddCore(&pReceivingProxy->GetAllocatedNodes()[nodeIndex], coreIndex, isBorrowedCore);

                    if (--numCoresToMigrate == 0)
                    {
                        ASSERT(numBorrowedIdle + numBorrowedInUse + numOwnedIdle + numOwnedInUse == 0);
                        return;
                    }
                }
            }
        }
        ASSERT(false);
    }

    /// <summary>
    ///     Assigns one core at a time to a partially filled node on a receiving scheduler proxy, if possible.
    /// </summary>
    bool ResourceManager::FindCoreForPartiallyFilledNode(unsigned int& unusedCoreQuota,
                                                         unsigned int& usedCoreQuota,
                                                         DynamicAllocationData * pReceivingProxyData,
                                                         unsigned int numGivers)
    {
        bool foundCore = false;

        // Consider one partially filled node in this receiver  Try to find a core on that node, either from
        // the set of unused cores or one of the schedulers that are willing to give up cores. Perform a
        // core transfer to the receiver if one is available, else advance the pointer 'startingNodeIndex'
        // to the next node and return.

        SchedulerNode * pAllocatedNodes = pReceivingProxyData->m_pProxy->GetAllocatedNodes();
        unsigned int * pSortedNodeOrder = pReceivingProxyData->m_pProxy->GetSortedNodeOrder();

        ASSERT(pReceivingProxyData->m_allocation > 0);
        ASSERT(pReceivingProxyData->m_numPartiallyFilledNodes > pReceivingProxyData->m_startingNodeIndex);

        unsigned int nodeIndex = pSortedNodeOrder[pReceivingProxyData->m_startingNodeIndex];
        SchedulerNode * pReceivingNode = &(pAllocatedNodes[nodeIndex]);

        // This node should be a partially filled node
        ASSERT(pReceivingNode->m_allocatedCores > 0 && pReceivingNode->m_allocatedCores < pReceivingNode->m_coreCount);

        if ((unusedCoreQuota > 0) && (m_pGlobalNodes[nodeIndex].m_availableCores> 0))
        {
            foundCore = true;
            // There are unused cores available in this node.
            DynamicAssignCores(pReceivingProxyData->m_pProxy, nodeIndex, 1, false);

            --unusedCoreQuota;
        }
        else if (usedCoreQuota > 0)
        {
            for (unsigned int giv = 0; giv < numGivers && !foundCore; ++giv)
            {
                SchedulerProxy * pGivingProxy = m_ppGivingProxies[giv]->m_pProxy;
                if (pGivingProxy->GetNumAllocatedCores() > m_ppGivingProxies[giv]->m_suggestedAllocation)
                {
                    // Note that we cannot migrate cores in a way that drops the owned core count below MinHWThreads.
                    SchedulerNode * pNodes = pGivingProxy->GetAllocatedNodes();
                    SchedulerNode * pGivingNode = &pNodes[nodeIndex];
                    if ((pGivingNode->GetNumBorrowedIdleCores() > 0) ||
                        (pGivingNode->GetNumBorrowedInUseCores() > 0 && m_ppGivingProxies[giv]->m_borrowedInUseCoresToMigrate > 0) ||
                        (pGivingNode->GetNumMigratableCores() > 0 && m_ppGivingProxies[giv]->m_ownedCoresToMigrate > 0))
                    {
                        ASSERT(pGivingNode->GetNumBorrowedIdleCores() == 0 || m_ppGivingProxies[giv]->m_borrowedIdleCoresToMigrate > 0);

                        foundCore = true;
                        DynamicMigrateCores(m_ppGivingProxies[giv], pReceivingProxyData->m_pProxy, nodeIndex, 1);
                        // Reduce the quota since we've allocated one core.
                        --usedCoreQuota;
                    }
                }
            }
        }

        if (foundCore)
        {
            // If the node is fully allocated, move the node index along.
            if (pReceivingNode->m_allocatedCores == pReceivingNode->m_coreCount)
            {
                ++pReceivingProxyData->m_startingNodeIndex;
            }
            pReceivingProxyData->m_allocation -= 1;
        }
        else
        {
            // We couldn't find any cores for this receiver in the partially filled node in the node we're looking at.
            // Move the starting node index along so that we look at the next partially filled node, if any, during
            // the next iteration.
            ++pReceivingProxyData->m_startingNodeIndex;
        }

        return foundCore;
    }

    /// <summary>
    ///     Attempts to assign cores to a receiver on a single empty node, taking cores from multiple givers, if necessary.
    /// </summary>
    unsigned int ResourceManager::FindBestFitExclusiveAllocation(unsigned int& unusedCoreQuota,
                                                                 unsigned int& usedCoreQuota,
                                                                 DynamicAllocationData * pReceivingProxyData,
                                                                 unsigned int remainingReceivers,
                                                                 unsigned int numTotalGivers)
    {
        SchedulerNode * pAllocatedNodes = pReceivingProxyData->m_pProxy->GetAllocatedNodes();
        unsigned int * pSortedNodeOrder = pReceivingProxyData->m_pProxy->GetSortedNodeOrder();

        // Unless only one receiver is present, we first look for an exact fit while satisfying an allocation.
        // For instance if we find a node with 3 cores available, but the current scheduler proxy needs only 2,
        // we do not allocate out of that node just yet, in case a different scheduler down the line needs
        // exactly two, and we can satisfy those cores out of the node we found. The next time around, we
        // will allocate out of the node that has the most cores available and so on.
        bool exactFit = (remainingReceivers == 1) ? false : pReceivingProxyData->m_fExactFitAllocation;

        unsigned int coresTransferred = 0;

        unsigned int bestFitNodeIndex = static_cast<unsigned int>(-1);
        unsigned int bestFitAllocation = 0;
        unsigned int bestNumGivers = 0;

        // Go through all nodes, starting at startingNodeIndex. The nodes before that node have already been considered.
        for (unsigned int i = pReceivingProxyData->m_startingNodeIndex; i < m_nodeCount; ++i)
        {
            unsigned int nodeIndex = pSortedNodeOrder[i];
            SchedulerNode * pReceivingNode = &(pAllocatedNodes[nodeIndex]);

            // We've already looked at the partially filled nodes on all receivers. The node is either empty or full,
            // proceed only if it is empty.
            ASSERT(pReceivingNode->m_allocatedCores == 0 || pReceivingNode->m_allocatedCores == pReceivingNode->m_coreCount);

            if (pReceivingNode->m_allocatedCores == 0)
            {
                unsigned int currentAllocation = min(pReceivingProxyData->m_allocation, pReceivingNode->m_coreCount);
                ASSERT(currentAllocation > 0);

                // We're going to try to satisfy 'currentAllocation' at this iteration. Find the
                // best fit node that will satisfy currentAllocation.
                unsigned int foundUnusedCores = 0;
                unsigned int numGivers = 0;

                if ((unusedCoreQuota > 0) && (m_pGlobalNodes[nodeIndex].m_availableCores > 0))
                {
                    foundUnusedCores = min(unusedCoreQuota, m_pGlobalNodes[nodeIndex].m_availableCores);
                    ++numGivers;
                }

                unsigned int foundUsedCores = 0;
                if (usedCoreQuota > 0)
                {
                    // Go through the giving proxies.
                    for (unsigned int giv = 0; giv < numTotalGivers && foundUsedCores < usedCoreQuota; ++giv)
                    {
                        if (m_ppGivingProxies[giv]->m_pProxy->GetNumAllocatedCores() > m_ppGivingProxies[giv]->m_suggestedAllocation)
                        {
                            ASSERT((m_ppGivingProxies[giv]->m_pProxy->GetNumAllocatedCores() - m_ppGivingProxies[giv]->m_suggestedAllocation) ==
                                   (m_ppGivingProxies[giv]->m_borrowedIdleCoresToMigrate + m_ppGivingProxies[giv]->m_borrowedInUseCoresToMigrate +
                                    m_ppGivingProxies[giv]->m_ownedCoresToMigrate));
                            // Find the number of cores this proxy can contribute to this node. This is the minimum of
                            //      - the remaining quota
                            //      - the number of migratable cores of the right type (borrowed or owned) the proxy has on this node
                            //      - the number of cores the proxy is able to give up (as suggested by hill climbing)
                            SchedulerNode * pNodes = m_ppGivingProxies[giv]->m_pProxy->GetAllocatedNodes();
                            SchedulerNode * pGivingNode = &pNodes[nodeIndex];
                            if (pGivingNode->GetNumMigratableCores() > 0)
                            {
                                // Caculate the number of cores that actually correspond to the numbers of borrowed and owned cores we are allowed
                                // to migrate from the giving scheduler. For instance, if this node has 3 available owned cores, but
                                // m_ppGivingProxies[giv]->m_ownedCoresToMigrate is 1, we cannot migrate the remaining 2 owned cores.
                                unsigned int numMigratableCores = min(pGivingNode->GetNumBorrowedIdleCores(), m_ppGivingProxies[giv]->m_borrowedIdleCoresToMigrate) +
                                                                  min(pGivingNode->GetNumBorrowedInUseCores(), m_ppGivingProxies[giv]->m_borrowedInUseCoresToMigrate) +
                                                                  min(pGivingNode->GetNumOwnedMigratableCores(), m_ppGivingProxies[giv]->m_ownedCoresToMigrate);

                                ASSERT(numMigratableCores <= m_ppGivingProxies[giv]->m_pProxy->GetNumAllocatedCores() - m_ppGivingProxies[giv]->m_suggestedAllocation);
                                if (numMigratableCores > 0)
                                {
                                    foundUsedCores += min(usedCoreQuota - foundUsedCores, numMigratableCores);
                                    ++numGivers;
                                }
                            }
                        }
                    }
                }

                unsigned int foundAllocation = foundUnusedCores + foundUsedCores;
                if ((exactFit && ((foundAllocation == currentAllocation) && (numGivers > bestNumGivers)))
                        ||
                    (!exactFit && ((bestFitAllocation < currentAllocation && foundAllocation > bestFitAllocation) ||
                                   (foundAllocation == bestFitAllocation && numGivers > bestNumGivers))))
                {
                    bestFitNodeIndex = i;
                    bestFitAllocation = foundAllocation;
                    ASSERT(bestFitAllocation > 0);
                    bestNumGivers = numGivers;
                }
            }
        }

        if (bestFitNodeIndex != -1)
        {
            // Satisfy the allocation at this node
            unsigned int nodeIndex = pSortedNodeOrder[bestFitNodeIndex];
            SchedulerNode * pReceivingNode = &(pAllocatedNodes[nodeIndex]);

            ASSERT(pReceivingNode->m_allocatedCores == 0);
            ASSERT(bestFitAllocation <= pReceivingNode->m_coreCount);

            unsigned int satisfiedAllocation = min(pReceivingProxyData->m_allocation, bestFitAllocation);
            ASSERT(satisfiedAllocation > 0);
            coresTransferred = satisfiedAllocation;

            pReceivingProxyData->m_allocation -= satisfiedAllocation;

            if ((unusedCoreQuota > 0) && (m_pGlobalNodes[nodeIndex].m_availableCores > 0))
            {
                unsigned int coresToTransfer = min(min(unusedCoreQuota, m_pGlobalNodes[nodeIndex].m_availableCores), satisfiedAllocation);
                DynamicAssignCores(pReceivingProxyData->m_pProxy, nodeIndex, coresToTransfer, false);

                ASSERT(unusedCoreQuota >= coresToTransfer);
                ASSERT(satisfiedAllocation >= coresToTransfer);

                unusedCoreQuota -= coresToTransfer;
                satisfiedAllocation -= coresToTransfer;
            }

            // Go through the giving proxies.
            if (satisfiedAllocation > 0 && usedCoreQuota > 0)
            {
                for (unsigned int giv = 0; satisfiedAllocation > 0 && giv < numTotalGivers && usedCoreQuota > 0; ++giv)
                {
                    SchedulerProxy * pGivingProxy = m_ppGivingProxies[giv]->m_pProxy;
                    if (pGivingProxy->GetNumAllocatedCores() > m_ppGivingProxies[giv]->m_suggestedAllocation)
                    {
                        SchedulerNode * pNodes = pGivingProxy->GetAllocatedNodes();
                        SchedulerNode * pGivingNode = &pNodes[nodeIndex];
                        if (pGivingNode->GetNumMigratableCores() > 0)
                        {
                            ASSERT((m_ppGivingProxies[giv]->m_pProxy->GetNumAllocatedCores() - m_ppGivingProxies[giv]->m_suggestedAllocation) ==
                                   (m_ppGivingProxies[giv]->m_borrowedIdleCoresToMigrate + m_ppGivingProxies[giv]->m_borrowedInUseCoresToMigrate +
                                    m_ppGivingProxies[giv]->m_ownedCoresToMigrate));

                            // Find the number of cores we will take from this proxy to satisfy an allocation on this node. This is the minimum of
                            //      - the remaining quota.
                            //      - the number of migratable cores of the right type (borrowed or owned) the proxy has on this node
                            //      - the number of cores the proxy is able to give up (as suggested by hill climbing)
                            //      - the remaining requirement

                            // Caculate the number of cores that actually correspond to the numbers of borrowed and owned cores we are allowed
                            // to migrate from the giving scheduler. For instance, if this node has 3 available owned cores, but
                            // m_ppGivingProxies[giv]->m_ownedCoresToMigrate is 1, we cannot migrate the remaining 2 owned cores.
                            unsigned int numMigratableCores = min(pGivingNode->GetNumBorrowedIdleCores(), m_ppGivingProxies[giv]->m_borrowedIdleCoresToMigrate) +
                                                              min(pGivingNode->GetNumBorrowedInUseCores(), m_ppGivingProxies[giv]->m_borrowedInUseCoresToMigrate) +
                                                              min(pGivingNode->GetNumOwnedMigratableCores(), m_ppGivingProxies[giv]->m_ownedCoresToMigrate);

                            if (numMigratableCores > 0)
                            {
                                ASSERT(pGivingNode->m_numBorrowedCores >= pGivingNode->m_numDRMBorrowedIdle);
                                ASSERT(numMigratableCores <= m_ppGivingProxies[giv]->m_pProxy->GetNumAllocatedCores() - m_ppGivingProxies[giv]->m_suggestedAllocation);

                                unsigned int coresToTransfer = min(min(usedCoreQuota, numMigratableCores), satisfiedAllocation);

                                ASSERT(coresToTransfer > 0);
                                DynamicMigrateCores(m_ppGivingProxies[giv], pReceivingProxyData->m_pProxy, nodeIndex, coresToTransfer);

                                ASSERT(usedCoreQuota >= coresToTransfer);
                                ASSERT(satisfiedAllocation >= coresToTransfer);

                                usedCoreQuota -= coresToTransfer;
                                satisfiedAllocation -= coresToTransfer;
                            }
                        }
                    }
                }
                ASSERT(satisfiedAllocation == 0);
            }

            // Move the startingNodeIndex up by 1, and push the node we have just populated back, so that we do not consider it
            // during the next iteration (if any).
            unsigned int tempIndex = pSortedNodeOrder[pReceivingProxyData->m_startingNodeIndex];
            pSortedNodeOrder[pReceivingProxyData->m_startingNodeIndex] = pSortedNodeOrder[bestFitNodeIndex];
            pSortedNodeOrder[bestFitNodeIndex] = tempIndex;

            ++pReceivingProxyData->m_startingNodeIndex;

            ASSERT(pReceivingProxyData->m_startingNodeIndex <= m_nodeCount);
            ASSERT(pReceivingProxyData->m_startingNodeIndex < m_nodeCount || pReceivingProxyData->m_allocation == 0);

            pReceivingProxyData->m_fExactFitAllocation = true;
        }
        else
        {
            ASSERT(exactFit);
            pReceivingProxyData->m_fExactFitAllocation = false;
        }

        return coresTransferred;
    }

    /// <summary>
    ///     Distributes unused cores and cores from scheduler proxies that are willing to give up cores to scheduler proxies that
    ///     need cores.
    /// </summary>
    void ResourceManager::DistributeExclusiveCores(unsigned int totalCoresNeeded,
                                                   unsigned int unusedCoreQuota,
                                                   unsigned int usedCoreQuota,
                                                   unsigned int numReceivers,
                                                   unsigned int numGivers)
    {
        // The array of receiving proxies is arranged by partial nodes (min to max), In addition, in each proxy with partially filled
        // nodes, the nodes are arranged with the most tightly packed nodes earlier in the sorted node array.

        // Go through the array of receivers, transferring one core at a time to a partially filled node on the scheduler proxy
        // if possible.
        bool foundPartialNode = false;

        do
        {
            foundPartialNode = false;
            for (unsigned int rec = 0; rec < numReceivers; ++rec)
            {
                // Allocate one core to each receiver at a time, to ensure some fairness among receivers if multiple receivers
                // have the same node partially filled.
                if ((m_ppReceivingProxies[rec]->m_allocation > 0) &&
                    (m_ppReceivingProxies[rec]->m_numPartiallyFilledNodes > m_ppReceivingProxies[rec]->m_startingNodeIndex))
                {
                    foundPartialNode = true;

                    if (FindCoreForPartiallyFilledNode(unusedCoreQuota, /* passed by reference */
                                                       usedCoreQuota, /* passed by reference */
                                                       m_ppReceivingProxies[rec],
                                                       numGivers))
                    {
                        --totalCoresNeeded;
                    }
                }
            }
        }
        while (foundPartialNode);

        if (totalCoresNeeded > 0)
        {
            unsigned int remainingReceivers = numReceivers;
            // Sort the array of receivers by number of cores needed first, highest first.
            for (unsigned int i = 0; i < numReceivers; ++i)
            {
                unsigned int maxIndex = i;
                for (unsigned int j = i + 1; j < numReceivers; ++j)
                {
                    if (m_ppReceivingProxies[j]->m_allocation > m_ppReceivingProxies[maxIndex]->m_allocation)
                    {
                        maxIndex = j;
                    }
                }
                if (i != maxIndex)
                {
                    DynamicAllocationData * pTemp = m_ppReceivingProxies[i];
                    m_ppReceivingProxies[i] = m_ppReceivingProxies[maxIndex];
                    m_ppReceivingProxies[maxIndex] = pTemp;
                }
                if (m_ppReceivingProxies[i]->m_allocation == 0)
                {
                    // We can stop looking, since all receivers after this point have 'allocation' equal to 0.
                    remainingReceivers = i;
                    break;
                }
            }

            numReceivers = remainingReceivers;
            ASSERT(numReceivers > 0);
            // Now for each receiver, try to satsify cores on an unallocated node.
            do
            {
                // Go through the remaining receivers and try to satisfy as many cores on an unallocated node as possible.
                for (unsigned int rec = 0; rec < numReceivers; ++rec)
                {
                    DynamicAllocationData * pReceivingProxyData = m_ppReceivingProxies[rec];
                    if (pReceivingProxyData->m_allocation > 0)
                    {
                        totalCoresNeeded -= FindBestFitExclusiveAllocation(unusedCoreQuota, /* passed by reference */
                                                                           usedCoreQuota, /* passed by reference */
                                                                           pReceivingProxyData,
                                                                           remainingReceivers,
                                                                           numGivers);

                        if (pReceivingProxyData->m_allocation == 0)
                        {
                            --remainingReceivers;
                        }
                    }
                }
            }
            while (totalCoresNeeded > 0);

            ASSERT(remainingReceivers == 0);
        }
    }

    /// <summary>
    ///     Attempts to assign cores to a receiver on a single empty node, using idle cores.
    /// </summary>
    unsigned int ResourceManager::FindBestFitIdleAllocation(unsigned int idleCoresAvailable, DynamicAllocationData * pReceivingProxyData, unsigned int remainingReceivers)
    {
        SchedulerNode * pAllocatedNodes = pReceivingProxyData->m_pProxy->GetAllocatedNodes();
        unsigned int * pSortedNodeOrder = pReceivingProxyData->m_pProxy->GetSortedNodeOrder();

        // Unless only one receiver is present, we first look for an exact fit while satisfying an allocation.
        // For instance if we find a node with 3 cores available, but the current scheduler proxy needs only 2,
        // we do not allocate out of that node just yet, in case a different scheduler down the line needs
        // exactly 3, and we can satisfy those cores out of the node we found. The next time around, we
        // will allocate out of the node that has the most cores available and so on.
        bool exactFit = (remainingReceivers == 1) ? false : pReceivingProxyData->m_fExactFitAllocation;

        unsigned int coresTransferred = 0;

        unsigned int bestFitNodeIndex = static_cast<unsigned int>(-1);
        unsigned int bestFitAllocation = 0;

        // Go through all nodes, starting at startingNodeIndex. The nodes before that node have already been considered.
        for (unsigned int i = pReceivingProxyData->m_startingNodeIndex; i < m_nodeCount; ++i)
        {
            unsigned int nodeIndex = pSortedNodeOrder[i];
            SchedulerNode * pReceivingNode = &(pAllocatedNodes[nodeIndex]);

            ASSERT(pReceivingNode->m_allocatedCores == 0 || pReceivingNode->m_allocatedCores == pReceivingNode->m_coreCount);

            if (pReceivingNode->m_allocatedCores == 0)
            {
                unsigned int currentAllocation = min (pReceivingProxyData->m_allocation, pReceivingNode->m_coreCount);

                // We're going to try to satisfy 'currentAllocation' at this iteration. Find the
                // best fit node that will satisfy currentAllocation.
                unsigned int foundAllocation = 0;

                if (m_pGlobalNodes[nodeIndex].m_idleCores > 0)
                {
                    foundAllocation = min(idleCoresAvailable, m_pGlobalNodes[nodeIndex].m_idleCores);
                }

                if ((exactFit && (foundAllocation == currentAllocation))
                        ||
                    (!exactFit && (foundAllocation > bestFitAllocation)))
                {
                    bestFitNodeIndex = i;
                    bestFitAllocation = foundAllocation;

                    if (exactFit)
                    {
                        // We can immediately satisfy this request.
                        break;
                    }
                }
            }
        }

        if (bestFitNodeIndex != -1)
        {
            // Satisfy the allocation at this node
            unsigned int nodeIndex = pSortedNodeOrder[bestFitNodeIndex];
            SchedulerNode * pReceivingNode = &(pAllocatedNodes[nodeIndex]);

            ASSERT(pReceivingNode->m_allocatedCores == 0);
            ASSERT(bestFitAllocation <= pReceivingNode->m_coreCount);

            coresTransferred = min(pReceivingProxyData->m_allocation, bestFitAllocation);
            ASSERT(coresTransferred > 0);

            ASSERT (m_pGlobalNodes[nodeIndex].m_idleCores >= coresTransferred);

            DynamicAssignCores(pReceivingProxyData->m_pProxy, nodeIndex, coresTransferred, true);

            ASSERT(pReceivingProxyData->m_allocation >= coresTransferred);
            pReceivingProxyData->m_allocation -= coresTransferred;

            // Move the startingNodeIndex up by 1, and push the node we have just populated back, so that we do not consider it
            // during the next iteration (if any).
            unsigned int tempIndex = pSortedNodeOrder[pReceivingProxyData->m_startingNodeIndex];
            pSortedNodeOrder[pReceivingProxyData->m_startingNodeIndex] = pSortedNodeOrder[bestFitNodeIndex];
            pSortedNodeOrder[bestFitNodeIndex] = tempIndex;

            ++pReceivingProxyData->m_startingNodeIndex;

            ASSERT(pReceivingProxyData->m_startingNodeIndex <= m_nodeCount);
            ASSERT(pReceivingProxyData->m_startingNodeIndex < m_nodeCount || pReceivingProxyData->m_allocation == 0);

            pReceivingProxyData->m_fExactFitAllocation = true;
        }
        else
        {
            ASSERT(exactFit);
            pReceivingProxyData->m_fExactFitAllocation = false;
        }

        return coresTransferred;
    }

    /// <summary>
    ///     Distributes idle cores to scheduler proxies that need cores.
    /// </summary>
    void ResourceManager::DistributeIdleCores(unsigned int totalCoresNeeded, unsigned int numReceivers)
    {
        // The array of receiving proxies is arranged by partial nodes (min to max), In addition, in each proxy with partially filled
        // nodes, the nodes are arranged with the most tightly packed nodes earlier in the sorted node array.

        // Go through the array of receivers, transferring one core at a time to a partially filled node on the scheduler proxy
        // if possible.
        bool foundPartialNode = false;

        do
        {
            foundPartialNode = false;
            for (unsigned int rec = 0; rec < numReceivers && totalCoresNeeded > 0; ++rec)
            {
                DynamicAllocationData * pReceivingProxyData = m_ppReceivingProxies[rec];
                // Allocate one core to each receiver at a time, to ensure some fairness among receivers if multiple receivers
                // have the same node partially filled.
                if ((pReceivingProxyData->m_allocation > 0) &&
                    (pReceivingProxyData->m_numPartiallyFilledNodes > pReceivingProxyData->m_startingNodeIndex))
                {
                    foundPartialNode = true;

                    unsigned int * pSortedNodeOrder = pReceivingProxyData->m_pProxy->GetSortedNodeOrder();
                    unsigned int nodeIndex = pSortedNodeOrder[pReceivingProxyData->m_startingNodeIndex];

                    SchedulerNode * pAllocatedNodes = pReceivingProxyData->m_pProxy->GetAllocatedNodes();
                    SchedulerNode * pReceivingNode = &(pAllocatedNodes[nodeIndex]);

                    // The first sorted node should be a partially filled node
                    ASSERT(pReceivingNode->m_allocatedCores > 0 && pReceivingNode->m_allocatedCores < pReceivingNode->m_coreCount);

                    if (m_pGlobalNodes[nodeIndex].m_idleCores > 0)
                    {
                        // There are idle cores available in this node.
                        DynamicAssignCores(pReceivingProxyData->m_pProxy, nodeIndex, 1, true);

                        // If the node is fully allocated, move the starting node index along.
                        if (pReceivingNode->m_allocatedCores == pReceivingNode->m_coreCount)
                        {
                            ++pReceivingProxyData->m_startingNodeIndex;
                        }
                        pReceivingProxyData->m_allocation -= 1;
                        --totalCoresNeeded;
                    }
                    else
                    {
                        // We couldn't find any cores for this receiver in the partially filled node in the node we're looking at.
                        // Move the starting node index along so that we look at the next partially filled node, if any, during
                        // the next iteration.
                        ++pReceivingProxyData->m_startingNodeIndex;
                    }
                }
            }
        }
        while (foundPartialNode);

        if (totalCoresNeeded > 0)
        {
            unsigned int remainingReceivers = numReceivers;

            // Sort the array of receivers by number of cores needed first, highest first.
            for (unsigned int i = 0; i < numReceivers; ++i)
            {
                unsigned int maxIndex = i;
                for (unsigned int j = i + 1; j < numReceivers; ++j)
                {
                    if (m_ppReceivingProxies[j]->m_allocation > m_ppReceivingProxies[maxIndex]->m_allocation)
                    {
                        maxIndex = j;
                    }
                }
                if (i != maxIndex)
                {
                    DynamicAllocationData * pTemp = m_ppReceivingProxies[i];
                    m_ppReceivingProxies[i] = m_ppReceivingProxies[maxIndex];
                    m_ppReceivingProxies[maxIndex] = pTemp;
                }
                if (m_ppReceivingProxies[i]->m_allocation == 0)
                {
                    // We can stop looking, since all receivers after this point have 'allocation' equal to 0.
                    remainingReceivers = i;
                    break;
                }
            }

            numReceivers = remainingReceivers;
            ASSERT(numReceivers > 0);
            // Now for each receiver, try to satsify cores on an unallocated node.
            do
            {
                for (unsigned int rec = 0; rec < numReceivers && totalCoresNeeded > 0; ++rec)
                {
                    DynamicAllocationData * pReceivingProxyData = m_ppReceivingProxies[rec];
                    if (pReceivingProxyData->m_allocation > 0)
                    {
                        totalCoresNeeded -= FindBestFitIdleAllocation(totalCoresNeeded,
                                                                      pReceivingProxyData,
                                                                      remainingReceivers);
                        if (pReceivingProxyData->m_allocation == 0)
                        {
                            --remainingReceivers;
                        }
                    }
                }
            }
            while (totalCoresNeeded > 0);
            ASSERT(remainingReceivers == 0);
        }
    }

    /// <summary>
    ///     Does a dynamic resource allocation based on feedback from hill climbing.
    /// </summary>
    void ResourceManager::DoCoreMigration()
    {
        TRACE(CONCRT_TRACE_DYNAMIC_RM, L"---------------------------------------------------------------------------\n");

        // Capture data needed for dynamic allocation for all existing schedulers. This includes gathering statistics
        // and invoking a per scheduler hill climbing instance to get a suggested future allocation.
        PopulateDynamicAllocationData();

        // Handle a subset of idle, borrowed and shared cores up front.
        PreProcessDynamicAllocationData();

        // Exclusive cores are cores that other schedulers can give up (not-shared) or cores that are unused by any scheduler.
        unsigned int exclusiveCoresAvailable = 0;
        // Used cores are cores that are assigned to other schedulers, but are up for grabs, because hill climbing or idle core
        // information has indicated to us that those schedulers can do without them.
        unsigned int usedCoresAvailable = 0;
        unsigned int numGivers = 0;

        // Find schedulers that are able to give up cores.
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            DynamicAllocationData * pDynamicData = static_cast<DynamicAllocationData *>(m_ppProxyData[index]);
            // For all priorities, get the schedulers that we can take cores away from.
            if (pDynamicData->m_pProxy->GetNumAllocatedCores() > pDynamicData->m_suggestedAllocation)
            {
                // Borrowed cores can be migrated as well. Clearly if the owning scheduler was using the borrowed core, the scheduler
                // would not still have it. Therefore, the owning scheduler is idle on the core, and if a borrowed core is migrated
                // the receiver also marks it as 'borrowed'. This also means that the same core can be migrated twice - if two schedulers
                // have borrowed that core.
                m_ppGivingProxies[numGivers++] = pDynamicData;
                usedCoresAvailable += pDynamicData->m_pProxy->GetNumAllocatedCores() - pDynamicData->m_suggestedAllocation;

                // Find out how many borrowed cores and owned cores this scheduler should give. We first prefer to transfer
                // borrowed cores before transferring owned cores. Note that all borrowed idle cores should be migrated.
                ASSERT(pDynamicData->m_numBorrowedIdleCores <= pDynamicData->m_pProxy->GetNumBorrowedCores());
                pDynamicData->m_borrowedIdleCoresToMigrate = min(pDynamicData->m_numBorrowedIdleCores,
                                                                     pDynamicData->m_pProxy->GetNumAllocatedCores() - pDynamicData->m_suggestedAllocation);

                pDynamicData->m_borrowedInUseCoresToMigrate = min(pDynamicData->m_pProxy->GetNumBorrowedCores() - pDynamicData->m_numBorrowedIdleCores,
                                                                      pDynamicData->m_pProxy->GetNumAllocatedCores() - pDynamicData->m_suggestedAllocation -
                                                                      pDynamicData->m_borrowedIdleCoresToMigrate);
                ASSERT(pDynamicData->m_borrowedIdleCoresToMigrate + pDynamicData->m_borrowedInUseCoresToMigrate <= pDynamicData->m_pProxy->GetNumBorrowedCores());

                pDynamicData->m_ownedCoresToMigrate = pDynamicData->m_pProxy->GetNumAllocatedCores() - pDynamicData->m_suggestedAllocation -
                                                                pDynamicData->m_borrowedIdleCoresToMigrate - pDynamicData->m_borrowedInUseCoresToMigrate;
                ASSERT(pDynamicData->m_pProxy->GetNumOwnedCores() - pDynamicData->m_ownedCoresToMigrate >= pDynamicData->m_pProxy->MinHWThreads());
            }
            else
            {
                ASSERT(pDynamicData->m_numBorrowedIdleCores == 0);
            }
        }

        // Find available cores (cores not assigned to any scheduler), and mark them as reserved.
        unsigned int unusedCoresAvailable = 0;

        // Find cores that are idle, i.e, all schedulers that have that core assigned are not using them at present.
        // We are able to temporarily share these cores with schedulers that indicate that they need cores.
        m_dynamicIdleCoresAvailable = 0;

        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            GlobalNode * pGlobalNode = &m_pGlobalNodes[nodeIndex];
            for (unsigned int coreIndex = 0; coreIndex < pGlobalNode->m_coreCount; ++coreIndex)
            {
                GlobalCore * pGlobalCore = &pGlobalNode->m_pCores[coreIndex];
                if (pGlobalCore->m_useCount == 0)
                {
                    pGlobalCore->m_coreState = ProcessorCore::Available;
                    ++pGlobalNode->m_availableCores;
                    ++unusedCoresAvailable;
                }
                else if (pGlobalCore->m_useCount == pGlobalCore->m_idleSchedulers)
                {
                    pGlobalCore->m_coreState = ProcessorCore::Idle;
                    ++pGlobalNode->m_idleCores;
                    // Calculate the total number of idle cores up front. This number could change as we transfer cores between schedulers,
                    // and will be updated as we go along.
                    ++m_dynamicIdleCoresAvailable;
                }
            }
        }

        exclusiveCoresAvailable = usedCoresAvailable + unusedCoresAvailable;

        // Perform two rounds of allocation/migration.
        // Round 1 : Only consider receivers whose suggested allocation (as given by hill climbing) is higher than their allocated
        // number of cores. After we have exhauted all such receivers, find fully loaded schedulers, and raise their suggested allocation to
        // their desired.
        // Round 2 : If cores are still available do a second round of migration to the new receivers if any.
        for (m_allocationRound = 0; (exclusiveCoresAvailable > 0 || m_dynamicIdleCoresAvailable > 0) && m_allocationRound < 2; ++m_allocationRound)
        {
            if (m_allocationRound == 1)
            {
                // This is the second round of allocation. We have already satisfied the increases that hill climbing recommended.
                // Now we try to find other schedulers who may benefit from resources - since we have some available to give.
                IncreaseFullyLoadedSchedulerAllocations();
            }

            unsigned int numReceivers = 0;
            unsigned int coresNeeded = 0;

            for (unsigned int index = 0; index < m_numSchedulers; ++index)
            {
                // Check if there are schedulers that we need to give resources to.
                DynamicAllocationData * pDynamicData = static_cast<DynamicAllocationData *>(m_ppProxyData[index]);
                if (pDynamicData->m_pProxy->GetNumAllocatedCores() < pDynamicData->m_suggestedAllocation)
                {
                    m_ppReceivingProxies[numReceivers++] = pDynamicData;
                    coresNeeded += pDynamicData->m_suggestedAllocation - pDynamicData->m_pProxy->GetNumAllocatedCores();
                }
            }

            if (numReceivers > 0)
            {
                ASSERT(coresNeeded > 0);

                // First check for unused cores and cores we can steal from other schedulers. We differentiate between exclusive cores
                // and idle cores because we first want to satisfy requests using either unused cores or cores other schedulers can give up.
                if (exclusiveCoresAvailable > 0)
                {
                    // AdjustDynamicAllocation populates the 'allocation' field of the dynamic data that represents the additional cores we
                    // must give the scheduler. It is guaranteed that we can satisfy all allocations since they will be reduced if the
                    // sum of requested allocations was greater than what was available.
                    unsigned int coresToTransfer = AdjustDynamicAllocation(exclusiveCoresAvailable, coresNeeded, numReceivers);

                    // Find the number of receivers that will still be granted cores (the AdjustDynamicAllocation API above could've reduced
                    // suggested allocations for some receivers), and sort the receivers by number of partially filled nodes.
                    unsigned int exclusiveCoreReceivers = PrepareReceiversForCoreTransfer(numReceivers);

                    // 'coresTransferred' is the total number of cores we are about to distribute among the receivers in the receiving proxy
                    // array. The order in which we give cores is important. We must first give receivers unused cores, then cores taken from
                    // other schedulers, and finally, idle cores.

                    unsigned int unusedCoreQuota = 0;
                    unsigned int usedCoreQuota = 0;
                    unsigned int coresDistributed = 0;

                    coresDistributed = unusedCoreQuota = min(unusedCoresAvailable, coresToTransfer);

                    ASSERT(unusedCoresAvailable >= unusedCoreQuota);
                    unusedCoresAvailable -= unusedCoreQuota;

                    if (coresDistributed < coresToTransfer)
                    {
                        unsigned int remainingCores = coresToTransfer - coresDistributed;

                        usedCoreQuota = min(remainingCores, usedCoresAvailable);
                        coresDistributed += usedCoreQuota;
                        usedCoresAvailable -= usedCoreQuota;
                    }
                    ASSERT(coresDistributed == coresToTransfer);

                    DistributeExclusiveCores(coresToTransfer, unusedCoreQuota, usedCoreQuota, exclusiveCoreReceivers, numGivers);

                    ASSERT(exclusiveCoresAvailable >= coresToTransfer);
                    exclusiveCoresAvailable -= coresToTransfer;

                    ASSERT(coresNeeded >= coresToTransfer);
                    coresNeeded -= coresToTransfer;
                } // end of if (exclusiveCoresAvailable > 0)

                // Now check if any more requests need to be satisfied. The reason we do this in two stages, (first unused and stolen
                // cores, followed by idle cores), is that we want to distribute idle cores evenly, since we're temporarily oversubscribing them, and
                // they could easy be taken away at the next iteration, if the schedulers that were not using the cores start using them.

                if (coresNeeded > 0 && m_dynamicIdleCoresAvailable > 0)
                {
                    ASSERT(unusedCoresAvailable == 0);
                    ASSERT(usedCoresAvailable == 0);
                    ValidateDRMSchedulerState();

                    // AdjustDynamicAllocation populates the 'allocation' field of the dynamic data that represents the additional cores we
                    // must give the scheduler. It is guaranteed that we can satisfy all allocations since they will be reduced if the
                    // sum of requested allocations was greater than what was available.
                    unsigned int coresToTransfer = AdjustDynamicAllocation(m_dynamicIdleCoresAvailable, coresNeeded, numReceivers);

                    // Find the number of receivers that will still be granted cores (the AdjustDynamicAllocation API above could've reduced
                    // suggested allocations for some receivers), and sort the receivers by number of partially filled nodes.
                    unsigned int idleCoreReceivers = PrepareReceiversForCoreTransfer(numReceivers);

                    DistributeIdleCores(coresToTransfer, idleCoreReceivers);

                    ASSERT(m_dynamicIdleCoresAvailable >= coresToTransfer && coresNeeded >= coresToTransfer);

                    ValidateDRMSchedulerState();
                    m_dynamicIdleCoresAvailable -= coresToTransfer;
                    coresNeeded -= coresToTransfer;
                } // end of if (coresNeeded > 0 && m_dynamicIdleCoresAvailable > 0)
            } // end of if (numReceivers > 0)
        }

        // Undo any global state we initialized for dynamic core migration.
        ResetGlobalAllocationData();

#if defined(_DEBUG)
        // Ensure that allocations are between min and max for all schedulers after a core migration pass
        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            SchedulerProxy * pSchedulerProxy = m_ppProxyData[index]->m_pProxy;
            ASSERT(pSchedulerProxy->GetNumOwnedCores() >= pSchedulerProxy->MinHWThreads());
            ASSERT(pSchedulerProxy->GetNumAllocatedCores() <= pSchedulerProxy->DesiredHWThreads());
        }
#endif

#if defined(CONCRT_TRACING)

        for (unsigned int index = 0; index < m_numSchedulers; ++index)
        {
            DynamicAllocationData * pDynamicData = static_cast<DynamicAllocationData *>(m_ppProxyData[index]);
            TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Scheduler %d[min=%d, max=%d]: Final values   - Allocated: %d, Suggested: %d, Idle: %d, Borrowed: %d",
                pDynamicData->m_pProxy->GetId(), pDynamicData->m_pProxy->MinHWThreads(), pDynamicData->m_pProxy->DesiredHWThreads(),
                pDynamicData->m_pProxy->GetNumAllocatedCores(), pDynamicData->m_suggestedAllocation, pDynamicData->m_numIdleCores,
                pDynamicData->m_pProxy->GetNumBorrowedCores());
        }

#endif

        TRACE(CONCRT_TRACE_DYNAMIC_RM, L"DRM:Remaining unused: %d, Remaining idle %d", unusedCoresAvailable, m_dynamicIdleCoresAvailable);
        TRACE(CONCRT_TRACE_DYNAMIC_RM, L"---------------------------------------------------------------------------\n");
    }

    /// <summary>
    ///     Returns the global subscription level of the underlying core.
    /// </summary>
    unsigned int ResourceManager::CurrentSubscriptionLevel(unsigned int nodeId, unsigned int coreIndex)
    {
        unsigned int currentGlobalSubscription = 0;
        { // begin locked region
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);
            // Walk the collection of schedulers and capture subscription information.
            for (SchedulerProxy * pSchedulerProxy = m_schedulers.First(); pSchedulerProxy != NULL; pSchedulerProxy = m_schedulers.Next(pSchedulerProxy))
            {
                SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
                SchedulerCore * pAllocatedCore = &pAllocatedNodes[nodeId].m_pCores[coreIndex];
                currentGlobalSubscription += pAllocatedCore->m_subscriptionLevel;
            }
        } // end locked region

        return currentGlobalSubscription;
    }

    /// <summary>
    ///     Sends NotifyResourcesExternallyIdle/NotifyResourcesExternallyBusy notifications to the schedulers that
    ///     qualify for them, to let them know that the hardware resources allocated to them are in use or out of use
    ///     by other schedulers that share those resources.
    /// </summary>
    /// <param name ="pNewlyAllocatedProxy">
    ///     The newly allocated scheduler proxy, if one was just allocated.
    /// </param>
    void ResourceManager::SendResourceNotifications(SchedulerProxy * pNewlyAllocatedProxy)
    {
        // We need to do this in two stages. First capture local subscription data for each scheduler and calculate
        // the global subscription data per core. Then send notifications, if needed.

        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            for (unsigned int coreIndex = 0; coreIndex < m_pGlobalNodes[nodeIndex].m_coreCount; ++coreIndex)
            {
                GlobalCore * pGlobalCore = &m_pGlobalNodes[nodeIndex].m_pCores[coreIndex];
                pGlobalCore->m_previousSubscriptionLevel = pGlobalCore->m_currentSubscriptionLevel;
                pGlobalCore->m_currentSubscriptionLevel = 0;

                // Walk the collection of schedulers and capture subscription information.
                for (SchedulerProxy * pSchedulerProxy = m_schedulers.First(); pSchedulerProxy != NULL; pSchedulerProxy = m_schedulers.Next(pSchedulerProxy))
                {
                    SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
                    SchedulerCore * pAllocatedCore = &pAllocatedNodes[nodeIndex].m_pCores[coreIndex];
                    pAllocatedCore->m_previousSubscriptionLevel = pAllocatedCore->m_currentSubscriptionLevel;
                    pAllocatedCore->m_currentSubscriptionLevel = pAllocatedCore->m_subscriptionLevel;
                    pGlobalCore->m_currentSubscriptionLevel += pAllocatedCore->m_currentSubscriptionLevel;
                }
            }
        }

        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            for (unsigned int coreIndex = 0; coreIndex < m_pGlobalNodes[nodeIndex].m_coreCount; ++coreIndex)
            {
                GlobalCore * pGlobalCore = &m_pGlobalNodes[nodeIndex].m_pCores[coreIndex];

                ULONG previousGlobal = pGlobalCore->m_previousSubscriptionLevel;
                ULONG currentGlobal = pGlobalCore->m_currentSubscriptionLevel;

                // Walk the collection of schedulers and send notifications if applicable.
                for (SchedulerProxy * pSchedulerProxy = m_schedulers.First(); pSchedulerProxy != NULL; pSchedulerProxy = m_schedulers.Next(pSchedulerProxy))
                {
                    if (pSchedulerProxy->ShouldReceiveNotifications())
                    {
                        SchedulerNode * pAllocatedNodes = pSchedulerProxy->GetAllocatedNodes();
                        SchedulerCore * pAllocatedCore = &pAllocatedNodes[nodeIndex].m_pCores[coreIndex];
                        ULONG previousLocal = pAllocatedCore->m_previousSubscriptionLevel;
                        ULONG currentLocal = pAllocatedCore->m_currentSubscriptionLevel;
                        ASSERT(previousGlobal >= previousLocal);
                        ASSERT(currentGlobal >= currentLocal);

                        if (pAllocatedCore->m_numAssignedThreads > 0)
                        {
                            if (pSchedulerProxy == pNewlyAllocatedProxy)
                            {
                                if (currentGlobal > currentLocal)
                                {
                                    // Ther are other scheduler proxies using this core.
                                    pSchedulerProxy->SendCoreNotification(pAllocatedCore, true /* isBusy */);
                                }
                                else
                                {
                                    // No one else is using this core at present.
                                    pSchedulerProxy->SendCoreNotification(pAllocatedCore, false /* isIdle */);
                                }
                            }
                            else
                            {
                                if (previousGlobal == previousLocal && currentGlobal > currentLocal)
                                {
                                    // If this scheduler proxy WAS the only thing running on this core, AND
                                    // there is another scheduler proxy that IS contributing to the current number,
                                    // THEN: Notify that the resources underneath virtual processors are now busy.
                                    pSchedulerProxy->SendCoreNotification(pAllocatedCore, true /* isBusy */);
                                }
                                else if (currentGlobal == currentLocal && previousGlobal > previousLocal)
                                {
                                    // If this scheduler proxy IS the only thing running on this core, AND
                                    // it WAS sharing the core with another scheduler proxy,
                                    // THEN: Notify that the resources underneath virtual processors are now idle.
                                    pSchedulerProxy->SendCoreNotification(pAllocatedCore, false /* isIdle */);
                                }
                            }
                        } // if (pAllocatedCore->m_numAssignedThreads > 0)
                    }
                } // end of for (.. scheduler proxy..
            } // end of for (.. core..)
        } // end of for (.. node..)
    }

    /// <summary>
    ///     Routine that performs dynamic resource management among existing schedulers at fixed time intervals.
    /// </summary>
    void ResourceManager::DynamicResourceManager()
    {
        const DWORD DynamicRMTimeInterval = 100;
        DWORD timeout = DynamicRMTimeInterval;

        // The dynamic RM thread waits on an event with a fixed timeout. It can be woken up from the wait for one of the following
        // reasons:
        // 1) TIMEOUT - the DRM thread should perform core migration and send any pending notifications.
        // 2) Number of schedulers just went from 1 to 2. The state is expected to be set to 'LoadBalance'. The DRM thread may have just been
        //      created, or woken up from a long wait. It needs to discard existing scheduler statistics, and set the next time interval
        //      appropriately.
        // 3) Number of schedulers just went from 2 to 1. The state is expected to be set to 'Standby'. The DRM thread should go into a infinite
        //      wait. Before this, it redistributes cores to the surviving scheduler if that scheduler doesn't already have all the cores it wants.
        // 4) An activation or a deactivation occured that could warrant core idle/busy notifications for existing schedulers. The state could
        //      be either standby or LoadBalance. If it is 'Standby', we likely raced with the last scheduler leaving. Since we cannot differentiate
        //      this from 2), we do exactly the same as 2). If the state is 'LoadBalance', someone signaled us to send notifications.
        // 5) RM is exiting and the DRM thread needs to quit.

        // We've just entered the function, simulate a longish wait by subtracting from the current tick count.
        ULONGLONG oldTickCount = (platform::__GetTickCount64() - (ULONGLONG)500);
        ULONGLONG newTickCount = 0;

        while (m_dynamicRMWorkerState != Exit)
        {
            DWORD retVal = WaitForSingleObjectEx(m_hDynamicRMEvent, timeout, FALSE);

            { // begin locked region
                _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

                // The DRM thread event is signaled for one of 4 reasons.
                //  The RM is shutting down, and the thread needs to exit - handled above
                //  The thread needs to go into standby.
                //  The thread needs to start load balancing at fixed intervals.
                //  The thread needs to send notifications
                switch (m_dynamicRMWorkerState)
                {
                case Standby:
                {
                    // We're holding the lock, and the state is Standby. There should be only one
                    // scheduler the RM knows about at this time.
                    ASSERT(m_numSchedulers <= 1);
                    if (DistributeCoresToSurvivingScheduler())
                    {
                        timeout = INFINITE;
                    }
                    else
                    {
                        // We might fail distributing cores to a scheduler if
                        // it has yet to be retired vprocs on cores that were
                        // removed previously. Since there is no DRM, we need
                        // to retry until the scheduler has the desired number
                        // of hardware threads.
                        timeout = DynamicRMTimeInterval;
                    }
                    break;
                }

                case LoadBalance:
                {
                    if (retVal == WAIT_TIMEOUT)
                    {
                       DoCoreMigration();
                       if (SchedulersNeedNotifications())
                       {
                            SendResourceNotifications();
                       }
                       // The old tick count needs to be snapped each time we go back to wait with the original timeout value.
                       // That way, if we are woken up in between to deliver notifications, we can go back to wait for the remainder
                       // of the timeout.
                       oldTickCount = platform::__GetTickCount64();
                       timeout = DynamicRMTimeInterval;
                    }
                    else
                    {
                        newTickCount = platform::__GetTickCount64();
                        DWORD tickDifference = (DWORD) (newTickCount - oldTickCount);
                        if (tickDifference > DynamicRMTimeInterval)
                        {
                            // We're holding the lock, and the state is LoadBalance. There should be at least two
                            // schedulers the RM knows about at this time.
                            ASSERT(m_numSchedulers > 1);
                            if (tickDifference > DynamicRMTimeInterval + 30)
                            {
                                // Since GetTickCount is accurate upto 10-15ms, do not throw away statistics,
                                // unless we've waited for a 'long' time.
                                DiscardExistingSchedulerStatistics();
                            }
                            else if (SchedulersNeedNotifications())
                            {
                                SendResourceNotifications();
                            }

                            oldTickCount = platform::__GetTickCount64();
                            timeout = DynamicRMTimeInterval;
                        }
                        else
                        {
                            // We were woken up within the 100 ms interval - most likely so that we could send notifications.
                            if (SchedulersNeedNotifications())
                            {
                                SendResourceNotifications();
                            }
                            timeout = DynamicRMTimeInterval - tickDifference;
                        }
                    }
                    break;
                }
                case Exit:
                default:
                {
                    ASSERT(m_dynamicRMWorkerState == Exit);

                    // We are shutting down
                    break;
                }

                }; // end switch (m_dynamicRMWorkerState)
            } // end locked region
        }
    }

    /// <summary>
    ///     Main thread procedure for the dynamic RM worker thread.
    /// </summary>
    /// <param name="lpParameter">
    ///     Resource manager pointer passed to the worker thread.
    /// </param>
    /// <returns>
    ///     Status on thread exit.
    /// </returns>
    DWORD CALLBACK ResourceManager::DynamicRMThreadProc(LPVOID lpParameter)
    {
        ResourceManager * pResourceManager = (ResourceManager *) lpParameter;
        pResourceManager->DynamicResourceManager();
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

    /// <summary>
    ///     The API returns after ensuring that all store buffers on processors that are running threads from this process,
    ///     are flushed. It does this by either calling a Win32 API that explictly does this on versions of Windows that
    ///     support the functionality, or by changing the protection on a page using VirtualProtect.
    ///
    ///     NOTE: We use the same mechanism the CLR uses for flushing write buffers in absense of the FlushProcessoWriteBuffers API,
    ///     and most of the comments below are taken directly from the CLR vm code.
    /// </summary>
    void ResourceManager::FlushStoreBuffers()
    {
        if (s_version >= ::Concurrency::IResourceManager::Vista)
        {
            FlushProcessWriteBuffers();
        }
#if !defined(_ONECORE)
        else
        {
            ASSERT(m_pPageVirtualProtect != NULL);
            ASSERT(*m_pPageVirtualProtect == 1);

            // Note that the read of *m_pPageVirtualProtect is very important, as it makes it extremely likely that this memory will
            // be in the working set when we call VirtualProtect (see comments below).
            if (*m_pPageVirtualProtect == 1)
            {
                //
                // VirtualProtect simulates FlushProcessWriteBuffers because it happens to send an inter-processor interrupt to all CPUs,
                // and inter-processor interrupts happen to cause the CPU's store buffers to be flushed.
                //
                // Unfortunately, VirtualProtect only does this if the page whose status is being changed is in the process' working set
                // (otherwise there's no need to tell the other CPUs that anything has changed).
                //
                // One way to do this is to lock the page into the process' working set. Unfortunately, it can fail if there are already too many
                // locked pages.
                //
                // We could increase the process' working set limit, using SetProcessWorkingSet, but that would be a) intrusive (the process may
                // have its own idea of what the limit should be), and b) race-prone (another thread may be trying to adjust the limit, to a
                // different value, at the same time).
                //
                // We could stop using *m_pPageVirtualProtect as the page we fiddle with, and instead use a page we know is already locked into
                // the working set. There's no way to enumerate such pages, so it'd have to be a well-known fixed location that we know is always
                // locked, and that can have its protection fiddled with without consequence.  We know of no such location, and if we did it would
                // undoubtedly be some internal Windows data structure that would be subject to changes in the way its memory is handled at any time.
                //
                // The VirtualProtect trick has worked for many years in the CLR, without the call to VirtualLock, without apparent problems.
                // Part of the reason is because of the equivalent of the check of *m_pPageVirtualProtect above.
                //
                DWORD oldProtect;

                // We have it on good authority from the kernel team that, although VirtualProtect is repeatedly called with the
                // same protection (PAGE_READONLY), the OS will not optimize out the flush buffers as a result.
                BOOL retVal = VirtualProtect(m_pPageVirtualProtect, sizeof(BYTE), PAGE_READONLY, &oldProtect);
                ASSERT(retVal);
            }
        }
#endif // !defined(_ONECORE)

    }

    ResourceManager::~ResourceManager()
    {
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            delete [] m_pGlobalNodes[i].m_pCores;
        }
        delete [] m_pGlobalNodes;
        delete [] m_pGlobalNumaNodes;

#if !defined(_ONECORE)
        if (m_pPageVirtualProtect != NULL)
        {
            VirtualFree(m_pPageVirtualProtect, 0, MEM_RELEASE);
        }
#endif // !defined(_ONECORE)

#if defined(_DEBUG)
            ASSERT(m_schedulers.Empty());
#endif

        ASSERT(m_hDynamicRMEvent != NULL);
        CloseHandle(m_hDynamicRMEvent);

        delete [] m_ppProxyData;

        if (m_hDynamicRMThreadHandle != NULL)
        {
            platform::__CloseThreadHandle(m_hDynamicRMThreadHandle);

            delete [] m_ppGivingProxies;
            delete [] m_ppReceivingProxies;
        }

#if defined(CONCRT_TRACING)
        delete [] m_drmInitialState;
#endif
    }

    /// <summary>
    ///     Returns an interface to the next node in enumeration order.
    /// </summary>
    /// <returns>
    ///     An interface to the next node in enumeration order.  If there are no more nodes in enumeration order of the system topology, this method
    ///     will return NULL.
    /// <returns>
    /// <seealso cref="IResourceManager::GetFirstNode Method"/>
    /// <seealso cref="ITopologyExecutionResource Structure"/>
    /**/
    ITopologyNode *GlobalNode::TopologyObject::GetNext() const
    {
        GlobalNode *pNode = m_pNode->m_pRM->GetNextGlobalNode(m_pNode);
        return pNode ? pNode->m_pTopologyObject : NULL;
    }

    /// <summary>
    ///     Returns an interface to the next execution resource in enumeration order.
    /// </summary>
    /// <returns>
    ///     An interface to the next execution resource in enumeration order.  If there are no more nodes in enumeration order of the node to which
    ///     this execution resource belongs, this method will return NULL.
    /// <returns>
    /// <seealso cref="ITopologyNode::GetFirstExecutionResource Method"/>
    /// <seealso cref="ITopologyNode Structure"/>
    /**/
    ITopologyExecutionResource *GlobalCore::TopologyObject::GetNext() const
    {
        GlobalCore *pCore = m_pCore->m_pOwningNode->GetNextGlobalCore(m_pCore);
        return pCore ? pCore->m_pTopologyObject : NULL;
    }

} // namespace details
} // namespace Concurrency

#pragma warning(pop)
