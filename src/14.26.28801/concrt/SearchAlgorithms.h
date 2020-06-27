// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SearchAlgorithms.h
//
// Header file containing definitions for all scheduling algorithms.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Variant type representing a work item returned from a search.
    /// </summary>
    class WorkItem
    {
    public:

        /// <summary>
        ///     The type of work item.
        /// </summary>
        enum WorkItemType
        {
            //
            // Specific types:
            //

            // Empty work item
            WorkItemTypeNone = 0x0,

            // Work item is a context
            WorkItemTypeContext = 0x1,

            // Work item is a realized chore.  Bind() will associate to a context if a context can be allocated
            WorkItemTypeRealizedChore = 0x2,

            // Work item is an unrealized chore.  Bind() will associate to a context if a context can be allocated
            WorkItemTypeUnrealizedChore = 0x4,

            // Work item is a token to an realized chore.  ResolveToken() will grab the item if it is still available.
            WorkItemTypeRealizedChoreToken = 0x8,

            // Work item is a token to an unrealized chore.  ResolveToken() will grab the item if it is still available.
            WorkItemTypeUnrealizedChoreToken = 0x10,

            //
            // General Masks:
            //

            WorkItemTypeMaskAnyRealizedChore = 0xA,
            WorkItemTypeMaskAnyUnrealizedChore = 0x14

        };

        /// <summary>
        ///     Default constructor for a work item.
        /// </summary>
        WorkItem() :
            m_type(WorkItemTypeNone),
            m_pItem(NULL)
        {
        }

        /// <summary>
        ///     Constructs a work item from an internal context.
        /// </summary>
        WorkItem(InternalContextBase *pContext);

        /// <summary>
        ///     Constructs a work item from a realized chore.
        /// </summary>
        WorkItem(RealizedChore *pRealizedChore, ScheduleGroupSegmentBase *pSegment) :
            m_type(WorkItemTypeRealizedChore),
            m_pSegment(pSegment),
            m_pRealizedChore(pRealizedChore)
        {
        }

        /// <summary>
        ///    Constructs a work item from an unrealized chore.
        /// </summary>
        WorkItem(_UnrealizedChore *pUnrealizedChore, ScheduleGroupSegmentBase *pSegment) :
            m_type(WorkItemTypeUnrealizedChore),
            m_pSegment(pSegment),
            m_pUnrealizedChore(pUnrealizedChore)
        {
        }

        /// <summary>
        ///     Constructs a work item from an unrealized chore token.
        /// </summary>
        WorkItem(WorkQueue *pWorkQueue, ScheduleGroupSegmentBase *pSegment) :
            m_type(WorkItemTypeUnrealizedChoreToken),
            m_pSegment(pSegment),
            m_pWorkQueue(pWorkQueue)
        {
        }

        /// <summary>
        ///     Constructs a work item from a realized chore token.
        /// </summary>
        WorkItem(ScheduleGroupSegmentBase *pSegment) :
            m_type(WorkItemTypeRealizedChoreToken),
            m_pSegment(pSegment)
        {
        }

        /// <summary>
        ///     Transfers reference counts as necessary to inline the given work item on the given context.  This may
        ///     only be called on a work item that can be inlined (e.g.: an unbound one).
        /// </summary>
        /// <param name="pContext">
        ///     The context that is attempting to inline the work item.
        /// </param>
        void TransferReferences(InternalContextBase *pContext);

        /// <summary>
        ///     Resolves a token to an underlying work item.
        /// </summary>
        bool ResolveToken();

        /// <summary>
        ///     Binds the work item to a context and returns the context.  This may or may not allocate a new context.  Note that
        ///     act of binding which performs a context allocation will transfer a single count of work to the counter of the new
        ///     context.
        /// </summary>
        InternalContextBase *Bind();

        /// <summary>
        ///     Binds the work item to the specified context (which is allocated).  This will never allocate a new context.
        /// </summary>
        void BindTo(InternalContextBase *pContext);

        /// <summary>
        ///     Invokes the work item.
        /// </summary>
        void Invoke();

        /// <summary>
        ///     Accessor for type.
        /// </summary>
        WorkItemType GetType() const
        {
            return m_type;
        }

        /// <summary>
        ///     Returns the work item.
        /// </summary>
        void *GetItem() const
        {
            return m_pItem;
        }

        /// <summary>
        ///     Returns whether the work item is a context or not.
        /// </summary>
        bool IsContext() const
        {
            return (m_type == WorkItemTypeContext);
        }

        /// <summary>
        ///     Returns whether the work item is a token or not.
        /// </summary>
        bool IsToken() const
        {
            return ((m_type & (WorkItemTypeRealizedChoreToken | WorkItemTypeUnrealizedChoreToken)) != 0);
        }

        /// <summary>
        ///     Accessor for a context.
        /// </summary>
        InternalContextBase *GetContext() const
        {
            CONCRT_COREASSERT(m_type == WorkItemTypeContext);
            return m_pContext;
        }

        /// <summary>
        ///     Accessor for a realized chore.
        /// </summary>
        RealizedChore *GetRealizedChore() const
        {
            CONCRT_COREASSERT(m_type == WorkItemTypeRealizedChore);
            return m_pRealizedChore;
        }

        /// <summary>
        ///     Accessor for an unrealized chore.
        /// </summary>
        _UnrealizedChore *GetUnrealizedChore() const
        {
            CONCRT_COREASSERT(m_type == WorkItemTypeUnrealizedChore);
            return m_pUnrealizedChore;
        }

        /// <summary>
        ///     Accessor for the schedule group segment.
        /// </summary>
        ScheduleGroupSegmentBase *GetScheduleGroupSegment() const
        {
            return m_pSegment;
        }

        /// <summary>
        ///     Accessor for the schedule group.
        /// </summary>
        ScheduleGroupBase *GetScheduleGroup() const;

    private:

        // The type of work item
        WorkItemType m_type;

        // The schedule group that the work item was found in.
        ScheduleGroupSegmentBase *m_pSegment;

        // The work item itself
        union
        {
            void *m_pItem;
            WorkQueue *m_pWorkQueue;
            InternalContextBase *m_pContext;
            RealizedChore *m_pRealizedChore;
            _UnrealizedChore *m_pUnrealizedChore;
        };

    };

    /// <summary>
    ///     A class which tracks iterator state for a search-for-work.  This is generic in terms of search algorithm.
    /// </summary>
    class WorkSearchContext
    {
    public:

        /// <summary>
        ///     Describes the search algorithm being utilized.
        /// </summary>
        enum Algorithm
        {
            AlgorithmNotSet = 0,
            AlgorithmCacheLocal,
            AlgorithmFair
        };

        /// <summary>
        ///     Describes the type of affinity we are allowed to search for.
        /// </summary>
        enum SearchAffinity
        {
            // Search for non-affine work within the domain of the search.
            SearchNonAffine,

            // Search for work which is affine to the search context within the domain of the search.
            SearchAffineLocal,

            // Search for work which is affine to something OTHER than the search context within the domain of the search.
            SearchAffineNotMe
        };

        /// <summary>
        ///     Constructs a work search context that will be reset later.
        /// </summary>
        WorkSearchContext() : m_pVirtualProcessor(NULL), m_pScheduler(NULL), m_pSearchFn(NULL), m_pSearchYieldFn(NULL)
        {
        }

        /// <summary>
        ///     Constructs a work search context (an iterator position for a search algorithm).
        /// </summary>
        WorkSearchContext(VirtualProcessor *pVirtualProcessor, Algorithm algorithm)
        {
            Reset(pVirtualProcessor, algorithm);
        }

        /// <summary>
        ///     Resets the work search context to utilize the specified algorithm at the starting iterator position.
        /// </summary>
        /// <param name="pVirtualProcessor">
        ///     The virtual processor binding the searching.
        /// </param>
        /// <param name="algorithm">
        ///     What algorithm to reset the iterator with.
        /// </param>
        void Reset(VirtualProcessor *pVirtualProcessor, Algorithm algorithm);

        /// <summary>
        ///     Searches from the last iterator position according to the set algorithm.  This can return any type of work
        ///     item (context, realized chore, or unrealized chore)
        /// </summary>
        /// <param name="pWorkItem">
        ///     Upon successful return, the resulting work item is placed here along with information about what group it was found in, etc...
        /// </param>
        /// <param name="pOriginSegment">
        ///     The schedule group segment of the context which is performing the search.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <param name="allowableTypes">
        ///     What type of work items are allowed to be returned.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool Search(WorkItem *pWorkItem,
                    ScheduleGroupSegmentBase *pOriginSegment,
                    bool fLastPass = false,
                    ULONG allowableTypes = WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChore | WorkItem::WorkItemTypeUnrealizedChore)
        {
            return (this->*m_pSearchFn)(pWorkItem, pOriginSegment, fLastPass, allowableTypes);
        }


        /// <summary>
        ///     Searches from the last iterator position according to the set algorithm for a yield.  This can return any type of
        ///     work item (context, realized chore, or unrealized chore)
        /// </summary>
        bool YieldingSearch(WorkItem *pWorkItem,
                                ScheduleGroupSegmentBase *pOriginSegment,
                                bool fLastPass = false,
                                ULONG allowableTypes = WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChore)
        {
            return (this->*m_pSearchYieldFn)(pWorkItem, pOriginSegment, fLastPass, allowableTypes);
        }

    private:

        // **************************************************
        // Common:
        //

        /// <summary>
        ///     Describes where the bias towards certain lists is at within SFW.
        /// </summary>
        enum BiasStageType
        {
            BiasStageNone,
            BiasStageFlipLRC,
            BiasStageSkipLRC
        };

        // The virtual processor binding the search.
        VirtualProcessor *m_pVirtualProcessor;

        // The scheduler
        SchedulerBase *m_pScheduler;

        // The mask ID of the virtual processor binding the search.
        unsigned int m_maskId;

        // How many times work has been found in the LRC since the last reset.
        ULONG m_LRCBias;

        // The service time stamp
        ULONGLONG m_serviceTick;

        // TRANSITION: This goes with real priority...
        ULONGLONG m_lastPriorityPull;

        // The search function to utilize.
        bool (WorkSearchContext::*m_pSearchFn)(WorkItem *pWorkItem,
                                               ScheduleGroupSegmentBase *pOriginSegment,
                                               bool fLastPass,
                                               ULONG allowableTypes);

        // The search function to utilize for yielding.
        bool (WorkSearchContext::*m_pSearchYieldFn)(WorkItem *pWorkItem,
                                                    ScheduleGroupSegmentBase *pOriginSegment,
                                                    bool fLastPass,
                                                    ULONG allowableTypes);

        //
        // TRANSITION: This goes with real priority...
        //
        bool CheckPriorityList(ULONGLONG serviceTime)
        {
            bool fCheck = ((serviceTime - m_lastPriorityPull) > (ULONGLONG)1000);
            if (fCheck)
                m_lastPriorityPull = serviceTime;

            return fCheck;

        }

        /// <summary>
        ///     Performs a quick search of a particular segment.
        /// </summary>
        bool QuickSearch(ScheduleGroupSegmentBase *pQCSegment,
                         WorkItem *pWorkItem,
                         bool fLastPass,
                         ULONG allowableTypes);

        /// <summary>
        ///     Performs a quick yielding search of a particular segment.
        /// </summary>
        bool QuickSearchYield(ScheduleGroupSegmentBase *pQCSegment,
                              WorkItem *pWorkItem,
                              bool fLastPass,
                              ULONG allowableTypes);

        /// <summary>
        ///     Performs a pre-search for any "special" contexts (e.g.: the UMS SUT)
        /// </summary>
        bool PreSearch(WorkItem *pWorkItem);

        /// <summary>
        ///     Steals a local runanble from a virtual processor within the specified node.  Note that this allows a given virtual processor
        ///     to be skipped.
        /// </summary>
        bool StealLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pNode, VirtualProcessor *pSkipVirtualProcessor);

        /// <summary>
        ///     Steals a local runnable from a virtual processor of any scheduling node other than the specified local node.
        /// </summary>
        bool StealForeignLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pLocalNode);

        /// <summary>
        ///     Gets a local runnable context from the specified virtual processor.
        /// </summary>
        bool GetLocalRunnable(WorkItem *pWorkItem, VirtualProcessor *pVirtualProcessor, bool fYieldingSearch);

        /// <summary>
        ///     Gets a runnable from the specified schedule group segment.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pSegment">
        ///     The schedule group segment in which to look for a runnable context.
        /// </param>
        /// <returns>
        ///     An indication of whether or not a runnable context was found in the segment.
        /// </returns>
        bool GetRunnableContext(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment);

        /// <summary>
        ///     Gets a realized chore from the specified schedule group segment.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pSegment">
        ///     The schedule group segment in which to look for a realized chore.
        /// </param>
        /// <param name="fRealWork">
        ///     If true, the actual work item is returned.  If false, a token to the work is returned.  The work is not dequeued until the token
        ///     is resolved.
        /// </param>
        /// <returns>
        ///     An indication of whether or not a realized chore was found in the segment.
        /// </returns>
        bool GetRealizedChore(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fRealWork);

        /// <summary>
        ///     Gets an unrealized chore from the specified schedule group segment.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pSegment">
        ///     The schedule group segment in which to look for an unrealized chore.
        /// </param>
        /// <param name="fForceStealLocalized">
        ///     Whether to steal the task at the bottom end of the work stealing queue even if it is an affinitized to a location
        ///     that has active searches. This is set to true on the final SFW pass to ensure a vproc does not deactivate while there
        ///     are chores higher up in the queue that are un-affinitized and therefore inaccessible via a mailbox.
        /// </param>
        /// <param name="fRealWork">
        ///     If true, the actual work item is returned.  If false, a token to the work is returned.  The work is not dequeued until the token
        ///     is resolved.
        /// </param>
        /// <returns>
        ///     An indication of whether or not an unrealized chore was found in the segment.
        /// </returns>
        bool GetUnrealizedChore(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fForceStealLocalized, bool fRealWork);

        /// <summary>
        ///     Determines if a segment should be skipped given the search parameters and the segment's affinity.
        /// </summary>
        /// <param name="pSegment">
        ///     The segment to query about skipping.
        /// </param>
        /// <param name="pSkipSegment">
        ///     A segment which should be arbitrarily skipped regardless of affinity type.  This parameter can be NULL.
        /// </param>
        /// <param name="affinity">
        ///     The search affinity type to query for.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication as to whether pSegment should be skipped according to the pSkipSegment and affinity parameters.
        /// </returns>
        bool SkipSegmentSearch(ScheduleGroupSegmentBase *pSegment, ScheduleGroupSegmentBase *pSkipSegment, SearchAffinity affinity, bool fLastPass);

        /// <summary>
        ///     Searches the schedule group to which pSegment belongs to find a runnable.  The group is searched for segments according to the specified
        ///     affinity type.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If an appropriate runnable is found, the resulting work item will be placed here.
        /// </param>
        /// <param name="pSegment">
        ///     A segment within the group to search.  This segment has bias within the group if it matches the specified affinity type.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool GetRunnableContextWithinGroup(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, SearchAffinity affinity, bool fLastPass);

        /// <summary>
        ///     Searches the schedule group to which pSegment belongs to find a realized chore.  The group is searched for segments according to the specified
        ///     affinity type.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If an appropriate realized chore is found, the resulting work item will be placed here.
        /// </param>
        /// <param name="pSegment">
        ///     A segment within the group to search.  This segment has bias within the group if it matches the specified affinity type.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool GetRealizedChoreWithinGroup(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fRealWork, SearchAffinity affinity, bool fLastPass);

        /// <summary>
        ///     Searches the schedule group to which pSegment belongs to find an unrealized chore.  The group is searched for segments according to the
        ///     specified affinity type.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If an appropriate unrealized chore is found, the resulting work item will be placed here.
        /// </param>
        /// <param name="pSegment">
        ///     A segment within the group to search.  This segment has bias within the group if it matches the specified affinity type.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool GetUnrealizedChoreWithinGroup(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fRealWork, SearchAffinity affinity, bool fLastPass);

        /// <summary>
        ///     Called on any biased work.
        /// </summary>
        void LRCBias()
        {
            m_LRCBias++;
        }

        /// <summary>
        ///     Resets the local bias counter but not the ring bias counter.
        /// </summary>
        void ResetLRCBias()
        {
            m_LRCBias = 0;
        }

        /// <summary>
        ///     Returns the current stage of local bias.
        /// </summary>
        BiasStageType BiasStage()
        {
            if (m_LRCBias < 101)
                return BiasStageNone; // (fwd) Normal --> LRC LIFO
            else if (m_LRCBias < 127)
                return BiasStageFlipLRC; // (fwd) Flip LRC --> LRC FIFO
            else
                return BiasStageSkipLRC; // (fwd) Skip LRC --> runnables
        }

        // **************************************************
        // Cache Local Algorithm:
        //

        /// <summary>
        ///     Searches for a runnable within the specified ring.  Before searching elsewhere, it searches the segment and group specified by
        ///     pBiasSegment according to the rules of the search and the requested affinity type.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pRing">
        ///     The scheduling ring to search.
        /// </param>
        /// <param name="pBiasSegment">
        ///     The segment to bias the search to.  This segment and its corresponding group are searched first!
        /// </param>
        /// <param name="fOtherLocalLRCCheck">
        ///     Determines whether or not to check other local LRCs in this search.
        /// </param>
        /// <param name="affinity">
        ///     The search affinity type to query for.
        /// </param>
        /// <param name="allowableTypes">
        ///     A bitmap of work-types allowed to be returned from the search.  This indicates whether or not runnables, realized chores, or unrealized chores
        ///     can be returned as well as whether the actual work item or only a token should be returned.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication of whether a runnable was found in the bias segment, group, or the specified ring.
        /// </returns>
        bool SearchCacheLocal_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupSegmentBase *pBiasSegment,
                                        bool fOtherLocalLRCCheck, SearchAffinity affinity, ULONG allowableTypes, bool fLastPass);

        /// <summary>
        ///     Searches for a realized chore within the specified ring.  Before searching elsewhere, it searches the segment and group specified by
        ///     pBiasSegment according to the rules of the search and the requested affinity type.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pRing">
        ///     The scheduling ring to search.
        /// </param>
        /// <param name="pBiasSegment">
        ///     The segment to bias the search to.  This segment and its corresponding group are searched first!
        /// </param>
        /// <param name="fRealWork">
        ///     If true, the actual work item is returned.  If false, a token to the work is returned.  The work is not dequeued until the token
        ///     is resolved.
        /// </param>
        /// <param name="affinity">
        ///     The search affinity type to query for.
        /// </param>
        /// <param name="allowableTypes">
        ///     A bitmap of work-types allowed to be returned from the search.  This indicates whether or not runnables, realized chores, or unrealized chores
        ///     can be returned as well as whether the actual work item or only a token should be returned.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication of whether a realized chore was found in the bias segment, group, or the specified ring.
        /// </returns>
        bool SearchCacheLocal_Realized(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupSegmentBase *pBiasSegment,
                                       bool fRealWork, SearchAffinity affinity, ULONG allowableTypes, bool fLastPass);

        /// <summary>
        ///     Searches for an unrealized chore within the specified ring.  Before searching elsewhere, it searches the segment and group specified by
        ///     pBiasSegment according to the rules of the search and the requested affinity type.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pRing">
        ///     The scheduling ring to search.
        /// </param>
        /// <param name="pBiasSegment">
        ///     The segment to bias the search to.  This segment and its corresponding group are searched first!
        /// </param>
        /// <param name="fRealWork">
        ///     If true, the actual work item is returned.  If false, a token to the work is returned.  The work is not dequeued until the token
        ///     is resolved.
        /// </param>
        /// <param name="affinity">
        ///     The search affinity type to query for.
        /// </param>
        /// <param name="allowableTypes">
        ///     A bitmap of work-types allowed to be returned from the search.  This indicates whether or not runnables, realized chores, or unrealized chores
        ///     can be returned as well as whether the actual work item or only a token should be returned.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <returns>
        ///     An indication of whether an unrealized chore was found in the bias segment, group, or the specified ring.
        /// </returns>
        bool SearchCacheLocal_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupSegmentBase *pBiasSegment,
                                         bool fRealWork, SearchAffinity affinity, ULONG allowableTypes, bool fLastPass);

        /// <summary>
        ///     Searches for work within the scheduler according to the cache local (schedule group local) search algorithm.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pOriginSegment">
        ///     The segment to bias the search to.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <param name="allowableTypes">
        ///     A bitmap of work-types allowed to be returned from the search.  This indicates whether or not runnables, realized chores, or unrealized chores
        ///     can be returned as well as whether the actual work item or only a token should be returned.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool SearchCacheLocal(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes);

        /// <summary>
        ///     Searches for work within the scheduler according to the cache local (schedule group local) search algorithm for yielding.
        /// </summary>
        /// <param name="pWorkItem">
        ///     If a work item is found, the work item will be returned here.
        /// </param>
        /// <param name="pOriginSegment">
        ///     The segment to bias the search to.
        /// </param>
        /// <param name="fLastPass">
        ///     An indication as to whether this is a last pass SFW.
        /// </param>
        /// <param name="allowableTypes">
        ///     A bitmap of work-types allowed to be returned from the search.  This indicates whether or not runnables, realized chores, or unrealized chores
        ///     can be returned as well as whether the actual work item or only a token should be returned.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool SearchCacheLocalYield(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes);

        // **************************************************
        // Fair Algorithm:
        //

        /// <summary>
        ///     Performs a fair search for runnables in the specified ring.
        /// </summary>
        bool SearchFair_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing);

        /// <summary>
        ///     Performs a fair search for realized chores in the specified ring.
        /// </summary>
        bool SearchFair_Realized(WorkItem *pWorkItem, SchedulingRing *pRing, bool fRealItem);

        /// <summary>
        ///     Performs a fair search for unrealized chores in the specified ring.
        /// </summary>
        bool SearchFair_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing, bool fRealItem);

        /// <summary>
        ///     Performs a fair search for work.
        /// </summary>
        bool SearchFair(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes);

        /// <summary>
        ///     Performs a fair search for work in the yielding case.
        /// </summary>
        bool SearchFairYield(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes);

    };

}
}
