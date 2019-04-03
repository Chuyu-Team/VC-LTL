// ==++==
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ContextBase.cpp
//
// Source file containing the implementation for an execution ContextBase/stack/thread.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

#pragma warning (disable : 4702)

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructor
    /// </summary>
    ContextBase::ContextBase(SchedulerBase *pScheduler, bool fIsExternal) :
        m_criticalRegionCount(0),
        m_hyperCriticalRegionCount(0),
        m_oversubscribeCount(0),
        m_pScheduler(pScheduler),
        m_pWorkQueue(NULL),
        m_pParentContext(NULL),
        m_blockedState(CONTEXT_BLOCKED),
        m_contextSwitchingFence(0),
        m_pRootCollection(NULL),
        m_pExecutingCollection(NULL),
        m_pGoverningTokenState(NULL),
        m_governingTokenDepth(-1),
        m_asyncTaskCollectionInlineDepth(0),
        m_threadId(0),
        m_fIsExternal(fIsExternal),
#if defined(_DEBUG)
        m_fShutdownValidations(false),
#endif // _DEBUG
        m_cancellationRefCount(0),
        m_minCancellationDepth(-1),
        m_maxCancellationDepth(-1),
        m_inlineCancellations(0),
        m_canceledContext(0),
        m_pendingCancellations(0),
        m_pIndirectAlias(NULL),
        //
        // The alias table must be sufficiently small that clearing it at the end of a stolen chore isn't a huge penalty, yet
        // large enough to splay a few task collections. Hopefully, the number of collections being utilized in stolen chores isn't very
        // large (1 or 2), so this size should be sufficient.
        //
        m_aliasTable(7)
    {
        m_id = m_pScheduler->GetNewContextId();
        TraceContextEvent(CONCRT_EVENT_START, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);
    }

    unsigned int ContextBase::ScheduleGroupRefCount() const
    {
        return m_pSegment != NULL ? (unsigned int)m_pSegment->GetGroup()->m_refCount : UINT_MAX;
    }

    /// <summary>
    ///     Returns a unique identifier to the context
    /// </summary>
    unsigned int ContextBase::GetId() const
    {
        return m_id;
    }

    /// <summary>
    ///     Returns an identifier to the schedule group the context is currently working on, if any.
    /// </summary>
    unsigned int ContextBase::GetScheduleGroupId() const
    {
        return (m_pSegment != NULL) ? m_pSegment->GetGroup()->Id() : UINT_MAX;
    }

    /// <summary>
    ///     Places a reference on the context preventing it from being destroyed until such time as the stealer is added to the chain
    ///     via AddStealer. Note that the operation of AddStealer should happen rapidly as it will *BLOCK* cleanup of the context.
    /// </summary>
    void ContextBase::ReferenceForCancellation()
    {
        InterlockedIncrement(&m_cancellationRefCount);
    }

    /// <summary>
    ///     Removes a reference on the context which was preventing it from being destroyed.
    /// </summary>
    void ContextBase::DereferenceForCancellation()
    {
        InterlockedDecrement(&m_cancellationRefCount);
    }

    /// <summary>
    ///     Adds a stealing context.
    /// </summary>
    void ContextBase::AddStealer(ContextBase *pStealer, bool fDereferenceForCancellation)
    {
        m_stealers.AddTail(&(pStealer->m_stealChain));
        pStealer->m_fContextChainedStealer = true;
        if (fDereferenceForCancellation)
            DereferenceForCancellation();
    }

    /// <summary>
    ///     Removes a stealing context.
    /// </summary>
    void ContextBase::RemoveStealer(ContextBase *pStealer)
    {
        m_stealers.Remove(&(pStealer->m_stealChain));
    }

    /// <summary>
    ///     Cancel everything stolen from pCanceledCollection outward from this context.
    /// </summary>
    void ContextBase::CancelStealers(_TaskCollectionBase *pCanceledCollection)
    {
        ASSERT(pCanceledCollection != NULL || IsEntireContextCanceled());

        SafeRWList<ListEntry>::_Scoped_lock_read readLock(m_stealers);
        ListEntry *pLE = m_stealers.First();
        while (pLE != NULL)
        {
            ContextBase *pStealingContext = CONTAINING_RECORD(pLE, ContextBase, m_stealChain);

            //
            // We don't want to be recursively traversing the tree needlessly every time the exception propagates back
            // up a given context. If a context is already canceled, nothing can steal from it and we don't need to traverse
            // there.
            //
            if (!pStealingContext->IsEntireContextCanceled())
            {
                _TaskCollectionBase *pRootCollection = pStealingContext->GetRootCollection();
                ASSERT(pRootCollection != NULL);
                //
                // If pCanceledCollection != NULL, it is an indication that we're at the first level. We can only cancel things that are stolen
                // from greater inlining depth or things from equal if the root collection is pCollection. Further, we cannot cancel things which are not
                // inlined. For example:
                //
                // _TaskCollection p1;
                // p1.Schedule( [] {
                //      _TaskCollection *p2 = new _TaskCollection;
                //      p2.Schedule(alpha);
                //      _TaskCollection p3;
                //      p3.Schedule( [] {
                //          Blah;
                //          });
                //      });
                //
                // A cancel of p1 while p1->p3 is running inline cannot cancel p2. The exception that backflows might indeed cancel p2 if it was stack
                // based, but remember we can have task collection pointers which are passed amongst threads and detached.
                //
                // Keep in mind that it's entirely possible to have a situation similar to above during the recursion where one of the stolen chores declared
                // a task collection and pushed chores that will not be waited upon but instead will be passed out to another thread. We cannot tear down contexts
                // that stole in this manner either.
                //

                if (
                    // A context whose root chore belongs to the task collection being canceled is fair game. No further checks are required.
                    (pRootCollection == pCanceledCollection) ||

                    // On recursion, as long as the root collection is inlined (no matter the depth), we are safe to cancel as it was inlined on a canceled
                    // context and that by definition gives it the correct parentage to be shot down.
                    (pCanceledCollection == NULL && pRootCollection->_IsCurrentlyInlined()) ||

                    // The only way cancellation can be satisfied if both aren't inlined is above. Otherwise, the one that stole must have greater
                    // inline depth than the one we're canceling.
                    (pCanceledCollection != NULL && pCanceledCollection->_IsCurrentlyInlined() && pRootCollection->_InliningDepth() > pCanceledCollection->_InliningDepth())
                   )
                {
                    ASSERT(pRootCollection == pCanceledCollection || pCanceledCollection == NULL || pRootCollection->_IsCurrentlyInlined());
                    //
                    // We must verify that it is okay to cancel the stealer based on any tokens which are present on 'this' context. We are further guaranteed
                    // stability on the inlining depth because of the lock on the stealers list.
                    //
                    bool fCancel = true;
                    if (pRootCollection != pCanceledCollection && m_governingTokenDepth != -1)
                    {
                        ASSERT(pRootCollection->_IsCurrentlyInlined() && m_pGoverningTokenState != NULL);
                        fCancel = IsCanceledAtDepth(pRootCollection);
                    }

                    if (fCancel)
                    {
                        pStealingContext->CancelEntireContext();
                        pStealingContext->CancelStealers(NULL);
                    }
                }
            }

            pLE = m_stealers.Next(pLE);
        }
    }

    /// <summary>
    ///     Cleans up the context.
    /// </summary>
    void ContextBase::Cleanup()
    {
        ReleaseWorkQueue();

        TraceContextEvent(CONCRT_EVENT_END, TRACE_LEVEL_INFORMATION, m_pScheduler->Id(), m_id);
    }

    /// <summary>
    ///     Called on both internal and external contexts, either when the are put into an idle pool to
    ///     be recycled, or when they are ready to be deleted. The API moves the contexts that are in
    ///     the list of 'stealers' (used for cancellation) to lists in the task collections from which
    ///     those contexts have stolen chores.
    /// </summary>
    void ContextBase::DetachStealers()
    {
        //
        // Make sure no one has a ref on us to add to the stealers list. We need to wait on that before running down the cancellation list.
        // Note that waiting here should be *EXTREMELY RARE*. The only time we'd ever see it would be if a task collection was used between threads and
        // and between the time of the steal and the time the wrapper executed the original thread went away.
        //

        if (m_cancellationRefCount != 0)
        {
            // Spin wait (no yielding)
            _SpinWaitNoYield spinWait;

            do
            {
                spinWait._SpinOnce();

            } while (m_cancellationRefCount != 0);
        }

        if (m_aliasTable.Count() > 0)
            ClearAliasTable();

        if (m_stealers.Empty())
        {
            //
            // After a DetachStealers, it is entirely possible that the context (the *this*) pointer goes away. Normally, the lock on the stealers
            // list is what guards against manipulation by stolen chores; however -- the early exit above presents an interesting risk. It is now entirely
            // possible that the last stolen chore is removing its context from the stealers list under the governance of the write lock and makes the
            // list empty. The detachment wants to bail due to the above check (there's nothing there) and the context pointer is freed before the stealing
            // thread releases the write lock.
            //
            // We do want the early bail to avoid taking and releasing a reader/writer frequently in this case for scenarios like parallel for. In order to
            // prevent touching freed memory, we need to flush out any write owner (take and release the lock if someone holds a write).
            //
            m_stealers.FlushWriteOwners();
            return;
        }

        //
        // If there is anything left on the stealers list, it means that a context is dying while a task collection bound to that context lives
        // on and still has stolen chores. In order to continue to facilitate cancellation of those task collections, any stealers in the list have
        // to be moved to the individual task collection lists.
        //
        bool isDone = false;

        while(!isDone)
        {
            bool fContinue = true;
            m_stealers.AcquireWrite();
            __try
            {
                fContinue = true;
                ListEntry *pEntry = m_stealers.First();
                while (pEntry != NULL && fContinue)
                {
                    ListEntry *pNext = m_stealers.Next(pEntry);

                    ContextBase *pContext = CONTAINING_RECORD(pEntry, ContextBase, m_stealChain);

                    _TaskCollectionBase *pCollectionBase = pContext->GetRootCollection();
                    ASSERT(pCollectionBase != NULL && !pCollectionBase->_IsStructured());

                    _TaskCollection *pCollection = static_cast<_TaskCollection *>(pCollectionBase);

                    //
                    // In all likelihood, we rarely get here; however -- there is an issue in that the lock ordering here is from the bottom up
                    // (task collection then context) in order to preserve patterns in stealing and cancellation.
                    //
                    // When we move, we must do so in a backwards order. The only time we should see contention on these locks is during minimal
                    // periods where we are cancelling or for tiny time frames during steal. We will play a pseudo-atomic lock acquire game. If we cannot
                    // get both, we back off and let the other thread through.
                    //
                    SafeRWList<ListEntry> *pCollectionList = reinterpret_cast<SafeRWList<ListEntry> *> (pCollection->_GetStealTrackingList());
                    if (!pCollectionList->TryAcquireWrite())
                    {
                        //
                        // Yield in an attempt to force the other thread through.
                        //
                        m_stealers.ReleaseWrite();
                        fContinue = false;
                        platform::__Sleep(1);
                        break;
                    }

                    __try
                    {
                        m_stealers.UnlockedRemove(&(pContext->m_stealChain));
                        pContext->m_fContextChainedStealer = false;
                        pCollectionList->UnlockedAddTail(&(pContext->m_stealChain));
                    }
                    __finally
                    {
                        pCollectionList->ReleaseWrite();
                    }

                    pEntry = pNext;
                }

                isDone = (pEntry == NULL);
            }
            __finally
            {
                //
                // It may have been released due to a back-off.
                //
                if (fContinue)
                {
                    m_stealers.ReleaseWrite();
                }
            }
        }
    }

    /// <summary>
    ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
    /// </summary>
    /// <param name="pChore">
    ///     The chore to push onto the structured work stealing queue.
    /// </param>
    void ContextBase::PushStructured(_UnrealizedChore *pChore, location *pLocation)
    {
#if _CHORETRACK
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, this, this, NULL, pChore);
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, m_pScheduler, this, NULL, pChore);
#endif // _CHORETRACK

        Mailbox<_UnrealizedChore>::Slot affinitySlot;

        // If the chore has been scheduled with a location and the scheduler supports location-based scheduling, the destination schedule
        // group segment may be different from the current one.
        ScheduleGroupSegmentBase * pDestinationSegment = m_pSegment;

        if (pLocation != NULL)
        {
            //
            // If the current segment this context is operating within has the same affinity as the requested task, there is *NO NEED* to mail
            // the task anywhere. It will get a natural affinity to pLocation without any additional work.
            //
            if (!pLocation->_Is_system())
            {
                if (*pLocation != m_pSegment->GetAffinity())
                {
                    affinitySlot = m_pSegment->GetGroup()->MailChore(pChore, pLocation, &pDestinationSegment);
                }

                pDestinationSegment->NotifyAffinitizedWork();
            }
        }

        GetStructuredWorkQueue()->PushStructured(pChore, affinitySlot);

        //
        // Update the enqueued task numbers for statistics. Since this is a critical performance
        // path we avoid making a virtual call since that will imply two memory dereferences plus
        // an indirect call. Instead, we make one memory dereference to get a condition and one
        // branch. This is faster ONLY because target function call will be inlined.
        //
        if (IsExternal())
        {
            static_cast<ExternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }
        else
        {
            static_cast<InternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }

        if (m_pScheduler->HasVirtualProcessorAvailableForNewWork())
        {
            m_pScheduler->StartupNewVirtualProcessor(pDestinationSegment, pDestinationSegment->GetAffinity());
        }
    }

    /// <summary>
    ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
    /// </summary>
    /// <param name="pChore">
    ///     The chore to push onto the structured work stealing queue.
    /// </param>
    void ContextBase::PushStructured(_UnrealizedChore *pChore)
    {
#if _CHORETRACK
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, this, this, NULL, pChore);
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, m_pScheduler, this, NULL, pChore);
#endif // _CHORETRACK

        GetStructuredWorkQueue()->PushStructured(pChore);

        //
        // Update the enqueued task numbers for statistics. Since this is a critical performance
        // path we avoid making a virtual call since that will imply two memory dereferences plus
        // an indirect call. Instead, we make one memory dereference to get a condition and one
        // branch. This is faster ONLY because target function call will be inlined.
        //
        if (IsExternal())
        {
            static_cast<ExternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }
        else
        {
            static_cast<InternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }

        if (m_pScheduler->HasVirtualProcessorAvailableForNewWork())
        {
            m_pScheduler->StartupNewVirtualProcessor(m_pSegment);
        }
    }

    /// <summary>
    ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.
    /// </summary>
    /// <param name="pChore">
    ///     The chore to push onto the unstructured work stealing queue.
    /// </param>
    int ContextBase::PushUnstructured(_UnrealizedChore *pChore, location *pLocation)
    {
#if _CHORETRACK
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, this, this, NULL, pChore);
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, m_pScheduler, this, NULL, pChore);
#endif // _CHORETRACK

        Mailbox<_UnrealizedChore>::Slot affinitySlot;

        // If the chore has been scheduled with a location and the scheduler supports location-based scheduling, the destination schedule
        // group segment may be different from the current one.
        ScheduleGroupSegmentBase * pDestinationSegment = m_pSegment;

        if (pLocation != NULL)
        {
            //
            // If the current segment this context is operating within has the same affinity as the requested task, there is *NO NEED* to mail
            // the task anywhere. It will get a natural affinity to pLocation without any additional work.
            //
            if (!pLocation->_Is_system())
            {
                if (*pLocation != m_pSegment->GetAffinity())
                {
                    affinitySlot = m_pSegment->GetGroup()->MailChore(pChore, pLocation, &pDestinationSegment);
                }

                pDestinationSegment->NotifyAffinitizedWork();
            }
        }

        int cookie = GetWorkQueue()->PushUnstructured(pChore, affinitySlot);

        //
        // Update the enqueued task numbers for statistics. Since this is a critical performance
        // path we avoid making a virtual call since that will imply two memory dereferences plus
        // an indirect call. Instead, we make one memory dereference to get a condition and one
        // branch. This is faster ONLY because target function call will be inlined.
        //
        if (IsExternal())
        {
            static_cast<ExternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }
        else
        {
            static_cast<InternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }

        if (m_pScheduler->HasVirtualProcessorAvailableForNewWork())
        {
            m_pScheduler->StartupNewVirtualProcessor(pDestinationSegment, pDestinationSegment->GetAffinity());
        }

        return cookie;
    }

    /// <summary>
    ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.
    /// </summary>
    /// <param name="pChore">
    ///     The chore to push onto the unstructured work stealing queue.
    /// </param>
    int ContextBase::PushUnstructured(_UnrealizedChore *pChore)
    {
#if _CHORETRACK
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, this, this, NULL, pChore);
        OMTRACE(MTRACE_EVT_ADDUNREALIZED, m_pScheduler, this, NULL, pChore);
