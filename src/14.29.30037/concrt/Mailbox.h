// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Mailbox.h
//
// Class definition for task affine mailbox.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#define SLOT_PENDING_EXPIRY 1
#define FIELD_RESERVED 1

namespace Concurrency
{
namespace details
{
    // This helper is used to avoid circular reference:
    // Mailbox -> InternalContextBase -> ContextBase -> WorkStealingQueue -> Mailbox
    unsigned int GetProcessorMaskId(InternalContextBase * pContext);

    // *** NOTES ***
    //
    // Work stealing queues are associated with each context that the scheduler runs.  Each context is, by nature of what it has executed or been bound to,
    // associated with a given schedule group segment and hence has a natural affinity.  This presents an interesting semantic with respect to work
    // stealing tasks scheduled from that context.  Imagine:
    //
    // CONTEXT A (affinity locA)
    //
    //     tg.run(lambda1);
    //     tg.run(lambda2, loc2);
    //     tg.run(lambda3, loc3);
    //     tg.run(lambda4, loc4);
    //     tg.run(lambda5, loc5);
    //     tg.wait();
    //
    // In this circumstance, lambda1 through lambda5 are pushed onto the work stealing queue associated with CONTEXT A.  Because context A has an associated
    // affinity (locA -- which might not be a *specific* affinity -- it might be the system), all of these tasks have a natural affinity to locA.  When
    // tasks lambda2 through lambda5 are scheduled, the caller has requested that, if stolen, those tasks run on locations other than the natural affinity
    // of the work stealing queue.  In order to accomodate this in a work stealing scheduler, we take the tact of mailing lambda2 through lambda5 to a mailbox.
    // The mailbox will be contained within a segment with affinity loc2, loc3, ..., loc5 within the schedule *GROUP* of CONTEXT A.
    //
    // This means that tasks lambda2 through lambda5 will be contained in two places in the scheduler simultaneously:
    //
    //     - On the work stealing queue for context A (which has natural affinity to locA)
    //     - On a mailbox within the group of context A with affinity loc*
    //
    // In this case, the affinities are chained.  lambda2 has primary affinity to loc2 and secondary affinity to locA.  If a vproc within loc2 is available,
    // it will go there; otherwise, if a vproc within locA is available, it will go there; otherwise, it will go anywhere subject to the rules of SFW.
    //
    // Having a given task in two places presents an interesting problem: task lifetime.  The ConcRT scheduler is not always in control of the lifetime of
    // objects that are pushed onto the work stealing queue.  A lambda which is scheduled to a task_group has lifetime owned by the scheduler.  A task_handle
    // which is scheduled to a (structured_)task_group has lifetime which is managed by the caller.
    //
    // Once a task is executed from *either* queue (the mailbox or the WSQ), the task can no longer safely be touched by the runtime.  In order to allow
    // for this, affine tasks work as follows: the low bit of the chore pointer on the WSQ is utilized to indicate whether a task is an affine (mailed) task
    // or not.  If the task is not affine, things work as they always have.  If the task *IS* affine, the WSQ keeps a side structure which holds a slot
    // for the given WSQ chore.  The "slot" is Mailbox<T>::Slot.  The chore cannot be touched until ClaimSlot is called successfully.  The mailbox can do
    // whatever is necessary under the covers to implement this interface.
    //

    //
    // At present, there are some rather subtle lifetime rules about mailboxes and the objects which actually manage their storage.
    //
    // - A mailbox is bound to the lifetime of a schedule group segment.  Mailboxes have two sub-objects: slots and segments.  Both of these objects
    //   can outlive the mailbox!
    //
    // - A mailbox slot is a handle to some location within a given mailbox.  The slot object is valid until Claim() is called on it.  After this method
    //   returns, the slot is invalid.  Calling a method on it again will result in undefined behavior.
    //
    // - A mailbox segment is the backing storage for a portion of the mailbox queue.  Slots are chained and allocated in FIFO order to amortize the cost
    //   of allocation.  Excepting the amortized allocation, a mailbox is lock-free (though not wait-free).  Mailbox segments have an implicit reference on them
    //   for every slot within the segment.  The segment is freed once EVERY reference is removed.  Mailbox segments are only freed at safe points to give the
    //   Dequeue code extra safety.  This implies that Dequeue operations on a mailbox must happen on an internal context within a critical region.
    //

