// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ExternalContextBase.h
//
// Header file containing the metaphor for an external execution context.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Provides a storage area for external contexts bound to this scheduler or alien threads (threads not
    ///     associated with any scheduler or those associated with scheduler other than this one) where they can put
    ///     the statistical data necessary to track the rate of work.
    /// </summary>

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class ExternalStatistics
    {
    public:
        //
        // Public methods
        //

        /// <summary>
        ///     Constructs the statistics object for an external context or alien thread.
        /// </summary>
        ExternalStatistics() : m_enqueuedTaskCounter(0), m_dequeuedTaskCounter(0), m_enqueuedTaskCheckpoint(0), m_dequeuedTaskCheckpoint(0), m_fIsActive(true)
        {
        }

        /// <summary>
        ///     Increments the count of work coming in.
        /// </summary>
        void IncrementEnqueuedTaskCounter()
        {
            m_enqueuedTaskCounter++;
        }

        /// <summary>
        ///     Increments the count of work being done.
        /// </summary>
        void IncrementDequeuedTaskCounter()
        {
            m_dequeuedTaskCounter++;
        }

        /// <summary>
        ///     Increments the count of work being done.
        /// </summary>
        void IncrementDequeuedTaskCounter(unsigned int count)
        {
            m_dequeuedTaskCounter += count;
        }

        /// <summary>
        ///     Resets the count of work coming in.
        /// </summary>
        /// <remarks>
        ///     This function will reset the state so that the next time it is called, it reports only the
        ///     units of work that came in since the last time. One obvious solution is to reset the
        ///     counter, but that introduces a race with a thread that tries to increment. Instead,
        ///     we update the trailing counter to match the current count. This way the difference
        ///     between the two is always the number of work coming in. By keeping these numbers unsigned
        ///     we make use of "modulo 2" behavior of unsigned ints and avoid overflow problems.
        ///
        ///     NOTE: There is a highly rare condition present in this code. If, for some reason,
        //      statistics calls were infrequent enough that UINT_MAX units of work were enqueued
        ///     between two calls we will wrap around and consequently think that no work came in at all.
        /// </remarks>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetEnqueuedTaskCount()
        {
            unsigned int currentValue = m_enqueuedTaskCounter;
            unsigned int retVal = currentValue - m_enqueuedTaskCheckpoint;

            // Update the checkpoint value with the current value
            m_enqueuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Resets the count of work being done.
        /// </summary>
        /// <remarks>
        ///     Look at remarks for GetEnqueuedTaskCount.
        /// </remarks>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetDequeuedTaskCount()
        {
            unsigned int currentValue = m_dequeuedTaskCounter;
            unsigned int retVal = currentValue - m_dequeuedTaskCheckpoint;

            // Update the checkpoint value with the current value
            m_dequeuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Marks this statistics as not active anymore. This means that external context
        ///     has gone away and it will no longer update the statistical information. However,
        ///     we can't remove statistics right away because they might not have been collected yet.
        ///     So, we mark it as inactive and we wait for the next collection to take place before
        ///     permanently retiring this statistics.
        /// </summary>
        void MarkInactive()
        {
            m_fIsActive = FALSE;
        }

        /// <summary>
        ///     Checks whether this statistics class expects any new updates.
        /// </summary>
        /// <returns>
        ///     True if statistics is still active.
        /// </returns>
        bool IsActive()
        {
            // By the memory ordering rules the only way that m_fIsActive would be marked as false
            // is if external context is being destroyed, which means there is no work coming in or
            // out of this external context. The task counts are final and there is no race between
            // task counts and active bit.
            return (m_fIsActive || (m_enqueuedTaskCounter != m_enqueuedTaskCheckpoint) || (m_dequeuedTaskCounter != m_dequeuedTaskCheckpoint));
        }

        // A field that is necessary to store the statistics data structure in a ListArray<ExternalStatistics>
        int m_listArrayIndex;

    private:
        //
        // Private data
        //

        template <class T> friend class ListArray;

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

        // Statistics data counters
        unsigned int m_enqueuedTaskCounter;
        unsigned int m_dequeuedTaskCounter;

        // Statistics data checkpoints
        unsigned int m_enqueuedTaskCheckpoint;
        unsigned int m_dequeuedTaskCheckpoint;

        // Whether this statistics is actively worked on
        volatile BOOL m_fIsActive;
    };
#pragma warning(pop)

    /// <summary>
    /// Implements the base class for ConcRT external contexts.
    /// </summary>
    class ExternalContextBase : public ContextBase
    {
    public:

        //
        // Public methods
        //

        /// <summary>
        ///     Constructs an external context.
        /// </summary>
        ExternalContextBase(SchedulerBase *pScheduler, bool explicitAttach);

        /// <summary>
        ///     Destroys an external context.
        /// </summary>
        virtual ~ExternalContextBase();

        /// <summary>
        ///     Causes the external context to block. Since external contexts do not execute on virtual processors,
        ///     the context does not switch to another one. Instead, it stops executing until it is unblocked.
        /// </summary>
        virtual void Block();

        /// <summary>
        ///     Unblocks the external context causing it to start running.
        /// </summary>
        virtual void Unblock();

        /// <summary>
        ///     Since there is no underlying virtual processor, the yield operation is a no-op for external contexts.
        /// </summary>
        virtual void Yield();

        /// <summary>
        ///     Since there is no underlying virtual processor, the yield operation is a no-op for external contexts.
        /// </summary>
        virtual void SpinYield()
        {
            Yield();
        }

        /// <summary>
        ///     See comments for Concurrency::Context::Oversubscribe.
        /// </summary>
        virtual void Oversubscribe(bool beginOversubscription);

        /// <summary>
        ///     Allocates a block of memory of the size specified.
        /// </summary>
        /// <param name="numBytes">
        ///     Number of bytes to allocate.
        /// </param>
        /// <returns>
        ///     A pointer to newly allocated memory.
        /// </returns>
        virtual void* Alloc(size_t numBytes);

        /// <summary>
        ///     Frees a block of memory previously allocated by the Alloc API.
        /// </summary>
        /// <param name="pAllocation">
        ///     A pointer to an allocation previously allocated by Alloc.
        /// </param>
        virtual void Free(void* pAllocation);

        /// <summary>
        ///     Tells whether the context was explicitly attached to the scheduler at the time it was created
        /// </summary>
        bool WasExplicitlyAttached() const { return m_fExplicitlyAttached; }

        /// <summary>
        ///     Returns an identifier to the virtual processor the context is currently executing on, if any.
        /// </summary>
        virtual unsigned int GetVirtualProcessorId() const { return UINT_MAX; }

        /// <summary>
        ///     Initializes fields that need re-initialization when an external context is reused. This is called
        ///     in the constructor and when an external context is taken off the idle pool for reuse.
        /// </summary>
        void PrepareForUse(bool explicitAttach);

        /// <summary>
        ///     Prepares an external context for the idle pool by releasing some resources.
        /// </summary>
        void RemoveFromUse();

        /// <summary>
        ///     Returns a handle to the underlying thread.
        /// </summary>
        HANDLE GetPhysicalContext() { return m_hPhysicalContext; }

        /// <summary>
        ///     Returns a pointer to the suballocator for this external context. Note that the RM call to get an
        ///     allocator can return NULL, since the RM only hands out a fixed number of allocators for external
        ///     contexts.
        /// </summary>
        SubAllocator* GetCurrentSubAllocator()
        {
            if (m_pSubAllocator == NULL)
            {
                m_pSubAllocator = SchedulerBase::GetSubAllocator(true);
            }
            return m_pSubAllocator;
        }

        /// <summary>
        ///     Increments the count of work coming in.
        /// </summary>
        void IncrementEnqueuedTaskCounter()
        {
            m_pStats->IncrementEnqueuedTaskCounter();
        }

        /// <summary>
        ///     Increments the count of work being done.
        /// </summary>
        void IncrementDequeuedTaskCounter()
        {
            m_pStats->IncrementDequeuedTaskCounter();
        }

        /// <summary>
        ///     Increments the count of work being done.
        /// </summary>
        void IncrementDequeuedTaskCounter(unsigned int count)
        {
            m_pStats->IncrementDequeuedTaskCounter(count);
        }

        /// <summary>
        ///     Orphan the statistics and let it know there will be no more updates.
        /// </summary>
        /// <returns>
        ///     The statistics that were attached to this external context.
        /// </returns>
        ExternalStatistics * DetachStatistics()
        {
            ExternalStatistics * externalStatistics = m_pStats;
            m_pStats = NULL;
            externalStatistics->MarkInactive();

            return externalStatistics;
        }

        /// <summary>
        ///     Determines whether or not the context is synchronously blocked at this given time.
        /// </summary>
        /// <returns>
        ///     Whether context is in synchronous block state.
        /// </returns>
        virtual bool IsSynchronouslyBlocked() const
        {
            return (m_contextSwitchingFence == 1);
        }

#if _DEBUG
        // _DEBUG helper
        DWORD GetThreadId() const { return m_threadId; }
#endif

    private:
        friend class SchedulerBase;
        template<class T> friend void _InternalDeleteHelper(T*);

        //
        // Private data
        //

        // Specifies whether the context was created as a result of an explicit or implicit attach.
        bool m_fExplicitlyAttached;

        // Statistical information for this external context.
        ExternalStatistics * m_pStats;

        // A pointer to the suballocator for this context.
        SubAllocator * m_pSubAllocator;

        // Handle to the underlying thread.
        HANDLE m_hPhysicalContext;

        // Handle to the event used for blocking.
        HANDLE m_hBlock;

        // Wait handle for thread exit event (used on XP)
        HANDLE m_hWaitHandle;

        //
        // Private methods
        //

        /// <summary>
        ///     Performs cleanup of the external context
        /// </summary>
        void Cleanup();

        /// <summary>
        ///     Callback to indicate the exit of one of the external threads.
        /// </summary>
        static void CALLBACK ImplicitDetachHandler(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WAIT waiter, TP_WAIT_RESULT waitResult);

        /// <summary>
        ///     Same callback function as ImplicitDetachHandler but used on XP.
        /// </summary>
        static void CALLBACK ImplicitDetachHandlerXP(PVOID parameter, BOOLEAN is_timeout);

        /// <summary>
        ///     Returns the type of context
        /// </summary>
        virtual ContextKind GetContextKind() const { return ExternalContext; }
    };
} // namespace details
} // namespace Concurrency
