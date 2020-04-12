// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingNode.cpp
//
// Source file containing the SchedulingNode implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    SchedulingNode::SchedulingNode(const QuickBitSet& resourceSet, DWORD numaNodeNumber, SchedulingRing *pRing)
        : m_pRing(pRing)
        , m_resourceSet(resourceSet)
        , m_virtualProcessorAvailableCount(0)
        , m_virtualProcessorsPendingThreadCreate(0)
        , m_virtualProcessorCount(0) // needed for scheduling rings
        , m_ramblingCount(0)
        , m_numaNodeNumber(numaNodeNumber)
        , m_virtualProcessors(pRing->m_pScheduler, 256, ListArray<VirtualProcessor>::DeletionThresholdInfinite)
    {
        m_pScheduler = m_pRing->m_pScheduler;
        m_id = m_pRing->Id();
    }

    SchedulingNode::~SchedulingNode()
    {
        Cleanup();
    }

    void SchedulingNode::Cleanup()
    {
        //
        // Do not clean up m_pRing here, it is done at SchedulerBase::m_rings
        //

        // Cleanup of the virtual processors does not need to explicitly happen.  When
        // the destructor of the list array is called, it will internally delete
        // all of its elements
    }

    /// <summary>
    ///     Creates and adds a new virtual processor in the node to associated with the root provided.
    ///     NOTE: For non-oversubscribed vprocs this API is currently will only work for initial allocation.
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
    VirtualProcessor* SchedulingNode::AddVirtualProcessor(IVirtualProcessorRoot *pOwningRoot, bool fOversubscribed)
    {
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();

        // Try and grab a virtual processor from the free pool before creating a new one
        VirtualProcessor *pVirtualProcessor = m_virtualProcessors.PullFromFreePool();
        if (pVirtualProcessor == NULL)
        {
            pVirtualProcessor = m_pScheduler->CreateVirtualProcessor(this, pOwningRoot);
        }
        else
        {
            pVirtualProcessor->Initialize(this, pOwningRoot);
        }

        if (fOversubscribed)
        {
            ASSERT(pCurrentContext != NULL && !pCurrentContext->IsExternal());
            InternalContextBase * pOversubsbscribingContext = static_cast<InternalContextBase *>(pCurrentContext);

            pVirtualProcessor->m_fOversubscribed = true;
            pVirtualProcessor->m_pOversubscribingContext = pOversubsbscribingContext;

            // The oversubscribed vproc is fenced by adding it to the list array below.
            pOversubsbscribingContext ->SetOversubscribedVProc(pVirtualProcessor);
        }

        // We increment the total count of virtual processors on the node since the rambling logic uses this count.
        InterlockedIncrement(&m_virtualProcessorCount);
        m_pScheduler->IncrementActiveResourcesByMask(pVirtualProcessor->GetMaskId());

        // If no virtual processors are 'available' in the scheduler, try to start this one up right away, if not, make it available,
        // and increment the counts to indicate this. The only exception is the first virtual processor added as part of the initial
        // set of virtual processors.
        //
        // @TODO: Q: Is the lack of relative atomicity between the two counts (avail / pending thread) a problem here for any real
        // scenario?
        if ((m_pScheduler->m_virtualProcessorAvailableCount == m_pScheduler->m_virtualProcessorsPendingThreadCreate) && (m_pScheduler->m_virtualProcessorCount > 0))
        {
            //
            // The check above is not accurate, since the count may increase right after the check -> in the worst case, the virtual
            // processor is activated when it should've been left available.
            //
            // We should only be activating virtual processors as they are added, if they are either oversubscribed or as a result
            // of core migration. The initial set of virtual processors should never be activated here.
            //
            ASSERT(pCurrentContext == NULL || fOversubscribed);

            //
            // The vproc should be added to the list array only after it is fully initialized. If this is an oversubscribed vproc,
            // we need to synchronize with a concurrent RemoveCore, which assumes it can party on the vproc if it is found in the list
            // array.
            //
            m_virtualProcessors.Add(pVirtualProcessor);

            //
            // Activation of a virtual processor synchronizes with finalization. If the scheduler is in the middle of finalization
            // or has already shutdown, the API will return false.
            //
            bool activated = m_pScheduler->VirtualProcessorActive(true);

            if (activated)
            {
                ScheduleGroupSegmentBase * pSegment = (pCurrentContext != NULL) ?
                                                            pCurrentContext->GetScheduleGroupSegment() :
                                                            m_pRing->GetAnonymousScheduleGroupSegment();
                pVirtualProcessor->StartupWorkerContext(pSegment);
            }
            else
            {
                //
                // We do nothing here since the scheduler is shutting down/has shutdown. The virtual processor remains unavailable,
                // and since we didn't increment availabe counts, we don't have to decrement them.
                //
            }
        }
        else
        {
            //
            // The vproc should be added to the list array only after it is fully initialized. If this is an oversubscribed vproc,
            // we need to synchronize with a concurrent RemoveVirtualProcessor, which assumes it can party on the vproc if it is
            // found in the list array.
            //
            m_virtualProcessors.Add(pVirtualProcessor);
            pVirtualProcessor->MakeAvailable(VirtualProcessor::AvailabilityInactive, false);

            OMTRACE(MTRACE_EVT_MADEAVAILABLE, m_pScheduler, SchedulerBase::FastCurrentContext(), pVirtualProcessor, NULL);
            OMTRACE(MTRACE_EVT_AVAILABLEVPROCS, m_pScheduler, SchedulerBase::FastCurrentContext(), this, m_pScheduler->m_virtualProcessorAvailableCount);
        }

        return pVirtualProcessor;
    }

    /// <summary>
    ///     Find the virtual processor in this node that matches the root provided.
    /// </summary>
    /// <param name="pRoot">
    ///     The virtual processor root to match.
    /// </param>
    /// <remarks>
    ///     IMPORTANT: This API is only called while removing virtual processors via IScheduler::RemoveVirtualProcessors.
    ///     If this functionality is needed at other call sites in the future, the implementation may need to be
    ///     reevaluated (see comments below).
    /// </remarks>
    VirtualProcessor* SchedulingNode::FindMatchingVirtualProcessor(IVirtualProcessorRoot* pRoot)
    {
        int arraySize = m_virtualProcessors.MaxIndex();

        for (int i = 0; i < arraySize; i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];

            // It is ok to test the owning root here without a lock. If the owning root matches what we're looking for,
            // we are guarannteed it will not change (by way of the virtual processor being retired and reused). This is because
            // the call to IVirtualProcessorRoot::Remove in the virtual processor retirement code path is serialized in the RM
            // before or after the call to IScheduler::RemoveVirtualProcessors. i.e. if we find an owning root that matches, the retirement
            // path is unable to set it to NULL until after we're done.
            if ((pVirtualProcessor != NULL) && (pVirtualProcessor->m_pOwningRoot == pRoot))
            {
                return pVirtualProcessor;
            }
        }

        return NULL;
    }

    InternalContextBase *SchedulingNode::StealLocalRunnableContext(VirtualProcessor* pSkipVirtualProcessor)
    {
        InternalContextBase *pContext = NULL;
        int skipIndex, startIndex;
        int arraySize = m_virtualProcessors.MaxIndex();

        if (pSkipVirtualProcessor != NULL)
        {
            skipIndex = pSkipVirtualProcessor->m_listArrayIndex;
            startIndex = 1;
        }
        else
        {
            skipIndex = 0;
            startIndex = 0;
        }

        for (int i = startIndex; i < arraySize; i++)
        {
            int index = (i + skipIndex) % arraySize;
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[index];
            if (pVirtualProcessor == NULL)
            {
                continue;
            }
#if _UMSTRACE
            ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
            CMTRACE(MTRACE_EVT_SEARCHEDLOCALRUNNABLES, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, pVirtualProcessor, pSkipVirtualProcessor);
#endif // _UMSTRACE

            pContext = pVirtualProcessor->m_localRunnableContexts.Steal();
            if (pContext != NULL)
            {
#if defined(_DEBUG)
                pContext->SetDebugBits(CTX_DEBUGBIT_STOLENFROMLOCALRUNNABLECONTEXTS);
#endif // _DEBUG

                break;
            }
        }
        return pContext;
    }

    /// <summary>
    ///     Find an available virtual processor in the scheduling node. We claim ownership of the virtual
    ///     processor and return it.
    /// </summary>
    bool SchedulingNode::FoundAvailableVirtualProcessor(VirtualProcessor::ClaimTicket& ticket,
                                                        location bias,
                                                        ULONG type)
    {
        if (bias._GetType() == location::_ExecutionResource)
        {
            VirtualProcessor *pBiasProc = FindVirtualProcessorByLocation(&bias);
            ASSERT(!pBiasProc || pBiasProc->GetOwningNode() == this);
            if (pBiasProc && pBiasProc->ClaimExclusiveOwnership(ticket, type))
                return true;
        }

        // The callers of this API MUST check that that the available virtual processor count in the scheduling node
        // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
        // most of the time, and it saves the function call overhead on fast paths (chore push)

        for (int i = 0; i < m_virtualProcessors.MaxIndex(); i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];

             if (pVirtualProcessor != NULL && pVirtualProcessor->ClaimExclusiveOwnership(ticket, type))
                return true;
        }

        return false;
    }

    /// <summary>
    ///     Gets a location object which represents the scheduling node.
    /// </summary>
    location SchedulingNode::GetLocation()
    {
        return location(location::_SchedulingNode, m_id, m_pScheduler->Id(), this);
    }

    /// <summary>
    ///     Returns a virtual processor from the given location.  The virtual processor must be within this node.
    /// </summary>
    VirtualProcessor* SchedulingNode::FindVirtualProcessorByLocation(const location* pLoc)
    {
        if (pLoc->_GetType() != location::_ExecutionResource)
            return NULL;

        if (m_pScheduler->IsLocationBound(pLoc))
            return pLoc->_As<VirtualProcessor>();

        //
        // The specified location has not been specifically bound yet.  Find any virtual processor which we deem appropriate for the binding
        // to the specified execution resource id.
        //
        for (int i = 0; i < m_virtualProcessors.MaxIndex(); i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];

            if (pVirtualProcessor != NULL && pVirtualProcessor->GetExecutionResourceId() == pLoc->_GetId())
                return pVirtualProcessor;
        }

        return NULL;
    }

} // namespace details
} // namespace Concurrency
