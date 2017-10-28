// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FreeThreadProxy.cpp
//
// Part of the ConcRT Resource Manager -- this source file contains the internal definition for the free thread
// proxy.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Called in order to perform a cooperative context switch between one context and another.  After this call, pContext will
    ///     be running atop the virtual processor root and the context which was running will not.  What happens to the context that
    ///     was running depends on the value of the reason argument.
    /// </summary>
    /// <param name="pContext">
    ///     The context to cooperatively switch to.
    /// </param>
    /// <param name="switchState">
    ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
    ///     proxy and context.
    /// </param>
    void FreeThreadProxy::SwitchTo(Concurrency::IExecutionContext *pContext, SwitchingProxyState switchState)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        // Find out if this context already has a thread proxy, if not, we have to request one from the factory.
        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (pProxy == NULL)
        {
            // Find a thread proxy from the pool that corresponds to the stack size and priority we need. Since this
            // is a context in the same scheduler as the current context's scheduler, we can use existing values of
            // stack size and priority.
            pProxy = static_cast<FreeThreadProxy *> (m_pRoot->GetSchedulerProxy()->GetNewThreadProxy(pContext));
        }

        FreeVirtualProcessorRoot *pRoot = static_cast<FreeVirtualProcessorRoot *>(m_pRoot);
        m_pRoot = NULL;

        if (switchState == Blocking)
        {
            ASSERT(m_fBlocked == FALSE);
            InterlockedExchange(&m_fBlocked, TRUE);
        }

        // The 'next' thread proxy must be affinitized to a copy of the 'this' proxy's vproc root VPRoot1, snapped BEFORE the blocked flag
        // is set. Not doing this could result in vproc root orphanage. See VirtualProcessorRoot::Affinitize for details.
        pRoot->Affinitize(pProxy);

        switch (switchState)
        {
            case Blocking:
                //
                // Signal the other thread proxy and block until switched to, or until a virtual processor is activated with
                // the context running on this thread proxy.
                //
                platform::__SignalObjectAndWait(pProxy->m_hBlock, m_hBlock, INFINITE, TRUE);
                ASSERT(m_fBlocked == TRUE);
                InterlockedExchange(&m_fBlocked, FALSE);

                break;
            case Nesting:
                //
                // Signal the other thread proxy that now owns this virtual processor, but do not block. The current thread proxy
                // is about to move to a nested scheduler.
                //
                ASSERT(pProxy->m_pRoot != NULL);
                ASSERT(pProxy->m_pContext != NULL);
                pProxy->ResumeExecution();

                break;
            case Idle:
                //
                // Return without blocking, indicating to the caller that the scheduler should yield this thread proxy
                // back to the RM, by exiting the contexts dispatch loop.
                //
                ASSERT(pProxy->m_pRoot != NULL);
                ASSERT(pProxy->m_pContext != NULL);
                pProxy->ResumeExecution();

                break;
            default:

                ASSERT(false);
                break;
        }
    }

    /// <summary>
    ///     Called in order to disassociate the currently executing context from its virtual processor root, and reinitialize the root
    ///     for future use.
    /// </summary>
    /// <param name="switchState">
    ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
    ///     proxy and context.
    /// </param>
    void FreeThreadProxy::SwitchOut(SwitchingProxyState switchState)
    {
        if ((switchState == Idle) || (m_pRoot == NULL && switchState != Blocking))
            throw std::invalid_argument("switchState");

        ASSERT(m_fBlocked == 0);

        //
        // If a virtual processor root is removed on the thread running atop it, the virtual processor root's removal will NULL out this field indicating
        // that we are now a free thread.  If there is a virtual processor root, the scheduler still wants to keep the vproc root around and we must
        // correspondingly act as both a switch out and a deactivate.
        //
        if (m_pRoot != NULL)
        {
            FreeVirtualProcessorRoot * pRoot = static_cast<FreeVirtualProcessorRoot *>(m_pRoot);
            if (switchState == Nesting)
            {
                // IThreadProxy::SwitchOut can be called with Nesting, if the context tried to InternalContextBase::SwitchTo(NULL, Nesting). Ensure the
                // root is set to NULL here so the right thing happens with this context/proxy rejoins the scheduler by calling IThreadProxy::SwitchOut(Blocking).
                m_pRoot = NULL;
            }
            (static_cast<FreeVirtualProcessorRoot *>(pRoot))->ResetOnIdle(switchState);

            // If we're nesting, we should return without blocking with the root unchanged. If not, we should have been affinitized to a different root.
            ASSERT(m_pRoot != NULL || switchState == Nesting);
        }
        else
        {
            // There are currently only two cases where the m_pRoot field is expected to be NULL.
            //  - A virtual processor is being retired and the caller invokes SwitchOut to block the thread proxy.
            //      (root was set to NULL in FreeVirtualProcessorRoot::DeleteThis)
            //  - A thread proxy that previously switched to a different, nested scheduler, is now joining its original scheduler again.
            //      (root was set to NULL in FreeThreadProxy::SwitchOut or FreeThreadProxy::SwitchTo)
            SuspendExecution();
        }
    }

    /// <summary>
    ///     Called right after obtaining a thread proxy from the factory. Associates the thread proxy with the execution
    ///     context it is about to run.
    /// </summary>
    void FreeThreadProxy::AssociateExecutionContext(Concurrency::IExecutionContext * pContext)
    {
        m_pContext = pContext;
        pContext->SetProxy(this);
    }

    /// <summary>
    ///     Returns a thread proxy to the factory when it is no longer in use.
    /// </summary>
    void FreeThreadProxy::ReturnIdleProxy()
    {
        _CONCRT_ASSERT(m_pFactory != NULL);
        m_pContext = NULL;
        m_pFactory->ReclaimProxy(this);
    }

    /// <summary>
    ///     The main dispatch loop for the free thread proxy.
    /// </summary>
    void FreeThreadProxy::Dispatch()
    {
        // Send the default dispatch state into Dispatch.
        DispatchState dispatchState;

        if (!m_fCanceled)
        {
            platform::__TlsSetValue(m_pFactory->GetExecutionResourceTls(), (LPVOID) (((size_t) this) | TlsResourceInProxy));
        }

        while (!m_fCanceled)
        {
            _CONCRT_ASSERT(m_pContext != NULL);
            _CONCRT_ASSERT(m_pRoot != NULL);

            // Call the dispatch loop of the registered context.
            m_pContext->SetProxy(this);
            m_pContext->Dispatch(&dispatchState);

            //
            // The dispatch loop returns when the scheduler that the proxy was given to, has decided to return it back to the RM.
            // It should be returned to the free proxy factory, so that it can be handed out to a different virtual processor root
            // (bound to a different context).
            //
            // Before doing so, however, we restore the virtual processor to its original state so that it can be activated again.  Note
            // that if the virtual processor deleted on the way out, m_pRoot is already NULL.  This is only thread which does this and
            // we are on the same thread.  There is no race.
            //
            FreeVirtualProcessorRoot *pRoot = static_cast<FreeVirtualProcessorRoot *>(m_pRoot);

            m_pContext = NULL;
            m_pRoot = NULL;

            // Return to the idle pool in the RM. If the pool is full, the proxy will be canceled.
            ReturnIdleProxy();

            if (pRoot != NULL)
            {
                pRoot->ResetOnIdle(Blocking);
            }
            else
            {
                SuspendExecution();
            }
        }
    }
} // namespace details
} // namespace Concurrency
