// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// CacheLocalScheduleGroup.h
//
// Header file containing CacheLocalScheduleGroup related declarations.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{

    class CacheLocalScheduleGroup;

    class CacheLocalScheduleGroupSegment : public ScheduleGroupSegmentBase
    {

    public:

        //
        // Public Methods
        //

        /// <summary>
        ///     Constructs a cache local schedule group segment
        /// </summary>
        CacheLocalScheduleGroupSegment(ScheduleGroupBase *pOwningGroup, SchedulingRing *pOwningRing, location* pSegmentAffinity) :
            ScheduleGroupSegmentBase(pOwningGroup, pOwningRing, pSegmentAffinity)
        {
        }

        /// <summary>
        ///     Places a chore in the mailbox associated with this schedule group segment.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to mail.
        /// </param>
        /// <returns>
        ///     The mailbox slot into which the chore was placed.
        /// </returns>
        /// <remarks>
        ///     A mailed chore should also be placed on its regular work stealing queue.  The mailing must come first and once mailed, the chore body
        ///     cannot be referenced until the slot is successfully claimed via a call to the ClaimSlot method.
        /// </remarks>
        Mailbox<_UnrealizedChore>::Slot MailChore(_UnrealizedChore *pChore);

        /// <summary>
        ///     Notifies virtual processors that work affinitized to them has become available in the schedule group segment.
        /// </summary>
        virtual void NotifyAffinitizedWork();

    protected:


    private:
        friend class SchedulerBase;
        friend class CacheLocalScheduleGroup;
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

    class CacheLocalScheduleGroup : public ScheduleGroupBase
    {
    public:

        /// <summary>
        ///     Constructs a new cache local schedule group.
        /// </summary>
        CacheLocalScheduleGroup(SchedulerBase *pScheduler, location* pGroupPlacement) :
            ScheduleGroupBase(pScheduler, pGroupPlacement)
        {
            m_kind = CacheLocalScheduling;
        }

        /// <summary>
        ///     Places a chore in a mailbox associated with the schedule group which is biased towards tasks being picked up from the specified
        ///     location.
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
                                                          ScheduleGroupSegmentBase ** ppDestinationSegment);
    protected:

        /// <summary>
        ///     Allocates a new cache local schedule group segment within the specified group and ring with the specified affinity.
        /// </summary>
        /// <param name="pSegmentAffinity">
        ///     The affinity for the segment.
        /// </param>
        /// <param name="pOwningRing">
        ///     The scheduling ring to which the newly allocated segment will belong.
        /// </param>
        /// <returns>
        ///     A new cache local schedule group within the specified group and ring with the specified affinity.
        /// </returns>
        virtual ScheduleGroupSegmentBase* AllocateSegment(SchedulingRing *pOwningRing, location* pSegmentAffinity)
        {
            return _concrt_new CacheLocalScheduleGroupSegment(this, pOwningRing, pSegmentAffinity);
        }
    };
} // namespace details
} // namespace Concurrency