    //
    /// <summary>
    ///     A lock-free fixed size FIFO of tasks associated with a particular object.  The mailbox is typically used
    ///     for work stealing tasks affine to a particular location.
    /// </summary>
    template<typename T>
    class Mailbox
    {
    private:

        /// <summary>
        ///     Represents a segment of a mailbox which contains a fixed number of slots.
        /// </summary>
        struct Segment
        {
            /// <summary>
            ///     Constructs a new segment.
            /// </summary>
            Segment(SchedulerBase *pScheduler, const QuickBitSet &affinitySet, unsigned int size, unsigned int baseIdx) :
                m_pScheduler(pScheduler), m_affinitySet(affinitySet), m_baseIdx(baseIdx), m_refs(0), m_pNext(NULL)
            {
                m_pQueue = _concrt_new T* volatile [size];
                memset((void*)(m_pQueue), 0, sizeof(T* volatile) * size);
            }

            /// <summary>
            ///     Destroys a segment.
            /// </summary>
            ~Segment()
            {
                delete[] m_pQueue;
            }

            bool AllSlotsClaimed(unsigned int count)
            {
                // Note that if this segment has already had its deletion refs set after all slots were claimed, this will
                // return false. However, for the purpose we are using it for (deciding whether or not to set deletion refs),
                // this is not a problem.
                return (m_refs + count == 0);
            }

            /// <summary>
            ///     Removes a reference from the segment.
            /// </summary>
            void Dereference()
            {
                if (static_cast<unsigned int>(InterlockedDecrement(reinterpret_cast<volatile long *>(&m_refs))) == 0)
                    Expire();
            }

            /// <summary>
            ///     Expires a segment.
            /// </summary>
            void Expire()
            {
                //
                // This can be called during search-for-work as we touch a work stealing queue that has had a task mailed.  We do *NOT* want heap
                // oprerations in search-for-work at ANY point.  As such, the deletion gets deferred to the scheduler's next safe point.
                //
                // This also guards against two Dequeuers (which are only on internal contexts during critical regions) from touching freed memory in
                // locating their segment.  Enqueues are guarded with a different mechanism.
                //
                m_deletionSafePoint.InvokeAtNextSafePoint(reinterpret_cast<SafePointInvocation::InvocationFunction>(&Segment::StaticDelete),
                                                          reinterpret_cast<void *>(this),
                                                          m_pScheduler);
            }

            /// <summary>
            ///      Marks how many dereferences must happen before the segment can delete itself.
            /// </summary>
            void SetDeletionReferences(unsigned int count)
            {
                if ((static_cast<unsigned int>(InterlockedExchangeAdd(reinterpret_cast<volatile long *>(&m_refs), count)) + count) == 0)
                    Expire();
            }

            /// <summary>
            ///     Safe point routine to delete a segment.
            /// </summary>
            static void StaticDelete(Segment *pSegment)
            {
                delete pSegment;
            }

            // The scheduler to which the segment belongs.
            SchedulerBase *m_pScheduler;

            // The affinity of the segment.
            QuickBitSet m_affinitySet;

            // The queue of objects within the segment.
            T* volatile * m_pQueue;

            // The base index of the segment.
            unsigned int m_baseIdx;

            // The number of references remaining on the segment.
            volatile unsigned int m_refs;

            // The next segment within the mailbox.
            Segment * m_pNext;

            // The safe point at which the segment will be deleted.
            SafePointInvocation m_deletionSafePoint;
        };

    public:

        /// <summary>
        ///     An opaque handle to a slot of a mailbox.  When an object is enqueued in the mailbox, a slot is returned.  If the item
        ///     is placed on another list, the slot must be claimed before the object is utilized.
        /// </summary>
        class Slot
        {
        public:

            Slot() : m_pSegment(NULL), m_relativeIdx(0)
            {
            }

            Slot(const Slot& src) : m_pSegment(src.m_pSegment), m_relativeIdx(src.m_relativeIdx)
            {
            }

            Slot& operator=(const Slot& rhs)
            {
                m_pSegment = rhs.m_pSegment;
                m_relativeIdx = rhs.m_relativeIdx;

                return *this;
            }

            bool IsEmpty() const
            {
                return m_pSegment == NULL;
            }

