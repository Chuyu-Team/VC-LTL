// ==++==
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// TaskCollection.cpp
//
// Internal implementation of task collections and related data structures
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <exception>

#pragma warning(disable:4297) //Function expected not to throw but does

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Destroys a task stack.
    /// </summary>
    TaskStack::~TaskStack()
    {
        if (m_pStack)
            delete [] m_pStack;
    }

    /// <summary>
    ///     Pushes an element onto the task stack. Returns a bool as to whether this could happen or not. The only
    ///     possible error here is out of memory.
    /// </summary>
    /// <param name="taskCookie">
    ///     The task cookie to push onto the stack
    /// </param>
    /// <returns>
    ///     An indication of whether the stack cap was reached.
    /// </returns>
    bool TaskStack::Push(int taskCookie)
    {
        if (m_stackPtr >= m_stackSize)
        {
            //
            // Prevent the task stack from growing beyond a predetermined size cap. If we exceed this cap, we will ignore the push.
            // Note that the CHORE itself is still pushed to the work stealing queue and can still be stolen. It just won't be on the inlining
            // list within the task collection. What this means is that a call to Wait will *NOT* be able to inline the chore. It also means that
            // any call to Wait after this return will suffer a *HUGE* penalty as every pop will be out-of-order and incur additional fencing
            // in the work stealing queue.
            //
            // The reason we cap this is specifically because we allow passing task collections between threads. It's entirely possible to have a pattern where
            // one thread (thread A) continues to add items to a task collection while another thread (thread B) waits on it. They never reverse roles. In this case,
            // the direct alias for thread A will continue to pile up items on this stack (the inlining list). Since wait is never called from that thread, the
            // stack will be popped. Without a cap, this list would grow infinitely. Note that in this scenario, there is no penalty in continuing to add
            // chores. The only time a penalty will happen is if Wait were called (and once the collection resets, the penalty goes away until the cap is reached
            // again).
            //
            if (m_stackPtr >= TASK_STACK_SIZE_CAP)
            {
                m_fOverflow = true;
                return false;
            }

            int size = m_stackSize + TASK_STACK_GROWTH_SIZE;
            int *pNewStack = _concrt_new int[size];

            memcpy(pNewStack, m_pStack, sizeof(int) * m_stackSize);
            m_stackSize = size;

            delete[] m_pStack;
            m_pStack = pNewStack;
        }

        ASSERT(m_stackPtr < m_stackSize);
        m_pStack[m_stackPtr++] = taskCookie;

        return true;
    }

    /// <summary>
    ///     Pops an element from the task stack.
    /// </summary>
    /// <returns>
    ///     The element
    /// </returns>
    int TaskStack::Pop()
    {
        ASSERT(m_stackPtr > 0);
        return m_pStack[--m_stackPtr];
    }

    /// <summary>
    ///     Returns an indication of whether or not the stack is empty.
    /// </summary>
    bool TaskStack::IsEmpty() const
    {
        return m_stackPtr == 0;
    }

    /// <summary>
    ///     Clears out everything on the stack.
    /// </summary>
    void TaskStack::Clear()
    {
        m_stackPtr = 0;
    }

    // **********************************************************************
    // Structured Task Collection:
    // **********************************************************************

    /// <summary>
    ///     Construct a new structured task collection whose cancellation is governed by the supplied cancellation token.
    /// </summary>
    /// <param name="_PTokenState">
    ///     When this cancellation token is canceled, the structured task group will be canceled.
    /// </param>
    /**/
    _StructuredTaskCollection::_StructuredTaskCollection(_CancellationTokenState *_PTokenState) :
        _TaskCollectionBase(_PTokenState)
    {
        _Construct();
        if (_PTokenState != NULL)
        {
            ContextBase *pCurrentContext = SchedulerBase::CurrentContext();
            _M_pOwningContext = pCurrentContext;

            if (_PTokenState != _CancellationTokenState::_None())
            {
                _PTokenState->_Reference();
            }

            //
            // If this is a new cancellation token, we need to register a callback. Remember, this is expensive as it involves a memory allocation.
            // We want to avoid this whenever possible.
            //
            if (pCurrentContext->GetGoverningTokenState() != _PTokenState)
            {
                if (_PTokenState != _CancellationTokenState::_None())
                {
                    _CancellationTokenRegistration *pRegistration = _PTokenState->_RegisterCallback(
                        reinterpret_cast<TaskProc>(&_StructuredTaskCollection::_CancelViaToken), this
                        );
                    _M_pTokenState = reinterpret_cast<_CancellationTokenState *>(
                        (reinterpret_cast<ULONG_PTR>(pRegistration) | TASKCOLLECTIONFLAG_POINTER_IS_REGISTRATION)
                        );
                }
                else
                {
                    // This should already have been set in the base class constructor.
                    ASSERT(_M_pTokenState == _CancellationTokenState::_None());
                }
            }
        }
    }

    /// <summary>
    ///     Destruct a task collection and wait on all associated work to finish. Clients must call '_StructuredTaskCollection::_Wait'
    ///     or '_StructuredTaskCollection::_RunAndWait' prior to destructing the object. If there are chores remaining in the queues, an
    ///     exception (missing_wait) is thrown. If the destructor is running because of exception unwinding, it will abort any scheduled work.
    ///     If another exception occurs because work is aborted, the process will terminate (C++ semantics).
    /// </summary>
    /**/
    _StructuredTaskCollection::~_StructuredTaskCollection()
    {
        if (!_TaskCleanup())
        {
            if (_M_pTokenState != NULL && _M_pTokenState != _CancellationTokenState::_None())
            {
                _CleanupToken();
            }

            throw missing_wait();
        }

        if (_M_pTokenState != NULL && _M_pTokenState != _CancellationTokenState::_None())
        {
            _CleanupToken();
        }
    }


    /// <summary>
    ///     The callback which is made when a cancellation occurs via a token associated with a structured_task_group on the boundary
    ///     of two cancellation tokens.
    /// </summary>
    void _StructuredTaskCollection::_CancelViaToken(_StructuredTaskCollection *pCollection)
    {
        //
        // NOTE: This is what we would normally consider a violation of the structured task group contract. This cancellation can happen from an
        // atribtrary thread. The only reason that this is safe is because _RunAndWait understands tokens and the synchronization is handled via
        // the deregister call contained within.
        //
        pCollection->_Cancel();
    }

    /// <summary>
    ///     Schedules a new unrealized chore on the task collection.
    /// </summary>
    /// <param name="_PChore">
    ///     The new unrealized chore to schedule
    /// </param>
    /// <param name="_PLocation">
    ///     The location where the unrealized chore should execute. Specifying the value NULL here indicates that the unrealized chore does not
    ///     have specific placement.
    /// </param>
    void _StructuredTaskCollection::_Schedule(_UnrealizedChore * _PChore, location *_PLocation)
    {
        if (_PChore->_M_pTaskCollection != NULL)
            throw invalid_multiple_scheduling();

        _PChore->_M_pTaskCollection = this;
        _PChore->_M_pChoreFunction = &_UnrealizedChore::_StructuredChoreWrapper;
        ++_M_unpoppedChores;
        if (_M_pOwningContext == NULL)
            _M_pOwningContext = SchedulerBase::CurrentContext();
        reinterpret_cast <ContextBase *> (_M_pOwningContext)->PushStructured(_PChore, _PLocation);
    }

    /// <summary>
    ///     Schedules a new unrealized chore on the task collection.
    /// </summary>
    /// <param name="_PChore">
    ///     The new unrealized chore to schedule
    /// </param>
    void _StructuredTaskCollection::_Schedule(_UnrealizedChore * _PChore)
    {
        if (_PChore->_M_pTaskCollection != NULL)
            throw invalid_multiple_scheduling();

        _PChore->_M_pTaskCollection = this;
        _PChore->_M_pChoreFunction = &_UnrealizedChore::_StructuredChoreWrapper;
        ++_M_unpoppedChores;
        if (_M_pOwningContext == NULL)
            _M_pOwningContext = SchedulerBase::CurrentContext();
        reinterpret_cast <ContextBase *> (_M_pOwningContext)->PushStructured(_PChore);
    }

    /// <summary>
    ///     Runs a specified chore (pChore) and subsequently waits on all chores associated with the task collection
    ///     to execute.
    /// </summary>
    /// <param name="pChore">
    ///     The chore to run locally.
    /// </param>
    /// <returns>
    ///     An indication of the status of the wait.
    /// </returns>
    __declspec(noinline)
    _TaskCollectionStatus __stdcall _StructuredTaskCollection::_RunAndWait(_UnrealizedChore *pChore)
    {
        ASSERT(_M_pOwningContext != NULL || _M_unpoppedChores == 0);
        if (_M_pOwningContext == NULL)
            _M_pOwningContext = SchedulerBase::CurrentContext();
        ContextBase *pCurrentContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);

        _M_pParent = pCurrentContext->GetExecutingCollection();
        _M_inliningDepth = _M_pParent != NULL ? _M_pParent->_InliningDepth() + 1 : 0;
        pCurrentContext->SetExecutingCollection(this);

        _CancellationTokenRegistration *pRegistration = NULL;
        _CancellationTokenState *pTokenState = NULL;

        if (_M_pTokenState != NULL)
        {
            pTokenState = _GetTokenState(&pRegistration);
            pCurrentContext->PushGoverningTokenState(pTokenState, _M_inliningDepth);
        }

        try
        {
            if (pChore != NULL)
            {
                //
                // Ordinarily, we need a full fence here to ensure that the write of _M_inliningDepth and the read of the context cancellation
                // flag are not reordered with respect to each other as perceived by a cancellation thread. If they are, the cancellation thread
                // can miss flagging an entire branch of the work tree rooted at pChore.
                //
                // The scenario is as follows:
                //
                //  -
                // |A|
                //  -
                //  | \
                //  |  (ch x -- already stolen) [](){A.cancel();}
                //  |
                //  |
                //  (ch y -- local chore -- pChore)
                //
                // - ch y checks whether it is locally marked for cancellation
                // - ch x cancels. It doesn't observe _M_inliningDepth yet because there is no barrier on this thread here
                //   therefore, it does not cancel the context
                // - We execute pChore. pChore's descendents do not see the cancellation because the context flag was not set
                //
                // While a full fence here addresses this issue, it is a cost we do not want to bear during the fast inlining path. Because of
                // the special properties of structured task collections, we are going to exploit this nature to elide the fence. When a
                // structured collection is canceled, the owning context will be marked as "pending cancellation" if it was not perceived as
                // inlined by the canceling thread. Even if we don't see the task collection marked canceled here at this interruption point,
                // an interruption point in the inline chore will see the pending flag set and throw the _Interruption_exception.
                //
                if (_IsMarkedForCancellation() || (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(this)))
                {
                    throw _Interruption_exception();
                }

                pChore->m_pFunction(pChore);
                pChore->_M_pTaskCollection = NULL;
            }

            long queuedChores = _M_unpoppedChores;

            while (queuedChores > 0)
            {
                pChore = pCurrentContext->PopStructured();

                //
                // **READ THIS** (This is rather subtle):
                //
                // In order to avoid a restriction on structured task collections that there cannot be an interruption point between the declaration
                // of the collection and its corresponding wait, we must guarantee that we only flag the owning context as canceled if the collection
                // is inlined (as evidenced by _M_inliningDepth above). The problem is that there is **NO FENCE** on this set. That means that if the
                // cancellation thread perceives the write of _M_inliningDepth out of order with respect to OUR read of the cancellation flags below,
                // this branch can fail to cancel for a single chore (and its nested subtrees).
                //
                // In order to avoid this (in at least the vast majority of cases), the interruption point is being strategically placed between the
                // PopStructured call above and the execution of the chore because Pop is -- the vast majority of the time -- a full barrier. We are,
                // in essence, borrowing the full fence in pop to order to eliminate this race.
                //
                // Note -- one of the optimizations of the WSQ (privatization) which may occur in the future can elide the fence on pop some of the time.
                // If this happens, it is entirely possible that in rare circumstances, we will STILL miss and the write/read will be perceived in the opposite
                // order by the canceling processor. In that case, the worst thing that happens is that we execute a single chore and its subtrees without
                // getting the cancel there. Given that an additional barrier specific to cancellation would result in ~25% performance hit on key benchmarks,
                // this is something we're living with.
                //
                // Note also that there must be a fence of _M_inliningDepth and a subsequent interruption point between the set of _M_inliningDepth and the
                // WaitOnStolenChores if everything was stolen prior to getting into this function. Otherwise, we can fail to cancel entire branches if the
                // Wait() happens **AFTER** all branches are stolen. Between the PopStructured (acting as fence) and the break below is the only place to
                // strategically do this without introducing extra overhead. This means that there will be code replication in the catch blocks below.
                //
                if (_IsMarkedForCancellation() || (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(this)))
                {
                    //
                    // We need to know whether the local chore has performed accounting or not. Flag this within the collection to avoid additional space
                    // on the local stack (which affects benchmark performance). This pushes **ALL** of the overhead into the cancellation path. This flag
                    // will be checked below when the exception is caught.
                    //
                    _M_inlineFlags |= _S_localCancel;
                    throw _Interruption_exception();
                }

                if (pChore == NULL)
                    break;

                --queuedChores;

                if (pChore == reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
                    continue;

                --_M_unpoppedChores;

                if (pCurrentContext->IsExternal())
                    static_cast<ExternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();
                else
                    static_cast<InternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();

                pChore->m_pFunction(pChore);
                pChore->_M_pTaskCollection = NULL;
            }

            if (_M_unpoppedChores > 0)
            {
                //
                // Note that the chore difference between _M_unpoppedChores and queuedChores indicates how many were executed via
                // explicit affinitization. The wait mechanism is, however, identical.
                //
                _WaitOnStolenChores(_M_unpoppedChores);
                _M_unpoppedChores = 0;
            }
        }
        catch(const _Interruption_exception &)
        {
            if (pChore != NULL && pChore != reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
            {
                if (_M_inlineFlags & _S_localCancel)
                {
                    //
                    // This did not happen above because the interruption point prevented it. The interruption point is located where it is for strategic fence
                    // reduction. Hence, this code should match **EXACTLY** what is done above between the break and the execution of m_pFunction.
                    //
                    --_M_unpoppedChores;

                    if (pCurrentContext->IsExternal())
                        static_cast<ExternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();
                    else
                        static_cast<InternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();
                }
                pChore->_M_pTaskCollection = NULL;
            }
            _RaisedCancel();
        }
        catch(...)
        {
            //
            // Track the exception that was thrown here and rethrow outside catch handler.
            //
            if (pChore != NULL && pChore != reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
            {
                pChore->_M_pTaskCollection = NULL;
            }
            _RaisedException();
        }

        //
        // If necessary remove any registration from the cancellation token -- the destructor will handle the reference
        // removal.
        //
        if (_M_pTokenState != NULL)
        {
            ASSERT(pTokenState != NULL);
            pCurrentContext->PopGoverningTokenState(pTokenState);
            //
            // This call will synchronize with a corresponding cancellation.
            //
            if (pRegistration != NULL)
            {
                pTokenState->_DeregisterCallback(pRegistration);
            }
        }

        pCurrentContext->SetExecutingCollection(_M_pParent);

        if (_M_pException != NULL) // this could be due to either a user exception or the internal _Interruption_exception.
        {
            //
            // This will rethrow if an exception was caught (both in the catch blocks above and in _UnrealizedChore::_StructuredChoreWrapper)
            // However, it will not rethrow an _Interruption_exception. That exception should only be thrown if there is a higher level
            // cancel that is visible to this task collection *AFTER* _Abort has returned. Abort will undo the effect of a CancelCollection
            // (with CancelCollectionComplete), if this collection was canceled while it was inlined.
            //
            _Abort();
            //
            // As _Abort undoes the effect of cancellations at this level, HasAnyCancellations() and IsCancellationVisible() from this
            // refer to cancellation at a higher level. However if this task group had an uncanceled token, _RunAndWait should not throw
            // the interruption exception up
            //
            if (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(this, _M_pTokenState != NULL))
            {
                throw _Interruption_exception();
            }
            return _Canceled;
        }

        //
        // It's possible that our last chore caused a cancellation higher up in the tree and we should interrupt for that case, if appropriate.
        //
        if (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(this, _M_pTokenState != NULL))
        {
            throw _Interruption_exception();
        }

        return _Completed;
    }

    /// <summary>
    ///     Internal routine to clean up after a cancellation token.
    /// </summary>
    void _StructuredTaskCollection::_CleanupToken()
    {
        ASSERT(_CancellationTokenState::_IsValid(_M_pTokenState));

        _CancellationTokenRegistration *pRegistration = NULL;
        _CancellationTokenState *pTokenState = _GetTokenState(&pRegistration);

        if (pRegistration != NULL)
        {
            pRegistration->_Release();
        }

        if (_CancellationTokenState::_IsValid(pTokenState))
        {
            pTokenState->_Release();
        }
    }

    /// <summary>
    ///     Aborts chores related to the task collection and waits for those which cannot be forcibly aborted.
    /// </summary>
    void _StructuredTaskCollection::_Abort()
    {
        //
        // _Abort cannot be called unless Schedule was called which guaranteed _M_pOwningContext != NULL
        //
        ASSERT(_M_pOwningContext != NULL);
        ContextBase *pCurrentContext = reinterpret_cast<ContextBase *> (_M_pOwningContext);

        long queuedChores = _M_unpoppedChores;

        while (queuedChores > 0)
        {
            _UnrealizedChore *pChore = pCurrentContext->PopStructured();
            if (pChore == NULL)
                break;

            --queuedChores;

            if (pChore == reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
                continue;

            pChore->_M_pTaskCollection = NULL;

            //
            // Update the statistical information with the fact that a task has been dequeued
            //
            if (pCurrentContext->IsExternal())
                static_cast<ExternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();
            else
                static_cast<InternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();

            --_M_unpoppedChores;
        }

        if (_M_unpoppedChores > 0)
        {
            //
            // If there are stolen chores outstanding, redo the cancellation to trigger marking of them in special circumstances. It's entirely possible
            // that the cancellation only happened as a result of chaining and all our chores were stolen at the time.
            //
            _Cancel();

            //
            // Note that the chore difference between _M_unpoppedChores and queuedChores indicates how many were executed via
            // explicit affinitization. The wait mechanism is, however, identical.
            //
            _WaitOnStolenChores(_M_unpoppedChores);
            _M_unpoppedChores = 0;
        }

        //
        // Any caught exception on the collection should be rethrown on this thread. The exception might be one of several things:
        //
        // _Interruption_exception (or another internal runtime exception):
        //
        //     - We want to let this exception continue propagating unless there's a *more important* one (like an arbitrary exception) that occurred
        //       elsewhere.
        //
        // an arbitrary exception:
        //
        //     - We are allowed to choose an arbitrary exception to flow back.
        //
        _SpinWaitBackoffNone spinWait;
        while (((size_t) _Exception() == _S_nonNull) || (_CancelState() == _S_cancelStarted)) // make sure the exception is ready or that cancellation is finished
        {
            spinWait._SpinOnce();
        }

        if (_PerformedInlineCancel())
        {
            pCurrentContext->CancelCollectionComplete(_M_inliningDepth);
        }
        else if (_PerformedPendingCancel())
        {
            pCurrentContext->PendingCancelComplete();
        }

        _RethrowException();
    }

    /// <summary>
    ///     Cancels work on the task collection.
    /// </summary>
    void _StructuredTaskCollection::_Cancel()
    {
        if (_M_pOwningContext == NULL)
            _M_pOwningContext = SchedulerBase::CurrentContext();

        //
        // Multiple stolen chores might cancel at the same time. We can only allow one person into the path
        // which fires down threads so the counters get set correctly.
        //
        if (_MarkCancellation())
        {
            //
            // Determine which inline context needs to be aborted (we could be canceling from a stolen chore which is perfectly
            // legal under the structured semantic).
            //
            // Note that the original context may not have inlined yet. If we arbitrarily cancel the owning context, we place a
            // heavy restriction on structured task collection that it cannot have an interruption point between its declaration
            // and its wait. It would be bad for such an interruption point to throw an exception because there may be no one on
            // the stack to catch that exception. At the moment, this is deemed to be too heavy a restriction. Therfore, we only
            // cancel the collection if it is inlining. There is a subtle implication to this too. Because a full fence is too
            // expensive on the inlining side, the setting of inline can be reordered with respect to the read of the cancellation
            // bit. If that reordering is perceived by a canceling thread, chores may execute despite cancellation on the inline
            // context. This would be unfortunate, but perfectly legal according to the cancellation semantic.
            //
            // In order to avoid this type of race for the inline chore of a _RunAndWait, we are going to exploit special properties
            // of a structured task collection: since we have a guarantee that this collection will be inlined on this thread. We
            // are going to mark the thread as pending cancellation if we do not perceive it as currently inlined. This will allow
            // us to elide a fence during a local chore in _RunAndWait.
            //
            ContextBase *pContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);
            if (_M_inliningDepth >= 0)
            {
                //
                // _M_inliningDepth is guaranteed to be stable if we perceive this. Only the inline context or a stolen chore can
                // cancel a structured collection. If the collection is currently inlined, we're in a wait which won't be satisfied
                // until this thread completes.
                //
                pContext->CancelCollection(_M_inliningDepth);
                _FinishCancelState(_S_cancelShotdownOwner);
            }
            else
            {
                pContext->PendingCancel();
                _FinishCancelState(_S_cancelDeferredShootdownOwner);
            }

            _CancelStolenContexts();
        }
    }

    /// <summary>
    ///     Called to cancel any contexts which stole chores from the given collection.
    /// </summary>
    void _StructuredTaskCollection::_CancelStolenContexts()
    {
        ContextBase *pContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);
        pContext->CancelStealers(this);
    }

    /// <summary>
    ///     Informs the caller whether or not the task collection is currently in the midst of cancellation. Note that this
    ///     does not necessarily indicate that Cancel was called on the collection (although such certainly qualifies this function
    ///     to return true). It may be the case that the task collection is executing inline and a task collection further up in the work
    ///     tree was canceled. In cases such as these where we can determine ahead of time that cancellation will flow through
    ///     this collection, true will be returned as well.
    /// </summary>
    /// <returns>
    ///     An indication of whether the task collection is in the midst of a cancellation (or is guaranteed to be shortly).
    /// </returns>
    bool _StructuredTaskCollection::_IsCanceling()
    {
        if (_M_pOwningContext == NULL)
            _M_pOwningContext = SchedulerBase::CurrentContext();

        //
        // If This collection has an exception or cancellation we can return true right away. Cancellation flags are stored in
        // the last two bits of the exception field
        //
        if (_M_pException != NULL)
            return true;
        //
        // If our token is canceled, flag us immediately.
        //
        _CancellationTokenState* pTokenState = _GetTokenState();
        if (_CancellationTokenState::_IsValid(pTokenState) && pTokenState->_IsCanceled())
        {
            _Cancel();
            return true;
        }

        ContextBase *pOwningContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);
        //
        // Either we were canceled or someone higher than us on our context was canceled. This is all safe without lock because of the rules for using
        // a structured task collection. NOTHING changes those rules. You may only call this from the owning context or a thread within the work tree. This has
        // the same "special" properties as ::_Cancel in that regard.
        //
        return ((_IsCurrentlyInlined() && pOwningContext->IsCanceledAtDepth(this)) ||
                    (pOwningContext->HasPendingCancellation() && _WillInterruptForPendingCancel()));
    }

    /// <summary>
    ///     Waits on a specified number of stolen chores.
    /// </summary>
    /// <param name="stolenChoreCount">
    ///     The number of stolen chores to wait upon
    /// </param>
    void _StructuredTaskCollection::_WaitOnStolenChores(long stolenChoreCount)
    {
        if (_M_completedStolenChores <= _CollectionInitializationInProgress)
            _Initialize();

        long count = InterlockedExchangeAdd(&_M_completedStolenChores, -stolenChoreCount) - stolenChoreCount;

        if (count < 0)
            reinterpret_cast <StructuredEvent*> (_M_event)->Wait();
    }

    /// <summary>
    ///     Indicates that a stolen chore has completed.
    /// </summary>
    void _StructuredTaskCollection::_CountUp()
    {
        if (_M_completedStolenChores <= _CollectionInitializationInProgress)
            _Initialize();

        LONG count = InterlockedIncrement(&_M_completedStolenChores);

        if (count == 0)
            reinterpret_cast <StructuredEvent*> (_M_event)->Set();
    }

    /// <summary>
    ///     Initializes the structured task collection to count stolen chores.
    /// </summary>
    void _StructuredTaskCollection::_Initialize()
    {
        if (InterlockedCompareExchange(&_M_completedStolenChores,
                                       _CollectionInitializationInProgress,
                                       _CollectionNotInitialized) == _CollectionNotInitialized)
        {
            new (reinterpret_cast <void *> (_M_event)) StructuredEvent();
#if _DEBUG
            long previousCompleted = InterlockedExchange(&_M_completedStolenChores, _CollectionInitialized);
            ASSERT(previousCompleted == _CollectionInitializationInProgress);
#else
            InterlockedExchange(&_M_completedStolenChores, _CollectionInitialized);
#endif
        }
        else
        {
            _SpinWaitBackoffNone spinWait;
            while (_M_completedStolenChores <= _CollectionInitializationInProgress)
                spinWait._SpinOnce();
        }
    }

    // **********************************************************************
    // Unstructured Task Collections:
    // **********************************************************************

    /// <summary>
    ///     Constructs a new unstructured task collection
    /// </summary>
    _TaskCollection::_TaskCollection()
        : _M_executionStatus(TASKCOLLECTION_EXECUTION_STATUS_CLEAR)
        , _M_pNextAlias(NULL)
        , _M_pTaskExtension(NULL)
        , _M_flags(0)
        , _M_stackPos(0)
        , _M_completionHandler(NULL)
    {
        //
        // CurrentContext may create a context
        //
        _M_pOwningContext = SchedulerBase::CurrentContext();
        ContextBase *pCurrentContext = reinterpret_cast<ContextBase*> (_M_pOwningContext);
        _M_pParent = pCurrentContext->GetExecutingCollection();

        _Initialize();
        _M_event.set();
        _M_pOriginalCollection = this;
        _M_boundQueueId = SchedulerBase::FastCurrentContext()->GetWorkQueueIdentity();
        _M_inlineFlags = 0;
    }

    /// <summary>
    ///     Constructs a new task collection whose cancellation is governed by the specified cancellation token state.
    /// </summary>
    /// <param name="_PTokenState">
    ///     When this cancellation token is canceled, the task collection is canceled.
    /// </param>
    _TaskCollection::_TaskCollection(_CancellationTokenState *_PTokenState)
        : _TaskCollectionBase(_PTokenState)
        , _M_executionStatus(TASKCOLLECTION_EXECUTION_STATUS_CLEAR)
        , _M_pNextAlias(NULL)
        , _M_pTaskExtension(NULL)
        , _M_flags(0)
        , _M_stackPos(0)
        , _M_completionHandler(NULL)
    {
        if (_CancellationTokenState::_IsValid(_PTokenState))
        {
            _PTokenState->_Reference();
        }

        //
        // CurrentContext may create a context
        //
        _M_pOwningContext = SchedulerBase::CurrentContext();
        ContextBase *pCurrentContext = reinterpret_cast<ContextBase*> (_M_pOwningContext);
        _M_pParent = pCurrentContext->GetExecutingCollection();

        _Initialize();
        _M_event.set();
        _M_pOriginalCollection = this;
        _M_boundQueueId = SchedulerBase::FastCurrentContext()->GetWorkQueueIdentity();
        _M_inlineFlags = 0;
    }

    /// <summary>
    ///     Performs task cleanup normally done at destruction time.
    /// </summary>
    /// <param name="fExceptional">
    ///     An indication if the cleanup is exceptional and the collection should be left in a canceled state.
    /// </param>
    bool _TaskCollection::_TaskCleanup(bool fExceptional)
    {
        bool fThrow = false;

        //
        // Direct alias destruction should not attempt to go through any wait/abort cycle. It's simply the deletion/abandonment
        // of the alias. The original collection might not even be around to touch.
        //
        if (!_IsDirectAlias())
        {
            if (!__uncaught_exception())
            {
                //
                // Users are required to call Wait() before letting the destructor run. Otherwise, throw. Note that before throwing,
                // we must actually wait on the tasks since they contain pointers into stack frames and unwinding without the wait is
                // instant stack corruption.
                //
                fThrow = (_M_unpoppedChores > 0);

                //
                // We must check all direct aliases as well.
                //
                if (_M_pOriginalCollection == this && _M_pNextAlias != NULL)
                {
                    _TaskCollection *pAlias = _M_pNextAlias;
                    while (pAlias != NULL)
                    {
                        if (pAlias->_M_unpoppedChores > 0)
                            fThrow = true;

                        pAlias = pAlias->_M_pNextAlias;
                    }
                }

                if (fThrow)
                    _Abort(fExceptional);
            }
            else
                _Abort(fExceptional);

        }

        return !fThrow;
    }

    /// <summary>
    ///     Destructs a new unstructured task collection
    /// </summary>
    _TaskCollection::~_TaskCollection()
    {
        bool fThrow = false;

        //
        // Direct alias destruction should not attempt to go through any wait/abort cycle. It's simply the deletion/abandonment
        // of the alias. The original collection might not even be around to touch.
        //
        if (!_IsDirectAlias())
        {
            fThrow = !_TaskCleanup(false);

            //
            // Go through and cleanup direct aliases. Note that there's an inherent problem and conflict here:
            //
            // - An internal context may go away and need to destroy its alias table -- it cannot touch the original task collection since it does
            //   not know when that collection may be deleted (it may have already)
            //
            // - The original task collection may be deleted but it cannot remove entries from alias tables.
            //
            // In order to resolve this and appropriately free the aliases, there's a simple cleanup state machine with a set of rules to act
            // as a last man out frees the object.
            //
            //     - If the destructor runs, it flags each alias so that the context can delete them
            //     - When a context exits, it flags each alias so that the destructor deletes them
            //     - First one to reach an alias with the flag set frees it.
            //
            // Note this is essentially a fixed reference count of two, but done with a bit flag to allow for other shared state in the flags.
            //
            if (_M_pOriginalCollection == this)
            {
                _TaskCollection *pAlias = _M_pNextAlias;
                _TaskCollection *pNext = NULL;

                for (; pAlias; pAlias = pNext)
                {
                    pNext = pAlias->_M_pNextAlias;
                    pAlias->_ReleaseAlias();
                }
            }
        }

        TaskStack *pStack = reinterpret_cast<TaskStack *> (_M_pTaskExtension);
        if (pStack) delete pStack;

        // If this task collection was used for a PPL task, the exception could still be stored here.
        std::exception_ptr *pException = _Exception();
        if (pException != NULL && (size_t)pException != _S_cancelException)
        {
            delete pException;
        }

        if (_CancellationTokenState::_IsValid(_M_pTokenState))
        {
            _M_pTokenState->_Release();
        }

        if (fThrow)
            throw missing_wait();
    }

    /// <summary>
    ///     Constructs a new unstructured task collection as an alias of an already existing one. An alias in this particular
    ///     case is a context-local representation of the original task collection.
    /// </summary>
    /// <param name="pOriginCollection">
    ///     The source of the aliasing. The newly constructed collection will be a direct or indirect
    ///     alias of this collection
    /// </param>
    /// <param name="fDirectAlias">
    ///     Indicates whether the collection is a direct alias (the collection is used on an arbitrary thread
    ///     not related to stolen work) or an indirect alias (a collection implicitly created for stolen chores).
    /// </param>
    _TaskCollection::_TaskCollection(_TaskCollection *pOriginCollection, bool fDirectAlias)
        : _M_executionStatus(TASKCOLLECTION_EXECUTION_STATUS_CLEAR)
        , _M_pOriginalCollection(pOriginCollection->_M_pOriginalCollection)
        , _M_pTaskExtension(NULL)
        , _M_flags(0)
        , _M_stackPos(0)
        , _M_completionHandler(NULL)
    {
        //
        // CurrentContext may create a context
        //
        _M_pOwningContext = SchedulerBase::CurrentContext();
        ContextBase *pCurrentContext = reinterpret_cast<ContextBase*> (_M_pOwningContext);
        _M_pParent = pCurrentContext->GetExecutingCollection();
        _M_pTokenState = pOriginCollection->_M_pTokenState;
        if (_CancellationTokenState::_IsValid(_M_pTokenState))
        {
            _M_pTokenState->_Reference();
        }
        _Initialize();
        _M_event.set();
        if (fDirectAlias)
        {
            _TaskCollection *pAlias = _M_pOriginalCollection->_M_pNextAlias;
            for (;;)
            {
                _M_pNextAlias = pAlias;
                _TaskCollection *pxchgAlias = reinterpret_cast <_TaskCollection *> (InterlockedCompareExchangePointer((volatile PVOID*)&_M_pOriginalCollection->_M_pNextAlias, this, pAlias));
                if (pxchgAlias == pAlias)
                    break;

                pAlias = pxchgAlias;
            }
        }
        else
        {
            _M_flags = _M_flags | TASKCOLLECTIONFLAG_ALIAS_IS_INDIRECT;
            _M_pNextAlias = NULL;
        }

        _M_boundQueueId = SchedulerBase::FastCurrentContext()->GetWorkQueueIdentity();
        _M_inlineFlags = 0;
    }

    /// <summary>
    ///     Determines whether the alias is stale (waiting to be deleted)
    /// </summary>
    bool _TaskCollection::_IsStaleAlias() const
    {
        ASSERT (_IsAlias());
        return (_M_flags & TASKCOLLECTIONFLAG_ALIAS_FREE_ON_VIEW) != 0;
    }

    /// <summary>
    ///     Releases an alias (frees it if appropriate)
    /// </summary>
    void _TaskCollection::_ReleaseAlias()
    {
        ASSERT (_IsAlias());
        long flags = _M_flags;
        //
        // Future proof against usage of the flags field.
        //
        for (;;)
        {
            //
            // If we observed the flag but weren't the one to set it, we're responsible for freeing the alias.
            //
            if (flags & TASKCOLLECTIONFLAG_ALIAS_FREE_ON_VIEW)
                break;

            long xchgFlags = InterlockedCompareExchange(&_M_flags, flags | TASKCOLLECTIONFLAG_ALIAS_FREE_ON_VIEW, flags);
            if (xchgFlags == flags)
            {
                //
                // If we get here, *this* is poison.
                //
                return;
            }
            flags = xchgFlags;
        }
        delete this;
    }

    /// <summary>
    ///     Returns the original task collection (the collection that this object is an alias for).
    /// </summary>
    _TaskCollection *_TaskCollection::_OriginalCollection() const
    {
        ASSERT(_IsAlias());
        return _M_pOriginalCollection;
    }

    /// <summary>
    ///     Returns the alias for the specified task collection on the current context. A NULL return would indicate
    ///     an error condition (e.g.: inability to allocate a new direct alias, etc...).
    /// </summary>
    /// <returns>
    ///     The alias for the specified task collection on the current context or NULL on error
    /// </returns>
    _TaskCollection *_TaskCollection::_Alias()
    {
        ASSERT(!_IsDirectAlias());

        //
        // Someone may have used this task collection on an arbitrary new thread -- hence, we need to make sure there's
        // a current context (not FastCurrentContext). Note that such usage will imply a direct alias (the code
        // will fall through to that point)
        //
        // Note that a task collection is bound to both the thread and the work queue. Normally, these won't differ, but may
        // in certain cases where a task collection is used on an internal context which exits before deletion and we get into
        // detached work queue cases. Those queues get deleted when empty and it's entirely possible that another queue
        // could get reallocated in the exact same memory location. Hence -- we bind to an identity assigned to each
        // work queue. Thus, aliasing checks both the owning context and the queue identity.
        //
        ContextBase *pCurrentContext = SchedulerBase::CurrentContext();
        DWORD queueId = pCurrentContext->GetWorkQueueIdentity();
        if (pCurrentContext != reinterpret_cast<ContextBase *> (_M_pOwningContext) || queueId != _M_boundQueueId)
        {
            //
            // The task collection has been used on an alternate thread. We need an alias for the task collection. The alias can
            // take one of two forms: a direct alias (the collection is used on an arbitrary thread) or an indirect alias
            // (the collection is used during a stolen chore).
            //
            // Indirect aliases are simple: they have the lifetime (and wait span) of the stolen chore. Direct aliases
            // have far more complication.
            //
            _TaskCollection *pIndirectAlias = pCurrentContext->GetIndirectAlias();
            if (pIndirectAlias != NULL)
            {
                if (pIndirectAlias->_M_pOriginalCollection == this)
                    return pIndirectAlias;

                //
                // It's still possible that this follows the pattern used by indirect aliases. It could be transitive:
                //
                // _TaskCollection rtp;
                // rtp.Schedule(
                //      {
                //          _TaskCollection tp;
                //          tp.Schedule(
                //              {
                //                  rtp.Schedule(...);  // <-- this is transitive.
                //                  rtp.Cancel(...);    // <-- this is transitive.
                //              }
                //      });
                //
                // The unfortunate reality of this situation is that indirect aliasing cannot work here (see below). We need
                // a direct alias.
                //
                // Second generation or older transitivity:  While the indirect alias could be used for this to satisfy the wait,
                // it would lead to deadlock and unexpected behavior if there are out-of-band dependencies between the code after the wait and the
                // whatever we add to the transitive object. For example,
                //
                // A -> B -> C
                //
                // If C does A.Schedule(x);
                // x == { receive_message(); }
                // and someone in the middle does B.Wait(); send_message();
                //
                // using the indirect alias would deadlock because C would wait on x, B waits on C, and after B waits on C, x is satisfied.
                //
                // Hence -- we must use a direct alias in this case.
                //
            }

            ASSERT(!_IsAlias());

            _TaskCollection *pAlias = pCurrentContext->GetArbitraryAlias(this);
            if (pAlias != NULL)
            {
                //
                // Make certain the alias we are returning to the client is an alias for the task collection and thread we think it is and that it is **NOT**
                // stale. Stale would imply that either the this pointer was deleted (bad) or that the context underlying the alias was deleted (bad). In any
                // of these cases, there's an issue with the alias we are returning and the caller will corrupt another thread's data structure.
                //
                ASSERT(pAlias->_M_pOriginalCollection == this && reinterpret_cast<ContextBase *>(pAlias->_M_pOwningContext) == pCurrentContext && !pAlias->_IsStaleAlias());
                return pAlias;
            }

            //
            // At this stage, we are forced to create a direct alias.
            //
            _TaskCollection *pDirectAlias = _concrt_new _TaskCollection(this, true);
            pCurrentContext->AddArbitraryAlias(this, pDirectAlias);

            return pDirectAlias;
        }
        return this;
    }

    /// <summary>
    ///     Returns whether the task collection is an alias.
    /// </summary>
    bool _TaskCollection::_IsAlias() const
    {
        return (_M_pOriginalCollection != this);
    }

    /// <summary>
    ///     Returns whether the task collection is an indirect alias.
    /// </summary>
    bool _TaskCollection::_IsIndirectAlias() const
    {
        return (_M_pOriginalCollection != this && (_M_flags & TASKCOLLECTIONFLAG_ALIAS_IS_INDIRECT) != 0);
    }

    /// <summary>
    ///     Returns whether the task collection has a direct alias
    /// </summary>
    bool _TaskCollection::_HasDirectAlias() const
    {
        return (_M_pOriginalCollection->_M_pNextAlias != NULL);
    }

    /// <summary>
    ///     Returns whether the task collection is a direct alias.
    /// </summary>
    bool _TaskCollection::_IsDirectAlias() const
    {
        return (_M_pOriginalCollection != this && (_M_flags & TASKCOLLECTIONFLAG_ALIAS_IS_INDIRECT) == 0);
    }

    /// <summary>
    ///     Returns whether this task collection is marked for abnormal exit.
    /// </summary>
    bool _TaskCollection::_IsMarkedForAbnormalExit() const
    {
        return (_M_pOriginalCollection->_M_exitCode != 0);
    }

    /// <summary>
    ///     Called when a new chore is placed upon the task collection. Guarantees forward synchronization with the completion of them.
    /// </summary>
    void _TaskCollection::_NotifyNewChore()
    {
        long val = InterlockedIncrement(&_M_unpoppedChores);
        ASSERT(val > 0);
        //
        // Because the task collection can be passed between threads and waited upon, either this thread or a stealer might need to wake
        // another thread on final completion (we might never wait). Thus, we need to fence these operations. We also need
        // to make sure 0->1 and 1->0 transitions correctly perform the appropriate signaling.
        //
        if (val == 1)
        {
            //
            // It's entirely possible that we're racing with a _NotifyCompletedChoreAndFree which just did a 1->0 and we just did a 0->1. We need to make
            // sure that the event is signaled before we clear it. Otherwise, it's possible that the event winds up out of sync with
            // the counter.
            //
            // In the vast majority of cases, the pEvent->Wait() call has no fences and merely checks the state seeing it signaled and returns.
            // The only time there's even a fence is during the race.
            //
            _M_event.wait();

            //
            // This is the barrier at which point other threads think there's something to wait upon. Note that it's not upon the WSQ yet
            // (meaning no one can steal and transition us from 1->0 as of yet).
            //
            _M_event.reset();
        }
    }

    /// <summary>
    ///     Called when a chore is completed.
    /// </summary>
    void _TaskCollection::_NotifyCompletedChoreAndFree(_UnrealizedChore * pChore)
    {
        // Check if the chore needs to be freed.
        if (pChore->_GetRuntimeOwnsLifetime())
        {
            _UnrealizedChore::_InternalFree(pChore);
        }

        // Save the member variables as locals since task collection could be deleted
        // as soon as the event is set.
        TaskProc completionCallback = _M_completionHandler;
        void * completionContext = _M_pCompletionContext;

        long val = InterlockedDecrement(&_M_unpoppedChores);
        ASSERT(val >= 0);
        //
        // Because the task collection can be passed between threads and waited upon, any transition from 1->0 needs to wake an arbitrary set
        // of threads, hence -- this needs a fence.
        //
        if (val == 0)
        {
            //
            // No games need be played here. Anyone who pushes a chore will see the event clear and wait before pushing it upon the WSQ. This
            // means there can be no race with messing up the event state. Setting the event suffices.
            //
            _M_event.set();
        }

        if (completionCallback != NULL)
        {
            completionCallback(completionContext);
        }
    }

    /// <summary>
    ///     Perform a wait on every alias. Note that we make no attempt to inline any of the executions of things pushed on other threads. We merely
    ///     wait for them. They'll be stolen and executed eventually.
    /// </summary>
    /// <param name="pSnapPoint">
    ///     The snapshot point which indicates which aliases are involved in the wait
    /// </param>
    void _TaskCollection::_FullAliasWait(_TaskCollection *pSnapPoint)
    {
        _TaskCollection *pAlias = pSnapPoint;
        int count = 0;
        while (pAlias != NULL)
        {
            count++;
            pAlias = pAlias->_M_pNextAlias;
        }

        if (count > 0)
        {
            _MallocaArrayHolder<event *> mholder;
            event **pEvents = mholder._InitOnRawMalloca(_malloca(sizeof (event *) * (count + 1)));

            pEvents[0] = &(_M_pOriginalCollection->_M_event);

            int i = 1;
            pAlias = pSnapPoint;
            while (i < count + 1)
            {
                pEvents[i] = &(pAlias->_M_event);
                i++;
                pAlias = pAlias->_M_pNextAlias;
            }

            event::wait_for_multiple(pEvents, (count + 1), true);
        }
        else
        {
            _M_event.wait();
        }
    }

    /// <summary>
    ///     Schedules a new unstructured chore upon an unstructured task collection
    /// </summary>
    /// <param name="pChore">
    ///     The new unrealized chore to schedule
    /// </param>
    /// <param name="_PLocation">
    ///     The location where the unrealized chore should execute. Specifying the value NULL here indicates that the unrealized chore does not
    ///     have specific placement.
    /// </param>
    void _TaskCollection::_Schedule(_UnrealizedChore *pChore, location *_PLocation)
    {
        if (pChore->_M_pTaskCollection != NULL)
            throw invalid_multiple_scheduling();

        try
        {
            _TaskCollection *pAlias = _Alias();

            pChore->_M_pTaskCollection = pAlias;
            pChore->_M_pChoreFunction = &_UnrealizedChore::_UnstructuredChoreWrapper;

            ASSERT(pAlias->_M_stackPos >= 0); // Satisfy static analyzers that might assume _M_unpoppedChores could be negative (it's signed).
            int locationBase = pAlias->_M_stackPos++;
            if (locationBase >= SIZEOF_ARRAY(pAlias->_M_taskCookies))
            {
                //
                // We've spilled outside the allowable internal allocation of tasks (this is largely an optimization to avoid
                // heap allocations on typically sized task collections).
                //
                TaskStack *pStack = reinterpret_cast<TaskStack *> (pAlias->_M_pTaskExtension);
                if (pStack == NULL)
                {
                    pStack = _concrt_new TaskStack();
                    pAlias->_M_pTaskExtension = pStack;
                }

                pAlias->_NotifyNewChore();
                //
                // ctor has already guaranteed context exists
                //
                if (!pStack->Push(SchedulerBase::FastCurrentContext()->PushUnstructured(pChore, _PLocation)))
                {
                    //
                    // It's not on the inlining list -- it must be stolen!  This is due to the cap being reached (see comments in
                    // TaskStack).
                    //
                    pAlias->_M_stackPos--;
                }
            }
            else
            {
                pAlias->_NotifyNewChore();
                //
                // ctor has already guarenteed context exists
                //
                ASSERT(locationBase < SIZEOF_ARRAY(pAlias->_M_taskCookies));
                pAlias->_M_taskCookies[locationBase] = SchedulerBase::FastCurrentContext()->PushUnstructured(pChore, _PLocation);
            }
        }
        catch (...)
        {
            //
            // We are responsible for the freeing of the chore. If any exception was thrown out, we didn't schedule it and hence
            // won't free it later. It must be done now.
            //
            if (pChore->_GetRuntimeOwnsLifetime())
            {
                _UnrealizedChore::_InternalFree(pChore);
            }
            throw;
        }
    }

    /// <summary>
    ///     Schedules a new unstructured chore upon an unstructured task collection
    /// </summary>
    /// <param name="pChore">
    ///     The new unrealized chore to schedule
    /// </param>
    void _TaskCollection::_Schedule(_UnrealizedChore *pChore)
    {
        if (pChore->_M_pTaskCollection != NULL)
            throw invalid_multiple_scheduling();

        try
        {
            _TaskCollection *pAlias = _Alias();

            pChore->_M_pTaskCollection = pAlias;
            pChore->_M_pChoreFunction = &_UnrealizedChore::_UnstructuredChoreWrapper;

            ASSERT(pAlias->_M_stackPos >= 0); // Satisfy static analyzers that might assume _M_unpoppedChores could be negative (it's signed).
            int locationBase = pAlias->_M_stackPos++;
            if (locationBase >= SIZEOF_ARRAY(pAlias->_M_taskCookies))
            {
                //
                // We've spilled outside the allowable internal allocation of tasks (this is largely an optimization to avoid
                // heap allocations on typically sized task collections).
                //
                TaskStack *pStack = reinterpret_cast<TaskStack *> (pAlias->_M_pTaskExtension);
                if (pStack == NULL)
                {
                    pStack = _concrt_new TaskStack();
                    pAlias->_M_pTaskExtension = pStack;
                }

                pAlias->_NotifyNewChore();
                //
                // ctor has already guaranteed context exists
                //
                if (!pStack->Push(SchedulerBase::FastCurrentContext()->PushUnstructured(pChore)))
                {
                    //
                    // It's not on the inlining list -- it must be stolen!  This is due to the cap being reached (see comments in
                    // TaskStack).
                    //
                    pAlias->_M_stackPos--;
                }
            }
            else
            {
                pAlias->_NotifyNewChore();
                //
                // ctor has already guarenteed context exists
                //
                ASSERT(locationBase < SIZEOF_ARRAY(pAlias->_M_taskCookies));
                pAlias->_M_taskCookies[locationBase] = SchedulerBase::FastCurrentContext()->PushUnstructured(pChore);
            }
        }
        catch (...)
        {
            //
            // We are responsible for the freeing of the chore. If any exception was thrown out, we didn't schedule it and hence
            // won't free it later. It must be done now.
            //
            if (pChore->_GetRuntimeOwnsLifetime())
            {
                _UnrealizedChore::_InternalFree(pChore);
            }
            throw;
        }
    }

    /// <summary>
    ///     Resets the task collection for future usage.
    /// </summary>
    /// <param name="pSnapPoint">
    ///     The snapshot from which to reset
    /// </param>
    void _TaskCollection::_Reset(_TaskCollection *pSnapPoint)
    {
        //
        // Clear the cancellation flag. Note that if a cancellation was done for the context, we must clear the collection cancel flag. This is only
        // safe to do for the owning context. If the execution status indicates any kind of inlining, the owning context is in the midst of a
        // _Abort, _Reset, or Wait and will take care of itself. Only on non-inline status do we need to do something cross thread. The only state
        // to which that applies is TASKCOLLECTION_EXECUTION_STATUS_CANCEL_DEFERRED.
        //
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        ContextBase *pOwningContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);

        LONG capturedStatus = _M_executionStatus;

        for(;;)
        {
            if (capturedStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS)
            {
                //
                // If someone is in the middle of canceling, we must let them proceed until they've reached the point where the cancellation
                // of the context happens. Spin wait. Note that if we do not do this, it's entirely possible that we check cancellation
                // of the context below BEFORE they cancel it, they cancel it, and some arbitrary task collection gets canceled instead of the one
                // intended on the inline side.
                //
                _SpinWaitBackoffNone spinWait;
                while (_M_executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS)
                {
                    spinWait._SpinOnce();
                }

                capturedStatus = _M_executionStatus;
                continue;
            }

            long xchgVal;

            if (pCurrentContext == pOwningContext)
            {
                xchgVal = InterlockedCompareExchange(&_M_executionStatus,
                                                     TASKCOLLECTION_EXECUTION_STATUS_CLEAR,
                                                     capturedStatus);

                if (xchgVal == capturedStatus)
                {
                    if (xchgVal == TASKCOLLECTION_EXECUTION_STATUS_CANCEL_COMPLETE)
                    {
                        pCurrentContext->CancelCollectionComplete(_M_inliningDepth);
                    }

                    _M_inliningDepth = -1;
                    break;
                }
            }
            else
            {
                xchgVal = InterlockedCompareExchange(&_M_executionStatus,
                                                     TASKCOLLECTION_EXECUTION_STATUS_CLEAR,
                                                     TASKCOLLECTION_EXECUTION_STATUS_CANCEL_DEFERRED);

                if (xchgVal != TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS)
                {
                    break;
                }
            }

            capturedStatus = xchgVal;
        }

        //
        // If there are direct aliases, we must clear those up too.
        //
        if (!_IsAlias())
        {
            if (pSnapPoint)
            {
                _TaskCollection *pAlias = pSnapPoint;
                while (pAlias)
                {
                    if (!pAlias->_IsStaleAlias())
                    {
                        pAlias->_Reset(NULL);
                    }
                    pAlias = pAlias->_M_pNextAlias;
                }
            }
            //
            // Any caught exception on the collection should be rethrown on this thread. The exception can be one of several things:
            //
            // _Interruption_exception (or another internal runtime exception):
            //
            //     - We want to let this exception continue propagating unless there's a *more important* one (like an arbitrary exception) that occurred
            //       elsewhere. There is an unfortunate situation here:
            //
            //         o We might be within a destructor. Here, by the C++ standard, we cannot throw a different exception or the
            //           process will terminate. This is unfortunate because it might be better to throw one of the exceptions
            //           which did happen. You might run into code like this where you have
            //
            //               try
            //               {
            //                   *_TaskCollection tp;
            //                   tp.Schedule(t1); // throws e1
            //                   tp.Schedule(t2); // throws e2
            //
            //                   // arbitrary code with an interruption point that causes _Interruption_exception to be thrown.
            //
            //                   tp.Wait();
            //               }
            //               catch (...) { }
            //
            // an arbitrary exception:
            //
            //     - We are allowed to choose an arbitrary exception to flow back.
            //
            long exitCode = InterlockedExchange(&_M_exitCode, 0);
            if ((exitCode & EXIT_STATUS_FLAG_EXCEPTION_RAISED) != 0)
            {
                _SpinWaitBackoffNone spinWait;
                while ((size_t) _M_pException == _S_nonNull) // make sure the exception is ready
                    spinWait._SpinOnce();
                _RethrowException();
            }
        }
        else
        {
            //
            // A reset of the alias must reset the overall collection.
            //
            if (_IsDirectAlias() && pSnapPoint != NULL)
                _M_pOriginalCollection->_Reset(pSnapPoint);
        }
    }

    /// <summary>
    ///     Called when the task collection is canceled via a cancellation token.
    /// </summary>
    void _TaskCollection::_CancelViaToken(_TaskCollection *pCollection)
    {
        pCollection->_Cancel();
    }

    /// <summary>
    ///     Runs a specified chore (pChore) and subsequently waits on all chores associated with the task collection
    ///     to execute.
    /// </summary>
    /// <param name="pChore">
    ///     The chore to run locally.
    /// </param>
    /// <returns>
    ///     An indication of the status of the wait.
    /// </returns>
    _TaskCollectionStatus __stdcall _TaskCollection::_RunAndWait(_UnrealizedChore *pChore)
    {
        ASSERT(!_IsDirectAlias());

        _TaskCollection *pAlias = _Alias();
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();

        //
        // Snapshot the list of aliases so we have internal consistency between what we wait upon, what we reset, etc...
        //
        _TaskCollection *pSnapPoint = _M_pNextAlias;
        bool fOverflow = false;

        //
        // The parent context needs to be snapped here. It's possible that the executing collection on the
        // context at the time that _RunAndWait is invoked, is different from the executing collection on the current
        // context when the task collection was created.
        //
        pAlias->_M_pParent = pCurrentContext->GetExecutingCollection();
        pAlias->_M_inliningDepth = pAlias->_M_pParent ? pAlias->_M_pParent->_InliningDepth() + 1 : 0;

        //
        // Set up the EH frame. We need to stop cancellation propagation when we hit someone who
        // has become canceled.
        //
        pCurrentContext->SetExecutingCollection(pAlias);

        //
        // The token on this collection is used interchangeably with the alias token (if the alias is not 'this'), so they must match.
        //
        ASSERT(pAlias->_M_pTokenState == _M_pTokenState);
        //
        // Handle any token which might be present. We only need to register for callbacks on token boundaries.
        //
        _CancellationTokenRegistration *pRegistration = NULL;
        if (_M_pTokenState != NULL)
        {
            if (_M_pTokenState != _CancellationTokenState::_None() && _M_pTokenState != pCurrentContext->GetGoverningTokenState())
            {
                pRegistration = _M_pTokenState->_RegisterCallback(
                    reinterpret_cast<TaskProc>(_TaskCollection::_CancelViaToken), this
                    );
            }
            pCurrentContext->PushGoverningTokenState(_M_pTokenState, pAlias->_M_inliningDepth);
        }

        try
        {
            //
            // This *MUST* be fenced due to allowing cancellation from arbitrary threads. The cancellation routine may have switched
            // to deferred cancellation based on us not being inline. We cannot arbitrarily overwrite that result.
            //
            LONG xchgStatus = InterlockedCompareExchange(&pAlias->_M_executionStatus, TASKCOLLECTION_EXECUTION_STATUS_INLINE, TASKCOLLECTION_EXECUTION_STATUS_CLEAR);
            if (xchgStatus == TASKCOLLECTION_EXECUTION_STATUS_CANCEL_DEFERRED)
            {
                //
                // The catch block will expect this.
                //
                if (pChore != NULL)
                    pAlias->_NotifyNewChore();
                throw _Interruption_exception();
            }

            if (pChore != NULL)
            {
                pAlias->_NotifyNewChore();

                if (_IsMarkedForAbnormalExit() || (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(pAlias)))
                {
                    throw _Interruption_exception();
                }

                pChore->m_pFunction(pChore);
                pChore->_M_pTaskCollection = NULL;
                pAlias->_NotifyCompletedChoreAndFree(pChore);
                pChore = NULL;
            }

            for(;;)
            {
                TaskStack *pStack;

                while (pAlias->_M_stackPos > 0)
                {
                    //
                    // The _IsMarkedForAbnormalExit() is a necessary semantic (pass a canceled task collection to a new thread -- this is the only check that
                    // will prevent stuff from going onto it prior to a reset). It's also necessary to check the exit code on the original collection  because
                    // we could have a scenario where a chore is stolen from a direct alias which then pushes chores back to the original collection. This will
                    // result in an indirect alias being used and the stealing won't see the alias inlined. Hence -- waiting on the indirect alias cannot be canceled.
                    //
                    if (_IsMarkedForAbnormalExit() || (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(pAlias)))
                    {
                        throw _Interruption_exception();
                    }

                    int taskCookie;

                    if (pAlias->_M_stackPos > SIZEOF_ARRAY(pAlias->_M_taskCookies))
                    {
                        pStack = reinterpret_cast<TaskStack *>(pAlias->_M_pTaskExtension);
                        ASSERT(!pStack->IsEmpty());
                        taskCookie = pStack->Pop();
                    }
                    else
                    {
                        taskCookie = pAlias->_M_taskCookies[pAlias->_M_stackPos - 1];
                    }

                    pAlias->_M_stackPos--;

                    pChore = pCurrentContext->TryPopUnstructured(taskCookie);
                    if (pChore == NULL)
                    {
                        //
                        // If we failed because something was stolen, everything underneath us was stolen as well and the wait on stolen chores
                        // will guarantee that we wait on everything necessary. We can clear out the stack to prevent reuse of the task collection
                        // from just building up excess entries.
                        //
                        TaskStack *pStack = reinterpret_cast<TaskStack *> (pAlias->_M_pTaskExtension);
                        if (pStack != NULL) pStack->Clear();
                        pAlias->_M_stackPos = 0;

                        break;
                    }

                    if (pChore == reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
                        continue;

                    if (pCurrentContext->IsExternal())
                        static_cast<ExternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();
                    else
                        static_cast<InternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();

                    pChore->m_pFunction(pChore);
                    pChore->_M_pTaskCollection = NULL;
                    pAlias->_NotifyCompletedChoreAndFree(pChore);
                    pChore = NULL;
                }

                //
                // If the task stack overflowed, there are potentially still items on the work stealing queue we could not inline. If we simply
                // block without care and one of those items cancels, we can deadlock (since we cannot steal from canceled contexts). If the
                // stack overflowed, we need to perform special handling.
                //
                pStack = reinterpret_cast<TaskStack *>(pAlias->_M_pTaskExtension);
                if (pStack != NULL && pStack->Overflow())
                {
                    fOverflow = true;

                    //
                    // We need to tell the canceling thread to perform the WSQ sweep or do ourselves as determined by a CAS.
                    //
                    LONG xchgStatus = InterlockedCompareExchange(&pAlias->_M_executionStatus,
                                                                 TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK,
                                                                 TASKCOLLECTION_EXECUTION_STATUS_INLINE);

                    switch(xchgStatus)
                    {
                        case TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS:
                        case TASKCOLLECTION_EXECUTION_STATUS_CANCEL_COMPLETE:
                            throw _Interruption_exception();
                        default:
                            break;
                    }
                }

                _FullAliasWait(pSnapPoint);

                if (fOverflow)
                {
                    //
                    // We cannot *EVER* touch the work stealing queue if another context has canceled and is sweeping it for cancellation.
                    // CAS back to INLINE. If the CAS turns up INLINE_CANCEL_IN_PROGRESS, another thread is playing with our WSQ and we must spin
                    // until that's done.
                    //
                    // Note that this path should be rather rare and requires the use both of direct aliasing (passing between threads) **AND** pushing
                    // more than the task collection cap onto a single alias (1026 tasks) before the wait operation.
                    //
                    if (InterlockedCompareExchange(&pAlias->_M_executionStatus,
                                               TASKCOLLECTION_EXECUTION_STATUS_INLINE,
                                               TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK) ==
                                               TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS)
                    {
                        _SpinWaitBackoffNone spinWait;
                        while(_M_executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS)
                        {
                            spinWait._SpinOnce();
                        }
                    }
                }

                //
                // It is entirely possible that we took a snapshot and during the execution of a chore on this task collection, the task collection
                // was passed to another thread that has not yet touched the task collection (be it an arbitrary one or an N-level descendent
                // (N > 1). In this case, a new alias was created and we did not see it in the snapshot. We cannot know until after
                // the _FullAliasWait call. If the snap point has changed, we must loop around or we will miss waiting on chores that
                // were created on other threads during execution of a chore which was known about. This would be contrary to user expectation.
                //
                if (pSnapPoint == _M_pNextAlias)
                    break;

                pSnapPoint = _M_pNextAlias;

            }
        }
        catch (const _Interruption_exception &)
        {
            if (pChore != NULL && pChore != reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
            {
                pChore->_M_pTaskCollection = NULL;
                pAlias->_NotifyCompletedChoreAndFree(pChore);
            }
            //
            // This exception will be rethrown to a higher level if cancellation is still triggered on this context. In order to conserve
            // stack space on x64 and consolidate this path with the exception path, the rethrow happens below outside this particular
            // catch.
            //
            pAlias->_RaisedCancel();
        }
        catch(...)
        {
            if (pChore != NULL && pChore != reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
            {
                pChore->_M_pTaskCollection = NULL;
                pAlias->_NotifyCompletedChoreAndFree(pChore);
            }

            pAlias->_RaisedException();
        }

        if (_M_pTokenState != NULL)
        {
            pCurrentContext->PopGoverningTokenState(_M_pTokenState);
            if (pRegistration != NULL)
            {
                _M_pTokenState->_DeregisterCallback(pRegistration);
                pRegistration->_Release();
            }
        }

        pCurrentContext->SetExecutingCollection(pAlias->_M_pParent);

        if (_IsMarkedForAbnormalExit())
        {
            //
            // _Abort invokes _Reset, which will rethrow a user exception that was caught either in the catch blocks above and in
            // _UnrealizedChore::_UnstructuredChoreWrapper for a stolen chore. _Interruption_exception is not thrown here. That exeption is only
            // thrown if a cancellation is visible after _Abort has returned.
            //
            pAlias->_Abort();
            //
            // _Abort will undo the effect of cancellations at this level, therefore HasAnyCancellations() and IsCancellationVisible() from this
            // refer to cancellation at a higher level.
            //
            if (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(pAlias, _M_pTokenState != NULL))
            {
                throw _Interruption_exception();
            }
            return _Canceled;
        }

        pAlias->_Reset(pSnapPoint);
        //
        // Similar to the structured task collection, if there is a cancellation at a higher level the interruption exception should be throwns
        // here since this is an interruption point.
        //
        if (pCurrentContext->HasAnyCancellation() && pCurrentContext->IsCancellationVisible(pAlias, _M_pTokenState != NULL))
        {
            throw _Interruption_exception();
        }

        return _Completed;
    }

    /// <summary>
    ///     Performs an abortive sweep of the WSQ for inline stack overflow.
    /// </summary>
    /// <param name="_PCtx">
    ///     The context to sweep
    /// </param>
    void _TaskCollection::_AbortiveSweep(void *_PCtx)
    {
        ContextBase *pContext = reinterpret_cast<ContextBase *>(_PCtx);

        SweeperContext ctx(this);
        pContext->SweepUnstructured(reinterpret_cast<WorkStealingQueue<_UnrealizedChore>::SweepPredicate> (_TaskCollection::_CollectionMatchPredicate),
                                    &ctx,
                                    &_TaskCollection::_SweepAbortedChore);

        //
        // Update the statistical information with the fact that a task has been dequeued
        //
        if (ctx.m_sweptChores > 0)
        {
            ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();

            if (pCurrentContext->IsExternal())
                static_cast<ExternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter(ctx.m_sweptChores);
            else
                static_cast<InternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter(ctx.m_sweptChores);
        }
    }

    /// <summary>
    ///     A predicate function checking whether a given chore belongs to a given collection.
    /// </summary>
    /// <param name="_PChore">
    ///     The chore to check
    /// </param>
    /// <param name="_PData">
    ///     The data to check against
    /// </param>
    /// <returns>
    ///     Whether or not the chore belongs to the collection
    /// </returns>
    bool _TaskCollection::_CollectionMatchPredicate(_UnrealizedChore *_PChore, void *_PData)
    {
        SweeperContext *pCtx = reinterpret_cast<SweeperContext *>(_PData);
        return (_PChore->_M_pTaskCollection == pCtx->m_pTaskCollection);
    }

    /// <summary>
    ///     Called to sweep an aborted chore in the case of inline stack overflow.
    /// </summary>
    /// <param name="_PChore">
    ///     The chore to sweep
    /// </param>
    /// <param name="_PData">
    ///     The data which was passed into the sweeper predicate
    /// </param>
    /// <returns>
    ///     An indication of whether the chore is now gone
    /// </returns>
    bool _TaskCollection::_SweepAbortedChore(_UnrealizedChore *_PChore, void *_PData)
    {
        SweeperContext *pCtx = reinterpret_cast<SweeperContext *>(_PData);
        _TaskCollection *pCollection = static_cast<_TaskCollection *>(_PChore->_M_pTaskCollection);

        //
        // Aggregate the number of chores that were aborted so that the dequeued task counter
        // can be updated appropriately.
        //
        pCtx->m_sweptChores++;
        pCollection->_NotifyCompletedChoreAndFree(_PChore);

        return true;
    }

    /// <summary>
    ///     Aborts chores related to the task collection and waits for those which cannot be forcibly aborted.
    /// </summary>
    /// <param name="fLeaveCanceled">
    ///     An indication as to whether or not to leave the task collection canceled after the abort.
    /// </param>
    void _TaskCollection::_Abort(bool fLeaveCanceled /* = false */)
    {
        //
        // ctor has already guaranteed context exists
        //
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        TaskStack *pStack = reinterpret_cast<TaskStack *> (_M_pTaskExtension);

        _TaskCollection *pSnapPoint = _IsIndirectAlias() ? NULL : _M_pOriginalCollection->_M_pNextAlias;

        //
        // If the stack hasn't overflowed, do this the "efficient way".
        //
        if (pStack == NULL || !pStack->Overflow())
        {
            while (_M_stackPos > 0)
            {
                int taskCookie;

                if (_M_stackPos > SIZEOF_ARRAY(_M_taskCookies))
                {
                    ASSERT(!pStack->IsEmpty());
                    taskCookie = pStack->Pop();
                }
                else
                    taskCookie = _M_taskCookies[_M_stackPos - 1];

                _M_stackPos--;

                _UnrealizedChore *pChore = static_cast<_UnrealizedChore *> (pCurrentContext->TryPopUnstructured(taskCookie));
                if (pChore == NULL)
                    break;

                if (pChore == reinterpret_cast<_UnrealizedChore *>(AFFINITY_EXECUTED))
                    continue;

                //
                // Update the statistical information with the fact that a task has been dequeued
                //
                if (pCurrentContext->IsExternal())
                    static_cast<ExternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();
                else
                    static_cast<InternalContextBase *>(pCurrentContext)->IncrementDequeuedTaskCounter();

                pChore->_M_pTaskCollection = NULL;
                _NotifyCompletedChoreAndFree(pChore);
            }
        }
        else
        {
            //
            // Because we've overflowed the inlining stack, some chores that were pushed onto this collection are unknown. This means we can't abort by popping
            // known ones and waiting for stolen ones. Stealing is not allowed until the _Reset call to avoid infighting. The unknown chores -- if still on the WSQ --
            // would deadlock a normal Abort. Instead, we sweep the ENTIRE work stealing queue looking for chores associated with this collection and remove them.
            // This is very inefficient compared to the above. It does, however, only happen if you cancel a task collection onto which greater than the inline cap
            // chores have been pushed.
            //
            _AbortiveSweep(pCurrentContext);
            pStack->ResetOverflow();
        }

        //
        // Only take the penalty of lock and traversal if there are stolen chores or direct aliases. This is what allows the transitive
        // takedown of stolen chores as well as the takedown of aliases.
        //
        for(;;)
        {
            if (fLeaveCanceled || _M_unpoppedChores > 0 || _IsDirectAlias() || pSnapPoint != NULL)
            {
                _M_pOriginalCollection->_Cancel(false, pSnapPoint);
            }
            _FullAliasWait(pSnapPoint);

            //
            // If the snap point changed, it's always possible that one of the aliases waited upon passed to a new thread. It's further possible
            // that the underlying client code guarantees that the wait on the collection transitively encapsulates the scheduling of the work.
            // In that case, we should probably go back and cancel much as we do for wait.
            //
            _TaskCollection *pNewSnapPoint = _IsIndirectAlias() ? NULL : _M_pOriginalCollection->_M_pNextAlias;

            if (pSnapPoint == pNewSnapPoint)
                break;

            pSnapPoint = pNewSnapPoint;
        }

        _M_stackPos = 0;
        if (pStack != NULL)
            pStack->Clear();

        if (!fLeaveCanceled)
            _Reset(pSnapPoint);
    }

    /// <summary>
    ///     Cancels work on the task collection.
    /// </summary>
    void _TaskCollection::_Cancel()
    {
        _M_pOriginalCollection->_Cancel(false, _M_pNextAlias);
    }

    /// <summary>
    ///     Performs an arbitrary thread cancellation for a single taskcollection/alias.
    /// </summary>
    void _TaskCollection::_CancelFromArbitraryThread(bool insideException)
    {
        LONG executionStatus = _M_executionStatus;
        LONG xchgStatus = TASKCOLLECTION_EXECUTION_STATUS_CLEAR;

        for (;;)
        {
            switch (executionStatus)
            {
                case TASKCOLLECTION_EXECUTION_STATUS_CLEAR:
                    //
                    // If it's not inlined, we must defer cancellation of the inline context. This will be cleared eventually by a Wait().
                    //
                    xchgStatus = TASKCOLLECTION_EXECUTION_STATUS_CANCEL_DEFERRED;
                    break;
                case TASKCOLLECTION_EXECUTION_STATUS_INLINE:
                case TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK:
                    //
                    // If it's inlined, we can cancel the underlying context (as long as it *IS* inlined).
                    //
                    xchgStatus = TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS;
                    break;
                default:
                    //
                    // Any other state, we do not fiddle with.
                    //
                    break;
            }

            if (xchgStatus == TASKCOLLECTION_EXECUTION_STATUS_CLEAR)
                break;

            xchgStatus = InterlockedCompareExchange(&_M_executionStatus, xchgStatus, executionStatus);
            if (xchgStatus == executionStatus)
            {
                bool fInlineInProgress = false;

                //
                // We succeeded in marking. If it wasn't a deferral (we swapped from inline), complete the cancellation of the underlying context.
                // Anything waiting on the alias will pause while we're in the middle of an in-progress cancel (which is what allows canceling the
                // context to be safe).
                //
                if (executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE ||
                    executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK)
                {
                    fInlineInProgress = true;
                    ContextBase *pContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);
                    pContext->CancelCollection(_M_inliningDepth);

                    if (executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK)
                    {
                        //
                        // Because the stack overflowed and the original thread is blocking and will do nothing that will throw the exception, we must sweep
                        // the WSQ and get rid of any chores. This is safe for several reasons:
                        //
                        // - The initial status was INLINE_WAIT_WITH_OVERFLOW_STACK. At the point where that particular status was set, we have
                        //   a guarantee that the context is waiting.
                        //
                        // - We successfully CAS'd that status to INLINE_CANCEL_IN_PROGRESS. This will prevent the original thread from progressing beyond
                        //   the _FullAliasWait and doing anything with the WSQ.
                        //
                        _AbortiveSweep(pContext);
                    }

                }

                _CancelStolenContexts(insideException, fInlineInProgress);

                if (fInlineInProgress)
                    InterlockedExchange(&_M_executionStatus, TASKCOLLECTION_EXECUTION_STATUS_CANCEL_COMPLETE);

                break;
            }
            executionStatus = xchgStatus;
        }
    }

    /// <summary>
    ///     Goes through the direct alias list and performs a cancellation of all contexts which are running chores from any alias.
    /// </summary>
    void _TaskCollection::_CancelDirectAliases(bool insideException, _TaskCollection *pSnapPoint)
    {
        ASSERT(!_IsAlias());

        _TaskCollection *pAlias = pSnapPoint;
        while (pAlias != NULL)
        {
            //
            // We *CANNOT* free the stale alias right now. Doing so will interfere with the lock free nature of this list and result in ABA.
            // Only the task collection destructor in this area is allowed to do this. Just skip stale aliases.
            //
            pAlias->_CancelFromArbitraryThread(insideException);

            pAlias = pAlias->_M_pNextAlias;
        }
    }

    /// <summary>
    ///     Cancels work on the task collection.
    /// </summary>
    /// <param name="insideException">
    ///     Indicates whether the cancellation is taking place due to exception unwinding within the runtime
    /// </param>
    /// <param name="pSnapPoint">
    ///     Identifies a snapshot within the direct alias list where the cancellation will take place. Only aliases within the snapshot are canceled.
    /// </param>
    void _TaskCollection::_Cancel(bool insideException, _TaskCollection *pSnapPoint)
    {
        //
        // There's several scenarios where we might have come into here:
        //
        // - On the thread that owns a task collection.
        // - On a context transitively stolen from the context that owns a task collection.
        // - On an abitrary thread.
        //
        // Further, the thread that owns the collection might be doing:
        //
        // - Something related to the collection
        // - Something related to a DIFFERENT collection.
        //
        // We cannot arbitrarily take down the owning context as we can with structured task collections. There's no guarantee
        // we'd be canceling the right context. If we're within a transitive steal and the thread that owns context
        // is running a Wait on the original collection, we're safe to take down the context (it'll still be within wait while
        // we're in here). If however, it's not within that collection or we're on an arbitrary thread, things get a whole
        // lot more interesting.
        //
        const _TaskCollection *pAlias = _Alias();
        ASSERT(_M_pOriginalCollection == this);
        //
        // Multiple stolen chores might cancel at the same time. We can only allow one person into the path
        // which fires down threads so the counters get set correctly.
        //
        if (_SetCancelState(EXIT_STATUS_START_CANCEL))
        {
            //
            // We cannot touch the owning context unless we are on it or we are an indirect alias.
            //
            if (pAlias->_IsIndirectAlias() || pAlias == this)
            {
                //
                // This is cancellation from a directly transitive child or on the owning thread. We do not need to play
                // games with execution state -- we can simply and safely cancel. The reasoning here is that we're guaranteed
                // that the inline status will *NOT* change during the call. Inlined means we're waiting on the collection and since
                // we're either on the owning context or a transitive steal, the wait on the collection waits on us. Since state only
                // changes after the wait, we're safe.
                //
                for (;;)
                {
                    LONG executionStatus = _M_executionStatus;
                    if (executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE ||
                        executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK)
                    {
                        //
                        // Only here are we allowed to touch the context. Now we need to determine
                        // which inline context needs to be aborted.
                        //
                        ContextBase *pContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);
                        pContext->CancelCollection(_M_inliningDepth);

                        if (executionStatus == TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK)
                        {
                            //
                            // The caller will no longer do anything with the task collection besides wait. We must sweep the WSQ (and are safe to do so because
                            // we are a stolen chore preventing the unblock from happening).
                            //
                            _AbortiveSweep(pContext);
                        }
                        InterlockedExchange(&_M_executionStatus, TASKCOLLECTION_EXECUTION_STATUS_CANCEL_COMPLETE);
                    }
                    else
                    {
                        //
                        // Note that being here doesn't mean we're not inlined. It only means we weren't inlined as far as WE COULD SEE
                        // a split second ago. It's entirely possible that it was inlined and has already done its check of _M_exitCode.
                        // In order to push all the weight to the cancellation side, there's a multi-phase approach to cancellation.
                        // Cancellation fences the exitCode to START_CANCEL and then checks the inlined flag. After checking,
                        // it fences the exit flag AGAIN to one of two states: SHOTDOWN_OWNER or DEFERRED_SHOOTDOWN_OWNER. The owning side will cancel
                        // and throw on any of these states; *HOWEVER* -- it will not propagate the exception until the state changes away from
                        // START_CANCEL and which way it changed will determine HOW it propagates the exception (how the counter manipulation needs
                        // to happen).
                        //
                        LONG xchgStatus = InterlockedCompareExchange(&_M_executionStatus, TASKCOLLECTION_EXECUTION_STATUS_CANCEL_DEFERRED, TASKCOLLECTION_EXECUTION_STATUS_CLEAR);
                        if (xchgStatus == executionStatus)
                            break;

                        executionStatus = xchgStatus;
                    }
                }
                _CancelStolenContexts(insideException, true);
            }
            else
            {
                _CancelFromArbitraryThread(insideException);
            }
            _CancelDirectAliases(insideException, pSnapPoint);
        }
    }

    /// <summary>
    ///     Called when an exception is raised on a chore on an unstructured task collection, this makes a determination of what to do with the exception
    ///     and stores it for potential transport back to the thread performing a join on a task collection.
    /// </summary>
    void _TaskCollection::_RaisedException()
    {
        _M_pOriginalCollection->_TaskCollectionBase::_RaisedException();

        //
        // _M_exitCode may be set by more than one thread
        //
        InterlockedOr(&(static_cast<_TaskCollection*> (_M_pOriginalCollection)->_M_exitCode), EXIT_STATUS_FLAG_EXCEPTION_RAISED);
    }

    /// <summary>
    ///     Called when an exception is raised on a chore on an unstructured task collection, this makes a determination of what to do with the exception
    ///     and stores it for potential transport back to the thread performing a join on a task collection.
    /// </summary>
    void _TaskCollection::_RaisedCancel()
    {
        _M_pOriginalCollection->_TaskCollectionBase::_RaisedCancel();

        //
        // _M_exitCode may be set by more than one thread
        //
        InterlockedOr(&(static_cast<_TaskCollection*> (_M_pOriginalCollection)->_M_exitCode), EXIT_STATUS_FLAG_CANCELLATION_RAISED);
    }

    /// <summary>
    ///     Informs the caller whether or not the task collection is currently in the midst of a cancellation. Note that this
    ///     does not necessarily indicate that Cancel was called on the collection (although such certainly qualifies this function
    ///     to return true). It may be the case that the task collection is executing inline and a task collection further up in the work
    ///     tree was canceled. In cases such as these where we can determine ahead of time that cancellation will flow through
    ///     this collection, true will be returned as well.
    /// </summary>
    /// <returns>
    ///     An indication of whether the task collection is in the midst of a cancellation (or is guaranteed to be shortly).
    /// </returns>
    bool _TaskCollection::_IsCanceling()
    {
        //
        // Right off the bat is the "easy" one -- if the task collection itself has been canceled we know we can answer the question immediately.
        // Note that the execution status of the alias is irrelevant to this question for now _M_exitCode of the original collection propagates to execution
        // status of the aliases.
        //
        if (_M_exitCode != 0) return true;

        //
        // If our token is canceled, flag us immediately.
        //
        if (_CancellationTokenState::_IsValid(_M_pTokenState) && _M_pTokenState->_IsCanceled())
        {
            _Cancel();
            return true;
        }

        //
        // It is slightly more difficult to answer the question the is someone higher than us in the work tree canceled to return a definitive answer
        // here. That's because we can pass task collections between arbitrary threads and it might be inlined on an arbitrary number of threads which would need
        // checked. Worse yet -- those contexts aren't guaranteed to be around unless we take locks and make validity checks. Since this is designed
        // to be a mechanism which can be polled, taking an arbitrary number of locks to return a more determinstic answer isn't what we want. We will return
        // an *OPTIMISTIC* answer -- one that we can answer QUICKLY.
        //
        _TaskCollection *pAlias = _Alias();

        //
        // We can always check the *CURRENT* thread since it's not going away while we're a frame on its stack. We can also check the original collection if we're
        // an indirect alias (though not for a direct one).
        //
        ContextBase *pOwningContext = reinterpret_cast <ContextBase *> (pAlias->_M_pOwningContext);
        if ((pAlias->_IsCurrentlyInlined() && pOwningContext->IsCanceledAtDepth(pAlias)) ||
                    (pOwningContext->HasPendingCancellation() && pAlias->_WillInterruptForPendingCancel()))
            return true;

        if (pAlias->_IsIndirectAlias())
        {
            ASSERT(pAlias->_M_pOriginalCollection == this);
            pOwningContext = reinterpret_cast <ContextBase *> (_M_pOwningContext);

            if ((_IsCurrentlyInlined() && pOwningContext->IsCanceledAtDepth(this)) ||
                        (pOwningContext->HasPendingCancellation() && _WillInterruptForPendingCancel()))
                return true;
        }

        //
        // There are additional cases where we could return true, but they are far too expensive. You could check inlining status on every thread that has ever touched
        // the task collection and perform a depth comparison. Unfortunately, as mentioned, this takes a large number of locks, so we take the optimistic tact. If someone
        // polls, an exception will eventually propagate there and we'll return true in one of the above cases.
        //
        return false;
    }

    /// <summary>
    ///     Returns the steal tracking list.
    /// </summary>
    void *_TaskCollection::_GetStealTrackingList() const
    {
        return (void *)_M_stealTracker;
    }

    /// <summary>
    ///     Initializes the task collection to count stolen chores.
    /// </summary>
    void _TaskCollection::_Initialize()
    {
        _M_activeStealersForCancellation = 0;
        _M_exitCode = 0;
        _M_chaining = 0;
        static_assert(sizeof(SafeRWList<ListEntry>) <= sizeof(_M_stealTracker), "size of _M_stealTracker too small for list entry");
        new(_M_stealTracker) SafeRWList<ListEntry>();
    }

    /// <summary>
    ///     Called in order to set the cancellation status of the collection.
    /// </summary>
    /// <param name="_Status">
    ///     The cancellation status to set
    /// </param>
    /// <returns>
    ///     An indication of whether the set succeeded. The set will fail if the task collection already has a cancellation status.
    /// </returns>
    bool _TaskCollection::_SetCancelState(long _Status)
    {
        long oldStatus = _M_exitCode;
        while((oldStatus & EXIT_CANCELLATION_MASK) == 0)
        {
            long xchgStatus = InterlockedCompareExchange(&_M_exitCode, _Status | (oldStatus & ~EXIT_CANCELLATION_MASK), oldStatus);
            if (xchgStatus == oldStatus)
            {
                return true;
            }
            oldStatus = xchgStatus;
        }
        return false;
    }

    /// <summary>
    ///     Called to cancel any contexts which stole chores from the given collection. This is *PART* of a cancellation
    ///     scheme. The remainder must be handled by the derived class in particular. This should be called last.
    /// </summary>
    void _TaskCollection::_CancelStolenContexts(bool, bool fInlineGated)
    {
        //
        // Terminate any contexts running stolen chores.
        //
        SafeRWList<ListEntry> *pList = reinterpret_cast<SafeRWList<ListEntry> *> (_M_stealTracker);
        {
            SafeRWList<ListEntry>::_Scoped_lock_read readLock(*pList);
            //
            // Most of the time, the task collection based list will be empty (it will only not upon detachment). We need to
            // go to the context list. Allowing all the passing between threads and detachment, however, means that we cannot guarantee
            // that pContext is valid to touch. We must first validate that before we walk there. Here is how we accomplish that:
            //
            // - First, if a chore is stolen from a work queue that's detached, it's flagged as detached and the steal chain goes onto the task collection list
            //
            // - Second, a chore stolen from a non-detached work queue puts a temporary reference count on the context which is removed AFTER it is added
            //       to the context list. The owning context cannot go away while the reference count is non-zero.
            //
            // - Third, under pList's write lock, the stealing context will increment _M_activeStealersForCancellation and will decrement it upon completion
            //       (again under the same write lock). With respect to this lock, the decrement and the removal from the CONTEXT LIST **OR** TASK LIST
            //       are atomic.
            //
            // - Fourth, when a context dies, it transfers everything from its lists to the task collection lists under BOTH its lock and pList's lock.
            //
            // This means that if _M_activeStealersForCancellation > 0, there is still an active stolen chore. As long as this is true and it hasn't yet
            // moved to pList from the context's list, the context is guaranteed to be safe. Since everything atomically moves from the context list to
            // pList under pList's write lock, we can simply check pList's count to validate the second.
            //
            if (fInlineGated || (_M_activeStealersForCancellation > 0 && pList->Empty()))
            {
                ContextBase *pContext = reinterpret_cast<ContextBase *>(_M_pOwningContext);
                pContext->CancelStealers(this);
            }

            ListEntry *pLE = pList->First();
            while (pLE != NULL)
            {
                InternalContextBase *pContext = CONTAINING_RECORD(pLE, InternalContextBase, m_stealChain);
                pContext->CancelEntireContext();
                pContext->CancelStealers(NULL);
                pLE = pList->Next(pLE);
            }
        }
    }

    /// <summary>
    ///     Registers a notification handler for completion of chores
    /// </summary>
    /// <param name="_Func">
    ///     The callback function
    /// </param>
    /// <param name="_PCompletionContext">
    ///     The completion context for the callback function
    /// </param>
    void _TaskCollection::_RegisterCompletionHandler(TaskProc _CompletionHandler, void * _PCompletionContext)
    {
        _M_completionHandler = _CompletionHandler;
        _M_pCompletionContext = _PCompletionContext;
    }

    /// <summary>
    ///     Constructs a new task collection whose cancellation is governed by the specified cancellation token state.
    /// </summary>
    /// <param name="_PTokenState">
    ///     When this cancellation token is canceled, the task collection is canceled.
    /// </param>
    /// <returns>
    ///     Pointer to a new instance of _AsyncTaskCollection.
    /// </returns>
    _AsyncTaskCollection * __cdecl _AsyncTaskCollection::_NewCollection(_CancellationTokenState *_PTokenState)
    {
        return new _AsyncTaskCollection(_PTokenState);
    }

    /// <summary>
    ///     Constructs a new task collection whose cancellation is governed by the specified cancellation token state.
    /// </summary>
    /// <param name="_PTokenState">
    ///     When this cancellation token is canceled, the task collection is canceled.
    /// </param>
    _AsyncTaskCollection::_AsyncTaskCollection(_CancellationTokenState *_PTokenState) :
    _M_taskCollection(_PTokenState)
    {
        _M_taskCollection._RegisterCompletionHandler(reinterpret_cast<TaskProc>(&_AsyncTaskCollection::_CompletionHandler), this);
    }

    /// <summary>
    ///     Delete this instance of the task collection
    /// </summary>
    void _AsyncTaskCollection::_Destroy()
    {
        delete this;
    }

    /// <summary>
    ///     Called when a chore is completed.
    /// </summary>
    void _AsyncTaskCollection::_NotificationHandler()
    {
        _Release();
    }

    /// <summary>
    ///     Chore execution completion callback
    /// </summary>
    __declspec(noinline)
    void __cdecl _AsyncTaskCollection::_CompletionHandler(void * _PCompletionContext)
    {
        _AsyncTaskCollection * asyncCollection = static_cast<_AsyncTaskCollection *>(_PCompletionContext);
        asyncCollection->_NotificationHandler();
    }

} // namespace details

} // namespace Concurrency
