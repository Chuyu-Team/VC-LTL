// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ResourceManager.h
//
// Implementation of IResourceManager.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    #pragma warning(push)
    #pragma warning(disable: 4265) // non-virtual destructor in base class
    class ResourceManager final : public ::Concurrency::IResourceManager
    {
    public:
        /// <summary>
        ///     Increments the reference count on a resource manager.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Reference();

        /// <summary>
        ///     Decrements the reference count on a resource manager.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Release();

        /// <summary>
        ///     Associates an IScheduler with the ISchedulerProxy that represents the part
        ///     of IResourceManager associated with the IScheduler.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler to be associated.
        /// </param>
        /// <param name="version">
        ///     The version of the RM<->Scheduler communication channel that is being utilized.
        /// </param>
        virtual ISchedulerProxy * RegisterScheduler(IScheduler *pScheduler, unsigned int version);

        /// <summary>
        ///     Returns the number of nodes available to the Resource Manager.
        /// </summary>
        /// <returns>
        ///     The number of nodes available to the Resource Manager.
        /// </returns>
        /**/
        virtual unsigned int GetAvailableNodeCount() const
        {
            // TRANSITION: At some point when RM honors process affinity, etc..., go back and ensure that this is the correct value to return!
            return m_nodeCount;
        }

        /// <summary>
        ///     Returns the first node in enumeration order as defined by the Resource Manager.
        /// </summary>
        /// <returns>
        ///     The first node in enumeration order as defined by the Resource Manager.
        /// </returns>
        /// <seealso cref="ITopologyNode::GetExecutionResourceCount Method"/>
        /**/
        virtual ITopologyNode* GetFirstNode() const
        {
            return m_pGlobalNodes->m_pTopologyObject;
        }

        /// <summary>
        ///     Creates an instance of the resource manager.
        /// </summary>
        static ResourceManager* CreateSingleton();

        /// <summary>
        ///     Returns the OS version.
        /// </summary>
        static IResourceManager::OSVersion Version();

        /// <summary>
        ///     Returns the number of nodes (processor packages or NUMA nodes)
        /// </summary>
        static unsigned int GetNodeCount();

        /// <summary>
        ///     Returns the number of cores.
        /// </summary>
        static unsigned int GetCoreCount();

        /// <summary>
        ///     Returns a pointer to the manager of factories for thread proxies.
        /// </summary>
        ThreadProxyFactoryManager * GetThreadProxyFactoryManager() { return &m_threadProxyFactoryManager; }

        /// <summary>
        ///     These APIs return unique identifiers for use by schedulers, execution contexts and thread proxies.
        /// </summary>
        static unsigned int GetSchedulerId() { return InterlockedIncrement(&s_schedulerIdCount); }
        static unsigned int GetExecutionContextId() { return InterlockedIncrement(&s_executionContextIdCount); }
        static unsigned int GetThreadProxyId() { return InterlockedIncrement(&s_threadProxyIdCount); }

        /// <summary>
        ///     These APIs restrict the execution resources used by the Concurrency Runtime's internal worker threads to the affinity set specified.
        /// </summary>
        static void SetTaskExecutionResources(DWORD_PTR dwAffinityMask);
        static void SetTaskExecutionResources(USHORT count, PGROUP_AFFINITY pGroupAffinity);

        /// <summary>
        ///     The main allocation routine that allocates cores for a newly created scheduler proxy.
        /// </summary>
        ExecutionResource * PerformAllocation(SchedulerProxy *pSchedulerProxy, bool fInitialAllocation, bool fSubscribeCurrentThread);

        /// <summary>
        ///     This API registers the current thread with the resource manager, associating it with this scheduler proxy,
        ///     and returns an instance of IExecutionResource back to the scheduler, for bookkeeping and maintenance.
        /// </summary>
        ExecutionResource * SubscribeCurrentThread(SchedulerProxy *pSchedulerProxy);

        /// <summary>
        ///     The allocation routine that allocates a single core for a newly registered external thread.
        /// </summary>
        ExecutionResource * PerformExternalThreadAllocation(SchedulerProxy *pSchedulerProxy);

        /// <summary>
        ///     Removes an execution resource that was created for an external thread.
        /// </summary>
        void RemoveExecutionResource(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Called in order to notify the resource manager that the given scheduler is shutting down.  This
        ///     will cause the resource manager to immediately reclaim all resources granted to the scheduler.
        /// </summary>
        void Shutdown(SchedulerProxy *pProxy);

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
        IExecutionResource * RequestInitialVirtualProcessors(SchedulerProxy *pProxy, bool doSubscribeCurrentThread);

        /// <summary>
        ///     Debug CRT test hook to create artificial topologies. With the retail CRT, this API simply returns.
        /// </summary>
        virtual void CreateNodeTopology(unsigned int nodeCount, unsigned int *pCoreCount, unsigned int **pNodeDistance, unsigned int *pProcessorGroups);

        /// <summary>
        ///     The API returns after ensuring that all store buffers on processors that are running threads from this process,
        ///     are flushed. It does this by either calling a Win32 API that explictly does this on versions of Windows that
        ///     support the functionality, or by changing the protection on a page that is locked into the working set, forcing
        ///     a TB flush on all processors in question.
        /// </summary>
        void FlushStoreBuffers();

        /// <summary>
        ///     Returns the TLS slot where information about an execution resource is supposed to be stored.
        /// </summary>
        /// <remarks>
        ///     Used to check whether SubscribeCurrentThread already has an execution resource on which it is running.
        /// </remarks>
        DWORD GetExecutionResourceTls() const
        {
            return m_threadProxyFactoryManager.GetExecutionResourceTls();
        }

        /// <summary>
        ///     Decrements the use count on a particular global core. Used for removal of external threads.
        /// </summary>
        void DecrementCoreUseCount(unsigned int nodeId, unsigned int coreIndex)
        {
            // NOTE: Caller is responsible for holding the RM lock!
            GlobalCore * pGlobalCore = &(m_pGlobalNodes[nodeId].m_pCores[coreIndex]);
            pGlobalCore->m_useCount--;
        }

        /// <summary>
        ///     Returns the current thread's node id and core id (relative to that node).
        /// </summary>
        unsigned int GetCurrentNodeAndCore(unsigned int * pCore);

        /// <summary>
        ///     Returns the global subscription level of the underlying core.
        /// </summary>
        unsigned int CurrentSubscriptionLevel(unsigned int nodeId, unsigned int coreIndex);

        /// <summary>
        ///     Returns true if there are any schedulers in the RM that need notifications sent, false, otherwise.
        /// </summary>
        bool SchedulersNeedNotifications()
        {
            return (m_numSchedulersNeedingNotifications > 0);
        }

        /// <summary>
        ///     Returns the number of schedulers that need core busy/idle notifications.
        /// </summary>
        unsigned int GetNumSchedulersForNotifications()
        {
            return m_numSchedulersNeedingNotifications;
        }

        /// <summary>
        ///     Wakes up the dynamic RM worker from a wait state.
        /// </summary>
        void WakeupDynamicRMWorker()
        {
            SetEvent(m_hDynamicRMEvent);
        }

        /// <summary>
        ///     Sends NotifyResourcesExternallyIdle/NotifyResourcesExternallyBusy notifications to the schedulers that
        ///     qualify for them, to let them know that the hardware resources allocated to them are in use or out of use
        ///     by other schedulers that share those resources.
        /// </summary>
        /// <param name ="pNewlyAllocatedProxy">
        ///     The newly allocated scheduler proxy, if one was just allocated.
        /// </param>
        void SendResourceNotifications(SchedulerProxy * pNewlyAllocatedProxy = NULL);

        /// <summary>
        ///     Allocates and initializes the data structure that will represent the allocated nodes for a scheduler proxy.
        ///     This function is called the first time a scheduler proxy requests an allocation.
        /// </summary>
        SchedulerNode * CreateAllocatedNodeData();

        /// <summary>
        ///     Destroys the data structures representing nodes/cores allocated to a scheduler proxy when the proxy has
        ///     shutdown.
        /// </summary>
        void DestroyAllocatedNodeData(SchedulerNode * pAllocatedNodes);

        /// <summary>
        ///     Gets the next node in enumeration order.
        /// </summary>
        GlobalNode *GetNextGlobalNode(const GlobalNode *pNode)
        {
            unsigned int idx = (unsigned int)((pNode - m_pGlobalNodes) + 1);
            return ((idx < m_nodeCount) ? &m_pGlobalNodes[idx] : NULL);
        }

    private:

        // State that controls what the dynamic RM worker thread does.
        enum DynamicRMWorkerState
        {
            Standby,
            LoadBalance,
            Exit
        };

        struct AffinityRestriction
        {
            AffinityRestriction() :
                m_count(0),
                m_pGroupAffinity(NULL)
            {
            }

            AffinityRestriction(USHORT count, HardwareAffinity * pGroupAffinity) :
                m_count(count),
                m_pGroupAffinity(pGroupAffinity)
            {
            }

            ~AffinityRestriction()
            {
                delete m_pGroupAffinity;
            }

            HardwareAffinity * FindGroupAffinity(USHORT group)
            {
                for (USHORT i = 0; i < m_count; ++i)
                {
                    if (m_pGroupAffinity[i].GetGroup() == group)
                    {
                        return &m_pGroupAffinity[i];
                    }
                }
                return NULL;
            }

            void ApplyAffinityLimits(PGROUP_AFFINITY pGroupAffinity)
            {
                HardwareAffinity * pAffinity = FindGroupAffinity(pGroupAffinity->Group);
                if (pAffinity != NULL)
                {
                    pGroupAffinity->Mask &= pAffinity->GetMask();
                }
                else
                {
                    // The user has not asked for this group to be included.
                    pGroupAffinity->Mask = 0;
                }
            }

            USHORT GetCount()
            {
                return m_count;
            }

        private:
            USHORT m_count;
            HardwareAffinity * m_pGroupAffinity;
        };

#if defined(CONCRT_TRACING)

        struct GlobalCoreData
        {
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            unsigned char m_useCount;
            unsigned char m_idleSchedulers;
        };

        // Maintains a trace for every core removed during preprocessing.
        struct PreProcessingTraceData
        {
            SchedulerProxy * m_pProxy;
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            bool m_fMarkedAsOwned : 1;
            bool m_fBorrowedCoreRemoved : 1;
            bool m_fSharedCoreRemoved : 1;
            bool m_fIdleCore : 1;
        };

        // Maintains a trace for each core allocation
        struct DynamicAllocationTraceData
        {
            SchedulerProxy * m_pGiver; // this is null if the core was unused or idle
            SchedulerProxy * m_pReceiver;
            unsigned char m_round;
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            bool m_fUnusedCoreMigration : 1;
            bool m_fIdleCoreSharing : 1;
            bool m_fBorrowedByGiver : 1;
            bool m_fIdleOnGiver : 1;
        };

#endif

        //
        // Private data
        //

        // The singleton resource manager instance.
        static ResourceManager *s_pResourceManager;

        // System and process affinities, eithr wenthe RM is created or an affinity limitation is specified. Note that this is limited to 64 cores on
        // >64 core systems; by default the OS puts a process in a single group. For processes on a >64 core system, the system affinity mask reflects the
        // affinity of the process in that group.
        static DWORD_PTR s_processAffinityMask;
        static DWORD_PTR s_systemAffinityMask;

        // Pointer to a data structure that is initialized if the task execution affinity for ConcRT is limited via the API SetTaskExecutionResources.
        static AffinityRestriction * s_pUserAffinityRestriction;
        // Pointer to a data structure that is initialized if the process affinity is different from the system affinity at the time the topology is created.
        static AffinityRestriction * s_pProcessAffinityRestriction;

        // A lock that protects setting of the singleton instance.
        static _StaticLock s_lock;

        // The total number of hardware threads available to the RM.
        static unsigned int s_coreCount;

        // Tells whether the node count is the number of processor packages or NUMA nodes.
        static bool s_countProcessorPackages;

        // The number of nodes that hardware threads are grouped into.
        static unsigned int s_nodeCount;

        // The number of NUMA nodes in the RM's global array of numa nodes. Note that not all NUMA nodes with numbers between 0 and (s_numaNodeCount- 1) will
        // necessarily be available to the RM for thread scheduling. RMs global array will only represent the NUMA nodes the RM is going to schedule threads on.
        static unsigned int s_numaNodeCount;

        // Operating system characteristics.
        static IResourceManager::OSVersion s_version;

        // Termination indicator
        static volatile LONG s_terminating;

        // Static counters to generate unique identifiers.
        static volatile LONG s_schedulerIdCount;
        static volatile LONG s_executionContextIdCount;
        static volatile LONG s_threadProxyIdCount;

        // Variables used to obtain information about the topology of the system.
        static DWORD s_logicalProcessorInformationLength;
        static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION s_pSysInfo;

        // Constants that are used as parameters to the ReleaseSchedulerResources API
        static const unsigned int ReleaseCoresDownToMin = static_cast<unsigned int>(-1);
        static const unsigned int ReleaseOnlyBorrowedCores = static_cast<unsigned int>(-2);

        // RM instance reference count
        volatile LONG m_referenceCount;

        // Number of schedulers currently using resources granted by the RM.
        unsigned int m_numSchedulers;

        // The maximum number of schedulers the dynamic RM worker has resources to handle, at any time. This can
        // change during the lifetime of the process.
        unsigned int m_maxSchedulers;

        // Number of schedulers in the RM that need resource notifications.
        unsigned int m_numSchedulersNeedingNotifications;

        // The number of NUMA nodes on the system.  This is a mirror of s_numaNodeCount above.
        unsigned int m_numaNodeCount;

        // The number of processor packages or NUMA nodes (whichever is greater) on the system. This is a mirror of s_nodeCount above.
        unsigned int m_nodeCount;

        // The number of HW threads on the machine. This is a mirror of s_coreCount above.
        unsigned int m_coreCount;

        // A field used during core migration to keep track of the number of idle cores - cores such that
        // all schedulers they are assigned to are not using them.
        unsigned int m_dynamicIdleCoresAvailable;

        // Keeps track of the allocation round during dynamic core migration. Used for tracing.
        unsigned int m_allocationRound;

        // A state variable that is used to control how the dynamic worker behaves.
        volatile DynamicRMWorkerState m_dynamicRMWorkerState;

        // A lock that protects resource allocation and deallocation.
        _NonReentrantBlockingLock m_lock;

        // The global map of NUMA nodes.
        GlobalNumaNode *m_pGlobalNumaNodes;

        // The global allocation map for all schedulers - array of processor nodes.
        GlobalNode * m_pGlobalNodes;

        // Handle to the dynamic RM worker thread.
        HANDLE m_hDynamicRMThreadHandle;

        // An event that is used to control the dynamic RM worker thread.
        HANDLE m_hDynamicRMEvent;

        // Data used for static and/or dynamic allocation.
        AllocationData ** m_ppProxyData;
        DynamicAllocationData ** m_ppGivingProxies;
        DynamicAllocationData ** m_ppReceivingProxies;

        // Lists to hold scheduler proxies.
        List<SchedulerProxy, CollectionTypes::Count> m_schedulers;

        // A collection of thread proxy factories
        ThreadProxyFactoryManager m_threadProxyFactoryManager;

        // A pointer to a page that is used to flush write buffers on versions of Windows < 6000.
        char* m_pPageVirtualProtect;

#if defined(CONCRT_TRACING)

        // Captures the initial global state during the DRM phase.
        GlobalCoreData * m_drmInitialState;
        unsigned int m_numTotalCores;

        // Maintains a trace for every core removed during preprocessing.
        PreProcessingTraceData m_preProcessTraces[100];
        unsigned int m_preProcessTraceIndex;

        // Maintains a trace for each core allocation
        DynamicAllocationTraceData m_dynAllocationTraces[100];
        unsigned int m_dynAllocationTraceIndex;
#endif

        //
        // Private methods
        //

        // Private constructor.
        ResourceManager();

        // Private destructor.
        ~ResourceManager();

        /// <summary>
        ///     Initializes static variables related to the operating system version.
        /// </summary>
        static void RetrieveSystemVersionInformation();

        /// <summary>
        ///     Captures the process affinity if it is not equal to the system affinity.
        /// </summary>
        static void CaptureProcessAffinity();

        /// <summary>
        ///     Modify the passed in affinity based on any user or process affinity restrictions.
        /// </summary>
        static void ApplyAffinityRestrictions(PGROUP_AFFINITY pGroupAffinity);
        static void ApplyAffinityRestrictions(PULONG_PTR pProcessorMask);

        /// <summary>
        ///     Retrieves a buffer from the operating system that contains topology information.
        /// </summary>
        static void GetTopologyInformation(LOGICAL_PROCESSOR_RELATIONSHIP relationship);

        /// <summary>
        ///     Cleans up the variables that store operating system topology information.
        /// </summary>
        static void CleanupTopologyInformation();

        /// <summary>
        ///     Initializes static information related to the operating system and machine topology.
        /// </summary>
        static void InitializeSystemInformation(bool fSaveTopologyInfo = false);

        /// <summary>
        ///     Creates a scheduler proxy for an IScheduler that registers with the RM.
        /// </summary>
        SchedulerProxy* CreateSchedulerProxy(IScheduler *pScheduler);

        /// <summary>
        ///     Increments the reference count to RM but does not allow a 0 to 1 transition.
        /// </summary>
        /// <returns>
        ///     True if the RM was referenced; false, if the reference count was 0.
        /// </returns>
        bool SafeReference();

        /// <summary>
        ///     Creates a structure of nodes and cores based on the machine topology.
        /// </summary>
        void DetermineTopology();

    /// <summary>
    ///     Instructs existing schedulers to release cores. Then tries to allocate available cores for the new scheduler.
    ///     The parameter 'numberToFree' can have a couple of special values:
    ///         ReleaseCoresDownToMin - scheduler should release all cores above its minimum. Preference is giving to releasing borrowed cores.
    ///         ReleaseOnlyBorrowedCores - scheduler should release all its borrowed cores.
    ///     If the parameter is not a special value, a call should have previously been made for this scheduler with the value ReleaseOnlyBorrowedCores.
    ///     i.e., the scheduler should not have any borrowed cores to release.
    /// </summary>
        unsigned int ReleaseCoresOnExistingSchedulers(SchedulerProxy * pNewProxy, unsigned int request, unsigned int numberToFree);

        /// <summary>
        ///     Tries to redistribute cores allocated to all schedulers, proportional to each schedulers value for 'DesiredHardwareThreads',
        ///     and allocates any freed cores to the new scheduler.
        /// </summary>
        unsigned int RedistributeCoresAmongAll(SchedulerProxy* pSchedulerProxy, unsigned int allocated, unsigned int minimum, unsigned int desired);

        /// <summary>
        ///     Reserves cores for the new scheduler at higher use counts - this is use only to satisify MinHWThreads.
        /// </summary>
        unsigned int ReserveAtHigherUseCounts(SchedulerProxy * pSchedulerProxy, unsigned int request);

        /// <summary>
        ///     Worker routine that does actual core reservation, using the supplied use count. It tries to
        ///     pack reserved cores onto nodes by preferring nodes where more free cores are available.
        /// </summary>
        unsigned int ReserveCores(SchedulerProxy * pSchedulerProxy, unsigned int request, unsigned int useCount);

        /// <summary>
        ///     Instructs a scheduler proxy to free up a fixed number of resources. This is only a temporary release of resources. The
        ///     use count on the global core is decremented and the scheduler proxy remembers the core as temporarily released. At a later
        ///     point, the release is either confirmed or rolled back, depending on whether the released core was used to satisfy a
        ///     different scheduler's allocation.
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
        ///         ReleaseOnlyBorrowedCores - scheduler should release all its borrowed cores.
        ///     If the parameter is not a special value, a call should have previously been made for this scheduler with the value ReleaseOnlyBorrowedCores.
        ///     i.e., the scheduler should not have any borrowed cores to release.
        /// </param>
        bool ReleaseSchedulerResources(SchedulerProxy * pReceivingProxy, SchedulerProxy *pGivingProxy, unsigned int numberToFree);

        /// <summary>
        ///     Called to claim back any previously released cores that were not allocated to a different scheduler. If released
        ///     cores were allocated (stolen), the proxy needs to notify its scheduler to give up the related virtual processor
        ///     roots.
        /// <summary>
        void CommitStolenCores(SchedulerProxy * pSchedulerProxy);

        /// <summary>
        ///     Starts up the dynamic RM worker thread if it is on standby, or creates a thread if one is not already created.
        ///     The worker thread wakes up at fixed intervals and load balances resources among schedulers, until it it put on standby.
        /// </summary>
        void CreateDynamicRMWorker();

        /// <summary>
        ///     Routine that performs dynamic resource management among existing schedulers at fixed time intervals.
        /// </summary>
        void DynamicResourceManager();

        /// <summary>
        ///     Performs a dynamic resource allocation based on feedback from hill climbing.
        /// </summary>
        void DoCoreMigration();

        /// <summary>
        ///     When the number of schedulers in the RM goes from 2 to 1, this routine is invoked to make sure the remaining scheduler
        ///     has its desired number of cores, before putting the dynamic RM worker on standby. It is also called when there is just
        ///     one scheduler with external subscribed threads that it removes -> there is a chance that this move may allow us to allocate
        ///     more vprocs.
        /// </summary>
        bool DistributeCoresToSurvivingScheduler();

        /// <summary>
        ///     This API is called by the dynamic RM worker thread when it starts up, and right after its state changed to
        ///     LoadBalance after being on Standby for a while. We need to find the existing schedulers, and discard the
        ///     statistics they have collected so far if any. Either we've never collected statistics for this scheduler before,
        ///     or too much/too little time has passed since we last collected statistics, and this information cannot be trusted.
        /// </summary>
        void DiscardExistingSchedulerStatistics();

        /// <summary>
        ///     Ensures that the memory buffers needed for static and dynamic RM are of the right size, and initializes them.
        /// </summary>
        void InitializeRMBuffers();

        /// <summary>
        ///     Populates data needed for allocation (static or dynamic).
        /// </summary>
        void PopulateCommonAllocationData(unsigned int index, SchedulerProxy * pSchedulerProxy, AllocationData * pAllocationData);

        /// <summary>
        ///     Captures data needed for static allocation, for all existing schedulers. This includes determining which
        ///     cores on a scheduler are idle.
        ///     A number of preprocessing steps are are also preformed before we are ready to allocate cores for a new scheduler.
        ///
        ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
        ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed any more.
        /// </summary>
        void SetupStaticAllocationData(SchedulerProxy * pNewProxy, bool fSubscribeCurrentThread);

        /// <summary>
        ///     Captures data needed for dynamic allocation for all existing schedulers. This includes gathering statistics
        ///     and invoking a per scheduler hill climbing instance, to get a suggested future allocation. Also, determines how may
        ///     idle cores a scheduler has.
        /// </summary>
        void PopulateDynamicAllocationData();

        /// <summary>
        ///     Undo global state that was initialized to perform static allocation or dynamic core migration.
        /// </summary>
        void ResetGlobalAllocationData();

        /// <summary>
        ///     Resets state that was set on the global cores during static or dynamic allocation.
        /// </summary>
        void ResetGlobalState();

        /// <summary>
        ///     Toggles the idle state on a core and updates tracking counts.
        /// </summary>
        void ToggleRMIdleState(SchedulerNode * pAllocatedNode, SchedulerCore * pAllocatedCore,
                                GlobalNode * pGlobalNode, GlobalCore * pGlobalCore, AllocationData * pDRMData);

        /// <summary>
        ///     A number of preprocessing steps are performed before we are ready to allocate cores. They include handling of borrowed and idle,
        ///     cores, as follows:
        ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
        ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed anymore.
        /// </summary>
        void PreProcessStaticAllocationData();

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
        void PreProcessDynamicAllocationData();

        /// <summary>
        ///     Preprocessing steps for borrowed cores - both static and dynamic allocation start out with a call to this API.
        /// </summary>
        void HandleBorrowedCores(SchedulerProxy * pSchedulerProxy, AllocationData * pAllocationData);

        /// <summary>
        ///     Preprocessing steps for shared cores - this is used during dynamic core migration.
        /// </summary>
        void HandleSharedCores(SchedulerProxy * pSchedulerProxy, DynamicAllocationData * pAllocationData);

        /// <summary>
        ///     This routine increases the suggested allocation to desired, for schedulers with the following characteristics:
        ///     1) Hill climbing has *not* recommended an allocation decrease.
        ///     2) They are using all the cores allocated to them (no idle cores).
        ///     In the second round of core migration, we try to satisfy these schedulers' desired allocation.
        /// </summary>
        void IncreaseFullyLoadedSchedulerAllocations();

        /// <summary>
        ///     Decides on the number of additional cores to give a set of schedulers, given what the schedulers need and what is available.
        /// </summary>
        unsigned int AdjustDynamicAllocation(unsigned int coresAvailable, unsigned int coresNeeded, unsigned int numReceivers);

        /// <summary>
        ///     Initializes receiving proxy data in preparation for core transfer. Calcuates the number of partially filled nodes
        ///     for schedulers that are receiving cores, and sorts the receiving proxy data in increasing order of partial nodes.
        /// </summary>
        /// <returns>
        ///     Number of recivers that still need cores (allocation field of the receiving proxy data > 0).
        /// </returns>
        unsigned int PrepareReceiversForCoreTransfer(unsigned int numReceivers);

        /// <summary>
        ///     Assigns one core at a time to a partially filled node on a receiving scheduler proxy, if possible
        /// </summary>
        bool FindCoreForPartiallyFilledNode(unsigned int& unusedCoreQuota,
                                            unsigned int& usedCoreQuota,
                                            DynamicAllocationData * pReceivingProxyData,
                                            unsigned int numGivers);

        /// <summary>
        ///     Attempts to assign cores to a receiver on a single empty node, taking cores from multiple givers if necessary.
        /// </summary>
        unsigned int FindBestFitExclusiveAllocation(unsigned int& unusedCoreQuota,
                                                    unsigned int& usedCoreQuota,
                                                    DynamicAllocationData * pReceivingProxyData,
                                                    unsigned int remainingReceivers,
                                                    unsigned int numGivers);

        /// <summary>
        ///     Distributes unused cores and cores from scheduler proxies that are willing to give up cores to scheduler proxies that
        ///     need cores.
        /// </summary>
        void DistributeExclusiveCores(unsigned int totalCoresNeeded,
                                      unsigned int unusedCoreQuota,
                                      unsigned int usedCoreQuota,
                                      unsigned int numReceivers,
                                      unsigned int numGivers);

        /// <summary>
        ///     Attempts to assign cores to a receiver on a single empty node, using idle cores.
        /// </summary>
        unsigned int FindBestFitIdleAllocation(unsigned int idleCoresAvailable, DynamicAllocationData * pReceivingProxyData, unsigned int remainingReceivers);

        /// <summary>
        ///     Distributes idle cores to scheduler proxies that need cores.
        /// </summary>
        void DistributeIdleCores(unsigned int totalCoresAvailable, unsigned int numReceivers);

        /// <summary>
        ///     Assigns a fixed number of unused or idle cores to a scheduler from a given node.
        /// </summary>
        void DynamicAssignCores(SchedulerProxy * pReceivingProxy, unsigned int nodeIndex, unsigned int numCoresToMigrate, bool fIdle);

        /// <summary>
        ///     Transfers a fixed number of cores from one scheduler to another.
        /// </summary>
        void DynamicMigrateCores(DynamicAllocationData * pGivingProxyData, SchedulerProxy * pReceivingProxy, unsigned int nodeIndex, unsigned int numCoresToMigrate);

#if defined (CONCRT_TRACING)

        /// <summary>
        ///     Captures the initial state of the global map at the beginning of core migration, each cycle.
        /// </summary>
        void TraceInitialDRMState();

        /// <summary>
        ///     Captures data relating to an action during DRM preprocessing.
        /// </summary>
        void TracePreProcessingAction(SchedulerProxy * pProxy, unsigned int nodeIndex, unsigned int coreIndex,
                                      bool fMarkedAsOwned, bool fBorrowedCoreRemoved, bool fSharedCoreRemoved, bool fIdleCore);

        /// <summary>
        ///     Captures data relating to an action during DRM core migration.
        /// </summary>
        void TraceCoreMigrationAction(SchedulerProxy * pGiver, SchedulerProxy * pReceiver, unsigned int round, unsigned int nodeIndex,
                                      unsigned int coreIndex, bool fUnusedCoreMigration, bool fIdleCoreSharing, bool fBorrowedByGiver,
                                      bool fIdleOnGiver);

#endif
        /// <summary>
        ///     Performs borrowed core validation. A core can be borrowed by only one scheduler at a time.
        /// </summary>
        void ValidateBorrowedCores();

        /// <summary>
        ///     Performs state validations during dynamic core migration.
        /// </summary>
        void ValidateDRMSchedulerState();

        /// <summary>
        ///     Performs state validations during static allocation.
        /// </summary>
        void ValidateStaticSchedulerState(SchedulerProxy * pSchedulerProxy);

        /// <summary>
        ///     Main thread procedure for the dynamic RM worker thread.
        /// </summary>
        /// <param name="lpParameter">
        ///     Resource manager pointer passed to the worker thread.
        /// </param>
        /// <returns>
        ///     Status on thread exit.
        /// </returns>
        static DWORD CALLBACK DynamicRMThreadProc(LPVOID lpParameter);

        /// <summary>
        ///     Given a set of scaled floating point allocations that add up to nSum, rounds them to integers.
        /// </summary>
        static void RoundUpScaledAllocations(AllocationData ** ppData, unsigned int count, unsigned int nSum);
    };

    #pragma warning(pop)
} // namespace details
} // namespace Concurrency