            /// <summary>
            ///     Claims an object from a slot in an out-of-order and thread-safe manner.  If true is returned, this indicates that
            ///     the caller has exclusive ownership of the object within that slot.
            /// </summary>
            bool Claim(T ** pClaimedObject = nullptr)
            {
                T* pObject = m_pSegment->m_pQueue[m_relativeIdx];
                ASSERT(pObject != NULL);

                if (pObject != reinterpret_cast<T*>(SLOT_PENDING_EXPIRY))
                {
                    T* pXchgObject = reinterpret_cast<T*>(
                        InterlockedExchangePointer(reinterpret_cast<void * volatile *>(m_pSegment->m_pQueue + m_relativeIdx),
                        reinterpret_cast<void *>(SLOT_PENDING_EXPIRY)));

                    if (pXchgObject == pObject)
                    {
#if _UMSTRACE && _CHORETRACK
                        CCMSTRACE(MTRACE_EVT_SLOTCLAIM, pObject);
#endif // _UMSTRACE && _CHORETRACK

                        if (pClaimedObject)
                            *pClaimedObject = pObject;
                        return true;
                    }

                }

#if _UMSTRACE && _CHORETRACK
                CCMSTRACE(MTRACE_EVT_SLOTCLAIMFAILED, pObject);
#endif // _UMSTRACE && _CHORETRACK

                m_pSegment->Dereference();

                return false;
            }

            bool DeferToAffineSearchers() const
            {
                InternalContextBase * pContext = static_cast<InternalContextBase *>(SchedulerBase::FastCurrentContext());
                return (m_pSegment->m_pScheduler->HasSearchers(m_pSegment->m_affinitySet) &&
                        !m_pSegment->m_affinitySet.IsSet(GetProcessorMaskId(pContext)));
            }

        private:

            friend class Mailbox;

            Slot(Segment *pSegment, unsigned int relativeIdx) : m_pSegment(pSegment), m_relativeIdx(relativeIdx)
            {
            }

            Segment *m_pSegment;
            unsigned int m_relativeIdx;

        };

        /// <summary>
        ///     Constructs a new mailbox with the specified segment size.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler to which this mailbox belongs.
        /// </param>
        /// <param name="fDeferAlloc">
        ///     Indicates whether or not to defer allocation of the first segment until the first enqueue.
        /// </param>
        /// <param name="segmentSize">
        ///     The size of the mailbox.  Note that the mailbox size is fixed once constructed.
        /// </param>

        Mailbox(SchedulerBase *pScheduler, const QuickBitSet&, bool fDeferAlloc = false, unsigned int segmentSize = s_segmentSize)
            : m_pScheduler(pScheduler)
            , m_segmentSize(segmentSize)
            , m_pTailSegment(NULL)
            , m_pHeadSegment(NULL)
            , m_head(0)
            , m_tail(0)
        {
            ASSERT((segmentSize & (segmentSize - 1)) == 0);

            Initialize(m_affinitySet);

            if (!fDeferAlloc)
            {
                m_pTailSegment = _concrt_new Segment(m_pScheduler, m_affinitySet, segmentSize, 0);
                m_pHeadSegment = m_pTailSegment;
            }
        }

        /// <summary>
        ///     Destroys a mailbox.
        /// </summary>
        ~Mailbox()
        {
            Segment *pSegment = m_pHeadSegment;
            while (pSegment != NULL)
            {
                Segment *pNextSegment = pSegment->m_pNext;

                if (pSegment != m_pTailSegment)
                    pSegment->SetDeletionReferences(m_segmentSize);
                else
                {
                    //
                    // How many items are in this segment?  That is how many must dereference the segment in order for its memory to be freed.
                    // Set this number.  Note that this should *ONLY* be for the tail segment.
                    //
                    unsigned int numElements = m_tail - pSegment->m_baseIdx;
                    ASSERT(numElements <= m_segmentSize);

                    pSegment->SetDeletionReferences(numElements);
                }

                pSegment = pNextSegment;
            }

        }

        /// <summary>
        ///     Initializes key fields of the mailbox.
        /// </summary>
        void Initialize(const QuickBitSet& bitSet)
        {
            m_affinitySet = bitSet;
            if (m_pHeadSegment)
                m_pHeadSegment->m_affinitySet = bitSet;
        }

