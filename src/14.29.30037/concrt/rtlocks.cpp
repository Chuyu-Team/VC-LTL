// ==++==
//
//   Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// rtlocks.cpp
//
// Implementation file for locks used only within the runtime implementation. The locks
// themselves are expected to be dependent on the underlying platform definition.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

#pragma warning (disable : 4702)

namespace Concurrency
{
namespace details
{
    const unsigned int SPIN_COUNT = 4000;

    unsigned int _SpinCount::_S_spinCount = SPIN_COUNT;

    static const long NotTriggered          = 0;
    static const long TriggeredByUnblock    = 1;
    static const long TriggeredByTimeout    = 2;

#if defined(_DEBUG)

    #define DebugBitsNone 0
    #define DebugBitsLockAcquiredOnPrimary 0x80000000
    #define DebugBitsLockAcquiredInHyperCritical 0x40000000
    #define DebugBitsLockAcquiredInCritical 0x20000000
    #define DebugBitsLockAcquiredOutsideCritical 0x10000000
    #define DebugBitsMask 0xF0000000

    /// <summary>
    ///     Returns a set of debug bits indicating where the lock was acquired.
    /// </summary>
    LONG GetDebugBits()
    {
        if (!SchedulerBase::IsOneShotInitialized())
            return DebugBitsNone;

        LONG dbgBits = DebugBitsNone;

#ifndef _UMS_DISABLED
        if (UMSSchedulingContext::OnPrimary() && UMS::GetCurrentUmsThread() != NULL)
            dbgBits |= DebugBitsLockAcquiredOnPrimary;

        //
        // UMS might not be initialized by the time we utilize a lock (think a global one).  Don't play games with the RM part
        // if it's not initialized.
        //
        UMSThreadProxy *pProxy = UMS::Initialized() ? UMSThreadProxy::GetCurrent() : NULL;
        ContextBase *pContext = SchedulerBase::FastCurrentContext();

        //
        // Once a proxy is transmogrified, it behaves as if it were an external thread with respect to all validations.
        //
        if (pProxy != NULL && !pProxy->IsShutdownValidations() && !pProxy->IsTransmogrified() && (pContext == NULL || !pContext->IsShutdownValidations()))
        {
            switch(pProxy->GetCriticalRegionType())
            {
                case OutsideCriticalRegion:
                    dbgBits |= DebugBitsLockAcquiredOutsideCritical;
                    break;
                case InsideCriticalRegion:
                    dbgBits |= DebugBitsLockAcquiredInCritical;
                    break;
                case InsideHyperCriticalRegion:
                    dbgBits |= DebugBitsLockAcquiredInHyperCritical;
                    break;
            }
        }
#endif // _UMS_DISABLED
        return dbgBits;
    }

    /// <summary>
    ///     Validates the lock conditions.
    /// </summary>
    void ValidateDebugBits(LONG dbgBits)
    {
#ifndef _UMS_DISABLED
        //
        // If a lock is taken on the primary, it may only be taken inside a hyper critical region.  It cannot be taken inside an ordinary critical
        // region or outside a critical region.
        //
        if ((dbgBits & (DebugBitsLockAcquiredOnPrimary | DebugBitsLockAcquiredInHyperCritical)) != 0)
        {
            //
            // **** READ THIS ****
            //
            // If this assert fires, you have placed a lock on a data structure and have not protected access to that lock appropriately on the UMS scheduler.
            // It's quite likely that you will randomly deadlock in stress.  Locks taken in the UMS primary can only be taken on other threads if they are taken
            // in a hyper critical region.
            //
            CONCRT_COREASSERT((dbgBits & (DebugBitsLockAcquiredInCritical | DebugBitsLockAcquiredOutsideCritical)) == 0);
        }
#else
        (dbgBits);
#endif // _UMS_DISABLED
    }
#endif // _DEBUG



    void _SpinCount::_Initialize()
    {
        _S_spinCount = (::Concurrency::GetProcessorCount() > 1) ? SPIN_COUNT : 0;
    }

    unsigned int _SpinCount::_Value()
    {
        return _S_spinCount;
    }

    //
    // The non-reentrant lock for use with the thread-based implementation is defined as
    // a 32-bit integer that is set to '1' when the lock is held, using interlocked
    // APIs.
    //
    _NonReentrantBlockingLock::_NonReentrantBlockingLock()
    {
        static_assert(sizeof(CRITICAL_SECTION) <= sizeof(_M_criticalSection), "_M_critical section buffer too small");
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        new(pCriticalSection) CRITICAL_SECTION;
        platform::__InitializeCriticalSectionEx(pCriticalSection, _SpinCount::_S_spinCount);
    }

    _NonReentrantBlockingLock::~_NonReentrantBlockingLock()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        DeleteCriticalSection(pCriticalSection);
    }

    //
    // Acquire the lock using an InterlockedExchange on _M_lock.  After s_spinCount
    // number of retries, it will begin calling sleep(0).
    //
    void _NonReentrantBlockingLock::_Acquire()
    {
         CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
         EnterCriticalSection(pCriticalSection);
    }

    void _NonReentrantLock::_DebugAcquire()
    {
#if defined(_DEBUG)
        LONG old;
        LONG dbgBits = GetDebugBits();
        _SpinWaitBackoffNone spinWait(_Sleep0);

        for (;;)
        {
            //
            // Under the debug build, verify lock sharing rules in the runtime by stealing high bits of the _M_lock field.
            // This is purely for UMS so we don't run into people changing lock structures and inadvertently causing HARD TO FIND
            // random deadlocks in UMS.
            //
            old = _M_Lock;
            if ((old & 1) == 0)
            {
                LONG destVal = old | 1 | dbgBits;
                LONG xchg = InterlockedCompareExchange(&_M_Lock, destVal, old);

                if (xchg == old)
                {
                    ValidateDebugBits(destVal);
                    break;
                }
            }

            spinWait._SpinOnce();
        }
#endif // _DEBUG
    }

    //
    // Try to acquire the lock, does not spin if it is unable to acquire.
    //
    bool _NonReentrantBlockingLock::_TryAcquire()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        return TryEnterCriticalSection(pCriticalSection) != 0;
    }

    bool _NonReentrantLock::_DebugTryAcquire()
    {
#if defined(_DEBUG)
        LONG dbgBits = GetDebugBits();
        LONG old = _M_Lock;

        if ((old & 1) == 0)
        {
            for(;;)
            {
                if ((old & 1) == 1)
                    break;

                LONG destVal = old | 1 | dbgBits;
                LONG xchg = InterlockedCompareExchange(&_M_Lock, destVal, old);
                if (xchg == old)
                {
                    ValidateDebugBits(destVal);
                    return true;
                }

                old = xchg;
            }
        }
#endif // _DEBUG
        return false;
    }

    //
    // Release the lock, which can be safely done without a memory barrier
    //
    void _NonReentrantBlockingLock::_Release()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        LeaveCriticalSection(pCriticalSection);
    }

