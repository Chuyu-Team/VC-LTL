// ==++==
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// TaskCollection.h
//
// Miscellaneous internal support structure definitions for a task collection
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//
// The amount that we resize the task stack by per alloc.
//
#define TASK_STACK_GROWTH_SIZE 8

//
// The number of chores that we cap the task stack at. If after this many tasks are pushed, another is -- it cannot
// be inlined and will cause performance penalty for out-of-order WSQ utilization.
//
#define TASK_STACK_SIZE_CAP 1024

// **************************************************************************
// The phases of task collection cancellation (particularly for unstructured task collections).
// **************************************************************************

//
// The exit status mask (indicating which portion actively indicates status)
//
#define EXIT_CANCELLATION_MASK 0x3FFFFFFF

//
// Indicates that an exception has happened and while cancellation may proceed in due course, the end result should
// be a thrown exception.
//
#define EXIT_STATUS_FLAG_EXCEPTION_RAISED 0x80000000

//
// Indicates that the cancellation exception was thrown because cancellation was detected. The phases of cancellation are defined below.
//
#define EXIT_STATUS_FLAG_CANCELLATION_RAISED 0x40000000

//
// Indicates that cancel has started on the task collection.
//
#define EXIT_STATUS_START_CANCEL 0x00000001

// **************************************************************************
// Execution status meanings: Execution status is used for cancellation of the orignial task collection and its direct aliases
// **************************************************************************

//
// The task collection is in clear state -- it's not inlined, it's not canceled, etc...
//
#define TASKCOLLECTION_EXECUTION_STATUS_CLEAR 0

//
// The task collection is inlined.
//
#define TASKCOLLECTION_EXECUTION_STATUS_INLINE 1

//
// The task collection's cancellation for this alias was deferred because it was not inline.
//
#define TASKCOLLECTION_EXECUTION_STATUS_CANCEL_DEFERRED 3

//
// The cancellation is complete on the arbitrary thread.
//
#define TASKCOLLECTION_EXECUTION_STATUS_CANCEL_COMPLETE 4

//
// The task collection is inlined and about to wait for stolen chores yet the task stack has overflowed. This requires
// extra care during cancellation.
//
#define TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK 5   // 4 | TASKCOLLECTION_EXECUTION_STATUS_INLINE

//
// The task collection is inlined and a cancellation is in progress some arbitrary thread.
//
#define TASKCOLLECTION_EXECUTION_STATUS_INLINE_CANCEL_IN_PROGRESS 9         // 8 | TASKCOLLECTION_EXECUTION_STATUS_INLINE

// **************************************************************************
// Task collection flags:
// **************************************************************************

//
// This is an indirect alias.
//
#define TASKCOLLECTIONFLAG_ALIAS_IS_INDIRECT 1

//
// The entity involved in aliasing which views this flag is responsible for cleaning up the alias.
//
#define TASKCOLLECTIONFLAG_ALIAS_FREE_ON_VIEW 2

// **************************************************************************
// Related flags:
// **************************************************************************

//
// The bit indicating that this pointer is a registration rather than a token
//
#define TASKCOLLECTIONFLAG_POINTER_IS_REGISTRATION 1

// **************************************************************************
// Class definitions:
// **************************************************************************

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     This class is an *INTERNAL* structure which will retain specific optimizations to keeping track
    ///     of tasks associated with an unstructured task collection.
    /// </summary>
    class TaskStack
    {
    public:

        /// <summary>
        ///     Constructs a new task stack
        /// </summary>
        TaskStack() : m_stackSize(0), m_stackPtr(0), m_pStack(NULL), m_fOverflow(false)
        {
        }

        /// <summary>
        ///     Destroys a task stack
        /// </summary>
        ~TaskStack();

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
        bool Push(int taskCookie);

        /// <summary>
        ///     Pops an element from the task stack.
        /// </summary>
        /// <returns>
        ///     The element
        /// </returns>
        int Pop();

        /// </summary>
        ///     Returns an indication of whether or not the stack is empty.
        /// </summary>
        bool IsEmpty() const;

        /// <summary>
        ///     Clears out everything on the stack. Does *NOT* reset the overflow flag.
        /// </summary>
        void Clear();

        /// <summary>
        ///     Resets the overflow flag.
        /// </summary>
        void ResetOverflow()
        {
            m_fOverflow = false;
        }

        /// <summary>
        ///     An indication if the stack overflowed (was pushed beyond the cap).
        /// </summary>
        bool Overflow() const
        {
            return m_fOverflow;
        }

    private:

        int m_stackSize;
        int m_stackPtr;
        int *m_pStack;
        bool m_fOverflow;
    };

#define EVENT_UNSIGNALED ((void*) 0)
#define EVENT_SIGNALED ((void*) 1)

    /// <summary>
    ///     A single fire (non-resettable) event supporting a single waiter.
    /// </summary>
    class StructuredEvent
    {

    public:

        /// <summary>
        ///     Constructs a new structured event.
        /// </summary>
        StructuredEvent()
            : m_ptr(EVENT_UNSIGNALED)
        {
        }

        /// <summary>
        ///     Waits until the event is signaled (via some other context calling Set())
        /// </summary>
        void Wait();

        /// <summary>
        ///     Set the event as signaled, and unblock any other contexts waiting on the event.
        /// </summary>
        void Set();

    private:
        void * volatile m_ptr;
    };

    /// <summary>
    ///     Context record for WSQ sweeps.
    /// </summary>
    struct SweeperContext
    {
        /// <summary>
        ///     Constructs a new sweeper context.
        /// </summary>
        SweeperContext(_TaskCollection *pTaskCollection) :
            m_pTaskCollection(pTaskCollection),
            m_sweptChores(0)
        {
        }

        _TaskCollection *m_pTaskCollection;
        unsigned int m_sweptChores;
    };
} // namespace details
} // namespace Concurrency
