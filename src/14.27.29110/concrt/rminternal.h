// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// rminternal.h
//
// Main internal header file for ConcRT's Resource Manager.
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    // The lowest two bits are used to determine the type of the pointer stored in the
    // execution resource TLS slot (created in the RM).
    static const size_t TlsResourceBitMask = 0x3;
    static const size_t TlsResourceInResource = 0x0;
    static const size_t TlsResourceInProxy = 0x1;
    static const size_t TlsResourceInUMSProxy = 0x2;

    // The RM has an array of processor nodes and cores representing the hardware topology on the machine.
    // Every scheduler that asks for an allocation, gets it's own copy of this array of nodes/cores once they
    // have been granted an allocation of cores by the RM, that is stored in the corresponding scheduler proxy.

    /// <summary>
    ///     An abstraction of a hardware affinity that understands how to deal with affinity on both Win7 and pre-Win7
    ///     platforms.
    /// </summary>
    struct HardwareAffinity
    {
    public:

        /// <summary>
        ///     Construct an empty affinity.
        /// </summary>
        HardwareAffinity()
        {
            memset(&m_affinity, 0, sizeof(m_affinity));
            m_affinity.Group = 0;
            m_affinity.Mask = 0;
        }

        /// <summary>
        ///     Construct a hardware affinity from a given thread.
        /// </summary>
        HardwareAffinity(HANDLE hThread);

        /// <summary>
        ///    Construct a Win7 understood affinity.
        /// </summary>
        HardwareAffinity(USHORT processorGroup, ULONG_PTR affinityMask)
        {
            memset(&m_affinity, 0, sizeof(m_affinity));
            m_affinity.Group = processorGroup;
            m_affinity.Mask = affinityMask;
        }

        /// <summary>
        ///     Construct a pre-Win7 understood affinity.
        /// </summary>
        HardwareAffinity(DWORD_PTR affinityMask)
        {
            memset(&m_affinity, 0, sizeof(m_affinity));
            m_affinity.Group = 0;
            m_affinity.Mask = affinityMask;
        }

        /// <summary>
        ///     Copy construct an affinity.
        /// </summary>
        HardwareAffinity(const HardwareAffinity &src)
        {
            memcpy(&m_affinity, &src.m_affinity, sizeof(m_affinity));
        }

        /// <summary>
        ///     Compare two affinities
        /// </summary>
        bool operator==(const HardwareAffinity &rhs)
        {
            return (rhs.m_affinity.Group == m_affinity.Group && rhs.m_affinity.Mask == m_affinity.Mask);
        }

        /// <summary>
        ///     Compare two affinities
        /// </summary>
        bool operator!=(const HardwareAffinity &rhs)
        {
            return !operator==(rhs);
        }

        /// <summary>
        ///     Copy an affinity.
        /// </summary>
        HardwareAffinity& operator=(const HardwareAffinity &rhs)
        {
            m_affinity.Group = rhs.m_affinity.Group;
            m_affinity.Mask = rhs.m_affinity.Mask;
            return *this;
        }

        /// <summary>
        ///     Return the group number.
        /// </summary>
        USHORT GetGroup()
        {
            return m_affinity.Group;
        }

        /// <summary>
        ///     Return the group mask.
        /// </summary>
        KAFFINITY GetMask()
        {
            return m_affinity.Mask;
        }

        /// <summary>
        ///     Modify the affinity to be the intersection of the existing affinity and the provided affinity.
        /// </summary>
        void IntersectWith(KAFFINITY affinity)
        {
            m_affinity.Mask &= affinity;
        }

        /// <summary>
        ///     Applies this hardware affinity to a thread.
        /// </summary>
        /// <param name="hThread">
        ///     The thread handle to which to apply this affinity.
        /// </param>
        void ApplyTo(HANDLE hThread);

        GROUP_AFFINITY m_affinity;
    };

    /// <summary>
    ///     Base class for description of a core or hardware thread.
    /// </summary>
    struct ProcessorCore
    {
        enum CoreState
        {
            // Initial state for all cores.
            Unknown = 0,

            // A core that is not assigned to a scheduler proxy, either because the scheduler proxy did
            // not ask for it (it already has what it desires), or because it is assigned to some other scheduler.
            // This state is only used for core in a scheduler's local view of resources.
            Unassigned,

            // The core is available and may be reserved for or allocated to a scheduler.
            Available,

            // The core is reserved for a scheduler by the Resource Manager. This state is only used for core
            // in a scheduler's local view of resources.
            Reserved,

            // The core is allocated to a scheduler. Cores transition from Reserved to Allocated once the
            // scheduler proxy has allocated execution resources or virtual processors for its scheduler.
            Allocated,

            // When a scheduler releases a core for a different scheduler, it sets its core state to Stolen.
            // This enables it to track the cores it has relinquished. If the core is not allocated to the
            // receiving scheduler, it will revert back to Allocated for the scheduler proxy it came from.
            Stolen,

            // A core is considered idle during dynamic core migration if the scheduler(s) that core is assigned
            // to, have all vprocs de-activated. This state is only used for cores in the global map.
            Idle
        };

        // 'Available' means available for assignment to a scheduler during the allocation calculation.
        CoreState m_coreState;

        // The Resource Manager assigned identifier for the processor core.
        unsigned int m_id;

        // The processor number in Win7 {group, processor number} id scheme.
        BYTE m_processorNumber;
    };

    // Forward declaration.
    struct GlobalNode;
    class ResourceManager;

    /// <summary>
    ///     Representation of a processor core within the RM's global map of execution resources. Information in this struct
    ///     represents a systemwide view of the underlying hardware thread.
    /// </summary>
    struct GlobalCore : public ProcessorCore
    {
        // Back pointer to the node for topological enumeration.
        GlobalNode *m_pOwningNode;

        // The number of schedulers that this core is assigned to.
        unsigned int m_useCount;

        // Used to send notifications to qualifying schedulers regarding external subscription level changes.
        LONG m_currentSubscriptionLevel;
        LONG m_previousSubscriptionLevel;

        // This field is used during core migration to represent the number of schedulers that this core has been allocated
        // to, that have also deactivated all virtual processors on the core, i.e. the number of schedulers that are 'idle'
        // with respect to this core. When this is equal to the use count, the core is considered 'idle'.
        unsigned int m_idleSchedulers;

        /// <summary>
        ///     Initializes a processor node.
        /// </summary>
        void Initialize(GlobalNode *pOwningNode, unsigned int id, BYTE processorNumber)
        {
            m_pTopologyObject = _concrt_new TopologyObject(this);

            m_pOwningNode = pOwningNode;
            m_id = id;
            m_processorNumber = processorNumber;
        }

        ~GlobalCore()
        {
            delete m_pTopologyObject;
        }

        //**************************************************
        // Topology walking:
        //

        //
        // TRANSITION: This needs to be eliminated.  There are a slew of memcpy and memset operations on GlobalCore and GlobalNode which
        // assume this is how they initialize and copy the base class.  We cannot add virtual methods on GlobalCore without cleaning ALL of that
        // up.  Hence, we heap alloc a side object until such time as it can be cleaned up.
        //
        struct TopologyObject : public ITopologyExecutionResource
        {
            GlobalCore *m_pCore;

            TopologyObject(GlobalCore *pCore) : m_pCore(pCore)
            {
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
            virtual ITopologyExecutionResource *GetNext() const;

            /// <summary>
            ///     Returns the resource manager's unique identifier for this execution resource.
            /// </summary>
            /// <returns>
            ///     The resource manager's unique identifier for this execution resource.
            /// </returns>
            /**/
            virtual unsigned int GetId() const
            {
                return m_pCore->m_id;
            }

        } *m_pTopologyObject;
    };

    /// <summary>
    ///     Representation of a processor core within a scheduler proxy's local map of execution resources. Information in this struct
    ///     represents the schedulers utilization of the underlying hardware thread.
    /// </summary>
    struct SchedulerCore : public ProcessorCore
    {
        // When virtual processor roots are created for a scheduler proxy, or external threads are subscribed, the corresponding
        // execution resources are inserted into this list.
        List<ExecutionResource, CollectionTypes::Count> m_resources;

        // Each scheduler core has a pointer to the corresponding global core's use count.
        unsigned int*  m_pGlobalUseCountPtr;

        // This field represents the number of activated virtual processors and subscribed threads that a scheduler has
        // on this core at any time. When a virtual processor root is deactivated, or when a thread subscription is released
        // the count is decremented. The core is considered to be 'idle' in the scheduler it belongs to, if this value is 0.
        volatile LONG m_subscriptionLevel;

        // Used to send notifications to qualifying schedulers regarding external subscription level changes.
        LONG m_currentSubscriptionLevel;
        LONG m_previousSubscriptionLevel;

        // The number of threads that were assigned to this core through initial allocation or core migration.
        // Note that this is not necessarily equal to the number of roots in the m_resources list, since the list
        // includes oversubscribed vproc roots as well.
        unsigned int m_numAssignedThreads;

        // The total number of threads (running on vprocs and external) that require this core to be fixed.
        unsigned int m_numFixedThreads;

        // The number of external threads that run on this core.
        unsigned int m_numExternalThreads;

        // This is set to true for a scheduler proxy's core during static allocation or core migration if the subscription
        // level on the core is found to be 0 when the Dynamic RM worker is executing. The subscription value can change
        // as soon as it is captured, but the captured value is what is used for successive computation.
        bool m_fIdleDuringDRM;

        // This is set to true for a scheduler proxy's core during core migration, if this is an borrowed core.
        // An borrowed core is a core that is assigned to one or more different schedulers, but was found to be idle.
        // The RM temporarily assigns idle resources to schedulers that need them.
        bool m_fBorrowed;

        // This variable is set to true when a borrowed core is converted to a fixed core. When the core is unfixed,
        // it is marked borrowed again.
        bool m_fPreviouslyBorrowed;

        /// <summary>
        ///     Returns whether this core is fixed, i.e., cannot be removed by the RM.
        /// </summary>
        bool IsFixed()
        {
            return m_numFixedThreads > 0;
        }

        /// <summary>
        ///     Returns whether this core is idle, i.e., its subscription level was 0 at the time it was retreived by the RM.
        ///     Note that this state could change, but once we capture it, we consider it idle until the next time it is captured.
        /// </summary>
        bool IsIdle()
        {
            return m_fIdleDuringDRM;
        }

        /// <summary>
        ///     Returns whether this core is borrowed, i.e., it was temporarily lent to this scheduler due to the owning
        ///     scheduler being idle on this core.
        /// </summary>
        bool IsBorrowed()
        {
            return m_fBorrowed;
        }
    };

    /// <summary>
    ///     Base class for the description of a processor package or NUMA node.
    /// </summary>
    struct ProcessorNode
    {
        // The affinity mask of the node.
        ULONG_PTR m_nodeAffinity;

        // The total number of cores in the node.
        unsigned int m_coreCount;

        // The number cores that are available for allocation (cores whose state is ProcessorCore::Available)
        unsigned int m_availableCores;

        // The group number in the Win7 {group, mask} id format.
        unsigned int m_processorGroup;

        // The node id which maps to a scheduler node id.
        unsigned int m_id;

        // The NUMA node to which this node belongs.
        DWORD m_numaNodeNumber;
    };

    /// <summary>
    ///     Representation of a processor node within a scheduler proxy's local map of execution resources. Information in this struct
    ///     represents the schedulers utilization of the underlying node.
    /// </summary>
    struct SchedulerNode : public ProcessorNode
    {
        // A scratch field used during allocation. The allocation routine works by looking at cores with m_useCount=0,
        // grabs all it can, then looks at m_useCount=1, then m_useCount=2, etc... During an allocation attempt at a particular
        // use count, cores that were reserved at previous use counts are stored in m_reservedCores, and cores available at the
        // current use count are stored in m_availableCores. A subset of the available cores may be reserved for the scheduler
        // proxy.
        unsigned int m_reservedCores;

        // number of cores allocated to the scheduler proxy (this field is only applicable to a scheduler proxy's nodes)
        unsigned int m_allocatedCores;

        // The number of allocated cores that are borrowed. An borrowed core is a core that is assigned to
        // one or more different schedulers, but was found to be idle. The RM temporarily assigns idle resources to
        // schedulers that need them.
        unsigned int m_numBorrowedCores;

        // The number of cores on this node that are considered fixed. Fixed cores cannot be removed by the RM during static/dynamic allocation.
        unsigned int m_numFixedCores;

        // The number of cores in this node for the scheduler in question that were found to be idle during the dynamic RM phase. This is
        // a scratch field, and the value is stale outside of dynamic RM phases.
        unsigned int m_numDRMIdle;

        // The number of borrowed cores in this node for the scheduler in question that were found to be idle during the dynamic RM phase.
        // This is a scratch field, and the value is stale outside of dynamic RM phases.
        unsigned int m_numDRMBorrowedIdle;

        // The array of cores in this node.
        SchedulerCore * m_pCores;

        /// <summary>
        ///     Returns the number of cores that were found to be idle.
        /// </summary>
        unsigned int GetNumIdleCores()
        {
            return m_numDRMIdle;
        }

        /// <summary>
        ///     Returns the number of allocated cores in this node that are fixed - cannot be removed by dynamic RM.
        /// </summary>
        unsigned int GetNumFixedCores()
        {
            return m_numFixedCores;
        }

        /// <summary>
        ///     Returns the number of movable cores within this node.
        /// </summary>
        unsigned int GetNumMigratableCores()
        {
            return m_allocatedCores - m_numFixedCores;
        }

        /// <summary>
        ///     Returns the number of owned cores - cores that are not borrowed from a different scheduler.
        /// </summary>
        unsigned int GetNumOwnedCores()
        {
            return m_allocatedCores - m_numBorrowedCores;
        }

        /// <summary>
        ///     Returns the number of non-borrowed, non-fixed cores.
        unsigned int GetNumOwnedMigratableCores()
        {
            return m_allocatedCores - m_numBorrowedCores - m_numFixedCores;
        }

        /// <summary>
        ///     Returns the number of borrowed cores - cores that were temporarily lent to this scheduler since the scheduler(s) they
        ///     were assigned to, were not using them.
        /// </summary>
        unsigned int GetNumBorrowedCores()
        {
            return m_numBorrowedCores;
        }

        /// <summary>
        ///     Returns the number of borrowed cores that are idle.
        /// </summary>
        unsigned int GetNumBorrowedIdleCores()
        {
            return m_numDRMBorrowedIdle;
        }

        /// <summary>
        ///     Returns the number of borrowed cores that are not idle.
        /// </summary>
        unsigned int GetNumBorrowedInUseCores()
        {
            ASSERT(m_numBorrowedCores >= m_numDRMBorrowedIdle);
            return (m_numBorrowedCores - m_numDRMBorrowedIdle);
        }

        /// <summary>
        ///     Deallocates memory allocated by the node.
        /// </summary>
        void Cleanup(void)
        {
            delete [] m_pCores;
        }
    };

    /// <summary>
    ///     Representation of a processor node within the RM's global map of execution resources. Information in this struct
    ///     represents a systemwide view of the underlying node.
    /// </summary>
    struct GlobalNode : public ProcessorNode
    {
        // Back pointer to the RM for topological enumeration
        ResourceManager *m_pRM;

        // A scratch field used during dynamic RM allocation, on the RM's global copy of nodes. Idle cores represents the number
        // of cores on this node that are idle and can temporarily be assigned to another scheduler that needs cores.
        unsigned int m_idleCores;

        // The array of cores in this node.
        GlobalCore * m_pCores;

        /// <summary>
        ///     Initializes a processor node.
        /// </summary>
        //
        // coreCount == 0 / baseProcNum == 0 is for regular topologies
        // coreCount != 0 / baseProcNum != 0 is for fake / created topologies
        //
        void Initialize(ResourceManager *pRM, USHORT id, USHORT processorGroup, ULONG_PTR affinityMask, unsigned int coreCount = 0, unsigned int baseProcNum = 0)
        {
            m_pTopologyObject = _concrt_new TopologyObject(this);

            m_pRM = pRM;
            m_id = id;
            m_processorGroup = processorGroup;
            m_nodeAffinity = affinityMask;

            if (coreCount == 0)
                m_coreCount = NumberOfBitsSet(affinityMask);
            else
                m_coreCount = coreCount;

            m_availableCores = 0;

            m_pCores = _concrt_new GlobalCore[m_coreCount];
            memset(m_pCores, 0, m_coreCount * sizeof(GlobalCore));

            for (unsigned int i = 0, j = 0; j < m_coreCount; ++i)
            {
                ASSERT(i < sizeof(ULONG_PTR) * 8);

                // Check if the LSB of the affinity mask is set.
                if (coreCount != 0 || ((affinityMask & 1) != 0))
                {
                    // Bit 0 of the affinity mask corresponds to processor number 0, bit 1 to processor number 1, etc...
                    if (coreCount == 0)
                        m_pCores[j++].Initialize(this, (m_processorGroup << 8) + i, (BYTE)i);
                    else
                        m_pCores[j++].Initialize(this, (m_processorGroup << 16) + (id << 8) + i, (BYTE)i + (BYTE)baseProcNum);
                }
                // Right shift the affinity by 1.
                affinityMask >>= 1;
            }
        }

        /// <summary>
        ///     Returns the next core from pCore within the node.
        /// </summary>
        GlobalCore *GetNextGlobalCore(const GlobalCore *pCore)
        {
            unsigned int idx = (unsigned int)((pCore - m_pCores) + 1);
            return ((idx < m_coreCount) ? &m_pCores[idx] : NULL);
        }

        ~GlobalNode()
        {
            delete m_pTopologyObject;
        }

        //**************************************************
        // Topology Walking:
        //

        //
        // TRANSITION: This needs to be eliminated.  There are a slew of memcpy and memset operations on GlobalCore and GlobalNode which
        // assume this is how they initialize and copy the base class.  We cannot add virtual methods on GlobalCore without cleaning ALL of that
        // up.  Hence, we heap alloc a side object until such time as it can be cleaned up.
        //
        struct TopologyObject : public ITopologyNode
        {
            GlobalNode *m_pNode;

            TopologyObject(GlobalNode *pNode) : m_pNode(pNode)
            {
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
            virtual ITopologyNode *GetNext() const;

            /// <summary>
            ///     Returns the resource manager's unique identifier for this node.
            /// </summary>
            /// <returns>
            ///     The resource manager's unique identifier for this node.
            /// </returns>
            /// <remarks>
            ///     The Concurrency Runtime represents hardware threads on the system in groups of processor nodes. Nodes are usually derived from
            ///     the hardware topology of the system. For example, all processors on a specific socket or a specific NUMA node may belong to the
            ///     same processor node. The Resource Manager assigns unique identifiers to these nodes starting with <c>0</c> up to and including
            ///     <c>nodeCount - 1</c>, where <c>nodeCount</c> represents the total number of processor nodes on the system.
            ///     <para>The count of nodes can be obtained from the function <see cref="GetProcessorNodeCount Function">GetProcessorNodeCount</see>.</para>
            /// </remarks>
            /**/
            virtual unsigned int GetId() const
            {
                return m_pNode->m_id;
            }

            /// <summary>
            ///     Returns the Windows assigned NUMA node number to which this Resource Maanger node belongs.
            /// </summary>
            /// <returns>
            ///     The Windows assigned NUMA node number to which this Resource Manager node belongs.
            /// </returns>
            /// <remarks>
            ///     A thread proxy running on a virtual processor root belonging to this node will have affinity to at least the NUMA node level for the NUMA
            ///     node returned by this method.
            /// </remarks>
            /**/
            virtual DWORD GetNumaNode() const
            {
                return m_pNode->m_numaNodeNumber;
            }

            /// <summary>
            ///     Returns the number of execution resources grouped together under this node.
            /// </summary>
            /// <returns>
            ///     The number of execution resources grouped together under this node.
            /// </returns>
            /// <seealso cref="ITopologyNode::GetFirstExecutionResource Method"/>
            /**/
            virtual unsigned int GetExecutionResourceCount() const
            {
                return m_pNode->m_coreCount;
            }

            /// <summary>
            ///     Returns the first execution resource grouped under this node in enumeration order.
            /// </summary>
            /// <returns>
            ///     The first execution resource grouped under this node in enumeration order.
            /// </returns>
            /// <seealso cref="ITopologyNode::GetExecutionResourceCount Method"/>
            /**/
            virtual ITopologyExecutionResource *GetFirstExecutionResource() const
            {
                return m_pNode->m_pCores->m_pTopologyObject;
            }
        } *m_pTopologyObject;
    };

    /// <summary>
    ///     Representation of a NUMA node within the RM's global map of execution resources.  Information in this struct represents
    ///     a systemwide view of the underlying NUMA node.
    /// </summary>
    struct GlobalNumaNode
    {
        // The affinity mask of the NUMA node.
        ULONG_PTR m_numaAffinity;

        // The group number in the Win7 {group, mask} id format.
        unsigned int m_processorGroup;

        // The NUMA node number for this NUMA node
        DWORD m_numaNodeNumber;

        /// <summary>
        ///     Initializes a numa node.
        /// </summary>
        void Initialize(DWORD numaNodeNumber, USHORT processorGroup, ULONG_PTR affinityMask)
        {
            m_numaNodeNumber = numaNodeNumber;
            m_processorGroup = processorGroup;
            m_numaAffinity = affinityMask;
        }
    };

    /// <summary>
    ///     Used to store information during static and dynamic allocation.
    /// </summary>
    struct AllocationData
    {
        // Index into an array of schedulers - used for sorting, etc.
        unsigned int m_index;

        // Additional allocation to give to a scheduler after proportional allocation decisions are made.
        unsigned int m_allocation;

        // Used to hold a scaled allocation value during proportional allocation.
        double m_scaledAllocation;

        // The scheduler proxy this allocation data is for.
        SchedulerProxy *m_pProxy;

        // Number of idle cores in a scheduler proxy during static allocation or dynamic core migration.
        unsigned int m_numIdleCores;

        // Number of idle cores in a scheduler proxy during static allocation or dynamic core migration that are also borrowed. During core
        // migration these cores are the first to go.
        unsigned int m_numBorrowedIdleCores;
    };

    struct StaticAllocationData : public AllocationData
    {
        // A field used during static allocation to decide on an allocation proportional to each scheduler's desired value.
        double m_adjustedDesired;

        // Tells if a thread subscription is a part of this static allocation request.
        bool m_fNeedsExternalThreadAllocation;

        // Keeps track of stolen cores during static allocation.
        unsigned int m_numCoresStolen;
    };

    struct DynamicAllocationData : public AllocationData
    {
        // This variable is toggled back in forth during dynamic migration to instruct the RM whether or not
        // an exact fit allocation should be attempted - i.e. if a node has 3 available cores, but this scheduler proxy
        // needs only 2, keep searching in case a later node is found with 2 available cores.
        bool m_fExactFitAllocation;

        // Fully loaded is set to true when a scheduler is using all the cores that are allocated to it (no cores are idle)
        // AND it has less than its desired number of cores.
        bool m_fFullyLoaded;

        // Number suggested as an appropriate allocation for the scheduler proxy, by the hill climbing instance.
        unsigned int m_suggestedAllocation;

#if defined(CONCRT_TRACING)
        unsigned int m_originalSuggestedAllocation;
#endif

        union
        {
#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
            // Struct used for a receiving proxy.
            struct
            {
                // Number of nodes in the scheduler proxy that are partially allocated.
                unsigned int m_numPartiallyFilledNodes;

                // As we go through dynamic allocation, the starting node index moves along the array of sorted nodes,
                // in a scheduling proxy that is receiving cores.
                unsigned int m_startingNodeIndex;
            };
            // Struct used for a giving proxy.
            struct
            {
                // Maximum number of borrowed idle cores this scheduler can give up.
                unsigned int m_borrowedIdleCoresToMigrate;

                // Maximum number of borrowed in-use cores this scheduler can give up.
                unsigned int m_borrowedInUseCoresToMigrate;

                // Maximum number of owned cores this scheduler can give up.
                unsigned int m_ownedCoresToMigrate;
            };
#pragma warning(pop)
        };
    };
} // namespace details
}; // namespace Concurrency