        /// <summary>
        ///     Enqueues an object onto the mailbox and returns a pointer to the slot if the enqueue is successful.  Note that
        ///     the Slot object may only be used in methods on the mailbox.
        /// </summary>
        /// <param name="pObject">
        ///     The object to enqueue.
        /// </param>
        Slot Enqueue(T* pObject)
        {
            //
            // Complete the pushes in order to avoid LocateMailboxSegment touching an invalid segment when an enqueue crosses a boundary in conjunction
            // with a dequeue/claim -> free.
            //
            m_enqueueLock._Acquire();

            Segment *pSegment = LocateMailboxSegment(m_tail, true);

            unsigned int relativeIdx = m_tail - pSegment->m_baseIdx;
            pSegment->m_pQueue[relativeIdx] = pObject;

            // The Dequeue function will calculate the number of available messages based on m_tail.
            // This memory fence will flush new m_tail to Dequeue. Be attention that there is no fence in the last lock release function.
            // If the write to m_tail is observed by the Dequeue, all write operations before this point must be observed by Dequeue as well.
            _InterlockedIncrement(reinterpret_cast<volatile long *>(&m_tail));

            m_enqueueLock._Release();
            return Slot(pSegment, relativeIdx);
        }

        /// <summary>
        ///     Dequeues an object from the mailbox.
        /// </summary>
        /// <param name="pDequeuedElement">
        ///     If the dequeue is successful, the dequeued element will be placed here.
        /// </param>
        ///
        bool Dequeue(T **pDequeuedElement)
        {
            //
            // Keep dequeueing until we either get something or the queue is empty.  We may dequeue a slot pending expiry.
            //
            for(;;)
            {
                unsigned int head = m_head;
                for (;;)
                {
                    if (head == m_tail)
                        return false;

                    unsigned int xchgHead = static_cast<unsigned int> (
                        InterlockedCompareExchange(reinterpret_cast<volatile long *>(&m_head), head + 1, head)
                        );

                    if (xchgHead == head)
                        break;

                    head = xchgHead;
                }

                Segment *pSegment = LocateMailboxSegment(head, false);

                //
                // Check if we need to update the head pointers if we have gone past the head segment. We will only remove segments from the queue if
                // all their slots have been claimed. This is so that we do not inadvertently remove a segment a different thread in this routine
                // is trying to to locate. Segments can only be located if they are between head and tail. The update must handle multiple dequeues
                // happening simultaneously and trying to update this simultaneously!
                //
                // There is no ABA here because segments are freed at a safe point and the calling thread is always an internal context which participates
                // in this mechanism.
                //
                if (pSegment != m_pHeadSegment)
                {
                    // Since the head is not moved until all slots are claimed, this segment's base index cannot be less than that of the head segment.
                    // i.e. this segment must still be in the set [head, tail].
                    CONCRT_COREASSERT(pSegment->m_baseIdx >= m_pHeadSegment->m_baseIdx);

                    Segment *pHeadSegment = m_pHeadSegment;
                    Segment *pReadSegment = pHeadSegment;

                    // Travel forward from the head as long as we continue to find segments that have had all slots claimed.
                    for(;;)
                    {
                        while (pReadSegment->AllSlotsClaimed(m_segmentSize))
                        {
                            pReadSegment = pReadSegment->m_pNext;
                        }

                        // If we've found a chain of segments (or a single segment) that has all slots claimed, try to change the head
                        if (pReadSegment->m_baseIdx > pHeadSegment->m_baseIdx)
                        {
                            Segment *pXchgSegment = reinterpret_cast<Segment *>(
                                InterlockedCompareExchangePointer(reinterpret_cast<void * volatile *>(&m_pHeadSegment),
                                                                  reinterpret_cast<void *>(pReadSegment),
                                                                  reinterpret_cast<void *>(pHeadSegment))
                                );

                            if (pXchgSegment == pHeadSegment)
                            {
                                //
                                // The person who removes a segment (or a series of segments) from the list via the head is responsible for
                                // setting their deletion references so that they properly delete!  The segments in the sublist described by
                                // the half open range [pXchgSegment, pSegment) must be set.
                                //
                                Segment *pDelRef = pXchgSegment;
                                while (pDelRef != pReadSegment)
                                {
                                    pDelRef->SetDeletionReferences(m_segmentSize);
                                    pDelRef = pDelRef->m_pNext;
                                }
                                break;
                            }

                            pHeadSegment = pReadSegment = pXchgSegment;
                        }
                        else
                        {
                            break;
                        }
                    }

                    CONCRT_COREASSERT(m_pHeadSegment != NULL);
                    CONCRT_COREASSERT(pSegment->m_baseIdx >= m_pHeadSegment->m_baseIdx);
                }

                unsigned int relativeIdx = head - pSegment->m_baseIdx;

                // If the slot we get has not been claimed by anyone else,
                // we will claim it and dequeue it, otherwise, keep searching next.
                if (Slot(pSegment, relativeIdx).Claim(pDequeuedElement))
                    return true;
            }
        }

