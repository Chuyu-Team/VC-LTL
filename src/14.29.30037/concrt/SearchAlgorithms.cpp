// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SearchAlgorithms.cpp
//
// Implementation file containing all scheduling algorithms.
//
// **PLEASE NOTE**:
//
//     Any search algorithm in here must be fully reentrant.  On UMS schedulers, the UMS primary will invoke these routines
//     to perform a search for work.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    //***************************************************************************
    //
    // General:
    //

    /// <summary>
    ///     Constructs a work item from an internal context.
    /// </summary>
    WorkItem::WorkItem(InternalContextBase *pContext) :
        m_type(WorkItemTypeContext),
        m_pSegment(pContext->GetScheduleGroupSegment()),
        m_pContext(pContext)
    {
    }

    /// <summary>
    ///     Resolves a token to an underlying work item.
    /// </summary>
    bool WorkItem::ResolveToken()
    {
        CONCRT_COREASSERT(IsToken());
        switch(m_type)
        {
            case WorkItemTypeRealizedChoreToken:
            {
                RealizedChore *pChore = m_pSegment->GetRealizedChore();
                if (pChore != NULL)
                {

#if (_CHORETRACK && _UMSTRACE)
                    ContextBase *pContext = SchedulerBase::FastCurrentContext();
                    ASSERT(pContext);
                    OMTRACE(MTRACE_EVT_TOKENRESOLVE, pContext, pContext, NULL, pChore);
                    OMTRACE(MTRACE_EVT_TOKENRESOLVE, pContext->GetScheduler(), pContext, NULL, pChore);
#endif // _CHORETRACK && _UMSTRACE

                    m_pRealizedChore = pChore;
                    m_type = WorkItemTypeRealizedChore;
                }
                break;
            }
            case WorkItemTypeUnrealizedChoreToken:
            {
                if (m_pWorkQueue == MAILBOX_LOCATION)
                {
                    _UnrealizedChore *pChore;
                    if (!m_pSegment->m_mailedTasks.Dequeue(&pChore))
                        pChore = NULL;

                    if (pChore != NULL)
                    {
                        // The chore may not be from a detached workqueue, but since it is dequeued from a mailbox, we set it as detached
                        // which will add the stealing context to a list in the task collection instead of the owning contexts stealer collection.
                        pChore->_SetDetached(true);
                        m_pUnrealizedChore = pChore;
                        m_type = WorkItemTypeUnrealizedChore;
                    }
                }
                else
                {
                    _UnrealizedChore *pChore = m_pWorkQueue->Steal(false);
                    if (pChore != NULL)
                    {

#if (_CHORETRACK && _UMSTRACE)
                        ContextBase *pContext = SchedulerBase::FastCurrentContext();
                        ASSERT(pContext);
                        OMTRACE(MTRACE_EVT_TOKENRESOLVE, pContext, pContext, NULL, pChore);
                        OMTRACE(MTRACE_EVT_TOKENRESOLVE, pContext->GetScheduler(), pContext, NULL, pChore);
#endif // _CHORETRACK && _UMSTRACE

                        m_pUnrealizedChore = pChore;
                        m_type = WorkItemTypeUnrealizedChore;

                    }
                    break;
                }
            }
        }

        return !IsToken();
    }

    /// <summary>
    ///     Binds the work item to a context and returns the context.  This may or may not allocate a new context.  Note that
    ///     act of binding which performs a context allocation will transfer a single count of work to the counter of the new
    ///     context.
    /// </summary>
    InternalContextBase *WorkItem::Bind()
    {
        if (IsToken() && !ResolveToken())
            return NULL;

        switch(m_type)
        {
            case WorkItemTypeUnrealizedChore:
                m_pContext = m_pSegment->GetInternalContext(m_pUnrealizedChore, true);
                if (m_pContext != NULL)
                {
                    m_pContext->SaveDequeuedTask();
                    m_type = WorkItemTypeContext;
                }
                break;
            case WorkItemTypeRealizedChore:
                m_pContext = m_pSegment->GetInternalContext(m_pRealizedChore);
                if (m_pContext != NULL)
                {
                    m_pContext->SaveDequeuedTask();
                    m_type = WorkItemTypeContext;
                }
                break;
            case WorkItemTypeContext:
                break;
        }

        return m_pContext;
    }

    /// <summary>
    ///     Binds the work item to the specified context (which is allocated).  This will never allocate a new context.
    /// </summary>
    void WorkItem::BindTo(InternalContextBase *pContext)
    {
        switch(m_type)
        {
            case WorkItemTypeUnrealizedChore:
                pContext->PrepareForUse(m_pSegment, m_pUnrealizedChore, true);
                break;
            case WorkItemTypeRealizedChore:
                pContext->PrepareForUse(m_pSegment, m_pRealizedChore, false);
                break;
        }

        m_pContext = pContext;
        m_type = WorkItemTypeContext;
    }

    /// <summary>
    ///     Invokes the work item.
    /// </summary>
    void WorkItem::Invoke()
    {
        CONCRT_COREASSERT(m_type == WorkItemTypeRealizedChore || m_type == WorkItemTypeUnrealizedChore);
        switch(m_type)
        {
            case WorkItemTypeUnrealizedChore:
                m_pUnrealizedChore->_Invoke();
                break;
            case WorkItemTypeRealizedChore:
                m_pRealizedChore->Invoke();
                m_pSegment->GetGroup()->GetScheduler()->ReleaseRealizedChore(m_pRealizedChore);
                break;
        }
    }

    /// <summary>
    ///     Transfers reference counts as necessary to inline the given work item on the given context.  This may
    ///     only be called on a work item that can be inlined (e.g.: an unbound one).
    /// </summary>
    /// <param name="pContext">
    ///     The context that is attempting to inline the work item.
    /// </param>
    void WorkItem::TransferReferences(InternalContextBase *pContext)
    {
        ASSERT(m_type == WorkItemTypeRealizedChore || m_type == WorkItemTypeUnrealizedChore);

        ScheduleGroupSegmentBase *pSegment = pContext->GetScheduleGroupSegment();
        if (m_type == WorkItemTypeRealizedChore)
        {
            if (pSegment->GetGroup() != m_pSegment->GetGroup())
            {
                pContext->SwapScheduleGroupSegment(m_pSegment, false);
            }
            else
            {
                //
                // If newGroup is the same as the existing group, we need to release a reference since both, the context,
                // and the realized chore, have a reference on the schedule group, and we only need to hold one reference.
                //
                OMTRACE(MTRACE_EVT_WORKITEMDEREFERENCE, pSegment->GetGroup(), NULL, NULL, 0);
                pSegment->GetGroup()->InternalRelease();
            }

        }
        else if (pSegment->GetGroup() != m_pSegment->GetGroup())
        {
            pContext->SwapScheduleGroupSegment(m_pSegment, true);
        }
    }

    /// <summary>
    ///     Resets the work search context to utilize the specified algorithm at the starting iterator position.
    /// </summary>
    /// <param name="pVirtualProcessor">
    ///     The virtual processor binding the searching.
    /// </param>
    /// <param name="algorithm">
    ///     The algorithm to reset the iterator with.
    /// </param>
    void WorkSearchContext::Reset(VirtualProcessor *pVirtualProcessor, Algorithm algorithm)
    {
        m_LRCBias = 0;;
        m_pVirtualProcessor = pVirtualProcessor;
        m_maskId = m_pVirtualProcessor->GetMaskId();
        m_pScheduler = pVirtualProcessor->GetOwningNode()->GetScheduler();
        m_serviceTick = m_lastPriorityPull = platform::__GetTickCount64();

        switch(algorithm)
        {
            case AlgorithmCacheLocal:
                m_pSearchFn = &WorkSearchContext::SearchCacheLocal;
                m_pSearchYieldFn = &WorkSearchContext::SearchCacheLocalYield;
                break;
            case AlgorithmFair:
                m_pSearchFn = &WorkSearchContext::SearchFair;
                m_pSearchYieldFn = &WorkSearchContext::SearchFairYield;
                break;
            default:
                ASSERT(false);
        }
    }

    /// <summary>
    ///     Steals a local runnable from a virtual processor within the specified node.  Note that this allows a given virtual processor
    ///     to be skipped.
    /// </summary>
    bool WorkSearchContext::StealLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pNode, VirtualProcessor *pSkipVirtualProcessor)
    {
        int idx;
        VirtualProcessor *pVProc = pNode->GetFirstVirtualProcessor(&idx);
        while (pVProc != NULL)
        {
            if (pVProc != pSkipVirtualProcessor)
            {
                pVProc->ServiceMark(m_serviceTick);
                InternalContextBase *pContext = pVProc->StealLocalRunnableContext();
                if (pContext != NULL)
                {
                    *pWorkItem = WorkItem(pContext);
                    return true;
                }
            }

            pVProc = pNode->GetNextVirtualProcessor(&idx);
        }

        return false;
    }

    /// <summary>
    ///     Steals a local runnable from a virtual processor of any scheduling node other than the specified local node.
    /// </summary>
    bool WorkSearchContext::StealForeignLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pLocalNode)
    {
        int idx;
        SchedulingNode *pNode = m_pScheduler->GetFirstSchedulingNode(&idx);
        while (pNode != NULL)
        {
            if (pNode != pLocalNode)
            {
                if (StealLocalRunnable(pWorkItem, pNode, NULL))
                    return true;
            }

            pNode = m_pScheduler->GetNextSchedulingNode(&idx);
        }

        return false;

    }

    /// <summary>
    ///     Performs a pre-search for any "special" contexts (e.g.: the UMS SUT)
    /// </summary>
    bool WorkSearchContext::PreSearch(WorkItem *pWorkItem)
    {
        InternalContextBase *pContext = m_pVirtualProcessor->PreRunnableSearch();
        if (pContext != NULL)
        {
            *pWorkItem = WorkItem(pContext);
            return true;
        }

        return false;
    }

    /// <summary>
    ///     Gets a local runnable context from the specified virtual processor.
    /// </summary>
    bool WorkSearchContext::GetLocalRunnable(WorkItem *pWorkItem, VirtualProcessor *pVirtualProcessor, bool fYieldingSearch)
    {
        if (fYieldingSearch)
        {
            InternalContextBase *pContext = pVirtualProcessor->GetLocalRunnableContext();
            if (pContext != NULL)
            {
                *pWorkItem = WorkItem(pContext);
                return true;
            }
        }
        else
        {
            BiasStageType biasStage = BiasStage();
            if (biasStage < BiasStageSkipLRC)
            {
                InternalContextBase *pContext = (biasStage == BiasStageFlipLRC) ? pVirtualProcessor->StealLocalRunnableContext() : pVirtualProcessor->GetLocalRunnableContext();
                if (pContext != NULL)
                {
                    *pWorkItem = WorkItem(pContext);
                    LRCBias();
                    return true;
                }
            }

            ResetLRCBias();
        }
        return false;
    }

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
    bool WorkSearchContext::GetRunnableContext(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment)
    {
        InternalContextBase *pContext = pSegment->GetRunnableContext();
        if (pContext != NULL)
        {
            *pWorkItem = WorkItem(pContext);
            return true;
        }

        return false;
    }

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
    bool WorkSearchContext::GetRealizedChore(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fRealWork)
    {
        if (fRealWork)
        {
            RealizedChore *pRealizedChore = pSegment->GetRealizedChore();
            if (pRealizedChore != NULL)
            {
                *pWorkItem = WorkItem(pRealizedChore, pSegment);
                return true;
            }
        }
        else
        {
            if (pSegment->HasRealizedChores())
            {
                *pWorkItem = WorkItem(pSegment);
                return true;
            }
        }

        return false;
    }

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
    bool WorkSearchContext::GetUnrealizedChore(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fForceStealLocalized, bool fRealWork)
    {
        if (fRealWork)
        {
            _UnrealizedChore *pUnrealizedChore = pSegment->StealUnrealizedChore(fForceStealLocalized);
            if (pUnrealizedChore != NULL)
            {
                *pWorkItem = WorkItem(pUnrealizedChore, pSegment);
                return true;
            }
        }
        else
        {
            // We should never be in the last pass of search for work (which is when fForceStealLocalized is set to true) if we're looking for a token.
            ASSERT(!fForceStealLocalized);
            WorkQueue *pWorkQueue = pSegment->LocateUnrealizedChores();
            if (pWorkQueue != NULL)
            {
                *pWorkItem = WorkItem(pWorkQueue, pSegment);
                return true;
            }
        }

        return false;
    }

    /// <summary>
    ///     Performs a quick search of a particular segment.
    /// </summary>
    bool WorkSearchContext::QuickSearch(ScheduleGroupSegmentBase *pQCSegment,
                                        WorkItem *pWorkItem,
                                        bool fLastPass,
                                        ULONG allowableTypes)
    {
        if (((allowableTypes & WorkItem::WorkItemTypeContext) && GetRunnableContext(pWorkItem, pQCSegment)) ||
            ((allowableTypes & WorkItem::WorkItemTypeMaskAnyRealizedChore) &&
                GetRealizedChore(pWorkItem, pQCSegment, !!(allowableTypes & WorkItem::WorkItemTypeRealizedChore))) ||
            ((allowableTypes & WorkItem::WorkItemTypeMaskAnyUnrealizedChore) &&
                GetUnrealizedChore(pWorkItem, pQCSegment, fLastPass, !!(allowableTypes & WorkItem::WorkItemTypeUnrealizedChore))))
        {
            return true;
        }

        return false;
    }

    /// <summary>
    ///     Performs a quick yielding search of a particular segment.
    /// </summary>
    bool WorkSearchContext::QuickSearchYield(ScheduleGroupSegmentBase *pQCSegment,
                                             WorkItem *pWorkItem,
                                             bool fLastPass,
                                             ULONG allowableTypes)
    {
        if (((allowableTypes & WorkItem::WorkItemTypeMaskAnyUnrealizedChore) &&
                GetUnrealizedChore(pWorkItem, pQCSegment, fLastPass, !!(allowableTypes & WorkItem::WorkItemTypeUnrealizedChore))) ||
            ((allowableTypes & WorkItem::WorkItemTypeMaskAnyRealizedChore) &&
                GetRealizedChore(pWorkItem, pQCSegment, !!(allowableTypes & WorkItem::WorkItemTypeRealizedChore))) ||
            ((allowableTypes & WorkItem::WorkItemTypeContext) && GetRunnableContext(pWorkItem, pQCSegment)))
        {
            return true;
        }

        return false;
    }

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
    bool WorkSearchContext::SkipSegmentSearch(ScheduleGroupSegmentBase *pSegment, ScheduleGroupSegmentBase *pSkipSegment, SearchAffinity affinity, bool fLastPass)
    {
        if (pSegment == pSkipSegment)
        {
#if _UMSTRACE && _CHORETRACK
            VCMTRACE(MTRACE_EVT_SFW_SKIPSEGMENT, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pSegment);
#endif // _UMSTRACE && _CHORETRACK

            return true;
        }

        bool fSkip = false;
        const location& segmentAffinity = pSegment->GetAffinity();

        switch(affinity)
        {
            case SearchNonAffine:
                //
                // Skip if it has any affinity and we're looking for non-affine work.
                //
                fSkip = !segmentAffinity._Is_system();
                break;

            case SearchAffineLocal:
                //
                // Skip if it has specific affinity to something that doesn't intersect with us.
                //
                fSkip = segmentAffinity._Is_system() || !m_pVirtualProcessor->GetLocation()._FastVPIntersects(segmentAffinity);
                break;

            case SearchAffineNotMe:
                //
                // Skip if it has specific affinity to us **OR** the virtual processors to which it has affinity are in the middle
                // of search-for-work.  This is an optimization to prevent us from ripping affine work out from underneath someone who
                // will soon find it.
                //
                // In the last pass of SFW, we ignore this heuristic in order to avoid deadlock in required dependence cases.
                //
                fSkip = segmentAffinity._Is_system() ||
                        (m_pVirtualProcessor->GetLocation()._FastVPIntersects(segmentAffinity) ||
                            (m_pScheduler->HasSearchers(pSegment->GetAffinitySet()) && !fLastPass));
                break;

            default:
                break;

        }

#if _UMSTRACE && _CHORETRACK
        if (fSkip)
        {
            VCMTRACE(MTRACE_EVT_SFW_SKIPSEGMENT, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pSegment);
        }
#endif // _UMSTRACE && _CHORETRACK

        return fSkip;
    }

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
    bool WorkSearchContext::GetRunnableContextWithinGroup(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, SearchAffinity affinity, bool fLastPass)
    {
        ScheduleGroupBase *pGroup = pSegment->GetGroup();

        //
        // @TODO_LOC:
        //
        // We need to slice this differently so that we pick up "local" affine work before "non-local" affine work, etc...
        //
        if (!SkipSegmentSearch(pSegment, NULL, affinity, fLastPass) && GetRunnableContext(pWorkItem, pSegment))
        {
            return true;
        }

        ScheduleGroupSegmentBase *pCurSegment = pGroup->GetFirstScheduleGroupSegment(affinity != SearchNonAffine);
        while(pCurSegment != NULL)
        {
            if (!SkipSegmentSearch(pCurSegment, pSegment, affinity, fLastPass) && GetRunnableContext(pWorkItem, pCurSegment))
            {
                return true;
            }

            pCurSegment = pGroup->GetNextScheduleGroupSegment(pCurSegment);
        }

        return false;
    }

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
    bool WorkSearchContext::GetRealizedChoreWithinGroup(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fRealWork, SearchAffinity affinity, bool fLastPass)
    {
        ScheduleGroupBase *pGroup = pSegment->GetGroup();

        //
        // @TODO_LOC:
        //
        // We need to slice this differently so that we pick up "local" affine work before "non-local" affine work, etc...
        //
        if (!SkipSegmentSearch(pSegment, NULL, affinity, fLastPass) && GetRealizedChore(pWorkItem, pSegment, fRealWork))
        {
            return true;
        }

        ScheduleGroupSegmentBase *pCurSegment = pGroup->GetFirstScheduleGroupSegment(affinity != SearchNonAffine);
        while(pCurSegment != NULL)
        {
            if (!SkipSegmentSearch(pCurSegment, pSegment, affinity, fLastPass) && GetRealizedChore(pWorkItem, pCurSegment, fRealWork))
            {
                return true;
            }

            pCurSegment = pGroup->GetNextScheduleGroupSegment(pCurSegment);
        }

        return false;
    }

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
    bool WorkSearchContext::GetUnrealizedChoreWithinGroup(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pSegment, bool fRealWork, SearchAffinity affinity, bool fLastPass)
    {
        ScheduleGroupBase *pGroup = pSegment->GetGroup();

        //
        // @TODO_LOC:
        //
        // We need to slice this differently so that we pick up "local" affine work before "non-local" affine work, etc...  We also might need to
        // slice out pSegment first.  The second aspect requires more thought.
        //
        if (!SkipSegmentSearch(pSegment, NULL, affinity, fLastPass) && GetUnrealizedChore(pWorkItem, pSegment, fLastPass, fRealWork))
        {
            return true;
        }

        ScheduleGroupSegmentBase *pCurSegment = pGroup->GetFirstScheduleGroupSegment(affinity != SearchNonAffine);
        while(pCurSegment != NULL)
        {
            if (!SkipSegmentSearch(pCurSegment, pSegment, affinity, fLastPass) && GetUnrealizedChore(pWorkItem, pCurSegment, fLastPass, fRealWork))
            {
                return true;
            }

            pCurSegment = pGroup->GetNextScheduleGroupSegment(pCurSegment);
        }

        return false;
    }


    //***************************************************************************
    //
    // Fair Searches
    //
    // NOTE: At present, we completely ignore affine lists because fair schedulers ignore location hints.  This means you cannot "switch" scheduling
    //       to fair on a cache local scheduler should that ever come up.
    //

    /// <summary>
    ///     Performs a fair search for runnables in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchFair_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing)
    {
        int idx;
        ScheduleGroupSegmentBase *pSegment = pRing->GetPseudoRRNonAffineScheduleGroupSegment(&idx);

        int idxStart = idx;

        while (pSegment != NULL)
        {
            InternalContextBase *pContext = pSegment->GetRunnableContext();
            if (pContext != NULL)
            {
                pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                *pWorkItem = WorkItem(pContext);
                return true;
            }

            pSegment = pRing->GetNextNonAffineScheduleGroupSegment(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a fair search for realized chores in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchFair_Realized(WorkItem *pWorkItem, SchedulingRing *pRing, bool fRealItem)
    {
        int idx;
        ScheduleGroupSegmentBase *pSegment = pRing->GetPseudoRRNonAffineScheduleGroupSegment(&idx);

        int idxStart = idx;

        while (pSegment != NULL)
        {
            if (fRealItem)
            {
                RealizedChore *pRealizedChore = pSegment->GetRealizedChore();
                if (pRealizedChore != NULL)
                {
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                    *pWorkItem = WorkItem(pRealizedChore, pSegment);
                    return true;
                }
            }
            else
            {
                if (pSegment->HasRealizedChores())
                {
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                    *pWorkItem = WorkItem(pSegment);
                    return true;
                }
            }

            pSegment = pRing->GetNextNonAffineScheduleGroupSegment(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a fair search for unrealized chores in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchFair_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing, bool fRealItem)
    {
        int idx;
        ScheduleGroupSegmentBase *pSegment = pRing->GetPseudoRRNonAffineScheduleGroupSegment(&idx);

        int idxStart = idx;

        while (pSegment != NULL)
        {
            if (fRealItem)
            {
                _UnrealizedChore *pUnrealizedChore = pSegment->StealUnrealizedChore();
                if (pUnrealizedChore != NULL)
                {
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                    *pWorkItem = WorkItem(pUnrealizedChore, pSegment);
                    return true;
                }
            }
            else
            {
                WorkQueue *pWorkQueue = pSegment->LocateUnrealizedChores();
                if (pWorkQueue != NULL)
                {
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                    *pWorkItem = WorkItem(pWorkQueue, pSegment);
                    return true;
                }
            }

            pSegment = pRing->GetNextNonAffineScheduleGroupSegment(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a fair search for work.
    /// </summary>
    bool WorkSearchContext::SearchFair(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool, ULONG allowableTypes)
    {
        bool fFound = false;

        CONCRT_COREASSERT(pOriginSegment != NULL);
        //
        // Do any up-front searching required for special circumstances (e.g.: UMS schedulers)
        //
        if (PreSearch(pWorkItem))
            return true;

        //
        // The fair search essentially round robins among scheduling rings and groups within a ring.
        // If you consider the search space as follows:
        //
        //               SR      SR     SR     SR
        // Contexts      ---------------------->
        // Realized      ---------------------->
        // Unrealized    ---------------------->
        //
        // fair scheduling will make horizontal slices through the search space to find work.
        //
        // Each entry in the above matrix can be viewed as:
        //
        // SG -> SG -> SG -> SG
        //
        // However, after finding work in a particular ring, fair will move onto the next ring in round-robin fashion.
        //

        //
        // At the top of each search, reset to the next ring in the round robin index.  This is simply the starting point for this search.
        //
        SchedulingRing *pStartingRing = m_pScheduler->GetNextSchedulingRing();

        if (allowableTypes & WorkItem::WorkItemTypeContext)
        {
            SchedulingRing *pRing = pStartingRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Runnables(pWorkItem, pRing);
                if (fFound)
                {
                    m_pScheduler->SetNextSchedulingRing(pRing);
                    break;
                }

                pRing = m_pScheduler->GetNextSchedulingRing(pStartingRing, pRing);
            }

            if (!fFound)
                fFound = StealForeignLocalRunnable(pWorkItem, m_pVirtualProcessor->GetOwningNode());
        }

        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeMaskAnyRealizedChore))
        {
            SchedulingRing *pRing = pStartingRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Realized(pWorkItem, pRing, !!(allowableTypes & WorkItem::WorkItemTypeRealizedChore));
                if (fFound)
                {
                    m_pScheduler->SetNextSchedulingRing(pRing);
                    break;
                }

                pRing = m_pScheduler->GetNextSchedulingRing(pStartingRing, pRing);
            }
        }

        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeMaskAnyUnrealizedChore))
        {
            SchedulingRing *pRing = pStartingRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Unrealized(pWorkItem, pRing, !!(allowableTypes & WorkItem::WorkItemTypeUnrealizedChore));
                if (fFound)
                {
                    m_pScheduler->SetNextSchedulingRing(pRing);
                    break;
                }

                pRing = m_pScheduler->GetNextSchedulingRing(pStartingRing, pRing);
            }
        }

        return fFound;
    }

    /// <summary>
    ///     Performs a fair search for work in the yielding case.
    /// </summary>
    bool WorkSearchContext::SearchFairYield(WorkItem *pWorkItem, ScheduleGroupSegmentBase *, bool, ULONG allowableTypes)
    {
        //
        // The yielding case slices identically to the regular case excepting that the search is done in a pseudo-reverse order
        //
        bool fFound = false;

        //
        // Do any up-front searching required for special circumstances (e.g.: UMS schedulers)
        //
        if (PreSearch(pWorkItem))
            return true;

        //
        // At the top of each search, reset to the next ring in the round robin index.  This is simply the starting point for this search.
        //
        SchedulingRing *pStartingRing = m_pScheduler->GetNextSchedulingRing();

        if (allowableTypes & WorkItem::WorkItemTypeMaskAnyUnrealizedChore)
        {
            SchedulingRing *pRing = pStartingRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Unrealized(pWorkItem, pRing, !!(allowableTypes & WorkItem::WorkItemTypeUnrealizedChore));
                if (fFound)
                {
                    m_pScheduler->SetNextSchedulingRing(pRing);
                    break;
                }

                pRing = m_pScheduler->GetNextSchedulingRing(pStartingRing, pRing);
            }
        }


        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeMaskAnyRealizedChore))
        {
            SchedulingRing *pRing = pStartingRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Realized(pWorkItem, pRing, !!(allowableTypes & WorkItem::WorkItemTypeRealizedChore));
                if (fFound)
                {
                    m_pScheduler->SetNextSchedulingRing(pRing);
                    break;
                }

                pRing = m_pScheduler->GetNextSchedulingRing(pStartingRing, pRing);
            }
        }

        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeContext))
        {
            SchedulingRing *pRing = pStartingRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Runnables(pWorkItem, pRing);
                if (fFound)
                {
                    m_pScheduler->SetNextSchedulingRing(pRing);
                    break;
                }

                pRing = m_pScheduler->GetNextSchedulingRing(pStartingRing, pRing);
            }

            if (!fFound)
                fFound = StealForeignLocalRunnable(pWorkItem, m_pVirtualProcessor->GetOwningNode());
        }

        return fFound;

    }

    //***************************************************************************
    //
    // Cache Local Searches
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
    bool WorkSearchContext::SearchCacheLocal_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupSegmentBase *pBiasSegment,
                                                       bool fOtherLocalLRCCheck, SearchAffinity affinity, ULONG allowableTypes, bool fLastPass)
    {
#if _UMSTRACE && _CHORETRACK
        VCMTRACE(MTRACE_EVT_SFW_RUNNABLES, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pRing);
#endif // _UMSTRACE && _CHORETRACK

        if (pBiasSegment != NULL && GetRunnableContextWithinGroup(pWorkItem, pBiasSegment, affinity, fLastPass))
        {
            return true;
        }

        //
        // As much as I abhor the one off placement of this here, it's the "cleanest" place to put this for its given location within SFW.
        // Attempt to steal a local runnable context from the current node.
        //
        if (fOtherLocalLRCCheck && StealLocalRunnable(pWorkItem, m_pVirtualProcessor->GetOwningNode(), m_pVirtualProcessor))
        {
            return true;
        }

        int idx;
        ScheduleGroupSegmentBase *pSegment =
            (affinity == SearchNonAffine) ?  pRing->GetPseudoRRNonAffineScheduleGroupSegment(&idx) : pRing->GetPseudoRRAffineScheduleGroupSegment(&idx);

        int idxStart = idx;

        while (pSegment != NULL)
        {
            ScheduleGroupSegmentBase *pQCSegment = m_pScheduler->AcquireQuickCacheSlot(m_maskId);
            if (pQCSegment)
            {
                if (QuickSearch(pQCSegment, pWorkItem, fLastPass, allowableTypes))
                    return true;
            }

            //
            // Is this a segment we should skip:
            //
            //     - If we are explicitly told to skip it in the search because it was checked elsewhere (pSkipSegment).
            //     - If we are searching for affine work and it matches or doesn't match the search context affinity as dictated by the affinity parameter.
            //
            if (!SkipSegmentSearch(pSegment, pBiasSegment, affinity, fLastPass) && GetRunnableContext(pWorkItem, pSegment))
            {
                if (affinity == SearchNonAffine)
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                else
                    pRing->SetPseudoRRAffineScheduleGroupSegmentNext(idx);

                return true;
            }

            pSegment = (affinity == SearchNonAffine) ? pRing->GetNextNonAffineScheduleGroupSegment(&idx, idxStart) :
                                                     pRing->GetNextAffineScheduleGroupSegment(&idx, idxStart);
        }

        return false;

    }

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
    bool WorkSearchContext::SearchCacheLocal_Realized(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupSegmentBase *pBiasSegment,
                                                      bool fRealWork, SearchAffinity affinity, ULONG allowableTypes, bool fLastPass)
    {
        bool fFound = false;

#if _UMSTRACE && _CHORETRACK
        VCMTRACE(MTRACE_EVT_SFW_REALIZED, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pRing);
        VCMTRACE(MTRACE_EVT_SFW_SEGMENT, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pBiasSegment);
#endif // _UMSTRACE && _CHORETRACK

        if (pBiasSegment != NULL && GetRealizedChoreWithinGroup(pWorkItem, pBiasSegment, fRealWork, affinity, fLastPass))
        {
            return true;
        }

        int idx;
        ScheduleGroupSegmentBase *pSegment =
            (affinity == SearchNonAffine) ? pRing->GetPseudoRRNonAffineScheduleGroupSegment(&idx) : pRing->GetPseudoRRAffineScheduleGroupSegment(&idx);

        int idxStart = idx;

        while (pSegment != NULL && !fFound)
        {
            ScheduleGroupSegmentBase *pQCSegment = m_pScheduler->AcquireQuickCacheSlot(m_maskId);
            if (pQCSegment)
            {
                if (QuickSearch(pQCSegment, pWorkItem, fLastPass, allowableTypes))
                    return true;
            }

#if _UMSTRACE && _CHORETRACK
            VCMTRACE(MTRACE_EVT_SFW_SEGMENT, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pSegment);
#endif // _UMSTRACE && _CHORETRACK

            //
            // Is this a segment we should skip:
            //
            //     - If we are explicitly told to skip it in the search because it was checked elsewhere (pSkipSegment).
            //     - If we are searching for affine work and it matches or doesn't match the search context affinity as dictated by the affinity parameter.
            //
            if (!SkipSegmentSearch(pSegment, pBiasSegment, affinity, fLastPass) && GetRealizedChore(pWorkItem, pSegment, fRealWork))
            {
                if (affinity == SearchNonAffine)
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                else
                    pRing->SetPseudoRRAffineScheduleGroupSegmentNext(idx);

                return true;
            }

            pSegment = (affinity == SearchNonAffine) ? pRing->GetNextNonAffineScheduleGroupSegment(&idx, idxStart) :
                                                       pRing->GetNextAffineScheduleGroupSegment(&idx, idxStart);
        }

        return false;
    }

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
    bool WorkSearchContext::SearchCacheLocal_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupSegmentBase *pBiasSegment,
                                                        bool fRealWork, SearchAffinity affinity, ULONG allowableTypes, bool fLastPass)
    {
        bool fFound = false;

#if _UMSTRACE && _CHORETRACK
        VCMTRACE(MTRACE_EVT_SFW_UNREALIZED, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pRing);
#endif // _UMSTRACE && _CHORETRACK

        if (pBiasSegment != NULL && GetUnrealizedChoreWithinGroup(pWorkItem, pBiasSegment, fRealWork, affinity, fLastPass))
        {
            return true;
        }

        int idx;
        ScheduleGroupSegmentBase *pSegment =
            (affinity == SearchNonAffine) ? pRing->GetPseudoRRNonAffineScheduleGroupSegment(&idx) : pRing->GetPseudoRRAffineScheduleGroupSegment(&idx);

        int idxStart = idx;

        while (pSegment != NULL && !fFound)
        {
            ScheduleGroupSegmentBase *pQCSegment = m_pScheduler->AcquireQuickCacheSlot(m_maskId);
            if (pQCSegment)
            {
                if (QuickSearch(pQCSegment, pWorkItem, fLastPass, allowableTypes))
                    return true;
            }

            //
            // Is this a segment we should skip:
            //
            //     - If we are explicitly told to skip it in the search because it was checked elsewhere (pSkipSegment).
            //     - If we are searching for affine work and it matches or doesn't match the search context affinity as dictated by the affinity parameter.
            //
            if (!SkipSegmentSearch(pSegment, pBiasSegment, affinity, fLastPass) && GetUnrealizedChore(pWorkItem, pSegment, fLastPass, fRealWork))
            {
                if (affinity == SearchNonAffine)
                    pRing->SetPseudoRRNonAffineScheduleGroupSegmentNext(idx);
                else
                    pRing->SetPseudoRRAffineScheduleGroupSegmentNext(idx);

                return true;
            }

            pSegment = (affinity == SearchNonAffine) ? pRing->GetNextNonAffineScheduleGroupSegment(&idx, idxStart) :
                                                       pRing->GetNextAffineScheduleGroupSegment(&idx, idxStart);
        }

        return false;
    }

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
    bool WorkSearchContext::SearchCacheLocal(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes)
    {
        bool fFound = false;

        if (PreSearch(pWorkItem))
            return true;

        ASSERT(pOriginSegment);

        m_serviceTick = platform::__GetTickCount64();
        m_pScheduler->PeriodicScan(m_serviceTick);

        //
        // @TODO: This is a temporary patch until we have priority and boosts in the scheduler.  Right now, search for any segment in the locked
        // FIFO of "priority" segments to see if we need to service someone to avoid livelock.  We also periodically scan to determine whether
        // groups need to go into this list.
        //

        if (CheckPriorityList(m_serviceTick))
        {
            if (m_pScheduler->HasPriorityObjects())
            {
                BoostedObject *pObject = m_pScheduler->GetNextPriorityObject();
                while (pObject != NULL)
                {
                    if (pObject->IsScheduleGroupSegment())
                    {
                        ScheduleGroupSegmentBase *pSegment = ScheduleGroupSegmentBase::FromBoostEntry(pObject);
                        OMTRACE(MTRACE_EVT_PRIORITYPULL, pSegment, NULL, m_pVirtualProcessor, 0);
                        if (QuickSearch(pSegment, pWorkItem, fLastPass, allowableTypes))
                        {
                            fFound = true;
                            break;
                        }
                    }
                    else if (allowableTypes & WorkItem::WorkItemTypeContext)
                    {
                        VirtualProcessor *pVProc = VirtualProcessor::FromBoostEntry(pObject);
                        InternalContextBase *pContext = pVProc->StealLocalRunnableContext();
                        if (pContext != NULL)
                        {
                            *pWorkItem = WorkItem(pContext);
                            fFound = true;
                            break;
                        }
                    }

                    pObject = m_pScheduler->GetNextPriorityObject();
                }
            }

            //
            // If we found something in a priority segment, we need to mark the V-Proc so that it once again looks for affinitized work
            // the next time through SFW.
            //
            m_pVirtualProcessor->MarkGrabbedPriority();
        }

        SchedulingRing *pOwningRing = m_pVirtualProcessor->GetOwningRing();

        if (!fFound)
        {
            //
            // Before describing the cache local search algorithm, some definitions to aid in this are in order:
            //
            // Work:
            //     local work      -- Work within a node/ring that a given virtual processor belongs to is considered local work
            //     foreign work    -- Work within a node/ring that a given virtual processor does *NOT* belong to is considered foreign work
            //     affine work     -- Work which is placed at a location more specific than the system location is affine work
            //     non-affine work -- Work which is placed at the system location (or no location) is considered non-affine work
            //
            // Virtual Processors:
            //     rambling   -- a given virtual processor is *rambling* when it is executing foreign work (regardless whether such work is affine to it or not)
            //     non-affine -- a given virtual processor is *non-affine* when it is executing non-affine work (regardless whether it is rambling or not)
            //
            // Virtual processors which are rambling or are executing non-affine work are tracked by the scheduler.  When such virtual processors
            // exist, notifications will be published whenever affine work or local work becomes available.
            //

            //
            // Look for work in the scheduler.  **IN GENERAL**, we want to find work in the following order of precedence:
            //
            // Phase 1: Affine work
            //       Affine work in our current group (preferring local affine work)
            //       Local affine (to us) work (in our starting (home) ring)
            //       Foreign affine (to us) work (in foreign ring segments that match our affinity mask -- such have a segment in our ring by definition)
            //
            //       - This phase may be skipped if we are executing non-affine work and have not received an affinity notification from the scheduler.
            //         Doing so prevents SFW from making a full pass per work item if no affinity has been used.
            //
            // Phase 2: Non-affine work
            //       Non-Affine work in our current group (preferring local work)
            //       Local non-affine work (in our starting (home ring))
            //       Foreign non-affine work (in foreign rings).
            //
            //       - This phase may *NEVER* be skipped.
            //
            // Phase 3: Affine work (non-local)
            //       Non-local affine (*NOT* to us) work (in foreign ring segments that do not match our affinity mask
            //
            //       - This phase may be skipped in theory if we know that there is no affine work in the scheduler.  For a case with no affinity used within
            //         any scheduling API, this phase is only hit if the scheduler is completely idle.  At idle, performing a pass is likely little additional
            //         cost to any performance sensitive scenario.  At present, this stage is not skipped for this reason.
            //
            // At a very high level, you can consider a cache local search to be making vertical slices through the search space as follows:
            //
            //               SR      SR     SR     SR
            // Contexts      |       |      |      |
            // Realized      |       |      |      |
            // Unrealized    v       v      v      v
            //
            // where each entry in this matrix is a piece of a schedule group.
            //
            // Note that in the last pass of SFW, we cannot skip phases or segments.  Doing so can lead us to deadlock in cases with required dependence
            // (assumptions on concurrency level).
            //
            static const SearchAffinity phaseAffinities[] = { SearchAffineLocal, SearchNonAffine, SearchAffineNotMe };
            //                                              {       1          ,       0        ,         2         };
            int startPhaseIndex = (!m_pVirtualProcessor->ExecutingAffine() && !m_pVirtualProcessor->CheckAffinityNotification() && !fLastPass) ? 1 : 0;
            int maxPhaseIndex = SIZEOF_ARRAY(phaseAffinities) - 1;

    #if _UMSTRACE && _CHORETRACK
            VCMTRACE(MTRACE_EVT_SFW_BIASGROUP, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, pOriginSegment);
    #endif // _UMSTRACE && _CHORETRACK

            // Make sure we only check the vproc's LRC and local node LRCs once.  This is done in this manner to ensure the odd placement of this within the
            // search algorithm. This variable is set to false at the end of the first pass through the inner loop.
            bool fLocalCheck = true;
            for(int phaseIndex = startPhaseIndex; !fFound && phaseIndex <= maxPhaseIndex; ++phaseIndex) // This loop goes through the phases in the phaseAffinity array.
            {
    #if _UMSTRACE && _CHORETRACK
                VCMTRACE(MTRACE_EVT_SFW_SEARCHPHASE, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, phase);
    #endif // _UMSTRACE && _CHORETRACK

                SearchAffinity srchType = phaseAffinities[phaseIndex];

                // At the outset of every pass, we bias to the originating segment/group and the current virtual processors owning ring.
                // Note that if we are searching for non-local affinework, we skip the owning ring (by definition, it's local).
                ScheduleGroupSegmentBase *pBiasSegment = pOriginSegment;
                SchedulingRing *pRing = pOwningRing;

                while (pRing != NULL)
                {
                    ScheduleGroupSegmentBase* pQCSegment = m_pScheduler->AcquireQuickCacheSlot(m_maskId);
                    if (pQCSegment)
                    {
                        if (QuickSearch(pQCSegment, pWorkItem, fLastPass, allowableTypes))
                        {
                            fFound = true;
                            break;
                        }
                    }

                    if ((fLocalCheck && (allowableTypes & WorkItem::WorkItemTypeContext) &&
                            GetLocalRunnable(pWorkItem, m_pVirtualProcessor, false)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeContext) &&
                            SearchCacheLocal_Runnables(pWorkItem,
                                                       pRing,
                                                       pBiasSegment,
                                                       fLocalCheck,
                                                       srchType,
                                                       allowableTypes,
                                                       fLastPass)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeMaskAnyRealizedChore) &&
                            SearchCacheLocal_Realized(pWorkItem,
                                                      pRing,
                                                      pBiasSegment,
                                                      !!(allowableTypes & WorkItem::WorkItemTypeRealizedChore),
                                                      srchType,
                                                      allowableTypes,
                                                      fLastPass)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeMaskAnyUnrealizedChore) &&
                            SearchCacheLocal_Unrealized(pWorkItem,
                                                        pRing,
                                                        pBiasSegment,
                                                        !!(allowableTypes & WorkItem::WorkItemTypeUnrealizedChore),
                                                        srchType,
                                                        allowableTypes,
                                                        fLastPass)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeContext) && phaseIndex == maxPhaseIndex &&
                            StealLocalRunnable(pWorkItem, pRing->GetOwningNode(), m_pVirtualProcessor)))
                    {
                        fFound = true;
                        break;
                    }

                    // Make sure we only steal from vproc LRCs in the local node once.  This is done in this manner to ensure the odd placement of this within the
                    // search algorithm.
                    fLocalCheck = false;

                    // The first time through the loop, not only did we bias to pBiasSegment, but to pStartingRing as well. Remove the bias and move to the next ring.
                    pBiasSegment = NULL;
                    pRing = m_pScheduler->GetNextSchedulingRing(pOwningRing, pRing);

                } // end of while (pRing != NULL) - this loop goes over all rings in the scheduler
            } // end of for (!fFound && phase <= maxPhase) - this loop goes through all search affinity types
        } // end of if (!fFound)

        if (fFound)
        {
            ScheduleGroupSegmentBase *pSegment = pWorkItem->GetScheduleGroupSegment();
            SchedulingRing *pRing = pSegment->GetSchedulingRing();

            pSegment->ServiceMark(m_serviceTick);

            bool fAffine = false;
            if (!pSegment->GetAffinity()._Is_system())
            {
                location vpLoc = m_pVirtualProcessor->GetLocation();
                if (vpLoc._FastVPIntersects(pSegment->GetAffinity()))
                    fAffine = true;
            }
            bool fLocal = pRing == pOwningRing;

            m_pVirtualProcessor->UpdateWorkState(fAffine, fLocal);
        }

