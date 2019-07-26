// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// CacheLocalScheduleGroup.cpp
//
// Implementation file for CacheLocalScheduleGroup.
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
    void CacheLocalScheduleGroupSegment::AddToRunnablesCollection(InternalContextBase* pContext)
    {
        m_runnableContexts.Enqueue(pContext);
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
    Mailbox<_UnrealizedChore>::Slot CacheLocalScheduleGroupSegment::MailChore(_UnrealizedChore *pChore)
    {
        //
        // There are two possible segments to which pChore can be accounted.  One is the segment where it will appear on the WSQ -- the other is
        // the segment where it will appear on the mailbox.  Both are in the same group and hence we do not at present have reference counting
        // issues.  It will be attributed to the group it was picked up from which will further honor that affinity if the task blocks, etc...
        //
        ASSERT(!m_affinity._Is_system());
        Mailbox<_UnrealizedChore>::Slot affinitySlot = m_mailedTasks.Enqueue(pChore);

#if _UMSTRACE && _CHORETRACK
        OMTRACE(MTRACE_EVT_CHOREMAILED, this, SchedulerBase::FastCurrentContext(), NULL, pChore);
        OMTRACE(MTRACE_EVT_CHOREMAILED, m_pOwningGroup, SchedulerBase::FastCurrentContext(), NULL, pChore);
#endif // _UMSTRACE && _CHORETRACK

        ASSERT(!affinitySlot.IsEmpty());
        return affinitySlot;
    }

    /// <summary>
    ///     Notifies virtual processors that work affinitized to them has become available in the schedule group segment.
    /// </summary>
    void CacheLocalScheduleGroupSegment::NotifyAffinitizedWork()
    {
        SchedulerBase *pScheduler = m_pOwningGroup->GetScheduler();
        pScheduler->PostAffinityMessage(m_affinitySet);

        //
        // If this item qualifies for the quick cache, stash it.
        //
        if (m_affinity._GetType() == location::_ExecutionResource)
        {
            pScheduler->SetQuickCacheSlot(m_maskIdIf, this);
        }
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
    Mailbox<_UnrealizedChore>::Slot CacheLocalScheduleGroup::MailChore(_UnrealizedChore * pChore,
                                                                       location * pPlacement,
                                                                       ScheduleGroupSegmentBase ** ppDestinationSegment)
    {
        CacheLocalScheduleGroupSegment * pCacheLocalSegment = static_cast<CacheLocalScheduleGroupSegment *>(LocateSegment(pPlacement, true));
        *ppDestinationSegment = pCacheLocalSegment;
        return pCacheLocalSegment->MailChore(pChore);
    }

} // namespace details
} // namespace Concurrency
