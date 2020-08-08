// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// event.cpp
//
// This file includes two parts: event and _Condition_variable.
// The core implementations of events and _Condition_variable which understand the cooperative nature of
// the scheduler and are designed to be scalable.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


#include "concrtinternal.h"

#pragma warning (disable : 4702)

//
// NOTE: The design of the wait-for-multiple semantic tries to keep the following goals:
//
// - Single event waits (create/wait/set) are very efficient requiring few memory barriers and
//   absolutely no shared locking.  This is necessary to support utilizing the event for stolen chore
//   signaling.  Currently, there are N*2+1 memory barriers (where N is the number to grab/release
//   a spin-lock).
//
// - Multiple event wait can be supported on the same Event type.  There is (as yet) no bifurcation between
//   an event that can be used in a WaitForMultiple and one that can't.
//
// - There are (few) shared locks between multiple events.
//
// This leads to a few unfortunate side effects in the implementation described below:
//
// - Each event has a spinlock which now guards its wait chains.  No longer do we have a simple CAS loop
//   for light-weight events.  This will add extra memory barriers in the fast path (5 or 3 depending
//   on the lock versus 2 in the prototype implementation).
//
// - Each wait-for-multiple requires a single heap allocation.  With normal sized wait lists, this should
//   come from the concurrent suballocator.
//
// - Wait-for-multiple on N events requires N spinlock acquisitions although the code is left open to the
//   possibility of lock pooling on a granularity to be decided by the scheduler.

namespace Concurrency
{
namespace details
{

    //**************************************************************************
    // Shared Functionality:
    //
    // This functionality is shared between agents infrastructure in msvcp for timeouts
    // there as well as in the eventing infrastructure here in msvcr for timeouts as well.
    // This is an msvcr export to msvcp to keep a shared timer queue throughout ConcRT.
    //
    //**************************************************************************

    volatile LONG g_TimerQueueDemandInit = 0;
    volatile HANDLE g_hTimerQueue = NULL;

    /// <summary>
    ///     Returns the demand initialized single timer queue used for event timeouts, timer agents, etc...
    /// </summary>
    HANDLE GetSharedTimerQueue()
    {
        // TimerQueue is needed for XP support
#if !defined(_ONECORE)
        if (Concurrency::GetOSVersion() < Concurrency::IResourceManager::Vista && g_hTimerQueue == NULL)
        {
            if (InterlockedCompareExchange(&g_TimerQueueDemandInit, 1, 0) == 0)
            {
                g_hTimerQueue = CreateTimerQueue();
                if (g_hTimerQueue == NULL)
                {
                    InterlockedExchange(&g_TimerQueueDemandInit, 0);
                }
            }
            else
            {
                _SpinWaitBackoffNone spinWait;
                while(g_hTimerQueue == NULL && g_TimerQueueDemandInit == 1)
                {
                    spinWait._SpinOnce();
                }
            }

            if (!g_hTimerQueue)
            {
                throw std::bad_alloc();
            }
        }
#endif // !defined(_ONECORE)
        return g_hTimerQueue;
    }

    //**************************************************************************
    // Internal Prototypes and Definitions:
    //
    // These are purely internal to the event implementation are placed here in lieu
    // of generally visible headers.
    //**************************************************************************

    class EventWaitNode;

    /// <summary>
    ///     Represents a wait block.  It is indirectly chained to an event via a Wait*Node.
    /// </summary>
    class WaitBlock
    {
    public:

        enum STATE {UNDECIDED, SKIP, DONT_SKIP};

        /// <summary>
        ///     Wait block constructor
        /// </summary>
        WaitBlock() : m_pContext(NULL), m_smSkip_BlockUnblock(UNDECIDED)
        {
            m_pContext = Context::CurrentContext();
        }

        /// <summary>
        ///     Called when the wait is satisfied (the event is signaled).  Note that the derived class may or may
        ///     not unblock depending on the exact wait semantics.
        /// </summary>
        /// <returns>
        ///     An indication of whether the event needs to track this node after a signal due to the potential
        ///     for a reset to impact the overall wait.
        /// </returns>
        virtual bool Satisfy(Context **pContextOut, EventWaitNode *pNode) = 0;

        /// <summary>
        ///     Called when the event is reset.  A wait-all may need to adjust counters to prevent the wait from being
        ///     satisfied.
        /// </summary>
        /// <returns>
        ///     An indication of whether the wait node is still valid
        /// </returns>
        virtual bool Reset() = 0;

        /// <summary>
        ///     Called when the underlying event is being destroyed / rundown.  Allows cleaning up of wait blocks.
        /// </summary>
        virtual void Destroy() = 0;

        /// <summary>
        ///     Called in order to check whether a node is still alive or dead during a sweep of the wait chain.
        /// </summary>
        virtual bool SweepWaitNode() = 0;

        /// <summary>
        ///     Called in order to check whether a node is still alive or dead during a sweep of the reset chain.
        /// </summary>
        virtual bool SweepResetNode() = 0;

        // The context which this wait must block/unblock.
        Context *m_pContext;

        // Flag to decide on skipping a pair of block/unblock to avoid unblocking of a context blocked due
        // to scoped lock and unblocking it via event's set operation, which is the wrong/mismatched reason for unblocking.
        // Further comments in MultiWaitBlock::SingleSatisfy() method.
        volatile long m_smSkip_BlockUnblock;
    };

    /// <summary>
    ///     Represents a wait on a single object (with or without a timer).
    /// </summary>
    class SingleWaitBlock : public WaitBlock
    {
    public:

        virtual bool Satisfy(Context **pContextOut, EventWaitNode *pNode);
        virtual bool Reset();
        virtual void Destroy();
        virtual bool SweepWaitNode();
        virtual bool SweepResetNode();
    };

    class MultiWaitBlock : public WaitBlock
    {
    public:

        // An indication of which object caused the wait to be satisfied.
        EventWaitNode *m_pSatisfiedBy;

        // Timer queue timer.
        HANDLE m_hTimer;

        // The final trigger count.
        volatile long m_finalTrigger;

        // The number of things pointing at the wait block (wait nodes or timers).
        size_t m_waiters;

        // When the count reaches the trigger limit, the wait block is satisfied.
        volatile size_t m_triggerLimit;

        // The number of signaled objects
        volatile size_t m_count;

        // The number of completed waiters (master counter of when the block can be freed)
        volatile size_t m_completions;

        // An indication of whether this wait has a timeout or not.  Timeouts are handled by a two stage
        // wait (m_count -> m_finalTrigger).
        bool m_fHasTimeout;

        // A variable that is set if there was a timeout associated with the wait block and the wait timed out.
        // It is set either by the timer thread, or in the wait_for_* operation itself if the timeout was 0 and the wait was not satisfied
        // when the wait_for_* method was invoked.
        volatile bool m_fWaitTimedOut;

