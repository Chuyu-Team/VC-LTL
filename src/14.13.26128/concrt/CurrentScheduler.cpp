// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// CurrentScheduler.cpp
//
// Implementation of static scheduler APIs for CurrentScheduler::
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
/// <returns>
///     Returns a unique identifier for the current scheduler.  Returns -1 if no current scheduler exists.
/// </returns>
unsigned int CurrentScheduler::Id()
{
    const SchedulerBase *pScheduler = SchedulerBase::SafeFastCurrentScheduler();
    return pScheduler != NULL ? pScheduler->Id() : UINT_MAX;
}

/// <returns>
///     Returns a current number of virtual processors for the current scheduler. Returns -1 if no current scheduler exists.
///     No error state.
/// </returns>
unsigned int CurrentScheduler::GetNumberOfVirtualProcessors()
{
    const SchedulerBase *pScheduler = SchedulerBase::SafeFastCurrentScheduler();
    return pScheduler != NULL ? pScheduler->GetNumberOfVirtualProcessors() : UINT_MAX;
}

/// <returns>
///     Returns a copy of the policy the current scheduler is using.  Returns NULL if no current
///     scheduler exists.
/// </returns>
SchedulerPolicy CurrentScheduler::GetPolicy()
{
    const SchedulerBase *pScheduler = SchedulerBase::CurrentScheduler();
    return pScheduler->GetPolicy();
}

/// <summary>
///     Returns a reference to the scheduler instance in TLS storage (viz., the current scheduler).
///     If one does not exist, the default scheduler for the process is attached to the calling thread and returned --
///     if the default scheduler does not exist it is created
/// </summary>
/// <returns>
///     The TLS storage for the current scheduler is returned.
/// </returns>
Scheduler* CurrentScheduler::Get()
{
    return SchedulerBase::CurrentScheduler();
}

/// <summary>
///     The normal scheduler factory. (Implicitly calls Scheduler::Attach on the internally represented scheduler instance.)
///     The created scheduler will become the current scheduler for the current context (if it is an OS context it will be
///     inducted to a ConcRT context).  To shutdown such a scheduler, Detach needs to be called. Any extra Reference calls
///     must be matched with Release for shutdown to commence.
/// </summary>
/// <param name="pPolicy">
///     [in] A const pointer to the scheduler policy (See Scheduler Policy API)
/// </param>
void CurrentScheduler::Create(const SchedulerPolicy& policy)
{
    SchedulerBase *pScheduler = SchedulerBase::Create(policy);
    pScheduler->Attach();
}

/// <summary>
///     Detaches the current scheduler from the calling thread and restores the previously attached scheduler as the current
///     scheduler. Implicitly calls Release. After this function is called, the calling thread is then managed by the scheduler
///     that was previously activated via Create() or Attach().
/// </summary>
void CurrentScheduler::Detach()
{
    SchedulerBase* pScheduler = SchedulerBase::SafeFastCurrentScheduler();

    if (pScheduler != NULL)
    {
        return pScheduler->Detach();
    }
    else
    {
        throw scheduler_not_attached();
    }
}

/// <summary>
///     Causes the OS event object 'shutdownEvent' to be set when the scheduler shuts down and destroys itself.
/// </summary>
/// <param name="shutdownEvent">
///     [in] A valid event object
/// </param>
void CurrentScheduler::RegisterShutdownEvent(HANDLE shutdownEvent)
{
    SchedulerBase* pScheduler = SchedulerBase::SafeFastCurrentScheduler();

    if (pScheduler != NULL)
    {
        return pScheduler->RegisterShutdownEvent(shutdownEvent);
    }
    else
    {
        throw scheduler_not_attached();
    }
}

/// <summary>
///     Create a schedule group within the current scheduler.
/// </summary>
ScheduleGroup* CurrentScheduler::CreateScheduleGroup()
{
    return SchedulerBase::CurrentScheduler()->CreateScheduleGroup();
}