#define NULL_THREAD_ID -1L

    _ReentrantBlockingLock::_ReentrantBlockingLock()
    {
        static_assert(sizeof(CRITICAL_SECTION) <= sizeof(_M_criticalSection), "_M_critical section buffer too small");
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        new(pCriticalSection) CRITICAL_SECTION;
        platform::__InitializeCriticalSectionEx(pCriticalSection, _SpinCount::_S_spinCount);
    }

    _ReentrantBlockingLock::~_ReentrantBlockingLock()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        DeleteCriticalSection(pCriticalSection);
    }

    _ReentrantLock::_ReentrantLock()
    {
        _M_owner = NULL_THREAD_ID;
        _M_recursionCount = 0;
    }

    void _ReentrantBlockingLock::_Acquire()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        EnterCriticalSection(pCriticalSection);
    }

    void _ReentrantLock::_Acquire()
    {
        LONG id = (LONG) GetCurrentThreadId();

        LONG old;
        _SpinWaitBackoffNone spinWait(_Sleep0);

#if defined(_DEBUG)
        LONG dbgBits = GetDebugBits();
#endif // _DEBUG

        for (;;)
        {

            old = InterlockedCompareExchange(&_M_owner, id, NULL_THREAD_ID);

            if ( old == NULL_THREAD_ID )
            {
#if defined(_DEBUG)
                //
                // Under the debug build, verify lock sharing rules in the runtime by stealing high bits of the _M_recursionCount field.
                // This is purely for UMS so we don't run into people changing lock structures and inadvertently causing HARD TO FIND
                // random deadlocks in UMS.
                //
                // This does mean you better not recursively acquire the lock more than a billion times ;)
                //
                _M_recursionCount = (_M_recursionCount & DebugBitsMask) | 1;
#else // _DEBUG
                _M_recursionCount = 1;
#endif // _DEBUG
                break;
            }
            else if ( old == id )
            {
#if defined(_DEBUG)
                CONCRT_COREASSERT((_M_recursionCount & ~DebugBitsMask) < (DebugBitsMask - 2));
                _M_recursionCount = ((_M_recursionCount & ~DebugBitsMask) + 1) | (_M_recursionCount & DebugBitsMask) | dbgBits;
#else
                _M_recursionCount++;
#endif // _DEBUG
                break;
            }

            spinWait._SpinOnce();
        }

#if defined(_DEBUG)
        ValidateDebugBits(_M_recursionCount);
#endif // _DEBUG

    }

    bool _ReentrantBlockingLock::_TryAcquire()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        return TryEnterCriticalSection(pCriticalSection) != 0;
    }

    bool _ReentrantLock::_TryAcquire()
    {
#if defined(_DEBUG)
        LONG dbgBits = GetDebugBits();
#endif // _DEBUG

        LONG id = (LONG) GetCurrentThreadId();

        LONG old = InterlockedCompareExchange(&_M_owner, id, NULL_THREAD_ID);

        if ( old == NULL_THREAD_ID  || old == id )
        {
#if defined(_DEBUG)
            CONCRT_COREASSERT((_M_recursionCount & ~DebugBitsMask) < (DebugBitsMask - 2));
            _M_recursionCount = ((_M_recursionCount & ~DebugBitsMask) + 1) | (_M_recursionCount & DebugBitsMask) | dbgBits;
#else // !_DEBUG
            _M_recursionCount++;
#endif
        }
        else
        {
            return false;
        }

#if defined(_DEBUG)
        ValidateDebugBits(_M_recursionCount);
#endif // _DEBUG

        return true;
    }

    void _ReentrantBlockingLock::_Release()
    {
        CRITICAL_SECTION * pCriticalSection = reinterpret_cast<CRITICAL_SECTION *>(_M_criticalSection);
        LeaveCriticalSection(pCriticalSection);
    }

    void _ReentrantLock::_Release()
    {
        if ( _M_owner != (LONG) GetCurrentThreadId() || _M_recursionCount < 1)
            return;

#if defined(_DEBUG)
        if ( (_M_recursionCount & ~DebugBitsMask) < 1 )
#else // !_DEBUG
        if ( _M_recursionCount < 1 )
#endif // _DEBUG
            return;

        _M_recursionCount--;

#if defined(_DEBUG)
        if ( (_M_recursionCount & DebugBitsMask) == 0 )
#else // !_DEBUG
        if ( _M_recursionCount == 0 )
#endif // DEBUG
        {
            _M_owner = NULL_THREAD_ID;
        }
    }

    //
    // NonReentrant PPL Critical Section Wrapper
    //
    _NonReentrantPPLLock::_NonReentrantPPLLock()
    {
    }

    void _NonReentrantPPLLock::_Acquire(void* _Lock_node)
    {
        _M_criticalSection._Acquire_lock(_Lock_node, true);
    }

    void _NonReentrantPPLLock::_Release()
    {
        _M_criticalSection.unlock();
    }

    //
    // Reentrant PPL Critical Section Wrapper
    //
    _ReentrantPPLLock::_ReentrantPPLLock()
    {
        _M_owner = NULL_THREAD_ID;
        _M_recursionCount = 0;
    }

    void _ReentrantPPLLock::_Acquire(void* _Lock_node)
    {
        LONG id = (LONG) GetCurrentThreadId();

        if ( _M_owner == id )
        {
            _M_recursionCount++;
        }
        else
        {
            _M_criticalSection._Acquire_lock(_Lock_node, true);
            _M_owner = id;
            _M_recursionCount = 1;
        }
    }

    void _ReentrantPPLLock::_Release()
    {
        ASSERT(_M_owner == (LONG) GetCurrentThreadId());
        ASSERT(_M_recursionCount >= 1);

        _M_recursionCount--;

        if ( _M_recursionCount == 0 )
        {
            _M_owner = NULL_THREAD_ID;
            _M_criticalSection.unlock();
        }
    }

    //
    // A Non-Reentrant Reader-Writer spin lock, designed for rare writers.
    //
    // A writer request immediately blocks future readers and then waits until all current
    // readers drain. A reader request does not block future writers and must wait until
    // all writers are  done, even those that cut in front In any race between requesting
    // and reader and a writer, the writer always wins.
    //
    _ReaderWriterLock::_ReaderWriterLock()
        : _M_state(_ReaderWriterLock::_Free), _M_numberOfWriters(0)
    {
    }

    //
    // Acquires the RWLock for reading.  Waits for the number of writers to drain.
    //
    void _ReaderWriterLock::_AcquireRead()
    {
#if defined(_DEBUG)
        LONG dbgBits = GetDebugBits();
        LONG val = _M_numberOfWriters;

        for(;;)
        {
            LONG xchgVal = InterlockedCompareExchange(&_M_numberOfWriters, val | dbgBits, val);
            if (xchgVal == val)
                break;

            val = xchgVal;
        }
#endif // _DEBUG

        for (;;)
        {
            if (_M_numberOfWriters > 0)
#if defined(_DEBUG)
                _WaitEquals(_M_numberOfWriters, 0, ~DebugBitsMask);
#else // !_DEBUG
                _WaitEquals(_M_numberOfWriters, 0);
#endif // _DEBUG
            int currentState = _M_state;
            // Try to acquire read lock by incrememting the current State.
            if (currentState != _Write &&
                InterlockedCompareExchange(&_M_state, currentState + 1, currentState) == currentState)
            {
#if defined(_DEBUG)
                ValidateDebugBits(_M_numberOfWriters);
#endif // _DEBUG
                return;
            }
        }
    }

    //
    // Release read lock -- the last reader will decrement _M_state to _Free
    //
    void _ReaderWriterLock::_ReleaseRead()
    {
        ASSERT(_M_state >= _Read);
        InterlockedDecrement(&_M_state);
    }

    //
    // Acquire write lock -- spin until there are no existing readers, no new readers will
    // be added
    //
    void _ReaderWriterLock::_AcquireWrite()
    {
        InterlockedIncrement(&_M_numberOfWriters);

        for (;;)
        {
            if (InterlockedCompareExchange(&_M_state, _Write, _Free) == _Free)
            {
#if defined(_DEBUG)
                ValidateDebugBits(_M_numberOfWriters);
#endif // _DEBUG
                return;
            }
            _WaitEquals(_M_state, _Free);
        }
    }

    //
    // Release writer lock -- there can only be one active, but a bunch might be pending
    //
    void _ReaderWriterLock::_ReleaseWrite()
    {
        ASSERT(_M_state == _Write);
#if defined(_DEBUG)
        ASSERT((_M_numberOfWriters & ~DebugBitsMask) > 0);
#else // !_DEBUG
        ASSERT(_M_numberOfWriters > 0);
#endif // _DEBUG

        // The following assignment does not need to be interlocked, as the interlocked
        // decrement can take care of the fence.
        _M_state = _Free;
        InterlockedDecrement(&_M_numberOfWriters);
    }

    //
    // Tries to acquire the write lock.  Returns true if the lock was acquired.
    //
    bool _ReaderWriterLock::_TryAcquireWrite()
    {
        if (InterlockedCompareExchange(&_M_state, _Write, _Free) == _Free)
        {
            InterlockedIncrement(&_M_numberOfWriters);
#if defined(_DEBUG)
            ValidateDebugBits(_M_numberOfWriters);
#endif // _DEBUG
            return true;
        }
        return false;
    }

    // Spin-Wait-Until variant -- spin for s_spinCount iterations, then Sleep(0) then repeat
    // 10 times (tunable), thereafter we spin and Sleep(1)
    void _ReaderWriterLock::_WaitEquals(volatile const LONG& location, LONG value, LONG mask)
    {
        unsigned int retries = 0;
        int spinInterval = 10;   // tuning

        for (;;)
        {
            if ((location & mask) == value)
                return;

            YieldProcessor();

            if (++retries >= _SpinCount::_S_spinCount)
            {
                if (spinInterval > 0)
                {
                    --spinInterval;
                    platform::__Sleep(0);
                }
                else
                    platform::__Sleep(1);
                retries = 0;
            }
        }
    }

    // Guarantees that all writers are out of the lock.  This does nothing if there are no pending writers.
    void _ReaderWriterLock::_FlushWriteOwners()
    {
        //
        // Ideally, if the read lock is held and we have pending writers, this would not need to grab the lock and release
        // it; however -- we must guarantee that any writer which was in the lock as of this call is completely out
        // of everything including _ReleaseWrite.  Since the last thing which happens there is the decrement of _M_numberOfWriters,
        // that is *currently* what we must key off.  It's possible that after the change of _M_state to free there, a reader
        // gets the lock because it was preempted after the initial check of _M_numberOfWriters which saw 0.  Hence, we cannot
        // rely on _M_state.
        //
        if (_M_numberOfWriters > 0)
        {
#if defined(_DEBUG)
                _WaitEquals(_M_numberOfWriters, 0, ~DebugBitsMask);
#else // !_DEBUG
                _WaitEquals(_M_numberOfWriters, 0);
#endif // _DEBUG
        }
    }

    //***************************************************************************
    //  Locking primitives and structures:
    //***************************************************************************

    // Reader-writer lock constants

    static const long RWLockWriterInterested     = 0x1; // Writer interested or active
    static const long RWLockWriterExclusive      = 0x2; // Writer active, no reader entry
    static const long RWLockReaderInterested     = 0x4; // Reader interested but not active
    static const long RWLockReaderCountIncrement = 0x8; // Reader count step (reader counter is scaled by it)

    /// <summary>
    ///     Node element used in the lock queues.
    /// </summary>
    class LockQueueNode
    {
    public:

        /// <summary>
        ///     Constructor for queue node. It keeps the context pointer in order
        ///     to block in a fashion visible to ConcRT.
        /// </summary>
        LockQueueNode(unsigned int timeout = COOPERATIVE_TIMEOUT_INFINITE)
            : m_pNextNode(NULL), m_ticketState(StateIsBlocked), m_hTimer(NULL), m_trigger(NotTriggered), m_fTimedNodeInvalid(0)
        {
            m_pContext = SchedulerBase::CurrentContext();
            if (timeout != COOPERATIVE_TIMEOUT_INFINITE)
            {
                if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
                {
                    if ((m_hTimer = RegisterAsyncTimerAndLoadLibrary(timeout, LockQueueNode::DispatchNodeTimeoutTimer, this)) == nullptr)
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
                      if (!platform::__CreateTimerQueueTimer(&m_hTimer,
                                            GetSharedTimerQueue(),
                                            LockQueueNode::DispatchNodeTimeoutTimerXP,
                                            this,
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
            }
        }

        /// <summary>
        ///     Constructor for queue node. It keeps the context pointer in order
        ///     to block in a fashion visible to ConcRT.
        /// </summary>
        LockQueueNode(Context * pContext, unsigned int ticket)
            : m_pContext(pContext), m_pNextNode(NULL), m_ticketState(ticket), m_hTimer(NULL), m_trigger(NotTriggered), m_fTimedNodeInvalid(0)
        {
        }

        /// <summary>
        ///     Waits until lock is available.
        /// </summary>
        /// <param name="currentTicketState">
        ///     The number of the node that is currently owning the lock, or has last owned it.
        /// </param>
        void Block(unsigned int currentTicketState = 0)
        {
            // Get the number of physical processors to determine the best spin times
            unsigned int numberOfProcessors = Concurrency::GetProcessorCount();
            _CONCRT_ASSERT(numberOfProcessors > 0);

            // If the previous node is blocked then there is no need to spin and waste cycles
            if (!IsPreviousBlocked())
            {
                // If there is a race and the ticket is not valid then use the default spin
                unsigned int placeInLine = IsTicketValid() ? ((m_ticketState >> NumberOfBooleanStates) - (currentTicketState >> NumberOfBooleanStates)) : 1;
                _CONCRT_ASSERT(placeInLine > 0);

                //
                // If the node is back in line by more than a processor count plus a threshold
                // then simply don't spin and block immediately. Otherwise, progressively increase the
                // amount of spin for the subsequent nodes until a double default spin count is reached.
                //
                if (placeInLine <= numberOfProcessors + TicketThreshold)
                {
                    const unsigned int defaultSpin = _SpinCount::_Value();
                    unsigned int totalSpin = defaultSpin + (defaultSpin * (placeInLine - 1)) / (numberOfProcessors + TicketThreshold);

                    _SpinWaitNoYield spinWait;
                    spinWait._SetSpinCount(totalSpin);

                    while (IsBlocked() && spinWait._SpinOnce())
                    {
                        // _YieldProcessor is called inside _SpinOnce
                    }
                }
            }

            //
            // After spin waiting for a while use the ConcRT blocking mechanism. It will return
            // immediately if the unblock already happened.
            //
            m_pContext->Block();
        }

        /// <summary>
        ///     Notifies that lock is available without context blocking.
        /// </summary>
        void UnblockWithoutContext()
        {
            m_ticketState = m_ticketState & ~StateIsBlocked;
        }

        /// <summary>
        ///     Compensate timer's unblock action, if necessary.
        /// </summary>
        void TryCompensateTimer()
        {
            // No matter the timer win the race or not, this thread always get unblocked (It actually never get blocked)
            // However, if the timer win the race, we do need to compensate its context::unblock.
            if (m_hTimer && InterlockedExchange(&m_trigger, TriggeredByUnblock) == TriggeredByTimeout)
            {
                // Timer won the race for the trigger and has unblocked this context - block to consume the unblock.
                m_pContext->Block();
                // The thread that failed to trigger the waiting context needs to release the reference
                DerefTimerNode();
            }
        }

        /// <summary>
        ///     Notifies that lock is available.
        /// </summary>
        bool Unblock()
        {
            if (InterlockedCompareExchange(&m_trigger, TriggeredByUnblock, NotTriggered) == NotTriggered)
            {
                // The unblock has won the race (if this was a timed node)
                UnblockWithoutContext();

                //
                // This call implies a fence which serves two purposes:
                //  a) it makes m_fIsBlocked visible sooner (in UnblockWithoutContext)
                //  b) it makes sure that we never block a context without unblocking it
                //
                m_pContext->Unblock();

                return true;
            }

            return false;
        }

        /// <summary>
        ///     Waits until the next node is set.
        /// </summary>
        /// <returns>
        ///     The next node.
        /// </returns>
        LockQueueNode * WaitForNextNode()
        {
            LockQueueNode * pNextNode = m_pNextNode;
            _SpinWaitBackoffNone spinWait;

            while (pNextNode == NULL)
            {
                //
                // There in no context blocking here so continue to spin even if maximum
                // spin is already reached. Since setting the tail and setting next pointer
                // are back-to-back operations it is very likely that while loop will not take
                // a long time.
                //
                spinWait._SpinOnce();
                pNextNode = m_pNextNode;
            }

            return pNextNode;
        }

        /// <summary>
        ///     Copies the contents of the passed in node to this node.
        /// </summary>
        /// <param name="pNextNode">
        ///     The node copy from.
        /// </param>
        /// <remarks>
        ///     Used only to transfer data to the internally allocated node.
        /// </remarks>
        void Copy(LockQueueNode * pCopyFromNode)
        {
            _CONCRT_ASSERT(pCopyFromNode->IsTicketValid());
            _CONCRT_ASSERT(!pCopyFromNode->IsBlocked());

            m_ticketState = pCopyFromNode->m_ticketState;
            m_pNextNode = pCopyFromNode->m_pNextNode;
            m_pContext = pCopyFromNode->m_pContext;
        }

        /// <summary>
        ///     Estimates the position of this node in the node queue based on the previous node.
        /// </summary>
        /// <param name="pPreviousNode">
        ///     The node to get the base number from, if available.
        /// </param>
        /// <remarks>
        ///     Used only as a heuristic for critical section and writers in reader writer lock.
        /// </remarks>
        void UpdateQueuePosition(LockQueueNode * pPreviousNode)
        {
            if (!IsTicketValid())
            {
                // If the previous node has a valid ticket then this one will have it as well
                if (pPreviousNode->IsTicketValid())
                {
                    unsigned int newState = (pPreviousNode->m_ticketState + TicketIncrement) & MaskBlockedStates;
                    _CONCRT_ASSERT((newState & StateIsTicketValid) != 0);

                    // If the previous node is blocked then set this information on the current node to save the spin
                    // We disabled the IsSynchronouslyBlocked check for timed-node. It is designed for work around an AV caused by accessing
                    // deleted context (After timer unblocking the thread, the thread may already exited and deleted its external context).
                    if (pPreviousNode->IsBlocked() && (pPreviousNode->IsPreviousBlocked() || pPreviousNode->m_hTimer == nullptr && pPreviousNode->m_pContext->IsSynchronouslyBlocked()))
                    {
                        newState |= StateIsPreviousBlocked;
                    }
                    m_ticketState = m_ticketState | newState;
                }
            }
        }

        /// <summary>
        ///     Estimates the state of this node based on the state of previous node.
        /// </summary>
        /// <param name="pPreviousNode">
        ///     The node to get the base from, if available.
        /// </param>
        /// <remarks>
        ///     Used only as a heuristic for readers in reader writer lock.
        /// </remarks>
        void UpdateBlockingState(LockQueueNode * pPreviousNode)
        {
            // If the previous node is blocked then set this information on the current node to save the spin
            // We don't need to check the m_hTimer here as UpdateQueuePosition because it is only used by read_write_lock, which does not have a timer.
            if (pPreviousNode->IsBlocked() && (pPreviousNode->IsPreviousBlocked() || pPreviousNode->m_pContext->IsSynchronouslyBlocked()))
            {
                m_ticketState = m_ticketState | StateIsPreviousBlocked;
            }
        }

        /// <summary>
        ///     Timed waiting node (m_hTimer != nullptr) is allocated on the heap, and needs to be
        ///     released after use.
        /// </summary>
        /// <remarks>
        ///     There are 2 references:
        ///        1. The thread that waits on or attempts to wait on the block.
        ///        2. The one who loses the race to trigger the waiting thread (this could be the timer callback,
        ///             the previous lock owner, or the waiting thread itself in the case where the lock was not already acquired).
        /// </remarks>
        void DerefTimerNode()
        {
            if (m_hTimer && InterlockedIncrement(&m_fTimedNodeInvalid) == 2)
            {
                delete this;
            }
        }

    /// <summary>
    ///     Called when a timer on an event is signaled.
    /// </summary>
    static void CALLBACK DispatchNodeTimeoutTimer(PTP_CALLBACK_INSTANCE instance, void * pContext, PTP_TIMER timer)
    {
        LockQueueNode *pNode = reinterpret_cast<LockQueueNode *> (pContext);

        if (InterlockedCompareExchange(&pNode->m_trigger, TriggeredByTimeout, NotTriggered) == NotTriggered)
        {
            pNode->m_pContext->Unblock();
        }
        else
        {
            // Unblock won the race, since we failed to trigger we're responsible for releasing the reference on the node.
            pNode->DerefTimerNode();
        }

        // Always delete the timer
        UnRegisterAsyncTimerAndUnloadLibrary(instance, timer);
    }

    /// <summary>
    ///     Same as DispatchNodeTimeoutTimer, but used only for XP and MSDK
    /// </summary>
    static void CALLBACK DispatchNodeTimeoutTimerXP(PVOID pContext, BOOLEAN)
    {
        LockQueueNode *pNode = reinterpret_cast<LockQueueNode *> (pContext);

        // Always delete the timer
        platform::__DeleteTimerQueueTimer(GetSharedTimerQueue(), pNode->m_hTimer, NULL);

        if (InterlockedCompareExchange(&pNode->m_trigger, TriggeredByTimeout, NotTriggered) == NotTriggered)
        {
            pNode->m_pContext->Unblock();
        }
        else
        {
            // Unblock won the race, since we failed to trigger we're responsible for releasing the reference on the node.
            pNode->DerefTimerNode();
        }
    }

    private:

        friend class critical_section;
        friend class reader_writer_lock;

        bool IsBlocked()
        {
            return (m_ticketState & StateIsBlocked) != 0;
        }

        bool IsPreviousBlocked()
        {
            return (m_ticketState & StateIsPreviousBlocked) != 0;
        }

        bool IsTicketValid()
        {
            return (m_ticketState & StateIsTicketValid) != 0;
        }

        // Const statics needed for blocking heuristics
        static const unsigned int TicketThreshold        = 2;
        static const unsigned int StateIsBlocked         = 0x00000001;
        static const unsigned int StateIsTicketValid     = 0x00000002;
        static const unsigned int StateIsPreviousBlocked = 0x00000004;
        static const unsigned int MaskBlockedStates      = ~(StateIsBlocked | StateIsPreviousBlocked);
        static const unsigned int NumberOfBooleanStates  = 0x00000003;
        static const unsigned int TicketIncrement        = 1 << NumberOfBooleanStates;

        Context *                 m_pContext;
        LockQueueNode * volatile  m_pNextNode;
        volatile unsigned int     m_ticketState;
        // Timer handle (valid only for XP; on Vista and above, this handle only used as the indication of whether current node is a timed node.
        HANDLE m_hTimer;
        // The trigger - for timed waits, the unblock mechanism competes with the timer to trigger the thread attempting to acquire the lock.
        // Note, the acquiring thread may fire the trigger itself if the lock is not held - this counts as a virtual 'unblock'
        volatile long m_trigger;
        volatile long m_fTimedNodeInvalid;
    };

    //
    // A C++ holder for a Non-reentrant PPL lock.
    //
    _CONCRTIMP _NonReentrantPPLLock::_Scoped_lock::_Scoped_lock(_NonReentrantPPLLock & _Lock) : _M_lock(_Lock)
    {
        new(reinterpret_cast <void *> (_M_lockNode)) LockQueueNode;
        _M_lock._Acquire(reinterpret_cast <void *> (_M_lockNode));
    }

    _CONCRTIMP _NonReentrantPPLLock::_Scoped_lock::~_Scoped_lock()
    {
        _M_lock._Release();
    }

    //
    // A C++ holder for a Reentrant PPL lock.
    //
    _CONCRTIMP _ReentrantPPLLock::_Scoped_lock::_Scoped_lock(_ReentrantPPLLock & _Lock) : _M_lock(_Lock)
    {
        new(reinterpret_cast <void *> (_M_lockNode)) LockQueueNode;
        _M_lock._Acquire(reinterpret_cast <void *> (_M_lockNode));
    }

    _CONCRTIMP _ReentrantPPLLock::_Scoped_lock::~_Scoped_lock()
    {
        _M_lock._Release();
    }

} // namespace details

/// <summary>
///     Constructs an critical section
/// </summary>
_CONCRTIMP critical_section::critical_section() : _M_pHead(NULL), _M_pTail(NULL)
{
    _CONCRT_ASSERT(sizeof(_M_activeNode) >= sizeof(LockQueueNode));

    // Hide the inside look of LockQueueNode behind a char array big enough to keep 3 pointers
    // This is why LockQueueNode is newed in place instead of a more traditional allocation.
    new(reinterpret_cast<void *>(_M_activeNode)) LockQueueNode(NULL, LockQueueNode::StateIsTicketValid);
}

/// <summary>
///     Destroys a critical section.  It is expected that the lock is no longer held.
/// </summary>
_CONCRTIMP critical_section::~critical_section()
{
    _ASSERT_EXPR(_M_pHead == NULL, L"Lock was destructed while held");
}

/// <summary>
///     Gets a critical section handle.
/// </summary>
/// <returns>
///     A reference to this critical section.
/// </returns>
_CONCRTIMP critical_section::native_handle_type critical_section::native_handle()
{
    return *this;
}

/// <summary>
///     Acquires this critical section.
/// </summary>
/// <remarks>
///     Throws a improper_lock exception if the lock is acquired recursively
/// </remarks>
_CONCRTIMP void critical_section::lock()
{
    LockQueueNode   newNode;                   // Allocated on the stack and goes out of scope before unlock()
    LockQueueNode * pNewNode = &newNode;

    //
    // Acquire the lock node that was just created on the stack
    //
    _Acquire_lock(pNewNode, false);

    //
    // At this point the context has exclusive ownership of the lock
    //

    _Switch_to_active(pNewNode);
}

/// <summary>
///     Tries to acquire the lock, does not block.
/// </summary>
/// <returns>
///     true if the lock is acquired, false otherwise
/// </returns>
_CONCRTIMP bool critical_section::try_lock()
{
    LockQueueNode   newNode;                   // Allocated on the stack and goes out of scope before unlock()
    LockQueueNode * pNewNode = &newNode;
    LockQueueNode * pPreviousNode = reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pTail, pNewNode, NULL));

    // Try and acquire this lock.  If this CAS succeeds, then the lock has been acquired.
    if (pPreviousNode == NULL)
    {
        _M_pHead = pNewNode;
        pNewNode->UpdateQueuePosition(reinterpret_cast<LockQueueNode *>(_M_activeNode));
        pNewNode->UnblockWithoutContext();
        _Switch_to_active(pNewNode);
        return true;
    }

    return false;
}

