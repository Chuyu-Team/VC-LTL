// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FreeVirtualProcessorRoot.cpp
//
// Part of the ConcRT Resource Manager -- this header file contains the internal implementation for the free virtual
// processor root (represents a virtual processor as handed to a scheduler).
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Constructs a new free virtual processor root.
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
    FreeVirtualProcessorRoot::FreeVirtualProcessorRoot(SchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex)
        : VirtualProcessorRoot(pSchedulerProxy, pNode, coreIndex),
        m_pExecutingProxy(NULL),
        m_pDeactivatedProxy(NULL)
    {
    }

    /// <summary>
    ///     Deletes the virtual processor.
    /// </summary>
    void FreeVirtualProcessorRoot::DeleteThis()
    {
        //
        // This comes in via a Remove() call on one of two threads:
        //
        //     - The thread that is running the virtual processor root.
        //           - There can be no race.  We just need to make sure that the thread on exit doesn't touch us after deletion.
        //
        //     - An arbitrary thread.
        //           - We need to be careful that we aren't racing between that thread's getting out (SwitchOut followed by returning from
        //             the context's dispatch loop), and it trying to reset the vproc root in ResetOnIdle.  We must spin until that has happened.
        //
        FreeThreadProxy *pCurrentProxy = NULL;

        DWORD tlsSlot = GetSchedulerProxy()->GetResourceManager()->GetExecutionResourceTls();
        void * tlsPointer = platform::__TlsGetValue(tlsSlot);
        size_t tlsValue = (size_t) tlsPointer;

        if (tlsPointer != NULL && ((tlsValue & TlsResourceBitMask) == TlsResourceInProxy))
            pCurrentProxy = (FreeThreadProxy *) (tlsValue & ~TlsResourceInProxy);

        if (pCurrentProxy != NULL && pCurrentProxy == m_pExecutingProxy)
        {
            pCurrentProxy->SetVirtualProcessorRoot(NULL);
        }
        else
        {
            //
            // Spin wait until there isn't anything running atop this virtual processor root.  Yes -- this means that someone had better be
            // on the way out.  If you call Remove on a virtual processor that's still running something, the resulting behavior is pretty much
            // undefined anyway.
            //
            SpinUntilIdle();
        }

        delete this;
    }

    /// <summary>
    ///     Called in order to reset this virtual processor root to a completely quiescent state (not running anything).
    /// </summary>
    /// <param name="switchState">
    ///     Indicates the state of the thread proxy that is making the call. The parameter is of type <typeparamref name="SwitchingProxyState"/>.
    /// </param>
    void FreeVirtualProcessorRoot::ResetOnIdle(SwitchingProxyState switchState)
    {
        FreeThreadProxy *pOriginalProxy = static_cast<FreeThreadProxy *>(m_pExecutingProxy);

        LONG newVal = InterlockedDecrement(&m_activationFence);
        if (newVal <= 0)
        {
            //
            // The value could be -1 if we raced with the virtual processor root being removed on a different thread.
            //
            ASSERT(newVal >= -1);
            //
            // The fence going down to zero arbitrates between a possible reset/remove race.
            //
            if (newVal == 0)
                Unsubscribe();

            m_pExecutingProxy = NULL;

            //
            // *** READ THIS ***:
            //
            // It is imperative on this path that once m_pExecutingProxy has been set to NULL, nothing touches the this pointer.  We are the race
            // resolution between a client getting off a vproc and removing it.  There can be a race between removal (DeleteThis) from outside and
            // a SwitchOut (here) on the vproc.
            //
            if (switchState == Blocking)
            {
                pOriginalProxy->SuspendExecution();
            }
        }
        else
        {
            Concurrency::IExecutionContext *pActivatedContext = AcquireActivatedContext();
            ASSERT(newVal == 1 && pActivatedContext != NULL);

            //
            // This means we had a race between an Activate and an Idling (via either SwitchOut or return from dispatch loop).  In either
            // of these cases, we stashed away the context which was activated in m_pActivatedContext.  This context now needs to run atop us.
            //
            FreeThreadProxy *pProxy = static_cast<FreeThreadProxy *> (pActivatedContext->GetProxy());
            ASSERT(pProxy != NULL);

            //
            // While it is safe to run through an X->X context switch after the blocked flag is set, there is no point.  If we raced a SwitchOut/Activate
            // for the same proxy on the same vproc, it's a NOP.
            //
            if (pOriginalProxy != pProxy)
            {
                pOriginalProxy->SwitchTo(pActivatedContext, switchState);
            }
        }
    }

    /// <summary>
    ///     Causes the scheduler to start running a thread proxy on the specified virtual processor root which will execute
    ///     the Dispatch method of the context supplied by pContext. Alternatively, it can be used to resume a
    ///     virtual processor root that was de-activated by a previous call to Deactivate.
    /// </summary>
    /// <param name="pContext">
    ///     The context which will be dispatched on a (potentially) new thread running atop this virtual processor root.
    /// </param>
    void FreeVirtualProcessorRoot::Activate(Concurrency::IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        //
        // If the context is being reused, it had better return a NULL thread proxy when we ask!  This is part of the spec contract.
        //
        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());
        if (pProxy == NULL)
        {
            pProxy = static_cast<FreeThreadProxy *> (GetSchedulerProxy()->GetNewThreadProxy(pContext));
        }

        //
        // All calls to Activate after the first one can potentially race with the paired deactivate. This is allowed by the API, and we use the fence below
        // to reduce kernel transitions in case of this race.
        //
        // We must also be careful because calls to activate can race with ResetOnIdle from either a SwitchOut() or a return from dispatch and we must
        // be prepared to deal with this and the implications around trying to bind pContext.
        //
        LONG newVal = InterlockedIncrement(&m_activationFence);
        if (newVal == 2)
        {
            ASSERT(m_pDeactivatedProxy == NULL);
            //
            // We received two activations in a row. According to the contract with the client, this is allowed, but we should expect a deactivation, a
            // SwitchOut, or a return from dispatch loop soon after.
            //
            // Simply return instead of signalling the event. The deactivation will reduce the count back to 1. In addition, we're not responsible
            // for changing the idle state on the core.
            //
            SetActivatedContext(pContext);
        }
        else
        {
            ASSERT(newVal == 1);

            SpinUntilIdle();
            ASSERT(m_pExecutingProxy == m_pDeactivatedProxy);

            if (m_pExecutingProxy != NULL)
            {
                //
                // The root already has an associated thread proxy.  Check that the context provided is associated with
                // the same proxy.
                //
                if (pProxy != m_pExecutingProxy)
                {
                    //
                    // This is a fatal exception.  We can potentially correct the state of the fence, but the scheduler is beyond confused about
                    // the spec.  @TODO: Is it worth making some attempt to correct *our* state given that it's already messed up above us?
                    //
                    throw invalid_operation();
                }
            }

            m_pDeactivatedProxy = NULL;

            //
            // An activated root increases the subscription level on the underlying core.
            //
            Subscribe();

            //
            // Affinitization sets this as the executing proxy for the virtual processor root.
            //
            Affinitize(pProxy);

            ASSERT(m_pExecutingProxy == pProxy);
            ASSERT(pProxy->GetVirtualProcessorRoot() != NULL);
            ASSERT(pProxy->GetExecutionContext() != NULL);

            pProxy->ResumeExecution();
        }
    }

    /// <summary>
    ///     Causes the thread proxy running atop this virtual processor root to temporarily stop dispatching pContext.
    /// </summary>
    /// <param name="pContext">
    ///     The context which should temporarily stop being dispatched by the thread proxy running atop this virtual processor root.
    /// </param>
    bool FreeVirtualProcessorRoot::Deactivate(Concurrency::IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        if (m_pExecutingProxy == NULL)
            throw invalid_operation();

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (m_pExecutingProxy != pProxy)
        {
            throw invalid_operation();
        }

        LONG newVal = InterlockedDecrement(&m_activationFence);

        if (newVal == 0)
        {
            //
            // Reduce the subscription level on the core while the root is suspended. The count is used by dynamic resource management
            // to tell which cores allocated to a scheduler are unused, so that they can be temporarily repurposed.
            //
            InterlockedExchangePointer(reinterpret_cast<void * volatile *>(&m_pDeactivatedProxy), m_pExecutingProxy);
            Unsubscribe();
            pProxy->SuspendExecution();
        }
        else
        {
            //
            // There should be no Deactivate/Remove races.
            //
            ASSERT(newVal == 1);

            Concurrency::IExecutionContext *pActivatedContext = AcquireActivatedContext();

            //
            // If we got here, it means while activated we saw an activation of pCtxX and a subsequent deactivation of pCtxY.  These contexts
            // must be equal to be spec legal.
            //
            ASSERT(pActivatedContext == pContext);

            //
            // The activation for this deactivation came in early, so we return early here without making a kernel transition.
            //
        }

        return true;
    }

    /// <summary>
    ///     Forces all data in the memory heirarchy of one processor to be visible to all other processors.
    /// </summary>
    /// <param name="pContext">
    ///     The context which is currently being dispatched by this root.
    /// </param>
    void FreeVirtualProcessorRoot::EnsureAllTasksVisible(Concurrency::IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        if (m_pExecutingProxy == NULL)
            throw invalid_operation();

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (m_pExecutingProxy != pProxy)
        {
            throw invalid_operation();
        }

        GetSchedulerProxy()->GetResourceManager()->FlushStoreBuffers();
    }

    /// <summary>
    ///     Called to affinitize the given thread proxy to this virtual processor.
    /// </summary>
    /// <param name="pThreadProxy">
    ///     The new thread proxy to run atop this virtual processor root.
    /// </param>
    void FreeVirtualProcessorRoot::Affinitize(FreeThreadProxy *pThreadProxy)
    {
        //
        // Wait until the thread proxy is firmly blocked. This is essential to prevent vproc root orphanage
        // if the thread proxy we're switching to is IN THE PROCESS of switching out to a different one. An example of how this
        // could happen:

        // 1] ctxA is running on vp1. It is in the process of blocking, and wants to switch to ctxB. This means ctxA's thread proxy
        // tpA must affintize ctxB's thread proxy tpB to its own vproc root, vproot1.

        // 2] At the exact same time, ctxA is unblocked by ctxY and put onto a runnables collection in its scheduler. Meanwhile, ctxZ
        // executing on vp2, has also decided to block. It picks ctxA off the runnables collection, and proceeds to switch to it.
        // This means that ctxZ's thread proxy tpZ must affinitize ctxA's thread proxy tpA to ITS vproc root vproot2.

        // 3] Now, if tpZ affintizes tpA to vproot2 BEFORE tpA has had a chance to affintize tpB to vproot1, tpB gets mistakenly
        // affintized to vproot2, and vproot1 is orphaned.

        // In order to prevent this, tpZ MUST wait until AFTER tpA has finished its affinitization. This is indicated via the
        // blocked flag. tpA will set its blocked flag to 1, after it has finished affintizing tpB to vproot1, at which point it is
        // safe for tpZ to modify tpA's vproc root and change it from vproot1 to vproot2.
        //

        pThreadProxy->SpinUntilBlocked();

        m_pExecutingProxy = pThreadProxy;
        pThreadProxy->SetVirtualProcessorRoot(this);

        HardwareAffinity newAffinity = GetSchedulerProxy()->GetNodeAffinity(GetNodeId());
        pThreadProxy->SetAffinity(newAffinity);
    }

} // namespace details
} // namespace Concurrency