/// <summary>
///     Creates a new schedule group within the scheduler associated with the calling context.  Tasks scheduled within the newly created
///     schedule group will be biased towards executing at the specified location.
/// </summary>
/// <param name="_Placement">
///     A reference to a location where the tasks within the schedule group will biased towards executing at.
/// </param>
/// <returns>
///     A pointer to the newly created schedule group.  This <c>ScheduleGroup</c> object has an initial reference count placed on it.
/// </returns>
/// <remarks>
///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
///     scheduler currently associated with the calling context.
///     <para>You must invoke the <see cref="ScheduleGroup::Release Method">Release</see> method on a schedule group when you are
///     done scheduling work to it. The scheduler will destroy the schedule group when all work queued to it has completed.</para>
///     <para>Note that if you explicitly created this scheduler, you must release all references to schedule groups within it, before
///     you release your reference on the scheduler, via detaching the current context from it.</para>
/// </remarks>
/// <seealso cref="ScheduleGroup Class"/>
/// <seealso cref="ScheduleGroup::Release Method"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/// <seealso cref="location Class"/>
ScheduleGroup* CurrentScheduler::CreateScheduleGroup(location& placement)
{
    return SchedulerBase::CurrentScheduler()->CreateScheduleGroup(placement);
}

/// <summary>
///     Create a light-weight schedule within the current scheduler in an implementation dependent schedule group.
/// </summary>
/// <param name="proc">
///     [in] A pointer to the main function of a task.
/// </param>
/// <param name="data">
///     [in] A void pointer to the data that will be passed in to the task.
/// <param>
_Use_decl_annotations_
void CurrentScheduler::ScheduleTask(TaskProc proc, void *data)
{
    SchedulerBase::CurrentScheduler()->ScheduleTask(proc, data);
}

/// <summary>
///     Schedules a light-weight task within the scheduler associated with the calling context.  The light-weight task will be placed
///     within a schedule group of the runtime's choosing.  It will also be biased towards executing at the specified location.
/// </summary>
/// <param name="proc">
///     A pointer to the function to execute to perform the body of the light-weight task.
/// </param>
/// <param name="data">
///     A void pointer to the data that will be passed as a parameter to the body of the task.
/// </param>
/// <param name="placement">
///     A reference to a location where the light-weight task will be biased towards executing at.
/// </param>
/// <remarks>
///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
///     scheduler currently associated with the calling context.
/// </remarks>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/// <seealso cref="ScheduleGroup Class"/>
/// <seealso cref="location Class"/>
_Use_decl_annotations_
void CurrentScheduler::ScheduleTask(TaskProc proc, void * data, location& placement)
{
    SchedulerBase::CurrentScheduler()->ScheduleTask(proc, data, placement);
}

/// <summary>
///     Determines whether a given location is available on the current scheduler.
/// </summary>
/// <param name="_Placement">
///     A reference to the location to query the current scheduler about.
/// </param>
/// <returns>
///     An indication of whether or not the location specified by the <paramref name="_Placement"/> argument is available on the current
///     scheduler.
/// </returns>
/// <remarks>
///     This method will not result in scheduler attachment if the calling context is not already associated with a scheduler.
///     <para>Note that the return value is an instantaneous sampling of whether the given location is available.  In the presence of multiple
///     schedulers, dynamic resource management may add or take away resources from schedulers at any point.  Should this happen, the given
///     location may change availability.</para>
/// </remarks>
/**/
bool CurrentScheduler::IsAvailableLocation(const location& _Placement)
{
    const SchedulerBase *pScheduler = SchedulerBase::SafeFastCurrentScheduler();
    return pScheduler != NULL ? pScheduler->IsAvailableLocation(_Placement) : false;
}

namespace details
{
    void _CurrentScheduler::_ScheduleTask(TaskProc _Proc, void * _Data)
    {
        SchedulerBase::CurrentScheduler()->ScheduleTask(_Proc, _Data);
    }

    unsigned int _CurrentScheduler::_Id()
    {
        return CurrentScheduler::Id();
    }

    unsigned int _CurrentScheduler::_GetNumberOfVirtualProcessors()
    {
        return SchedulerBase::CurrentScheduler()->GetNumberOfVirtualProcessors();
    }

    _Scheduler _CurrentScheduler::_Get()
    {
        return _Scheduler(SchedulerBase::CurrentScheduler());
    }
} // namespace details

} // namespace Concurrency
