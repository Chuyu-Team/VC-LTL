// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerProxy.cpp
//
// RM proxy for a scheduler instance
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
#pragma warning (push)
#pragma warning (disable : 4702)
    /// <summary>
    ///     Constructs a scheduler proxy.
    /// </summary>
    SchedulerProxy::SchedulerProxy(IScheduler * pScheduler, ResourceManager * pResourceManager, const SchedulerPolicy &policy)
        : m_pThreadProxyFactory(NULL)
        , m_pResourceManager(pResourceManager)
        , m_pHillClimbing(NULL)
        , m_queueLength(0)
        , m_currentConcurrency(0)
        , m_numAllocatedCores(0)
        , m_numBorrowedCores(0)
        , m_numFixedCores(0)
        , m_numAssignedThreads(0)
        , m_numExternalThreads(0)
        , m_numExternalThreadCores(0)
    {
        ASSERT(pScheduler != NULL);

        m_pScheduler = pScheduler;

        m_maxConcurrency                = policy.GetPolicyValue(::Concurrency::MaxConcurrency);
        m_minConcurrency                = policy.GetPolicyValue(::Concurrency::MinConcurrency);
        m_targetOversubscriptionFactor  = policy.GetPolicyValue(::Concurrency::TargetOversubscriptionFactor);
        m_contextStackSize              = policy.GetPolicyValue(::Concurrency::ContextStackSize);
        m_contextPriority               = policy.GetPolicyValue(::Concurrency::ContextPriority);
        m_fDoHillClimbing               = policy.GetPolicyValue(::Concurrency::DynamicProgressFeedback) == ::Concurrency::ProgressFeedbackEnabled;

        if (m_contextPriority == INHERIT_THREAD_PRIORITY)
        {
            m_contextPriority = (char) platform::__GetThreadPriority(GetCurrentThread());
        }

        m_id = m_pScheduler->GetId();
        ASSERT(m_id != -1);

        unsigned int coreCount = m_pResourceManager->GetCoreCount();

        m_coreCount = coreCount;

        ASSERT(coreCount > 0 && coreCount <= INT_MAX);
        ASSERT(m_maxConcurrency > 0 && m_maxConcurrency >= m_minConcurrency);

        unsigned int originalTof = m_targetOversubscriptionFactor;

        // Find the mininum target oversubscription factor required to satisfy MaxConcurrency with the cores available.
        unsigned int minTof = (m_maxConcurrency + coreCount - 1)/coreCount;

        if (originalTof < minTof)
        {
            // Adjust target oversubscription factor to ensure that we can satisfy MaxConcurrency with the cores on the system.
            m_targetOversubscriptionFactor  = minTof;
            // The scheduler needs all the cores on the machine to satisfy max threads. Moreover we will need to oversubscribe
            // more than the user indicated.
            m_desiredHardwareThreads = coreCount;
        }
        else
        {
            m_desiredHardwareThreads = (m_maxConcurrency + originalTof - 1)/originalTof;
        }

        // Now adjust target oversubscription factor to ensure that MaxConcurrency virtual processors are evenly distributed
        // over the desired number of hardware threads (i.e each core gets either m_tof vprocs or m_tof - 1 vprocs). Also
        // calculate how many of the assigned cores will get m_tof vprocs.
        if ((m_maxConcurrency % m_desiredHardwareThreads) == 0)
        {
            // This is the common case. We have a simple distribution and every allocated core will get tof vprocs.
            m_targetOversubscriptionFactor = m_maxConcurrency/m_desiredHardwareThreads;
            m_numFullySubscribedCores = m_desiredHardwareThreads;
            m_minimumHardwareThreads = (m_minConcurrency + m_targetOversubscriptionFactor - 1)/m_targetOversubscriptionFactor;
        }
        else
        {
            // We have an uneven distribution; some cores will get tof vprocs and some will get tof - 1.
            ASSERT(m_targetOversubscriptionFactor > 1);

            m_targetOversubscriptionFactor = (m_maxConcurrency + m_desiredHardwareThreads - 1)/m_desiredHardwareThreads;
            m_numFullySubscribedCores = m_desiredHardwareThreads - ((m_desiredHardwareThreads * m_targetOversubscriptionFactor) - m_maxConcurrency);

            // Calculate min hardware threads. We need to make sure that given the way vprocs are distributed to cores
            // (where some cores could get tof vprocs and some could get tof - 1 vprocs), the scheduler proxy will never go below
            // min concurrency if it is left with just the minimum number of cores (and all of those cores happen to have tof -1
            // vprocs assigned to them).
            if (((m_desiredHardwareThreads - m_numFullySubscribedCores) * (m_targetOversubscriptionFactor - 1)) >= m_minConcurrency)
            {
                m_minimumHardwareThreads = (m_minConcurrency + m_targetOversubscriptionFactor - 2)/(m_targetOversubscriptionFactor - 1);
            }
            else
            {
                m_minimumHardwareThreads = (m_desiredHardwareThreads - m_numFullySubscribedCores);

                unsigned int remainingThreads = (m_minConcurrency - (m_minimumHardwareThreads * (m_targetOversubscriptionFactor - 1)));
                ASSERT(remainingThreads < m_minConcurrency);
                m_minimumHardwareThreads += (remainingThreads + m_targetOversubscriptionFactor - 1)/m_targetOversubscriptionFactor;
            }
        }

        ASSERT(m_maxConcurrency <= m_targetOversubscriptionFactor * m_desiredHardwareThreads);
        ASSERT(m_numFullySubscribedCores <= m_desiredHardwareThreads);
        ASSERT(m_targetOversubscriptionFactor > 1 || m_numFullySubscribedCores == m_desiredHardwareThreads);

        ASSERT(m_targetOversubscriptionFactor > 0 && m_targetOversubscriptionFactor <= INT_MAX);
        ASSERT(m_desiredHardwareThreads > 0 && m_desiredHardwareThreads <= coreCount);
        ASSERT(m_desiredHardwareThreads > 0 && m_minimumHardwareThreads <= m_desiredHardwareThreads);

        // Hold a reference to the resource manager.
        int ref = m_pResourceManager->Reference();
        (ref);
        CONCRT_COREASSERT(ref > 1);

        if (m_fDoHillClimbing)
        {
            m_pHillClimbing = _concrt_new HillClimbing(m_id, coreCount, this);
        }

        m_nodeCount = GetProcessorNodeCount();
        // The allocated nodes structure is created when the first allocation is made for this scheduler proxy. We need to read global core
        // state during this allocation, and therefore we need to perform it while holding the RM lock.
        m_pAllocatedNodes = NULL;

        m_pSortedNodeOrder = _concrt_new unsigned int[m_nodeCount];
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            m_pSortedNodeOrder[i] = i;
        }

#if defined(CONCRT_TRACING)
        m_drmInitialState = NULL;
#endif
    }
