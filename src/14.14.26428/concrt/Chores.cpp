// ==++==
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Chores.cpp
//
// Miscellaneous implementations of things related to individuals chores
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Sets the attachment state of the chore at the time of stealing.
    /// </summary>
    void _UnrealizedChore::_SetDetached(bool _FDetached)
    {
        _M_fDetached = _FDetached;
    }

    /// <summary>
    ///     To free memory allocated with _InternalAlloc.
    /// </summary>
    void _UnrealizedChore::_InternalFree(_UnrealizedChore * _PChore)
    {
        ASSERT(_PChore->_M_fRuntimeOwnsLifetime);
        delete _PChore;
    }

    /// <summary>
    ///     Place associated task collection in a safe state.
    /// </summary>
    void _UnrealizedChore::_CheckTaskCollection()
    {
        //
        // If _M_pTaskCollection is non-NULL, the chore is still scheduled to a task collection. This is only happening
        // from a handle destructor and we have blown back through a stack based handle while it's still scheduled. We must
        // wait. The semantic we choose is that this means cancellation too.
        //
        Concurrency::details::_TaskCollectionBase *pBase = _M_pTaskCollection;
        if (pBase != NULL)
        {
            bool fThrow = false;

            if (_M_pChoreFunction == &_UnrealizedChore::_StructuredChoreWrapper)
            {
                _StructuredTaskCollection *pTaskCollection = static_cast<_StructuredTaskCollection *>(pBase);
                fThrow = !pTaskCollection->_TaskCleanup();
            }
            else
            {
                _TaskCollection *pTaskCollection = static_cast<_TaskCollection *>(pBase);
                fThrow = !pTaskCollection->_TaskCleanup(true);
            }

            if (fThrow)
                throw missing_wait();
        }
    }

    /// <summary>
    ///     Prepares for execution as a stolen chore.
    /// </summary>
    void _UnrealizedChore::_PrepareSteal(ContextBase *pContext)
    {
        if (_M_pChoreFunction == &_UnrealizedChore::_StructuredChoreWrapper)
        {
            _PrepareStealStructured(pContext);
        }
        else
        {
            _PrepareStealUnstructured(pContext);
        }
    }

    /// <summary>
    ///     Called when a stolen chore from a given cancellation token is canceled.
    /// </summary>
    void _UnrealizedChore::_CancelViaToken(ContextBase *pContext)
    {
        pContext->CancelEntireContext();
        pContext->CancelStealers(NULL);
    }

    /// <summary>
    ///     Prepares for execution as a stolen chore.
    /// </summary>
    void _UnrealizedChore::_PrepareStealStructured(ContextBase *pBaseContext)
    {
        InternalContextBase *pContext = static_cast<InternalContextBase *> (pBaseContext);

        if (pContext->GetRootCollection() == NULL)
        {
            _StructuredTaskCollection *pTaskCollection = static_cast<_StructuredTaskCollection *> (_M_pTaskCollection);
            ContextBase *pOriginContext = reinterpret_cast <ContextBase *> (pTaskCollection->_M_pOwningContext);

            pContext->SetRootCollection(pTaskCollection);

            pOriginContext->AddStealer(pContext, false);
        }
    }

    /// <summary>
    ///     Wrapper around execution of a structured chore that performs appropriate notification
    ///     and exception handling semantics.
    /// </summary>
    __declspec(noinline)
    void __cdecl _UnrealizedChore::_StructuredChoreWrapper(_UnrealizedChore * pChore)
    {
        InternalContextBase *pContext = static_cast<InternalContextBase *> (SchedulerBase::FastCurrentContext());
        // The context could be canceled if it was already prepared for steal (this happens during a block unblock race)
        ASSERT(pContext != NULL && (!pContext->HasInlineCancellation() || pContext->GetRootCollection() != NULL));

        _StructuredTaskCollection *pTaskCollection = static_cast<_StructuredTaskCollection *> (pChore->_M_pTaskCollection);
        ContextBase *pOriginContext = reinterpret_cast <ContextBase *> (pTaskCollection->_M_pOwningContext);

        pChore->_PrepareStealStructured(pContext);

        //
        // This allows cancellation of stolen chores based on a cancellation token between the declaration of a stg and its inlining.
        //
        _CancellationTokenState *pTokenState = pTaskCollection->_GetTokenState();
        _CancellationTokenRegistration *pRegistration = NULL;

        if (_CancellationTokenState::_IsValid(pTokenState))
        {
            pRegistration = pTokenState->_RegisterCallback(reinterpret_cast<TaskProc>(&_UnrealizedChore::_CancelViaToken), (ContextBase *)pContext);
        }

        try
        {
            //
            // We need to consider this a possible interruption point. It's entirely possible that we stole and raced with a
            // cancellation thread. The collection was canceled after we stole(e.g.: removed from the WSQ) but before we added ourselves
            // to the stealing chain list above. In this case, the entire context will wait until completion (bad). Immediately
            // after we go on the list (a memory barrier) we need to check the collection cancelation flag. If the collection is going away,
            // we need to get out *NOW* otherwise the entire subtree executes.
            //
            if (pTaskCollection->_IsAbnormalExit())
                throw _Interruption_exception();

            pChore->m_pFunction(pChore);
        }
        catch(const _Interruption_exception &)
        {
            //
            // If someone manually threw the _Interruption_exception exception, we will have a cancel count but not a canceled context. This
            // means we need to apply the cancel one level up. Normally, the act of throwing would do that via being caught in the
            // wait, but this is special "marshaling" for _Interruption_exception.
            //
            if (pContext->HasInlineCancellation() && !pContext->IsEntireContextCanceled())
                pTaskCollection->_Cancel();
        }
        catch(...)
        {
            //
            // Track the exception that was thrown here. _RaisedException makes the decision on what
            // exceptions to keep and what to discard. The flags it sets will indicate to the thread calling ::Wait that it must rethrow.
            //
            pTaskCollection->_RaisedException();
            pTaskCollection->_Cancel();
        }

        pOriginContext->RemoveStealer(pContext);
        ASSERT(pContext->GetGoverningTokenState() == NULL);

        //
        // This allows cancellation of stolen chores based on a cancellation token between the declaration of a stg and its inlining.
        //
        if (pRegistration != NULL)
        {
            ASSERT(pTokenState != NULL);
            pTokenState->_DeregisterCallback(pRegistration);
            pRegistration->_Release();
        }

        pContext->ClearCancel();
        pContext->SetRootCollection(NULL);
        pChore->_M_pTaskCollection = NULL;
        pTaskCollection->_CountUp();
    }

    /// <summary>
    ///     Prepares for execution as a stolen chore.
    /// </summary>
    void _UnrealizedChore::_PrepareStealUnstructured(ContextBase *pBaseContext)
    {
        InternalContextBase *pContext = static_cast<InternalContextBase *> (pBaseContext);

        if (pContext->GetRootCollection() == NULL)
        {
            _TaskCollection* pTaskCollection = static_cast<_TaskCollection *> (_M_pTaskCollection);
            ContextBase *pOriginContext = reinterpret_cast <ContextBase *> (pTaskCollection->_M_pOwningContext);

            pContext->SetRootCollection(pTaskCollection);

            //
            // pOriginContext is only safe to touch if the act of stealing from a non-detached context put a hold on that context
            // to block deletion until we are chained for cancellation.
            //
            SafeRWList<ListEntry> *pList = reinterpret_cast<SafeRWList<ListEntry> *> (pTaskCollection->_M_stealTracker);
            ASSERT(sizeof(pTaskCollection->_M_stealTracker) >= sizeof(*pList));

            if (_M_fDetached)
            {
                //
                // We cannot touch the owning context -- it was detached as of the steal. The chain goes onto the task collection.
                //
                pContext->NotifyTaskCollectionChainedStealer();
                pList->AddTail(&(pContext->m_stealChain));
            }
            else
            {
                pList->AcquireWrite();
                pTaskCollection->_M_activeStealersForCancellation++;
                pList->ReleaseWrite();
                pOriginContext->AddStealer(pContext, true);
            }
        }
    }

    /// <summary>
    ///     Wrapper around execution of an unstructured chore that performs appropriate notification
    ///     and exception handling semantics.
    /// </summary>
    __declspec(noinline)
    void __cdecl _UnrealizedChore::_UnstructuredChoreWrapper(_UnrealizedChore * pChore)
    {
        InternalContextBase *pContext = static_cast<InternalContextBase *> (SchedulerBase::FastCurrentContext());
        // The context could be canceled if it was already prepared for steal (this happens during a block unblock race)
        ASSERT(pContext != NULL && (!pContext->HasInlineCancellation() || pContext->GetRootCollection() != NULL));

        _TaskCollection* pTaskCollection = static_cast<_TaskCollection *> (pChore->_M_pTaskCollection);

        //
        // pOriginContext is only safe to touch if the act of stealing from a non-detached context put a hold on that context
        // to block deletion until we are chained for cancellation.
        //
        ContextBase *pOriginContext = reinterpret_cast <ContextBase *> (pTaskCollection->_M_pOwningContext);
        SafeRWList<ListEntry> *pList = reinterpret_cast<SafeRWList<ListEntry> *> (pTaskCollection->_M_stealTracker);

        pChore->_PrepareStealUnstructured(pContext);

        _CancellationTokenState *pTokenState = pTaskCollection->_GetTokenState();
        _CancellationTokenRegistration *pRegistration = NULL;
        if (_CancellationTokenState::_IsValid(pTokenState))
        {
            pRegistration = pTokenState->_RegisterCallback(reinterpret_cast<TaskProc>(&_UnrealizedChore::_CancelViaToken), (ContextBase *)pContext);
        }

        //
        // Waiting on the indirect alias may throw (e.g.: the entire context may have been canceled). If it
        // throws, we need to deal with appropriate marshaling.
        //
        try
        {
            //
            // Set up an indirect alias for this task collection. Any usage of the original task collection
            // within this stolen chore will automatically redirect through the indirect alias. This allows
            // preservation of single-threaded semantics within the task collection while allowing it to be "accessed"
            // from stolen chores (multiple threads).
            //
            // This stack based collection will wait on stolen chores at destruction time. In the event the collection is not
            // used during the steal, this doesn't do much.
            //
            _TaskCollection indirectAlias(pTaskCollection, false);

            pContext->SetIndirectAlias(&indirectAlias);

            try
            {
                //
                // We need to consider this a possible interruption point. It's entirely possible that we stole and raced with a
                // cancellation thread. The collection was canceled after we stole(e.g.: removed from the WSQ) but before we added ourselves
                // to the stealing chain list above. In this case, the entire context will wait until completion (bad). Immediately
                // after we go on the list (a memory barrier), we need to check the collection cancellation flag. If the collection is going away,
                // we need to get out *NOW* otherwise the entire subtree executes.
                //
                if (pTaskCollection->_M_pOriginalCollection->_M_exitCode != 0 ||
                    (_CancellationTokenState::_IsValid(pTokenState) && pTokenState->_IsCanceled()) ||
                    (pTaskCollection->_M_executionStatus != TASKCOLLECTION_EXECUTION_STATUS_CLEAR &&
                    pTaskCollection->_M_executionStatus != TASKCOLLECTION_EXECUTION_STATUS_INLINE &&
                    pTaskCollection->_M_executionStatus != TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK))
                    throw _Interruption_exception();

                pChore->m_pFunction(pChore);
            }
            catch(const _Interruption_exception &)
            {
                //
                // If someone manually threw _Interruption_exception, we will have a cancel count but not a canceled context. This
                // means we need to apply the cancel one level up. Normally, the act of throwing would do that via being caught in the
                // wait, but this is special "marshaling" for _Interruption_exception.
                //
                if (pContext->HasInlineCancellation() && !pContext->IsEntireContextCanceled())
                    pTaskCollection->_Cancel();
            }
            catch(...)
            {
                //
                // Track the exception that was thrown here and subsequently cancel all work. _RaisedException makes the decision on what
                // exceptions to keep and what to discard. The flags it sets will indicate to the thread calling ::Wait that it must rethrow.
                //
                pTaskCollection->_RaisedException();
                pTaskCollection->_Cancel();
            }

            indirectAlias._Wait();
        }
        catch(const _Interruption_exception &)
        {
            //
            // If someone manually threw _Interruption_exception out of a task on the indirect alias, the same thing applies as to
            // a directly stolen chore (above).
            //
            if (pContext->HasInlineCancellation() && !pContext->IsEntireContextCanceled())
                pTaskCollection->_Cancel();
        }
        catch(...)
        {
            //
            // Track the exception that was thrown here and subsequently cancel all work. _RaisedException makes the decision on what
            // exceptions to keep and what to discard. The flags it sets will indicate to the thread calling ::Wait that it must rethrow.
            //
            pTaskCollection->_RaisedException();
            pTaskCollection->_Cancel();
        }

        pContext->SetIndirectAlias(NULL);
        ASSERT(pContext->GetGoverningTokenState() == NULL);

        if ( !pChore->_M_fDetached)
        {
            //
            // pOriginContext may die at any point (detachment). When it does, it will transfer the stolen chore trace from the context to the
            // given task collection (us) under lock. We can, therefore, take this lock and check if we are still okay to check the context.
            //
            pList->AcquireWrite();

            if (pContext->IsContextChainedStealer())
                pOriginContext->RemoveStealer(pContext);
            else
                pList->UnlockedRemove(&(pContext->m_stealChain));

            pTaskCollection->_M_activeStealersForCancellation--;

            pList->ReleaseWrite();

        }
        else
        {
            pList->Remove(&(pContext->m_stealChain));
        }

        if (pRegistration != NULL)
        {
            pTokenState->_DeregisterCallback(pRegistration);
            pRegistration->_Release();
        }

        pContext->ClearCancel();
        pContext->ClearAliasTable();
        pContext->SetRootCollection(NULL);
        pChore->_M_pTaskCollection = NULL;
        pTaskCollection->_NotifyCompletedChoreAndFree(pChore);
    }
} // namespace details

} // namespace Concurrency
