// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadInternalContext.h
//
// Header file containing the metaphor for a UMS thread based internal execution context/stack.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class UMSThreadInternalContext : public InternalContextBase
    {
    public:
        //
        // Public Methods
        //

        /// <summary>
        ///     Construct an internal thread based context.
        /// </summary>
        UMSThreadInternalContext(SchedulerBase *pScheduler);

        /// <summary>
        ///     Destroys an internal thread based context.
        /// </summary>
        virtual ~UMSThreadInternalContext();

        /// <summary>
        ///     Causes the internal context to block yielding the virtual processor to a different internal context.
        /// </summary>
        virtual void Block();

        /// <summary>
        ///     Returns the type of context
        /// </summary>
        virtual ContextKind GetContextKind() const
        {
            return UMSThreadContext;
        }

        /// <summary>
        ///     Depending on the argument, causes the scheduler to add an extra virtual processor for the
        ///     duration of a block of code or remove a previously added one.
        ///
        ///         Oversubscribe(true);
        ///         // some slow kernel or I/O code, etc.
        ///         Oversubscribe(false);
        ///
        ///     UMS provides automatic oversubscription. This API is largely ignored in UMS except for enforcing
        ///     the matching Oversubscribe(false) API as per the spec.
        /// </summary>
        /// <param name="beginOversubscription">
        ///     A boolean value specifying whether oversubscription is to be turned on or off.
        /// </param>
        virtual void Oversubscribe(bool beginOversubscription);

        /// <summary>
        ///     The method that is called when a thread proxy starts executing a particular context.  The thread proxy which executes
        ///     the context is passed into this method and must be saved and returned on a call to the get_Proxy method.
        /// </summary>
        /// <param name="pDispatchState">
        ///     The state under which this IExecutionContext is being dispatched.
        /// </param>
        virtual void Dispatch(DispatchState * pDispatchState);

        /// <summary>
        ///     Enters a critical region of the scheduler.  Calling this guarantees that the virtual processor on which this context lives
        ///     is guaranteed to be stable throughout the critical region.  For some context types, this is virtually a NOP.  For others
        ///     (UMS), this makes it appear that blocking on the context actually blocks the UMS thread instead of triggering return to
        ///     primary.  Note that critical regions suppress asynchronous blocking but not synchronous blocking.
        /// </summary>
        virtual int EnterCriticalRegionHelper();

        /// <summary>
        ///     Exits a critical region of the scheduler.
        /// </summary>
        virtual int ExitCriticalRegionHelper();

        /// <summary>
        ///     Checks if a context is in a critical region.  This is only safe from either the current context or from a UMS primary which
        ///     has woken due to a given context blocking.
        /// </summary>
        virtual CriticalRegionType GetCriticalRegionType() const
        {
            CONCRT_COREASSERT(m_pThreadProxy != NULL);
            return (static_cast<IUMSThreadProxy *>(m_pThreadProxy))->GetCriticalRegionType();
        }

        /// <summary>
        ///     Enters a hyper-critical region of the scheduler.  Calling this guarantees not only the conditions of a critical region but it
        ///     guarantees that synchronous blocking is suppressed as well.  This allows for lock sharing between the primary and hyper-critical
        ///     regions running on UTs.  No lock sharing can occur between the inside of this region type and the outside of this region type
        ///     on a UT.
        /// </summary>
        virtual int EnterHyperCriticalRegionHelper();

        /// <summary>
        ///     Exits a hyper-critical region of the scheduler.
        /// </summary>
        virtual int ExitHyperCriticalRegionHelper();

        /// <summary>
        ///     Determines whether or not the context is synchronously blocked at this given time.
        /// </summary>
        /// <returns>
        ///     Whether context is in synchronous block state.
        /// </returns>
        virtual bool IsSynchronouslyBlocked() const
        {
            return InternalContextBase::IsSynchronouslyBlocked() || ((m_blockedState & CONTEXT_UMS_SYNC_BLOCKED) != 0);
        }

    private:

        friend class UMSThreadScheduler;
        friend class UMSSchedulingContext;
        friend class UMSThreadVirtualProcessor;

        enum BlockingType
        {
            // No blocking registered.
            BlockingNone,

            // Normal blocking
            BlockingNormal,

            // Critical blocking (e.g.: Win32 blocking call inside a normal critical region)
            BlockingCritical
        };

        // The previous virtual processor we were running on.
        UMSThreadVirtualProcessor *m_pLastVirtualProcessor;

        // How we last blocked.
        volatile BlockingType m_blockingType;

        /// <summary>
        ///     Sets the blocking type.
        /// </summary>
        void SetBlockingType(BlockingType blockingType)
        {
            m_blockingType = blockingType;
        }

        /// <summary>
        ///     Notifies the context that it's blocked.
        /// </summary>
        /// <param name="fAsynchronous">
        ///     Is previously running context asynchronously blocked.
        /// </param>
        void NotifyBlocked(bool fAsynchronous);

        /// <summary>
        ///     Spins until we've registered what blocking type the last block operation was and returns the type.
        /// </summary>
        BlockingType SpinOnAndReturnBlockingType();

        /// <summary>
        ///     Called before this executes on a given virtual processor.
        /// </summary>
        virtual void PrepareToRun(VirtualProcessor *pVProc);

    private:

        friend class UMSThreadScheduler;
        friend class UMSSchedulingContext;
        friend class UMSThreadVirtualProcessor;

        /// <summary>
        ///     Called when the RM wakes up the thread for some reason.
        /// </summary>
        virtual void RMAwaken()
        {
            CMTRACE(MTRACE_EVT_RMAWAKEN, this, m_pVirtualProcessor, NULL);

            //
            // *NOTE*: It is absolutely imperative that this bias awakening of virtual processors to the scheduling node of
            // m_pVirtualProcessor.  RMAwaken is called from an internal context when something comes back on the completion
            // list of a deactivated virtual processor.  Because things are added to runnables on an internal context, a cache
            // local schedule group may put items in the LRC.  The LRC is *NOT* checked when rambling from virtual processors
            // belonging to other scheduling nodes.  If we unblock a context and add it to the LRC on node A and then awake
            // a virtual processor on node B, the work will never be found and the scheduler will deadlock.  This is an artifact
            // of the way in which UMS completion list events come back on deactivated virtual processors.
            //
            (static_cast<UMSThreadScheduler *>(m_pScheduler))->MoveCompletionListToRunnables(m_pVirtualProcessor->GetLocation());
        }
    };
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