#if _UMSTRACE && _CHORETRACK
        VCMTRACE(MTRACE_EVT_SFW_END, m_pVirtualProcessor->GetExecutingContext(), m_pVirtualProcessor, (ULONG_PTR)(fFound ? pWorkItem->GetScheduleGroupSegment() : NULL));
#endif // _UMSTRACE && _CHORETRACK

        return fFound;
    }

    /// <summary>
    ///     Searches for work within the scheduler according to the cache local (schedule group local) search algorithm for yielding.
    /// </summary>
    /// <param name="pWorkItem">
    ///     If a work item is found, the work item will be returned here.
    /// </param>
    /// <param name="pOriginSegment">
    ///     The segment to bias the search to. The need to prefer localized and prioritized work will often trump this bias.
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
    bool WorkSearchContext::SearchCacheLocalYield(WorkItem *pWorkItem, ScheduleGroupSegmentBase *pOriginSegment, bool fLastPass, ULONG allowableTypes)
    {
        bool fFound = false;

        if (PreSearch(pWorkItem))
            return true;

        ASSERT(pOriginSegment);

        m_serviceTick = platform::__GetTickCount64();
        m_pScheduler->PeriodicScan(m_serviceTick);

        //
        // @TODO: This is a temporary patch until we have priority and boosts in the scheduler.  Right now, search for any segment in the locked
        // FIFO of "priority" segments to see if we need to service someone to avoid livelock.  We also periodically scan to determine whether
        // groups need to go into this list.
        //

        if (CheckPriorityList(m_serviceTick))
        {
            if (m_pScheduler->HasPriorityObjects())
            {
                BoostedObject *pObject = m_pScheduler->GetNextPriorityObject();
                while (pObject != NULL)
                {
                    if (pObject->IsScheduleGroupSegment())
                    {
                        ScheduleGroupSegmentBase *pSegment = ScheduleGroupSegmentBase::FromBoostEntry(pObject);
                        OMTRACE(MTRACE_EVT_PRIORITYPULL, pSegment, NULL, m_pVirtualProcessor, 1);
                        if (QuickSearchYield(pSegment, pWorkItem, fLastPass, allowableTypes))
                        {
                            fFound = true;
                            break;
                        }
                    }
                    else if (allowableTypes & WorkItem::WorkItemTypeContext)
                    {
                        VirtualProcessor *pVProc = VirtualProcessor::FromBoostEntry(pObject);
                        InternalContextBase *pContext = pVProc->StealLocalRunnableContext();
                        if (pContext != NULL)
                        {
                            *pWorkItem = WorkItem(pContext);
                            fFound = true;
                            break;
                        }
                    }

                    pObject = m_pScheduler->GetNextPriorityObject();
                }
            }

            //
            // If we found something in a priority segment, we need to mark the V-Proc so that it once again looks for affinitized work
            // the next time through SFW.
            //
            m_pVirtualProcessor->MarkGrabbedPriority();
        }

        SchedulingRing *pOwningRing = m_pVirtualProcessor->GetOwningRing();

        if (!fFound)
        {
            //
            // Before describing the cache local search algorithm, some definitions to aid in this are in order:
            //
            // Work:
            //     local work      -- Work within a node/ring that a given virtual processor belongs to is considered local work
            //     foreign work    -- Work within a node/ring that a given virtual processor does *NOT* belong to is considered foreign work
            //     affine work     -- Work which is placed at a location more specific than the system location is affine work
            //     non-affine work -- Work which is placed at the system location (or no location) is considered non-affine work
            //
            // Virtual Processors:
            //     rambling   -- a given virtual processor is *rambling* when it is executing foreign work (regardless whether such work is affine to it or not)
            //     non-affine -- a given virtual processor is *non-affine* when it is executing non-affine work (regardless whether it is rambling or not)
            //
            // Virtual processors which are rambling or are executing non-affine work are tracked by the scheduler.  When such virtual processors
            // exist, notifications will be published whenever affine work or local work
            //

            //
            // **IN GENERAL**, we want to find work in the following order of precedence:
            //
            // Phase 1: Affine work
            //       Affine work in our current group (preferring local affine work)
            //       Local affine (to us) work (in our starting (home) ring)
            //       Foreign affine (to us) work (in foreign ring segments that match our affinity mask -- such have a segment in our ring by definition)
            //
            //       - This phase may be skipped if we are executing non-affine work and have not received an affinity notification from the scheduler.
            //         Doing so prevents SFW from making a full pass per work item if no affinity has been used.
            //
            // Phase 2: Non-affine work
            //       Non-Affine work in our current group (preferring local work)
            //       Local non-affine work (in our starting (home ring))
            //       Foreign non-affine work (in foreign rings).
            //
            //       - This phase may *NEVER* be skipped.
            //
            // Phase 3: Affine work (non-local)
            //       Non-local affine (*NOT* to us) work (in foreign ring segments that do not match our affinity mask
            //
            //       - This phase may be skipped in theory if we know that there is no affine work in the scheduler.  For a case with no affinity used within
            //         any scheduling API, this phase is only hit if the scheduler is completely idle.  At idle, performing a pass is likely little additional
            //         cost to any performance sensitive scenario.  At present, this stage is not skipped for this reason.
            //
            // At a very high level, you can consider a cache local search to be making vertical slices through the search space as follows:
            //
            //               SR      SR     SR     SR
            // Contexts      |       |      |      |
            // Realized      |       |      |      |
            // Unrealized    v       v      v      v
            //
            // where each entry in this matrix is a piece of a schedule group.
            //
            // Note that in the last pass of SFW, we cannot skip phases or segments.  Doing so can lead us to deadlock in cases with required dependence
            // (assumptions on concurrency level).
            //
            static const SearchAffinity phaseAffinities[] = { SearchAffineLocal, SearchNonAffine, SearchAffineNotMe };
            //                                              {       1          ,       0        ,         2         };
            int startPhaseIndex = (!m_pVirtualProcessor->ExecutingAffine() && !m_pVirtualProcessor->CheckAffinityNotification() && !fLastPass) ? 1 : 0;
            int maxPhaseIndex = SIZEOF_ARRAY(phaseAffinities) - 1;

            // @TODO_LOC: We need livelock prevention by doing a priority boost on segments that haven't been serviced in "too long".

            bool fLocalCheck = true;
            for (int phaseIndex = startPhaseIndex; !fFound && phaseIndex <= maxPhaseIndex; ++phaseIndex) // This loop goes through the phases in the phaseAffinity array.
            {
                SearchAffinity srchType = phaseAffinities[phaseIndex];

                // At the outset of every pass, we bias to the originating segment/group and the current virtual processor's owning ring.
                // Note that if we are searching for non-local affine work, we skip the owning ring (by definition, it's local).
                ScheduleGroupSegmentBase *pBiasSegment = pOriginSegment;
                SchedulingRing *pRing = pOwningRing;

                while (pRing != NULL)
                {
                    if (((allowableTypes & WorkItem::WorkItemTypeMaskAnyUnrealizedChore) &&
                            SearchCacheLocal_Unrealized(pWorkItem,
                                                        pRing,
                                                        pBiasSegment,
                                                        !!(allowableTypes & WorkItem::WorkItemTypeUnrealizedChore),
                                                        srchType,
                                                        allowableTypes,
                                                        fLastPass)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeMaskAnyRealizedChore) &&
                            SearchCacheLocal_Realized(pWorkItem,
                                                      pRing,
                                                      pBiasSegment,
                                                      !!(allowableTypes & WorkItem::WorkItemTypeRealizedChore),
                                                      srchType,
                                                      allowableTypes,
                                                      fLastPass)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeContext) &&
                            SearchCacheLocal_Runnables(pWorkItem,
                                                       pRing,
                                                       pBiasSegment,
                                                       fLocalCheck,
                                                       srchType,
                                                       allowableTypes,
                                                       fLastPass)) ||

                        ((allowableTypes & WorkItem::WorkItemTypeContext) && phaseIndex == maxPhaseIndex &&
                            StealLocalRunnable(pWorkItem, pRing->GetOwningNode(), m_pVirtualProcessor)) ||

                        (fLocalCheck && (allowableTypes & WorkItem::WorkItemTypeContext) &&
                            GetLocalRunnable(pWorkItem, m_pVirtualProcessor, true)))

                    {
                        fFound = true;
                        break;
                    }

                    // Make sure we only steal from vproc LRCs in the local node once.  This is done in this manner to ensure the odd placement of this within the
                    // search algorithm.
                    fLocalCheck = false;

                    // The first time through the loop, not only did we bias to pBiasSegment, but to pStartingRing as well. Remove the bias and move to the next ring.
                    pBiasSegment = NULL;
                    pRing = m_pScheduler->GetNextSchedulingRing(pOwningRing, pRing);

                } // end of while (pRing != NULL) - this loop goes over all rings in the scheduler
            } // end of for (!fFound && phase <= maxPhase) - this loop goes through all search affinity types
        } // end of if (!fFound)

        if (fFound)
        {
            ScheduleGroupSegmentBase *pSegment = pWorkItem->GetScheduleGroupSegment();
            SchedulingRing *pRing = pSegment->GetSchedulingRing();

            pSegment->ServiceMark(m_serviceTick);

            bool fAffine = pWorkItem->GetScheduleGroupSegment()->GetAffinity()._Is_system();
            bool fLocal = pRing == pOwningRing;

            m_pVirtualProcessor->UpdateWorkState(fAffine, fLocal);
        }

        return fFound;
    }
}
}
