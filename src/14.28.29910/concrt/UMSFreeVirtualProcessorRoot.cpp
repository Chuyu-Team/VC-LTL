// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSFreeVirtualProcessorRoot.cpp
//
// Part of the ConcRT Resource Manager -- this header file contains the internal implementation for the UMS free virtual
// processor root (represents a virtual processor as handed to a scheduler).
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <awint.hpp> // TRANSITION, shouldn't include this STL-build-internal header

#pragma warning (push)
#pragma warning (disable : 4702) // unreachable code

namespace Concurrency
{
namespace details
{

    //
    // Defines the number of times we retry ExecuteUmsThread if the thread was suspended before we go on to some other thread.
    //
    const int NUMBER_OF_EXECUTE_SPINS = 10;

#if defined(_DEBUG)
    //
    // **************************************************
    // READ THIS VERY CAREFULLY:
    // **************************************************
    //
    // If you single step around in the UMS code, it's quite likely that the debugger triggered suspensions and context fetches will cause
    // the ExecuteUmsThread calls that SwitchTo utilizes to fail.  This will effectively trigger *DRAMATICALLY* different behavior when single
    // stepping the UMS code than not single stepping it.  If you set this flag (available for special kinds of debugging only), we will force
    // all ExecuteUmsThread calls to loop until they succeed.  This means that the vproc will spin wait until the thread is no longer suspended.
    // Note that if you set this flag, you *CANNOT* selectively suspend and resume threads in the debugger.  Doing so may cause the entire
    // scheduler to freeze.
    //
    BOOL g_InfiniteSpinOnExecuteFailure = FALSE;
#endif

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
    UMSFreeVirtualProcessorRoot::UMSFreeVirtualProcessorRoot(UMSSchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex)
        : VirtualProcessorRoot(pSchedulerProxy, pNode, coreIndex)
        , m_hPrimary(NULL)
        , m_hBlock(NULL)
        , m_pSchedulingContext(NULL)
        , m_pExecutingProxy(NULL)
        , m_fWokenByScheduler(true)
        , m_fActivated(false)
        , m_fDelete(false)
        , m_fStarted(false)
    {
        m_id = ResourceManager::GetThreadProxyId();

        m_hCriticalNotificationEvent = CreateEventW(NULL, FALSE, FALSE, NULL); // VSO#459907
        if (m_hCriticalNotificationEvent == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_hBlock = CreateEventW(NULL, FALSE, FALSE, NULL); // VSO#459907
        if (m_hBlock == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        CreatePrimary();
    }

    /// <summary>
    ///     Destroys a free virtual processor root.
    /// </summary>
    UMSFreeVirtualProcessorRoot::~UMSFreeVirtualProcessorRoot()
    {
        CloseHandle(m_hCriticalNotificationEvent);
        CloseHandle(m_hBlock);
        CloseHandle(m_hPrimary);
    }

    /// <summary>
    ///     Deletes the virtual processor.
    /// </summary>
    void UMSFreeVirtualProcessorRoot::DeleteThis()
    {
        //
        // We must be extraordinarily careful here!  The scheduler might have called for the removal of the virtual processor from one of two threads:
        // an arbitrary thread (no worries), the virtual processor thread itself (many worries).  Because the primary *IS* the virtual processor root,
        // we cannot simply delete the virtual processor out from underneath the running thread.  What if it page faults on the way out!  We must defer this
        // to the primary after the thread has exited the dispatch loop.  Hence, the deletion happens in a virtual function that can detect this!
        //
        CONCRT_COREASSERT(!OnPrimary());

        UMSThreadProxy *pProxy = UMSThreadProxy::GetCurrent();

        //
        // From now until the end of time, the proxy is in a hyper-critical region.  Let the thread running EXIT. This will be reset once the proxy
        // is on the free list.
        //
        if (pProxy != NULL)
            pProxy->EnterHyperCriticalRegion();

        m_fDelete = true;

        if (pProxy != NULL && pProxy->m_pRoot == this)
        {
            //
            // We are running atop *THIS* virtual processor.  The deletion must be deferred back to the primary thread *AFTER* getting off this one.
            // The switch back to the primary after getting OFF this thread will exit the primary dispatch loop and perform deletion of the virtual processor root.
            //

#if defined(_DEBUG)
            pProxy->SetShutdownValidations();
#endif // _DEBUG
        }
        else
        {
            //
            // We were running atop a *DIFFERENT* virtual processor (or an external context).  It's okay to let go of the critical region.
            //
            if (pProxy != NULL)
                pProxy->ExitHyperCriticalRegion();

            if (m_hPrimary != NULL)
            {
                //
                // We're not on the primary. It must be blocked on m_hBlock.  Wake it up and let it go.  The exit of the loop inside the primary will
                // delete this.
                //
                if (!m_fStarted)
                {
                    StartupPrimary();
                }
                else
                {
                    SetEvent(m_hBlock);
                }
            }
        }
    }

    /// <summary>
    ///     Creates the primary thread.
    /// </summary>
    void UMSFreeVirtualProcessorRoot::CreatePrimary()
    {
        CONCRT_COREASSERT(m_hPrimary == NULL);
        InitialThreadParam param(this);

        m_hPrimary = LoadLibraryAndCreateThread(NULL,
                                  0,
                                  PrimaryMain,
                                  &param,
                                  0,
                                  &m_primaryId);

        //
        // Keep a reference on the scheduler proxy.  The primary needs it as long as it is running!  The reference count needs to be placed after we are guaranteed
        // that the thread will run, before it does.
        //
        SchedulerProxy()->Reference();

        //
        // Make sure that the primary is appropriately affinitized before we actually run anything atop it.  The primary should **NEVER** need
        // to be reaffinitized and any UT which runs atop it will magically pick up this affinity through UMS.  The only cavaet to this is that
        // the affinity will only apply to K(p) and any UT running atop it.  Once a UT makes a transition into the kernel, a directed switch happens
        // to K(u) which has a separate affinity from KT(p).  If this only happens when the code is in the kernel, it might not be a problem; however --
        // if this kernel call does **NOT** block, UMS allows the UT to ride out on KT(u) instead of switching back to KT(p) as an optimization.  Now,
        // user code will be running atop KT(u) with a differing affinity than the primary.
        //
        // How often this happens is subject to performance analysis to determine whether it is worth it to reaffinitize KT(u) on user mode switching.  This
        // should only be done if **ABSOLUTELY NECESSARY** as it will force a call into the kernel for a user mode context switch (which somewhat defeats
        // the purpose).
        //
        SchedulerProxy()->GetNodeAffinity(GetNodeId()).ApplyTo(m_hPrimary);
        SetThreadPriority(m_hPrimary, SchedulerProxy()->ContextPriority());

        //
        // Wait for the thread to start. This ensures that the thread is at its PrimaryMain. When we start the primary due to an activation, it
        // needs to be able to handle blocked UTs. Therefore the primary shall not take any locks shared with the UT during StartupPrimary.
        //
        WaitForSingleObject(param.m_hEvent, INFINITE);
    }

    /// <summary>
    ///     Causes the scheduler to start running a thread proxy on the specified virtual processor root which will execute
    ///     the Dispatch method of the context supplied by pContext. Alternatively, it can be used to resume activate a
    ///     virtual processor root that was de-activated by a previous call to Deactivate.
    /// </summary>
    /// <param name="pContext">
    ///     The context which will be dispatched on a (potentially) new thread running atop this virtual processor root.
    /// </param>
    void UMSFreeVirtualProcessorRoot::Activate(Concurrency::IExecutionContext *pContext)
    {
        //
        // We must allow the scheduling context to be here in addition to what we think is here if it is allowed to activate/deactivate.
        // It's entirely possible that executing proxy is INVALID (already freed) if we get here from that path.
        //
        if (m_fActivated)
        {
            if (!IsSchedulingContext(pContext))
            {
                //
                // If the context is being reused, it had better return a NULL thread proxy when we ask!  This is part of the spec contract.
                //
                // One would hope that this is pre-bound.  Alas, it doesn't have to be.  I can't imagine any scheduler not doing so as the possibility
                // of thread explosion is too great.  Regardless, if the context is not bound, ensure that it is now.  The scheduler must deal with
                // any races due to the bind itself.
                //
                UMSFreeThreadProxy *pProxy = static_cast<UMSFreeThreadProxy *>(pContext->GetProxy());
                if (pProxy == NULL)
                {
                    try
                    {
                        pProxy = static_cast<UMSFreeThreadProxy *> (GetSchedulerProxy()->GetNewThreadProxy(pContext));
                    }
                    catch (const scheduler_worker_creation_error&)
                    {
                        InterlockedDecrement(&m_activationFence);
                        throw;
                    }
                }
            }

            //
            // Because a primary may spin on m_pActivatedContext after the CAS, the point between the CAS below and the setting of m_pActivatedContext
            // must be guarded by a critical region.
            //
            UMSThreadProxy::ScopedCriticalRegion cr;

            //
            // m_pExecutingProxy could be NULL. When a vproot is initially activated, it attempts to create an
            // internal context for SFW. However, the creation needs to happen outside the primary. Thus it is
            // possible that the vproot fails to get an internal context and deactivates. Note that its
            // m_pExecutingProxy is NULL since we haven't run any context on it.
            //

            //
            // All calls to Activate after the first one can potentially race with the paired deactivate. This is allowed by the API, and we use the fence below
            // to reduce kernel transitions in case of this race.
            //
            LONG newVal = InterlockedIncrement(&m_activationFence);
            if (newVal == 2)
            {
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

                Subscribe();
                SetActivatedContext(pContext);
                SetEvent(m_hBlock);

                //
                // In the event of an activation/completion race, the scheduler must swallow this set by performing a deactivate.  The scheduler can tell
                // via the return code from Deactivate.
                //

            }
        }
        else
        {
            CONCRT_COREASSERT(m_pExecutingProxy == NULL);

            //
            // The first activation *MUST* be the scheduling context.  It is uniquely bound to the virtual processor on which activate was called.
            //

            m_pSchedulingContext = static_cast<IExecutionContext *> (pContext);
            pContext->SetProxy(this);

            //
            // This is the first time a virtual processor root is activated. Mark it as non-idle for dynamic RM. In future, the core will
            // be marked as idle and non-idle in Deactivate. Also remember that the root is activated. A brand new root is considered idle
            // by dynamic RM until it is activated, but if it is removed from a scheduler before ever being activated, we need to revert the
            // idle state on the core.
            //
            m_fActivated = true;

            //
            // The activation fence Need not be interlocked, since it is not possible that this variable is being synchronously accessed
            // at the same time. The only other place the variable is accessed is in deactivate, and since this is the first activation -
            // a concurrent deactivation is impossible.  Likewise, an activate/remove race is impossible by spec.
            //
            m_activationFence = 1;

            //
            // An activated root increases the subscription level on the underlying core.
            //
            Subscribe();

            //
            // Only the primary has responsibility for affinitizing and actually executing the thread proxy.
            //
            StartupPrimary();
        }

        return;
    }

    /// <summary>
    ///     Causes the thread proxy running atop this virtual processor root to temporarily stop dispatching pContext.
    /// </summary>
    /// <param name="pContext">
    ///     The context which should temporarily stop being dispatched by the thread proxy running atop this virtual processor root.
    /// </param>
    bool UMSFreeVirtualProcessorRoot::Deactivate(Concurrency::IExecutionContext *pContext)
    {
        bool fPrimary = OnPrimary();

        if (pContext == NULL || (fPrimary && pContext != m_pSchedulingContext))
        {
            throw std::invalid_argument("pContext");
        }

        if (m_pExecutingProxy == NULL && !fPrimary)
        {
            throw invalid_operation();
        }

        //
        // As with Activate, the scheduling context may activate and deactivate which requires it to utilize its own IContext and not
        // the previously executing one.  Handle this case.
        //
        // Note that if pProxy is NULL at the end of this, we cannot touch m_pExecutingContext other than comparisons.  No fields may be
        // touched.  It may already be gone and freed.
        //
        UMSFreeThreadProxy *pProxy = NULL;
        IThreadProxy *pProxyIf = pContext->GetProxy();
        if (pProxyIf != this)
            pProxy = static_cast<UMSFreeThreadProxy *> (pContext->GetProxy());

        if (!fPrimary)
        {
            //
            // Deactivate has to come from the running thread (or the primary)
            //
            if (pProxy != NULL && (m_pExecutingProxy != pProxy || UMSThreadProxy::GetCurrent() != static_cast<UMSThreadProxy *>(pProxy)))
            {
                throw invalid_operation();
            }

            //
            // We had better be in a critical region on the **SCHEDULER SIDE** prior to calling this or all sorts of fun will ensue.
            //
            CONCRT_COREASSERT(pProxy == NULL || pProxy->GetCriticalRegionType() != OutsideCriticalRegion);
        }

        //
        // The activation fence is used to pair scheduler activates with corresponding deactivates. After the first activation, it is possible
        // that the next activation may arrive before the deactivation that it was meant for. In this case we skip the kernel transitions, and
        // avoid having to change the core subscription. Now, with UMS, it's possible that an 'activation' arrives from the RM. We can tell
        // that this is the case if the return value from the ->Deactivate and ->InternalDeactivate APIs is false. We count this as an RM
        // awakening, and don't decrement the fence on a subsequent deactivate.
        //

        LONG newVal = 0;

        if (m_fWokenByScheduler)
        {
            newVal = InterlockedDecrement(&m_activationFence);
        }
        else
        {
            //
            // We were woken up by the RM. newVal is left at 0, which will force us to deactivate. The activation fence could change
            // from 0 to 1 if a corresponding activation arives from the scheduler. The order of the assert below is important.
            // First compare with 0, then 1.
            //
            ASSERT(m_activationFence == 0 || m_activationFence == 1);
        }

        bool fAcquireCtx = false;

        if (newVal == 0)
        {
            //
            // Reduce the subscription level on the core while the root is suspended. The count is used by dynamic resource management
            // to tell which cores allocated to a scheduler are unused, so that they can be temporarily repurposed.
            //
            if (m_fWokenByScheduler)
            {
                Unsubscribe();
            }

            if (pProxy != NULL)
            {
                m_fWokenByScheduler = pProxy->Deactivate();
            }
            else
            {
                m_fWokenByScheduler = InternalDeactivate();
            }

            fAcquireCtx = m_fWokenByScheduler;
        }
        else
        {
            //
            // There should be no Deactivate/Remove races.
            //
            ASSERT(newVal == 1);

            fAcquireCtx = true;

            //
            // The activation for this deactivation came in early, so we return early here without making a kernel transition.
            //
        }

        if (fAcquireCtx)
        {
            Concurrency::IExecutionContext *pActivatedContext = AcquireActivatedContext();

            //
            // If we got here, it means while activated we saw an activation of pCtxX and a subsequent deactivation of pCtxY.  These contexts
            // must be equal to be spec legal.
            //
            ASSERT(pActivatedContext == pContext);
        }

        return m_fWokenByScheduler;
    }

    /// <summary>
    ///     Forces all data in the memory heirarchy of one processor to be visible to all other processors.
    /// </summary>
    /// <param name="pContext">
    ///     The context which is currently being dispatched by this root.
    /// </param>
    void UMSFreeVirtualProcessorRoot::EnsureAllTasksVisible(Concurrency::IExecutionContext *pContext)
    {
        bool fPrimary = OnPrimary();

        if (pContext == NULL || (fPrimary && pContext != m_pSchedulingContext))
        {
            throw std::invalid_argument("pContext");
        }

        if (m_pExecutingProxy == NULL && !fPrimary)
        {
            throw invalid_operation();
        }

        //
        // Note that if pProxy is NULL at the end of this, we cannot touch m_pExecutingContext other than comparisons.  No fields may be
        // touched.  It may already be gone and freed.
        //
        UMSFreeThreadProxy *pProxy = NULL;
        IThreadProxy *pProxyIf = pContext->GetProxy();
        if (pProxyIf != this)
            pProxy = static_cast<UMSFreeThreadProxy *> (pContext->GetProxy());

        if (!fPrimary)
        {
            //
            // We expect this call to be made from within a dispatch loop or the primary.
            //
            if (pProxy != NULL && (m_pExecutingProxy != pProxy || UMSThreadProxy::GetCurrent() != static_cast<UMSThreadProxy *>(pProxy)))
            {
                throw invalid_operation();
            }

            //
            // We must be in a critical region on the **SCHEDULER SIDE** prior to calling this
            //
            CONCRT_COREASSERT(pProxy == NULL || pProxy->GetCriticalRegionType() != OutsideCriticalRegion);
        }

        // This will never run on XP, so calling this API here is fine.
        FlushProcessWriteBuffers();
    }

    /// <summary>
    ///     Performs a deactivation of the virtual processor.  This is always called on the primary.  A user thread which deactivates must defer the
    ///     call to the primary to perform the blocking action.
    /// </summary>
    bool UMSFreeVirtualProcessorRoot::InternalDeactivate()
    {
        CONCRT_COREASSERT(OnPrimary());

        HANDLE hObjects[3];
        int count = 2;

        //
        // Wait for either an activation (m_hBlock) or a completion (the other two events).  Signal back the
        // context the reason it awoke.
        //
        hObjects[0] = m_hBlock;
        hObjects[1] = SchedulerProxy()->GetCompletionListEvent();
        hObjects[2] = SchedulerProxy()->GetTransferListEvent();

        if (hObjects[2] != NULL) count++;

        CONCRT_COREASSERT(count <= (sizeof(hObjects) / sizeof(HANDLE)));

        DWORD result;
        for(;;)
        {
            result = WaitForMultipleObjectsEx(count, hObjects, FALSE, INFINITE, FALSE);
            int index = result - WAIT_OBJECT_0;

            if (index == 1)
            {
                CONCRT_COREASSERT(index < count);
                CONCRT_COREASSERT(hObjects[index] == SchedulerProxy()->GetCompletionListEvent());

                // Completion list events should be handled by RM
                SchedulerProxy()->SweepCompletionList();

                // Go back to the loop and wait.
                continue;
            }

            // The scheduler needs to be notified for both activation and transfer list events
            break;
        }

        return (result == WAIT_OBJECT_0);
    }

    /// <summary>
    ///     Marks a particular UMS thread proxy as running atop this UMS virtual processor root.
    /// </summary>
    /// <param name="pProxy">
    ///     The proxy which is to run atop this virtual processor root.
    /// </param>
    /// <param name="fCriticalReexecute">
    ///     Is the affinitization due to a critical execution happening on the same vproc.
    /// </param>
    void UMSFreeVirtualProcessorRoot::Affinitize(UMSFreeThreadProxy *pProxy, bool fCriticalReexecute)
    {
        //
        // See FreeVirtualProcessorRoot::Affinitize.  There is an additional detail here: it's entirely possible that a UMS thread blocks
        // and comes back on the completion list before the primary has a chance to do anything (like resetting m_pRoot, etc...).  Another
        // primary might pick this up off the completion list before we even *REALIZE* it's blocked.  We must maintain consistent state.  Until
        // the original primary has had the chance to clear m_pRoot and do everything necessary to maintain state, prevent the switching to
        // pProxy.
        //
        // Critical reexecution need not "spin until blocked" as we've never set the blocked flag.  We must *COMPLETELY MASK* such blocking from
        // above layers.
        //
        if (!fCriticalReexecute)
            pProxy->SpinUntilBlocked();

        RVPMTRACE(MTRACE_EVT_AFFINITIZED, pProxy, this, NULL);

        if (pProxy->m_pLastRoot != this)
        {
#if defined(_DEBUG)
            //
            // If we were last running on a different vproc, we had better not be flagged as critically blocked!
            //
            CONCRT_COREASSERT((pProxy->m_UMSDebugBits & UMS_DEBUGBIT_CRITICALBLOCK) == 0);
#endif // _DEBUG

            //
            // Normally, we will inherit affinity from the primary as UT(u)/KT(p) pair.  If pProxy enters the kernel, however, the directed switch
            // happens and we run atop KT(u).  This does not have the same affinity.  While the original switch elides the scheduler and runs atop the same
            // core, there are two potential problems:
            //
            // - If the thread quantum expires while KT(u) is in the kernel and the NT scheduler reschedules KT(u), it will reschedule with the affinity
            //   of KT(u) not that picked up off KT(p).
            //
            // - If KT(u) does not block in the kernel, the UMS system will let UT(u) ride out on KT(u) as an optimization for threads making repeated non-blocking
            //   calls into the kernel.  Again, if there is no thread quantum expiration, no problem exists.  If the thread quantum does expire, however, the
            //   same issue exists as above -- the NT scheduler will reschedule UT(u)/KT(u) with the affinity of KT(u) not that picked up off KT(p).
            //
            // Ideally, we'd like to have the same affinity for KT(u) as KT(p).  Making a call to change the affinity here, however, is a performance hit for
            // user mode context switching -- we will enter the kernel for a user mode switch. SetAffinity avoids the kernel transition if the affinity is the
            // same.
            //
            HardwareAffinity newAffinity = SchedulerProxy()->GetNodeAffinity(GetNodeId());
            pProxy->SetAffinity(newAffinity);

#if defined(_DEBUG)

            //
            // Validate that all the affinities are what we think they are.  If we switch to a UT that doesn't overlap our affinity mask, there's an absolutely
            // enormous performance penalty in Win7.
            //
            HardwareAffinity realPrimaryAffinity(GetCurrentThread());
            HardwareAffinity expectedPrimaryAffinity = SchedulerProxy()->GetNodeAffinity(GetNodeId());
            HardwareAffinity realUTAffinity(pProxy->GetThreadHandle());

            CONCRT_COREASSERT(expectedPrimaryAffinity == realPrimaryAffinity);
            CONCRT_COREASSERT(newAffinity == realPrimaryAffinity);
            CONCRT_COREASSERT(newAffinity == realUTAffinity);

#endif // _DEBUG

        }

        pProxy->PrepareToRun(this);

        m_pExecutingProxy = pProxy;
    }

    /// <summary>
    ///     Executes the specified proxy.  This can only be called from the primary thread!
    /// </summary>
    /// <param name="pProxy">
    ///     The thread proxy to execute.
    /// </param>
    /// <param name="fromSchedulingContext">
    ///     Whether the switch is happening as a result of a SwitchTo from the scheduling context.  On failure, we do not recursively reinvoke
    ///     the scheduling context, we simply return -- indicating failure.
    /// </param>
    /// <param name="fCriticalBlockAndExecute">
    ///     An indication as to whether the execution was due to the result of a critical block and subsequent execute.
    /// </param>
    /// <returns>
    ///     This does *NOT* return if execution is successful.  Any return indicates failure.
    /// </returns>
    void UMSFreeVirtualProcessorRoot::Execute(UMSFreeThreadProxy *pProxy, bool fromSchedulingContext, bool fCriticalBlockAndExecute)
    {
        //
        // *NOTE*:
        // This is the *ONLY* function which should call ExecuteUmsThread!
        //
        RVPMTRACE(MTRACE_EVT_EXECUTE, pProxy, this, NULL);

        CONCRT_COREASSERT(OnPrimary());

        if (pProxy == NULL)
        {
            //
            // Someone wants to *DIRECTLY* invoke the primary.  Note that this is *NOT* an infinite loop as the execution
            // will snap out the stack.
            //
            for(;;)
            {
                InvokeSchedulingContext(false);
            }
        }

        Affinitize(pProxy, fCriticalBlockAndExecute);

        //
        // If we blocked during a critical region and performed a critical block and execute, we must not play with the messaging block.  It's entirely
        // possible that someone page faulted in the critical region during UMSThreadProxy::InternalSwitchTo where the messaging block was being filled in
        // or during UmsThreadYield before we get back to the primary.  Altering the messaging block in that case will corrupt the thread proxy and lead
        // to issues down the line.  We can safely reaffinitize because it will restore exact state, but we cannot touch the messaging block.
        //
        if (!fCriticalBlockAndExecute)
            pProxy->m_yieldAction = UMSThreadProxy::ActionNone;

        bool fCritical = pProxy->GetCriticalRegionType() != OutsideCriticalRegion;

#if defined(_DEBUG)
        bool fSuspended = pProxy->IsSuspended();
        bool fTerminated = pProxy->IsTerminated();

        pProxy->m_UMSDebugBits = 0;
        pProxy->m_UMSLastExecuteError = 0;
#endif // _DEBUG

        int spinCount = NUMBER_OF_EXECUTE_SPINS;

        for(;;)
        {
            CONCRT_COREASSERT(spinCount > 0);

            UMS::ExecuteUmsThread(pProxy->GetUMSContext());

            //
            // If g_InfiniteSpinOnExecuteFailure is set, we want special debuggability around single stepping this code and we *FOREVER* spin waiting
            // for pProxy to become unsuspended.  This will allow us to single step around this code without triggering context switches
            //
#if defined(_DEBUG)
            if (g_InfiniteSpinOnExecuteFailure)
            {
                continue;
            }
#else // _DEBUG
            if (--spinCount == 0)
            {
                //
                // There's absolutely no point in handing something to the poller that's not suspended.  It's likely a KAPC is firing and handing to the
                // poller will just play thread debounce since there's no API to query for kernel locks.
                //
                if (pProxy->IsSuspended())
                    break;
                else
                    spinCount = NUMBER_OF_EXECUTE_SPINS;
            }
#endif // _DEBUG

            YieldProcessor();
        }

        //
        // If we returned from ExecuteUmsThread, the thread couldn't execute.  It might have been suspended since last going on the completion list or
        // yielding or it might be running an APC due to a GetThreadContext, etc...  The obvious thought here is to unwind and fail the SwitchTo call.
        // The unfortunate reality is that from the scheduler's perspective, pPreviousProxy might be runnable (it might have yielded) and another virtual
        // processor might be trying to SwitchTo that.  In this case, it will spin until NotifyBlocked is called.  The obvious thought here is that one
        // might just cascade failures and set some special signal to that thread in the event of failure.  The reality here is that ExecuteUmsThread
        // does *NOT RETURN* on success.  The stack of the primary is immediately snapped back to some RTL frame for UMS.  Hence, in the success case,
        //  we need to NotifyBlocked **BEFORE** calling ExecuteUmsThread.  This means that another virtual processor might **ALREADY** be
        // running pPreviousProxy.  We cannot unwind back to return failure from the SwitchTo.
        //
        // In order to deal with this situation, we take an alternate tact.  If the failure happens, we treat it as if the SwitchTo succeeded and then immediately
        // blocked on the next instruction triggering a return to primary.  We will reinvoke the scheduling context in order to make a decision.
        //
#if defined(_DEBUG)

        pProxy->m_UMSLastExecuteError = GetLastError();
        pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_HANDEDTOPOLLER |
                                  (fSuspended ? UMS_DEBUGBIT_EXECUTEFAILURESUSPENDED : 0) |
                                  (fTerminated ? UMS_DEBUGBIT_EXECUTEFAILURETERMINATED : 0);

        RVPMTRACE(MTRACE_EVT_EXECUTEFAIL, pProxy, this, pProxy->m_UMSDebugBits);

#endif // _DEBUG

        //
        // The second unfortunate reality is that once ExecuteUmsThread fails for a particular UMS context, it will *NEVER* come back on
        // the completion list.  From UMS's perspective, the USched is now responsible for polling until the end of time whether this particular context
        // is not suspended.
        //
        SchedulerProxy()->PollForCompletion(pProxy);
        if (!fromSchedulingContext || fCritical)
            HandleBlocking(pProxy, true);
        else
            pProxy->NotifyBlocked(false);
    }

    /// <summary>
    ///     Performs a critical blocking of the primary until a specific UT appears on the completion list.  The specified UT must
    ///     be in a critical region!  This can only be called from the primary thread!
    /// </summary>
    void UMSFreeVirtualProcessorRoot::CriticalBlockAndExecute(UMSFreeThreadProxy *pProxy)
    {
        CONCRT_COREASSERT(OnPrimary());
        CONCRT_COREASSERT(pProxy->GetCriticalRegionType() != OutsideCriticalRegion);

        HANDLE hWaitObjects[2];
        int count = 2;

        //
        // NOTE: It is *ABSOLUTELY* imperative that the critical notification event come FIRST in the wait order!
        // Critically blocked proxies cannot be on the transfer list. So do not wait on transfer list events.
        //
        hWaitObjects[0] = m_hCriticalNotificationEvent;
        hWaitObjects[1] = SchedulerProxy()->GetCompletionListEvent();

        //
        // Continue blocking until we either pull something off the completion list (in which case we simply sleep again) or the
        // critical notification event is signaled (indicating that someone pulled the critical item off the list).  When the critical
        // notification event is signaled, whatever we are critically blocking upon *SHOULD* be runnable (assuming it wasn't asynchronously
        // suspended in the interim or some such silly thing).
        //
        for(;;)
        {
            SchedulerProxy()->SweepCompletionList();

            //
            // Even if we were the one to sweep pProxy off the list, we must still perform the wait -- the event must be reset for the next
            // critical blocking.
            //
            DWORD result = WaitForMultipleObjectsEx(count, hWaitObjects, FALSE, INFINITE, FALSE);

            if (result == WAIT_OBJECT_0)
            {
#if defined(_DEBUG)
                pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_CRITICALAWAKENING;
#endif // _DEBUG

                //
                // For threads we burn, they come back through the critical notification path here and simply do not get executed.  Any return
                // from CriticalBlockAndExecute indicates thread termination.
                //
                if (pProxy->IsTerminated())
                    return;

                Execute(pProxy, false, true);

                //
                // If we get here, it means that ExecuteUmsThread failed (because of asynchronous suspension such as APC).
                // In that case, we really just need to loop up and wait for this object to appear on the completion list again.
                // Note that if we succeed, they just reset the stack frame of the primary and this magically disappears off the stack (e.g.:
                // ExecuteUmsThread does *NOT* return ever on success).
                //
            }

            //
            // In either case -- failure or something appearing on the completion list, we need to sweep the list and wait again until the critical
            // notification event is signaled.
            //

        }
    }

    /// <summary>
    ///     Called in order to handle a UMS thread blocking.
    /// </summary>
    /// <param name="pBlockedProxy">
    ///     The thread that is blocking.
    /// </param>
    /// <param name="fAsynchronous">
    ///     An indication of whether the blocking was due to an asynchronous event (e.g.: page fault) or a synchronous one (e.g.: calling an API
    ///     which explicitly blocked.
    /// </param>
    void UMSFreeVirtualProcessorRoot::HandleBlocking(UMSFreeThreadProxy *pBlockedProxy, bool fAsynchronous)
    {
        RVPMTRACE(MTRACE_EVT_RETURNTOPRIMARY_BLOCKED, pBlockedProxy, this, fAsynchronous);

        CriticalRegionType type = pBlockedProxy->GetCriticalRegionType();

#if defined(_WIN64) && _TRACK_BLOCKING_POINTS
        CONTEXT thCtx;
        thCtx.ContextFlags = (CONTEXT_FULL | CONTEXT_EXCEPTION_REQUEST);
        if (GetThreadContext(pBlockedProxy->GetThreadHandle(), &thCtx))
        {
            RVPMTRACE(MTRACE_EVT_RIP, pBlockedProxy, this, thCtx.Rip);
            RVPMTRACE(MTRACE_EVT_CTXFLAGS, pBlockedProxy, this, thCtx.ContextFlags);
            char *pBytes = (char *)(thCtx.Rip);

            //
            // Verify that if the blocking was reported as synchronous that the previous instruction was a SYSENTER/SYSCALL.
            //
            CONCRT_COREASSERT(fAsynchronous || (*(pBytes - 2) == 0x0f && *(pBytes - 1) == 0x05));
        }
#endif // defined(_WIN64) && _TRACK_BLOCKING_POINTS

        //
        // We control thread lifetimes within the RM.  Any thread which we are burning on the way out must be in a hyper-critical region.
        //
        CONCRT_COREASSERT(!pBlockedProxy->IsTerminated() || type == InsideHyperCriticalRegion);

        for (;;)
        {
            if (type == InsideHyperCriticalRegion || (type == InsideCriticalRegion && fAsynchronous))
            {
                RVPMTRACE(MTRACE_EVT_CRITICALBLOCK, pBlockedProxy, this, fAsynchronous);
#if defined(_DEBUG)
                pBlockedProxy->m_UMSDebugBits |= UMS_DEBUGBIT_CRITICALBLOCK;
#endif // _DEBUG
                pBlockedProxy->NotifyBlocked(true);
                CriticalBlockAndExecute(pBlockedProxy);

                //
                // CriticalBlockAndExecute always performs a UMS::ExecuteUMSThread until it succeeds or the thread terminates.  If it succeeds,
                // the stack is snapped back and we never get here.  Getting here implies that the thread terminated.
                //
                CONCRT_COREASSERT(pBlockedProxy->IsTerminated());
                delete pBlockedProxy;
                return;
            }

            RVPMTRACE(MTRACE_EVT_UMSBLOCKED, pBlockedProxy, this, fAsynchronous);

#if defined(_DEBUG)
            pBlockedProxy->m_UMSDebugBits |= UMS_DEBUGBIT_BLOCKED;
#endif // _DEBUG
            pBlockedProxy->NotifyBlocked(false);

            InvokeSchedulingContext(fAsynchronous);

        }
    }

    /// <summary>
    ///     Called in order to reset this virtual processor root to a completely quiescent state (not running anything).
    /// </summary>
    void UMSFreeVirtualProcessorRoot::ResetOnIdle()
    {
        m_pExecutingProxy = NULL;
        LONG newVal = InterlockedDecrement(&m_activationFence);
        if (newVal <= 0)
        {
            ASSERT(newVal >= -1);

            //
            // The fence going down to zero arbitrates between a possible reset/remove race.
            //
            if (newVal == 0)
                Unsubscribe();

            WaitForSingleObject(m_hBlock, INFINITE);

            //
            // If we awoke because of a deferred deletion, return and let things go the way they will.
            //
            if (m_fDelete)
                return;
        }

        Concurrency::IExecutionContext *pActivatedContext = AcquireActivatedContext();
        ASSERT(newVal == 1 && pActivatedContext != NULL);

        //
        // This means we had a race between an Activate and an Idling (via either SwitchOut or return from dispatch loop).  In either
        // of these cases, we stashed away the context which was activated in m_pActivatedContext.  This context now needs to run atop us.
        //
        UMSFreeThreadProxy *pProxy = static_cast<UMSFreeThreadProxy *> (pActivatedContext->GetProxy());
        ASSERT(pProxy != NULL);

        Execute(pProxy, false, false);
    }

    /// <summary>
    ///     Called in order to handle a UMS thread cooperative yielding.
    /// </summary>
    /// <param name="pProxy">
    ///     The thread that is yielding.
    /// </param>
    void UMSFreeVirtualProcessorRoot::HandleYielding(UMSFreeThreadProxy *pProxy)
    {
        RVPMTRACE(MTRACE_EVT_RETURNTOPRIMARY_YIELD, pProxy, this, NULL);
        CONCRT_COREASSERT(pProxy->m_yieldAction != UMSThreadProxy::ActionNone);

#if defined(_DEBUG)
        pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_YIELDED;
#endif // _DEBUG

        switch(pProxy->m_yieldAction)
        {
            //
            // Someone performed a SwitchTo(..., Nesting).  The UT which performed this call already created a transmogrified primary
            // to transmogrify itself into a "virtual"-thread.  We simply need to message that primary that it's now safe to execute the
            // thread.
            //
            case UMSThreadProxy::ActionTransmogrifyAndSwitch:

                //
                // Intentional Fall Through:
                //

            //
            // Someone performed a SwitchTo.  We do not block the thread (per-kernel-block), the thread simply put data in its
            // messaging block and explicitly yielded to the primary.
            //
            case UMSThreadProxy::ActionSwitchTo:
            {
                UMSFreeThreadProxy *pNextProxy = NULL;

                if (pProxy->m_pNextProxy != NULL)
                {
                    pNextProxy = static_cast<UMSFreeThreadProxy *>(pProxy->m_pNextProxy);

#if defined(_DEBUG)
                    CONCRT_COREASSERT((pNextProxy->m_UMSDebugBits & UMS_DEBUGBIT_DEACTIVATEDPROXY) == 0);
#endif
                }

                //
                // We need to make sure that the transmogrified primary does this when the transmogrification is complete.  Otherwise, someone
                // may wind up trying to run pProxy prematurely on the exit of the nesting.
                //
                if (pProxy->m_yieldAction == UMSThreadProxy::ActionTransmogrifyAndSwitch)
                {
                    //
                    // The transmogrification might not even be created yet!  Notify the transmogrificator.  It will defer unblock it if necessary.
                    //
                    pProxy->NotifyTransmogrification();
                }
                else
                {
                    pProxy->NotifyBlocked(false);
                }

                Execute(pNextProxy, false, false);

                CONCRT_COREASSERT(false);
                break;
            }

            //
            // Someone performed a SwitchOut(Nesting). The UT which performed this call already created a transmogrified primary
            // to transmogrify itself into a "virtual"-thread.  We simply need to message that primary that it's now safe to execute the
            // thread. We also reset the primary so it can be reactivated.
            //
            case UMSThreadProxy::ActionTransmogrifyAndReset:
            {
                //
                // The transmogrification might not even be created yet!  Notify the transmogrificator.  It will defer unblock it if necessary.
                //
                pProxy->NotifyTransmogrification();

                ResetOnIdle();

                //
                // ResetOnIdle will continue the execution loop if the virtual processor was reactivated.  It should only return if
                // the virtual processor is going away.
                //
                CONCRT_COREASSERT(m_fDelete);

                break;
            }
            //
            // Someone performed a SwitchTo(..., idle).  We do not block the thread (per-kernel-block), the thread simply put data
            // in its messaging block and explicitly yielded to the primary.  We do, however, put the thread back on the idle pool.
            //
            case UMSThreadProxy::ActionSwitchToAndRetire:
            {
                UMSFreeThreadProxy *pNextProxy = NULL;

                if (pProxy->m_pNextProxy != NULL)
                {
                    pNextProxy = static_cast<UMSFreeThreadProxy *>(pProxy->m_pNextProxy);

                    RVPMTRACE(MTRACE_EVT_CONTEXT_RELEASED, pProxy, this, pNextProxy);
                }
                else
                {
                    RVPMTRACE(MTRACE_EVT_CONTEXT_RELEASED, pProxy, this, NULL);
                }

        #if defined(_DEBUG)
                pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_FREELIST;
        #endif // _DEBUG

                pProxy->NotifyBlocked(false);
                pProxy->ReturnIdleProxy();

                Execute(pNextProxy, false, false);

                CONCRT_COREASSERT(false);
                break;
            }

            //
            // Someone called YieldToSystem. Yield the time quantum to the operating system using SwitchToThread
            // and re-execute the UT.
            //
            case UMSThreadProxy::ActionYieldToSystem:
            {
                pProxy->NotifyBlocked(false);
                YieldToSystem();
                Execute(pProxy, false, false);
                CONCRT_COREASSERT(false);
                break;
            }

            //
            // Someone performed a Deactivate on the virtual processor root that was running this context.
            //
            case UMSThreadProxy::ActionDeactivate:
            {
                CONCRT_COREASSERT(pProxy->m_pLastRoot == this);

                //
                // The semantics around Deactivate make it perfectly safe to mark it blocked now.  Normally, we cannot touch pProxy
                // afterward, but Deactivate is special.
                //
                pProxy->NotifyBlocked(false);

        #if defined(_DEBUG)
                pProxy->m_UMSDebugBits |= UMS_DEBUGBIT_DEACTIVATEDPROXY;
        #endif // _DEBUG

                if (InternalDeactivate())
                    pProxy->m_activationCause = UMSThreadProxy::ActivationCauseActivate;
                else
                    pProxy->m_activationCause = UMSThreadProxy::ActivationCauseCompletionNotification;

                CONCRT_COREASSERT(pProxy->m_pLastRoot == this);
                Execute(static_cast<UMSFreeThreadProxy *>(m_pExecutingProxy), false, false);
                break;
            }

            //
            // The thread is being freed (because it exited the dispatch loop).  It may or may not be pooled.
            //
            case UMSThreadProxy::ActionFree:              // Return from dispatch loop
            case UMSThreadProxy::ActionResetForSwitchOut: // SwitchOut()
            {
                bool fFreeThread = (pProxy->m_yieldAction == UMSThreadProxy::ActionFree);

                if (fFreeThread)
                {
                    RVPMTRACE(MTRACE_EVT_CONTEXT_RELEASED, pProxy, this, NULL);
                    pProxy->NotifyBlocked(false);
                    pProxy->ReturnIdleProxy();
                }
                else
                {
                    //
                    // If the proxy is switching out blocking, then we need to exit hypercritical
                    // region since it could be resumed on another vproc.
                    //
                    CONCRT_COREASSERT(pProxy->GetCriticalRegionType() == InsideHyperCriticalRegion);
                    pProxy->ExitHyperCriticalRegion();
                    pProxy->NotifyBlocked(false);
                }

                if (!m_fDelete)
                {
                    ResetOnIdle();

                    //
                    // ResetOnIdle will continue the execution loop if the virtual processor was reactivated.  It should only return if
                    // the virtual processor is going away.
                    //
                    CONCRT_COREASSERT(m_fDelete);
                }

                break;
            }

            case UMSThreadProxy::ActionStartup:
            {
                //
                // UT startup (Thread engine should take care of running proxies on startup.
                // Virtual processor root shall not see any proxy prior to startup).
                //
                CONCRT_COREASSERT(false);
                break;
            }

            default:
                CONCRT_COREASSERT(false);
        }
    }

    /// <summary>
    ///     Called in order to invoke the scheduler's scheduling context.
    /// </summary>
    /// <param name="fAsynchronous">
    ///     If invocation of this context is due to previous context blocking, then was it due to an asynchronous event (e.g.: page fault).
    ///     Otherwise, false is passed in.
    /// </param>
    void UMSFreeVirtualProcessorRoot::InvokeSchedulingContext(bool fAsynchronous)
    {
        DispatchState dispatchState;
        dispatchState.m_fIsPreviousContextAsynchronouslyBlocked = fAsynchronous;
        m_pSchedulingContext->Dispatch(&dispatchState);
    }

    /// <summary>
    ///     Returns a process unique identifier for the thread proxy.
    /// </summary>
    /// <returns>
    ///     The IThreadProxy id.
    /// </returns>
    unsigned int UMSFreeVirtualProcessorRoot::GetId() const
    {
        return m_id;
    }

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
    void UMSFreeVirtualProcessorRoot::SwitchTo(IExecutionContext *pContext, SwitchingProxyState switchState)
    {
        //
        // The semantics around the primary context are slightly different.
        //
        CONCRT_COREASSERT(switchState == Blocking);
        if (switchState != Blocking)
        {
           throw invalid_operation();
        }

        UMSFreeThreadProxy * pProxy = static_cast<UMSFreeThreadProxy *> (pContext->GetProxy());
        CONCRT_COREASSERT(pProxy != NULL);

        if (pProxy != NULL)
            Execute(pProxy, true, false);

    }

    /// <summary>
    ///     Called in order to disassociate the currently executing context from its virtual processor root, and reinitialize the root
    ///     for future use.
    /// </summary>
    /// <param name="switchState">
    ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
    ///     proxy and context.
    /// </param>
    void UMSFreeVirtualProcessorRoot::SwitchOut(SwitchingProxyState switchState)
    {
        (switchState);
        //
        // The scheduling context can't SwitchOut.  I don't even understand the semantics around this.
        //
        CONCRT_COREASSERT(false);
        throw invalid_operation();
    }

    /// <summary>
    ///     Called in order to yield to the underlying operating system. This allows the operating system to schedule
    ///     other work in that time quantum.
    /// </summary>
    void UMSFreeVirtualProcessorRoot::YieldToSystem()
    {
        SwitchToThread();
    }

    /// <summary>
    ///     The UMS primary function.  This is invoked when the virtual processor switches into UMS scheduling mode or whenever a given
    ///     context blocks or yields.
    /// </summary>
    /// <param name="reason">
    ///     The reason for the UMS invocation.
    /// </param>
    /// <param name="activationPayload">
    ///     The activation payload (depends on reason)
    /// </param>
    /// <param name="pData">
    ///     The context (the virtual processor pointer)
    /// </param>
    void NTAPI UMSFreeVirtualProcessorRoot::PrimaryInvocation(UMS_SCHEDULER_REASON reason, ULONG_PTR activationPayload, PVOID pData)
    {
        UMSFreeVirtualProcessorRoot *pRoot = NULL;
        UMSFreeThreadProxy *pProxy = NULL;
        PUMS_CONTEXT pPrimaryContext = UMS::GetCurrentUmsThread();
        CONCRT_COREASSERT(pPrimaryContext != NULL);

        if (reason == UmsSchedulerStartup)
        {
            InitialThreadParam * param = reinterpret_cast<InitialThreadParam *>(pData);
            pRoot = param->m_pRoot;

            //
            // Upon startup of the primary, we must stash the *this* pointer somewhere.  We can snap this into a TLS slot or the UMS
            // context.
            //
            UMSBaseObject *pObj = pRoot;
            if (!UMS::SetUmsThreadInformation(pPrimaryContext, UmsThreadUserContext, &pObj, sizeof(pObj)))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

            //
            // Indicate that the Primary is ready to start.
            // The thread parameter lifetime is managed by the caller. Do not touch param (pData) once the
            // event is set below.
            //
            SetEvent(param->m_hEvent);
        }
        else
        {
            //
            // activationPayload and pData might be NULL (blocking), so we're left with storing the UMSFreeVirtualProcessorRoot* in either
            // TLS or the UMS context (the primary does have one).  At present, it's in the UMS context.
            //
            UMSBaseObject *pObj = NULL;
            if (!UMS::QueryUmsThreadInformation(pPrimaryContext, UmsThreadUserContext, &pObj, sizeof(pObj), NULL))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

            pRoot = static_cast<UMSFreeVirtualProcessorRoot *>(pObj);
            pProxy = static_cast<UMSFreeThreadProxy *>(pRoot->m_pExecutingProxy);

            CONCRT_COREASSERT(pProxy->GetVirtualProcessorRoot() == pRoot);
            CONCRT_COREASSERT(pRoot->m_pSchedulingContext != NULL);
        }

        //
        // **************************************************
        // READ THIS RIGHT NOW:
        // **************************************************
        //
        // Anything this function does is highly sensitive.  It's entirely possible that we are the *ONLY* primary within a process
        // and a UT just blocked (pPreviousContext) on some arbitrary object.  If we block on the same object, the UT will never be rescheduled
        // and process deadlock will ensue.  It is therefore IMPERATIVE that everything done in this function be completely lock free and wait
        // free.  Nothing here can block on **ANY** lock that **MIGHT** be held by arbitrary user code running on the scheduler.  This means no
        // memory allocation, no scheduler locks, nothing...
        //
        switch(reason)
        {
            case UmsSchedulerStartup:
            {
                //
                // Wait for the root to be activated.
                //
                WaitForSingleObject(pRoot->m_hBlock, INFINITE);

                if (!pRoot->m_fDelete)
                {
                    //
                    // Activation should have set the scheduling context
                    //
                    CONCRT_COREASSERT(pRoot->m_fActivated);
                    CONCRT_COREASSERT(pRoot->m_pSchedulingContext != NULL);

                    //
                    // Upon first start-up, we immediately invoke the Scheduling context in order to make a scheduling decision.  The factory for creation has made
                    // us a guarantee that threads that come from it are already "ready to run".
                    //
                    pRoot->InvokeSchedulingContext(false);
                }

                break;
            }
            case UmsSchedulerThreadBlocked:
            {
                bool fAsynchronous = (activationPayload & 0x1) == 0;

                //
                // One of two things can have happened here:
                //
                //     - pProxy actually blocked and we will wind up switching into the scheduling context
                //     - pProxy terminated.  In this case, we return from HandleBlocking and fall through.
                //
                pRoot->HandleBlocking(pProxy, fAsynchronous);
                break;
            }
            case UmsSchedulerThreadYield:
            {
                CONCRT_COREASSERT(reinterpret_cast<UMSThreadProxy *>(pData) == pProxy);
                PUMS_CONTEXT pPreviousContext = reinterpret_cast<PUMS_CONTEXT>(activationPayload);
                CONCRT_COREASSERT(UMSFreeThreadProxy::FromUMSContext(pPreviousContext) == pProxy);

                pRoot->HandleYielding(pProxy);
                break;
            }
            default:
                CONCRT_COREASSERT(false);
                break;
        }
        CONCRT_COREASSERT(pRoot->m_fDelete);
    }

    /// <summary>
    ///     The primary thread for this UMS virtual processor.
    /// </summary>
    /// <param name="pContext">
    ///     The UMSFreeVirtualProcessorRoot that the primary manages.
    /// </param>
    DWORD CALLBACK UMSFreeVirtualProcessorRoot::PrimaryMain(LPVOID pContext)
    {
        InitialThreadParam * param = reinterpret_cast<InitialThreadParam *>(pContext);
        UMSFreeVirtualProcessorRoot *pRoot = param->m_pRoot;

        UMS_SCHEDULER_STARTUP_INFO si;

        si.UmsVersion = UMS_VERSION;
        si.CompletionList = pRoot->SchedulerProxy()->GetCompletionList();
        si.SchedulerProc = (PUMS_SCHEDULER_ENTRY_POINT) &PrimaryInvocation;
        si.SchedulerParam = pContext;

        if (!UMS::EnterUmsSchedulingMode(&si))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        //
        // Release our reference count on the scheduler.  This is the only point it is safe.  We require the data structures maintained by the proxy (e.g.:
        // all the completion / transfer lists, etc...) until the primary is actually completed.  Hence, all of this is reference counted and the proxy doesn't
        // die until every last primary is out.
        //
        pRoot->SchedulerProxy()->Release();

        //
        // This is the only point at which it is *SAFE* to delete the virtual processor root.  Any time we reenter the primary, we need it.  The primary thread
        // has to have exited UMS scheduling mode before anything can be done to get rid of structures.
        //
        delete pRoot;

        FreeLibraryAndDestroyThread(0);
        return 0;

    }

    /// <summary>
    ///     Returns our RM.
    /// </summary>
    ResourceManager *UMSFreeVirtualProcessorRoot::GetResourceManager()
    {
        return SchedulerProxy()->GetResourceManager();
    }

} // namespace details
} // namespace Concurrency

#pragma warning (pop)
