// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadProxy.h
//
// Proxy for an OS context.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    class ThreadProxy : public ::Concurrency::IThreadProxy
    {
    public:
        /// <summary>
        ///     Constructs a thread proxy.
        /// </summary>
        ThreadProxy(IThreadProxyFactory * pFactory, unsigned int stackSize);

        /// <summary>
        ///     Destroys a thread proxy.
        /// </summary>
        virtual ~ThreadProxy();

        /// <summary>
        ///     Retrieves a process unique id for the thread proxy.
        /// </summary>
        unsigned int GetId() const;

        /// <summary>
        ///     Blocks the thread proxy until is is resumed via ResumeExecution or a different thread proxy switching to it.
        /// </summary>
        void SuspendExecution();

        /// <summary>
        ///     Resumes execution of a thread proxy.
        /// </summary>
        void ResumeExecution();

        /// <summary>
        ///     Spins until the 'this' thread proxy is in a firmly blocked state.
        /// </summary>
        /// <remarks>
        ///     This implements a sort of barrier. At certain points during execution, it is essential to wait until a thread proxy
        ///     has set the flag indicating it is blocked, in order to preserve correct behavior. One example is if there is a race
        ///     between block and unblock for the same proxy, i.e. if a thread proxy is trying to block at the same time a different
        ///     context is trying to unblock it.
        /// </remarks>
        void SpinUntilBlocked();

        /// <summary>
        ///     Gets the priority of the thread proxy.
        /// </summary>
        int GetPriority() { return m_threadPriority; }

        /// <summary>
        ///     Sets the priority of the underlying thread.
        /// </summary>
        /// <param name="priority">
        ///     The new priority value for the thread.
        /// </param>
        void SetPriority(int priority);

        /// <summary>
        ///     Gets the stack size of the thread proxy. Multiply by 1 KB to get actual stack size in bytes.
        /// </summary>
        unsigned int GetStackSize() { return m_stackSize; }

        /// <summary>
        ///     Cancels the thread proxy causing the underlying thread to exit.
        /// </summary>
        void Cancel();

        /// <summary>
        ///     Returns the virtual processor root the thread proxy is running on.
        /// </summary>
        VirtualProcessorRoot * GetVirtualProcessorRoot() { return m_pRoot; }

        /// <summary>
        ///     Sets the virtual processor root - used during affinitization.
        /// </summary>
        void SetVirtualProcessorRoot(VirtualProcessorRoot * pRoot) { m_pRoot = pRoot; }

        /// <summary>
        ///     Returns a Win32 handle to the thread that is backing this proxy.
        /// </summary>
        HANDLE GetThreadHandle() { return m_hPhysicalContext; }

#if _DEBUG
        // _DEBUG helper
        DWORD GetThreadId() const { return m_threadId; }
#endif

    protected:

        // The thread proxy factory that created this thread proxy, and maintains the idle pool of thread proxies.
        IThreadProxyFactory * m_pFactory;

        // The OS handle for the underlying UT.
        HANDLE m_hPhysicalContext;

        // The blocking handle.
        HANDLE m_hBlock;

        // The virtual processor root on which this thread proxy is executing.
        VirtualProcessorRoot *m_pRoot;

        // Stores the stack size of the thread proxy. Multiply by 1 KB to get actual stack size in bytes.
        unsigned int m_stackSize;

        // Stores the last priority value that was set on the thread. Initial value is normal priority.
        int m_threadPriority;

        bool m_fSuspended;
        volatile LONG m_fBlocked;
        volatile LONG m_fCanceled;

    private:

        // Process wide unique identifier.
        unsigned int m_id;

        // Thread id.
        DWORD m_threadId;

        /// <summary>
        ///     Dispatch routine for thread proxies.
        /// </summary>
        virtual void Dispatch() = 0;

        /// <summary>
        ///     Thread start routine for proxies.
        /// </summary>
        static DWORD CALLBACK ThreadProxyMain(LPVOID lpParameter);
    };

} // namespace details
} // namespace Concurrency
