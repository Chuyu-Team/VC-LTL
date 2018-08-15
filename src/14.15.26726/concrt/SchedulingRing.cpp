// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingRing.cpp
//
// Source file containing the SchedulingRing implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Construct a new scheduling ring.
    /// </summary>
    SchedulingRing::SchedulingRing(SchedulerBase *pScheduler, int id)
        : m_pScheduler(pScheduler)
        , m_pNode(NULL) // Will be set later explicitly by the creating scheduler
        , m_pAnonymousSegment(NULL)
        , m_affineSegments(pScheduler, 256, 64)
        , m_nonAffineSegments(pScheduler, 256, 64)
        , m_nextAffineSegment(0)
        , m_nextNonAffineSegment(0)
        , m_id(id)
        , m_active(0)
    {
        //
        // Create the anonymous schedule group early.  UMS schedulers need somewhere to begin a search for a given node that is guaranteed
        // to be safe.  This is the only such place.
        //
        // Create schedule group takes a reference to the schedule group. The scheduling
        // node maintains this reference and will release it when it disassociates from the
        // schedule group (either in the destructor, or if the schedule group is moved to
        // a different node due to resource management reclaiming the node).
        //
        location unbiased;
        m_pAnonymousSegment = pScheduler->GetAnonymousScheduleGroup()->CreateSegment(&unbiased, this);
    }

    SchedulingRing::~SchedulingRing()
    {
        ASSERT(m_pAnonymousSegment != NULL);
        m_pAnonymousSegment = NULL;
    }

    //
    // Called when a schedule group's ref count is 0. remove this schedule group from the action.
    //
    void SchedulingRing::RemoveScheduleGroupSegment(ScheduleGroupSegmentBase *pSegment)
    {
        if (pSegment->GetAffinity()._Is_system())
            m_nonAffineSegments.Remove(pSegment);
        else
            m_affineSegments.Remove(pSegment);
    }

    /// <summary>
    ///     Activates the ring.
    /// </summary>
    void SchedulingRing::Activate()
    {
        InterlockedExchange(&m_active, 1);
    }

} // namespace details
} // namespace Concurrency
