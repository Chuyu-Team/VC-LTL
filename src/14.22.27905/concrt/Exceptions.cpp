// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Exceptions.cpp
//
// Implementation for concurrency runtime exceptions.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
//
// scheduler_resource_allocation_error
//

/// <summary>
///     Construct a scheduler_resource_allocation_error exception with a message and an error code
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
/// <param name="hresult">
///     HRESULT of error that caused this exception
/// </param>
_Use_decl_annotations_
_CONCRTIMP scheduler_resource_allocation_error::scheduler_resource_allocation_error(const char* message, HRESULT hresult) noexcept
    : exception(message), _Hresult(hresult)
{ }

/// <summary>
///     Construct a scheduler_resource_allocation_error exception with an error code
/// </summary>
/// <param name="hresult">
///     HRESULT of error that caused this exception
/// </param>
_CONCRTIMP scheduler_resource_allocation_error::scheduler_resource_allocation_error(HRESULT hresult) noexcept
    : exception(), _Hresult(hresult)
{
}

/// <summary>
///     Get the error code that caused this exception
/// </summary>
///<returns>HRESULT of error that caused the exception</returns>
_CONCRTIMP HRESULT scheduler_resource_allocation_error::get_error_code() const noexcept
{
    return _Hresult;
}

//
// scheduler_worker_creation_error
//

/// <summary>
///     Constructs a <c>scheduler_worker_creation_error</c> object.
/// </summary>
/// <param name="_Message">
///     A descriptive message of the error.
/// </param>
/// <param name="_Hresult">
///     The <c>HRESULT</c> value of the error that caused the exception.
/// </param>
/**/
_Use_decl_annotations_
_CONCRTIMP scheduler_worker_creation_error::scheduler_worker_creation_error(const char * message, HRESULT hresult) noexcept
    : scheduler_resource_allocation_error(message, hresult)
{ }

/// <summary>
///     Constructs a <c>scheduler_worker_creation_error</c> object.
/// </summary>
/// <param name="_Hresult">
///     The <c>HRESULT</c> value of the error that caused the exception.
/// </param>
/**/
_CONCRTIMP scheduler_worker_creation_error::scheduler_worker_creation_error(HRESULT hresult) noexcept
    : scheduler_resource_allocation_error(hresult)
{ }

//
// unsupported_os -- exception thrown whenever an unsupported OS is used
//

/// <summary>
///     Construct a unsupported_os exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP unsupported_os::unsupported_os(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a unsupported_os exception
/// </summary>
_CONCRTIMP unsupported_os::unsupported_os() noexcept
    : exception()
{
}

//
// scheduler_not_attached
//

/// <summary>
///     Construct a scheduler_not_attached exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP scheduler_not_attached::scheduler_not_attached(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a scheduler_not_attached exception
/// </summary>
_CONCRTIMP scheduler_not_attached::scheduler_not_attached() noexcept
    : exception()
{
}

//
// improper_scheduler_attach
//

/// <summary>
///     Construct a improper_scheduler_attach exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP improper_scheduler_attach::improper_scheduler_attach(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a improper_scheduler_attach exception
/// </summary>
_CONCRTIMP improper_scheduler_attach::improper_scheduler_attach() noexcept
    : exception()
{
}

//
// improper_scheduler_detach
//

/// <summary>
///     Construct a improper_scheduler_detach exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP improper_scheduler_detach::improper_scheduler_detach(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a improper_scheduler_detach exception
/// </summary>
_CONCRTIMP improper_scheduler_detach::improper_scheduler_detach() noexcept
    : exception()
{
}

//
// improper_scheduler_reference
//

/// <summary>
///     Construct a improper_scheduler_reference exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP improper_scheduler_reference::improper_scheduler_reference(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a improper_scheduler_reference exception
/// </summary>
_CONCRTIMP improper_scheduler_reference::improper_scheduler_reference() noexcept
    : exception()
{
}

//
// default_scheduler_exists
//

/// <summary>
///     Construct a default_scheduler_exists exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP default_scheduler_exists::default_scheduler_exists(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a default_scheduler_exists exception
/// </summary>
_CONCRTIMP default_scheduler_exists::default_scheduler_exists() noexcept
    : exception()
{
}

//
// context_unblock_unbalanced
//

/// <summary>
///     Construct a context_unblock_unbalanced exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP context_unblock_unbalanced::context_unblock_unbalanced(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a context_unblock_unbalanced exception
/// </summary>
_CONCRTIMP context_unblock_unbalanced::context_unblock_unbalanced() noexcept
    : exception()
{
}

//
// context_self_unblock
//

/// <summary>
///     Construct a context_self_unblock exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP context_self_unblock::context_self_unblock(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a context_unblock_unbalanced exception
/// </summary>
_CONCRTIMP context_self_unblock::context_self_unblock() noexcept
    : exception()
{
}

//
// missing_wait -- Exception thrown whenever a task collection is destructed without being waited upon and still contains work
//

