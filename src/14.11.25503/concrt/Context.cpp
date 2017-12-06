// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Context.cpp
//
// Implementation of static context APIs
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
/// <summary>
///     Returns a per scheduler unique identifier for the current context.
/// </summary>
/// <returns>
///     A per scheduler unique identifier for the current context or -1 if no such context exists.
/// </returns>
unsigned int Context::Id()
{
    const ContextBase *pContext = SchedulerBase::SafeFastCurrentContext();
    return (pContext != NULL ? pContext->GetId() : UINT_MAX);
}

/// <summary>
///     Returns an identifier for the virtual processor the current context is executing on.
/// </summary>
/// <returns>
///     An identifier for the virtual processor the current context is executing on or -1 if there is no such context
///     or it is not executing on a virtual processor at present.
/// </returns>
unsigned int Context::VirtualProcessorId()
{
    const ContextBase *pContext = SchedulerBase::SafeFastCurrentContext();
    return (pContext != NULL ? pContext->GetVirtualProcessorId() : UINT_MAX);
}

/// <summary>
///     Returns an identifier for the schedule group the current context is working on.
/// </summary>
/// <returns>
///     An identifier for the schedule group the current context is working on or -1 if there is no such context
///     or it is not executing a schedule group at present.
/// </returns>
unsigned int Context::ScheduleGroupId()
{
    const ContextBase *pContext = SchedulerBase::SafeFastCurrentContext();
    return (pContext != NULL ? pContext->GetScheduleGroupId() : UINT_MAX);
}

/// <summary>
///     Causes the current context to block, yielding execution to another context.  If the current
///     thread does not have a ConcRT context associated with it, it is inducted into one.
/// </summary>
void Context::Block()
{
    return SchedulerBase::CurrentContext()->Block();
}

/// <summary>
///     Yields execution so that another context may execute.  The current context is placed on the
///     scheduler's list of runnable contexts.  If the current thread does not have a context, it is inducted
///     into a ConcRT context.  If no other function is available to yield to, the function simply returns.
/// </summary>
void Context::Yield()
{
    SchedulerBase::CurrentContext()->Yield();
}

/// <summary>
///     Yields execution so that another context may execute.  The current context is placed on the
///     scheduler's list of runnable contexts.  If the current thread does not have a context, it is inducted
///     into a ConcRT context.  If no other function is available to yield to, the function simply returns.
///
///     This is intended for spin loops.
/// </summary>
void Context::_SpinYield()
{
    SchedulerBase::CurrentContext()->SpinYield();
}

/// <summary>
///     Returns an indication of whether the task collection which is currently executing inline on the current context
///     is in the midst of an active cancellation (or will be shortly).
/// </summary>
bool Context::IsCurrentTaskCollectionCanceling()
{
    ContextBase *pCurrentContext = SchedulerBase::SafeFastCurrentContext();
    if (pCurrentContext != NULL)
    {
        //
        // If a structured collection has an unstructured collection as a parent,
        // then GetExecutingCollection will always return the parent.
        //
        _TaskCollectionBase *pCollection = pCurrentContext->GetExecutingCollection();
        if (pCollection != NULL)
        {
            if (pCollection->_IsStructured())
            {
                return static_cast<details::_StructuredTaskCollection*>(pCollection)->_IsCanceling();
            }
            else if (static_cast<details::_TaskCollection*>(pCollection)->_IsAlias())
            {
                return static_cast<details::_TaskCollection*>(pCollection)->_OriginalCollection()->_IsCanceling();
            }
            else
            {
                return static_cast<details::_TaskCollection*>(pCollection)->_IsCanceling();
            }
        }
    }
    return false;
}

/// <summary>
///     Returns the ConcRT context associated with the current thread.
/// </summary>
/// <returns>
///     A pointer to the ConcRT context associated with the current thread if it exists.  If one does not exist,
///     a new context is created.
/// <returns>
_Ret_writes_(1) Context* Context::CurrentContext()
{
    return SchedulerBase::CurrentContext();
}

/// <summary>
///     Depending on the argument, causes the scheduler to add an extra virtual processor for the
///     duration of a block of code or remove a previously added one.
///
///         Oversubscribe(true);
///         /* some slow kernel or I/O code, etc.*/
///         Oversubscribe(false);
///
///     An extra virtual processor is allocated on the current hardware thread between the two calls
///     to Oversubscribe. If additional idle virtual processors are available, the virtual processor is created
///     and made available, but if no available virtual processors exist, the virtual processor is kicked into
///     action with an internal context that searches for work.
///     Calls to Oversubscribe(TRUE) must be matched with calls to Oversubscribe(FALSE) -> calls can be nested,
///     but only a maximum of one additional virtual processor is created. The additional vproc, if any, will
///     be retired after the outermost call to Oversubscribe(FALSE), as soon as the currently executing root
///     chore on the vproc is completed.
/// </summary>
/// <param name="beginOversubscription">
///     [in] A boolean value specifying whether oversubscription is to be turned on or off.
/// </param>
void Context::Oversubscribe(bool beginOversubscription)
{
    SchedulerBase::CurrentContext()->Oversubscribe(beginOversubscription);
}

namespace details
{
    _Context _Context::_CurrentContext()
    {
        return _Context(SchedulerBase::CurrentContext());
    }

    void _Context::_Yield()
    {
        SchedulerBase::CurrentContext()->Yield();
    }

    void _Context::_Oversubscribe(bool _BeginOversubscription)
    {
        SchedulerBase::CurrentContext()->Oversubscribe(_BeginOversubscription);
    }

    bool _Context::_IsSynchronouslyBlocked() const
    {
        return _M_pContext->IsSynchronouslyBlocked();
    }
}

} // namespace Concurrency