/// <summary>
///     Tries to acquire the lock for <c>timeout</c> number of milliseconds, does not block.
/// </summary>
/// <returns>
///     true if the lock is acquired, false otherwise
/// </returns>
_CONCRTIMP bool critical_section::try_lock_for(unsigned int timeout)
{
    LockQueueNode * pNewNode = new LockQueueNode(timeout);                   // Allocated on the heap

    //
    // Acquire the lock node that was just created
    //
    if (_Acquire_lock(pNewNode, false))
    {
        //
        // At this point the context has exclusive ownership of the lock
        //
        _Switch_to_active(pNewNode);
        pNewNode->DerefTimerNode();
        return true;
    }
    else
    {
        pNewNode->DerefTimerNode();
        return false;
    }
}

/// <summary>
///     Unlocks an acquired lock.
/// </summary>
_CONCRTIMP void critical_section::unlock()
{
    LockQueueNode * pCurrentNode = reinterpret_cast<LockQueueNode *>(_M_pHead);

    _ASSERT_EXPR(pCurrentNode != nullptr, L"Lock not being held");
    _ASSERT_EXPR(pCurrentNode->m_pContext == SchedulerBase::SafeFastCurrentContext(), L"Lock being held by different context");

    // Reset the context on the active node to ensure that it is possible to detect the error case
    // where the same context tries to enter the lock twice. (enjoy the fence below)
    reinterpret_cast<LockQueueNode *>(&_M_activeNode)->m_pContext = nullptr;

    LockQueueNode * pNextNode = pCurrentNode->m_pNextNode;

    // This assignment must be put before ICEP(_M_pTail) because
    // 1. It must be visible before ICEP(_M_pTail), and
    // 2. It must be visible before unblock.
    _M_pHead = pNextNode;

    // If we reach the end of the queue of waiters, we need to handle a potential race with new incoming waiters.
    if (pNextNode == nullptr && reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pTail, nullptr, pCurrentNode)) != pCurrentNode)
    {
        // If someone is adding a context then wait until next node pointer is populated.
        pNextNode = pCurrentNode->WaitForNextNode();
        // DO NOT try to combine this assignment with the one above by moving it out of and after the if statement. Moving it to after the if statement
        // could lead to situations where the set of _M_pHead is not fenced.
        _M_pHead = pNextNode;
    }

    // It's no longer safe to touch pNextNode after it gets unblocked
    while (pNextNode != nullptr && !pNextNode->Unblock())
    {
        // The unblock could only have failed because the block is a timed block and was woken up by the timer.
        pCurrentNode = pNextNode;
        pNextNode = pCurrentNode->m_pNextNode;

        // This assignment must be put before ICEP(_M_pTail) below
        _M_pHead = pNextNode;

        // If we reach the tail end of the waiters queue, we need to handle a potential race due to a new waiter being added to the tail.
        if (pNextNode == nullptr && reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pTail, nullptr, pCurrentNode)) != pCurrentNode)
        {
            // If someone is adding a context then wait until next node pointer is populated.
            pNextNode = pCurrentNode->WaitForNextNode();
            _M_pHead = pNextNode;
        }

        // Since this was a timer node that was released by the timer firing we need to release our reference on it so it can be deleted.
        pCurrentNode->DerefTimerNode();
    }
}