/// <summary>
///     Construct a missing_wait exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP missing_wait::missing_wait(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a missing_wait exception
/// </summary>
_CONCRTIMP missing_wait::missing_wait() noexcept
    : exception()
{
}

//
// bad_target -- Exception thrown whenever a messaging block is given a bad target pointer
//

/// <summary>
///     Construct a bad_target exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP bad_target::bad_target(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a bad_target exception
/// </summary>
_CONCRTIMP bad_target::bad_target() noexcept
    : exception()
{
}

//
// message_not_found -- Exception thrown whenever a messaging block is unable to find a requested message
//

/// <summary>
///     Construct a message_not_found exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP message_not_found::message_not_found(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a message_not_found exception
/// </summary>
_CONCRTIMP message_not_found::message_not_found() noexcept
    : exception()
{
}

//
// invalid_link_target -- Exception thrown whenever a messaging block tries to link a target twice
// when it should only occur once
//

/// <summary>
///     Construct a invalid_link_target exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP invalid_link_target::invalid_link_target(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a message_not_found exception
/// </summary>
_CONCRTIMP invalid_link_target::invalid_link_target() noexcept
    : exception()
{
}

//
// invalid_scheduler_policy_key -- Exception thrown whenever a policy key is invalid
//

/// <summary>
///     Construct a invalid_scheduler_policy_key exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP invalid_scheduler_policy_key::invalid_scheduler_policy_key(const char* message) noexcept
    : exception(message)
{
}

/// <summary>
///     Construct a invalid_scheduler_policy_key exception
/// </summary>
_CONCRTIMP invalid_scheduler_policy_key::invalid_scheduler_policy_key() noexcept
    : exception()
{
}

//
// invalid_scheduler_policy_value -- Exception thrown whenever a policy value is invalid
//

/// <summary>
///     Construct a invalid_scheduler_policy_value exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP invalid_scheduler_policy_value::invalid_scheduler_policy_value(const char* message) noexcept
    : exception(message)
{
}

/// <summary>
///     Construct a invalid_scheduler_policy_value exception
/// </summary>
_CONCRTIMP invalid_scheduler_policy_value::invalid_scheduler_policy_value() noexcept
    : exception()
{
}

//
// invalid_scheduler_policy_thread_specification -- Exception thrown whenever a combination of thread specifications are invalid
//

/// <summary>
///     Construct a invalid_scheduler_policy_thread_specification exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP invalid_scheduler_policy_thread_specification::invalid_scheduler_policy_thread_specification(const char* message) noexcept
    : exception(message)
{
}

/// <summary>
///     Construct a invalid_scheduler_policy_thread_specification exception
/// </summary>
_CONCRTIMP invalid_scheduler_policy_thread_specification::invalid_scheduler_policy_thread_specification() noexcept
    : exception()
{
}

//
// nested_scheduler_missing_detach -- Exception thrown when the runtime can detect that
// a Detach() was missing for a nested scheduler.
//

/// <summary>
///     Construct an nested_scheduler_missing_detach exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP nested_scheduler_missing_detach::nested_scheduler_missing_detach(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct an nested_scheduler_missing_detach exception
/// </summary>
_CONCRTIMP nested_scheduler_missing_detach::nested_scheduler_missing_detach() noexcept
    : exception()
{
}

//
// operation_timed_out -- An operation has timed out.
//

/// <summary>
///     Construct an operation_timed_out exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP operation_timed_out::operation_timed_out(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct an operation_timed_out exception
/// </summary>
_CONCRTIMP operation_timed_out::operation_timed_out() noexcept
    : exception()
{
}

//
// invalid_multiple_scheduling -- An exception thrown when a chore/task_handle is scheduled multiple
// times on one or more *TaskCollection/*task_group constructs before completing.
//

/// <summary>
///     Construct an invalid_multiple_scheduling exception with a message
/// </summary>
/// <param name="_Message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP invalid_multiple_scheduling::invalid_multiple_scheduling(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct an invalid_multiple_scheduling exception
/// </summary>
_CONCRTIMP invalid_multiple_scheduling::invalid_multiple_scheduling() noexcept
    : exception()
{
}

//
//
// invalid_oversubscribe_operation -- An exception thrown when Context::Oversubscribe(false)
// is called without calling Context::Oversubscribe(true) first.
//

/// <summary>
///     Construct an invalid_oversubscribe_operation exception with a message
/// </summary>
/// <param name="_Message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP invalid_oversubscribe_operation::invalid_oversubscribe_operation(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct an invalid_oversubscribe_operation exception
/// </summary>
_CONCRTIMP invalid_oversubscribe_operation::invalid_oversubscribe_operation() noexcept
    : exception()
{
}

//
// improper_lock
//

/// <summary>
///     Construct a improper_lock exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_Use_decl_annotations_
_CONCRTIMP improper_lock::improper_lock(const char* message) noexcept
    : exception(message)
{ }

/// <summary>
///     Construct a improper_lock exception
/// </summary>
_CONCRTIMP improper_lock::improper_lock() noexcept
    : exception()
{
}

} // namespace Concurrency
