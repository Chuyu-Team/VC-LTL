// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// WorkStealingQueue.h
//
// Header file containing the core implementation of the work stealing data structures and algorithms.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#define AFFINITY_EXECUTED 0x1
#define IS_AFFINITIZED_TASK(t) ((ULONG_PTR)t & 0x1)
#define STRIP_AFFINITY_INDICATOR(type, t) ((type *)((ULONG_PTR)t & ~((ULONG_PTR)0x1)))
#define ADD_AFFINITY_INDICATOR(type, t) ((type *)((ULONG_PTR)(t) | 0x1))

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A WorkStealingQueue is a wait-free, lock-free structure associated with a single
    ///     thread that can Push and Pop elements. Other threads can do Steal operations
    ///     on the other end of the WorkStealingQueue with little contention.
    /// </summary>
    template <typename T, typename LOCK=_NonReentrantLock>
    class WorkStealingQueue
    {
        // A 'WorkStealingQueue' always runs its code in a single OS thread. We call this the
        // 'bound' thread. Only the code in the Steal operation can be executed by
        // other 'foreign' threads that try to steal work.
        //
        // The queue is implemented as an array. The m_head and m_tail index this
        // array. To avoid copying elements, the m_head and m_tail index the array modulo
        // the size of the array. By making this a power of two, we can use a cheap
        // bit-and operation to take the modulus. The "m_mask" is always equal to the
        // size of the task array minus one (where the size is a power of two).
        //
        // The m_head and m_tail are volatile as they can be updated from different OS threads.
        // The "m_head" is only updated by foreign threads as they Steal a task from
        // this queue. By putting a lock in Steal, there is at most one foreign thread
        // changing m_head at a time. The m_tail is only updated by the bound thread.
        //
        // invariants:
        //   tasks.length is a power of 2
        //   m_mask == tasks.length-1
        //   m_head is only written to by foreign threads
        //   m_tail is only written to by the bound thread
        //   At most one foreign thread can do a Steal
        //   All methods except Steal are executed from a single bound thread
        //   m_tail points to the first unused location
        //
        // This work stealing implementation also supports the notion of out-of-order waiting
        // and out-of-order removal from the bound thread given that it is initialized to do so.
        // There is additional cost to performing this.
        //

    public:

        /// <summary>
        ///     The callback for a sweep of the workstealing queue.  This will be called under the stealing lock on the owning thread
        ///     for every chore matching a predefined predicate.  If true is returned, the item is pulled from the WSQ.  If false is returned,
        ///     the item stays in the WSQ.
        /// </summary>
        typedef bool (*SweepFunction)(T *pObject, void *pData);

        /// <summary>
        ///     A predicate for a WSQ sweep.
        /// </summary>
        typedef bool (*SweepPredicate)(T *pObject, void *pData);


        /// <summary>
        ///     Constructs a new work stealing queue
        /// </summary>
        WorkStealingQueue(LOCK *pLock)
            : m_pLock(pLock)
        {
            ASSERT(m_pLock != NULL);
            ASSERT(s_initialSize > 1);
            Reinitialize();
            m_mask = s_initialSize - 1;
            m_pTasks = _concrt_new T*[s_initialSize];
            m_pSlots = _concrt_new typename Mailbox<T>::Slot[s_initialSize];
        }

        /// <summary>
        ///     Reinitializes a workqueue to the state just after construction.  This is used when recycling a work
        ///     queue from its ListArray
        /// </summary>
        void Reinitialize()
        {
            m_head = 0;
            m_tail = 0;
            m_detachmentTail = 0;
            m_fMarkedForDetachment = false;
            m_cookieBase = 0;
        }

        /// <summary>
        ///     Unlocked count
        /// </summary>
        int Count() const
        {
            return (m_tail - m_head);
        }

        /// <summary>
        ///     Unlocked check if empty
        /// </summary>
        bool Empty() const
        {
            return (m_tail <= m_head);
        }

        /// <summary>
        ///     Check whether to skip the steal
        /// </summary>
        bool MarkedForDetachment() const
        {
            return m_fMarkedForDetachment;
        }

        //
        // Push/Pop and Steal can be executed interleaved. In particular:
        // 1) A steal and pop should be careful when there is just one element
        //    in the queue. This is done by first incrementing the m_head/decrementing the m_tail
        //    and than checking if it interleaved (m_head > m_tail).
        // 2) A push and steal can interleave in the sense that a push can overwrite the
        //    value that is just stolen. To account for this, we check conservatively in
        //    the push to assume that the size is one less than it actually is.
        //

        /// <summary>
        ///     Attempts to steal the oldest element in the queue.  This handles potential interleaving with both
        ///     a Pop and TryPop operation.
        /// </summary>
        T* UnlockedSteal(bool fForceStealLocalized, bool = false)
        {
            T* pResult = NULL;

            for (;;)
            {
                //
                // increment the m_head. Save in local h for efficiency
                //
                int h = m_head;

                InterlockedExchange((volatile LONG*)&m_head, h + 1);

                //
                // insert a memory fence here -- memory may not be sequentially consistent
                //

                if (h < m_tail)
                {
                    //
                    // Do not allow a steal from this work stealing queue if the bottom task was mailed to a location which has active searchers.
                    // This will not block finalization in any way as the last pass SFW will pull the task out of the mailbox regardless of affinity.
                    // We should be careful not to do this if the current context's virtual processor is in the affinity set of the segment.
                    // If not, there could be cases where all virtual processors deactivate, but the scheduler does not shutdown since there are chores
                    // in the queue, even if they have been dequeued via the mailbox.
                    //
                    if(IS_AFFINITIZED_TASK(m_pTasks[h & m_mask]))
                    {
                        if (!fForceStealLocalized && m_pSlots[h & m_mask].DeferToAffineSearchers())
                        {
                            //
                            // Skip this workqueue if there are affine searchers and we are not one of them.
                            //
                            m_head = h;
                            return NULL;
                        }
                    }

                    //
                    // If the queue is detached and we've crossed the point of detachment, end the detachment marker.
                    //
                    if (m_fMarkedForDetachment && m_head >= m_detachmentTail)
                        m_fMarkedForDetachment = false;

                    //
                    // == (h+1 <= m_tail) == (m_head <= m_tail)
                    //
                    // When we allow out-of-order waits, it's entirely possible that a TryPop
                    // executing on the bound thread will grab this out from underneath us.  Not
                    // only do we need guards against interleave with ordered pop, but we also
                    // need a guard against an out-of-order trypop.
                    //
                    pResult = reinterpret_cast<T*> (InterlockedExchangePointer(
                        reinterpret_cast<PVOID volatile *>( &(m_pTasks[h & m_mask])),
                        (PVOID) NULL
                        ));

                    if (pResult != NULL)
                    {
#if _UMSTRACE && _CHORETRACK
                        CCMTRACE(MTRACE_EVT_VIEWWSQ, pResult)
#endif // _UMSTRACE && _CHORETRACK

                        //
                        // If the task had an associated affinity, we must deal with the possibility that it was already executed by a virtual
                        // processor to which it was affine.
                        //
                        if (IS_AFFINITIZED_TASK(pResult))
                        {
                            pResult = STRIP_AFFINITY_INDICATOR(T, pResult);

                            //
                            // If the task was already executed via a mailbox dequeue, move on and try to steal again.
                            //
                            if (!m_pSlots[h & m_mask].Claim())
                            {
                                pResult = NULL;
                                continue;
                            }
                        }
                        break;
                    }
                }
                else
                {
                    //
                    // failure: either empty or single element interleaving with pop
                    //
                    m_head = h;              // restore the m_head
                    break;
                }
            }

#if _UMSTRACE && _CHORETRACK
            CCMSTRACE(MTRACE_EVT_STEALUNREALIZED, pResult);
#endif // _UMSTRACE && _CHORETRACK

            return pResult;
        }

        // only used in a test
        T* Steal(bool fForceStealLocalized = false)
        {
            typename LOCK::_Scoped_lock lockHolder(*m_pLock);
            return UnlockedSteal(fForceStealLocalized);
        }

        /// <summary>
        ///     Attempts to pop the newest element on the work stealing queue.  It may return NULL if there is no such
        ///     item (either unbalanced push/pop, a chore stolen)
        /// </summary>
        T* Pop()
        {
            T* pResult = NULL;
            int t;

            for(;;)
            {
                //
                // decrement the m_tail. Use local t for efficiency.
                //
                t = m_tail - 1;
                InterlockedExchange((volatile LONG*)&m_tail, t);

                //
                // insert a memory fence here (InterlockedExchange does the job) --
                // memory may not be sequentially consistent
                //

                if (m_head <= t)
                {
                    //
                    // == (m_head <= m_tail)
                    //
                    pResult = m_pTasks[t & m_mask];

                    //
                    // Out of order TryPops on the bound thread will set this without
                    // the need for a fence.
                    //
                    if (pResult == NULL) continue;
                    break;
                }
                else
                {
                    //
                    // failure: either empty or single element interleaving with steal
                    //
                    m_tail = t + 1;     // restore the m_tail
                    return SyncPop();   // do a single-threaded pop
                }
            }

            if (IS_AFFINITIZED_TASK(pResult))
            {
                pResult = STRIP_AFFINITY_INDICATOR(T, pResult);

                //
                // If the task was already executed via a mailbox dequeue, return an indication to the caller.
                //
                if (!m_pSlots[t & m_mask].Claim())
                {
#if _UMSTRACE && _CHORETRACK
                    CCMSTRACE(MTRACE_EVT_AFFINITYEXECUTED, pResult);
#endif // _UMSTRACE && _CHORETRACK

                    return (T*)AFFINITY_EXECUTED;
                }
            }

            return pResult;
        }

        /// <summary>
        ///     Tries to pop a previously pushed element from the work stealing queue.  Note that this executes
        ///     a potentially out-of-order wait.
        /// </summary>
        /// <param name="cookie">
        ///     The value returned from a Push() call for the work in question
        /// </param>
        T* TryPop(int cookie)
        {
            cookie = (cookie - m_cookieBase);

            //
            // TryPop() has Pop() semantics if we try the topmost element.  We only need to do something
            // "special" in the out of order case.
            //
            if (cookie == m_tail - 1) return Pop();

            if (cookie >= m_tail || cookie < m_head) return NULL;

            T* pResult = reinterpret_cast<T*> (InterlockedExchangePointer(
                reinterpret_cast<PVOID volatile *>( &(m_pTasks[cookie & m_mask])),
                (PVOID) NULL
                ));

            if (IS_AFFINITIZED_TASK(pResult))
            {
                pResult = STRIP_AFFINITY_INDICATOR(T, pResult);

                //
                // If the task was already executed via a mailbox dequeue, return an indication to the caller.
                //
                if (!m_pSlots[cookie & m_mask].Claim())
                {
#if _UMSTRACE && _CHORETRACK
                    CCMSTRACE(MTRACE_EVT_AFFINITYEXECUTED, pResult);
#endif // _UMSTRACE && _CHORETRACK

                    return (T*)AFFINITY_EXECUTED;
                }
            }

            return pResult;

        }

        /// <summary>
        ///     Pushes an element onto the work stealing queue.  The returned cookie can be utilized to identify
        ///     the work item for a future TryPop() call.  Note that the returned cookie is only valid until a Pop()
        ///     or TryPop() call removes the work in question.
        /// </summary>
        int Push(T* elem, typename ::Concurrency::details::Mailbox<T>::Slot affinitySlot)
        {
            int t = m_tail;
            //
            // Careful here since we might interleave with Steal.
            // This is no problem since we just conservatively check if there is
            // enough space left (t < m_head + size). However, Steal might just have
            // incremented m_head and we could potentially overwrite the old m_head
            // entry, so we always leave at least one extra 'buffer' element and
            // check (m_tail < m_head + size - 1). This also plays nicely with our
            // initial m_mask of 0, where size is 2^0 == 1, but the tasks array is
            // still null.
            //
            if (t < m_head + m_mask)  // == t < m_head + size - 1
            {
                if (!affinitySlot.IsEmpty())
                {
#if _UMSTRACE && _CHORETRACK
                    CCMSTRACE(MTRACE_EVT_TAGAFFINITY, elem);
#endif // _UMSTRACE && _CHORETRACK

                    //
                    // Flag the element as affinitized.  On popping this element, the box slot must be cleared to prevent
                    // multiple execution.
                    //
                    m_pSlots[t & m_mask] = affinitySlot;
                    elem = ADD_AFFINITY_INDICATOR(T, elem);
                }
                m_pTasks[t & m_mask] = elem;
                // Only increment once we have initialized the task entry. This is a volatile write and has release semantics on weaker memory models
                m_tail = t + 1;
                return t + m_cookieBase;
            }
            else
            {
                //
                // failure: we need to resize or re-index
                //
                return SyncPush(elem, affinitySlot);
            }
        }

        /// <summary>
        ///     Pushes an element onto the work stealing queue.  The returned cookie can be utilized to identify
        ///     the work item for a future TryPop() call.  Note that the returned cookie is only valid until a Pop()
        ///     or TryPop() call removes the work in question.
        /// </summary>
        int Push(T* elem)
        {
            int t = m_tail;
            //
            // Careful here since we might interleave with Steal.
            // This is no problem since we just conservatively check if there is
            // enough space left (t < m_head + size). However, Steal might just have
            // incremented m_head and we could potentially overwrite the old m_head
            // entry, so we always leave at least one extra 'buffer' element and
            // check (m_tail < m_head + size - 1). This also plays nicely with our
            // initial m_mask of 0, where size is 2^0 == 1, but the tasks array is
            // still null.
            //
            if (t < m_head + m_mask)  // == t < m_head + size - 1
            {
                m_pTasks[t & m_mask] = elem;
                // Only increment once we have initialized the task entry. This is a volatile write and has release semantics on weaker memory models
                m_tail = t + 1;
                return t + m_cookieBase;
            }
            else
            {
                //
                // failure: we need to resize or re-index
                //
                return SyncPush(elem, Mailbox<T>::Slot());
            }
        }

        /// <summary>
        ///     Only called from the bound thread, this sweeps the work stealing queue under the steal lock for any chores matching the
        ///     specified predicate.
        /// </summary>
        void Sweep(SweepPredicate pPredicate, void *pData, SweepFunction pSweepFn)
        {
            typename LOCK::_Scoped_lock lockHolder(*m_pLock);

            int nt = m_tail;
            int t = m_tail - 1;

            while (t >= m_head)
            {
                T* pResult = m_pTasks[t & m_mask];
                if (pResult != NULL)
                {
                    if (pPredicate(pResult, pData))
                    {
                        if (pSweepFn(pResult, pData))
                        {
                            //
                            // If it's atop the WSQ, just decrement the tail (nt == new tail); otherwise,
                            // make sure to NULL out the entry to indicate an out-of-order rip.
                            //
                            if (t + 1 == nt)
                                nt--;
                            else
                                m_pTasks[t & m_mask] = NULL;
                        }
                    }
                }

                t--;
            }

            InterlockedExchange((volatile LONG *)&m_tail, nt);
        }

        /// <summary>
        ///     Marks the work stealing queue as detached.  The current head pointer marks the end point of detachment.  Note
        ///     that this should only be called when there is a guarantee of no concurrent pushes or pops from the owning thread.
        /// </summary>
        void MarkDetachment()
        {
            typename LOCK::_Scoped_lock lockHolder(*m_pLock);
            m_fMarkedForDetachment = true;
            m_detachmentTail = m_tail;
        }

        /// <summary>
        ///     Destroys a work stealing queue.
        /// </summary>
        ~WorkStealingQueue()
        {
            delete [] m_pTasks;
            delete [] m_pSlots;
        }

    private:

        // The m_head and m_tail are volatile as they can be updated from different OS threads.
        // The "m_head" is only updated by foreign threads as they Steal a task from
        // this queue. By putting a lock in Steal, there is at most one foreign thread
        // changing m_head at a time. The m_tail is only updated by the bound thread.
        //
        // invariants:
        //   tasks.length is a power of 2
        //   m_mask == tasks.length-1
        //   m_head is only written to by foreign threads
        //   m_tail is only written to by the bound thread
        //   At most one foreign thread can do a Steal
        //   All methods except Steal are executed from a single bound thread
        //   m_tail points to the first unused location
        //

        static const int s_initialSize = 64;  // must be a power of 2
        volatile int m_head;                  // only updated by Steal
        volatile int m_tail;                  // only updated by Push and Pop

        int m_mask;                           // the m_mask for taking modulus
        int m_cookieBase;                     // the base cookie index

        LOCK *m_pLock;                        // the lock that guards stealing from push/pops

        bool m_fMarkedForDetachment;          // Indicates whether or not the work stealing queue is marked for detachment
        int m_detachmentTail;                 // The tail pointer for detachment.  When the head crosses this, the mark ends

        T**  m_pTasks;                        // the array of tasks
        typename Mailbox<T>::Slot *m_pSlots;  // the array of side-band affinity structures

        /// <summary>
        ///     Pushes an element onto the work stealing queue under the queue lock.  This guarantees that no steal
        ///     interleaves and guarantees the ability to reallocate the physical store.  The returned value is a cookie
        ///     per Push().
        /// </summary>
        int SyncPush(T* elem, typename ::Concurrency::details::Mailbox<T>::Slot affinitySlot)
        {
            //
            // Because WorkStealingQueue is used for LRC and LRC needs to be searched in a SFW from a UMS primary, the lock here is a hyper
            // lock and no memory allocations can happen inside its scope.  Preallocate everything up front!
            //
            // Keep in mind that the only thing that's going to happen without the lock held is a steal.  No one else will try to resize,
            // pop, push, etc...
            //
            //
            // == (count >= size-1)
            //
            int oldsize = m_mask + 1;
            int newsize = 2 * oldsize; // highly unlikely, but throw out-of-memory if this overflows
            int count = m_tail - m_head;

            //
            // Yes -- it's entirely possible that we allocate and DON'T need to in rare circumstances - steal just opened up a slot.  In that particular
            // case, we will just do the resizing since it's almost full.
            //
            T** pNewTasks = _concrt_new T*[newsize];
            //
            // Again, for reasons of UMS, we cannot delete the old array until after we release the hyper lock.  Stash it away
            // and defer the deletion.
            //
            T** pOldTasks = m_pTasks;

            typename Mailbox<T>::Slot *pNewSlots = _concrt_new typename Mailbox<T>::Slot[newsize];
            typename Mailbox<T>::Slot *pOldSlots = m_pSlots;

            {
                //
                // ensure that no Steal interleaves here
                //
                typename LOCK::_Scoped_lock lockHolder(*m_pLock);

                //
                // cache m_head, and calculate number of tasks
                //
                int h = m_head;
                count = m_tail - h;

                //
                // normalize indices
                //
                h = h & m_mask;           // normalize m_head
                m_cookieBase += m_tail - (h + count);
                m_head = h;
                m_tail = h + count;

#pragma warning(disable:26010)

                // we get here the first time we've overflowed,
                // so as long as m_mask >= 1, which is asserted in the ctor, there's plenty of room
                CONCRT_COREASSERT(count < newsize);
                CONCRT_COREASSERT(pNewTasks != NULL);
                for (int i = 0; i < count; ++i)
                {
                    pNewTasks[i] = m_pTasks[(h + i) & m_mask];
                    pNewSlots[i] = m_pSlots[(h + i) & m_mask];
                }

                m_pTasks = pNewTasks;
                m_pSlots = pNewSlots;

#pragma warning(default:26010)

                //
                // Rebase the cookie index.  We can't hand out duplicate cookies due to this.
                //
                m_cookieBase += m_head;

                //
                // Rebase the detachment point if necessary.
                //
                if (m_fMarkedForDetachment)
                {
                    CONCRT_COREASSERT(m_detachmentTail >= m_head);
                    m_detachmentTail -= m_head;
                }

                m_mask = newsize - 1;
                m_head = 0;
                m_tail = count;

                CONCRT_COREASSERT(count < m_mask);

                //
                // push the element
                //
                int t = m_tail;

                if (!affinitySlot.IsEmpty())
                {
#if _UMSTRACE && _CHORETRACK
                    CCMSTRACE(MTRACE_EVT_TAGAFFINITY, elem);
#endif // _UMSTRACE && _CHORETRACK

                    //
                    // Flag the element as affinitized.  On popping this element, the box slot must be cleared to prevent
                    // multiple execution.
                    //
                    m_pSlots[t & m_mask] = affinitySlot;
                    elem = ADD_AFFINITY_INDICATOR(T, elem);
                }

                m_pTasks[t & m_mask] = elem;
                m_tail = t + 1;

            }

            delete[] pOldTasks;
            delete[] pOldSlots;
            return m_tail - 1 + m_cookieBase;
        }

        /// <summary>
        ///     Synchronously pops an element from the work stealing queue.  Note that this is called in the case where
        ///     a Pop() call and a Steal() call interleave.
        /// </summary>
        T* SyncPop()
        {
            //
            // ensure that no Steal interleaves with this pop
            //
            typename LOCK::_Scoped_lock lockHolder(*m_pLock);

            typename Mailbox<T>::Slot affinitySlot;

            T* pResult = NULL;
            int t = m_tail - 1;
            m_tail = t;
            if (m_head <= t)
            {
                //
                // == (m_head <= m_tail)
                //
                pResult = m_pTasks[t & m_mask];

                //
                // Because this was a single element / interleave with steal, there is nothing
                // below this in the WSQ in the event of a NULL return.  Hence, we do not need
                // to perform an explicit skip as in Pop().
                //
                affinitySlot = m_pSlots[t & m_mask];
            }
            else
            {
                m_tail = t + 1;       // restore m_tail
            }
            if (m_head >= t)
            {
                //
                // Rebase the cookie index so we guarantee that currently handed out cookie values are
                // still valid until they are trypop()'d.
                //
                m_cookieBase += m_head;

                //
                // queue is empty: reset m_head and m_tail
                //
                m_head = 0;
                m_tail = 0;
                m_detachmentTail = 0;
                m_fMarkedForDetachment = false;
            }

            if (IS_AFFINITIZED_TASK(pResult))
            {
                pResult = STRIP_AFFINITY_INDICATOR(T, pResult);

                //
                // If the task was already executed via a mailbox dequeue, return an indication to the caller.
                //
                if (!affinitySlot.Claim())
                {
#if _UMSTRACE && _CHORETRACK
                    CCMSTRACE(MTRACE_EVT_AFFINITYEXECUTED, pResult);
#endif // _UMSTRACE && _CHORETRACK

                    return (T*)AFFINITY_EXECUTED;
                }
            }

            return pResult;
        }

    };
} // namespace details
} // namespace Concurrency