/// <summary>
///     If no one owns the lock at the instant the API is called, it returns instantly. If there is an owner,
///     it performs a lock followed by an unlock.
/// </summary>
void critical_section::_Flush_current_owner()
{
    if (_M_pTail != NULL)
    {
        lock();
        unlock();
    }
}

/// <summary>
///     Acquires this critical section given a specific node to lock.
/// </summary>
/// <param name="_PLockingNode">
///     The node that needs to own the lock.
/// </param>
/// <returns>
///     Returns true if the lock was acquired, false if the timer woke us up.
/// </returns>
/// <remarks>
///     Throws a improper_lock exception if the lock is acquired recursively
/// </remarks>
bool critical_section::_Acquire_lock(void * _PLockingNode, bool _FHasExternalNode)
{
    LockQueueNode * pNewNode = reinterpret_cast<LockQueueNode *>(_PLockingNode);
    LockQueueNode * pActiveNode = reinterpret_cast<LockQueueNode *>(&_M_activeNode);

    // Locks are non-reentrant, so throw if this condition is detected.
    if (pNewNode->m_pContext == pActiveNode->m_pContext)
    {
        throw improper_lock("Lock already taken");
    }

    LockQueueNode * pPrevious = reinterpret_cast<LockQueueNode *>(InterlockedExchangePointer(&_M_pTail, pNewNode));

    // No one held this critical section, so this context now acquired the lock
    if (pPrevious == NULL)
    {
        _M_pHead = pNewNode;

        pNewNode->UpdateQueuePosition(pActiveNode);
        pNewNode->UnblockWithoutContext();

        // If this was a timed wait, we need to compensate for the fact that the timer callback may race with us,
        // and try to unblock this context. If that happens we need to invoke Context::Block to consume the unblock
        // or the state on the context will be invalid.
        pNewNode->TryCompensateTimer();
    }
    else
    {
        pNewNode->UpdateQueuePosition(pPrevious);
        pPrevious->m_pNextNode = pNewNode;

        // NOT SAFE TO TOUCH pPrevious AFTER THE ASSIGNMENT ABOVE!

        pNewNode->Block(pActiveNode->m_ticketState);

        // Do another position estimation in case we missed the previous number due to race if we've acquired the lock.
        if (pNewNode->m_trigger != TriggeredByTimeout)
        {
            pNewNode->UpdateQueuePosition(pActiveNode);
        }
    }

    // Since calls with external nodes will not call _Switch_to_active, make
    // sure that we are setting the head and the active node properly.
    if (_FHasExternalNode)
    {
        pActiveNode->Copy(pNewNode);
        _M_pHead = pNewNode;
    }

    // The block can be in the NotTriggered state if it is not a timed block and the lock was not already held.
    return pNewNode->m_trigger != TriggeredByTimeout;
}

