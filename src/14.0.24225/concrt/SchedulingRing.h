// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingRing.h
//
// Source file containing the SchedulingRing declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A scheduling node corresponds to a NUMA node or a processor package; containing one or more virtual processor groups.
    /// </summary>
    class SchedulingRing
    {
    public:
        SchedulingRing(SchedulerBase *pScheduler, int id);

        ~SchedulingRing();

        int Id() const
        {
            return m_id;
        }

        // Create a new Schedule Group
        ScheduleGroupBase *AllocateScheduleGroup();

        // Delete a Schedule Group
        void FreeScheduleGroup(ScheduleGroupBase *pGroup);

        // Create a schedule group, add it to the list of groups
        ScheduleGroupBase *CreateScheduleGroup();

        ScheduleGroupSegmentBase *GetAnonymousScheduleGroupSegment() const
        {
            return m_pAnonymousSegment;
        }

        /// <summary>
        ///     Returns a shared index to pseudo-round robin through affine schedule group segments within the ring.
        /// </summary>
        ScheduleGroupSegmentBase *GetPseudoRRAffineScheduleGroupSegment(int *pIdx)
        {
            int min = m_nextAffineSegment;

            ScheduleGroupSegmentBase *pSegment = FindScheduleGroupSegment(min, m_affineSegments.MaxIndex(), pIdx, &m_affineSegments);
            if (pSegment == NULL && min != 0)
                pSegment = FindScheduleGroupSegment(0, min, pIdx, &m_affineSegments);

            return pSegment;
        }

        /// <summary>
        ///     Returns a shared index to pseudo-round robin through non-affine schedule group segments within the ring.
        /// </summary>
        ScheduleGroupSegmentBase *GetPseudoRRNonAffineScheduleGroupSegment(int *pIdx)
        {
            int min = m_nextNonAffineSegment;

            ScheduleGroupSegmentBase *pSegment = FindScheduleGroupSegment(min, m_nonAffineSegments.MaxIndex(), pIdx, &m_nonAffineSegments);
            if (pSegment == NULL && min != 0)
                pSegment = FindScheduleGroupSegment(0, min, pIdx, &m_nonAffineSegments);

            return pSegment;
        }

        /// <summary>
        ///     Sets a shared index to pseudo-round robin through affine schedule group segments within the ring.  This sets the index
        ///     to the schedule group segment *AFTER* idx in the iterator position.
        /// </summary>
        void SetPseudoRRAffineScheduleGroupSegmentNext(int idx)
        {
            m_nextAffineSegment = (idx + 1) % (m_affineSegments.MaxIndex());
            ASSERT(m_nextAffineSegment >= 0);
        }

        /// <summary>
        ///     Sets a shared index to pseudo-round robin through non-affine schedule group segments within the ring.  This sets the index
        ///     to the schedule group segment *AFTER* idx in the iterator position.
        /// </summary>
        void SetPseudoRRNonAffineScheduleGroupSegmentNext(int idx)
        {
            m_nextNonAffineSegment = (idx + 1) % (m_nonAffineSegments.MaxIndex());
            ASSERT(m_nextNonAffineSegment >= 0);
        }

        /// <summary>
        ///     Returns the first affine schedule group segment within the ring.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned schedule group segment will be placed here.  This can only be
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextAffineScheduleGroup.
        /// </param>
        ScheduleGroupSegmentBase *GetFirstAffineScheduleGroupSegment(int *pIdx)
        {
            return GetFirstScheduleGroupSegment(pIdx, &m_affineSegments);
        }

        /// <summary>
        ///     Returns the next affine schedule group segment in an iteration.
        /// </summary>
        ScheduleGroupSegmentBase *GetNextAffineScheduleGroupSegment(int *pIdx, int idxStart = 0)
        {
            return GetNextScheduleGroupSegment(pIdx, idxStart, &m_affineSegments);
        }

        /// <summary>
        ///     Returns the first non-affine schedule group segment within the ring.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned schedule group segment will be placed here.  This can only be
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextNonAffineScheduleGroup.
        /// </param>
        ScheduleGroupSegmentBase *GetFirstNonAffineScheduleGroupSegment(int *pIdx)
        {
            return GetFirstScheduleGroupSegment(pIdx, &m_nonAffineSegments);
        }

        /// <summary>
        ///     Returns the next non-affine schedule group segment in an iteration.
        /// </summary>
        ScheduleGroupSegmentBase *GetNextNonAffineScheduleGroupSegment(int *pIdx, int idxStart = 0)
        {
            return GetNextScheduleGroupSegment(pIdx, idxStart, &m_nonAffineSegments);
        }

        /// <summary>
        ///     Returns the node which owns this ring.
        /// </summary>
        SchedulingNode *GetOwningNode() const
        {
            return m_pNode;
        }

        /// <summary>
        ///     Returns whether this is an active ring or not.
        /// </summary>
        bool IsActive() const
        {
            return (m_active != 0);
        }

        /// <summary>
        ///     Activates the ring.
        /// </summary>
        void Activate();

    private:
        friend class SchedulerBase;
        friend class ScheduleGroupBase;
        friend class ScheduleGroupSegmentBase;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class SchedulingNode;
        friend class VirtualProcessor;
        friend class InternalContextBase;
        friend class ThreadInternalContext;

        // Owning scheduler
        SchedulerBase *m_pScheduler;

        // Owning Node
        SchedulingNode *m_pNode;

        // The anonymous schedule group - for external contexts and tasks without an explicitly specified schedule group.
        // There is one anonymous group segment per scheduling node.
        ScheduleGroupSegmentBase * m_pAnonymousSegment;

        // Scheduler group segments owned by this ring which have explicitly specified affinity.
        ListArray<ScheduleGroupSegmentBase> m_affineSegments;

        // Scheduler groups segments owned by this ring which do not have explicitly specified affinity.
        ListArray<ScheduleGroupSegmentBase> m_nonAffineSegments;

        // Pseudo Round robin indicies.
        int m_nextAffineSegment;
        int m_nextNonAffineSegment;

        int m_id;

        // An indication as to whether the ring is active.
        volatile LONG m_active;

        // Removes the schedule group segment from the appropriate list.
        void RemoveScheduleGroupSegment(ScheduleGroupSegmentBase* pGroup);

        void SetOwningNode(SchedulingNode *pNode)
        {
            m_pNode = pNode;
        }

        /// <summary>
        ///     Returns the first schedule group segment in the non-cyclic range [min, max).  If such is found
        ///     the schedule group segment is returned and pIdx contains its index within the list array.
        ///     If not found, NULL is returned and the value in pIdx is unspecified.
        /// </summary>
        ScheduleGroupSegmentBase *FindScheduleGroupSegment(int min, int max, int *pIdx, ListArray<ScheduleGroupSegmentBase> *pSegmentList)
        {
            ScheduleGroupSegmentBase *pSegment = NULL;
            int i = min;
            for (; i < max && pSegment == NULL; ++i)
            {
                pSegment = (*pSegmentList)[i];
            }

            //
            // The loop incremented "i" prior to the check.  If found, the index is i - 1.  If not, we care
            // not what pIdx contains.
            //
            *pIdx = i - 1;
            return pSegment;
        }

        /// <summary>
        ///     Returns the first schedule group.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned schedule group will be placed here.  This can only be
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextScheduleGroup.
        /// </param>
        ScheduleGroupSegmentBase *GetFirstScheduleGroupSegment(int *pIdx, ListArray<ScheduleGroupSegmentBase>* pSegmentList)
        {
            return FindScheduleGroupSegment(0, pSegmentList->MaxIndex(), pIdx, pSegmentList);
        }

        /// <summary>
        ///     Returns the next schedule group in an iteration.
        /// </summary>
        ScheduleGroupSegmentBase *GetNextScheduleGroupSegment(int *pIdx, int idxStart, ListArray<ScheduleGroupSegmentBase>* pSegmentList)
        {
            ScheduleGroupSegmentBase *pSegment = NULL;

            int min = *pIdx + 1;
            if (min > idxStart)
            {
                pSegment = FindScheduleGroupSegment(min, pSegmentList->MaxIndex(), pIdx, pSegmentList);
                min = 0;
            }

            if (pSegment == NULL)
                pSegment = FindScheduleGroupSegment(min, idxStart, pIdx, pSegmentList);

            return pSegment;
        }
    };
} // namespace details
} // namespace Concurrency
