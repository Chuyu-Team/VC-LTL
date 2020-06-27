// ==++==
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ContextBase.h
//
// Header file containing the metaphor for an execution context/stack/thread.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

// Defines used for context blocking (m_blockedState):
// * Possible blocked states
#define CONTEXT_NOT_BLOCKED         0x0
#define CONTEXT_BLOCKED             0x1
#define CONTEXT_UMS_SYNC_BLOCKED    0x2
#define CONTEXT_UMS_ASYNC_BLOCKED   0x4

// * Useful bit-masks
#define CONTEXT_SYNC_BLOCKED        (CONTEXT_BLOCKED | CONTEXT_UMS_SYNC_BLOCKED)
#define CONTEXT_UMS_BLOCKED         (CONTEXT_UMS_SYNC_BLOCKED | CONTEXT_UMS_ASYNC_BLOCKED)

#define ENTIRE_CONTEXT_CANCELED     -999

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Implements the base class for a ConcRT execution context.
    /// </summary>
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class ContextBase : public Context
    {
    public:
        //
        // Public Methods
        //

        /// <summary>
        ///     Constructor
        /// </summary>
        ContextBase(SchedulerBase *pScheduler, bool fIsExternal);

        /// <summary>
        ///     Returns a unique identifier to the context.
        /// </summary>
        virtual unsigned int GetId() const;

        /// <summary>
        ///     Returns an identifier to the virtual processor the context is currently executing on, if any.
        /// </summary>
        virtual unsigned int GetVirtualProcessorId() const =0;

        /// <summary>
        ///     Returns an identifier to the schedule group the context is currently working on, if any.
        /// </summary>
        virtual unsigned int GetScheduleGroupId() const;

        /// <summary>
        ///     Returns the reference count of the underlying schedule group, which is equivalent
        ///     to the number of contexts performing work on the schedule group.
        /// <summary>
        unsigned int ScheduleGroupRefCount() const;

        /// <summary>
        ///     Causes the context to block, yielding the virtual processor to another context.
        /// </summary>
        virtual void Block() =0;

        /// <summary>
        ///     Unblocks the context and makes it runnable.
        /// </summary>
        virtual void Unblock() =0;

        /// <summary>
        ///     Determines whether or not the context is synchronously blocked at this given time.
        /// </summary>
        /// <returns>
        ///     Whether context is in synchronous block state.
        /// </returns>
        virtual bool IsSynchronouslyBlocked() const =0;

        /// <summary>
        ///     Returns whether the context is blocked or not. Note that this definition of blocked is "blocked and requires
        ///     eventual reexecution -- e.g.: finalization will call this during the sweep phase).
        /// </summary>
        bool IsBlocked() const
        {
            return (m_blockedState != CONTEXT_NOT_BLOCKED);
        }

        /// <summary>
        ///     Yields execution to a different runnable context, and puts this context on a runnables collection.
        ///     If no context is found to yield to, the context continues to run.
        /// </summary>
        virtual void Yield() =0;

        /// <summary>
        ///     Yields execution to a different runnable context, and puts this context on a runnables collection.
        ///     If no context is found to yield to, the context continues to run.
        ///
        ///     This is intended for spin loops.
        /// </summary>
        virtual void SpinYield() =0;

        /// <summary>
        ///     See comments for Concurrency::Context::Oversubscribe.
        /// </summary>
        virtual void Oversubscribe(bool beginOversubscription) =0;

        /// <summary>
        ///     Cleans up the Context.
        /// </summary>
        void Cleanup();

        /// <summary>
        ///     Allocates a block of memory of the size specified.
        /// </summary>
        /// <param name="numBytes">
        ///     Number of bytes to allocate.
        /// </param>
        /// <returns>
        ///     A pointer to newly allocated memory.
        /// </returns>
        virtual void* Alloc(size_t numBytes) =0;

        /// <summary>
        ///     Frees a block of memory previously allocated by the Alloc API.
        /// </summary>
        /// <param name="pAllocation">
        ///     A pointer to an allocation previously allocated by Alloc.
        /// </param>
        virtual void Free(void* pAllocation) =0;

        /// <summary>
        ///     Enters a critical region of the scheduler. Calling this guarantees that the virtual processor on which this context lives
        ///     is guaranteed to be stable throughout the critical region. For some context types, this is virtually a NOP. For others
        ///     (UMS), this makes it appear that blocking on the context actually blocks the UMS thread instead of triggering return to
        ///     primary. Note that critical regions suppress asynchronous blocking but not synchronous blocking.
        /// </summary>
        virtual int EnterCriticalRegionHelper()
        {
            CONCRT_COREASSERT(Context::CurrentContext() == this);
            return ++m_criticalRegionCount;
        }

        int EnterCriticalRegion();

        /// <summary>
        ///    Static version of EnterCriticalRegion.
        /// </summary>
        static void StaticEnterCriticalRegion();

        /// <summary>
        ///     Enters a hyper-critical region of the scheduler. Calling this guarantees not only the conditions of a critical region but it
        ///     guarantees that synchronous blocking is suppressed as well. This allows for lock sharing between the primary and hyper-critical
        ///     regions running on UTs. No lock sharing can occur between the inside of this region type and the outside of this region type
        ///     on a UT.
        /// </summary>
        virtual int EnterHyperCriticalRegionHelper()
        {
            m_criticalRegionCount++;
            return ++m_hyperCriticalRegionCount;
        }

        int EnterHyperCriticalRegion();

        /// <summary>
        ///    Static version of EnterHyperCriticalRegion.
        /// </summary>
        static void StaticEnterHyperCriticalRegion();

        /// <summary>
        ///     Exits a critical region of the scheduler.
        /// </summary>
        virtual int ExitCriticalRegionHelper()
        {
            CONCRT_COREASSERT(m_criticalRegionCount > 0);
            CONCRT_COREASSERT(Context::CurrentContext() == this);
            return --m_criticalRegionCount;
        }

        int ExitCriticalRegion();

        /// <summary>
        ///    Static version of ExitCriticalRegion.
        /// </summary>
        static void StaticExitCriticalRegion();

        /// <summary>
        ///     Exits a hyper-critical region of the scheduler.
        /// </summary>
        virtual int ExitHyperCriticalRegionHelper()
        {
            CONCRT_COREASSERT(m_hyperCriticalRegionCount > 0);
            CONCRT_COREASSERT(m_criticalRegionCount > 0);
            m_criticalRegionCount--;
            return --m_hyperCriticalRegionCount;
        }

        int ExitHyperCriticalRegion();

        /// <summary>
        ///    Static version of ExitHyperCriticalRegion.
        /// </summary>
        static void StaticExitHyperCriticalRegion();

        /// <summary>
        ///     Checks if a context is in a critical region. This is only safe from either the current context or from a UMS primary which
        ///     has woken due to a given context blocking.
        /// </summary>
        virtual CriticalRegionType GetCriticalRegionType() const
        {
            if (m_hyperCriticalRegionCount > 0)
                return InsideHyperCriticalRegion;
            if (m_criticalRegionCount > 0)
                return InsideCriticalRegion;
            return OutsideCriticalRegion;
        }

        /// <summary>
        ///     Since critical region counts are turned off for thread schedulers, this method is used
        ///     where the return value is expected to be true. For a thread scheduler, it always returns true.
        ///     For a ums scheduler it returns (GetCriticalRegionType() != OutsideCriticalRegion).
        ///     IsInsideContextLevelCriticalRegion only checks (ContextBase::GetCriticalRegionType() != OutsideCriticalRegion).
        /// </summary>
        bool IsInsideCriticalRegion() const;

        /// <summary>
        ///     Static version of GetCriticalRegionType.
        /// </summary>
        static CriticalRegionType StaticGetCriticalRegionType();

        /// <summary>
        ///     Set critical region counts to zero
        /// </summary>
        void ClearCriticalRegion()
        {
            m_hyperCriticalRegionCount = m_criticalRegionCount = 0;
        }

#if defined(_DEBUG)
        /// <summary>
        ///     Tells the context it's shutting down a virtual processor and normal lock validations don't apply.
        /// </summary>
        void SetShutdownValidations()
        {
            m_fShutdownValidations = true;
        }

        /// <summary>
        ///     Re-enable normal lock validations
        /// </summary>
        void ClearShutdownValidations()
        {
            m_fShutdownValidations = false;
        }

        /// <summary>
        ///     Returns whether or not the context is in a "shutting down a virtual processor" mode where normal lock validations don't apply.
        /// </summary>
        bool IsShutdownValidations() const
        {
            return m_fShutdownValidations;
        }
#endif // _DEBUG

        /// <summary>
        ///     Wrapper for m_pWorkQueue for use in unstructured task collections
        ///     that performs delay construction as well as insertion into schedule group.
        /// </summary>
        WorkQueue *GetWorkQueue()
        {
            // want inlining
            if (m_pWorkQueue == NULL)
                CreateWorkQueue();
            return m_pWorkQueue;
        }

        /// <summary>
        ///     Wrapper for m_pWorkQueue for use in structured task collections
        ///     that performs delay construction as well as insertion into schedule group.
        /// </summary>
        WorkQueue *GetStructuredWorkQueue()
        {
            // want inlining
            if (m_pWorkQueue == NULL)
                CreateStructuredWorkQueue();
            return m_pWorkQueue;
        }

        /// <summary>
        ///     Create a workqueue for use in unstructured task collections.
        /// </summary>
        void CreateWorkQueue();

        /// <summary>
        ///     Create a workqueue for use in structured task collections.
        /// </summary>
        void CreateStructuredWorkQueue();

        /// <summary>
        ///     Returns a unique identifier for the work queue associated with this context. Note that this should only be used
        ///     for binding (e.g.: task collection binding)
        /// </summary>
        unsigned int GetWorkQueueIdentity()
        {
            return GetWorkQueue()->Id();
        }

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the structured work stealing queue.
        /// </param>
        /// <param name="_PLocation">
        ///     The location where the unrealized chore should execute.
        /// </param>
        void PushStructured(_UnrealizedChore *pChore, location *pLocation);

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the structured work stealing queue.
        /// </param>
        void PushStructured(_UnrealizedChore *pChore);

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the unstructured work stealing queue.
        /// </param>
        /// <param name="_PLocation">
        ///     The location where the unrealized chore should execute.
        /// </param>
        int PushUnstructured(_UnrealizedChore *pChore, location *pLocation);

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the unstructured work stealing queue.
        /// </param>
        int PushUnstructured(_UnrealizedChore *pChore);

        /// <summary>
        ///     Sweeps the unstructured work stealing queue for items matching a predicate and potentially removes them
        ///     based on the result of a callback.
        /// </summary>
        /// <param name="pPredicate">
        ///     The predicate for things to call pSweepFn on.
        /// </param>
        /// <param name="pData">
        ///     The data for the predicate callback
        /// </param>
        /// <param name="pSweepFn">
        ///     The sweep function
        /// </param>
        void SweepUnstructured(WorkStealingQueue<_UnrealizedChore>::SweepPredicate pPredicate,
                               void *pData,
                               WorkStealingQueue<_UnrealizedChore>::SweepFunction pSweepFn
                               );

        /// <summary>
        ///     Pops the topmost chore from the work stealing queue for structured parallelism. Failure
        ///     to pop typically indicates stealing.
        /// </summary>
        /// <returns>
        ///     An unrealized chore from the structured work stealing queue or NULL if none is present.
        /// </returns>
        _UnrealizedChore *PopStructured();

        /// <summary>
        ///     Attempts to pop the chore specified by a cookie value from the unstructured work stealing queue. Failure
        ///     to pop typically indicates stealing.
        /// </summary>
        /// <param name="cookie">
        ///     A cookie returned from PushUnstructured indicating the chore to attempt to pop from
        ///     the unstructured work stealing queue.
        /// </param>
        /// <returns>
        ///     The specified unrealized chore (as indicated by cookie) or NULL if it could not be popped from
        ///     the work stealing queue.
        /// </returns>
        _UnrealizedChore *TryPopUnstructured(int cookie);

        /// <summary>
        ///     Returns the scheduler the specified context is associated with.
        /// </summary>
        SchedulerBase *GetScheduler() const;

        /// <summary>
        ///     Returns the schedule group the specified context is associated with.
        /// </summary>
        ScheduleGroupBase *GetScheduleGroup() const;

        /// <summary>
        ///     Returns the schedule group segment the specified context is associated with.
        /// </summary>
        ScheduleGroupSegmentBase *GetScheduleGroupSegment() const;

        /// <summary>
        ///     Tells whether the context is an external context
        /// </summary>
        bool IsExternal() const { return m_fIsExternal; }

        /// <summary>
        ///     Gets the indirect alias.
        /// </summary>
        _TaskCollection *GetIndirectAlias() const;

        /// <summary>
        ///     Sets the indirect alias.
        /// </summary>
        void SetIndirectAlias(_TaskCollection *pAlias);

        /// <summary>
        ///     Returns whether a task collection or structured task collection executing on this context was canceled while it was inlined.
        /// </summary>
        bool HasInlineCancellation() const
        {
            return (m_inlineCancellations > 0);
        }

        /// <summary>
        ///     Returns whether the entire context was canceled due to a steal.
        /// </summary>
        bool IsEntireContextCanceled() const
        {
            return (m_canceledContext != 0);
        }

        /// <summary>
        ///     Called in order to indicate that a cancellation is happening for a structured task collection associated with this thread
        ///     that has not been inlined yet.
        /// </summary>
        void PendingCancel()
        {
            InterlockedIncrement(&m_pendingCancellations);
        }

        /// <summary>
        ///     Called when a pending cancel completes.
        /// </summary>
        void PendingCancelComplete()
        {
            ASSERT(m_pendingCancellations > 0);
            InterlockedDecrement(&m_pendingCancellations);
        }

        /// <summary>
        ///     Returns whether a structured task collection executing on this context was canceled before it was inlined.
        /// </summary>
        bool HasPendingCancellation() const
        {
            return (m_pendingCancellations > 0);
        }

        /// <summary>
        ///     Returns whether there is any cancellation on the context (pending or inline)
        /// </summary>
        bool HasAnyCancellation() const
        {
            return (m_pendingCancellations + m_inlineCancellations > 0);
        }

        /// <summary>
        ///     Called to determine if a confirmed cancellation on this context is hidden at the depth of the caller.
        ///     A governing token that is not canceled could be protecting the task collection from cancellation from above.
        /// <summary>
        bool TokenHidesCancellation(_TaskCollectionBase* pCurrentTaskCollection, bool hasOverrideToken) const;

        /// <summary>
        ///     Called to determine whether a inline or pending cancellation on this context is visible to the caller.
        /// </summary>
        bool IsCancellationVisible(_TaskCollectionBase* pCurrentTaskCollection, bool hasOverrideToken = false) const;

        /// <summary>
        ///     Called in order to indicate that a collection executing on this context was canceled. This will often cause cancellation
        ///     and unwinding of the entire context (up to the point where we get to the canceled collection).
        /// </summary>
        void CancelCollection(int inliningDepth);

        /// <summary>
        ///     Called in order to indicate that we're blowing away the entire context. It's stolen from a collection which was canceled.
        /// </summary>
        void CancelEntireContext()
        {
            InterlockedExchange(&m_canceledContext, TRUE);
            CancelCollection(ENTIRE_CONTEXT_CANCELED);
        }

        /// <summary>
        ///     When a cancellation bubbles up to the collection being canceled, this function is called in order to stop propagation of
        ///     the cancellation further up the work tree.
        /// </summary>
        bool CancelCollectionComplete(int inliningDepth);

        /// <summary>
        ///     Completely clears the cancel count. This should be called when a root stolen chore completes on a context.
        /// </summary>
        void ClearCancel()
        {
            m_minCancellationDepth = -1;
            m_maxCancellationDepth = -1;
            m_inlineCancellations = 0;
            m_canceledContext = 0;
        }

        /// <summary>
        ///     Returns the task collection executing atop a stolen context.
        /// </summary>
        _TaskCollectionBase *GetRootCollection()
        {
            return m_pRootCollection;
        }

        /// <summary>
        ///     Sets the task collection executing atop a stolen context. Note that this also sets the executing collection since the root
        ///     collection is executing at the time it is set.
        /// </summary>
        void SetRootCollection(_TaskCollectionBase *pRootCollection)
        {
            m_pRootCollection = pRootCollection;
            m_pExecutingCollection = pRootCollection;
        }

        /// <summary>
        ///     Gets the task collection currently executing atop the context.
        /// </summary>
        _TaskCollectionBase *GetExecutingCollection()
        {
            return m_pExecutingCollection;
        }

        /// <summary>
        ///     Sets the task collection currently executing atop the context.
        /// </summary>
        void SetExecutingCollection(_TaskCollectionBase *pExecutingCollection)
        {
            m_pExecutingCollection = pExecutingCollection;
        }

        /// <summary>
        ///     Gets the cancellation token currently governing this context.
        /// </summary>
        _CancellationTokenState *GetGoverningTokenState()
        {
            return m_pGoverningTokenState;
        }

        /// <summary>
        ///     Sets the cancellation token currently governing this context.
        /// </summary>
        void PushGoverningTokenState(_CancellationTokenState *pTokenState, int inliningDepth);

        /// <summary>
        ///     Reverts to the previously set cancellation token.
        /// </summary>
        void PopGoverningTokenState(_CancellationTokenState *pTokenState);

        /// <summary>
        ///     Returns an indication as to whether a cancellation is occurring at the specified depth. The result here is only valid when
        ///     called from the thread representing this context.
        /// </summary>
        bool IsCanceledAtDepth(int depth)
        {
            return IsCanceledAtDepth(m_pExecutingCollection, depth);
        }

        /// <summary>
        ///     Returns an indication as to whether a cancellation is occurring at the depth specified by the inlining depth of the given collection. This
        ///     may ONLY be called while the context chaining (stealers) lock is held and only where it is known that there is a steal from
        ///     pStartingCollection.
        /// </summary>
        bool IsCanceledAtDepth(_TaskCollectionBase *pStartingCollection)
        {
            return IsCanceledAtDepth(pStartingCollection, pStartingCollection->_M_inliningDepth);
        }

        /// <summary>
        ///     Verifies that a cancellation beacon signal is really a cancellation for that beacon.
        /// </summary>
        bool ConfirmCancel(_Beacon_reference *pBeaconRef)
        {
            CancellationBeacon *pBeacon = CONTAINING_RECORD(pBeaconRef, CancellationBeacon, m_beacon);
            return IsCanceledAtDepth(pBeacon->m_beaconDepth);
        }

        /// <summary>
        ///     Places a reference on the context preventing it from being destroyed until such time as the stealer is added to the chain
        ///     via AddStealer. Note that the operation of AddStealer should happen rapidly as it will *BLOCK* cleanup of the context.
        /// </summary>
        void ReferenceForCancellation();

        /// <summary>
        ///     Removes a reference on the context which was preventing it from being destroyed.
        /// </summary>
        void DereferenceForCancellation();

        /// <summary>
        ///     Adds a stealing context. Removes a reference.
        /// </summary>
        void AddStealer(ContextBase *pStealer, bool fDereferenceForCancellation);

        /// <summary>
        ///     Removes a stealing context.
        /// </summary>
        void RemoveStealer(ContextBase *pStealer);

        /// <summary>
        ///     Called by a stolen chore to flag the context as running a chore for which the steal is chained to a task collection instead
        ///     of the context.
        /// </summary>
        void NotifyTaskCollectionChainedStealer()
        {
            m_fContextChainedStealer = false;
        }

        /// <summary>
        ///     Returns whether the given context's steal is chained to the context (true) or some task collection (false)
        /// </summary>
        bool IsContextChainedStealer() const
        {
            return m_fContextChainedStealer;
        }

        /// <summary>
        ///     Called on both internal and external contexts, either when the are put into an idle pool to
        ///     be recycled, or when they are ready to be deleted. The API moves the contexts that are in
        ///     the list of 'stealers' (used for cancellation) to lists in the task collections from which
        ///     those contexts have stolen chores.
        /// </summary>
        void DetachStealers();

        /// <summary>
        ///     Gets an arbitrary alias out of the context's alias table.
        /// </summary>
        _TaskCollection *GetArbitraryAlias(_TaskCollection *pCollection)
        {
            Hash<_TaskCollection*, _TaskCollection*>::ListNode *pNode = m_aliasTable.Find(pCollection, NULL);
            _TaskCollection *pAlias = (pNode != NULL ? pNode->m_value : NULL);
            if (pAlias != NULL && pAlias->_IsStaleAlias())
            {
                m_aliasTable.Delete(pAlias->_OriginalCollection());
                delete pAlias;
                pAlias = NULL;
            }
            return pAlias;
        }

        /// <summary>
        ///     Adds an arbitrary alias (direct or indirect) to the alias table.
        /// </summary>
        void AddArbitraryAlias(_TaskCollection *pOriginCollection, _TaskCollection *pAliasCollection)
        {
            SweepAliasTable();
            m_aliasTable.Insert(pOriginCollection, pAliasCollection);
        }

        /// <summary>
        ///     Sweeps the alias table for stale entries. Anything considered stale is deleted.
        /// </summary>
        void SweepAliasTable();

        /// <summary>
        ///     Clears the alias table.
        /// </summary>
        void ClearAliasTable();

        /// <summary>
        ///     Cancel everything stolen from pCollection outward from this context.
        /// </summary>
        void CancelStealers(_TaskCollectionBase *pCollection);

        /// <summary>
        ///     Returns the highest inlining depth (tree wise) of a canceled task collection. Note that it will return -1
        ///     if there is no in-progress cancellation on the context.
        /// </summary>
        int MinimumCancellationDepth() const
        {
            //
            // If the entire context is canceled, the minimum depth is reported to be zero so as to be less than all inlining depths
            // for the purposes of checking cancellation. Note that even if the top collection has inlining depth of zero, it does not matter
            // since it **IS** the top collection.
            //
            return IsEntireContextCanceled() ? 0 : m_minCancellationDepth;
        }

        // An enumerated type that tells the type of the underlying execution context.
        enum ContextKind
        {
            ExternalContext,
            ThreadContext,
            UMSThreadContext,
            UMSThreadSchedulingUT
        };

        /// <summary>
        ///     Returns the type of context
        /// </summary>
        virtual ContextKind GetContextKind() const = 0;

#if _DEBUG
        // _DEBUG helper
        virtual DWORD GetThreadId() const = 0;
#endif

        /// <summary>
        ///     Returns a bool which can be polled from the current location in lieu of calling is_current_task_group_canceling.
        /// </summary>
        _Beacon_reference *PushCancellationBeacon();

        /// <summary>
        ///     Releases the topmost bool acquired in RAII fashion from PushCancellationBeacon.
        /// </summary>
        void PopCancellationBeacon();

        /// <summary>
        ///     Flags any cancellation beacons that are inlined at or below the specified point.
        /// </summary>
        void FlagCancellationBeacons(int inliningDepth);

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

    protected:
        class ScopedCriticalRegion
        {
        public:
            ScopedCriticalRegion(ContextBase* pCB) : m_pCB(pCB)
            {
                m_pCB->EnterCriticalRegion();
            }

            ~ScopedCriticalRegion()
            {
                m_pCB->ExitCriticalRegion();
            }

        private:
            const ScopedCriticalRegion& operator=(const ScopedCriticalRegion&); //no assigment operator
            ContextBase* m_pCB;
        };

        //
        // Protected data members
        //

        // Entry for freelist
        SLIST_ENTRY m_slNext;

        // Unique identifier
        unsigned int m_id;

        // Critical region counter.
        DWORD m_criticalRegionCount;

        // Hyper-critical region counter.
        DWORD m_hyperCriticalRegionCount;

        // Oversubscription count - the number of outstanding Oversubscribe(true) calls on this context.
        DWORD m_oversubscribeCount;

        // The schedule group segment that the context picked up work from
        ScheduleGroupSegmentBase *m_pSegment;

        // The scheduler instance the context belongs to.
        SchedulerBase *m_pScheduler;

        // Workqueue for unrealized chores.
        WorkQueue *m_pWorkQueue;

        // Link to implement the stack of parent contexts for nested schedulers.
        ContextBase *m_pParentContext;

        // Flag indicating whether the context is blocked.
        volatile LONG m_blockedState;

        // Memory fence to assist Block/Unblock.
        volatile LONG m_contextSwitchingFence;

        // Tracks the task collection from which this context stole (if it's a context executing a stolen chore).
        _TaskCollectionBase *m_pRootCollection;

        // Tracks the task collection currently executing (used to maintain parent/child relationships).
        _TaskCollectionBase *m_pExecutingCollection;

        // Tracks the current cancellation token (for optimization)
        _CancellationTokenState *m_pGoverningTokenState;

        // The inlining depth of the first construct on this stack to utilize the governing token
        int m_governingTokenDepth;

        // The depth of ppltask being inlining scheduled on this context.
        size_t m_asyncTaskCollectionInlineDepth;

        // The thread id for the thread backing the context.
        DWORD m_threadId;

        //
        // Protected methods
        //

        /// <summary>
        ///     Clean up the work queue for this Context.
        /// </summary>
        void ReleaseWorkQueue();

        /// <summary>
        ///     Sets the 'this' context into the tls slot as the current context. This is used by internal contexts in
        ///     their dispatch loops.
        /// </summary>
        void SetAsCurrentTls();

        ///<summary>Send a context ETW event</summary>
        void TraceContextEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD contextId)
        {
            if (g_TraceInfo._IsEnabled(level, ContextEventFlag))
                ThrowContextEvent(eventType, level, schedulerId, contextId);
        }

        static void ThrowContextEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD contextId);

    private:

        //
        // Friend declarations
        //
        friend class SchedulerBase;
        friend class ThreadScheduler;
        friend class UMSThreadScheduler;
        friend class InternalContextBase;
        friend class SchedulingRing;
        friend class VirtualProcessor;
        friend class ScheduleGroupBase;
        friend class ScheduleGroupSegmentBase;
        friend class UMSThreadVirtualProcessor;
        friend class ScheduleGroup;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class _UnrealizedChore;
        friend class _TaskCollection;
        friend class _StructuredTaskCollection;
        friend class _StackGuard;
        friend class UMSSchedulingContext;
        template <class T> friend class LockFreeStack;

        //
        // Private data
        //

        // Used in finalization to distinguish between blocked and free-list contexts
        LONG m_sweeperMarker;

        // Flag indicating context kind.
        bool m_fIsExternal;

        // Keeps track as to whether this context is chained to a context (true) or a schedule group (false) for the purposes of stealing/cancellation.
        bool m_fContextChainedStealer;

        // Indicates that normal lock validations should not be performed -- the context is shutting down a virtual processor.
        bool m_fShutdownValidations;

        // Tracks all contexts which stole from any collection on *this* context.
        SafeRWList<ListEntry> m_stealers;
        // Link for contexts added to m_stealers
        ListEntry m_stealChain;

        // Reference count of things waiting to be added to the steal chain of this context.
        volatile LONG m_cancellationRefCount;

        // Depth is inversely proportion to height in the description of min and max depths. If inlined tg A at depth 0 inlines tg B,
        //    tg A is considered to be higher than tg B on that context.
        // The inlining depth of the highest canceled task collection.
        volatile LONG m_minCancellationDepth;
        // The inlining depth of the lowest canceled task collection.
        volatile LONG m_maxCancellationDepth;

        // The number of task collections and structured task collections running on this context that were canceled when they were inlined
        volatile LONG m_inlineCancellations;
        // An indication that the context was shot down as it stole from a canceled collection.
        volatile LONG m_canceledContext;
        // An indication that there is a pending cancellation of a structured collection on this thread (the collection was canceled before it
        // was inlined).
        volatile LONG m_pendingCancellations;
        // The indirect alias for this context. This allows an unstructured task collection to carry into a stolen chore and be
        // utilized there without any cross threaded semantics within the task collection.
        _TaskCollection *m_pIndirectAlias;
        // The table of aliases for this context. This allows transitive indirect aliases as well as direct aliases (which
        // are not presently implemented).
        Hash<_TaskCollection*, _TaskCollection*> m_aliasTable;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

        //
        // A cancellation beacon is a flag that the runtime signals when cancellation occurs. It allows hot code paths to poll
        // for cancellation in an inlinable way. Parallel for, for instance, gets huge performance wins utilizing this mechanism over
        // a non-inlinable call to is_current_task_group_canceling.
        //
        // Beacons also allow manual raising from the outside for various things in conjunction with cancellation.
        //
        struct CancellationBeacon
        {
            _Beacon_reference m_beacon;
            int m_beaconDepth;

            void Raise()
            {
                InterlockedIncrement(&m_beacon._M_signals);
            }

            void Lower()
            {
                InterlockedDecrement(&m_beacon._M_signals);
            }

            void InternalSignal()
            {
                Raise();
            }
        };

        //
        // In order for the cancellation beacon mechanism to work, we must manage all storage for beacons. A cancellation beacon may disappear
        // asynchronously with respect to cancellation walking the stack of the thread holding the beacon. Once we hand out a beacon flag, we must
        // NOT allow that memory location to become invalid until the runtime owns the base of the stack again.
        //
        // Because of the rules and RAII idiom around this, it does not matter if we hand out the same beacon to a new owner on the same thread.
        //
        // The CancellationBeaconStack manages a growing stack of beacons without reallocations.
        //
        class CancellationBeaconStack
        {
        private:

            // Defines the number of segments in the index
            static const LONG NODE_INDEX_SIZE = 4;

            // Defines how large (and the bitmasks) each segment is.
            static const LONG BEACON_NODE_SIZE = 16;
            static const LONG BEACON_NODE_MASK = 0xFFFFFFF0;
            static const LONG BEACON_NODE_ITEM_MASK = 0xF;
            static const LONG BEACON_NODE_SHIFT = 4;

            //
            // A segment of the stack. Once allocated, data can never move (even after being released).
            //
            struct CancellationBeaconNode
            {
                CancellationBeaconNode() : m_pNext(NULL)
                {
                    m_pBeacons = _concrt_new CancellationBeacon[BEACON_NODE_SIZE];
                }

                ~CancellationBeaconNode()
                {
                    delete[] m_pBeacons;
                }

                CancellationBeacon *m_pBeacons;
                CancellationBeaconNode *m_pNext;
            };

            // The stack pointer
            LONG m_beaconDepth;

            // The size of the stack
            LONG m_size;

            // The index to stack segments
            CancellationBeaconNode **m_pNodeIndex;

            /// <summary>
            ///     Increases the size of the beacon stack without moving any memory associated with beacons that have been handed
            ///     out.
            /// </summary>
            void Grow()
            {
                CancellationBeaconNode *pNewNode;
                CancellationBeaconNode *pPrevNode = NULL;

                LONG idx = (m_size & BEACON_NODE_MASK) >> BEACON_NODE_SHIFT;
                if (idx < NODE_INDEX_SIZE)
                {
                    if (idx > 0)
                        pPrevNode = m_pNodeIndex[idx - 1];

                    pNewNode = m_pNodeIndex[idx] = _concrt_new CancellationBeaconNode;
                }
                else
                {
                    pNewNode = pPrevNode = m_pNodeIndex[NODE_INDEX_SIZE - 1];

                    idx -= (NODE_INDEX_SIZE - 1);
                    while (idx--)
                    {
                        pPrevNode = pNewNode;
                        pNewNode = pNewNode->m_pNext;
                    }

                    ASSERT(pNewNode == NULL);
                    pNewNode = _concrt_new CancellationBeaconNode;
                }

                if (pPrevNode)
                    pPrevNode->m_pNext = pNewNode;

                m_size += BEACON_NODE_SIZE;

            }

        public:

            /// <summary>
            ///     Creates a new cancellation beacon stack.
            /// </summary>
            CancellationBeaconStack() :
                m_beaconDepth(0),
                m_size(0)
            {
                m_pNodeIndex = _concrt_new CancellationBeaconNode* [NODE_INDEX_SIZE];
            }

            /// <summary>
            ///     Destroys a cancellation beacon stack.
            /// </summary>
            ~CancellationBeaconStack()
            {
                if (m_size > 0)
                {
                    CancellationBeaconNode *pNode = m_pNodeIndex[0];
                    while (pNode != NULL)
                    {
                        CancellationBeaconNode *pNext = pNode->m_pNext;
                        delete pNode;
                        pNode = pNext;
                    }
                }

                delete [] m_pNodeIndex;
            }

            /// <summary>
            ///     Acquires memory for a new cancellation beacon and initializes it for the specified inlining depth.
            /// </summary>
            CancellationBeacon *AcquirePushBeacon(int inliningDepth)
            {
                if (m_beaconDepth >= m_size)
                    Grow();

                CancellationBeacon *pBeacon = operator[](m_beaconDepth);
                pBeacon->m_beacon._M_signals = 0;
                pBeacon->m_beaconDepth = inliningDepth;
                m_beaconDepth++;

                //
                // Force a full fence here for any R/W dependencies between the cancellation thread reading the beacon stack and us reading
                // the cancellation state later once the beacon is acquired.
                //
                MemoryBarrier();

                return pBeacon;
            }

            /// <summary>
            ///     Releases the topmost cancellation beacon. The beacon can be handed out
            /// </summary>
            void ReleaseBeacon()
            {
                ASSERT(m_beaconDepth > 0);
                m_beaconDepth--;
            }

            /// <summary>
            ///     Returns a view of the number of beacons.
            /// </summary>
            LONG BeaconCount() const
            {
                return m_beaconDepth;
            }

            /// <summary>
            ///     Returns the cancellation beacon specified as specified by the supplied index. Note that the index must be within
            ///     the bounds of BeaconCount when used externally and m_size when used internally.
            /// </summary>
            CancellationBeacon *operator[](LONG idx)
            {
                CancellationBeaconNode *pNode = NULL;

                LONG nIdx = (idx & BEACON_NODE_MASK) >> BEACON_NODE_SHIFT;
                if (nIdx < NODE_INDEX_SIZE)
                {
                    pNode = m_pNodeIndex[nIdx];
                }
                else
                {
                    pNode = m_pNodeIndex[NODE_INDEX_SIZE - 1];
                    nIdx -= (NODE_INDEX_SIZE - 1);
                    while(nIdx--)
                    {
                        pNode = pNode->m_pNext;
                    }
                }

                return &(pNode->m_pBeacons[idx & BEACON_NODE_ITEM_MASK]);
            }
        };

        CancellationBeaconStack m_cancellationBeacons;

        //
        // Private member functions
        //

        /// <summary>
        ///     When schedulers are nested on the same stack context, the nested scheduler creates a new external context that overrides
        ///     the previous context. PopContextFromTls will restore the previous context by setting the TLS value appropriately.
        /// </summary>
        ContextBase* PopContextFromTls();

        /// <summary>
        ///     When schedulers are nested on the same stack context, the nested scheduler creates a new external context that overrides
        ///     the previous context. PushContextToTls will remember the parent context and set the new context into TLS.
        /// </summary>
        void PushContextToTls(ContextBase* pParentContext);

        /// <summary>
        ///     Context TLS is cleared during nesting on internal contexts before the external context TLS is correctly setup. If not,
        ///     code that executes between the clear and setting the new TLS could get confused.
        /// </summary>
        void ClearContextTls();

        /// <summary>
        ///     Recomputes the maximum depth of cancellation after a canceled task group clears its cancellation flag.
        /// </summary>
        void RecomputeMaximumCancellationDepth();

        /// <summary>
        ///     Returns an indication as to whether a cancellation is occurring at the specified depth. The result here is normally only valid when
        ///     called from the thread representing this context. There are times under the context chaining lock (stealers list) where this can be
        ///     called safely **FOR CERTAIN DEPTHS** from another thread. This variant should never be called directly. Always utilize one of the
        ///     other overloads.
        /// </summary>
        bool IsCanceledAtDepth(_TaskCollectionBase *pStartingCollection, int depth);
    };
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