/// <summary>
///     The acquiring node allocated on the stack never really owns the lock. The reason for that is that
///     it would go out of scope and its insides would not be visible in unlock() where it would potentially
///     need to unblock the next in the queue. Instead, its state is transferred to the internal
///     node which is used as a scratch node.
/// </summary>
/// <param name="_PLockingNode">
///     The node that needs to own the lock.
/// </param>
void critical_section::_Switch_to_active(void * _PLockingNode)
{
    LockQueueNode * pLockingNode = reinterpret_cast<LockQueueNode *>(_PLockingNode);
    LockQueueNode * pActiveNode = reinterpret_cast<LockQueueNode *>(&_M_activeNode);

    //
    // Copy the contents of the node allocated on the stack which now owns the lock, so that we would
    // have its information available during unlock.
    //
    pActiveNode->Copy(pLockingNode);

    //
    // If someone is acquiring the critical_section then wait until next node pointer is populated. Otherwise, there will be no way
    // to unblock that acquiring context after pLockingNode goes out of scope.
    //
    if (pActiveNode->m_pNextNode == NULL)
    {
        //
        // If the compare-and-swap to active node succeeds that means that a new acquirer coming in will
        // properly set the _M_pHead. Otherwise, it has to be set manually when next node is done.
        //
        if (reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pTail, pActiveNode, pLockingNode)) != pLockingNode)
        {
            pLockingNode->WaitForNextNode();

            //
            // During the initial copy the next pointer was not copied over and it has been populated in the meantime.
            // This copy can now be safely performed because tail has moved, so next will point to the second element.
            //
            pActiveNode->Copy(pLockingNode);
        }
    }

    _CONCRT_ASSERT(_PLockingNode != _M_pTail);

    _M_pHead = pActiveNode;
}

