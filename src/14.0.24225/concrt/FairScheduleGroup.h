// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FairScheduleGroup.h
//
// Header file containing FairScheduleGroup related declarations.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    class FairScheduleGroup;

    class FairScheduleGroupSegment : public ScheduleGroupSegmentBase
    {

    public:

        //
        // Public Methods
        //

        /// <summary>
        ///     Constructs a fair schedule group segment
        /// </summary>
        FairScheduleGroupSegment(ScheduleGroupBase *pOwningGroup, SchedulingRing *pOwningRing, location* pSegmentAffinity) :
            ScheduleGroupSegmentBase(pOwningGroup, pOwningRing, pSegmentAffinity)
        {
        }

        /// <summary>
        ///     Notifies virtual processors that work affinitized to them has become available in the schedule group segment.
        /// </summary>
        virtual void NotifyAffinitizedWork() { }

    private:
        friend class SchedulerBase;
        friend class FairScheduleGroup;
        friend class ContextBase;
        friend class ExternalContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class SchedulingNode;
        friend class SchedulingRing;
        friend class VirtualProcessor;

        //
        // Private data
        //

        // Each schedule group has three stores of work. It has a collection of runnable contexts,
        // a FIFO queue of realized chores and a list of workqueues that hold unrealized chores.

        // A collection of Runnable contexts.
        SafeSQueue<InternalContextBase, _HyperNonReentrantLock> m_runnableContexts;

        //
        // Private methods
        //

        /// <summary>
        ///     Puts a runnable context into the runnables collection in the schedule group.
        /// </summary>
        void AddToRunnablesCollection(InternalContextBase *pContext);

        InternalContextBase *GetRunnableContext()
        {
            if (m_runnableContexts.Empty())
                return NULL;

            InternalContextBase *pContext = m_runnableContexts.Dequeue();
#if defined(_DEBUG)
            SetContextDebugBits(pContext, CTX_DEBUGBIT_REMOVEDFROMRUNNABLES);
#endif // _DEBUG
            return pContext;
        }

    };

    class FairScheduleGroup : public ScheduleGroupBase
    {
    public:

        /// <summary>
        ///     Constructs a new fair schedule group.
        /// </summary>
        FairScheduleGroup(SchedulerBase *pScheduler, location* pGroupPlacement) :
            ScheduleGroupBase(pScheduler, pGroupPlacement),
            m_pDefaultSegment(NULL)
        {
            ASSERT(pGroupPlacement->_Is_system());
            m_kind = FairScheduling;
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
        virtual ScheduleGroupSegmentBase *LocateSegment(location* pSegmentAffinity, bool fCreateNew);

        /// <summary>
        ///     Places a chore in a mailbox associated with the schedule group which is biased towards tasks being picked up from the specified
        ///     location. For a fair schedule group, the function returns an empty slot
        /// </summary>
        /// <param name="pChore">
        ///     The chore to mail.
        /// </param>
        /// <param name="pPlacement">
        ///     A pointer to a location where the chore will be mailed.
        /// </param>
        /// <returns>
        ///     The mailbox slot into which the chore was placed.
        /// </returns>
        /// <remarks>
        ///     A mailed chore should also be placed on its regular work stealing queue.  The mailing must come first and once mailed, the chore body
        ///     cannot be referenced until the slot is successfully claimed via a call to the ClaimSlot method.
        /// </remarks>
        virtual Mailbox<_UnrealizedChore>::Slot MailChore(_UnrealizedChore * pChore,
                                                          location * pPlacement,
                                                          ScheduleGroupSegmentBase **)
        {
            (pChore); (pPlacement);
            return Mailbox<_UnrealizedChore>::Slot();
        }
    protected:

        /// <summary>
        ///     Allocates a new fair schedule group segment within the specified group and ring with the specified affinity.
        /// </summary>
        /// <param name="pSegmentAffinity">
        ///     The affinity for the segment.
        /// </param>
        /// <param name="pOwningRing">
        ///     The scheduling ring to which the newly allocated segment will belong.
        /// </param>
        /// <returns>
        ///     A new fair schedule group within the specified group and ring with the specified affinity.
        /// </returns>
        virtual ScheduleGroupSegmentBase* AllocateSegment(SchedulingRing *pOwningRing, location* pSegmentAffinity)
        {
            //
            // For fair schedule groups, we completely ignore any location hint since we are directed to round robin the groups anyway!
            //
            (pSegmentAffinity);
            location unbiased;
            return _concrt_new FairScheduleGroupSegment(this, pOwningRing, &unbiased);
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
        virtual ScheduleGroupSegmentBase *FindSegment(location* pSegmentAffinity, SchedulingRing *pRing);

        /// <summary>
        ///     Removes all schedule group segments from the group.
        /// </summary>
        virtual void RemoveSegments()
        {
            ScheduleGroupBase::RemoveSegments();
            m_pDefaultSegment = NULL;
        }

    private:

        FairScheduleGroupSegment *m_pDefaultSegment;

    };

} // namespace details
} // namespace Concurrency