        /// <summary>
        ///     Returns whether the mailbox is empty or not.
        /// </summary>
        bool IsEmpty() const
        {
            return (m_head == m_tail);
        }

    private:

        /// <summary>
        ///     Expires a slot.
        /// </summary>
        void ExpireSlot(Segment *pSegment, unsigned int relativeIdx)
        {
            pSegment->Dereference();
        }

        Segment *Grow(Segment *pPreviousSegment)
        {
            // This "Grow" function is always protected by the lock in "enqueue".

            Segment *pNewSegment = _concrt_new Segment(m_pScheduler, m_affinitySet, m_segmentSize, pPreviousSegment->m_baseIdx + m_segmentSize);
            m_pTailSegment = pNewSegment;
            return pPreviousSegment->m_pNext = pNewSegment;
        }

        /// <summary>
        ///     Performs one time demand initialization of the mailbox if the segments were set to be allocated on demand.
        /// </summary>
        void DemandInitialize()
        {
            if (m_pTailSegment == NULL)
            {
                Segment *pXchgSegment = reinterpret_cast<Segment *>(
                    InterlockedCompareExchangePointer(reinterpret_cast<void * volatile *>(&m_pTailSegment),
                                                      reinterpret_cast<Segment *>(FIELD_RESERVED),
                                                      NULL)
                    );

                if (pXchgSegment == NULL)
                {
                    Segment *pNewSegment = _concrt_new Segment(m_pScheduler, m_affinitySet, m_segmentSize, 0);
                    m_pTailSegment = pNewSegment;
                    // sfence
                    m_pHeadSegment = pNewSegment;
                }
            }

            if (m_pHeadSegment == NULL)
            {
                _SpinWaitBackoffNone spinWait(_Sleep0);
                while(m_pHeadSegment == NULL)
                {
                    spinWait._SpinOnce();
                }
            }
        }

        /// <summary>
        ///     Locates the appropriate mailbox segment for the specified absolute index.  This is only utilized during enqueue and dequeue and *NOT* during
        ///     an arbitrary slot claim!
        /// </summary>
        Segment *LocateMailboxSegment(unsigned int absoluteIdx, bool fStartTail)
        {
            if (m_pHeadSegment == NULL)
                DemandInitialize();

            // lfence

            Segment *pSegment = fStartTail ? m_pTailSegment : m_pHeadSegment;
            ASSERT(absoluteIdx >= pSegment->m_baseIdx);

            Segment *pPreviousSegment = pSegment;
            while (pSegment && absoluteIdx >= pSegment->m_baseIdx + m_segmentSize)
            {
                pSegment = pSegment->m_pNext;
                if (pSegment == NULL)
                {
                    ASSERT(fStartTail); // Only enqueue will "Grow" the queue.
                    pSegment = Grow(pPreviousSegment);
                }
                pPreviousSegment = pSegment;
            }

            return pSegment;
        }

        //
        // Determines the size of a mailbox segment.  Every mailbox pre-allocates a single segment.  The size should be large enough to amortize heap
        // allocation but small enough not to be prohibitively waste memory.
        //
        // This value should be a power of two.
        //
        static const unsigned int s_segmentSize = 64;

        // The scheduler to which the mailbox belongs
        SchedulerBase *m_pScheduler;

        // The mailbox's affinity set
        QuickBitSet m_affinitySet;

        // The size of segments for this mailbox.
        unsigned int m_segmentSize;

        // The head and tail segments for the mailbox.  These are within [m_head, m_tail].
        Segment * volatile m_pTailSegment;
        Segment * volatile m_pHeadSegment;

        // The current head pointer
        volatile unsigned int m_head;

        // The current tail pointer
        volatile unsigned int m_tail;

        // Protect enqueue function, which should only accept one message for a time.
        _NonReentrantLock m_enqueueLock;
    };
} // namespace details
} // namespace Concurrency