/// <summary>
///     Constructs a holder object and acquires the critical_section passed to it.
//      If the critical_section is held by another thread this call will block.
/// </summary>
/// <param name="_Critical_section">
///     Critical section to lock.
/// </param>
critical_section::scoped_lock::scoped_lock(critical_section& _Critical_section) : _M_critical_section(_Critical_section)
{
    static_assert(sizeof(LockQueueNode) <= sizeof(_M_node), "_M_node buffer too small");
    LockQueueNode * pNewNode = reinterpret_cast<LockQueueNode *>(_M_node);
    new(pNewNode) LockQueueNode;
    _M_critical_section._Acquire_lock(pNewNode, true);
}

/// <summary>
///     Destructs a holder object and releases the critical_section.
/// </summary>
critical_section::scoped_lock::~scoped_lock()
{
    _M_critical_section.unlock();
}

/// <summary>
///     Constructs a new reader_writer_lock object.
/// </summary>
_CONCRTIMP reader_writer_lock::reader_writer_lock() : _M_pReaderHead(NULL), _M_pWriterHead(NULL), _M_pWriterTail(NULL), _M_lockState(0)
{
    _CONCRT_ASSERT(sizeof(_M_activeWriter) >= sizeof(LockQueueNode));

    // Hide the inside look of LockQueueNode behind a char array big enough to keep 3 pointers
    // This is why LockQueueNode is newed in place instead of a more traditional allocation.
    new(reinterpret_cast <void *>(_M_activeWriter)) LockQueueNode(NULL, LockQueueNode::StateIsTicketValid);
}

/// <summary>
///     Destructs reader_writer_lock object. If lock is held during the destruction an exception is thrown.
/// </summary>
_CONCRTIMP reader_writer_lock::~reader_writer_lock()
{
    _ASSERT_EXPR(_M_lockState == 0, L"Lock was destructed while held");

    // Since LockQueueNode has a trivial destructor, no need to call it here. If it ever becomes
    // non-trivial then it would be called here instead of calling delete (since memory is allocated
    // in the char array and will be reclaimed anyway when reader_writer_lock is destructed).
}

/// <summary>
///     Writer entering the lock. If there are readers active they are immediately notified to finish
///     and relinquish the lock.
/// </summary>
/// <remarks>
///     Writer blocks by doing spinning on a local variable. Writers are chained so that a writer
///     exiting the lock releases the next writer in line.
/// </remarks>
_CONCRTIMP void reader_writer_lock::lock()
{
    LockQueueNode   newWriterNode;                   // Allocated on the stack and goes out of scope before unlock()
    LockQueueNode * pNewWriter = &newWriterNode;

    //
    // Acquire the lock node that was just created on the stack
    //
    _Acquire_lock(pNewWriter, false);

    //
    // At this point the writer has exclusive ownership of the lock
    //

    _Switch_to_active(pNewWriter);
}

/// <summary>
///     Try to take a writer lock.
/// </summary>
/// <returns>
///     true if the lock is immediately available and lock succeeded; false otherwise.
/// </returns>
_CONCRTIMP bool reader_writer_lock::try_lock()
{
    LockQueueNode   newWriterNode;                   // Allocated on the stack and goes out of scope before unlock()
    LockQueueNode * pNewWriter = &newWriterNode;
    LockQueueNode * pPreviousWriter = reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pWriterTail, pNewWriter, NULL));

    // Is this the only writer present? If yes, it will win over any new writer coming in.
    if (pPreviousWriter == NULL)
    {
        _M_pWriterHead = pNewWriter;

        // Is there any active readers? If no, our lock succeeded.
        if (InterlockedCompareExchange(&_M_lockState, (RWLockWriterInterested | RWLockWriterExclusive), 0) == 0)
        {
            pNewWriter->UpdateQueuePosition(reinterpret_cast<LockQueueNode *>(_M_activeWriter));
            pNewWriter->UnblockWithoutContext();
            _Switch_to_active(pNewWriter);
            return true;
        }
        else
        {
            // Lock failed, but other writers may now be linked to this failed write attempt.
            // Thus, unwind all the actions and leave the lock in a consistent state.
            _Remove_last_writer(pNewWriter);
        }
    }

    return false;
}