#pragma warning (pop)

    /// <summary>
    ///     Called by a scheduler in order make an initial request for an allocation of virtual processors.  The request
    ///     is driven by policies within the scheduler queried via the IScheduler::GetPolicy method.  If the request
    ///     can be satisfied via the rules of allocation, it is communicated to the scheduler as a call to
    ///     IScheduler::AddVirtualProcessors.
    /// </summary>
    /// <param name="doSubscribeCurrentThread">
    ///     Whether to subscribe the current thread and account for it during resource allocation.
    /// </param>
    /// <returns>
    ///     The IExecutionResource instance representing current thread if doSubscribeCurrentThread was true; NULL otherwise.
    /// </returns>
    IExecutionResource * SchedulerProxy::RequestInitialVirtualProcessors(bool doSubscribeCurrentThread)
    {
        return m_pResourceManager->RequestInitialVirtualProcessors(this, doSubscribeCurrentThread);
    }

    /// <summary>
    ///     Called in order to notify the resource manager that the given scheduler is shutting down.  This
    ///     will cause the resource manager to immediately reclaim all resources granted to the scheduler.
    /// </summary>
    void SchedulerProxy::Shutdown()
    {
        m_pResourceManager->Shutdown(this);
    }

    /// <summary>
    ///     Gets a new thread proxy from the factory.
    /// </summary>
    IThreadProxy * SchedulerProxy::GetNewThreadProxy(IExecutionContext * pContext)
    {
        if (m_pThreadProxyFactory == NULL)
        {
            // Populate the cached pointer from the one in the RM
            m_pThreadProxyFactory = GetResourceManager()->GetThreadProxyFactoryManager()->GetFreeThreadProxyFactory();
        }

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *>(m_pThreadProxyFactory->RequestProxy(ContextStackSize(), ContextPriority()));
        pProxy->AssociateExecutionContext(pContext);

        return pProxy;
    }

    /// <summary>
    ///     Ensures that a context is bound to a thread proxy.  This API should *NOT* be called in the vast majority of circumstances.
    ///     The IThreadProxy::SwitchTo will perform late binding to thread proxies as necessary.  There are, however, circumstances
    ///     where it is necessary to pre-bind a context to ensure that the SwitchTo operation switches to an already bound context.  This
    ///     is the case on a UMS scheduling context as it cannot call allocation APIs.
    /// </summary>
    /// <param name="pContext">
    ///     The context to bind.
    /// </param>
    void SchedulerProxy::BindContext(IExecutionContext * pContext)
    {
        if (pContext == NULL)
        {
            throw std::invalid_argument("pContext");
        }

        // Find out if this context already has a thread proxy, if not we have to request one from the factory.
        if (pContext->GetProxy() == NULL)
        {
            // Find a thread proxy from the pool that corresponds to the stack size and priority we need.
            GetNewThreadProxy(pContext);
        }
    }

    /// <summary>
    ///     Returns an **unstarted** thread proxy attached to pContext, to the thread proxy factory.
    ///     Such a thread proxy **must** be unstarted.
    ///     This API should *NOT* be called in the vast majority of circumstances.
    /// </summary>
    /// <param name="pContext">
    ///     The context to unbind.
    /// </param>
    void SchedulerProxy::UnbindContext(IExecutionContext * pContext)
    {
        if (pContext == NULL)
        {
            throw std::invalid_argument("pContext");
        }

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        ASSERT(pProxy != NULL);
        pProxy->ReturnIdleProxy();
    }

    /// <summary>
    ///     This function retrieves the execution resource associated with this thread, if one exists
    /// </summary>
    /// <returns>
    ///     The ExecutionResource instance representing current thread in the runtime.
    /// </returns>
    ExecutionResource * SchedulerProxy::GetCurrentThreadExecutionResource()
    {
        ExecutionResource * pExecutionResource = NULL;
        DWORD tlsSlot = GetResourceManager()->GetExecutionResourceTls();
        void * tlsPointer = platform::__TlsGetValue(tlsSlot);
        size_t tlsValue = (size_t) tlsPointer;

        if ((tlsPointer != NULL) && ((tlsValue & TlsResourceBitMask) == TlsResourceInResource))
        {
            pExecutionResource = (ExecutionResource *) tlsValue;
        }

        return pExecutionResource;
    }

    /// <summary>
    ///     This function retrieves the execution resource associated with this thread, if one exists,
    ///     and updates the reference count on it for better bookkeeping.
    /// </summary>
    /// <returns>
    ///     The ExecutionResource instance representing current thread in the runtime.
    /// </returns>
    ExecutionResource * SchedulerProxy::ReferenceCurrentThreadExecutionResource()
    {
        ExecutionResource * pExecutionResource = NULL;
        DWORD tlsSlot = GetResourceManager()->GetExecutionResourceTls();
        void * tlsPointer = platform::__TlsGetValue(tlsSlot);

        if (tlsPointer != NULL)
        {
            size_t tlsValue = (size_t) tlsPointer;

            if ((tlsValue & TlsResourceBitMask) == TlsResourceInResource)
            {
                // The current thread was previously subscribed with the RM.
                pExecutionResource = (ExecutionResource *) tlsValue;

                VirtualProcessorRoot * pVPRoot = pExecutionResource->GetVirtualProcessorRoot();

                // If this is a nested subscribe call then if there was a virtual processor root,
                // it could not have been removed, because it would have been marked as "fixed".
                ASSERT(pVPRoot == NULL || !pVPRoot->IsRootRemoved());
                pExecutionResource->IncrementUseCounts();
            }
            else if ((tlsValue & TlsResourceBitMask) == TlsResourceInProxy)
            {
                // The current thread is a thread proxy.
                FreeThreadProxy * pThreadProxy = (FreeThreadProxy *) (((size_t) tlsValue) & ~TlsResourceInProxy);
                pExecutionResource = pThreadProxy->GetVirtualProcessorRoot()->GetExecutionResource();
                VirtualProcessorRoot * pVPRoot = pExecutionResource->GetVirtualProcessorRoot();
                if (pVPRoot != NULL && pVPRoot->IsRootRemoved())
                {
                    // The virtual processor root that this thread is running on has been removed. We have to
                    // create a new execution resource abstraction for the current thread and perform an external
                    // thread allocation for this scheduler proxy.
                    pExecutionResource = NULL;
                }
                else
                {
                    pExecutionResource->IncrementUseCounts();
                }
            }
#ifndef _UMS_DISABLED
            else
            {
                // The current thread is a UMS thread proxy.
                ASSERT((tlsValue & TlsResourceBitMask) == TlsResourceInUMSProxy);
                UMSFreeThreadProxy * pThreadProxy = (UMSFreeThreadProxy *) (((size_t) tlsValue) & ~TlsResourceInUMSProxy);

                // For a UMS thread proxy we need to be in a critical region while accessing the virtual processor root, and
                // until we increment a count on the execution resource, making the underlying vproc 'fixed', if it is not
                // already so.
                pThreadProxy->EnterCriticalRegion();
                pExecutionResource = pThreadProxy->GetVirtualProcessorRoot()->GetExecutionResource();

                VirtualProcessorRoot * pVPRoot = pExecutionResource->GetVirtualProcessorRoot();
                if (pVPRoot != NULL && pVPRoot->IsRootRemoved())
                {
                    // The virtual processor root that this thread is running on has been removed. We have to
                    // create a new execution resource abstraction for the current thread and perform an external
                    // thread allocation for this scheduler proxy.
                    pExecutionResource = NULL;
                }
                else
                {
                    pExecutionResource->IncrementUseCounts();
                }
                pThreadProxy->ExitCriticalRegion();
            }
#endif // _UMS_DISABLED
        }

        if (pExecutionResource != NULL)
        {
            return GetResourceForNewSubscription(pExecutionResource);
        }

        return pExecutionResource;
    }

    /// <summary>
    ///     Creates or reuses an execution resource for the thread subscription
    /// </summary>
    ExecutionResource * SchedulerProxy::GetResourceForNewSubscription(ExecutionResource * pParentExecutionResource)
    {
        ExecutionResource * pExecutionResource = NULL;

        if (pParentExecutionResource->GetSchedulerProxy() != this)
        {
            pExecutionResource = _concrt_new ExecutionResource(this, pParentExecutionResource);
            pExecutionResource->IncrementUseCounts();
        }
        else
        {
            pExecutionResource = pParentExecutionResource;
        }

        return pExecutionResource;
    }

    /// <summary>
    ///     Registers that a call to SubscribeCurrentThread has occured for this core, making this core immovable.
    /// </summary>
    void SchedulerProxy::IncrementFixedCoreCount(unsigned int nodeId, unsigned int coreIndex, bool isExternalThread)
    {
        SchedulerCore * pCore = &m_pAllocatedNodes[nodeId].m_pCores[coreIndex];
        if (pCore->m_numFixedThreads++ == 0)
        {
            SchedulerNode * pNode = &m_pAllocatedNodes[nodeId];
            pNode->m_numFixedCores++;
            m_numFixedCores++;
            if (pCore->IsBorrowed())
            {
                // When a core becomes fixed, we temporarily remove the borrowed flag on it, and restore it when it
                // becomes movable again.
                pCore->m_fPreviouslyBorrowed = true;
                ToggleBorrowedState(pNode, coreIndex);
            }

            // If this core has no virtual processors on it, count it as a core exclusively dedicated to external threads.
            if (isExternalThread &&  m_pAllocatedNodes[nodeId].m_pCores[coreIndex].m_numAssignedThreads == 0)
            {
                ++m_numExternalThreadCores;
            }
        }

        // Increment the external thread count on the core, which helps account for all the resources running on that core.
        if (isExternalThread)
        {
            m_numExternalThreads++;
            pCore->m_numExternalThreads++;
        }
    }

    /// <summary>
    ///     Registers that a call to IExecutionResource::Release has occured, potentially freeing this core.
    /// </summary>
    void SchedulerProxy::DecrementFixedCoreCount(unsigned int nodeId, unsigned int coreIndex, bool isExternalThread)
    {
        SchedulerCore * pCore = &m_pAllocatedNodes[nodeId].m_pCores[coreIndex];
        // Decrement external thread count on the core which helps account for all the resources running on that core.
        if (isExternalThread)
        {
            ASSERT(pCore->m_numExternalThreads > 0);
            pCore->m_numExternalThreads--;
            m_numExternalThreads--;
        }

        ASSERT(pCore->m_numFixedThreads > 0);
        if (--pCore->m_numFixedThreads == 0)
        {
            SchedulerNode * pNode = &m_pAllocatedNodes[nodeId];
            ASSERT(pCore->m_numExternalThreads == 0);
            m_numFixedCores--;
            pNode->m_numFixedCores--;

            if (pCore->m_fPreviouslyBorrowed)
            {
                // If this was a borrowed core convereted to fixed due to a subscription request, we restore the state
                // back to borrowed, here.
                ASSERT(!pCore->IsBorrowed());
                ToggleBorrowedState(pNode, coreIndex);
                pCore->m_fPreviouslyBorrowed = false;
            }

            // If this core was owned only due to an external thread being on it, then there is
            // no more reason for it to be marked as such.
            if (isExternalThread && m_pAllocatedNodes[nodeId].m_pCores[coreIndex].m_numAssignedThreads == 0)
            {
                m_numExternalThreadCores--;
            }
        }
    }

    /// <summary>
    ///     This API registers the current thread with the resource manager associating it with this scheduler proxy,
    ///     and returns an instance of IExecutionResource back to the scheduler for bookkeeping and maintenance.
    /// </summary>
    /// <returns>
    ///     The IExecutionResource instance representing current thread in the runtime.
    /// </returns>
    IExecutionResource * SchedulerProxy::SubscribeCurrentThread()
    {
        return m_pResourceManager->SubscribeCurrentThread(this);
    }

    /// <summary>
    ///     Creates a new execution resource for the external thread and registers it with the scheduler proxy.
    /// </summary>
    ExecutionResource * SchedulerProxy::CreateExternalThreadResource(SchedulerNode * pNode, unsigned int coreIndex)
    {
        ExecutionResource * pExecutionResource = _concrt_new ExecutionResource(this, pNode, coreIndex);
        pExecutionResource->IncrementUseCounts();
        return pExecutionResource;
    }

    /// <summary>
    ///     Adds the execution resource to the list of subscribed threads
    /// </summary>
    void SchedulerProxy::AddThreadSubscription(ExecutionResource * pExecutionResource)
    {
        m_threadSubscriptions.AddTail(pExecutionResource);
    }

    /// <summary>
    ///     Removes the execution resource from the list of subscribed threads
    /// </summary>
    void SchedulerProxy::RemoveThreadSubscription(ExecutionResource * pExecutionResource)
    {
        m_threadSubscriptions.Remove(pExecutionResource);
        delete pExecutionResource;
    }

    /// <summary>
    ///     Called by the RM when it is done reserving cores for the scheduler proxy. The scheduler proxy allocates virtual processors
    ///     or standalone execution resources based on the cores that were reserved for it.
    /// </summary>
    ExecutionResource * SchedulerProxy::GrantAllocation(unsigned int numberReserved, bool fInitialAllocation, bool fSubscribeCurrentThread)
    {
        ASSERT(m_numAllocatedCores == 0 || !fInitialAllocation);
        ASSERT(m_numExternalThreads == 0 || !fInitialAllocation);

        // The scheduler proxy's allocated node map contains 'numberReserved' cores that the RM has reserved in order to
        // satisfy the proxy's request based on its request and the availability of resources. The cores are marked with
        // ProcessorCore::Reserved, and will be converted to ProcessorCore::Allocated here.

        // Note that 'numberReserved' could have the value 0, if this is an allocation for an external thread. In this case, depending
        // on whether the scheduler has more than its minimum, we will either oversubscribe a core, or remove virtual processors
        // assigned to a core in order to accomodate the external thread.
        unsigned int reservationsAllocated = 0;
        ExecutionResource * pExecutionResource = NULL;

        ASSERT(!fInitialAllocation || m_minimumHardwareThreads == MinHWThreads());
        // Calculate the number of virtual processors we will give this scheduler based on the core allocation
        // we received. Each core will be allocated either m_tof vprocs or m_tof - 1 vprocs, based on the
        // desired hardware threads and the value for max concurrency.

        // The current thread subscription we are about to make does not contribute to MinHWThreads() at present. The external thread
        // gets an exclusive core, if the remaining cores, allocated and reserved, can satisfy at least 1 more then the current minimum.
        // Note that 'externalThreadCore' can be 1 even if no cores were reserved -> in this case we will have to remove vprocs from an allocated
        // core and use it exclusively for the external thread.
        unsigned int externalThreadCores = fSubscribeCurrentThread ? (m_numAllocatedCores + numberReserved > MinHWThreads() ? 1 : 0) : 0;
        unsigned int vprocCores = (numberReserved > externalThreadCores) ? (numberReserved - externalThreadCores) : 0;
        bool fRemoveVProcs = (externalThreadCores > 0 && numberReserved == 0);
        bool fShareExternalThreadCore = (fSubscribeCurrentThread && externalThreadCores == 0);

        // These variables are used if a thread subscription is part of this allocation. For a thread subscription assignment to a core,
        // there are 3 possibilities:
        //     1. A Reserved core exists in the current allocation map exclusive for the thread subscription.
        //     2. The thread subscription will share a core with virtual processors.
        //     3. An existing allocated core will be assigned to the external thread after removing all vprocs that are currently allocated to it.
        unsigned int externalThreadUseCount = (unsigned int) -1;
        unsigned int externalThreadCoreIndex = (unsigned int) -1;
        SchedulerNode * pExternalThreadNode = NULL;
        unsigned int currentNodeIndex = fSubscribeCurrentThread ? m_pResourceManager->GetCurrentNodeAndCore(NULL) : (unsigned int) -1;

        ASSERT(!fRemoveVProcs || (m_numAllocatedCores > MinHWThreads()));

        unsigned int vprocCount = 0;
        if (vprocCores > 0)
        {
            ASSERT(m_numFullySubscribedCores > 0 && m_numFullySubscribedCores <= m_desiredHardwareThreads);
            if (vprocCores <= m_numFullySubscribedCores)
            {
                vprocCount = vprocCores * m_targetOversubscriptionFactor;
            }
            else
            {
                vprocCount = (m_numFullySubscribedCores * m_targetOversubscriptionFactor) +
                                ((vprocCores - m_numFullySubscribedCores) * (m_targetOversubscriptionFactor - 1));
            }
        }

        ASSERT(!fInitialAllocation || (vprocCount >= m_minConcurrency && vprocCount <= m_maxConcurrency));

        IVirtualProcessorRoot** vprocArray = (vprocCount > 0) ? _concrt_new IVirtualProcessorRoot *[vprocCount] : NULL;
        unsigned int vprocIndex = 0;
        bool externalThreadCoreFound= !fSubscribeCurrentThread;

        // We may not have a core reserved for the external thread, so we should loop until the external thread is assigned to an
        // existing core, if thread subscription is requested.
        for (unsigned int nodeIndex = 0; (reservationsAllocated < numberReserved || !externalThreadCoreFound) &&
                                            nodeIndex < m_nodeCount; ++nodeIndex)
        {
            // If the core is marked Reserved, we will either assign to it virtual processors, the external thread or both. Whether or not
            // the external thread shares the core with virtual processors depends on the value of fShareExternalThreadCore.

            // If we find any cores marked Allocated, it implies that this is not the initial allocation, and all we're looking to do here
            // is assign a core to the external thread. The external thread could either share a core with vprocs or displace vprocs, depending
            // on the value of fRemoveVProcs.

            SchedulerNode * pNode = &m_pAllocatedNodes[nodeIndex];
            if (pNode->m_reservedCores > 0 || pNode->m_allocatedCores > 0)
            {
                for(unsigned int coreIndex = 0; (reservationsAllocated < numberReserved || !externalThreadCoreFound) &&
                                                    coreIndex < pNode->m_coreCount; ++coreIndex)
                {
                    SchedulerCore * pCore = &pNode->m_pCores[coreIndex];
                    if (pCore->m_coreState == ProcessorCore::Reserved)
                    {
                        bool assignExternalThread = (!externalThreadCoreFound && (reservationsAllocated == numberReserved - 1 || currentNodeIndex == nodeIndex));
                        bool assignVProcs = (!assignExternalThread || externalThreadCores == 0);

                        ASSERT(pCore->m_numAssignedThreads == 0 && pCore->m_numFixedThreads == 0);
                        pCore->m_coreState = ProcessorCore::Allocated;
                        ASSERT(pNode->m_allocatedCores < pNode->m_coreCount);
                        ++pNode->m_allocatedCores;
                        ++m_numAllocatedCores;

                        // If the external thread also needs a core, first try to put it in a node whose affinity is a superset of the hardware thread
                        // it is currently running on. If not, reaffinitize it.
                        if (assignExternalThread)
                        {
                            // The execution resource is created right before returning from the function.
                            pExternalThreadNode = pNode;
                            externalThreadCoreIndex = coreIndex;
                            externalThreadCoreFound = true;
                        }

                        if (assignVProcs)
                        {
                            ASSERT(!assignExternalThread || fShareExternalThreadCore);
                            // Create virtual processor roots in the scheduler proxy, corresponding to the node and core we're currently looking at.
                            unsigned int numVprocs = 0;
                            if (m_numFullySubscribedCores > 0)
                            {
                                numVprocs = m_targetOversubscriptionFactor;
                                // As we assign m_tof threads to a core, we decrement this value. This value is also updated in
                                // AddCore and RemoveCore. After the scheduler proxy has been given its initial allocation
                                // or resources, this variable keeps track of how many out of the remaining quota of cores the
                                // scheduler proxy could acquire (desired - allocated) would get tof threads per core if they
                                // were added to the scheduler during dynamic core migration.
                                --m_numFullySubscribedCores;
                            }
                            else
                            {
                                numVprocs = m_targetOversubscriptionFactor - 1;
                            }
                            pCore->m_numAssignedThreads += numVprocs;
                            m_numAssignedThreads += numVprocs;

                            while (numVprocs-- > 0)
                            {
                                vprocArray[vprocIndex++] = CreateVirtualProcessorRoot(pNode, coreIndex);
                            }
                            ASSERT(vprocIndex <= vprocCount);
                        }
                        ++reservationsAllocated;
                    }
                    else if (pCore->m_coreState == ProcessorCore::Allocated)
                    {
                        // If we encounter allocated cores, this is a subsequent allocation for an external core. Determine if the external
                        // thread should share a core with existing vprocs and external threads, or displace some vprocs to get a core to itself.

                        // Walk through all the allocated cores to find the right one to either oversubscribe or displace. For over
                        // subscription find the core with the least number of vprocs + external thread assigned to it (favouring the node
                        // where the current thread is running if there is more than one such core).
                        // For displacement, we need to find an unfixed core, favouring the node where the current thread is running.
                        if (fShareExternalThreadCore)
                        {
                            ASSERT(!fRemoveVProcs && externalThreadCores == 0);

                            unsigned int useCount = pCore->m_numAssignedThreads + pCore->m_numExternalThreads;
                            if (useCount < externalThreadUseCount || (useCount == externalThreadUseCount && nodeIndex == currentNodeIndex))
                            {
                                externalThreadUseCount = useCount;
                                pExternalThreadNode = pNode;
                                externalThreadCoreIndex = coreIndex;

                                // We don't set externalThreadCoreFound here, since we want to examine all allocated cores.
                            }
                        }
                        else if (fRemoveVProcs)
                        {
                            ASSERT(externalThreadCores == 1);
                            if (!pCore->IsFixed() && (pExternalThreadNode == NULL || nodeIndex == currentNodeIndex))
                            {
                                pExternalThreadNode = pNode;
                                externalThreadCoreIndex = coreIndex;
                                if (nodeIndex == currentNodeIndex)
                                {
                                    // Stop looking if we find an unfixed core on the current node.
                                    externalThreadCoreFound = true;
                                }
                            }
                        }
                    }
                    else
                    {
                        ASSERT(pCore->m_coreState == ProcessorCore::Unassigned);
                    }
                }
                pNode->m_reservedCores = 0;
            }
        }

        ASSERT(vprocIndex == vprocCount);
        if (vprocCount > 0)
        {
            AddVirtualProcessorRoots(vprocArray, vprocCount);
            delete [] vprocArray;
        }

        if (fSubscribeCurrentThread)
        {
            ASSERT(pExternalThreadNode != NULL && externalThreadCoreIndex != (unsigned int) -1);
            if (fShareExternalThreadCore)
            {
                ASSERT(externalThreadCores == 0);
            }
            else if (fRemoveVProcs)
            {
                ASSERT(externalThreadCores == 1);
                ASSERT(m_numAllocatedCores > MinHWThreads());
                // Remove the core and replace with an external thread subscription. Note that the use count for this core
                // stays the same, as we simply replace virtual processors with a thread subscription.
                RemoveCore(pExternalThreadNode, externalThreadCoreIndex);

                pExternalThreadNode->m_pCores[externalThreadCoreIndex].m_coreState = ProcessorCore::Allocated;
                ASSERT(pExternalThreadNode->m_allocatedCores < pExternalThreadNode->m_coreCount);
                ++pExternalThreadNode->m_allocatedCores;
                ++m_numAllocatedCores;
            }
            else
            {
                ASSERT(externalThreadCores == 1);
            }
            pExecutionResource = CreateExternalThreadResource(pExternalThreadNode, externalThreadCoreIndex);
        }
#if defined(CONCRT_TRACING)
        m_numTotalCores = m_nodeCount * m_pAllocatedNodes[0].m_coreCount;
        m_drmInitialState = _concrt_new SchedulerCoreData[m_numTotalCores];
        memset(m_drmInitialState, 0, sizeof(SchedulerCoreData) * m_numTotalCores);
#endif
        ASSERT(m_numAllocatedCores >= MinHWThreads() && m_numAllocatedCores <= DesiredHWThreads());
        return pExecutionResource;
    }

    /// <summary>
    ///     Causes the resource manager to create a new virtual processor root running atop the same hardware thread as this
    ///     execution resource. Typically, this is used when a scheduler wishes to oversubscribe a particular hardware thread
    ///     for a limited amount of time.
    /// </summary>
    /// <param name="pExecutionResource">
    ///     The execution resource abstraction on which to oversubscribe.
    /// </param>
    /// <returns>
    ///     A new virtual processor root running atop the same hardware thread as this execution resource.
    /// </returns>
    IVirtualProcessorRoot * SchedulerProxy::CreateOversubscriber(IExecutionResource * pExecutionResource)
    {
        // The scheduler proxy on the virtual processor root has to match 'this'
        VirtualProcessorRoot * pOversubscribedRoot = NULL;
        ExecutionResource * pResource = dynamic_cast<ExecutionResource *>(pExecutionResource);
        bool isVprocRoot = false;

        // If dynamic cast failed then we must have a virtual processor root.
        if (pResource == NULL)
        {
            pResource = static_cast<VirtualProcessorRoot *>(pExecutionResource)->GetExecutionResource();
            isVprocRoot = true;
        }

        // Cannot verify the scheduler proxy for external threads because they can "live" on
        // multiple schedulers at the same time (nested).
        if (isVprocRoot && pResource->GetSchedulerProxy() != this)
        {
            throw std::invalid_argument("pExecutionResource");
        }

        // Synchronize with other concurrent calls that are adding/removing virtual processor roots.
        {
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);
            // Use the scheduler proxy to clone this virtual processor root.
            SchedulerNode * pNode = &m_pAllocatedNodes[pResource->GetNodeId()];
            unsigned int coreIndex = pResource->GetCoreIndex();

            pOversubscribedRoot = CreateVirtualProcessorRoot(pNode, coreIndex);

            // We mark these vproc roots as oversubscribed to indicate that they do not contribute
            // towards concurrency levels bounded by the policy
            pOversubscribedRoot->MarkAsOversubscribed();
            pNode->m_pCores[coreIndex].m_resources.AddTail(pOversubscribedRoot->GetExecutionResource());
        }

        return pOversubscribedRoot;
    }

    /// <summary>
    ///     Creates a virtual processor root and adds it to the scheduler proxys list of roots.
    /// </summary>
    VirtualProcessorRoot * SchedulerProxy::CreateVirtualProcessorRoot(SchedulerNode * pNode, unsigned int coreIndex)
    {
        return _concrt_new FreeVirtualProcessorRoot(this, pNode, coreIndex);
    }

    /// <summary>
    ///     Notifies the scheduler associated with this proxy to adds the virtual processor roots provided.
    ///     Called by the RM during initial allocation and dynamic core migration.
    /// </summary>
    void SchedulerProxy::AddVirtualProcessorRoots(IVirtualProcessorRoot ** vprocRoots, unsigned int count)
    {
        // Note, that we are holding the global RM allocation lock when this API is called.
        {
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);
            for (unsigned int i = 0; i < count; ++i)
            {
                VirtualProcessorRoot * pRoot = static_cast<VirtualProcessorRoot *>(vprocRoots[i]);
                // Add the resources associated with the roots to the corresponding lists in the scheduler proxy.
                unsigned int nodeId = pRoot->GetNodeId();
                unsigned int coreIndex = pRoot->GetCoreIndex();

                m_pAllocatedNodes[nodeId].m_pCores[coreIndex].m_resources.AddTail(pRoot->GetExecutionResource());
            }
            m_pScheduler->AddVirtualProcessors((IVirtualProcessorRoot **) vprocRoots, count);

            m_currentConcurrency += count;
        }
    }

    /// <summary>
    ///     Adds an execution resource to the list of resources that run on a particular core.
    /// </summary>
    void SchedulerProxy::AddExecutionResource(ExecutionResource * pExecutionResource)
    {
        {
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);

            // Add the resource to the corresponding list in the scheduler proxy.
            unsigned int nodeId = pExecutionResource->GetNodeId();
            unsigned int coreIndex = pExecutionResource->GetCoreIndex();

            m_pAllocatedNodes[nodeId].m_pCores[coreIndex].m_resources.AddTail(pExecutionResource);
        }
    }

    /// <summary>
    ///     Toggles the state on a core from borrowed to owned (and vice versa), and updates necessary counts.
    /// </summary>
    void SchedulerProxy::ToggleBorrowedState(SchedulerNode * pNode, unsigned int coreIndex)
    {
        SchedulerCore * pCore = &pNode->m_pCores[coreIndex];

        if (pCore->m_fBorrowed)
        {
            --m_numBorrowedCores;
            --pNode->m_numBorrowedCores;
            pCore->m_fBorrowed = false;
        }
        else
        {
            ++m_numBorrowedCores;
            ++pNode->m_numBorrowedCores;
            pCore->m_fBorrowed = true;
        }
    }

    /// <summary>
    ///     Adds an appropriate number of virtual processor roots to the scheduler associated with this proxy.
    ///     Called by the RM during core migration when the RM decides to give this scheduler an additional
    ///     core.
    /// </summary>
    void SchedulerProxy::AddCore(SchedulerNode * pNode, unsigned int coreIndex, bool fBorrowed)
    {
        // Note, that we are holding the global RM allocation lock when this API is called.

        // Decide how many virtual processors to give the scheduler on this core. Note that this value is required
        // to be either m_tof or m_tof - 1.

        unsigned int numThreads = 0;
        if (m_numFullySubscribedCores > 0)
        {
            numThreads = m_targetOversubscriptionFactor;
            --m_numFullySubscribedCores;
        }
        else
        {
            numThreads = m_targetOversubscriptionFactor - 1;
        }

        ASSERT(numThreads > 0 && numThreads <= INT_MAX);

        ASSERT(pNode->m_allocatedCores < pNode->m_coreCount);
        ++pNode->m_allocatedCores;
        ASSERT(m_numAllocatedCores < DesiredHWThreads());
        ++m_numAllocatedCores;

        SchedulerCore * pCore = &pNode->m_pCores[coreIndex];

        ASSERT(pCore->m_coreState == ProcessorCore::Unassigned);
        pCore->m_coreState = ProcessorCore::Allocated;

        ASSERT(pCore->m_numAssignedThreads == 0);
        pCore->m_numAssignedThreads = numThreads;
        m_numAssignedThreads += pCore->m_numAssignedThreads;
        ASSERT(m_numAssignedThreads <= m_maxConcurrency);

        if (fBorrowed)
        {
            ASSERT(!pCore->IsBorrowed());
            ToggleBorrowedState(pNode, coreIndex);
        }

        // Special case for when there is 1 vproc per core - this is likely to be the common case.
        IVirtualProcessorRoot * pRoot;
        IVirtualProcessorRoot ** pRootArray = (numThreads == 1) ? &pRoot : _concrt_new IVirtualProcessorRoot *[numThreads];

        for (unsigned int i = 0; i < numThreads; ++i)
        {
            pRootArray[i] = CreateVirtualProcessorRoot(pNode, coreIndex);
        }

        AddVirtualProcessorRoots(pRootArray, numThreads);

        if (pRootArray != &pRoot)
        {
            delete [] pRootArray;
        }
    }

    /// <summary>
    ///     Notifies the scheduler associated with this proxy to remove the virtual processor roots associated
    ///     with the core provided. Called by the RM during core migration.
    /// </summary>
    void SchedulerProxy::RemoveCore(SchedulerNode * pNode, unsigned int coreIndex)
    {
        // Note, that we are holding the global RM allocation lock when this API is called.
        ASSERT(pNode->m_allocatedCores > 0 && pNode->m_allocatedCores <= pNode->m_coreCount);
        --pNode->m_allocatedCores;
        ASSERT(m_numAllocatedCores > MinVprocHWThreads());
        --m_numAllocatedCores;

        SchedulerCore * pCore = &pNode->m_pCores[coreIndex];

        ASSERT(pCore->m_coreState == ProcessorCore::Allocated || pCore->m_coreState == ProcessorCore::Stolen);
        pCore->m_coreState = ProcessorCore::Unassigned;

        ASSERT(pCore->m_numAssignedThreads == m_targetOversubscriptionFactor ||
                    pCore->m_numAssignedThreads == m_targetOversubscriptionFactor - 1);
        if (pCore->m_numAssignedThreads == m_targetOversubscriptionFactor)
        {
            ++m_numFullySubscribedCores;
        }

        m_numAssignedThreads -= pCore->m_numAssignedThreads;
        ASSERT(m_numAssignedThreads >= m_minConcurrency && m_numAssignedThreads < m_maxConcurrency);
        pCore->m_numAssignedThreads = 0;

        if (pCore->m_fBorrowed)
        {
            ToggleBorrowedState(pNode, coreIndex);
        }

        pCore->m_fIdleDuringDRM = false;

        ASSERT(GetNumOwnedCores() >= MinHWThreads());

        // A lock is required around the iteration of nodes and the call to AddVirtualProcessors to synchronize with concurrent
        // calls to DestroyVirtualProcessorRoot, which removes roots from the array and deletes them.

        { // begin locked region
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);
            ExecutionResource * pExecutionResource = pCore->m_resources.First();
            while (pExecutionResource != NULL)
            {
                // Remember the next root before hand, since a IVirtualProcessorRoot::Remove call could happen inline
                // for the root we're removing, and by the time we get back, that root could be deleted.
                ExecutionResource * pNextExecutionResource = pCore->m_resources.Next(pExecutionResource);
                VirtualProcessorRoot * pVPRoot = pExecutionResource->GetVirtualProcessorRoot();
                if (pVPRoot != NULL && !pVPRoot->IsRootRemoved())
                {
                    pVPRoot->MarkRootRemoved();
                    IVirtualProcessorRoot * pIRoot = pVPRoot;
                    m_pScheduler->RemoveVirtualProcessors(&pIRoot, 1);
                }
                pExecutionResource = pNextExecutionResource;
            }
        } // end locked region
    }

    /// <summary>
    ///     Called by the RM to instruct this scheduler proxy to notify its scheduler that this core is now
    ///     externally busy or externally idle.
    /// </summary>
    void SchedulerProxy::SendCoreNotification(SchedulerCore * pCore, bool isBusyNotification)
    {
        // Avoid a memory allocation under two locks if we have less than 8 roots per core - this is expected to be
        // the common case.
        IVirtualProcessorRoot * pRootArray[8];
        IVirtualProcessorRoot ** pRoots= NULL;

        // Note, that we are holding the global RM allocation lock when this API is called.
        { // begin locked region
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);
            unsigned int numThreadsIndex = 0;

            if (pCore->m_resources.Count() > 8)
            {
                pRoots = _concrt_new IVirtualProcessorRoot * [pCore->m_resources.Count()];
            }
            else
            {
                pRoots = pRootArray;
            }

            ExecutionResource * pExecutionResource = pCore->m_resources.First();
            while (pExecutionResource != NULL)
            {
                ExecutionResource * pNextExecutionResource = pCore->m_resources.Next(pExecutionResource);
                VirtualProcessorRoot * pVPRoot = pExecutionResource->GetVirtualProcessorRoot();
                if (pVPRoot != NULL && !pVPRoot->IsRootRemoved())
                {
                    pRoots[numThreadsIndex++] = pVPRoot;
                }
                pExecutionResource = pNextExecutionResource;
            }
            ASSERT(numThreadsIndex <= (unsigned int) pCore->m_resources.Count());

            // Now that the array is populated, send notifications for this core
            if (isBusyNotification)
            {
                m_pScheduler->NotifyResourcesExternallyBusy(pRoots, numThreadsIndex);
            }
            else
            {
                m_pScheduler->NotifyResourcesExternallyIdle(pRoots, numThreadsIndex);
            }

        } // end locked region

        if (pRoots!= pRootArray)
        {
            delete [] pRoots;
        }
    }
    /// <summary>
    ///     Removes a root from the scheduler proxy and destroys it. This API is called in response to a scheduler
    ///     informing the RM that it is done with a virtual processor root.
    /// </summary>
    void SchedulerProxy::DestroyVirtualProcessorRoot(VirtualProcessorRoot * pRoot)
    {
        // Synchronize with other concurrent calls that are adding/removing virtual processor roots.
        { // begin locked region
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);
            SchedulerNode * pNode = &m_pAllocatedNodes[pRoot->GetNodeId()];
            ASSERT(pNode->m_id == pRoot->GetNodeId());

            // NOTE: This API is called in response to a scheduler being done with a virtual processor root.
            // The scheduler is expected not to invoke ISchedulerProxy::Shutdown, which destroys
            // all remaining roots in the proxy, until all individual calls for removing virtual processor
            // roots have completed.

            pNode->m_pCores[pRoot->GetCoreIndex()].m_resources.Remove(pRoot->GetExecutionResource());

            if (!pRoot->IsOversubscribed())
            {
                // Oversubscibed vprocs do not contribute towards concurrency level
                ASSERT(m_currentConcurrency > 0);
                --m_currentConcurrency;
            }

        } // end locked region

        pRoot->DeleteThis();
    }

    /// <summary>
    ///     Removes an execution resource from the scheduler proxy, and destroys it. This API is called in response to a scheduler
    ///     informing the RM that it is done with an execution resource.
    /// </summary>
    void SchedulerProxy::DestroyExecutionResource(ExecutionResource * pExecutionResource)
    {
        // NOTE: This function should be called with the RM lock held.
        SchedulerNode * pNode = &m_pAllocatedNodes[pExecutionResource->GetNodeId()];
        SchedulerCore * pCore = &pNode->m_pCores[pExecutionResource->GetCoreIndex()];
        ASSERT(pNode->m_id == pExecutionResource->GetNodeId());

        // Mark this core as available to others if this was the last resource on it
        // If this is the last running resource on this core then mark it as available again
        if (pCore->m_numAssignedThreads + pCore->m_numExternalThreads == 0)
        {
            // If there are no vprocs or external threads, then core cannot be fixed
            ASSERT(!pCore->IsFixed());
            ASSERT(pNode->m_allocatedCores > 0 && pNode->m_allocatedCores <= pNode->m_coreCount);
            pNode->m_allocatedCores--;
            ASSERT(m_numAllocatedCores > MinHWThreads());
            pCore->m_coreState = ProcessorCore::Unassigned;
            m_numAllocatedCores--;
            ASSERT(m_numAllocatedCores <= DesiredHWThreads());
            m_pResourceManager->DecrementCoreUseCount(pExecutionResource->GetNodeId(), pExecutionResource->GetCoreIndex());
        }

        // Synchronize with other concurrent calls that are adding/removing execution resources.
        { // begin locked region
            _ReentrantBlockingLock::_Scoped_lock lock(m_lock);
            pCore->m_resources.Remove(pExecutionResource);
        } // end locked region

        delete pExecutionResource;
    }

    /// <summary>
    ///     Called to assist dynamic resourcemanagement in determining whether cores assigned to schedulers
    ///     are idle. An idle core is one whose subscription level is 0.
    /// </summary>
    void SchedulerProxy::IncrementCoreSubscription(ExecutionResource * pExecutionResource)
    {
        unsigned int nodeId = pExecutionResource->GetNodeId();
        unsigned int coreIndex = pExecutionResource->GetCoreIndex();

        if ((InterlockedIncrement(&m_pAllocatedNodes[nodeId].m_pCores[coreIndex].m_subscriptionLevel) == 1) &&
            (m_pResourceManager->GetNumSchedulersForNotifications() > (ShouldReceiveNotifications() ? 1UL : 0UL)))
        {
            // We've incremented the local subscription from 0 to 1 -> this may warrant notifications.
            // Note that the number of schedulers needing notifications may change right after we read it, but any
            // missed notifications will be sent at the next Dynamic RM Poll.

            // We simply set the dynamic RM event here. Note -> there may not yet be a dynamic RM thread at this point.
            // We clearly have 2 schedulers, but it could be that the second one is just being created. In that case,
            // notifications will be sent when the dynamic RM starts up (right after the second scheduler has finished
            // receiving all its resources). We may even race with shutdown for the penultimate scheduler. If the DRM
            // thread wakes up and there is only one scheduler left, it will go back to waiting.
            m_pResourceManager->WakeupDynamicRMWorker();
        }
    }

    /// <summary>
    ///     Called to assist dynamic resourcemanagement in determining whether cores assigned to schedulers
    ///     are idle. An idle core is one whose subscription level is 0.
    /// </summary>
    void SchedulerProxy::DecrementCoreSubscription(ExecutionResource * pExecutionResource)
    {
        unsigned int nodeId = pExecutionResource->GetNodeId();
        unsigned int coreIndex = pExecutionResource->GetCoreIndex();

        if ((InterlockedDecrement(&m_pAllocatedNodes[nodeId].m_pCores[coreIndex].m_subscriptionLevel) == 0) &&
            (m_pResourceManager->GetNumSchedulersForNotifications() > (ShouldReceiveNotifications() ? 1UL : 0UL)))
        {
            // We've decremented the local subscription from 1 to 0 -> this may warrant notifications.
            // Note that the number of schedulers needing notifications may change right after we read it, but any
            // missed notifications will be sent at the next Dynamic RM Poll.

            // We simply set the dynamic RM event here. Note -> there may not yet be a dynamic RM thread at this point.
            // We clearly have 2 schedulers, but it could be that the second one is just being created. In that case,
            // notifications will be sent when the dynamic RM starts up (right after the second scheduler has finished
            // receiving all its resources). We may even race with shutdown for the penultimate scheduler. If the DRM
            // thread wakes up and there is only one scheduler left, it will go back to waiting.
            m_pResourceManager->WakeupDynamicRMWorker();
        }
    }

    /// <summary>
    ///     Called to adjust the suggested allocation such that we do not exceed maxConcurrency.
    ///     This routine takes into account vprocs that are marked for removal but haven't yet been
    ///     retired by the scheduler. The suggested allocation would be decreased to account for such
    ///     vprocs.
    /// </summary>
    unsigned int SchedulerProxy::AdjustAllocationIncrease(unsigned int suggestedAllocation) const
    {
        ASSERT(suggestedAllocation >= GetNumAllocatedCores());
        ASSERT(suggestedAllocation <= DesiredHWThreads());

        // Figure out the max number of new cores we can add
        unsigned int newCores = 0;

        // Since we could be not holding the scheduler proxy lock the value in m_currentConcurrency could
        // be changing. This is fine since a later DRM sweep will migrate appropriate number of cores.
        if (m_maxConcurrency > m_currentConcurrency)
        {
            unsigned int remainingConcurrency = m_maxConcurrency - m_currentConcurrency;

            // Convert remaining concurrency to number of cores
            unsigned int fullySubscribedConcurrency = m_numFullySubscribedCores * m_targetOversubscriptionFactor;

            if (fullySubscribedConcurrency >= remainingConcurrency)
            {
                newCores = remainingConcurrency / m_targetOversubscriptionFactor;
            }
            else
            {
                ASSERT(m_targetOversubscriptionFactor > 1);
                newCores = m_numFullySubscribedCores;
                newCores += ((remainingConcurrency - fullySubscribedConcurrency) / (m_targetOversubscriptionFactor - 1));
            }
        }

        unsigned int maxAllocation = (GetNumAllocatedCores() + newCores);

        // Cores used exclusively by external threads are included in numAllocatedCores. As a result
        // maxAllocation could go above desired.
        maxAllocation = min(maxAllocation, DesiredHWThreads());

#if defined(CONCRT_TRACING)
        if (maxAllocation < suggestedAllocation)
        {
            TRACE(CONCRT_TRACE_DYNAMIC_RM, L"Scheduler %d: Allocated: %d, Suggested: %d, Adjusted Suggested: %d",
                GetId(), GetNumAllocatedCores(), suggestedAllocation, maxAllocation);
        }
#endif

        return min(maxAllocation, suggestedAllocation);
    }

    SchedulerProxy::~SchedulerProxy()
    {
        //
        // Clean up anything which might be used during the asynchronous delete.
        //
        m_pResourceManager->DestroyAllocatedNodeData(m_pAllocatedNodes);
        delete [] m_pSortedNodeOrder;

#if defined(CONCRT_TRACING)
        delete [] m_drmInitialState;
#endif
        //
        // Release the reference on the Resource manager
        //
        m_pResourceManager->Release();
    }

    /// <summary>
    ///     Called to shutdown a scheduler proxy.  Derived classes can override shutdown behavior based on this.
    /// </summary>
    void SchedulerProxy::FinalShutdown()
    {
        Cleanup();
        DeleteThis();
    }

    /// <summary>
    ///     Cleans up resources associated with the scheduler.
    /// </summary>
    void SchedulerProxy::Cleanup()
    {
        //
        // Delete vproc roots that exist in the allocated nodes at this time.  The deletion here is a notification.  It may happen asynchronously
        // depending on the type of scheduler proxy.  The data structures maintained for the scheduler proxy cannot go away until the deferred
        // deletion happens.
        //
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            SchedulerNode * pNode = &m_pAllocatedNodes[i];

            for (unsigned int j = 0; j < pNode->m_coreCount; ++j)
            {
                ExecutionResource * pExecutionResource = pNode->m_pCores[j].m_resources.First();

                while (pExecutionResource != NULL)
                {
                    ExecutionResource * pExecutionResourceToDelete = pExecutionResource;
                    pExecutionResource = pNode->m_pCores[j].m_resources.Next(pExecutionResource);
                    VirtualProcessorRoot * pVPRoot = pExecutionResourceToDelete->GetVirtualProcessorRoot();
                    ASSERT(pVPRoot != NULL);

                    // Since the root is going away, check if it contributes to the subscription count on the core, and
                    // fix up the count, if so.
                    pVPRoot->ResetSubscriptionLevel();
                    pVPRoot->DeleteThis();
                }
            }
        }

        delete m_pHillClimbing;
    }

