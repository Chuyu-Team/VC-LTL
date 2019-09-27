// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ExternalContextBase.cpp
//
// Source file containing the metaphor for an external execution ContextBase/stack/thread.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

#pragma warning (disable : 4702)

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs an external context.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler the context will belong to.
    /// </param>
    /// <param name="explicitAttach">
    ///     Whether or not this is an explicit attach. An explicit attach occurs as a result of calling a scheduler
    ///     creation API, or the scheduler attach API. The scheduler will not detach implicitly for explicitly
    ///     attached threads, on thread exit.
    /// </param>
    ExternalContextBase::ExternalContextBase(SchedulerBase *pScheduler, bool explicitAttach) :
        ContextBase(pScheduler, true),
        m_pSubAllocator(NULL),
        m_hPhysicalContext(NULL)
    {
        // Create an auto-reset event that is initially not signaled.
        m_hBlock = platform::__CreateAutoResetEvent(); // VSO#459907

        // External contexts are all grouped together in the 'anonymous' schedule group.
        m_pSegment = m_pScheduler->GetAnonymousScheduleGroupSegment();

        // Create external context statistics as a place where this external context we are about to create
        // will store all the statistical data.
        m_pStats = _concrt_new ExternalStatistics(); // VSO#459907
        m_pScheduler->AddExternalStatistics(m_pStats);

        // Initialize data that is reset each time the external context is reused.
        PrepareForUse(explicitAttach);
    }

#if !defined(_ONECORE)
    /// <summary>
    ///     Callback to indicate the exit of one of the external threads. This function
    ///     is invoked on the wait thread. It is assumed that this function is short and quick.
    /// </summary>
    void CALLBACK ExternalContextBase::ImplicitDetachHandler(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WAIT waiter, TP_WAIT_RESULT waitResult)
    {
        ExternalContextBase * pContext = reinterpret_cast<ExternalContextBase *>(parameter);

        ASSERT(waitResult == WAIT_OBJECT_0);

        pContext->m_pScheduler->DetachExternalContext(pContext, false);

        // This is non-blocking
        UnRegisterAsyncWaitAndUnloadLibrary(instance, waiter);
    }