/// <summary>
///     Reader entering the lock. If there are writers active readers have to wait until they are done.
///     Reader simply registers an interest in the lock and waits for writers to release it.
/// </summary>
/// <remarks>
///     Reader blocks by doing spinning on a local variable. All readers cache previous reader (if available)
///     locally, so they could all be unblocked once the lock is available.
/// </remarks>
_CONCRTIMP void reader_writer_lock::lock_read()
{
    LockQueueNode   newReaderNode;
    LockQueueNode * pNewReader = &newReaderNode;

    // Locks are non-reentrant, so throw if this condition is detected.
    if (pNewReader->m_pContext == reinterpret_cast<LockQueueNode *>(_M_activeWriter)->m_pContext)
    {
        throw improper_lock("Lock already taken as a writer");
    }

    LockQueueNode * pNextReader = reinterpret_cast<LockQueueNode *>(InterlockedExchangePointer(&_M_pReaderHead, pNewReader));

    //
    // If this is the only read that currently exists and there are no interested writers
    // then unblock this read.
    //
    if (pNextReader == NULL)
    {
        if ((InterlockedOr(&_M_lockState, RWLockReaderInterested) & (RWLockWriterInterested | RWLockWriterExclusive)) == 0)
        {
            LockQueueNode * pHeadReader = reinterpret_cast<LockQueueNode *>(_Get_reader_convoy());

            //
            // If the new reader is still the head of the reader list that means that it is
            // unblocking itself, in which case using UnblockWithoutContext will not include
            // context unblocking. Otherwise, the full unblock/block mechanism is needed.
            //
            if (pHeadReader == pNewReader)
            {
                pHeadReader->UnblockWithoutContext();
                return;
            }

            _CONCRT_ASSERT(pHeadReader != pNewReader);
            pHeadReader->Unblock();
        }
    }
    else
    {
        pNewReader->UpdateBlockingState(pNextReader);
    }

    pNewReader->Block();

    // Unblock the reader that preceeded this one as a head or the list
    if (pNextReader != NULL)
    {
        InterlockedExchangeAdd(&_M_lockState, RWLockReaderCountIncrement);
        pNextReader->Unblock();
    }
}

/// <summary>
///     Try to take a reader lock.
/// </summary>
/// <returns>
///     true if the lock is immediately available and lock succeeded; false otherwise.
/// </returns>
_CONCRTIMP bool reader_writer_lock::try_lock_read()
{
    long oldState = _M_lockState;

    //
    // Try to increment the reader count while no writer is interested.
    //
    while ((oldState & (RWLockWriterInterested | RWLockWriterExclusive)) == 0)
    {
        if (InterlockedCompareExchange(&_M_lockState, oldState + RWLockReaderCountIncrement, oldState) == oldState)
        {
            return true;
        }
        oldState = _M_lockState;
    }

    return false;
}

/// <summary>
///     Unlock the lock based on who locked it, reader or writer.
/// </summary>
_CONCRTIMP void reader_writer_lock::unlock()
{
    if (_M_lockState >= RWLockReaderCountIncrement)
    {
        _Unlock_reader();
    }
    else if ((_M_lockState & RWLockWriterExclusive) != 0)
    {
        _Unlock_writer();
    }
    else
    {
        _ASSERT_EXPR(false, L"Lock not being held");
    }
}

/// <summary>
///     Called for the first context in the writer queue. It sets the queue head and it tries to
///     claim the lock if readers are not active.
/// </summary>
/// <param name="_PWriter">
///     The first writer in the queue.
/// </param>
bool reader_writer_lock::_Set_next_writer(void * _PWriter)
{
    _M_pWriterHead = _PWriter;

    if (((InterlockedOr(&_M_lockState, RWLockWriterInterested) & RWLockReaderInterested) == 0) &&
        (InterlockedOr(&_M_lockState, RWLockWriterExclusive) < RWLockReaderCountIncrement))
    {
        return true;
    }

    return false;
}

/// <summary>
///     Called when writers are done with the lock, or when lock was free for claiming by
///     the first reader coming in. If in the meantime there are more writers interested
///     the list of readers is finalized and they are convoyed, while head of the list
///     is reset to NULL.
/// </summary>
/// <returns>
///     Pointer to the head of the reader list.
/// </returns>
void * reader_writer_lock::_Get_reader_convoy()
{
    // In one interlocked step, clear reader interested flag and increment the reader count.
    long prevLockState = InterlockedExchangeAdd(&_M_lockState, RWLockReaderCountIncrement - RWLockReaderInterested);

    //
    // If a lock is in the race between a reader and a writer allow this last reader batch
    // to go through and then close the lock for the new incoming readers, granting
    // exclusive access to writers.
    //
    if ((prevLockState & RWLockWriterInterested) != 0 && (prevLockState & RWLockWriterExclusive) == 0)
    {
        InterlockedOr(&_M_lockState, RWLockWriterExclusive);
    }

    // Return the batch of readers to be unblocked
    return reinterpret_cast<void *>(InterlockedExchangePointer(&_M_pReaderHead, NULL));
}

/// <summary>
///     Called from unlock() when a writer is holding the lock. Writer unblocks the next writer in the list
///     and is being retired. If there are no more writers, but there are readers interested, then readers
///     are unblocked.
/// </summary>
/// <remarks>
///     If there wasn't for a race to add a writer while the last writer is unlocking the lock, there would be
///     no need for the writer structure in unlock. However, because of this race there is an ABA problem and
///     writer information had to be passed onto a scratch writer (_M_activeWriter), internal to the lock.
/// </remarks>
void reader_writer_lock::_Unlock_writer()
{
    _CONCRT_ASSERT((_M_lockState & RWLockWriterExclusive) != 0);
    _CONCRT_ASSERT(_M_pWriterHead != NULL);

    LockQueueNode * pCurrentNode = reinterpret_cast<LockQueueNode *>(_M_pWriterHead);

    _ASSERT_EXPR(pCurrentNode->m_pContext == SchedulerBase::SafeFastCurrentContext(), L"Lock being held by different writer");

    LockQueueNode * pNextNode = pCurrentNode->m_pNextNode;
    _M_pWriterHead = pNextNode;

    // Reset context on the active writer to ensure that it is possible to detect the error case
    // where the same writer tries to enter the lock twice.
    reinterpret_cast<LockQueueNode *>(&_M_activeWriter)->m_pContext = NULL;

    if (pNextNode != NULL)
    {
        pNextNode->Unblock();
    }
    else
    {
        // If there are readers lined up, then unblock them
        if ((InterlockedAnd(&_M_lockState, ~(RWLockWriterInterested | RWLockWriterExclusive)) & RWLockReaderInterested) != 0)
        {
            LockQueueNode * pHeadNode = reinterpret_cast<LockQueueNode *>(_Get_reader_convoy());
            pHeadNode->Unblock();
        }

        // Safely remove this writer, keeping in mind there might be a race for the queue tail.
        _Remove_last_writer(pCurrentNode);
    }
}

/// <summary>
///     When last writer leaves the lock it needs to reset the tail to NULL so that the next coming
///     writer would know to try to grab the lock. If the CAS to NULL fails, then some other writer
///     managed to grab the tail before the reset, so this writer needs to wait until the link to
///     the next writer is complete before trying to release the next writer.
/// </summary>
/// <param name="_PWriter">
///     Last writer in the queue.
/// </param>
void reader_writer_lock::_Remove_last_writer(void * _PWriter)
{
    // If someone is adding a writer then wait until next node pointer is populated.
    if (reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pWriterTail, NULL, _PWriter)) != _PWriter)
    {
        LockQueueNode * pWriter = reinterpret_cast<LockQueueNode *>(_PWriter);
        LockQueueNode * pNextWriter = pWriter->WaitForNextNode();

        if (_Set_next_writer(pNextWriter))
        {
            pNextWriter->Unblock();
        }
    }
}