        /// <summary>
        /// MultiWaitBlock constructor.
        /// </summary>
        MultiWaitBlock(size_t waitObjects, bool timeout, bool timer)
            : m_pSatisfiedBy(NULL)
            , m_hTimer(NULL)
            , m_finalTrigger(0)
            , m_waiters(waitObjects + static_cast<int>(timer))
            , m_count(0)
            , m_completions(0)
            , m_fHasTimeout(timeout)
            , m_fWaitTimedOut(false)
        {
        }

        /// <summary>
        ///     Called when a node (or something masquerading as such) is done with its reference on the block.
        /// </summary>
        void NotifyCompletedNode();

        /// <summary>
        ///     Called when a timer on the wait block fires.
        /// </summary>
        static void CALLBACK DispatchEventTimer(PTP_CALLBACK_INSTANCE instance, void * pContext, PTP_TIMER timer);

        /// <summary>
        ///     Same as DispatchEventTimer, only used on WinXP platform.
        /// </summary>
        static void CALLBACK DispatchEventTimerXP(PVOID pContext, BOOLEAN timerOrWaitFired);
    protected:

        virtual void SingleSatisfy(Context **pContextOut, EventWaitNode *pNode);

    };

    class MultiWaitBlockHolder
    {
    public:

        MultiWaitBlockHolder(bool fWaitAll, size_t count, bool timeout, bool timer);

        ~MultiWaitBlockHolder();

        void Release()
        {
            m_count++;
        }

        MultiWaitBlock *GetWaitBlock() const
        {
            return m_pWaitBlock;
        }

        EventWaitNode *GetWaitNode(size_t i) const
        {
            return reinterpret_cast <EventWaitNode *> (m_pMemBlock + m_blockSize + m_nodeSize * i);
        }

        size_t GetIndexOfNode(EventWaitNode *pNode) const
        {
            return (size_t) (reinterpret_cast <PBYTE> (pNode) - (m_pMemBlock + m_blockSize)) / m_nodeSize;
        }

    private:

        size_t m_blockSize;
        size_t m_nodeSize;
        size_t m_totalBlockSize;
        BYTE *m_pMemBlock;
        MultiWaitBlock *m_pWaitBlock;

        size_t m_count;
        size_t m_refs;

    };

    class WaitAllBlock : public MultiWaitBlock
    {
    public:

        WaitAllBlock(size_t waitObjects, bool timeout, bool timer) : MultiWaitBlock(waitObjects, timeout, timer)
        {
            m_triggerLimit = waitObjects;
        }

        virtual bool Satisfy(Context **pContextOut, EventWaitNode *pNode);
        virtual bool Reset();
        virtual void Destroy();
        virtual bool SweepWaitNode();
        virtual bool SweepResetNode();
    };

    class WaitAnyBlock : public MultiWaitBlock
    {
    public:

        WaitAnyBlock(size_t waitObjects, bool timeout, bool timer) : MultiWaitBlock(waitObjects, timeout, timer)
        {
            m_triggerLimit = 1;
        }

        virtual bool Satisfy(Context **pContextOut, EventWaitNode *pNode);
        virtual bool Reset();
        virtual void Destroy();
        virtual bool SweepWaitNode();
        virtual bool SweepResetNode();
    };

    /// <summary>
    ///     An event wait node represents an abstract wait block which is chained to each event such that when the
    ///     event is signaled, the wait block is notified and performs the appropriate unblocking (or additional
    ///     waiting) required.
    /// </summary>
    class EventWaitNode
    {
    public:

        EventWaitNode* m_pNext;
        WaitBlock *m_pWaitBlock;

        EventWaitNode(WaitBlock *pWaitBlock) noexcept : m_pWaitBlock(pWaitBlock)
        {
        }

        bool Satisfy(Context **pContextOut)
        {
            return m_pWaitBlock->Satisfy(pContextOut, this);
        }

        bool Reset()
        {
            return m_pWaitBlock->Reset();
        }

        void Destroy()
        {
            m_pWaitBlock->Destroy();
        }

        bool SweepWaitNode()
        {
            return m_pWaitBlock->SweepWaitNode();
        }

        bool SweepResetNode()
        {
            return m_pWaitBlock->SweepResetNode();
        }

    };

    EventWaitNode * Sweep(EventWaitNode *pNode, bool fWaitChain);

} // namespace details

// Details for _Condition_variable
namespace details
{
    /// <summary>
    /// This is the wait-block design for handling two signal sources:
    ///   1. signals fired by user
    ///   2. signals fired by timer
    /// If there is no timer in this block, the block will be allocated on
    /// the stack, which will be finally destroyed when the context get released;
    /// however, when it comes with the timer, the block will be allocated on
    /// the heap, and 2 phases de-reference checks are required to *delete* this object.
    /// </summary>
    class TimedSingleWaitBlock : public SingleWaitBlock
    {
        // The event node nested in wait-block,
        // which shares the lifetime with wait-block
        EventWaitNode m_eventNode;

        HANDLE m_hTimer;

        const bool m_hasTimer;

        // The de-reference times
        volatile long m_deRef;

        // The number of signaled objects
        // It is used to determine who fires the signal first.
        volatile long m_signalCounter;

    public:
        TimedSingleWaitBlock & operator =(const TimedSingleWaitBlock &) = delete;

        // An indication of whether this wait has a timeout or not.
        volatile bool m_fWaitTimedOut;

        virtual bool SweepWaitNode();

        virtual void Destroy();

        /// <summary>
        /// Called when a timer on the wait-block fires.
        /// </summary>
        static void CALLBACK DispatchEventTimer(PTP_CALLBACK_INSTANCE instance, void * pContext, PTP_TIMER timer);

        /// <summary>
        /// Same as DispatchEventTimer, only used on WinXP platform.
        /// </summary>
        static void CALLBACK DispatchEventTimerXP(PVOID pContext, BOOLEAN timerOrWaitFired);

        virtual bool Satisfy(Context **pContextOut, EventWaitNode *pNode);

        /// <summary>
        /// TimedSingleWaitBlock constructor.
        /// </summary>
        TimedSingleWaitBlock(bool hasTimer)
            : m_eventNode(nullptr)
            , m_hasTimer(hasTimer)
            , m_deRef(0)
            , m_signalCounter(0)
            , m_fWaitTimedOut(false)
        {
            m_eventNode.m_pWaitBlock = this;
        }

        /// <summary>
        /// Get the event node nested in the wait-block
        /// </summary>
        EventWaitNode *getEventNode()
        {
            return &m_eventNode;
        }

        bool createTimer(unsigned int timerout)
        {
            if (m_hasTimer)
            {
                if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
                {
                    return (m_hTimer = RegisterAsyncTimerAndLoadLibrary(timerout, TimedSingleWaitBlock::DispatchEventTimer, this)) != nullptr;
                }
                else
                {
                    return platform::__CreateTimerQueueTimer(&m_hTimer,
                                    GetSharedTimerQueue(),
                                    TimedSingleWaitBlock::DispatchEventTimerXP,
                                    this,
                                    timerout,
                                    0,
                                    WT_EXECUTEINTIMERTHREAD) == TRUE;
                }
            }
            return false;
        }

