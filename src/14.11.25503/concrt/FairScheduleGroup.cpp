// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FairScheduleGroup.cpp
//
// Implementation file for FairScheduleGroup.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Puts a runnable context into the runnables collection in the schedule group.
    /// </summary>
    void FairScheduleGroupSegment::AddToRunnablesCollection(InternalContextBase* pContext)
    {
        m_runnableContexts.Enqueue(pContext);
    }

    /// <summary>
    ///     Locates a segment that is appropriate for scheduling a task within the schedule group given information about the task's placement
    ///     and the origin of the thread making the call.
    /// </summary>
    /// <param name="pSegmentAffinity">
    ///     A segment with affinity to this particular location will be located.
    /// </paaram>
    /// <param name="fCreateNew">
    ///     An indication as to whether the schedule group can create a new segment if an appropriate segment cannot be found.  If this parameter is
    ///     specified as true, NULL will never be returned from this method; otherwise, it can be if no matching segment can be found.
    /// </param>
    /// <returns>
    ///     A segment appropriate for scheduling work with affinity to segmentAffinity from code executing at origin.  Note that NULL may be returned
    ///     if fCreateNew is specified as false and no appropriate segment yet exists for the group.
    /// </returns>
    ScheduleGroupSegmentBase *FairScheduleGroup::LocateSegment(location*, bool fCreateNew)
    {
        // Ignore the passed in affinity for fair schedule groups.
        location unbiased;
        if (m_kind & AnonymousScheduleGroup)
        {
            //
            // In order to provide a "like" functionality to Dev10 for anonymous fair groups, we still let the group be split by rings.  Non-anonymous
            // groups are also treated identically to Dev10 -- they live in one ring which is more for separation than any biasing.
            //
            return ScheduleGroupBase::LocateSegment(&unbiased, fCreateNew);
        }
        else
        {
            ScheduleGroupSegmentBase *pSegment = m_pDefaultSegment;
            if (fCreateNew && !pSegment)
            {
                m_segmentLock._Acquire();
                if (m_pDefaultSegment)
                {
                    pSegment = m_pDefaultSegment;
                }
                else
                {
                    pSegment = CreateSegment(&unbiased, m_pScheduler->GetNextSchedulingRing());
                    // CreateSegment adds the segment to the list array as its last step, which generates a fence ensuring that the segment
                    // is fully initialized before it is published on weaker memory models.
                    m_pDefaultSegment = static_cast<FairScheduleGroupSegment *>(pSegment);
                }
                m_segmentLock._Release();
            }

            return pSegment;
        }
    }

    /// <summary>
    ///     Internal routine which finds an appropriate segment for a task placement.
    /// </summary>
    /// <param name="pSegmentAffinity">
    ///     A segment with this affinity will be located.
    /// </param>
    /// <param name="pRing">
    ///     A segment with segmentAffinity within this ring will be found.  A given location may be split into multiple segments by node in order
    ///     to keep work local.
    /// </param>
    /// <returns>
    ///     A segment with the specified affinity close to the specified location.
    /// </returns>
    ScheduleGroupSegmentBase *FairScheduleGroup::FindSegment(location*, SchedulingRing *pRing)
    {
        // Ignore the passed in affinity for fair schedule groups
        location unbiased;
        if (m_kind & AnonymousScheduleGroup)
        {
            return ScheduleGroupBase::FindSegment(&unbiased, pRing);
        }
        else
        {
            return m_pDefaultSegment;
        }
    }

} // namespace details
} // namespace Concurrency
