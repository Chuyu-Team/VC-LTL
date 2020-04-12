// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadProxy.cpp
//
// Proxy for an OS context.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a thread proxy.
    /// </summary>
    ThreadProxy::ThreadProxy(IThreadProxyFactory * pFactory, unsigned int stackSize)
        : m_pFactory(pFactory)
        , m_stackSize(stackSize)
        , m_threadPriority(THREAD_PRIORITY_NORMAL)
        , m_fSuspended(false)
        , m_fBlocked(TRUE)
        , m_fCanceled(FALSE)
    {
        // Thread proxy factories for Win32 threads need to be reference counted by the individual thread proxies, unlike
        // UMS based thread proxy factories. This is because thread proxies that were loaned out to threads based schedulers
        // could still be executing their dispatch loop and about to put themselves onto the idle pool on the factory at
        // the time that the schedulers and corresponding scheduler proxies are actually destroyed (and have removed their
        // references on the RM). If no references exist on the RM, the RM goes aheads and destroys the factories. However,
        // it is dangerous to do this while thread proxies are possibly in the process of returning to the factory. Therefore,
        // the outstanding thread proxies (alive but not in the idle pool), need to keep the factory alive until they have all
        // returned.
        //
        // UMS thread proxies on the other hand, need the existence of a UMS virtual processor root in order to execute, and the
        // UMS virtual processor roots are responsible for adding them to the idle pool. It is safe to say that all UMS thread
        // proxies loaned out to a UMS scheduler are back in the idle pool of the factory at the time the UMS scheduler/scheduler
        // proxy (virtual processors roots and all) are destroyed, and the factory can safely be shutdown without worrying about
        // stragglers.
        m_pFactory->Reference();

        m_id = ResourceManager::GetThreadProxyId();

        // Auto-reset event that is not signalled initially
        m_hBlock = platform::__CreateAutoResetEvent(); // VSO#459907

        m_hPhysicalContext = LoadLibraryAndCreateThread(NULL,
                                          m_stackSize*KB,
                                          ThreadProxyMain,
                                          this,
                                          STACK_SIZE_PARAM_IS_A_RESERVATION,
                                          &m_threadId);

        if (m_hPhysicalContext == NULL)
        {
            // Cleanup everything we've allocated because this exception may be caught by a higher
            // layer to provide resiliency against thread creation failures during thread proxy construction.
            CloseHandle(m_hBlock);
            m_pFactory->Release();
            throw scheduler_worker_creation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    /// <summary>
    ///     Destroys a thread proxy.
    /// </summary>
    ThreadProxy::~ThreadProxy()
    {
        CloseHandle(m_hBlock);
        platform::__CloseThreadHandle(m_hPhysicalContext);
        m_pFactory->Release();
    }

    /// <summary>
    ///     Returns a process unique identifier for the thread proxy.
    /// </summary>
    unsigned int ThreadProxy::GetId() const
    {
        return m_id;
    }

#pragma warning (push)
#pragma warning (disable : 4702) // unreachable code
    /// <summary>
    ///     Sets the priority of the underlying thread.
    /// </summary>
    /// <param name="priority">
    ///     The new priority value for the thread.
    /// </param>
    void ThreadProxy::SetPriority(int priority)
    {
        m_threadPriority = priority;

        platform::__SetThreadPriority(m_hPhysicalContext, m_threadPriority);
    }
#pragma warning (pop)

    /// <summary>
    ///     Blocks the thread proxy until is is resumed via ResumeExecution or a different thread proxy switching to it.
    /// </summary>
    void ThreadProxy::SuspendExecution()
    {
        ASSERT(m_fBlocked == FALSE);
        InterlockedExchange(&m_fBlocked, TRUE);

        WaitForSingleObjectEx(m_hBlock, INFINITE, FALSE);

        ASSERT(m_fBlocked == TRUE);
        InterlockedExchange(&m_fBlocked, FALSE);
    }

    /// <summary>
    ///     Resumes execution of a thread proxy.
    /// </summary>
    void ThreadProxy::ResumeExecution()
    {
        SetEvent(m_hBlock);
    }

    /// <summary>
    ///     Cancels the thread proxy causing the underlying thread to exit.
    /// </summary>
    void ThreadProxy::Cancel()
    {
        ASSERT(m_fCanceled == false);
        m_fCanceled = true;
        ResumeExecution();
    }

    /// <summary>
    ///     Spins until the 'this' thread proxy is in a firmly blocked state.
    /// </summary>
    /// <remarks>
    ///     This implements a sort of barrier. At certain points during execution, it is essential to wait until a thread proxy
    ///     has set the flag inidicating it is blocked, in order to preserve correct behavior. One example is if there is a race
    ///     between block and unblock for the same proxy, i.e. if a thread proxy is trying to block at the same time a different
    ///     context is trying to unblock it.
    /// </remarks>
    void ThreadProxy::SpinUntilBlocked()
    {
        if (m_fBlocked == FALSE)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();

            } while (m_fBlocked == FALSE);
        }
        ASSERT(m_fBlocked == TRUE);
    }

    /// <summary>
    ///     Thread start routine for proxies.
    /// </summary>
    DWORD CALLBACK ThreadProxy::ThreadProxyMain(LPVOID lpParameter)
    {
        ThreadProxy* pThreadProxy = reinterpret_cast<ThreadProxy*> (lpParameter);

        // To start the dispatch loop cleanly, the context must block until it is switched to, or resumed..
        WaitForSingleObjectEx(pThreadProxy->m_hBlock, INFINITE, FALSE);
        InterlockedExchange(&pThreadProxy->m_fBlocked, FALSE);

        pThreadProxy->Dispatch();

        ASSERT(pThreadProxy->m_fCanceled);
        // Thread proxy needs to be deleted after it is canceled and it returns from the dispatch loop.
        delete pThreadProxy;
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

} // namespace details
} // namespace Concurrency