#endif // _CHORETRACK

        int cookie = GetWorkQueue()->PushUnstructured(pChore);

        //
        // Update the enqueued task numbers for statistics. Since this is a critical performance
        // path we avoid making a virtual call since that will imply two memory dereferences plus
        // an indirect call. Instead, we make one memory dereference to get a condition and one
        // branch. This is faster ONLY because target function call will be inlined.
        //
        if (IsExternal())
        {
            static_cast<ExternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }
        else
        {
            static_cast<InternalContextBase *>(this)->IncrementEnqueuedTaskCounter();
        }

        if (m_pScheduler->HasVirtualProcessorAvailableForNewWork())
        {
            m_pScheduler->StartupNewVirtualProcessor(m_pSegment);
        }

        return cookie;
    }

    /// <summary>
    ///     Pops the topmost chore from the work stealing queue for structured parallelism. Failure
    ///     to pop typically indicates stealing.
    /// </summary>
    /// <returns>
    ///     An unrealized chore from the structured work stealing queue or NULL if none is present.
    /// </returns>
    _UnrealizedChore *ContextBase::PopStructured()
    {
        ASSERT(m_pWorkQueue != NULL);
        _UnrealizedChore *pChore = m_pWorkQueue->PopStructured();

#if (_CHORETRACK && _UMSTRACE)
        if (pChore)
        {
            OMTRACE(MTRACE_EVT_POPUNREALIZED, this, this, NULL, pChore);
            OMTRACE(MTRACE_EVT_POPUNREALIZED, m_pScheduler, this, NULL, pChore);
        }
#endif // _CHORETRACK

        return pChore;
    }

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
    ///     the work stealing queue
    /// </returns>
    _UnrealizedChore *ContextBase::TryPopUnstructured(int cookie)
    {
        ASSERT(m_pWorkQueue != NULL);
        _UnrealizedChore *pChore = m_pWorkQueue->TryPopUnstructured(cookie);

#if (_CHORETRACK && _UMSTRACE)
        if (pChore)
        {
            OMTRACE(MTRACE_EVT_POPUNREALIZED, this, this, NULL, pChore);
            OMTRACE(MTRACE_EVT_POPUNREALIZED, m_pScheduler, this, NULL, pChore);
        }
#endif // _CHORETRACK

        return pChore;
    }

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
    void ContextBase::SweepUnstructured(WorkStealingQueue<_UnrealizedChore>::SweepPredicate pPredicate,
                                        void *pData,
                                        WorkStealingQueue<_UnrealizedChore>::SweepFunction pSweepFn
                                        )
    {
        ASSERT(m_pWorkQueue != NULL);
        return m_pWorkQueue->SweepUnstructured(pPredicate, pData, pSweepFn);
    }

    /// <summary>
    ///     Create a workqueue for use in unstructured task collections.
    /// </summary>
    void ContextBase::CreateWorkQueue()
    {
        //
        // First try and reuse a detached workqueue.
        //
        m_pWorkQueue = m_pSegment->GetDetachedWorkQueue();
        //
        // A detached work queue is still on m_pGroup->m_workQueues.
        //
        if (m_pWorkQueue == NULL)
        {
            //
            // If that failed, try and reuse a workqueue from the free pool.
            //
            m_pWorkQueue = m_pSegment->m_workQueues.PullFromFreePool();

            if (m_pWorkQueue == NULL)
            {
                //
                // Must create a new one.
                //
                m_pWorkQueue = _concrt_new WorkQueue();
            }
            else
            {
                //
                // Reinitialize the work queue from the free pool.
                //
                m_pWorkQueue->Reinitialize();
            }

            m_pSegment->m_workQueues.Add(m_pWorkQueue);
        }

        ASSERT(m_pWorkQueue != NULL);
        m_pWorkQueue->SetOwningContext(this);
    }

    /// <summary>
    ///     Create a workqueue for use in structured task collections.
    /// </summary>
    void ContextBase::CreateStructuredWorkQueue()
    {
        //
        // First, try and reuse a workqueue from the free pool.
        // When using structured task collections, quite often there are
        // no previous unstructured task collections that neglected to wait (thus generating detached workqueues).
        //
        m_pWorkQueue = m_pSegment->m_workQueues.PullFromFreePool();

        if (m_pWorkQueue == NULL)
        {
            //
            // If that failed, see if there is a workqueue on the detachedWorkQueues list to reuse.
            //
            m_pWorkQueue = m_pSegment->GetDetachedWorkQueue();

            //
            // A detached work queue is still on m_pSegment->m_workQueues.
            //
            if (m_pWorkQueue == NULL)
            {
                m_pWorkQueue = _concrt_new WorkQueue();
                m_pSegment->m_workQueues.Add(m_pWorkQueue);
            }
        }
        else
        {
            //
            // Reinitialize the work queue from the free pool.
            //
            m_pWorkQueue->Reinitialize();
            m_pSegment->m_workQueues.Add(m_pWorkQueue);
        }

        ASSERT(m_pWorkQueue != NULL);
        m_pWorkQueue->SetOwningContext(this);
    }

    /// <summary>
    ///     Cleans up the internal workqueue.
    /// </summary>
    void ContextBase::ReleaseWorkQueue()
    {
        if (m_pWorkQueue != NULL)
        {
            //
            // It's entirely possible that this particular work queue had chores left on the unstructured work queue.
            // Someone could create an unstructured task collection within an LWT, queue chores, and subsequently pass
            // the collection out of the LWT to be waited upon later. In this case, we must leave the work queue around
            // in order for stealing to appropriately happen. This work queue will not be dechained from the schedule
            // group, but will remain until empty. It will go on a lookaside and, while in this state, can be handed
            // to some new context working on an item within the same schedule group.
            //

            // Save off a local copy of the workqueue and work with that. The debugger mines the workqueue information
            // held in this context, and if we remove the work queue while it's still pointed at by this context, the
            // debugger can become confused.
            WorkQueue* workQueue = m_pWorkQueue;
            m_pWorkQueue = NULL;

            if ( !workQueue->IsUnstructuredEmpty())
            {
                workQueue->LockedSetOwningContext(NULL);
                m_pSegment->DetachActiveWorkQueue(workQueue);
            }
            else
            {
                //
                // Unless someone really side-stepped the intent of _StructuredTaskCollection, it's almost certain that
                // workQueue->IsStructuredEmpty() is true or else a missing_wait was already thrown.
                //
                if (workQueue->IsLockHeld())
                {
                    // Somebody is stealing, don't want to NULL out owning ctx until they're done.
                    workQueue->LockedSetOwningContext(NULL);
                }
                else
                {
                    // We know workQueue has no unstructured, since we're on the owning thread.
                    // Moreover, structured must be empty at this point, because we cannot ever get here until the wait is satisfied.
                    // If the UnlockedSteal is entered, then we'll early exit w/o ever touching the owning ctx of workQueue.
                    workQueue->SetOwningContext(NULL);
                }
                m_pSegment->m_workQueues.Remove(workQueue);
            }
        }

        //
        // Make sure that any detachment triggers the stealers to move into the task collection list. Otherwise, we can wind up with
        // an A<-B<-A stealing pattern:
        //
        // TC 1 on thread A
        //   Thread B steals from TC 1 (A<-B)
        //   Thread A detaches (no wait on TC1)
        //   Thread A does SFW and steals from TC 2 deeper inline on thread B (B<-A)
        //
        // The overall stealers pattern is A<-B<-A which will wind up with lock traversal in this order. The recursive reacquire of
        // R/W lock (or out of order acquire: A<-B on one thread, B<-A on the other) will result in later deadlock.
        //
        DetachStealers();
    }

    /// <summary>
    ///     Sets the 'this' context into the tls slot as the current context. This is used by internal contexts in
    ///     their dispatch loops.
    /// </summary>
    void ContextBase::SetAsCurrentTls()
    {
        platform::__TlsSetValue(SchedulerBase::t_dwContextIndex, this);
    }

    /// <summary>
    ///     When schedulers are nested on the same thread, the nested scheduler creates a new external context that overrides
    ///     the previous context. PopContextFromTls will restore the previous context by setting the TLS value appropriately.
    /// </summary>
    ContextBase* ContextBase::PopContextFromTls()
    {
        ContextBase* pPreviousContext = m_pParentContext;
        platform::__TlsSetValue(SchedulerBase::t_dwContextIndex, pPreviousContext);
        m_pParentContext = NULL;
        return pPreviousContext;
    }

    /// <summary>
    ///     When schedulers are nested on the same thread, the nested scheduler creates a new external context that overrides
    ///     the previous context. PushContextToTls will store the previous context and set the new context into TLS.
    /// </summary>
    void ContextBase::PushContextToTls(ContextBase* pParentContext)
    {
        m_pParentContext = pParentContext;

        // For the first context on a thread, we expect the TLS values to be null. If there is a parent context,
        // the TLS value should have been cleared right before nesting.
        ASSERT(platform::__TlsGetValue(SchedulerBase::t_dwContextIndex) == NULL);
        platform::__TlsSetValue(SchedulerBase::t_dwContextIndex, this);
    }

    /// <summary>
    ///     Context TLS is cleared during nesting on internal contexts before the external context TLS is correctly setup. If not,
    ///     code that executes between the clear and setting the new TLS could get confused.
    /// </summary>
    void ContextBase::ClearContextTls()
    {
        ASSERT(platform::__TlsGetValue(SchedulerBase::t_dwContextIndex) != NULL);
        platform::__TlsSetValue(SchedulerBase::t_dwContextIndex, NULL);
    }

    /// <summary>
    ///     Returns the scheduler the specified context is associated with.
    /// </summary>
    SchedulerBase* ContextBase::GetScheduler() const
    {
        return m_pScheduler;
    }

    /// <summary>
    ///     Returns the schedule group the specified context is associated with.
    /// </summary>
    ScheduleGroupBase* ContextBase::GetScheduleGroup() const
    {
        return m_pSegment != NULL ? m_pSegment->GetGroup() : NULL;
    }

    /// <summary>
    ///     Returns the schedule group the specified context is associated with.
    /// </summary>
    ScheduleGroupSegmentBase* ContextBase::GetScheduleGroupSegment() const
    {
        return m_pSegment;
    }

    /// <summary>
    ///     Gets the indirect alias.
    /// </summary>
    _TaskCollection *ContextBase::GetIndirectAlias() const
    {
        return m_pIndirectAlias;
    }

    /// <summary>
    ///     Sets the indirect alias.
    /// </summary>
    void ContextBase::SetIndirectAlias(_TaskCollection *pAlias)
    {
        m_pIndirectAlias = pAlias;
    }

    /// <summary>
    ///     Sweeps the alias table removing anything that's marked for delete. This is done every time we create a new direct alias
    ///     in order to avoid growing the table arbitrarily for a context which isn't going away. Note -- passing a task collection between
    ///     threads is expensive the first time it's used.
    /// </summary>
    void ContextBase::SweepAliasTable()
    {
        int x;
        Hash<_TaskCollection*, _TaskCollection*>::ListNode *pNode = m_aliasTable.First(&x);
        while (pNode != NULL)
        {
            Hash<_TaskCollection*, _TaskCollection*>::ListNode *pNextNode = m_aliasTable.Next(&x, pNode);

            if (pNode->m_value->_IsStaleAlias())
            {
                _TaskCollection *pCollection = pNode->m_value;
                m_aliasTable.Delete(pCollection->_OriginalCollection()); // may delete pNode
                delete pCollection;
            }

            pNode = pNextNode;
        }
    }

    /// <summary>
    ///     Clears the alias table.
    /// </summary>
    void ContextBase::ClearAliasTable()
    {
        int x;
        Hash<_TaskCollection*, _TaskCollection*>::ListNode *pNode = m_aliasTable.First(&x);
        while (pNode != NULL)
        {
            pNode->m_value->_ReleaseAlias();
            pNode = m_aliasTable.Next(&x, pNode);
        }
        m_aliasTable.Wipe();
    }

    /// <summary>
    ///     Sets the cancellation token currently governing this context.
    /// </summary>
    void ContextBase::PushGoverningTokenState(_CancellationTokenState *pTokenState, int inliningDepth)
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        m_pGoverningTokenState = pTokenState;
        m_governingTokenDepth = inliningDepth;
    }

    /// <summary>
    ///     Reverts to the previously set cancellation token.
    /// </summary>
    void ContextBase::PopGoverningTokenState(_CancellationTokenState *pTokenState)
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this);
        ASSERT(m_pGoverningTokenState == pTokenState);
        ASSERT(m_pExecutingCollection->_InliningDepth() == m_governingTokenDepth);

        // Move back up to find the parent. Even if the parent has the same token, we need to change the
        // governing token depth to *its* inlining depth
        _TaskCollectionBase *pCollection = m_pExecutingCollection->_SafeGetParent();

        while (pCollection != NULL && pCollection != m_pRootCollection && pCollection->_GetTokenState() == NULL)
        {
            pCollection = pCollection->_SafeGetParent();
        }
        //
        // We only keep governing tokens for THIS context.
        //
        if (pCollection != NULL && pCollection != m_pRootCollection)
        {
            ASSERT(pCollection->_GetTokenState() != NULL && pCollection->_InliningDepth() != -1);
            m_pGoverningTokenState = pCollection->_GetTokenState();
            m_governingTokenDepth = pCollection->_M_inliningDepth;
        }
        else
        {
            m_pGoverningTokenState = NULL;
            m_governingTokenDepth = -1;
        }
    }

    /// <summary>
    ///     Called in order to indicate that a collection executing on this context was canceled. This will often cause cancellation
    ///     and unwinding of the entire context (up to the point where we get to the canceled collection). This method is paired with
    ///     CancelCollectionComplete.
    ///     NOTE: Callers of CancelCollection must first guarantee through other means that the collection they're cancelling (the one at the
    ///     depth by the argument) will have a stable inlining depth through the duration of the CancelCollection call.
    ///         *   For structured task collections, since cancel is only allowed to be called by the owning context or within a stolen chore, if an
    ///     inlining depth greater than zero is observed, it is stable since the owning thread will have to wait until the chore invoking CancelCollection
    ///     completes.
    ///         *   For general task collections, cancel is allowed from arbitrary threads. If the calling thread is an indirect alias, the inlining
    ///     depth will be stable if observed to be greater than 0 (because CancelCollection is executing inside a stolen chore). Alternatively the thread can use
    ///     a CAS based state lock (see _TaskCollection::_CancelFromArbitraryThread) to ensure that inlining depth is stable.
    /// </summary>
    void ContextBase::CancelCollection(int inliningDepth)
    {
        InterlockedIncrement(&m_inlineCancellations);

        long curDepth = m_minCancellationDepth;
        //
        // Keep track of the minimum cancellation depth.
        //
        for(;;)
        {
            if (curDepth != -1 && inliningDepth > curDepth)
                break;

            long xchgDepth = InterlockedCompareExchange(&m_minCancellationDepth, inliningDepth, curDepth);
            if (xchgDepth == curDepth)
            {
                //
                // Cancellation beacons are a bit different. If the entire context was canceled due to a steal, we flag top level cancellation
                // beacons even though they are not considered to have inlining depth since the caller might not have been inlined.
                //
                FlagCancellationBeacons(IsEntireContextCanceled() ? -1 : inliningDepth);
                break;
            }
            curDepth = xchgDepth;
        }

        long curMaxDepth = m_maxCancellationDepth;
        //
        // Keep track of the maximum cancellation depth
        //
        for(;;)
        {
            if (curMaxDepth != -1 && inliningDepth < curMaxDepth)
                break;

            long xchgDepth = InterlockedCompareExchange(&m_maxCancellationDepth, inliningDepth, curMaxDepth);
            if (xchgDepth == curMaxDepth)
            {
                break;
            }
            curMaxDepth = xchgDepth;
        }
    }

    /// <summary>
    ///     Recomputes the maximum depth of cancellation after a canceled task group clears its cancellation flag.
    /// </summary>
    void ContextBase::RecomputeMaximumCancellationDepth()
    {
        //
        // Before doing the recompute, we **MUST** reset to uninitialized to avoid a race between someone setting this in ::CancelCollection and
        // someone doing a recompute across a boundary.
        //
        InterlockedExchange(&m_maxCancellationDepth, -1);

        long computedMaximumDepth = IsEntireContextCanceled() ? ENTIRE_CONTEXT_CANCELED : -1;

        _TaskCollectionBase *pCollection = m_pExecutingCollection;
        while (pCollection != NULL && pCollection != m_pRootCollection)
        {
            if ((pCollection->_IsStructured() && (static_cast<_StructuredTaskCollection *>(pCollection))->_IsMarkedForCancellation()) ||
                (!pCollection->_IsStructured() && (static_cast<_TaskCollection *>(pCollection))->_IsMarkedForAbnormalExit()))
            {
                computedMaximumDepth = pCollection->_M_inliningDepth;
                break;
            }

            pCollection = pCollection->_SafeGetParent();
        }

        long curMaxDepth = -1;

        //
        // Keep track of the maximum cancellation depth
        //
        for(;;)
        {
            if (curMaxDepth != -1 && computedMaximumDepth < curMaxDepth)
                break;

            long xchgDepth = InterlockedCompareExchange(&m_maxCancellationDepth, computedMaximumDepth, curMaxDepth);
            if (xchgDepth == curMaxDepth)
            {
                break;
            }

            curMaxDepth = xchgDepth;
        }
    }

    /// <summary>
    ///     When a cancellation bubbles up to the collection being canceled, this function is called in order to stop propagation of
    ///     the cancellation further up the work tree. This method is paired with CancelCollection.
    /// </summary>
    bool ContextBase::CancelCollectionComplete(int inliningDepth)
    {
        ASSERT(m_inlineCancellations > 0);

        //
        // Keep track of minimum/maximum cancellation depth.
        //
        InterlockedCompareExchange(&m_minCancellationDepth, -1, inliningDepth);
        RecomputeMaximumCancellationDepth();

        return (InterlockedDecrement(&m_inlineCancellations) == 0);
    }

    /// <summary>
    ///     Send a context ETW event.
    /// </summary>
    void ContextBase::ThrowContextEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD contextId)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Class.Type = (UCHAR)eventType;
            concrtHeader.header.Class.Level = level;
            concrtHeader.header.Guid = ContextEventGuid;

            concrtHeader.SchedulerID = schedulerId;
            concrtHeader.ContextID = contextId;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }

    /// <summary>
    ///     Enters a critical region of the scheduler. Calling this guarantees that the virtual processor on which this context lives
    ///     is guaranteed to be stable throughout the critical region. For some context types, this is virtually a NOP. For others
    ///     (UMS), this makes it appear that blocking on the context actually blocks the UMS thread instead of triggering return to
    ///     primary. Note that critical regions suppress asynchronous blocking but not synchronous blocking.
    /// </summary>
    int ContextBase::EnterCriticalRegion()
    {
        if (m_pScheduler->IsUMSScheduler())
        {
            return EnterCriticalRegionHelper();
        }

        return 0;
    }

    /// <summary>
    ///     Exits a critical region of the scheduler.
    /// </summary>
    int ContextBase::ExitCriticalRegion()
    {
        if (m_pScheduler->IsUMSScheduler())
        {
            return ExitCriticalRegionHelper();
        }

        return 0;
    }

    /// <summary>
    ///     Enters a hyper-critical region of the scheduler. Calling this guarantees not only the conditions of a critical region but it
    ///     guarantees that synchronous blocking is suppressed as well. This allows for lock sharing between the primary and hyper-critical
    ///     regions running on UTs. No lock sharing can occur between the inside of this region type and the outside of this region type
    ///     on a UT.
    /// </summary>
    int ContextBase::EnterHyperCriticalRegion()
    {
        if (m_pScheduler->IsUMSScheduler())
        {
            return EnterHyperCriticalRegionHelper();
        }

        return 0;
    }

    /// <summary>
    ///     Exits a hyper-critical region of the scheduler.
    /// </summary>
    int ContextBase::ExitHyperCriticalRegion()
    {
        if (m_pScheduler->IsUMSScheduler())
        {
            return ExitHyperCriticalRegionHelper();
        }

        return 0;
    }

    /// <summary>
    ///    Static version of EnterCriticalRegion.
    /// </summary>
    void ContextBase::StaticEnterCriticalRegion()
    {
        ContextBase *pContext = SchedulerBase::FastCurrentContext();
        if (pContext != NULL)
            pContext->EnterCriticalRegion();
    }

    /// <summary>
    ///    Static version of EnterHyperCriticalRegion.
    /// </summary>
    void ContextBase::StaticEnterHyperCriticalRegion()
    {
        ContextBase *pContext = SchedulerBase::FastCurrentContext();
        if (pContext != NULL)
            pContext->EnterHyperCriticalRegion();
    }

    /// <summary>
    ///    Static version of ExitCriticalRegion.
    /// </summary>
    void ContextBase::StaticExitCriticalRegion()
    {
        ContextBase *pContext = SchedulerBase::FastCurrentContext();
        if (pContext != NULL)
            pContext->ExitCriticalRegion();
    }

    /// <summary>
    ///    Static version of ExitHyperCriticalRegion.
    /// </summary>
    void ContextBase::StaticExitHyperCriticalRegion()
    {
        ContextBase *pContext = SchedulerBase::FastCurrentContext();
        if (pContext != NULL)
            pContext->ExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Static version of GetCriticalRegionType.
    /// </summary>
    CriticalRegionType ContextBase::StaticGetCriticalRegionType()
    {
        ContextBase *pContext = SchedulerBase::FastCurrentContext();
        if (pContext != NULL)
            return pContext->GetCriticalRegionType();
        return OutsideCriticalRegion;
    }

    /// <summary>
    ///     Since critical region counts are turned off for thread schedulers, this method is used
    ///     where the return value is expected to be true. For a thread scheduler, it always returns true.
    ///     For a UMS scheduler it returns (GetCriticalRegionType() != OutsideCriticalRegion).
    /// </summary>
    bool ContextBase::IsInsideCriticalRegion() const
    {
        if (m_pScheduler->IsUMSScheduler())
            return (GetCriticalRegionType() != OutsideCriticalRegion);
        return true;
    }

    /// <summary>
    ///     Returns a bool which can be polled from the current location in lieu of calling is_current_task_group_canceling.
    /// </summary>
    _Beacon_reference *ContextBase::PushCancellationBeacon()
    {
        int inliningDepth = m_pExecutingCollection ? m_pExecutingCollection->_InliningDepth() : -1;

        CancellationBeacon *pBeacon = m_cancellationBeacons.AcquirePushBeacon(inliningDepth);

        //
        // AcquirePushBeacon has a full fence to guard R/W ordering here.
        //
        if (IsEntireContextCanceled() || (m_minCancellationDepth != -1 && m_minCancellationDepth <= pBeacon->m_beaconDepth))
            pBeacon->InternalSignal();

        return &(pBeacon->m_beacon);
    }

    /// <summary>
    ///     Releases the topmost bool acquired in RAII fashion from PushCancellationBeacon.
    /// </summary>
    void ContextBase::PopCancellationBeacon()
    {
        m_cancellationBeacons.ReleaseBeacon();
    }

    /// <summary>
    ///     Flags any cancellation beacons that are inlined at or below the specified point.
    /// </summary>
    void ContextBase::FlagCancellationBeacons(int inliningDepth)
    {
        LONG snapSize = m_cancellationBeacons.BeaconCount();
        for (LONG i = 0; i < snapSize; i++)
        {
            //
            // The beacon list is guaranteed to exist. Further, because we do this during cancellation for inlined collections, we can
            // never unpop and reuse a beacon for a lower depth until the cancellation is complete because of strict nesting on the
            // beacon stack (RAII).
            //
            CancellationBeacon *pBeacon = m_cancellationBeacons[i];
            if (pBeacon->m_beaconDepth >= inliningDepth)
            {
                //
                // We have one interesting conundrum here. Everything from depth 0 -> inliningDepth is guaranteed to be stable. Anything from
                // inliningDepth + 1 -> N can change. That change might include what cancellation tokens are active. This might, in fact, change
                // whether a cancellation has truly happened or not.
                //
                // In order to solve this, two things will happen:
                //
                //     - Whoever observes a beacon signaled must do a further check to CONFIRM the cancellation.
                //
                //     - If there is a guarantee that the cancellation will not hit us in THIS call, we will not flag the beacon as a performance
                //       optimization.
                //
                // This effectively means that we may see a **false positive** on the beacon but never a false negative due to the tokens. A false positive
                // can be double checked. A false negative will never be flagged again and will lead to uncancellable trees!
                //
                int governingDepth = m_governingTokenDepth; // *MUST* be captured to be observationally consistent in the check below
                if (governingDepth == -1 || governingDepth <= inliningDepth)
                {
                    pBeacon->InternalSignal();
                }
            }
        }
    }

    /// <summary>
    ///     Called to determine if a confirmed cancellation on this context is hidden at the depth of the caller.
    ///     A governing token that is not canceled could be protecting the task collection from cancellation from above.
    ///     The token of the supplied task collection is used to veto an interruption. See comments in IsCancellationVisible
    /// <summary>
    bool ContextBase::TokenHidesCancellation(_TaskCollectionBase* pCurrentTaskCollection, bool hasOverrideToken) const
    {
        //
        // An override token is used to determine visibility of cancellation at the end of _RunAndWait for structured
        // and unstructured task collections. The governing token and cancellation depths apply to higher level task
        // collections at this point, however, _RunAndWait should not interrupt if the token of pCurrentTaskCollection
        // is not canceled, so that the token provides total isolation from parent cancellation.
        //
        if (hasOverrideToken)
        {
            _CancellationTokenState * pOverrideTokenState = pCurrentTaskCollection->_GetTokenState();
            if (pOverrideTokenState == _CancellationTokenState::_None() || !pOverrideTokenState->_IsCanceled())
            {
                return true;
            }
        }
        //
        // Any token hides the propagation of implicit cancellation from above unless the token itself is EXPLICITLY canceled.
        // Note, that m_maxCancellationDepth can be ENTIRE_CONTEXT_CANCELED, which is < -1, therefore we must check that the
        // governing token depth is not -1.
        //
        if (m_maxCancellationDepth < m_governingTokenDepth && m_governingTokenDepth != -1)
        {
            ASSERT(m_pGoverningTokenState != NULL);
            if (m_pGoverningTokenState == _CancellationTokenState::_None())
            {
                return true;
            }
            return (!m_pGoverningTokenState->_IsCanceled());
        }
        return false;
    }

    /// <summary>
    ///     Called to determine whether a committed or pending cancellation on this context is visible at the level of the caller.
    ///     NOTE: This method should only be called on the current context from _RunAndWait for the task collection supplied at an argument.
    ///     The asserts below will check for that.
    ///     The interruption points at the end of _RunAndWait must use the token of the task collection to override cancellation from
    ///     above - i.e, even if the cancellation depth and governing token depth determine that an interruption point would've thrown
    ///     an interruption exception, if there was an uncanceled token on this task collection, no interruption should take place.
    ///     This allows total isolation from parent cancellation using cancellation tokens.
    /// </summary>
    bool ContextBase::IsCancellationVisible(_TaskCollectionBase* pCurrentTaskCollection, bool hasOverrideToken /* = false */) const
    {
        ASSERT(SchedulerBase::FastCurrentContext() == this && pCurrentTaskCollection->_M_pOwningContext == this);
        ASSERT(m_pExecutingCollection == pCurrentTaskCollection || m_pExecutingCollection == pCurrentTaskCollection->_M_pParent);
        ASSERT(HasAnyCancellation());

        return ((HasInlineCancellation() && !TokenHidesCancellation(pCurrentTaskCollection, hasOverrideToken)) ||
                    (HasPendingCancellation() && pCurrentTaskCollection->_WillInterruptForPendingCancel()));
    }

    /// <summary>
    ///     Returns an indication as to whether a cancellation is occurring at the specified depth. The result here is normally only valid when
    ///     called from the thread representing this context. There are times under the context chaining lock (stealers list) or from an indirect
    ///     alias of a collection on this context, where this can be called safely **FOR CERTAIN DEPTHS** from another thread.
    /// </summary>
    /// <param name="depth">
    ///     The depth at which to check for cancellation. If the method is called from a thread other than the one representing this context,
    ///     the caller must guarantee that this context will not unwind past a task group or structured task group of inlining depth = 'depth'.
    ///     If the caller has observed a collection inlined at 'depth' != -1 while holding the stealers lock on this context, or the caller
    ///     is executing a chores on an indirect alias while the original task group is inlined, this guarantee is automatically provided.
    /// </param>
    bool ContextBase::IsCanceledAtDepth(_TaskCollectionBase *pStartingCollection, int depth)
    {
        ASSERT(pStartingCollection->_M_inliningDepth >= depth);
        if (HasInlineCancellation() && m_minCancellationDepth <= depth)
        {
            //
            // Normally, this would be an indication of a cancellation in progress. There may, however, be a cancellation token which stops
            // us from observing it. Detecting this at arbitrary depth is more complex than detecting it at a current interruption point because
            // we only track min/max on cancellation depth. Arbitrary depth requires us to walk back up the inlining tree until we find the token
            // governing depth.
            //
            // If there is no token or all tokens are above the cancellation depth, clearly we do not need to do this.
            //
            if (m_governingTokenDepth == -1 || m_minCancellationDepth >= m_governingTokenDepth)
                return true;

            //
            // If we are checking a cancellation beacon in strictly nested order, this is a simple bottom check.
            //
            if (pStartingCollection == m_pExecutingCollection && depth == m_pExecutingCollection->_M_inliningDepth)
            {
                if (m_pGoverningTokenState == _CancellationTokenState::_None())
                    return false;

                return m_pGoverningTokenState->_IsCanceled();
            }

            //
            // At this point, we have exhausted every quick check since we can no longer rely on min/max. We need to walk the tree. Fortunately,
            // this should not need to be done often.
            //
            _TaskCollectionBase *pCollection = pStartingCollection;
            while (pCollection != NULL && pCollection != m_pRootCollection && pCollection->_M_inliningDepth != depth)
            {
                pCollection = pCollection->_SafeGetParent();
            }

            while (pCollection != NULL && pCollection != m_pRootCollection && pCollection->_GetTokenState() == NULL)
            {
                if ((pCollection->_IsStructured() && (static_cast<_StructuredTaskCollection *>(pCollection))->_IsMarkedForCancellation()) ||
                    (!pCollection->_IsStructured() && (static_cast<_TaskCollection *>(pCollection))->_IsMarkedForAbnormalExit()))
                {
                    return true;
                }
                pCollection = pCollection->_SafeGetParent();
            }

            if (pCollection != NULL && pCollection != m_pRootCollection)
            {
                _CancellationTokenState *pGoverningTokenState = pCollection->_GetTokenState();
                ASSERT(pGoverningTokenState != NULL);
                if (pGoverningTokenState != _CancellationTokenState::_None())
                {
                    return pGoverningTokenState->_IsCanceled();
                }
            }
        }
        return false;
    }

    _Cancellation_beacon::_Cancellation_beacon()
    {
        ContextBase *pContext = SchedulerBase::CurrentContext();
        _M_pRef = pContext->PushCancellationBeacon();
    }

    _Cancellation_beacon::~_Cancellation_beacon()
    {
        ContextBase *pContext = SchedulerBase::CurrentContext();
        pContext->PopCancellationBeacon();
    }

    bool _Cancellation_beacon::_Confirm_cancel()
    {
        ContextBase *pContext = SchedulerBase::CurrentContext();
        bool fCanceled =  pContext->ConfirmCancel(_M_pRef);
        if (!fCanceled)
        {
            _Lower();
        }
        return fCanceled;
    }

    /// <summary>
    ///     Return a reference to the ppltask inline schedule depth slot on current context
    ///     The inline depth will be set to 0 when the context is first initialized,
    ///     and the caller is responsible to maintain that depth.
    /// </summary>
    _CONCRTIMP size_t & __cdecl _StackGuard::_GetCurrentInlineDepth()
    {
        return SchedulerBase::CurrentContext()->m_asyncTaskCollectionInlineDepth;
    }
} // namespace details

} // namespace Concurrency