#endif // !defined(_ONECORE)

    /// <summary>
    ///     Same callback function as ImplicitDetachHandler but used on XP.
    /// </summary>
    void CALLBACK ExternalContextBase::ImplicitDetachHandlerXP(PVOID parameter, BOOLEAN is_timeout)
    {
        ExternalContextBase * pContext = reinterpret_cast<ExternalContextBase *>(parameter);

        // This is non-blocking
        platform::__UnregisterWait(pContext->m_hWaitHandle);

        ASSERT(!is_timeout);

        pContext->m_pScheduler->DetachExternalContext(pContext, false);
    }

    /// <summary>
    ///     Initializes fields that need re-initialization when an external context is recycled. This is called
    ///     in the constructor and when an external context is taken off the idle pool for reuse.
    /// </summary>
    /// <param name="explicitAttach">
    ///     Whether or not this is an explicit attach. An explicit attach occurs as a result of calling a scheduler
    ///     creation API, or the scheduler attach API. The scheduler will not detach implicitly for explicitly
    ///     attached threads, on thread exit.
    /// </param>
    void ExternalContextBase::PrepareForUse(bool explicitAttach)
    {
        // Even in the case of a nested external context being initialized, we expect the TLS slot to be clear.
        ASSERT(SchedulerBase::FastCurrentContext() == NULL);

        m_fExplicitlyAttached = explicitAttach;
        m_threadId = GetCurrentThreadId();

        if (!explicitAttach)
        {
            // We only need to capture the current thread's handle for an implicit attach, so that we can register the
            // handle for exit tracking, in order that references may be released on thread exit.
            if (!DuplicateHandle(GetCurrentProcess(),
                             GetCurrentThread(),
                             GetCurrentProcess(),
                             &m_hPhysicalContext,
                             0,
                             FALSE,
                             DUPLICATE_SAME_ACCESS))
            {
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
            }
#if !defined(_ONECORE)
            if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
            {
                // Request a thread pool thread to wait for this thread exit.
                if ((m_hWaitHandle = RegisterAsyncWaitAndLoadLibrary(m_hPhysicalContext, ExternalContextBase::ImplicitDetachHandler, this)) == nullptr)
                {
                    throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
                }
            }
            else
#endif // !defined(_ONECORE)
            {
                m_hWaitHandle = platform::__RegisterWaitForSingleObject(m_hPhysicalContext, ExternalContextBase::ImplicitDetachHandlerXP, this);
            }
        }
    }

    /// <summary>
    ///     Causes the external context to block. Since external contexts do not execute on virtual processors,
    ///     the context does not switch to another one. Instead, it stops executing until it is unblocked.
    /// </summary>
    void ExternalContextBase::Block()
    {
        ASSERT(this == SchedulerBase::FastCurrentContext());

        TraceContextEvent(CONCRT_EVENT_BLOCK, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

        if (InterlockedIncrement(&m_contextSwitchingFence) == 1)
        {
            WaitForSingleObjectEx(m_hBlock, INFINITE, FALSE);
        }
        else
        {
            // Skip the block, since an unblock has already been encountered.
        }
    }

    /// <summary>
    ///     Unblocks the external context causing it to start running.
    /// </summary>
    void ExternalContextBase::Unblock()
    {
        if (this != SchedulerBase::FastCurrentContext())
        {
            TraceContextEvent(CONCRT_EVENT_UNBLOCK, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

            LONG newValue = InterlockedDecrement(&m_contextSwitchingFence);

            if (newValue == 0)
            {
                SetEvent(m_hBlock);
            }
            else
            {
                if ((newValue < -1) || (newValue > 0))
                {
                    // Should not be able to get m_contextSwitchingFence above 0.
                    ASSERT(newValue < -1);

                    throw context_unblock_unbalanced();
                }
            }
        }
        else
        {
            throw context_self_unblock();
        }
    }

    /// <summary>
    ///     Just a thread yield on the current processor.
    /// </summary>
    void ExternalContextBase::Yield()
    {
        TraceContextEvent(CONCRT_EVENT_YIELD, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);

        platform::__SwitchToThread();
    }

    /// <summary>
    ///     See comments for Concurrency::Context::Oversubscribe.
    ///     External contexts do not support oversubscription. However, we keep track of calls and throw exceptions
    ///     when appropriate.
    /// </summary>
    void ExternalContextBase::Oversubscribe(bool beginOversubscription)
    {
        if (beginOversubscription)
        {
            ++m_oversubscribeCount;
        }
        else
        {
            if (m_oversubscribeCount == 0)
            {
                throw invalid_oversubscribe_operation();
            }
            --m_oversubscribeCount;
        }
    }

    /// <summary>
    ///     Allocates a block of memory of the size specified.
    /// </summary>
    /// <param name="numBytes">
    ///     Number of bytes to allocate.
    /// </param>
    /// <returns>
    ///     A pointer to newly allocated memory.
    /// </returns>
    void* ExternalContextBase::Alloc(size_t numBytes)
    {
        void* pAllocation = NULL;
        ASSERT(SchedulerBase::FastCurrentContext() == this);

        // Find the suballocator for this external context if there is one. Note that if we are unable to get an allocator now,
        // we may be able to get one for a later Alloc or Free call (if a different external context released its allocator to
        // the free pool).
        SubAllocator* pAllocator = GetCurrentSubAllocator();

        if (pAllocator != NULL)
        {
            pAllocation = pAllocator->Alloc(numBytes);
        }
        else
        {
            // Allocate from the CRT heap. At the point this allocation is freed, if the context has a suballocator, it will be
            // freed to the suballocator of the context.
            pAllocation = SubAllocator::StaticAlloc(numBytes);
        }

        return pAllocation;
    }

    /// <summary>
    ///     Frees a block of memory previously allocated by the Alloc API.
    /// </summary>
    /// <param name="pAllocation">
    ///     A pointer to an allocation previously allocated by Alloc.
    /// </param>
    void ExternalContextBase::Free(void* pAllocation)
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(pAllocation != NULL);

        // Find the suballocator for this external context if there is one. Note that if we are unable to get an allocator now,
        // we may be able to get one for a later Alloc or Free call (if a different external context released its allocator to
        // the free pool).
        SubAllocator* pAllocator = GetCurrentSubAllocator();

        if (pAllocator != NULL)
        {
            pAllocator->Free(pAllocation);
        }
        else
        {
            // Free to the CRT heap.
            SubAllocator::StaticFree(pAllocation);
        }
    }

    /// <summary>
    ///     Prepares an external context for the idle pool by releasing some resources.
    /// </summary>
    void ExternalContextBase::RemoveFromUse()
    {
        ReleaseWorkQueue();

        CONCRT_COREASSERT(GetCriticalRegionType() == OutsideCriticalRegion);

        if (m_hPhysicalContext != NULL)
        {
            CloseHandle(m_hPhysicalContext);
            m_hPhysicalContext = NULL;
        }
    }

    /// <summary>
    ///     Destroys an external thread based context.
    /// </summary>
    ExternalContextBase::~ExternalContextBase()
    {
        // This takes care of calling the cleanup routine for ContextBase.
        Cleanup();
    }

    /// <summary>
    ///     Performs cleanup of the external context
    /// </summary>
    void ExternalContextBase::Cleanup()
    {
        ContextBase::Cleanup();
        //
        // m_pGroup is an anonymous schedule group that is destroyed at scheduler shutdonw, so don't release here.
        //
        if (m_hPhysicalContext != NULL)
        {
            CloseHandle(m_hPhysicalContext);
            m_hPhysicalContext = NULL;
        }
        if (m_hBlock)
        {
            CloseHandle(m_hBlock);
        }
        if (m_pSubAllocator != NULL)
        {
            SchedulerBase::ReturnSubAllocator(m_pSubAllocator);
        }

        // Mark the scheduler's list of non-internal contexts (external or non-bound to context) for removal. We
        // can't remove this item yet because statistics might not have had a chance to aggregate this information yet.
        DetachStatistics();
    }
} // namespace details
} // namespace Concurrency