        void destroyTimer(bool waitForOutstandingCallback)
        {
            if (m_hasTimer)
            {
                if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
                {
                    if (waitForOutstandingCallback && m_hTimer)
                        DeleteAsyncTimerAndUnloadLibrary(static_cast<PTP_TIMER>(m_hTimer));
                    // If it's an async deletion (happens inside the callback) we don't do anything here, new callback handler will handle it.
                }
                else
                {
                    platform::__DeleteTimerQueueTimer(GetSharedTimerQueue(), m_hTimer, waitForOutstandingCallback ? INVALID_HANDLE_VALUE : NULL);
                }
            }
        }
    };

    /// <summary>
    /// It will be called by two event sources :
    ///   1. the timer, with argument *pNode* nullptr
    ///   2. the user trigger, with *pNode* the address of the event node.
    /// This function will take actions ONLY when FIRST time being called,
    /// any further calls will be ignored with return value False.
    /// If the first caller sets pContextOut nullptr, it will Unblock the
    /// context immediatelly, otherwise, it will pass out the blocked context
    /// by pContextOut.
    /// </summary>
    bool TimedSingleWaitBlock::Satisfy(Context **pContextOut, EventWaitNode *pNode)
    {
        if (InterlockedIncrement(&m_signalCounter) == 1)
        {
            // Timer will be destroyed when as soon as
            // it is satisfied
            destroyTimer(pNode != nullptr);

            // check who initiated this *Satisfy*
            m_fWaitTimedOut = pNode == nullptr;

            if (pContextOut)
                *pContextOut = m_pContext;
            else
                m_pContext->Unblock();

            return true;
        }
        return false;
    }

    /// <summary>
    /// Delete the wait-block after 2 times de-reference.
    /// If the block is allocated on the stack, it should only
    /// be de-referenced once -- when he is removed from the
    /// event list.
    /// </summary>
    void TimedSingleWaitBlock::Destroy()
    {
        if (InterlockedIncrement(&m_deRef) == 2)
            delete this;
    }

    /// <summary>
    /// Clean the timed-out node.
    /// </summary>
    bool TimedSingleWaitBlock::SweepWaitNode()
    {
        if (m_fWaitTimedOut)
        {
            Destroy();
            return false;
        }
        return true;
    }

    /// <summary>
    ///     Called when a timer on an condition variable is signaled.
    /// </summary>
    void TimedSingleWaitBlock::DispatchEventTimer(PTP_CALLBACK_INSTANCE instance, void * pContext, PTP_TIMER timer)
    {
        TimedSingleWaitBlock *pWaitBlock = reinterpret_cast<TimedSingleWaitBlock *> (pContext);
        if (pWaitBlock->Satisfy(nullptr, nullptr))
        {
            // We need to release the timer and dereference the module at the very end of the callback.
            UnRegisterAsyncTimerAndUnloadLibrary(instance, timer);
        }
    }

    void TimedSingleWaitBlock::DispatchEventTimerXP(LPVOID pContext, BOOLEAN)
    {
        TimedSingleWaitBlock *pWaitBlock = reinterpret_cast<TimedSingleWaitBlock *> (pContext);
        pWaitBlock->Satisfy(nullptr, nullptr);
        // It does not need to de-reference anything.
    }

    _Condition_variable::_Condition_variable() :
        _M_pWaitChain(nullptr), _M_lock()
    {
    }

    _Condition_variable::~_Condition_variable()
    {
        //
        // It's entirely possible that some other thread is currently executing inside ::set, and is currently holding the lock.
        // Since the waiter that was woken up could destroy the event, either by deleting a heap allocated, or unwinding the
        // stack, we need to let that other thread (that invoked ::set) get out of the lock before we proceed.
        //
        _M_lock._Flush_current_owner();

        // release all contexts
        notify_all();
    }

    /// <summary>
    /// Fast method for waiting on _Condition_variable without timeout
    /// </summary>
    void _Condition_variable::wait(Concurrency::critical_section& _Lck)
    {
        // Please refers to the wait_for function
        TimedSingleWaitBlock block(false);

        EventWaitNode *pEventNode = block.getEventNode();
        {
            critical_section::scoped_lock lock(_M_lock);
            pEventNode->m_pNext = Sweep(reinterpret_cast <EventWaitNode *> (_M_pWaitChain), true);
            _M_pWaitChain = pEventNode;
            _Lck.unlock();
        }

        Context::Block();
        _Lck.lock();
    }

    /// <summary>
    /// Timed wait method
    /// </summary>
    bool _Condition_variable::wait_for(Concurrency::critical_section& _Lck, unsigned int _Timeout)
    {
        // If check special case for _Timeout for efficiency reason
        if (_Timeout == 0)
            return false;
        else if (_Timeout == COOPERATIVE_TIMEOUT_INFINITE)
        {
            wait(_Lck);
            return true;
        }

        // Create timed node on heap, which will be destroyed when
        // the de-reference counts 2.
        TimedSingleWaitBlock *pBlock = _concrt_new TimedSingleWaitBlock(true);
        EventWaitNode *pNode = pBlock->getEventNode();

        // Commit to wait:
        //   Step 1: Chain itself on the event list, set the timer,
        //   and release the mutex.
        //   The _Lck must be unlocked after chaining, to protect the atomic.
        {

            critical_section::scoped_lock lock(_M_lock);

            pNode->m_pNext = Sweep(reinterpret_cast <EventWaitNode *> (_M_pWaitChain), true);
            _M_pWaitChain = pNode;

            if (!pBlock->createTimer(_Timeout))
                throw std::bad_alloc();

            _Lck.unlock();
        }

        //  Step 2: blocks itself
        Context::Block();

        // After being waken up, collect the result and de-reference itself.
        bool res = !pBlock->m_fWaitTimedOut;
        pBlock->Destroy();

        // Re-acquire the mutex
        _Lck.lock();

        return res;
    }

    /// <summary>
    /// It only wake up one context if there are
    /// some contexts waiting, otherwise, it does nothing.
    /// </summary>
    void _Condition_variable::notify_one()
    {
        // optimization for reducing unnecessary lock
        if (!_M_pWaitChain)
            return;

        critical_section::scoped_lock lock(_M_lock);

        EventWaitNode *p = reinterpret_cast <EventWaitNode *> (_M_pWaitChain);
        EventWaitNode *np;
        Context *cp = nullptr;

        // It iterates over the chain, and finds first
        // wait-block not timed out yet.
        while (p && !p->Satisfy(&cp))
        {
            np = p->m_pNext;
            p->Destroy();
            p = np;
        }

        if (p)
        {
            // If it finds one available wait-block
            _M_pWaitChain = p->m_pNext;
            p->Destroy();

            // Unblock() must be called after Destroy()
            // since releasing context may invalidate the wait-block.
            cp->Unblock();
        }
        else
            _M_pWaitChain = NULL;
    }

