// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingNode.h
//
// Source file containing the SchedulingNode declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    /// A scheduling node corresponds to a NUMA node or a processor package; containing one or more virtual processor groups.
    /// </summary>
    class SchedulingNode
    {
    public:

        /// <summary>
        ///     Constructs a scheduling node.
        /// </summary>
        SchedulingNode(const QuickBitSet& resourceSet, DWORD numaNodeNumber, SchedulingRing *pRing);

        /// <summary>
        ///     Destroys a scheduling node.
        /// </summary>
        ~SchedulingNode();

        /// <summary>
        ///     Creates and adds a new virtual processor in the node to associated with the root provided.
        ///     NOTE: For non-oversubscribed vprocs this API is currently will only work for intial allocation.
        /// </summary>
        /// <param name="pOwningRoot">
        ///     The virtual processor root to create the virtual processor with.
        /// </param>
        /// <param name="fOversubscribed">
        ///     True if this is an oversubscribed virtual processor.
        /// </param>
        /// <returns>
        ///     The newly created virtual processor.
        /// </returns>
        VirtualProcessor* AddVirtualProcessor(IVirtualProcessorRoot *pOwningRoot, bool fOversubscribed = false);

        /// <summary>
        ///     Returns the scheduler associated with the node.
        /// </summary>
        SchedulerBase * GetScheduler() { return m_pScheduler; }

        /// <summary>
        ///     Returns the scheduling ring associated with the node.
        /// </summary>
        SchedulingRing * GetSchedulingRing() { return m_pRing; }

        /// <summary>
        ///     Find the virtual processor in this node that matches the root provided.
        /// </summary>
        /// <param name="pRoot">
        ///     The virtual processor root to match.
        /// </param>
        VirtualProcessor* FindMatchingVirtualProcessor(IVirtualProcessorRoot* pRoot);

        /// <summary>
        ///     Returns the ID of the scheduling node.
        /// </summary>
        int Id() const
        {
            return m_id;
        }

        /// <summary>
        ///     Returns the first virtual processor in the non-cyclic range [min, max).  If such is found
        ///     the virtual processor is returned and pIdx contains its index within the list array.
        ///     If not found, NULL is returned and the value in pIdx is unspecified.
        /// </summary>
        VirtualProcessor *FindVirtualProcessor(int min, int max, int *pIdx)
        {
            VirtualProcessor *pVProc = NULL;
            int i = min;
            for (; i < max && pVProc == NULL; ++i)
            {
                pVProc = m_virtualProcessors[i];
            }

            //
            // The loop incremented "i" prior to the check.  If found, the index is i - 1.  If not, we care
            // not what pIdx contains.
            //
            *pIdx = i - 1;
            return pVProc;
        }

        /// <summary>
        ///     Returns the first virtual processor.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned virtual processor will be placed here.  This can only be
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextVirtualProcessor.
        /// </param>
        VirtualProcessor *GetFirstVirtualProcessor(int *pIdx)
        {
            return FindVirtualProcessor(0, m_virtualProcessors.MaxIndex(), pIdx);
        }

        /// <summary>
        ///     Returns the next virtual processor in an iteration.
        /// </summary>
        VirtualProcessor *GetNextVirtualProcessor(int *pIdx, int idxStart = 0)
        {
            VirtualProcessor *pVProc = NULL;

            int min = *pIdx + 1;
            if (min > idxStart)
            {
                pVProc = FindVirtualProcessor(min, m_virtualProcessors.MaxIndex(), pIdx);
                min = 0;
            }

            if (pVProc == NULL)
                pVProc = FindVirtualProcessor(min, idxStart, pIdx);

            return pVProc;
        }

        /// <summary>
        ///     Returns whether a virtual processor is available.
        /// </summary>
        bool HasVirtualProcessorAvailable() const
        {
            return m_virtualProcessorAvailableCount > 0;
        }

        /// <summary>
        ///     Returns whether a virtual processor is waiting for throttling.
        /// </summary>
        bool HasVirtualProcessorPendingThread() const
        {
            return m_virtualProcessorsPendingThreadCreate > 0;
        }

        /// <summary>
        ///     Returns whether a virtual processor is available to execute new work.
        /// </summary>
        bool HasVirtualProcessorAvailableForNewWork() const
        {
            //
            // The observational race (lack of atomicity between the two reads) should not matter.  If it does in some obscure
            // case, a new atomic counter can be added.
            //
            return (m_virtualProcessorAvailableCount - m_virtualProcessorsPendingThreadCreate) > 0;
        }

        /// <summary>
        ///     Gets a location object which represents the scheduling node.
        /// </summary>
        location GetLocation();

        /// <summary>
        ///     Returns a virtual processor from the given location.  The virtual processor must be within this node.
        /// </summary>
        VirtualProcessor* FindVirtualProcessorByLocation(const location* pLoc);

        /// <summary>
        ///     Determines whether the scheduling node contains an execution resource with ID as specified.  Note that this does NOT return
        ///     whether the said resource is available in the scheduler -- only whether the given resource ID is logically contained in the
        ///     node.  The scheduler may have no virtual processor with that execution resource ID at the moment.
        /// </summary>
        bool ContainsResourceId(unsigned int resourceId) /*const*/
        {
            return m_resourceBitMap.Exists(resourceId);
        }

        /// <summary>
        ///     Notifies the node of a resource that is contained within it and its assigned position in all bitmasks used by all ConcRT
        ///     schedulers.
        /// </summary>
        void NotifyResource(unsigned int resourceId, unsigned int maskId)
        {
            m_resourceBitMap.Insert(resourceId, maskId);
        }

        /// <summary>
        ///     Returns the bitset for all resources in the node.
        /// </summary>
        const QuickBitSet& GetResourceSet()
        {
            return m_resourceSet;
        }

        /// <summary>
        ///     Gets the NUMA node to which this scheduling node belongs.
        /// </summary>
        DWORD GetNumaNodeNumber() const
        {
            return m_numaNodeNumber;
        }

    private:
        friend class SchedulerBase;
        friend class VirtualProcessor;
        friend class UMSThreadVirtualProcessor;
        friend class InternalContextBase;
        friend class FairScheduleGroup;
        template <typename T> friend class ListArray;

        // Owning scheduler
        SchedulerBase *m_pScheduler;

        // Owning ring
        SchedulingRing * const m_pRing;

        // The bit-set identifying execution resources within this node for quick affinity masking.
        QuickBitSet m_resourceSet;

        // Maps resource IDs contained within the node to a mask identifier
        Hash<unsigned int, unsigned int> m_resourceBitMap;

        volatile LONG m_virtualProcessorAvailableCount;
        volatile LONG m_virtualProcessorsPendingThreadCreate;

        volatile LONG m_virtualProcessorCount;
        volatile LONG m_ramblingCount; // rambling -- searching foreign nodes for work

        DWORD m_numaNodeNumber;

        int m_id;

        // Virtual processors owned by this node.
        ListArray<VirtualProcessor> m_virtualProcessors;

        InternalContextBase *StealLocalRunnableContext(VirtualProcessor* pSkipVirtualProcessor = NULL);

        /// <summary>
        ///     Find an available virtual processor in the scheduling node.
        /// </summary>
        bool FoundAvailableVirtualProcessor(VirtualProcessor::ClaimTicket& ticket,
                                            location bias = location(),
                                            ULONG type = VirtualProcessor::AvailabilityAny);

        void Cleanup();

        // Prevent warning about generated assignment operator & copy constructors.
        SchedulingNode(const SchedulingNode&);
        void operator=(const SchedulingNode&);
    };
} // namespace details
} // namespace Concurrency
