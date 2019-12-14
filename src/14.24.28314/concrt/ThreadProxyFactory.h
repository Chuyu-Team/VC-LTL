// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadProxyFactory.h
//
// Factory for creating thread proxies.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    struct IThreadProxyFactory
    {
        virtual ::Concurrency::IThreadProxy* RequestProxy(unsigned int stackSize, int contextPriority) =0;
        virtual void ReclaimProxy(::Concurrency::IThreadProxy* pThreadProxy) =0;
        virtual LONG Reference() =0;
        virtual LONG Release() =0;
        virtual DWORD GetExecutionResourceTls() =0;
        virtual ~IThreadProxyFactory() {}
    };

    class ThreadProxyFactoryManager;

#pragma warning (push)
#pragma warning (disable : 4702) // unreachable code
    template <typename threadProxy>
    class ThreadProxyFactory : public IThreadProxyFactory
    {
    public:
        /// <summary>
        ///     Returns a thread proxy from a pool of proxies, creating a new one, if needed.
        /// </summary>
        /// <param name = stackSize">
        ///     The required minimum stack size for the thread proxy.
        /// </param>
        /// <param name = contextPriority">
        ///     The required thread priority for the thread proxy.
        /// </param>
        virtual ::Concurrency::IThreadProxy* RequestProxy(unsigned int stackSize, int contextPriority)
        {
            // Based on the requested stack size of the proxy, find the index into the pool array.
            threadProxy * pProxy = NULL;

            for (int i = 0; i < s_numBuckets; ++i)
            {
                if (stackSize <= s_proxyStackSize[i])
                {
                    pProxy = m_proxyPool[i].Pop();
                    if (pProxy != NULL)
                        break;
                }
            }

            if (pProxy == NULL)
            {
                // Either we couldn't find a proxy in one of the pools, or we received a stack size
                // larger than the largest size we pool.
                pProxy = Create(stackSize);
            }

            if (pProxy != NULL)
            {
                Prepare(pProxy, contextPriority);
            }

            return pProxy;
        }

        /// <summary>
        ///     Returns a proxy back to the idle pool for reuse.
        /// </summary>
        /// <param name="pThreadProxy">
        ///     The thread proxy being returned.
        /// </param>
        virtual void ReclaimProxy(::Concurrency::IThreadProxy* pThreadProxy)
        {
            threadProxy * pProxy = static_cast<threadProxy *>(pThreadProxy);
            for (int i = 0; i < s_numBuckets; ++i)
            {
                if (pProxy->GetStackSize() == s_proxyStackSize[i])
                {
                    // If the pool is close to full, cancel the proxy and allow the thread to exit.
                    if (m_proxyPool[i].Count() < s_bucketSize)
                    {
                        m_proxyPool[i].Push(pProxy);
                        pProxy = NULL;
                    }
                    break;
                }
            }

            if (pProxy != NULL)
            {
                Retire(pProxy);
            }
        }

        /// <summary>
        ///     Destroys a thread proxy factory.
        /// </summary>
        virtual ~ThreadProxyFactory()
        {
        }

        /// <summary>
        ///     Retires the proxies that are present in the free pools of a thread proxy factory, causing them to run to
        ///     completion, and exit.
        /// </summary>
        void RetireThreadProxies()
        {
            for (int i = 0; i < s_numBuckets; ++i)
            {
                threadProxy *pProxy = m_proxyPool[i].Flush();
                while (pProxy != NULL)
                {
                    threadProxy *pNextProxy = LockFreeStack<threadProxy>::Next(pProxy);

                    // Retiring the proxy will cause it to perform any necessary cleanup, and exit its dispatch loop.
                    Retire(pProxy);
                    pProxy = pNextProxy;
                }
            }
        }

        /// <summary>
        ///     Initiates shutdown of the factory, and deletes it if shutdown can be completed inline.
        /// </summary>
        virtual void ShutdownFactory() =0;

        /// <summary>
        ///     Returns a TLS index used by thread proxies and subscribed threads to store per-thread data.
        /// </summary>
        virtual DWORD GetExecutionResourceTls()
        {
            return m_executionResourceTlsIndex;
        }

    protected:

        /// <summary>
        ///     Protected constructor. All construction must go through the CreateFactory API.
        /// </summary>
        ThreadProxyFactory(ThreadProxyFactoryManager * pManager);

        /// <summary>
        ///     Initialize static data
        /// </summary>
        static void StaticInitialize()
        {
            if (s_bucketSize == 0)
            {
                s_bucketSize = 4*::Concurrency::GetProcessorCount();
            }
            ASSERT(s_bucketSize >= 4);
        }

    protected:

        /// <summary>
        ///     Creates a new thread proxy.
        /// </summary>
        /// <param name="stackSize">
        ///     The stack size for the thread proxy.
        /// </param>
        virtual threadProxy* Create(unsigned int stackSize) =0;

        /// <summary>
        ///     Retires a thread proxy.
        /// </summary>
        virtual void Retire(threadProxy *pProxy) =0;

        /// <summary>
        ///     Prepares a thread proxy for use.
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy to prepare.
        /// </param>
        /// <param name="contextPriority">
        ///     The required thread priority for the thread proxy.
        /// <param>
        virtual void Prepare(threadProxy *pProxy, int contextPriority)
        {
            //
            // Adjust the thread priority if necessary.
            //
            if (pProxy->GetPriority() != contextPriority)
            {
                pProxy->SetPriority(contextPriority);
            }
        }

        // Each factory supports a small number of pools of specific stack sizes.
        // Currently supported stack sizes are the default process stack size, 64KB, 256KB and 1024KB (1MB)
        static const int s_numBuckets = 4;
        static int s_bucketSize;
        static const unsigned int s_proxyStackSize[s_numBuckets];

        // Cached copy of the execution resource TLS index that was created by the factory manager.
        DWORD m_executionResourceTlsIndex;

        // A list that will hold thread proxies.
        LockFreeStack<threadProxy> m_proxyPool[s_numBuckets];
    };
