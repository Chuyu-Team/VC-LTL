// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FreeThreadProxy.h
//
// Part of the ConcRT Resource Manager -- this header file contains the internal definition for the free thread
// proxy.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class FreeThreadProxy : public ThreadProxy
    {
    public:

        /// <summary>
        ///     Construct a free thread proxy.
        /// </summary>
        FreeThreadProxy(IThreadProxyFactory * pFactory, unsigned int stackSize)
            : ThreadProxy(pFactory, stackSize)
        { }

        /// <summary>
        ///     Destroy a free thread proxy.
        /// </summary>
        virtual ~FreeThreadProxy()
        { }

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
        virtual void YieldToSystem()
        {
            platform::__SwitchToThread();
        }

        /// <summary>
        ///     Returns the execution context currently attached to the thread proxy.
        /// </summary>
        ::Concurrency::IExecutionContext * GetExecutionContext() { return m_pContext; }

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

        /// <summary>
        ///     Set the thread affinity to the given affinity
        /// </summary>
        /// <param name="newAffinity">
        ///     The new affinity for the thread
        /// </param>
        void SetAffinity(HardwareAffinity newAffinity)
        {
            // Set the new affinity only if it is different
            if (m_previousAffinity != newAffinity)
            {
                newAffinity.ApplyTo(GetThreadHandle());
            }

            m_previousAffinity = newAffinity;
        }

    private:
        //
        // Friend declarations
        //
        template <class T> friend class LockFreeStack;

        //
        // Private member variables
        //

        // Node affinity
        HardwareAffinity m_previousAffinity;

        // Entry for freelist
        SLIST_ENTRY m_slNext;

        // The context that the thread proxy is running at any time. This is updated when the free proxy is first created, and everytime is
        // is taken from the idle pool and associated with a virtual processor root that was handed to a scheduler. A free thread proxy
        // is only associated with one context at a time.
        ::Concurrency::IExecutionContext * m_pContext;

        //
        // Private member functions
        //

        /// <summary>
        ///     The main dispatch routine for a free thread proxy
        /// </summary>
        virtual void Dispatch();
    };
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
