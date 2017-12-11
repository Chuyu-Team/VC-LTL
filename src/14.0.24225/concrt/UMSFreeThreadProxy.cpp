// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSFreeThreadProxy.cpp
//
// Proxy for a UMS thread on a free scheduler.
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
    void UMSFreeThreadProxy::SwitchTo(Concurrency::IExecutionContext * pContext, SwitchingProxyState switchState)
    {
        if (pContext == NULL)
        {
            throw std::invalid_argument("pContext");
        }

        //
        // It's always possible that someone decided to switch to the primary.  They had better be inside a critical region if they
        // are performing this action since the primary is bound to a particular virtual processor.
        //
        EnterCriticalRegion();
        SchedulerProxy * pSchedulerProxy = NULL;

        if (m_pRoot->IsSchedulingContext(pContext))
        {
            pContext = NULL;
        }
        else
        {
            // Capture the scheduler proxy within the critical region in case we need to retrieve a thread proxy later
            pSchedulerProxy = m_pRoot->GetSchedulerProxy();
        }
        ExitCriticalRegion();

        CONCRT_COREASSERT(pContext != NULL || GetCriticalRegionType() != OutsideCriticalRegion);
        UMSFreeThreadProxy *pProxy = NULL;

        if (pContext != NULL)
        {
            // Find out if this context already has a thread proxy, if not we have to request one from the factory.

            pProxy = static_cast<UMSFreeThreadProxy *> (pContext->GetProxy());

            if (pProxy == NULL)
            {
                ASSERT(pSchedulerProxy != NULL);
                pProxy = static_cast<UMSFreeThreadProxy *> (pSchedulerProxy->GetNewThreadProxy(pContext));
            }
        }

        InternalSwitchTo(pProxy, switchState);
        return;
    }

    /// <summary>
    ///     Called in order to disassociate the currently executing context from its virtual processor root, and reinitialize the root
    ///     for future use.
    /// </summary>
    /// <param name="switchState">
    ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
    ///     proxy and context.
    /// </param>
    void UMSFreeThreadProxy::SwitchOut(SwitchingProxyState switchState)
    {
        if (switchState == Idle)
            throw std::invalid_argument("switchState");

        InternalSwitchOut(switchState);
        return;
    }

    /// <summary>
    ///     Called in order to yield to the underlying operating system. This allows the operating system to schedule
    ///     other work in that time quantum.
    /// </summary>
    void UMSFreeThreadProxy::YieldToSystem()
    {
        InternalYieldToSystem();
        return;
    }

    /// <summary>
    ///     The main dispatch routine for a UMS free thread proxy
    /// </summary>
    void UMSFreeThreadProxy::Dispatch()
    {
        // Send the default dispatch state into Dispatch
        DispatchState dispatchState;

        if (!m_fCanceled)
        {
            TlsSetValue(m_pFactory->GetExecutionResourceTls(), (LPVOID) (((size_t) this) | TlsResourceInUMSProxy));
        }

        while (!m_fCanceled)
        {
            // Reset idle pooled since it could be set if the proxy is from
            // the idle pool.
            m_fIdlePooled = false;

            Concurrency::IExecutionContext * pContext = m_pContext;
            pContext->Dispatch(&dispatchState);

            //
            // The thread proxy might have already been handed out.  It was put on the idle pool during the SwitchTo(..., Idle) inside the
            // primary.  We need to make a distinction between this case and someone simply arbitrarily exiting the dispatch loop.  If the last
            // yielding action (this is reset upon SwitchTo) was an ActionSwitchToAndRetire, the thread proxy has been grabbed off the pool by someone
            // else and we simply redispatch to a new context.  Note that it might be the same context if the scheduler reuses contexts (which
            // ConcRT does), so don't we cannot make any determination based on that.  We cannot simply check m_yieldAction because the thread proxy might
            // have been reused and that cleared.  The base class (UMSThreadProxy) will set m_fIdlePooled if the thread was idle pooled.  In this case,
            // redispatch.
            //
            if (m_fIdlePooled)
            {
#if defined(_DEBUG)
                ClearShutdownValidations();
#endif // _DEBUG

                m_fIdlePooled = false;
                continue;
            }

            FreeViaExit();

#if defined(_DEBUG)
            ClearShutdownValidations();
#endif // _DEBUG
        }
    }

    /// <summary>
    ///     Called right after obtaining a thread proxy from the factory. Associates the thread proxy with the execution
    ///     context it is about to run.
    /// </summary>
    void UMSFreeThreadProxy::AssociateExecutionContext(Concurrency::IExecutionContext * pContext)
    {
        m_pContext = pContext;

        //
        // We utilize NULL upon burning a thread to exit a virtual processor when we do not need to actually dispatch a context.
        //
        if (pContext != NULL)
            pContext->SetProxy(this);

        RPMTRACE(MTRACE_EVT_CONTEXT_ACQUIRED, this, NULL, pContext);
    }

    /// <summary>
    ///     Returns a thread proxy to the factory when it is no longer in use.
    /// </summary>
    void UMSFreeThreadProxy::ReturnIdleProxy()
    {
        //
        // This should only be called on the primary!
        //
        ClearCriticalRegion();
        _CONCRT_ASSERT(m_pFactory != NULL);
        m_fIdlePooled = true;
        m_pContext = NULL;
        m_pFactory->ReclaimProxy(this);
    }

} // namespace details
} // namespace Concurrency