#pragma warning (pop)

    template <typename threadProxy> int ThreadProxyFactory<threadProxy>::s_bucketSize = 0;
    template <typename threadProxy> const unsigned int ThreadProxyFactory<threadProxy>::s_proxyStackSize[s_numBuckets] = { 0, 64, 256, 1024 };

    /// <summary>
    ///     A factory that creates thread proxies for thread schedulers.
    /// </summary>
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class FreeThreadProxyFactory : public ThreadProxyFactory<FreeThreadProxy>
    {
    public:

        /// <summary>
        ///     Creates a singleton thread proxy factory.
        /// </summary>
        static FreeThreadProxyFactory * CreateFactory(ThreadProxyFactoryManager * pManager)
        {
            StaticInitialize();
            return _concrt_new FreeThreadProxyFactory(pManager);
        }

        /// <summary>
        ///     Destroys a free thread proxy factory.
        /// </summary>
        virtual ~FreeThreadProxyFactory()
        {
        }

        /// <summary>
        ///     Adds a reference to the thread proxy factory.
        /// </summary>
        LONG Reference()
        {
            return InterlockedIncrement(&m_referenceCount);
        }

        /// <summary>
        ///     Releases a reference on the thread proxy factory.
        /// </summary>
        LONG Release()
        {
            LONG refCount = InterlockedDecrement(&m_referenceCount);
            if (refCount == 0)
                delete this;
            return refCount;
        }

        /// <summary>
        ///     Returns a proxy back to the idle pool, for reuse.
        /// </summary>
        /// <param name="pThreadProxy">
        ///     The thread proxy being returned.
        /// </param>
        virtual void ReclaimProxy(::Concurrency::IThreadProxy* pThreadProxy)
        {
            FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *>(pThreadProxy);

            // If the factory has been shut down, we should retire the proxy right away.
            if (!m_fShutdown)
            {
                for (int i = 0; i < s_numBuckets; ++i)
                {
                    if (pProxy->GetStackSize() == s_proxyStackSize[i])
                    {
                        // If the pool is close to full, cancel the proxy and allow the thread to exit.
                        if (m_proxyPool[i].Count() < s_bucketSize)
                        {
                            m_proxyPool[i].Push(pProxy);

                            // After adding the thread proxy to the pool, check if the factory has been shut down.
                            // At shutdown, the flag is set to true before the shutdown routine goes through
                            // and retires all the thread proxies. However, if we've added this proxy after
                            // that point, there is a good chance that the shutdown routine missed us. We
                            // need to make sure the bucket is empty and all proxies are retired, in this case.
                            if (m_fShutdown)
                            {
                                pProxy = m_proxyPool[i].Flush();
                                while (pProxy != NULL)
                                {
                                    FreeThreadProxy *pNextProxy = LockFreeStack<FreeThreadProxy>::Next(pProxy);
                                    // Retiring the proxy will cause it to perform any necessary cleanup, and exit its dispatch loop.
                                    Retire(pProxy);
                                    pProxy = pNextProxy;
                                }
                            }

                            pProxy = NULL;
                        }
                        break;
                    }
                }
            }

            if (pProxy != NULL)
            {
                Retire(pProxy);
            }
        }

        /// <summary>
        ///     Initiates shutdown of the factory, and deletes it if shutdown can be completed inline.
        /// </summary>
        virtual void ShutdownFactory()
        {
            m_fShutdown = true;
            RetireThreadProxies();
            Release();
        }

    protected:

        /// <summary>
        ///     Creates a new Win32 free thread proxy factory.
        ///     Protected constructor. All construction must go through the CreateFactory API.
        /// </summary>
        FreeThreadProxyFactory(ThreadProxyFactoryManager * pManager) :
            ThreadProxyFactory(pManager),
            m_referenceCount(1),
            m_fShutdown(false)
        {
        }


    private:

        /// <summary>
        ///     Creates a new thread proxy.
        /// </summary>
        virtual FreeThreadProxy* Create(unsigned int stackSize)
        {
            return _concrt_new FreeThreadProxy(this, stackSize);
        }

        /// <summary>
        ///     Retires a thread proxy.
        /// </summary>
        virtual void Retire(FreeThreadProxy *pProxy)
        {
            // Canceling the proxy will cause it to perform any necessary cleanup, and exit its dispatch loop.
            pProxy->Cancel();
        }

        // Reference count for the thread proxy factory. For details, see comments in the constructor of ThreadProxy.
        volatile LONG m_referenceCount;

        // Flag that is set to true if shutdown has been initiated on the thread proxy factory.
        volatile bool m_fShutdown;
    };
