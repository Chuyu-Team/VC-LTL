// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadProxyFactoryManager.cpp
//
// Manager for thread proxy factories. The RM relies on a factory manager to pool thread proxies of different types.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Creates a thread proxy factory manager.
    /// </summary>
    ThreadProxyFactoryManager::ThreadProxyFactoryManager() :
        m_pFreeThreadProxyFactory(NULL),
        m_pUMSFreeThreadProxyFactory(NULL)
    {
        // Allocate a TLS slot to track execution resources in the RM.
        m_dwExecutionResourceTlsIndex = platform::__TlsAlloc();
    }

    /// <summary>
    ///     Destroys a thread proxy factory manager.
    /// </summary>
    ThreadProxyFactoryManager::~ThreadProxyFactoryManager()
    {
        if (m_pFreeThreadProxyFactory != NULL)
        {
            m_pFreeThreadProxyFactory->ShutdownFactory();
        }

#ifndef _UMS_DISABLED
        if (m_pUMSFreeThreadProxyFactory != NULL)
        {
            m_pUMSFreeThreadProxyFactory->ShutdownFactory();
        }
#endif

        platform::__TlsFree(m_dwExecutionResourceTlsIndex);
    }

    /// <summary>
    ///     Returns a Win32 thread proxy factory.
    /// </summary>
    FreeThreadProxyFactory * ThreadProxyFactoryManager::GetFreeThreadProxyFactory()
    {
        if (m_pFreeThreadProxyFactory == NULL)
        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_proxyFactoryCreationLock);
            if (m_pFreeThreadProxyFactory == NULL)
            {
                m_pFreeThreadProxyFactory = FreeThreadProxyFactory::CreateFactory(this);
            }
        }
        return m_pFreeThreadProxyFactory;
    }

#ifndef _UMS_DISABLED
    /// <summary>
    ///     Returns a UMS thread proxy factory.
    /// </summary>
    UMSFreeThreadProxyFactory * ThreadProxyFactoryManager::GetUMSFreeThreadProxyFactory()
    {
        if (m_pUMSFreeThreadProxyFactory == NULL)
        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_proxyFactoryCreationLock);
            if (m_pUMSFreeThreadProxyFactory == NULL)
            {
                m_pUMSFreeThreadProxyFactory = UMSFreeThreadProxyFactory::CreateFactory(this);
            }
        }
        return m_pUMSFreeThreadProxyFactory;
    }
#endif

} // namespace details
} // namespace Concurrency
