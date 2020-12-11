// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Agent.cpp
//
// Source file containing code for the agent creation APIs.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <agents.h>

namespace Concurrency
{

// A Filter function for a filter_block to check if the Agent has completed
bool _IsDone(agent_status const &status)
{
    return status == agent_done || status == agent_canceled;
}

// A Filter function for a filter_block to check if the Agent has started (or completed)
bool _IsStarted(agent_status const &status)
{
    return _IsDone(status) || status == agent_started;
}

/// <summary>
///     Creates an agent within the default scheduler, and places it any schedule
///     group of the scheduler's choosing.
/// </summary>
agent::agent() :
    _M_fStartable(TRUE), _M_fCancelable(TRUE), _M_pScheduler(NULL), _M_pScheduleGroup(NULL)
{
    _Trace_agents(AGENTS_EVENT_CREATE,
        details::_Trace_agents_get_id(this),
        details::_Trace_agents_get_id(this));

    send<agent_status> (_M_status, agent_created);
}

/// <summary>
///     Create an agent within the specified scheduler, in a schedule group of the
///     scheduler's choosing.
/// </summary>
agent::agent(Scheduler& pScheduler) :
    _M_fStartable(TRUE), _M_fCancelable(TRUE), _M_pScheduler(&pScheduler), _M_pScheduleGroup(NULL)
{
    _Trace_agents(AGENTS_EVENT_CREATE,
        details::_Trace_agents_get_id(this),
        details::_Trace_agents_get_id(this));

    send<agent_status> (_M_status, agent_created);
}

/// <summary>
///     Create an agent within the specified schedule group.  The scheduler is implied
///     by the schedule group.
/// </summary>
agent::agent(ScheduleGroup& pGroup) :
    _M_fStartable(TRUE), _M_fCancelable(TRUE), _M_pScheduler(NULL), _M_pScheduleGroup(&pGroup)
{
    _Trace_agents(AGENTS_EVENT_CREATE,
        details::_Trace_agents_get_id(this),
        details::_Trace_agents_get_id(this));

    send<agent_status> (_M_status, agent_created);
}

/// <summary>
///     Cleans up any resources that may have been created by the Agent.
/// </summary>
agent::~agent()
{
    _Trace_agents(AGENTS_EVENT_DESTROY, details::_Trace_agents_get_id(this));
}

/// <returns>
///     Returns a message source that can pass messages about the current state of the agent
/// </returns>
ISource<agent_status> * agent::status_port()
{
    return &_M_status;
}

/// <returns>
///     Returns the current state of the agent.  Note that this returned state could change
///     immediately after being returned.
/// </returns>
agent_status agent::status()
{
    return receive<agent_status>(_M_status);
}

/// <summary>
///     Moves an Agent from the agent_created state to the agent_runnable state, and schedules it for execution.
/// </summary>
/// <returns>
///     true if the agent started correctly, false otherwise
/// </returns>
bool agent::start()
{
    if(_M_status.value() != agent_created)
    {
        return false;
    }

    //
    // Check if the agent is Startable.  If the agent had already called start() or
    // this variable was set to FALSE in cancel(), return false.
    //
    if(InterlockedCompareExchange(&_M_fStartable, FALSE, TRUE) == FALSE)
    {
        return false;
    }

    _Trace_agents(AGENTS_EVENT_SCHEDULE, details::_Trace_agents_get_id(this));
    send<agent_status> (_M_status, agent_runnable);

    TaskProc proc = &Concurrency::agent::_Agent_task_wrapper;
    if(_M_pScheduleGroup != NULL)
    {
        _M_pScheduleGroup->ScheduleTask(proc, this);
    }
    else if(_M_pScheduler != NULL)
    {
        _M_pScheduler->ScheduleTask(proc, this);
    }
    else
    {
        CurrentScheduler::ScheduleTask(proc, this);
    }

    return true;
}

/// <summary>
///     Moves an agent into the done state, indicating the completion of the agent
/// </summary>
/// <returns>
///     true if the agent is moved to the agent_done state, false otherwise
/// </returns>
bool agent::done()
{
    //
    // current status
    //
    agent_status currentStatus = this->status();

    //
    // Indicate that the agent can no longer be started.
    //
    if (InterlockedCompareExchange(&_M_fStartable, FALSE, TRUE) != TRUE)
    {
        //
        // agent is either canceled, started or completed run.
        //
        currentStatus = receive<agent_status>(_M_status, _IsStarted);
    }

    //
    // Agent is not cancelable anymore.
    //
    InterlockedExchange(&_M_fCancelable, FALSE);

    //
    // Transition to agent_done state if it not already in one of
    // the terminal states.
    //
    if ((currentStatus != agent_canceled) && (currentStatus != agent_done))
    {
        send<agent_status> (_M_status, agent_done);

        return true;
    }

    return false;
}

/// <summary>
///     Moves an agent from the agent_created or agent_runnable to the agent_canceled state.
/// </summary>
/// <returns>
///     true if the agent was canceled correctly, false otherwise
/// </returns>
bool agent::cancel()
{
    //
    // In case this agent has been canceled before it was even started
    // mark it as no longer Startable and send a agent_canceled message to the
    // status port
    //
    if(InterlockedCompareExchange(&_M_fStartable, FALSE, TRUE) == TRUE)
    {
        send<agent_status> (_M_status, agent_canceled);
    }

    //
    // Check to see if the agent is still Cancelable.  Agents are initialized
    // m_fCancelable == TRUE, and set to false either here in cancel(), so
    // cancel() will not be called twice, or in the LWT, once the execution
    // of the Agent task has begun.
    //
    if(InterlockedCompareExchange(&_M_fCancelable, FALSE, TRUE) == TRUE)
    {
        // Wait for the agent to reach a canceled state state
        receive<agent_status>(_M_status, _IsDone);

        // The above InterlockedCompareExchange marked this agent for cancelation
        // When the LWT that has been spun up tries to execute the task, it will
        // find it has been canceled and will propagate out the canceled state to
        // the state buffer.
        return true;
    }

    return false;
}


// Private helper class to order an input array of agents. This is used by
// wait_for_all and wait_for_one to create an array of appropriate order nodes.
// The template _OrderNode specifies an _Order_node_base that accepts agent_status.
// For example, _Reserving_node<agent_status>
template<class _OrderNode>
class _OrderBlock
{
public:

    // Constructs an orderBlock which has an array of ordernodes connected to the agents.
    // The ordernodes are given a filter method to filter out non-terminal agent states
    _OrderBlock(size_t _Count, agent ** _PAgents, ITarget<size_t> * _PTarget) : _M_count(_Count)
    {
        // Create an array of order nodes
        _M_ppNodes = _concrt_new _OrderNode*[_M_count];
        for (size_t i = 0; i < _M_count; i++)
        {
            _M_ppNodes[i] = _concrt_new _OrderNode(_PAgents[i]->status_port(), i, _PTarget, _IsDone);
        }
    }

    // Destroys the block
    ~_OrderBlock()
    {
        for (size_t i = 0; i < _M_count; i++)
        {
            delete _M_ppNodes[i];
        }

        delete [] _M_ppNodes;
    }

    // Retrieve the agent status for the agent at the given index
    agent_status _Status(size_t _Index)
    {
        _CONCRT_ASSERT(_M_ppNodes[_Index]->has_value());

        return _M_ppNodes[_Index]->value();
    }

private:

    // Number of order nodes
    size_t _M_count;

    // Array of order nodes
    _OrderNode ** _M_ppNodes;
};


/// <summary>
///     Wait for an agent to complete its task. A task is completed when it enters the agent_canceled,
///     or agent_done states.
/// </summary>
agent_status agent::wait(agent * pAgent, unsigned int timeout)
{
    if(pAgent == NULL)
    {
        throw std::invalid_argument("pAgent");
    }

    return receive<agent_status>(pAgent->status_port(), _IsDone, timeout);
}

/// <summary>
/// Wait for all agents in a given Agent array to complete their tasks. A task is completed
/// when it enters the agent_canceled or agent_done states.
/// </summary>
void agent::wait_for_all(size_t count, agent ** pAgents, agent_status * pStatus, unsigned int timeout)
{
    if ( pAgents == NULL )
    {
        throw std::invalid_argument("pAgents");
    }

    for (size_t i = 0; i < count; i++)
    {
        if ( pAgents[i] == NULL )
        {
            throw std::invalid_argument("pAgents");
        }
    }

    // Create the following network
    //
    //  agent - orderNode -
    //                     \
    //  agent - orderNode - --call ~~~ single_assignment
    //                     /
    //  agent - orderNode -

    single_assignment<size_t> _Sa;
    volatile size_t _CompletedAgents = 0;
    call<size_t> _Call([&](size_t const&)
    {
        // Safe to access without synchronization since call blocks
        // guarantee that the function is not called for multiple
        // messages at the same time.
        _CONCRT_ASSERT(_CompletedAgents < count);
        size_t value = _CompletedAgents;
        _CompletedAgents = ++value;
        if (_CompletedAgents == count)
        {
            // All the agents have completed. Indicate the same by sending a message
            // (initialize) to the single assignment.
            send<size_t>(_Sa, 1);
        }
    });

    _OrderBlock<_Greedy_node<agent_status>>  _OrderedAgents(count, pAgents, &_Call);

    receive(&_Sa, timeout);

    // single_assignment has a message => all agents completed
    // Retrieve their status messages.
    if(pStatus != NULL)
    {
        for (size_t i = 0; i < count; i++)
        {
            pStatus[i] = _OrderedAgents._Status(i);
        }
    }
}

/// <summary>
///     Wait for any one of the agents in a given AgentTask array to complete its task. A task is completed
///     when it enters the agent_canceled or agent_done states.
/// </summary>
void agent::wait_for_one(size_t count, agent ** pAgents, agent_status &status, size_t& index, unsigned int timeout)
{
    if ( pAgents == NULL )
    {
        throw std::invalid_argument("pAgents");
    }

    for (size_t i = 0; i < count; i++)
    {
        if ( pAgents[i] == NULL )
        {
            throw std::invalid_argument("pAgents");
        }
    }

    // Create the following network
    //
    //  agent - orderNode -
    //                     \
    //  agent - orderNode - --single_assignment
    //                     /
    //  agent - orderNode -

    single_assignment<size_t> _Sa;
    _OrderBlock<_Greedy_node<agent_status>>  _OrderedAgents(count, pAgents, &_Sa);

    index = receive(&_Sa, timeout);

    // We were able to receive the index. Get the message (agent_status)
    status = _OrderedAgents._Status(index);
}

// A static wrapper function that calls the Run() method.  Used for scheduling of the task
void agent::_Agent_task_wrapper(void* data)
{
    agent *pAgent = (agent *) data;

    if(InterlockedCompareExchange(&pAgent->_M_fCancelable, FALSE, TRUE) == TRUE)
    {
        send<agent_status> (pAgent->_M_status, agent_started);

        // Invoke the run() function of the agent.
        _Trace_agents(AGENTS_EVENT_START, details::_Trace_agents_get_id(pAgent));
        pAgent->run();
        _Trace_agents(AGENTS_EVENT_END, details::_Trace_agents_get_id(pAgent), 0);
    }
    else
    {
        // This else path can be entered only if an agent was canceled before it
        // ran.  Send a agent_canceled message to the status.
        send<agent_status> (pAgent->_M_status, agent_canceled);
    }
}

// Implementation of agent APIs that should not be publicly exposed

namespace details
{
    static volatile runtime_object_identity s_RuntimeObjectIdentity = 0;

    _CONCRTIMP _Runtime_object::_Runtime_object()
    {
        // Increment the id by 2.  This is done because certain blocks (like join) need to have
        // a special message id to indicate a NULL id.  In this case, we use -1.  Incrementing by 2
        // will avoid any wrap-around issues causing us to hit -1.
        runtime_object_identity id = InterlockedExchangeAdd((volatile long *) &s_RuntimeObjectIdentity, 2);
        _CONCRT_ASSERT(id != -1);
        _M_id = id;
    }

    _CONCRTIMP _Runtime_object::_Runtime_object(runtime_object_identity _Id) : _M_id(_Id)
    {
    }
} // namespace details
} // namespace Concurrency
