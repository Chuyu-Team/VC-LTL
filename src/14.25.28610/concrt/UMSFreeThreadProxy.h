// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSFreeThreadProxy.h
//
// Proxy for a UMS thread on a free scheduler.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Representation of a thread running on a virtual processor root for a UMS free scheduler.
    /// </summary>
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class UMSFreeThreadProxy : public UMSThreadProxy
    {
    public:

        /// <summary>
        ///     Constructs a thread proxy for a UMS free scheduler.
        /// </summary>
        /// <param name="pFactory">
        ///     The thread proxy factory that created this thread proxy, and maintains the idle pool of thread proxies.
        /// </param>
        /// <param name="pStartupList">
        ///     The initial UMS completion list that this UMS thread should be put on upon creation.  A primary cannot schedule
        ///     the thread until it has appeared on the specified completion list.
        /// </param>
        /// <param name="stackSize">
        ///     The stack size of the created thread.
        /// </param>
        UMSFreeThreadProxy(IThreadProxyFactory * pFactory, PUMS_COMPLETION_LIST pStartupList, unsigned int stackSize) :
            UMSThreadProxy(pFactory, pStartupList, stackSize)
        {
        }

        /// <summary>
        ///     Destroys a thread proxy for a UMS free scheduler.
        /// </summary>
        virtual ~UMSFreeThreadProxy()
        {
        }

        /// <summary>
        ///     Returns our understanding of a UMS context (a UMS thread proxy) from the UMS system's understanding (a UMS_CONTEXT).  Note
        ///     that this performs no validation that it's actually a *FREE* proxy underneath.  It is the caller's responsibility to ensure
        ///     this prior to making the call!
        /// </summary>
        /// <param name="pUMSContext">
        ///     UMS context
        /// </param>
        /// <returns>
        ///     Pointer to a UMS thread proxy
        /// </returns>
        static UMSFreeThreadProxy *FromUMSContext(PUMS_CONTEXT pUMSContext)
        {
            return static_cast<UMSFreeThreadProxy *> (UMSThreadProxy::FromUMSContext(pUMSContext));
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
        virtual void SwitchTo(::Concurrency::IExecutionContext * pContext, SwitchingProxyState switchState);

        /// <summary>
        ///     Called in order to disassociate the currently executing context from its virtual processor root, and reinitialize the root
        ///     for future use.
        /// </summary>
        /// <param name="switchState">
        ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
        ///     proxy and context.
        /// </param>
        virtual void SwitchOut(SwitchingProxyState switchState = Blocking);

        /// <summary>
        ///     Called in order to yield to the underlying operating system. This allows the operating system to schedule
        ///     other work in that time quantum.
        /// </summary>
        virtual void YieldToSystem();

        /// <summary>
        ///     Returns the context associated with the thread proxy.  This is called from an IUMSUnblockNotification to get the context which
        ///     unblocked (from the scheduler side) so that it can be SwitchTo'd.
        /// </summary>
        IExecutionContext *GetContext()
        {
            return m_pContext;
        }

        /// <summary>
        ///     Set the thread affinity to the given affinity
        /// </summary>
        /// <param name="newAffinity">
        ///     The new affinity for the thread
        /// </param>
        void SetAffinity(HardwareAffinity newAffinity)
        {
            //
            // Since the ApplyTo call is SO expensive (relative to a pure user mode context switch), only make it if the affinity on the previous execution
            // is different.  Different virtual processors may have the same affinity mask (currently likely if they're cores on the same package).
            //
            if (m_previousAffinity != newAffinity)
            {
                newAffinity.ApplyTo(GetThreadHandle());
            }

            m_previousAffinity = newAffinity;
        }

    protected:

        // The context that this thread proxy is executing.
        ::Concurrency::IExecutionContext *m_pContext;

    private:

        template <class T> friend class LockFreeStack;
        friend class UMSFreeVirtualProcessorRoot;
        friend class UMSSchedulerProxy;
        friend class TransmogrifiedPrimary;

        /// <summary>
        ///     Entry for freelist
        /// </summary>
        SLIST_ENTRY m_slNext;

        /// <summary>
        ///     Affinity that was set for this UT
        /// </summary>
        HardwareAffinity m_previousAffinity;

        /// <summary>
        ///     The main dispatch routine for a free thread proxy
        /// </summary>
        virtual void Dispatch();

        /// <summary>
        ///     Called right after obtaining a thread proxy from the factory. Associates the thread proxy with the execution
        ///     context it is about to run.
        /// </summary>
        /// <param name="pContext">
        ///     The context to associate with the thread proxy.
        /// </param>
        void AssociateExecutionContext(::Concurrency::IExecutionContext * pContext);

        /// <summary>
        ///     Returns a thread proxy to the factory when it is no longer in use.
        /// </summary>
        void ReturnIdleProxy();
    };
#pragma warning(pop)

} // namespace details
} // namespace Concurrency