#pragma warning(pop)

#ifndef _UMS_DISABLED
    /// <summary>
    ///     A factory that creates UMS thread proxies for UMS thread schedulers.
    /// </summary>
    class UMSFreeThreadProxyFactory : public ThreadProxyFactory<UMSFreeThreadProxy>
    {
    public:

        /// <summary>
        ///     Creates a singleton thread proxy factory.
        /// </summary>
        static UMSFreeThreadProxyFactory * CreateFactory(ThreadProxyFactoryManager * pManager)
        {
            StaticInitialize();
            return _concrt_new UMSFreeThreadProxyFactory(pManager);
        }

        /// <summary>
        ///     Destroys a UMS thread proxy factory.
        /// </summary>
        virtual ~UMSFreeThreadProxyFactory()
        {
        }

        /// <summary>
        ///     Initiates shutdown of the factory, and deletes it if shutdown can be completed inline.
        /// </summary>
        virtual void ShutdownFactory()
        {
            RetireThreadProxies();
            m_pUmsThreadEngine->Shutdown();

            if (m_pCompletionList != NULL)
                UMS::DeleteUmsCompletionList(m_pCompletionList);

            // A UMS thread proxy factory can be deleted inline. This is because it is guaranteed that all thread proxies
            // that were loaned out to UMS scheduler proxies were added back to the idle pool before it is possible for
            // the factory to be shutdown. (The factory is shutdown by the RM only after all scheduler proxies have shutdown).
            delete this;
        }

        /// <summary>
        ///     UMS thread proxy factories do not support reference counting.
        /// </summary>
        LONG Reference()
        {
            return 0;
        }

        /// <summary>
        ///     UMS thread proxy factories do not support reference counting.
        /// </summary>
        LONG Release()
        {
            return 0;
        }

        /// <summary>
        ///     Returns an instance of the UMS background poller thread.
        /// </summary>
        UMSBackgroundPoller *GetUMSBackgroundPoller()
        {
            return m_pUmsThreadEngine->GetUMSBackgroundPoller();
        }

        /// <summary>
        ///     Returns an instance of transmogrificator.
        /// </summary>
        Transmogrificator *GetTransmogrificator()
        {
            return &m_transmogrificator;
        }

    protected:

        /// <summary>
        ///     Creates a new UMS thread proxy factory.
        ///     Protected constructor. All construction must go through the CreateFactory API.
        /// </summary>
        UMSFreeThreadProxyFactory(ThreadProxyFactoryManager * pManager) :
            ThreadProxyFactory(pManager),
            m_pCompletionList(NULL),
            m_hCompletionEvent(NULL)
        {
            if (!UMS::CreateUmsCompletionList(&m_pCompletionList))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

            if (!UMS::GetUmsCompletionListEvent(m_pCompletionList, &m_hCompletionEvent))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

            m_pUmsThreadEngine = _concrt_new TransmogrifiedPrimary();
        }

    private:

        /// <summary>
        ///     Creates a new thread proxy.
        /// </summary>
        virtual UMSFreeThreadProxy * Create(unsigned int stackSize)
        {
            HANDLE hObjects[2];

            UMSFreeThreadProxy *pProxy = _concrt_new UMSFreeThreadProxy(this, m_pCompletionList, stackSize);

            hObjects[0] = pProxy->m_hBlock;
            hObjects[1] = m_hCompletionEvent;

            //
            // Make *ABSOLUTELY CERTAIN* that the thread has come back on a completion list prior to returning from Create.  This will ensure that a primary
            // can execute it right away (which is the likely use for creating one of these to begin with).
            //
            for(;;)
            {
                DWORD result = WaitForMultipleObjectsEx(2, hObjects, FALSE, INFINITE, FALSE);
                if (result == WAIT_OBJECT_0)
                {
                    return pProxy;
                }

                PUMS_CONTEXT pUMSContext;

                if (!UMS::DequeueUmsCompletionListItems(m_pCompletionList, 0, &pUMSContext))
                {
                    delete pProxy;
                    throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
                }

                while (pUMSContext != NULL)
                {
                    UMSFreeThreadProxy* pReturnedProxy = static_cast<UMSFreeThreadProxy*>(UMSFreeThreadProxy::FromUMSContext(pUMSContext));
                    RPMTRACE(MTRACE_EVT_ORIGINALCOMPLETION, pReturnedProxy, NULL, pUMSContext);
                    pUMSContext = UMS::GetNextUmsListItem(pUMSContext);

                    // Let the thread engine run it to ThreadMain
                    m_pUmsThreadEngine->QueueToCompletion(pReturnedProxy);
                }
            }
        }

        /// <summary>
        ///     Prepares a thread proxy for use.
        /// </summary>
        /// <param name="pProxy">
        ///     The proxy to prepare.
        /// </param>
        /// <param name="contextPriority">
        ///     The required thread priority for the thread proxy.
        /// <param>
        virtual void Prepare(UMSFreeThreadProxy *pProxy, int contextPriority)
        {
            ThreadProxyFactory::Prepare(pProxy, contextPriority);
            pProxy->ClearCriticalRegion();
        }

        /// <summary>
        ///     Retires a thread proxy.
        /// </summary>
        virtual void Retire(UMSFreeThreadProxy *pProxy)
        {
            RPMTRACE(MTRACE_EVT_RETIRE, pProxy, NULL, NULL);

            // Canceling the proxy will cause it to perform any necessary cleanup, and exit its dispatch loop.
            pProxy->Cancel();

            m_pUmsThreadEngine->QueueToCompletion(pProxy);
        }

        // The primary responsible for retiring UTs.
        TransmogrifiedPrimary *m_pUmsThreadEngine;

        // A background thread responsible for creating UMS primary for nesting
        Transmogrificator m_transmogrificator;

        // The initial completion list upon which threads created from this factory will be placed.  No UMS thread can be scheduled by any primary
        // until it appears on an initial UMS completion list.  We will block Create until this is done.
        PUMS_COMPLETION_LIST m_pCompletionList;

        // The UMS completion list event.
        HANDLE m_hCompletionEvent;
    };