#if defined(CONCRT_TRACING)

    /// <summary>
    ///     Sets or clears a flag indicating that the RM needs to do an external thread allocation for this
    ///     scheduler proxy.
    /// </summary>
    void SchedulerProxy::TraceInitialDRMState()
    {
        int traceCoreIndex = 0;
        for (unsigned int nodeIndex = 0; nodeIndex < m_nodeCount; ++nodeIndex)
        {
            SchedulerNode * pAllocatedNode = &m_pAllocatedNodes[nodeIndex];
            for (unsigned int coreIndex = 0; coreIndex < pAllocatedNode->m_coreCount; ++coreIndex)
            {
                SchedulerCore * pAllocatedCore = &pAllocatedNode->m_pCores[coreIndex];
                SchedulerCoreData * pCoreData = &m_drmInitialState[traceCoreIndex++];
                pCoreData->m_nodeIndex = (unsigned char)nodeIndex;
                pCoreData->m_coreIndex = (unsigned char)coreIndex;
                pCoreData->m_fAllocated = pAllocatedCore->m_coreState == ProcessorCore::Allocated;
                pCoreData->m_fFixed = pAllocatedCore->IsFixed();
                pCoreData->m_fBorrowed = pAllocatedCore->IsBorrowed();
                pCoreData->m_fIdle = pAllocatedCore->IsIdle();
            }
        }
    }

#endif
} // namespace details
} // namespace Concurrency