/// <summary>
///     Acquires a write lock given a specific write node to lock.
/// </summary>
/// <param name="_PLockingNode">
///     The node that needs to own the lock.
/// </param>
/// <param name="_FHasExternalNode">
///     Whether the node being locked is external to the reader_writer_lock.
/// </param>
/// <remarks>
///     Throws a improper_lock exception if the lock is acquired recursively
/// </remarks>
void reader_writer_lock::_Acquire_lock(void * _PLockingNode, bool _FHasExternalNode)
{
    LockQueueNode * pNewWriter = reinterpret_cast<LockQueueNode *>(_PLockingNode);
    LockQueueNode * pActiveWriter = reinterpret_cast<LockQueueNode *>(_M_activeWriter);

    // Locks are non-reentrant, so throw if this condition is detected.
    if (pNewWriter->m_pContext == reinterpret_cast<LockQueueNode *>(pActiveWriter)->m_pContext)
    {
        throw improper_lock("Lock already taken");
    }

    LockQueueNode * pPreviousWriter = reinterpret_cast<LockQueueNode *>(InterlockedExchangePointer(&_M_pWriterTail, pNewWriter));

    bool doNeedBlock = true;

    if (pPreviousWriter == NULL)
    {
        pNewWriter->UpdateQueuePosition(pActiveWriter);

        // This is the only write that currently exists
        if (_Set_next_writer(pNewWriter))
        {
            doNeedBlock = false;
            pNewWriter->UnblockWithoutContext();
        }
    }
    else
    {
        pNewWriter->UpdateQueuePosition(pPreviousWriter);
        pPreviousWriter->m_pNextNode = pNewWriter;

        // Note: pPreviousWriter is *unsafe* after the assignment above!
    }

    // Don't block if the context unblocked itself already
    if (doNeedBlock)
    {
        pNewWriter->Block(pActiveWriter->m_ticketState);

        // Do another position estimation in case we missed the previous number due to race
        pNewWriter->UpdateQueuePosition(pActiveWriter);
    }

    // Since calls with external nodes will not call _Switch_to_active, make
    // sure that we are setting the head and the active node properly.
    if (_FHasExternalNode)
    {
        pActiveWriter->Copy(pNewWriter);
        // NOTE: The write to _M_pWriterHead below could be re-ordered on ARM with the writes within the ->Copy() above (when Copy is inlined).
        // However, this is not an issue because _M_pWriterHead is not concurrently read when the lock has already been acquired by the writer
        // (which is the case here). The read of _M_pWriterHead in _Unlock_reader can occur only when a writer is blocked or about to block.
        _M_pWriterHead = pNewWriter;
    }
}

/// <summary>
///     The writer node allocated on the stack never really owns the lock. The reason for that is that
///     it would go out of scope and its insides would not be visible in unlock() where it would potentially
///     need to unblock the next writer in the queue. Instead, its state is transferred to the internal
///     writer node which is used as a scratch node.
/// </summary>
/// <param name="_PWriter">
///     The writer that needs to own the lock.
/// </param>
void reader_writer_lock::_Switch_to_active(void * _PWriter)
{
    _CONCRT_ASSERT((_M_lockState & RWLockWriterExclusive) != 0);

    LockQueueNode * pWriter = reinterpret_cast<LockQueueNode *>(_PWriter);
    LockQueueNode * pActiveWriter = reinterpret_cast<LockQueueNode *>(_M_activeWriter);

    //
    // Copy the contents of the writer allocated on the stack which now owns the lock, so that we would
    // have its information available during unlock.
    //
    pActiveWriter->Copy(pWriter);

    //
    // If someone is adding a writer then wait until next node pointer is populated. Otherwise, there will be no way
    // to unblock the next writer after newWriterNode goes out of scope.
    //
    if (pActiveWriter->m_pNextNode == NULL)
    {
        //
        // If the compare-and-swap to active writer succeeds that means that a new writer coming in will call _Set_next_writer, which
        // will properly set the _M_pWriterHead. Otherwise, it has to be set manually when next node is done.
        //
        if (reinterpret_cast<LockQueueNode *>(InterlockedCompareExchangePointer(&_M_pWriterTail, pActiveWriter, pWriter)) != pWriter)
        {
            pWriter->WaitForNextNode();

            //
            // During the initial copy the next pointer was not copied over and it has been populated in the meantime.
            // This copy can now be safely performed because tail has moved, so next will point to the second element.
            //
            pActiveWriter->Copy(pWriter);
        }
    }

    _CONCRT_ASSERT(_PWriter != _M_pWriterTail);
    // NOTE: The write to _M_pWriterHead below could be re-ordered on ARM with the writes within the ->Copy() above (when Copy is inlined).
    // However, this is not an issue because _M_pWriterHead is not concurrently read when the lock has already been acquired by the writer
    // (which is the case here). The read of _M_pWriterHead in _Unlock_reader can occur only when a writer is blocked or about to block.
    _M_pWriterHead = pActiveWriter;
}

/// <summary>
///     Called from unlock() when a reader is holding the lock. Reader count is decremented and if this
///     is the last reader it checks whether there are interested writers that need to be unblocked.
/// </summary>
void reader_writer_lock::_Unlock_reader()
{
    long resultState = InterlockedExchangeAdd(&_M_lockState, -RWLockReaderCountIncrement);

    //
    // If this is the last reader and there are writers lined up then unblock them. However,
    // if exclusive writer flag is not set, then writers will take care of themselves.
    //
    if ((resultState & (~RWLockReaderInterested)) == (RWLockReaderCountIncrement | RWLockWriterInterested | RWLockWriterExclusive))
    {
        _CONCRT_ASSERT(_M_pWriterTail != NULL);
        reinterpret_cast<LockQueueNode *>(_M_pWriterHead)->Unblock();
    }
}

/// <summary>
///     Constructs a holder object and acquires the reader_writer_lock passed to it.
//      If the reader_writer_lock is held by another thread this call will block.
/// </summary>
/// <param name="_Reader_writer_lock">
///     Reader writer to lock.
/// </param>
reader_writer_lock::scoped_lock::scoped_lock(reader_writer_lock& _Reader_writer_lock) : _M_reader_writer_lock(_Reader_writer_lock)
{
    static_assert(sizeof(LockQueueNode) <= sizeof(_M_writerNode), "_M_writerNode buffer too small");
    LockQueueNode * pNewWriterNode = reinterpret_cast<LockQueueNode *>(_M_writerNode);
    new(pNewWriterNode) LockQueueNode;
    _M_reader_writer_lock._Acquire_lock(pNewWriterNode, true);
}

/// <summary>
///     Destructs a holder object and releases the reader_writer_lock.
/// </summary>
reader_writer_lock::scoped_lock::~scoped_lock()
{
    _M_reader_writer_lock.unlock();
}

/// <summary>
///     Constructs a holder object and acquires the reader_writer_lock passed to it.
//      If the reader_writer_lock is held by another thread this call will block.
/// </summary>
/// <param name="_Reader_writer_lock">
///     Reader Writer to lock.
/// </param>
reader_writer_lock::scoped_lock_read::scoped_lock_read(reader_writer_lock& _Reader_writer_lock) : _M_reader_writer_lock(_Reader_writer_lock)
{
    _M_reader_writer_lock.lock_read();
}

/// <summary>
///     Destructs a holder object and releases the reader_writer_lock.
/// </summary>
reader_writer_lock::scoped_lock_read::~scoped_lock_read()
{
    _M_reader_writer_lock.unlock();
}

} // namespace Concurrency