    /// <summary>
    /// It wakes up all contexts if there are
    /// some contexts waiting, otherwise, it does nothing.
    /// </summary>
    void _Condition_variable::notify_all()
    {
        // optimization for reducing unnecessary lock
        if (!_M_pWaitChain)
            return;
        EventWaitNode *p, *np;
        {
            critical_section::scoped_lock lock(_M_lock);
            p = reinterpret_cast <EventWaitNode *> (_M_pWaitChain);
            _M_pWaitChain = nullptr;
        }

        // It is safe to go over the list without lock
        // since the chain has been removed from the head.
        while (p)
        {
            Context *cp = nullptr;
            p->Satisfy(&cp);
            np = p->m_pNext;
            p->Destroy();

            // Unblock() must be called after Destroy()
            // since releasing context may invalidate the wait-block.
            if (cp)
                cp->Unblock();
            p = np;
        }
    }

}

/// <summary>
///     Constructs an event.
/// </summary>
event::event() :
    _M_pWaitChain(EVENT_UNSIGNALED),
    _M_pResetChain(NULL)
{
}

/// <summary>
///     Destroys an event.
/// </summary>
event::~event()
{
    //
    // It's entirely possible that some other thread is currently executing inside ::set, and is currently holding the lock.
    // Since the waiter that was woken up could destroy the event, either by deleting a heap allocated, or unwinding the
    // stack, we need to let that other thread (that invoked ::set) get out of the lock before we proceed.
    //
    _M_lock._Flush_current_owner();

    //
    // Go through and make sure any event blocks are satisfied.  One would expect items only on the reset list,
    // but we'll handle both cases -- the runtime should not be leaking regardless.
    //
    EventWaitNode *pNext;

    EventWaitNode *pNode = reinterpret_cast <EventWaitNode *> (_M_pWaitChain);
    if (pNode > EVENT_SIGNALED)
    {
        for(; pNode != NULL; pNode = pNext)
        {
            pNext = pNode->m_pNext;
            if (pNode->Satisfy(NULL))
            {
                pNode->Destroy();
            }
        }
    }

    for (pNode = reinterpret_cast <EventWaitNode *> (_M_pResetChain); pNode != NULL; pNode = pNext)
    {
        pNext = pNode->m_pNext;
        pNode->Destroy();
    }
}

/// <summary>
///     Waits on the specified event.
/// </summary>
size_t event::wait(unsigned int timeout)
{
    const EventWaitNode *pOldChain;

    //
    // Waits with timeout fall back on the heavy weight "wait for multiple" mechanism.  The only place
    // we use a light-weight spin/stack semantic is with a single *WAIT*.
    //
    // We can specially handle a 0 timeout "check" here though.
    //
    if (timeout != COOPERATIVE_TIMEOUT_INFINITE)
    {
        if (timeout == 0)
        {
            if (reinterpret_cast <const EventWaitNode *> (_M_pWaitChain) == EVENT_SIGNALED)
                return 0;
            else
                return COOPERATIVE_WAIT_TIMEOUT;
        }

        event *pThis = this;
        return event::wait_for_multiple(&pThis, 1, true, timeout);
    }

    // Spin wait (no yielding) for the event to be set.
    _SpinWaitNoYield spinWait;

    do
    {
        pOldChain = reinterpret_cast <const EventWaitNode *> (_M_pWaitChain);
        if (pOldChain == EVENT_SIGNALED)
        {
            return 0;
        }

    } while (spinWait._SpinOnce());

    //
    // Give up and block, first putting our context on a stack-based
    // list of waiting contexts for this event.
    //
    SingleWaitBlock block;
    EventWaitNode node(&block);
    bool fSatisfied = false;
    {
        critical_section::scoped_lock lockGuard(_M_lock);

        if (_M_pWaitChain == EVENT_SIGNALED)
            fSatisfied = true;
        else
        {
            node.m_pNext = Sweep(reinterpret_cast <EventWaitNode *> (_M_pWaitChain), true);
            _M_pWaitChain = &node;
        }
    }


    if (!fSatisfied )
    {
        bool bSkip = block.m_smSkip_BlockUnblock == WaitBlock::SKIP // Avoid unnecessary InterlockedCompareExchange for optimizing.
                     || InterlockedCompareExchange(&block.m_smSkip_BlockUnblock, WaitBlock::DONT_SKIP, WaitBlock::UNDECIDED) == WaitBlock::SKIP;

        if(!bSkip)
            Context::Block();
    }

    return 0;
}

/// <summary>
///     Resets the specified event.
/// </summary>
void event::reset()
{
    critical_section::scoped_lock lockGuard(_M_lock);

    if (_M_pWaitChain == EVENT_SIGNALED)
    {
        EventWaitNode *pRoot = NULL;
        EventWaitNode *pNext = NULL;

        EventWaitNode *pNode = reinterpret_cast <EventWaitNode *> (_M_pResetChain);
        _M_pResetChain = NULL;

        for (; pNode != NULL; pNode = pNext)
        {
            pNext = pNode->m_pNext;
            if (pNode->Reset())
            {
                //
                // We need to shift this back to the wait list.  The wait hasn't been satisfied and
                // this reset impacts the block.
                //
                pNode->m_pNext = pRoot;
                pRoot = pNode;
            }

        }

        _M_pWaitChain = pRoot;
    }
}

/// <summary>
///     Sets the specified event.
/// </summary>
void event::set()
{
    Context **pContexts = NULL;
    ULONG nodeCount = 0;
    _MallocaArrayHolder<Context *> mholder;

    {
        critical_section::scoped_lock lockGuard(_M_lock);

        //
        // Although it's not technically necessary to interlock this, it allows an optimization for light-weight events
        // in that they are able to spin for a period before blocking.  Without the fence here, they would not.
        //
        EventWaitNode *pOldChain;
        pOldChain = reinterpret_cast <EventWaitNode *> (
            InterlockedExchangePointer (reinterpret_cast <volatile PVOID *> (&_M_pWaitChain), EVENT_SIGNALED)
            );

        if (pOldChain > EVENT_SIGNALED)
        {
            ASSERT(_M_pResetChain == NULL);

            EventWaitNode *pNext;

            //
            // Note that the lock grabbed above is within the event, so it's entirely possible that the moment we unblock
            // the context, the lock is gone.  We also don't want to diddle in the scheduler lists while under a hot
            // lock, so build the list of contexts to unblock, release the lock, and then diddle in the scheduler.
            //
            nodeCount = 0;
            for (EventWaitNode *pNode = pOldChain; pNode != NULL; pNode = pNode->m_pNext)
                nodeCount++;

            pContexts = mholder._InitOnRawMalloca(_malloca(sizeof (Context *) * nodeCount));
            nodeCount = 0;

            for (EventWaitNode *pNode = pOldChain; pNode != NULL; pNode = pNext)
            {
                //
                // Need to cache the next pointer, since as soon as we unblock,
                // the stack-based EventWaitNode may be deallocated.
                //
                pNext = pNode->m_pNext;

                Context *pContext;
                if (pNode->Satisfy(&pContext))
                {
                    //
                    // If Satisfy returned true, we need to track the node as it's part of
                    // a wait-for-all and a reset on this event could impact it.
                    //
                    pNode->m_pNext = reinterpret_cast <EventWaitNode *> (_M_pResetChain);

                    //
                    // Guarded via the spinlock.
                    //
                    _M_pResetChain = pNode;
                }

                if (pContext != NULL)
                    pContexts[nodeCount++] = pContext;
            }
        }
    }

    //
    // Unblock contexts outside the given dispatch lock.
    //
    while(nodeCount-- > 0)
    {
        pContexts[nodeCount]->Unblock();
    }
}


#pragma warning(disable:26010)

/// <summary>
///     Waits for multiple events to become signaled.
/// </summary>
/// <param name="pEvents">
///     An array of events to wait upon
/// </param>
/// <param name="count">
///     A count of events within the array
/// </param>
/// <param name="fWaitAll">
///     An indication of whether to wait for all events or just a single one
/// </param>
_Use_decl_annotations_
size_t event::wait_for_multiple(event** pEvents, size_t count, bool fWaitAll, unsigned int timeout)
{
    //
    // Handle some trivial cases up front
    //
    if (pEvents == NULL)
    {
        throw std::invalid_argument("pEvents");
    }

    //
    // Nothing to wait on.
    //
    if (count == 0)
        return 0;

    //
    // Optimize for any caller which decides to call this to wait on a single event.  All waits with timeouts
    // flow through here as we need the heavier weight mechanism.
    //
    if (count == 1 && (timeout == 0 || timeout == COOPERATIVE_TIMEOUT_INFINITE))
    {
        if (pEvents[0] == NULL)
        {
            throw std::invalid_argument("pEvents");
        }

        return pEvents[0]->wait(timeout);
    }

    for (size_t i = 0; i < count; i++)
    {
        if (pEvents[i] == NULL)
        {
            throw std::invalid_argument("pEvents");
        }
    }

    MultiWaitBlockHolder waitBlock(fWaitAll, count, timeout != COOPERATIVE_TIMEOUT_INFINITE, (timeout != 0 && timeout != COOPERATIVE_TIMEOUT_INFINITE));
    MultiWaitBlock *pWaitBlock = waitBlock.GetWaitBlock();

    //
    // Chain to each event, carefully checking signal state for each as we go.  Note that a wait
    // any can be satisfied immediately if any fail due to an event already being signaled.  In
    // that case, we must carefully dispose the rest of the nodes and make sure the counters are
    // appropriate for wait block disposal as the chained ones get dechained later on other event
    // set/reset/destruction.
    //
    bool fSatisfied = false;

    for (size_t i = 0; i < count; i++)
    {
        event *pEvent = pEvents[i];

        Context *pSatisfiedContext;

        critical_section::scoped_lock lockGuard(pEvent->_M_lock);

        EventWaitNode *pWaitNode = waitBlock.GetWaitNode(i);
        waitBlock.Release();
        EventWaitNode *pOldChain = reinterpret_cast <EventWaitNode *> (pEvent->_M_pWaitChain);

        if (pOldChain == EVENT_SIGNALED)
        {
            //
            // Event was signaled before we could add ourself to the wait list...  We must be
            // very careful here.  For a "wait any", we are satisfied but need to take care
            // to ensure that the heap blocks get appropriately freed and dechained.  For a wait
            // all, we need to chain to the reset list as it's possible that the event is reset
            // before some other event that would satisfy the wait is signaled.
            //
            if (fWaitAll)
            {
                if (pWaitNode->Satisfy(&pSatisfiedContext))
                {
                    pWaitNode->m_pNext = Sweep(reinterpret_cast <EventWaitNode *> (pEvent->_M_pResetChain), false);
                    pEvent->_M_pResetChain = pWaitNode;
                }

                if (pSatisfiedContext != NULL)
                {
                    ASSERT(i == count - 1);
                    fSatisfied = true;
                }
            }
            else
            {
                //
                // The wait is satisfied.
                //
                pWaitNode->Satisfy(&pSatisfiedContext);
                if(pSatisfiedContext != NULL)
                    fSatisfied = true;
                for (size_t j = i + 1; j < count; j++)
                {
                    pWaitNode = waitBlock.GetWaitNode(j);
                    waitBlock.Release();
                    pWaitNode->Satisfy(&pSatisfiedContext);
                    ASSERT(pSatisfiedContext == NULL);
                }

                break;
            }
        }
        else
        {
            pWaitNode->m_pNext = Sweep(pOldChain, true);
            pEvent->_M_pWaitChain = pWaitNode;
        }
    }

    if (!fSatisfied )
    {

        //
        // For explanation of skipping Block/Unblock please see the comments in MultiWaitBlock::SingleSatisfy() method.
        //
        bool bSkip = pWaitBlock->m_smSkip_BlockUnblock == WaitBlock::SKIP // Avoid unnecessary InterlockedCompareExchange for optimizing.
                     || InterlockedCompareExchange(&pWaitBlock->m_smSkip_BlockUnblock, WaitBlock::DONT_SKIP, WaitBlock::UNDECIDED) == WaitBlock::SKIP;

        if( !bSkip )
        {
            //
            // Handle timeouts of zero specially.  We don't want to block the thread.
            //
            if (timeout == 0)
            {
                if (InterlockedIncrement(&pWaitBlock->m_finalTrigger) == 1)
                {
                    pWaitBlock->m_pSatisfiedBy = NULL;
                    fSatisfied = true;
                    pWaitBlock->m_fWaitTimedOut = true;
                }
                else
                {
                    Context::Block();
                }
            }
            else
            {
                if (timeout != COOPERATIVE_TIMEOUT_INFINITE)
                {
                    if (pWaitBlock->m_finalTrigger == 0)
                    {
                        if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
                        {
                            if ((pWaitBlock->m_hTimer = RegisterAsyncTimerAndLoadLibrary(timeout, MultiWaitBlock::DispatchEventTimer, pWaitBlock)) == nullptr)
                            {
                                //
                                // Note that the thread is left in a state unexplicable by the scheduler here.  It's quite possible someone ::Unblocks this context in
                                // the future.  With this error, we make no attempt to unwind that.
                                //
                                throw std::bad_alloc();
                            }
                        }
                        else
                        {

                            if (!platform::__CreateTimerQueueTimer(&pWaitBlock->m_hTimer,
                                                       GetSharedTimerQueue(),
                                                       MultiWaitBlock::DispatchEventTimerXP,
                                                       pWaitBlock,
                                                       timeout,
                                                       0,
                                                       WT_EXECUTEINTIMERTHREAD))
                            {
                                //
                                // Note that the thread is left in a state unexplicable by the scheduler here.  It's quite possible someone ::Unblocks this context in
                                // the future.  With this error, we make no attempt to unwind that.
                                //
                                throw std::bad_alloc();
                            }
                        }
                        waitBlock.Release();
                    }
                }
                Context::Block();
            }
        }
    }
    return (pWaitBlock->m_pSatisfiedBy == NULL) ? COOPERATIVE_WAIT_TIMEOUT : waitBlock.GetIndexOfNode(pWaitBlock->m_pSatisfiedBy);
}

#pragma warning(default:26010)

namespace details
{
    /// <summary>
    ///     Constructs a holder for a single allocation wait block which gets split into a wait block and a series of wait nodes,
    ///     one per wait object.
    /// </summary>
    MultiWaitBlockHolder::MultiWaitBlockHolder(bool fWaitAll, size_t count, bool timeout, bool timer) : m_count(0)
    {
        //
        // Allocate a single block comprised of all the wait nodes / block that we need to satisfy
        // the wait for multiple.
        //
        m_blockSize = ALIGNED_SIZE(fWaitAll ? sizeof(WaitAllBlock) : sizeof(WaitAnyBlock), P2_ALIGN);
        m_nodeSize = ALIGNED_SIZE(sizeof(EventWaitNode), P2_ALIGN);
        m_totalBlockSize = m_blockSize + m_nodeSize * count;

        m_pMemBlock = _concrt_new BYTE[m_totalBlockSize];

        m_pWaitBlock = reinterpret_cast<MultiWaitBlock *> (m_pMemBlock);
        if (fWaitAll)
            new(m_pMemBlock) WaitAllBlock(count, timeout, timer);
        else
            new(m_pMemBlock) WaitAnyBlock(count, timeout, timer);

        BYTE *pWaitNodeAddr = m_pMemBlock + m_blockSize;

        for (size_t i = 0; i < count; i++)
        {
            new(pWaitNodeAddr) EventWaitNode(m_pWaitBlock);
            pWaitNodeAddr += m_nodeSize;
        }

        //
        // The number of references on the block is the number of wait objects plus the timer plus one for
        // the stack frame of the WaitForMultiple which initialized us.  The block gets freed when NotifyCompletedNode
        // is called m_refs number of times.  This object is responsible, in normal cases, for releasing the single
        // stack frame reference.  It's also responsible for cleaning up and releasing any references that won't come
        // from wait objects / timers due to exceptions thrown in the midst of setting up the wait.
        //
        m_refs = count + (timer ? 2 : 1);

    }

