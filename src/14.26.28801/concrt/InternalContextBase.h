// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// InternalContextBase.h
//
// Header file containing the base class definition for an internal execution context.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Implements the base class for ConcRT internal contexts.
    /// </summary>

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class InternalContextBase : public IExecutionContext, public ContextBase
    {
    public:

        using ContextBase::GetId;

        //
        // Public methods
        //

        /// <summary>
        ///     Constructs the base class object for an internal context.
        /// </summary>
        InternalContextBase(SchedulerBase *pScheduler);

        /// <summary>
        ///     Causes the internal context to block yielding the virtual processor to a different internal context.
        /// </summary>
        virtual void Block();

        /// <summary>
        ///     Unblocks the internal context putting it on a runnables collection in its schedule group.
        /// </summary>
        virtual void Unblock();

        /// <summary>
        ///     Determines whether or not the context is synchronously blocked at this given time.
        /// </summary>
        /// <returns>
        ///     Whether context is in synchronous block state.
        /// </returns>
        virtual bool IsSynchronouslyBlocked() const
        {
            return (m_contextSwitchingFence == 2);
        }

        /// <summary>
        ///     Yields the virtual processor to a different runnable internal context if one is found.
        /// </summary>
        virtual void Yield();

        /// <summary>
        ///     Yields the virtual processor to a different runnable internal context if one is found.
        ///
        ///     This is intended for spin loops.
        /// </summary>
        virtual void SpinYield();

        /// <summary>
        ///     See comments for Concurrency::Context::Oversubscribe.
        /// </summary>
        virtual void Oversubscribe(bool beginOversubscription);

        /// <summary>
        ///     Destroys the base class object for an internal context.
        /// </summary>
        virtual ~InternalContextBase();

        /// <summary>
        ///     Returns an identifier to the virtual processor the context is currently executing on, if any.
        /// </summary>
        virtual unsigned int GetVirtualProcessorId() const;

        /// <summary>
        ///     Toggle the flag that ensures that scheduler is not deleted until adding is completely finished.
        /// </summary>
        /// <param name="value">
        ///     The value to set the flag to.
        /// </param>
        void CrossGroupRunnable(LONG value) { m_fCrossGroupRunnable = value; }

        /// <summary>
        ///     Set the value of the oversubscribed virtual processor for a context that invokes Oversubscribe.
        /// </summary>
        void SetOversubscribedVProc(VirtualProcessor * pVirtualProcessor) { m_pOversubscribedVProc = pVirtualProcessor; }

        /// <summary>
        ///     Called to retreive the oversubscribed vproc and reset it to null.
        /// </summary>
        VirtualProcessor * GetAndResetOversubscribedVProc(VirtualProcessor * pExpectedVirtualProcessor);

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
        ///     Returns the thread proxy which is executing this context.  Until the Dispatch method has been called on the given
        ///     context, this will return NULL.  Once the Dispatch method has been called, this returns the IThreadProxy which
        ///     was passed into the Dispatch method.
        /// </summary>
        /// <returns>
        ///     The thread proxy which dispatched this particular context, otherwise NULL.
        /// </returns>
        virtual IThreadProxy * GetProxy();

#if _DEBUG
        // _DEBUG helper
        DWORD GetThreadId() const;
#endif

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
        ///     the context is passed into this method and must be saved and returned on a call to the get_Proxy method.
        /// </summary>
        /// <param name="pDispatchState">
        ///     The state under which this IExecutionContext is being dispatched.
        /// </param>
        virtual void Dispatch(DispatchState * pDispatchState);

        /// <summary>
        ///     Allocates a block of memory of the size specified.
        /// </summary>
        /// <param name="numBytes">
        ///     Number of bytes to allocate.
        /// </param>
        /// <returns>
        ///     A pointer to newly allocated memory.
        /// </returns>
        virtual void* Alloc(size_t numBytes);

        /// <summary>
        ///     Frees a block of memory previously allocated by the Alloc API.
        /// </summary>
        /// <param name="pAllocation">
        ///     A pointer to an allocation previously allocated by Alloc.
        /// </param>
        virtual void Free(void* pAllocation);

        /// <summary>
        ///     Swaps the existing schedule group with the one supplied. This function should be called when the context already
        ///     has a schedule group. It decrements the existing group reference count, and references the new one if the caller
        ///     indicates so.
        /// </summary>
        /// <param name="pNewSegment">
        ///     The new group to assign to the context. This may be NULL.
        /// </param>
        /// <param name="referenceNewGroup">
        ///     Whether the context should reference the new group. In some cases there may be an existing reference
        ///     transferred to the context, in which case this parameter is false.
        /// </param>
        void SwapScheduleGroupSegment(ScheduleGroupSegmentBase* pNewSegment, bool referenceNewGroup = false);

        /// <summary>
        ///     Increments the count of work coming in.
        /// </summary>
        void IncrementEnqueuedTaskCounter()
        {
            if (m_pScheduler->IsUMSScheduler())
                IncrementEnqueuedTaskCounterHelper();
            else
                m_pVirtualProcessor->m_enqueuedTaskCounter++;
        }

        void IncrementEnqueuedTaskCounterHelper();

        /// <summary>
        ///     Increments the count of work being done.
        /// </summary>
        void IncrementDequeuedTaskCounter()
        {
            if (m_pScheduler->IsUMSScheduler())
                IncrementDequeuedTaskCounterHelper(1);
            else
                m_pVirtualProcessor->m_dequeuedTaskCounter++;
        }

        /// <summary>
        ///     Increments the count of work being done.
        /// </summary>
        void IncrementDequeuedTaskCounter(unsigned int count)
        {
            if (m_pScheduler->IsUMSScheduler())
                IncrementDequeuedTaskCounterHelper(count);
            else
                m_pVirtualProcessor->m_dequeuedTaskCounter += count;
        }

        void IncrementDequeuedTaskCounterHelper(unsigned int count);

        /// <summary>
        ///     In some cases internal context has not yet recieved a virtual processor so we have
        ///     to save the fact that the work was dequeued and we'll update it in Affinitize.
        /// </summary>
        void SaveDequeuedTask()
        {
            ASSERT(!m_fHasDequeuedTask);
            m_fHasDequeuedTask = true;
        }

        /// <summary>
        ///     Notifies that some work was skipped by an iteration of dispatch loop of this context
        /// </summary>
        void NotifyWorkSkipped()
        {
            m_fWorkSkipped = true;
        }

#if defined(_DEBUG)
        /// <summary>
        ///     Gets the debug bits.
        /// </summary>
        DWORD GetDebugBits() const
        {
            return m_ctxDebugBits;
        }

        /// <summary>
        ///     Sets a series of internal debugging bits for the context.
        /// </summary>
        /// <param name="bits">
        ///     A bitmapped series of CTX_DEBUGBIT_* flags to set within the context.
        /// </param>
        void SetDebugBits(DWORD bits)
        {
            m_ctxDebugBits |= bits;
        }

        /// <summary>
        ///     Clears a series of internal debugging bits for the context.
        /// </summary>
        /// <param name="bits">
        ///     A bitmapped series of CTX_DEBUGBIT_* flags to clear within the context.
        /// </param>
        void ClearDebugBits(DWORD bits)
        {
            m_ctxDebugBits &= ~bits;
        }

        /// <summary>
        ///     Completelky clears all debug bits.
        /// </summary>
        void ClearDebugBits()
        {
            m_ctxDebugBits = 0;
        }

        void NotifyAcquired()
        {
            m_lastAcquiredTid = GetCurrentThreadId();
        }
#endif // _DEBUG

        /// <summary>
        ///     Returns whether the context is in the idle pool or not. Finalization will call this during the sweep phase to
        //      determine all the blocked contexts. A context in the idle pool is considered "not blocked".
        /// </summary>
        bool IsIdle() const
        {
            return m_fIdle;
        }

        /// <summary>
        ///     Prepare a context for execution by associating a scheduler group/chore with it. Scheduler
        //      shall call this routine before executing an internal context
        /// </summary>
        void PrepareForUse(ScheduleGroupSegmentBase* pSegment, _Chore *pChore, bool choreStolen);

        /// <summary>
        ///     Returns whether the context is prepared for execution or must be initialized prior to use.  An unprepared context
        ///     must be initialized via PrepareForUse().
        /// </summary>
        bool IsPrepared() const
        {
            return (m_pSegment != NULL);
        }

        /// <summary>
        ///     Remove a context from execution by dis-associating it from any scheduler group/chore.
        /// </summary>
        void RemoveFromUse();

#if _UMSTRACE
        VirtualProcessor *DBG_GetCurrentVirtualProcessor()
        {
            return UNSAFE_CurrentVirtualProcessor();
        }
#endif // _UMSTRACE

    protected:

        //
        // Protected types
        //

        enum ReasonForSwitch
        {
            GoingIdle,
            Blocking,
            Yielding,
            Nesting
        };

        //
        // Protected data members
        //

        // The thread proxy that is executing this context's dispatch loop, if any.
        IThreadProxy * volatile m_pThreadProxy; // 4/8

        //
        // Protected methods
        //

        /// <summary>
        ///     Spins until the 'this' context is in a firmly blocked state
        /// </summary>
        void SpinUntilBlocked();

        /// <summary>
        ///     Adds the context to a runnables collection, either on the virtual processor, or the schedule group
        /// </summary>
        /// <param name="bias">
        ///     A location specifying where to bias the awakening of virtual processors to.
        /// </param>
        virtual void AddToRunnables(location bias = location());

        /// <summary>
        ///     Switches from one internal context to another.
        /// </summary>
        void SwitchTo(InternalContextBase* pContext, ReasonForSwitch reason);

        /// <summary>
        ///     Switches out the internal context. Useful when the virtual processor is to be retired.
        ///     Is also used when un-nesting a scheduler and the context is returning to its original scheduler.
        /// </summary>
        /// <param name="reason">
        ///     The reason for switching out of this vproc
        /// </param>
        /// <returns>
        ///     True if the context has been canceled.
        /// </returns>
        bool SwitchOut(ReasonForSwitch reason);

        /// <summary>
        ///     Cancels the context, causing it to exit the dispatch loop if it is executing on a virtual processor
        /// </summary>
        virtual void Cancel();

        /// <summary>
        ///     If internal context does not own this virtual processor then claim it back. This might require
        ///     waiting until it becomes available.
        /// </summary>
        void ReclaimVirtualProcessor();

        /// <summary>
        ///     This function is called to execute the associated chore if one is available. The chore can be a stolen unrealized
        ///     chore or realized chore.
        /// </summary>
        /// <returns>
        ///     Returns true if an associated chore was executed, false otherwise.
        /// </returns>
        bool ExecutedAssociatedChore();

        /// <summary>
        ///     Performs the necessary cleanup for a canceled context in its dispatch routine.
        /// <summary>
        void CleanupDispatchedContextOnCancel();

        /// <summary>
        ///     Called in the dispatch loop to check if the virtual processor the context is running on is marked for retirement,
        ///     and retires the virtual processor if it is.
        /// <summary>
        /// <returns>
        ///     True if the virtual processor was retired, false otherwise.
        /// </returns>
        bool IsVirtualProcessorRetired();

        /// <summary>
        ///     Searches for work using the search algorithm specified by the scheduler's policy. Also prepares the context to execute
        ///     work by reclaiming the virtual processor if necessary.
        /// </summary>
        /// <param name=pWork>
        ///     A pointer to a work item which is filled in if work was found.
        /// </param>
        /// <returns>
        ///     True if work was found, false otherwise.
        /// </returns>
        bool WorkWasFound(WorkItem * pWork);

        /// <summary>
        ///     Switches to the runnable context represented by the work item.
        /// </summary>
        /// <param name=pWork>
        ///     A pointer to a work item to be executed.
        /// </param>
        void SwitchToRunnableContext(WorkItem * pWork);

        /// <summary>
        ///     Executes the chore (realized or unrealized) specified by the work item.
        /// </summary>
        /// <param name=pWork>
        ///     A pointer to a work item that represents a realized or unrealized chore.
        /// </param>
        void ExecuteChoreInline(WorkItem * pWork);

        /// <summary>
        ///     This method implements the wait-for-work and cancelation protocol.
        /// </summary>
        void WaitForWork(void);

        /// <summary>
        ///     Performs cleanup of the internal thread context.
        /// </summary>
        void Cleanup();

        /// <summary>
        ///     Called before this executes on a given virtual processor.
        /// </summary>
        virtual void PrepareToRun(VirtualProcessor *pVProc)
        {
#if defined(_DEBUG)
            m_lastRunPrepareTimeStamp = _ReadTimeStampCounter();
            m_prepareCount++;
            m_lastAffinitizedTid = GetCurrentThreadId();
#endif // _DEBUG
            m_pVirtualProcessor = pVProc;
            CONCRT_COREASSERT(m_pSegment != NULL);
            InterlockedExchange(&m_blockedState, CONTEXT_NOT_BLOCKED);
        }

        // Virtual processor the context is executing on.
#if defined(_DEBUG)
        void _PutVirtualProcessor(VirtualProcessor *pVirtualProcessor)
        {
            //
            // If this assertion fires, someone is changing m_pVirtualProcessor outside a critical region.  Doing this violates safety
            // on a UMS scheduler.  m_pVirtualProcessor is not guaranteed to be stable on a UMS context.  All manipulation must happen
            // inside a critical region.
            //
            CONCRT_COREASSERT(_m_pVirtualProcessor == NULL || IsInsideCriticalRegion());
            _m_pVirtualProcessor = pVirtualProcessor;
        }

        VirtualProcessor *_GetVirtualProcessor() const
        {
            //
            // If this assertion fires, someone is examining m_pVirtualProcessor outside a critical region.  Doing this violates safety
            // on a UMS scheduler.  m_pVirtualProcessor is not guaranteed to be stable on a UMS context.  All manipulation must happen
            // inside a critical region.
            //
            CONCRT_COREASSERT(_m_pVirtualProcessor == NULL || IsInsideCriticalRegion());
            return _m_pVirtualProcessor;
        }

        __declspec(property(get=_GetVirtualProcessor, put=_PutVirtualProcessor)) VirtualProcessor *m_pVirtualProcessor;
        VirtualProcessor * volatile _m_pVirtualProcessor;

        VirtualProcessor *UNSAFE_CurrentVirtualProcessor() const
        {
            return _m_pVirtualProcessor;
        }

        void UNSAFE_SetVirtualProcessor(VirtualProcessor *pVirtualProcessor)
        {
            _m_pVirtualProcessor = pVirtualProcessor;
        }
#else
        VirtualProcessor * volatile m_pVirtualProcessor;

        VirtualProcessor *UNSAFE_CurrentVirtualProcessor() const
        {
            return m_pVirtualProcessor;
        }

        void UNSAFE_SetVirtualProcessor(VirtualProcessor *pVirtualProcessor)
        {
            m_pVirtualProcessor = pVirtualProcessor;
        }
#endif

    private:
        friend class ExternalContextBase;
        friend class SchedulerBase;
        friend class ThreadScheduler;
        friend class UMSThreadScheduler;
        friend class VirtualProcessor;
        friend class UMSThreadVirtualProcessor;
        friend class SchedulingRing;
        friend class UMSSchedulingContext;
        friend class location;
        template <typename T> friend class Mailbox;
        template <class T, class Counter> friend class Stack;
        template <typename T> friend class SQueue;

        // This helper is used to avoid circular reference:
        // Mailbox -> InternalContextBase -> ContextBase -> WorkStealingQueue -> Mailbox
        friend unsigned int GetProcessorMaskId(InternalContextBase * pContext);

        //
        // Private data
        //

        // Pointer to an oversubscribed virtual processor if one is present.
        VirtualProcessor * volatile m_pOversubscribedVProc;

        // Chore associated with the context - this could be a realized chore or a stolen chore. The chore is associated with the context
        // either when the internal context first starts up, or it is picked out of the idle pool by the scheduler. The context must execute this chore
        // before it starts looking for other work. This is used for indirect aliasing of unstructured task collections.
        _Chore *m_pAssociatedChore;

        // Counter that indicates how many times the internal context has spun waiting for work.
        unsigned int m_searchCount;

        // Flag that indicates whether the internal context is canceled.
        volatile bool m_fCanceled;

        // Flag that indicates whether the associated chore is a stolen unrealized chore or a realized chore.
        bool m_fAssociatedChoreStolen;

        // Flag that indicates whether internal context is in the final search for work state.
        bool m_fIsVisibleVirtualProcessor;

        // Flag that indicates whether internal context has dequeued a piece of work without being able
        // to immediately update the statistics numbers on a virtual processor (it was not affinitized).
        bool m_fHasDequeuedTask : 1;

        // Indicates that some work was skipped in the dispatch loop. Currently, this is set if we failed to check some of the work stealing
        // queues due to in-progress task collection cancellation.
        bool m_fWorkSkipped : 1;

        // Debugging purposes: this informs whether the context was *EVER* put on a free list or whether it is a fresh context.
        bool m_fEverRecycled : 1;

        // Debug information (particularly useful for UMS)

        //
        // Time logging for forward progress determinations.
        //
        __int64 m_workStartTimeStamp;
        __int64 m_lastRunPrepareTimeStamp;
        DWORD m_prepareCount;

        DWORD m_ctxDebugBits;

        // The last TID this context was dispatched on.  You can normally get this from m_pThreadProxy.
        DWORD m_lastDispatchedTid;

        // The last TID this context was acquired/created on.
        DWORD m_lastAcquiredTid;

        // The last TID this context was affinitized on.
        DWORD m_lastAffinitizedTid;

        //
        // Tracks the last assigned thread proxy (normally the same as m_pThreadProxy) -- but may not be for recycled contexts.
        //
        IThreadProxy *m_pAssignedThreadProxy;
        IThreadProxy *m_pLastAssignedThreadProxy;

        // A flag that is used by contexts adding runnables to a scheduler. When those contexts (the ones performing the add)
        // do not implicitly have a reference to the schedule group the runnable belongs to, setting this flag on the runnable
        // context they are adding to the scheduler's queues, ensures that the group does not get destroyed and the scheduler
        // does not get finalized while they are touching scheduler/schedule group data.
        volatile LONG m_fCrossGroupRunnable;

        // Intrusive next pointer for SafeSQueue.
        InternalContextBase *m_pNext;

        // Flag that indicates whether the internal context is in the idle pool or not
        volatile bool m_fIdle;

        //
        // Private methods
        //

        /// <summary>
        ///     Called to find work to switch to, when the current context needs to block or nest a different scheduler.
        ///     The function may return NULL if no work was found and thread creation was disallowed by the thread
        ///     throttler.
        /// </summary>
        InternalContextBase* FindWorkForBlockingOrNesting(bool& fSFWContext, bool& fBoundUnrealized);

        /// <summary>
        ///     Called when a context is nesting a scheduler. If nesting takes place on what is an internal context in
        ///     the 'parent' scheduler, the context must return the virtual processor to the parent scheduler.
        /// </summary>
        void LeaveScheduler();

        /// <summary>
        ///     Called when a context is un-nesting a scheduler. If the parent context is an internal context, it needs
        ///     to rejoin the parent scheduler by looking for a virtual processor it can execute on.
        /// </summary>
        void RejoinScheduler();

        /// <summary>
        ///     Called when the RM wakes up the thread for some reason.
        /// </summary>
        virtual void RMAwaken()
        {
        }

    };

    unsigned int GetProcessorMaskId(InternalContextBase* pContext);
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