#endif // _UMS_DISABLED

    //
    // A class that holds a collection of thread proxy factories, one for each type of thread proxy.
    //
    class ThreadProxyFactoryManager
    {
    public:

        /// <summary>
        ///     Creates a thread proxy factory manager.
        /// </summary>
        ThreadProxyFactoryManager();

        /// <summary>
        ///     Destroys a thread proxy factory manager.
        /// </summary>
        ~ThreadProxyFactoryManager();

        /// <summary>
        ///     Returns a Win32 thread proxy factory.
        /// </summary>
        FreeThreadProxyFactory * GetFreeThreadProxyFactory();

#ifndef _UMS_DISABLED
        /// <summary>
        ///     Returns a UMS thread proxy factory.
        /// </summary>
        UMSFreeThreadProxyFactory * GetUMSFreeThreadProxyFactory();
#endif // _UMS_DISABLED

        /// <summary>
        ///     Returns the TLS index used to store execution resource information by subscribed threads and thread proxies.
        /// </summary>
        DWORD GetExecutionResourceTls() const
        {
            return m_dwExecutionResourceTlsIndex;
        }

    private:
        // A thread proxy factory for Win32 thread proxies.
        FreeThreadProxyFactory * m_pFreeThreadProxyFactory;

#ifndef _UMS_DISABLED
        // A thread proxy factory for UMS thread proxies.
        UMSFreeThreadProxyFactory * m_pUMSFreeThreadProxyFactory;
#else
        void * m_pUMSFreeThreadProxyFactory;
#endif // _UMS_DISABLED

        // An index to a TLS slot where execution resource pointers are stored.
        DWORD m_dwExecutionResourceTlsIndex;

        // A lock that guards creation of the thread proxy factories.
        _NonReentrantBlockingLock m_proxyFactoryCreationLock;
    };

template <typename threadProxy>
ThreadProxyFactory<threadProxy>::ThreadProxyFactory(ThreadProxyFactoryManager * pManager) :
    m_executionResourceTlsIndex(pManager->GetExecutionResourceTls())
{ }

} // namespace details
} // namespace Concurrency