    /// <summary>
    ///     Destructor for the wait block holder.  Releases any references on the block which will not come as the result
    ///     of a release.
    /// </summary>
    MultiWaitBlockHolder::~MultiWaitBlockHolder()
    {
        while(m_count++ < m_refs)
            m_pWaitBlock->NotifyCompletedNode();
    }

    /// <summary>
    ///     Called in order to satisfy the wait.  This handles a single wait/timer combination.  Any multi-wait semantic
    ///     must override this and call the base class in order to present a single-wait semantic.
    /// </summary>
    void MultiWaitBlock::SingleSatisfy(Context **pContextOut, EventWaitNode *pNode)
    {
        //
        // If there is a timeout, the timer may already have unblocked the context.
        //
        Context *pContext = m_pContext;
        bool fSatisfied = true;
        if (m_fHasTimeout)
        {
            if (InterlockedIncrement(&m_finalTrigger) != 1)
                fSatisfied = false;
        }

        if (fSatisfied)
        {
            // SingleSatisfy can be called with pNode set to NULL, but only when the wait has timed out.
            ASSERT(pNode != NULL);
            m_pSatisfiedBy = pNode;

            if (m_hTimer)
            {
                if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
                {
                    DeleteAsyncTimerAndUnloadLibrary(static_cast<PTP_TIMER>(m_hTimer));
                }
                else
                {
                    platform::__DeleteTimerQueueTimer(GetSharedTimerQueue(), m_hTimer, INVALID_HANDLE_VALUE);
                }

                //
                // Now, we need to answer the question of whether the timer fired and incremented the
                // trigger or not.  That will answer the question of when we delete the wait block.
                //
                if (m_finalTrigger == 1)
                    NotifyCompletedNode();
            }

            //
            // The wait_for_multiple() or wait() may be in the process of chaining the context to wait
            // chain of the event. Before chaining it has taken a lock on the event. It is possible
            // that the current context being unblocked (in this SingleSatisfy()) could be the one
            // blocked because of the lock taken above. In this case m_fOkToUnblock flag was set to FALSE in
            // wait_for_multiple() or wait() and so here(in set()) we should not Unblock() the context and also set
            // a flag to not Block() the context in the wait_for_multiple(), for which m_fDoNotBlock flag
            // is set here. This cancels out the Block() Unblock().
            // If we do not take this measure and Unblock if above situation occurs, then Context blocked on
            // above lock will run, thus the critical region will be executed concurrently, which is
            // disastrous. Also this (Unblocking here) could result in Unblock/Unblock sequence on a
            // context which is illegal.
            //
            bool bSkip = !(pNode->m_pWaitBlock->m_smSkip_BlockUnblock == WaitBlock::DONT_SKIP // Avoid unnecessary InterlockedCompareExchange for optimizing.
                           || InterlockedCompareExchange(&pNode->m_pWaitBlock->m_smSkip_BlockUnblock, WaitBlock::SKIP, WaitBlock::UNDECIDED) == WaitBlock::DONT_SKIP);

            //
            // It is *NOT* safe to touch the this pointer if bSkip is true, or right after the context is unblocked (if bSkip is false) since the context associated
            // with this wait block could return from the wait, and destroy the wait block in the process.
            //
            if(bSkip)
            {
                if(pContextOut != NULL)
                    *pContextOut = NULL;        // No context in list, hence no Unblocking in set()
            }
            else if (pContextOut != NULL)
                *pContextOut = pContext;
            else
                pContext->Unblock();
        }
    }

