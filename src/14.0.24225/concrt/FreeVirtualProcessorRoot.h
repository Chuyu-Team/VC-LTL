// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FreeVirtualProcessorRoot.h
//
// Part of the ConcRT Resource Manager -- this header file contains the internal definition for the free virtual
// processor root (represents a virtual processor as handed to a scheduler).
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{

    class FreeVirtualProcessorRoot : public VirtualProcessorRoot
    {
    public:

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
        FreeVirtualProcessorRoot(SchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex);

        /// <summary>
        ///     Causes the scheduler to start running a thread proxy on the specified virtual processor root which will execute
        ///     the Dispatch method of the context supplied by pContext.
        /// </summary>
        /// <param name="pContext">
        ///     The context which will be dispatched on a (potentially) new thread running atop this virtual processor root.
        /// </param>
        virtual void Activate(::Concurrency::IExecutionContext *pContext);

        /// <summary>
        ///     Causes the thread proxy running atop this virtual processor root to temporarily stop dispatching pContext.
        /// </summary>
        /// <param name="pContext">
        ///     The context which should temporarily stop being dispatched by the thread proxy running atop this virtual processor root.
        /// </param>
        virtual bool Deactivate(::Concurrency::IExecutionContext *pContext);

        /// <summary>
        ///     Forces all data in the memory heirarchy of one processor to be visible to all other processors.
        /// </summary>
        /// <param name="pContext">
        ///     The context which is currently being dispatched by this root.
        /// </param>
        virtual void EnsureAllTasksVisible(::Concurrency::IExecutionContext *pContext);

        // **************************************************
        // Internal
        // **************************************************

        /// <summary>
        ///     Deletes the virtual processor.
        /// </summary>
        virtual void DeleteThis();

        /// <summary>
        ///     Called to affinitize the given thread proxy to this virtual processor.
        /// </summary>
        /// <param name="pThreadProxy">
        ///     The new thread proxy to run atop this virtual processor root.
        /// </param>
        void Affinitize(FreeThreadProxy *pThreadProxy);

        /// <summary>
        ///     Called in order to reset this virtual processor root to a completely quiescent state (not running anything).
        /// </summary>
        /// <param name="switchState">
        ///     Indicates the state of the thread proxy that is making the call. The parameter is of type <typeparamref name="SwitchingProxyState"/>.
        /// </param>
        void ResetOnIdle(SwitchingProxyState switchState);

    protected:

        /// <summary>
        ///     Spins until there is no thread proxy executing atop the virtual processor root.
        /// </summary>
        void SpinUntilIdle()
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);
            while(m_pExecutingProxy != NULL && m_pDeactivatedProxy == NULL)
            {
                spinWait._SpinOnce();
            }
        }

        // The thread proxy which is currently executing atop this virtual processor root.
        ThreadProxy * volatile m_pExecutingProxy;

        // The deactivated proxy.
        ThreadProxy * volatile m_pDeactivatedProxy;

    };
} // namespace details
} // namespace Concurrency
