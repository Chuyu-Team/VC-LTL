// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessorRoot.cpp
//
// Part of the ConcRT Resource Manager -- this file contains the internal implementation for the base virtual
// processor root.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    // The current unique identifier being handed out to created virtual processor roots.
    long VirtualProcessorRoot::s_currentId = 0;

    /// <summary>
    ///     Constructs a new virtual processor root.
    /// </summary>
    /// <param name="pSchedulerProxy">
    ///     The scheduler proxy this root is created for. A scheduler proxy holds RM data associated with an instance of
    ///     a scheduler.
    /// </param>
    /// <param name="pNode">
    ///     The processor node that this root belongs to. The processor node is one among the nodes allocated to the
    ///     scheduler proxy.
    /// </param>
    /// <param name="coreIndex">
    ///     The index into the array of cores for the processor node specified.
    /// </param>
    VirtualProcessorRoot::VirtualProcessorRoot(SchedulerProxy * pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex)
        : m_pActivatedContext(NULL)
        , m_executionResource(pSchedulerProxy, pNode, coreIndex)
        , m_fRemovedFromScheduler(false)
        , m_fOversubscribed(false)
        , m_activationFence(0)
    {
        m_id = (unsigned int)InterlockedIncrement(&s_currentId);
        m_executionResource.MarkAsVirtualProcessorRoot(this);
    }

    /// <summary>
    ///     Called to indicate that a scheduler is done with a virtual processor root and wishes to return it to the resource manager.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler making the request to remove this virtual processor root.
    /// </param>
    void VirtualProcessorRoot::Remove(IScheduler *pScheduler)
    {
        if (pScheduler == NULL)
            throw std::invalid_argument("pScheduler");

        if (GetSchedulerProxy()->Scheduler() != pScheduler)
            throw invalid_operation();

        ResetSubscriptionLevel();

        // This particular call does not have to worry about the RM receiving a SchedulerShutdown for the scheduler proxy in question.
        GetSchedulerProxy()->DestroyVirtualProcessorRoot(this);
    }

    /// <summary>
    ///     Notifies the underlying RM that the core is subscribed by this root *if* necessary.
    /// </summary>
    void VirtualProcessorRoot::Subscribe()
    {
        //
        // Note that there is a possibility in a race between activate/deactivate or activate/idle that we might race between the increment and
        // decrement and see a subscription count of:
        //
        // 1->2->1 instead of 1->0->1
        //
        // This *SHOULD* be fine in the RM layer and might even be a more desirable outcome so RM doesn't try to "give away" the idle core
        // which is idle for a split second.  In any case, it is noted here and can be fixed here if deemed necessary at a later point
        // in time.
        //
        GetSchedulerProxy()->IncrementCoreSubscription(GetExecutionResource());
    }

    /// <summary>
    ///    Notifies the underlying RM that the core is not subscribed by this root *if* necessary.
    /// </summary>
    void VirtualProcessorRoot::Unsubscribe()
    {
        GetSchedulerProxy()->DecrementCoreSubscription(GetExecutionResource());
    }

    /// <summary>
    ///     This API is called when a virtual processor root is being destroyed. It removes the effect of this virtual processor root
    ///     on the subscription level for the underlying core.
    /// <summary>
    void VirtualProcessorRoot::ResetSubscriptionLevel()
    {

        //
        // This should **ONLY** be called in the remove path to quickly address the subscription level change.  There's now a race between this
        // happening from two different paths -- one a thread removing itself from a virtual processor via IThreadProxy::SwitchOut and another
        // via the removal of said virtual processor via IVirtualProcessorRoot::Remove.
        //
        // Since the fence can only be 1 or 0 at this point (you can't race remove/activate or there's another bug at a higher level), only the
        // entity which decrements the fence to 0 plays with the subscription level.
        //

#if defined(_DEBUG)
        //
        // Assert that there is no activate/remove race.  We must snap the value of the fence as our observation might change between two
        // reads.
        //
        LONG snapFence = m_activationFence;
        ASSERT(snapFence == 0 || snapFence == 1);
#endif // _DEBUG
        LONG newVal = InterlockedDecrement(&m_activationFence);
        if (newVal == 0)
        {
            //
            // The virtual processor was in an activated state when it was removed -- we need to reduce the subscription level here.
            //
            Unsubscribe();
        }
        else
        {
            //
            // The value could be -1 if we raced with the virtual processor switching out.
            //
            ASSERT(newVal == -1);
        }
    }

} // namespace details
} // namespace Concurrency