    void MultiWaitBlock::DispatchEventTimer(PTP_CALLBACK_INSTANCE instance, void * pContext, PTP_TIMER timer)
    {
        MultiWaitBlock *pWaitBlock = reinterpret_cast<MultiWaitBlock *> (pContext);
        Context *pUnblockContext = NULL;
        bool deleteTimer = false;
        if (InterlockedIncrement(&pWaitBlock->m_finalTrigger) == 1)
        {
            pUnblockContext = pWaitBlock->m_pContext;
            // Defer the timer deletion until the very end of this callback.
            deleteTimer = true;

            //
            // Note that after this point, m_hTimer is invalid.  Only the entity that transitions m_finalTrigger
            // to 1 is allowed to play with deleting the timer.
            //

            // Mark the block as timed out. This will allow us to cleanup the wait nodes associated with this wait block
            // the next time the event's wait and reset chains are swept.
            pWaitBlock->m_fWaitTimedOut = true;
        }

        if (pUnblockContext != NULL)
        {
            pWaitBlock->m_pSatisfiedBy = NULL;
            pUnblockContext->Unblock();
        }

        pWaitBlock->NotifyCompletedNode();

        if (deleteTimer)
        {
            UnRegisterAsyncTimerAndUnloadLibrary(instance, timer);
        }
    }

