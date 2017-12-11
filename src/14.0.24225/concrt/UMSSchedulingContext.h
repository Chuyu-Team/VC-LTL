// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSSchedulingContext.h
//
// Header file for the UMS scheduling context.  This is a special context whose sole purpose in life is to determine
// which context to run next at initial startup of a virtual processor and whenever a UMS thread running on the virtual
// processor blocks.  The RM will invoke this scheduling context whenever a return to primary happens.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    #pragma warning(push)
    #pragma warning(disable: 4265) // non-virtual destructor in base class
    class UMSSchedulingContext final : public ::Concurrency::IExecutionContext
    {
    public:

        /// <summary>
        ///     Creates a new UMS scheduling context that is bound to a particular virtual processor.  Once the scheduling context
        ///     is created, it is always bound to the virtual processor used at creation time.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler to which this virtual processor belongs.
        /// </param>
        /// <param name="pBoundVProc">
        ///     The virtual processor to which this scheduling context is bound.
        /// </param>
        UMSSchedulingContext(UMSThreadScheduler *pScheduler, UMSThreadVirtualProcessor *pBoundVProc);

        /// <summary>
        ///     Destroys a UMS scheduling context.
        /// </summary>
        ~UMSSchedulingContext()
        {
        }

        /// <summary>
        ///     Returns a scheduler unique identifier for the context.
        /// </summary>
        /// <returns>
        ///     The Id of the context.
        /// </returns>
        virtual unsigned int GetId() const;

        /// <summary>
        ///     Returns the scheduler to which this context belongs.
        /// </summary>
        /// <returns>
        ///     The owning scheduler.
        /// </returns>
        virtual IScheduler * GetScheduler();

        /// <summary>
        ///     Returns the thread proxy which is executing this context.  Until the SetProxy method has been called on the given
        ///     context, this will return NULL.  Once the SetProxy method has been called, this returns the IThreadProxy which
        ///     was passed into the SetProxy method.
        /// </summary>
        /// <param name="pThreadProxy">
        ///     The thread proxy which dispatched this particular context.
        /// </param>
        /// <returns>
        ///     An indication of success.
        /// </returns>
        virtual IThreadProxy * GetProxy();

        /// <summary>
        ///     Sets the thread proxy which is executing this context.  The caller must save this and return it upon a call to the GetProxy method.
        ///     Note that the resource manager guarantees stability of the thread proxy while inside the Dispatch method.
        /// </summary>
        /// <param name="pThreadProxy">
        ///     The thread proxy which dispatched this particular context.
        /// </param>
        /// <returns>
        ///     An indication of success.
        /// </returns>
        virtual void SetProxy(IThreadProxy *pThreadProxy);

        /// <summary>
        ///     The method that is called when a thread proxy starts executing a particular context.  The thread proxy which executes
        ///     the context is set in SetProxy before entering the dispatch loop and must be saved and returned on a call to the GetProxy method.
        /// </summary>
        /// <param name="pDispatchState">
        ///     The state under which this IExecutionContext is being dispatched.
        /// </param>
        virtual void Dispatch(DispatchState * pDispatchState);

        /// <summary>
        ///     Returns whether we are on a primary thread.
        /// </summary>
        static bool OnPrimary();

    private:

        /// <summary>
        ///     Sets the current scheduling context to TLS.
        /// </summary>
        void SetAsCurrentTls()
        {
            TlsSetValue(UMSThreadScheduler::t_dwSchedulingContextIndex, this);
        }

        /// <summary>
        ///     Sets all blocked status on a given context.
        /// </summary>
        /// <param name="pPreviousContext">
        ///     The previously running context.
        /// </param>
        /// <param name="fAsynchronous">
        ///     Is previously running context asynchronously blocked.
        /// </param>
        void SetUMSBlocked(UMSThreadInternalContext *pPreviousContext, bool fAsynchronous);

        /// <summary>
        ///     Claims the bound virtual processor and swallows an activation.
        /// </summary>
        void ClaimBoundProcessorAndSwallowActivation();

        // Unique identifier
        unsigned int m_id;

        // The scheduler to which we belong.
        UMSThreadScheduler *m_pScheduler;

        // The virtual processor to which this context is bound.
        UMSThreadVirtualProcessor *m_pBoundVProc;

        // The underlying thread proxy.
        IThreadProxy *m_pThreadProxy;
    };

    #pragma warning(pop)
} // namespace details
} // namespace Concurrency