    /// <summary>
    ///     Called when a timer on an event is signaled.
    /// </summary>
    void MultiWaitBlock::DispatchEventTimerXP(LPVOID pContext, BOOLEAN)
    {
        MultiWaitBlock *pWaitBlock = reinterpret_cast<MultiWaitBlock *> (pContext);
        Context *pUnblockContext = NULL;

        if (InterlockedIncrement(&pWaitBlock->m_finalTrigger) == 1)
        {
            pUnblockContext = pWaitBlock->m_pContext;
            platform::__DeleteTimerQueueTimer(GetSharedTimerQueue(), pWaitBlock->m_hTimer, NULL);

            //
            // Note that after this point, m_hTimer is invalid.  Only the entity that transitions m_finalTrigger
            // to 1 is allowed to play with deleting the timer.
            //

            // Mark the block as timed out. This will allow us to cleanup the wait nodes associated with this wait block
            // the next time the event's wait and reset chains are swept.
            pWaitBlock->m_fWaitTimedOut = true;
        }

        if (pUnblockContext != NULL)
        {
            pWaitBlock->m_pSatisfiedBy = NULL;
            pUnblockContext->Unblock();
        }

        pWaitBlock->NotifyCompletedNode();

    }

    /// <summary>
    ///     Called to indicate that the event wait has been satisfied.
    /// </summary>
    bool SingleWaitBlock::Satisfy(Context **pContextOut, EventWaitNode *pNode)
    {
        //
        // For explanation of skipping Block/Unblock please see the comments in MultiWaitBlock::SingleSatisfy() method.
        //
        bool bSkip = !(pNode->m_pWaitBlock->m_smSkip_BlockUnblock == WaitBlock::DONT_SKIP // Avoid unnecessary InterlockedCompareExchange for optimizing.
                       || InterlockedCompareExchange(&pNode->m_pWaitBlock->m_smSkip_BlockUnblock, WaitBlock::SKIP, WaitBlock::UNDECIDED) == WaitBlock::DONT_SKIP );

        if( bSkip )
        {
            if(pContextOut)
                *pContextOut = NULL;        // No context in list, hence no Unblocking in set()
        }
        else if (pContextOut != NULL)
            *pContextOut = m_pContext;
        else
            m_pContext->Unblock();

        return false;
    }

#pragma warning(push)
#pragma warning(disable: 4702)
    /// <summary>
    ///     Called to indicate that the event for a single wait has been reset.
    /// </summary>
    bool SingleWaitBlock::Reset()
    {
        ASSERT(false);
        return false;
    }

    /// <summary>
    ///     Called to indicate that the event node was on the rundown list at event destruction.
    /// </summary>
    void SingleWaitBlock::Destroy()
    {
        ASSERT(false);
    }

    /// <summary>
    ///     Called in order to check whether a node is still alive or dead during a sweep of the wait chain.
    /// </summary>
    bool SingleWaitBlock::SweepWaitNode()
    {
        return true;
    }

    /// <summary>
    ///     Called in order to check whether a node is still alive or dead during a sweep of the reset chain.
    /// </summary>
    bool SingleWaitBlock::SweepResetNode()
    {
        ASSERT(false);
        return false;
    }
#pragma warning(pop)

    void MultiWaitBlock::NotifyCompletedNode()
    {
        size_t waiters = m_waiters;

        //
        // Once satisfied, we are responsible for incrementing the completion counter.  When it hits
        // the number of waiters, we can destroy the shared wait block.
        //
        if (InterlockedIncrementSizeT(&m_completions) == waiters + 1)
            delete[] (reinterpret_cast <BYTE *> (this));
    }

    /// <summary>
    ///     Called to indicate that an event for the wait-any has triggered and we should satisfy this
    ///     wait block.
    /// </summary>
    bool WaitAnyBlock::Satisfy(Context **pContextOut, EventWaitNode *pNode)
    {
        if (pContextOut != NULL)
            *pContextOut = NULL;

        //
        // NOTE: m_pWaitBlock is unsafe as soon as we increment the counter if we are not the entity
        // to increment the counter to the wait limit.  Cache everything up front!
        //
        ASSERT(m_triggerLimit == 1);

        size_t triggerCount = InterlockedIncrementSizeT(&m_count);
        if (triggerCount == m_triggerLimit)
            SingleSatisfy(pContextOut, pNode);

        NotifyCompletedNode();

        //
        // On a wait-any, we no longer need the wait node.  The single wait block containing the node is
        // freed by the last satisfied waiter.
        //
        return false;
    }

    /// <summary>
    ///     Called to indicate that an event in the wait-any has reset.  This is irrelevant to us.
    /// </summary>
    bool WaitAnyBlock::Reset()
    {
        ASSERT(false);
        return false;
    }

    /// <summary>
    ///     Called to indicate that an event with the node present on the rundown list is being
    ///     destroyed.  This should never be called for a wait any.
    /// </summary>
    void WaitAnyBlock::Destroy()
    {
    }

    /// <summary>
    ///     Called in order to check whether a node is still alive or dead during a sweep of the wait chain.
    /// </summary>
    bool WaitAnyBlock::SweepWaitNode()
    {
        if (m_count >= m_triggerLimit || m_fWaitTimedOut)
        {
            Context *pContext;
            // If the wait has timed out, go ahead and satisfy the block. Since the timer has already fired and woken up the context,
            // we are not in danger of doing so.
            Satisfy(&pContext, NULL);
            ASSERT(pContext == NULL);
            return false;
        }

        return true;
    }

    /// <summary>
    ///     Called in order to check whether a node is still alive or dead during a sweep of the reset chain.
    /// </summary>
    bool WaitAnyBlock::SweepResetNode()
    {
        ASSERT(false);
        return false;
    }

    /// <summary>
    ///     Called to indicate that an event for the wait-all has triggered and we should satisfy this
    ///     wait node.  Note that this does *NOT* indicate that the wait should be satisfied yet.
    /// </summary>
    bool WaitAllBlock::Satisfy(Context **pContextOut, EventWaitNode *pNode)
    {
        if (pContextOut != NULL)
            *pContextOut = NULL;

        ASSERT(m_triggerLimit >= 1);

        size_t triggerCount = InterlockedIncrementSizeT(&m_count);
        if (triggerCount == m_triggerLimit)
        {
            SingleSatisfy(pContextOut, pNode);
            NotifyCompletedNode();
            return false;
        }

        return true;

    }

    /// <summary>
    ///     Called to indicate that an event which was previously signaled and counting towards a satisfied
    ///     wait all block has reset.
    /// </summary>
    bool WaitAllBlock::Reset()
    {
        size_t triggerLimit = m_triggerLimit;

        //
        // Ensure that we never decrement once the wait is satisfied.  We need to make sure that a reset subsequent
        // just gets rid of the wait block.
        //
        size_t previousTriggerCount = m_count;
        for(;;)
        {
            if (previousTriggerCount == triggerLimit)
                break;

            size_t xchgCount = InterlockedCompareExchangeSizeT(&m_count, previousTriggerCount - 1, previousTriggerCount);
            if (xchgCount == previousTriggerCount)
                break;

            previousTriggerCount = xchgCount;
        }

        if (previousTriggerCount == triggerLimit)
        {
            NotifyCompletedNode();
            return false;
        }

        return true;

    }

    /// <summary>
    ///     Called in order to check whether a node is still alive or dead during a sweep of the wait chain.
    /// </summary>
    bool WaitAllBlock::SweepWaitNode()
    {
        ASSERT(m_count < m_triggerLimit);
        if (m_fWaitTimedOut)
        {
            Context * pContext;
            if (Satisfy(&pContext, NULL))
                Destroy();
            ASSERT(pContext == NULL);
            return false;
        }
        return true;
    }

    /// <summary>
    ///     Called in order to check whether a node is still alive or dead during a sweep of the reset chain.
    /// </summary>
    bool WaitAllBlock::SweepResetNode()
    {
        ASSERT(m_count <= m_triggerLimit);
        if (m_count >= m_triggerLimit)
        {
            //
            // The reset will clear us out.
            //
            Reset();
            return false;
        }
        else if (m_fWaitTimedOut)
        {
            Destroy();
            return false;
        }
        return true;
    }

    /// <summary>
    ///     Called when an event with an all-node is destroyed with the event present on a rundown list, this
    ///     destroys the wait node and releases its shared reference on the wait block.
    /// </summary>
    void WaitAllBlock::Destroy()
    {
        NotifyCompletedNode();
    }

    /// <summary>
    ///     Called in order to sweep out unused entries from a given node list.  This clears dead wait-for-all nodes
    ///     on a reset-list or dead wait-for-any nodes on the wait-list, as well as nodes associated with timed out wait blocks on both lists.
    /// </summary>
    /// <param name="fWaitChain">
    ///     true if the wait chain of an event is being swept and false if the reset chain is being swept
    /// </param>
    EventWaitNode * Sweep(EventWaitNode *pNode, bool fWaitChain)
    {
        EventWaitNode *pRoot = NULL;
        EventWaitNode *pNext = NULL;
        for (; pNode != NULL; pNode = pNext)
        {
            // Cache the next pointer since the sweep could destroy the swept node.
            pNext = pNode->m_pNext;

            bool keepNode = fWaitChain ? pNode->SweepWaitNode() : pNode->SweepResetNode();
            if (keepNode)
            {
                pNode->m_pNext = pRoot;
                pRoot = pNode;
            }
        }

        return pRoot;
    }

//
// A StructuredEvent is simply a pointer with a few distinguished values.  A newly
// initialized StructuredEvent will be set to 0.  A StructuredEvent that has one or more waiters
// on it, that is, contexts which called StructuredEvent::Wait before StructuredEvent::Set has
// signaled the StructuredEvent, will simply point to a linked list of those waiters,
// via stack-blocks so no heap allocation is required.  A StructuredEvent that is
// signaled is set to 1.  Once an event is signaled, it can be safely
// deallocated, even if StructuredEvent::Set is still running.
//

//
// StructuredEvent - Synchronization object mediating access to the low-level context
// Block and Unblock APIs.
//
struct StructuredEventWaitNode
{
    StructuredEventWaitNode *m_next;
    ::Concurrency::Context *m_context;
};

//
// Wait until the event is signaled (via some other context calling Set())
//
void StructuredEvent::Wait()
{
    //
    // Spin a short time waiting to be signaled before we block
    //
    void *oldPtr = m_ptr;
    if (oldPtr == EVENT_SIGNALED)
        return;

    _SpinWaitBackoffNone spinWait;
    for (;;)
    {
        oldPtr = m_ptr;
        if (oldPtr == EVENT_SIGNALED)
            return;

        if ( !spinWait._SpinOnce())
            break;
    }

    //
    // Give up and block, first putting our context on a stack-based
    // list of waiting contexts for this event
    //
    ::Concurrency::Context *context = SchedulerBase::FastCurrentContext();
    StructuredEventWaitNode node;

    node.m_context = context;

    for (;;)
    {
        node.m_next = (StructuredEventWaitNode*)oldPtr;

        void *xchgPtr = InterlockedCompareExchangePointer(&m_ptr, &node, oldPtr);

        if (xchgPtr == oldPtr)
            break;

        oldPtr = xchgPtr;

        if (oldPtr == EVENT_SIGNALED)
        {
            //
            // Event was signaled before we could add ourself to the wait
            // list, so no need to block any longer
            //
            return;
        }
    }

    context->Block();
}

//
// Set the event as signaled, and unblock any other contexts waiting
// on the event.
//
void StructuredEvent::Set()
{
    void *oldPtr = m_ptr;

    //
    // Mark the event signaled, and get the waiters list, if any
    //

    for (;;)
    {
        void *xchgPtr = InterlockedCompareExchangePointer(&m_ptr, EVENT_SIGNALED, oldPtr);

        if (xchgPtr == oldPtr)
            break;

        oldPtr = xchgPtr;
    }

    //
    // If the event had any waiters, then unblock them
    //
    if (oldPtr > EVENT_SIGNALED)
    {
        for (StructuredEventWaitNode *node = (StructuredEventWaitNode *)oldPtr, *next; node != NULL; node = next)
        {
            //
            // Need to cache the next pointer, since as soon as we unblock,
            // the stack-based StructuredEventWaitNode may be deallocated.
            //
            // Technically, there should be a memory fence after retrieving
            // the next pointer, but practically it's unnecessary, as long
            // as there is a locked operation inside the call to Unblock
            // before the blocked context starts running.  I don't think
            // it's possible to write a scheduler unblock operation without
            // needing a locked op, so I'm avoiding the extra cost per
            // waiter here.
            //

            next = node->m_next;
            node->m_context->Unblock();
        }
    }
}

} // namespace details

} // namespace Concurrency
