// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadVirtualProcessor.h
//
// Header file containing the metaphor for a UMS thread based virtual processor
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    class UMSThreadVirtualProcessor : public VirtualProcessor
    {
    public:

        /// <summary>
        ///     Construct a UMS virtual processor
        /// </summary>
        UMSThreadVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

        /// <summary>
        ///     Destroy the UMS virtual processor
        /// </summary>
        virtual ~UMSThreadVirtualProcessor();

        /// <summary>
        ///     Start a worker context executing on this.virtual processor.
        /// </summary>
        virtual bool StartupWorkerContext(ScheduleGroupSegmentBase* pSegment, InternalContextBase *pContext);

        /// <summary>
        ///     Makes a virtual processor available for scheduling work.  This can only be called from the scheduling
        ///     context.
        /// </summary>
        void MakeAvailableFromSchedulingContext();

        /// <summary>
        ///     Returns true if it is safe for a context to enter the critical region on this virtual processor.
        ///     If there is already a context in the critical region on this vproc, the routine returns false.
        /// </summary>
        bool CanCriticalEnter()
        {
            return (m_pCriticalContext == NULL);
        }

        /// <summary>
        ///     Notification that a critically blocked context has come off the completion list.
        /// </summary>
        void CriticalNotify();

        /// <summary>
        ///     Returns the default destination of a SwitchTo(NULL).  UMS schedulers trigger a return to primary.
        /// </summary>
        virtual IExecutionContext *GetDefaultDestination()
        {
            return (IExecutionContext *)m_pSchedulingContext;
        }

        /// <summary>
        ///     Attempts to wake the virtual processor due to a notification coming in that is critical to progress on
        ///     the virtual processor (e.g.: a critically blocked context coming back on the completion list from
        ///     another virtual processor).
        /// </summary>
        void AttemptWake();

        /// <summary>
        ///     Stub called in SFW before we search for runnable contexts.
        /// </summary>
        /// <returns>
        ///     A context which should be run.
        /// </returns>
        virtual InternalContextBase *PreRunnableSearch();

    protected:

        /// <summary>
        ///     Affinitizes an internal context to the virtual processor.
        /// </summary>
        /// <param name="pContext">
        ///     The internal context to affinitize.
        /// </param>
        virtual void Affinitize(InternalContextBase *pContext);

        /// <summary>
        ///     Returns a type-cast of pContext to an InternalContextBase or NULL if it is not.
        /// </summary>
        virtual InternalContextBase *ToInternalContext(IExecutionContext *pContext);

        /// <summary>
        ///     Initializes the virtual processor. This API is called by the constructor, and when a virtual processor is to
        ///     be re-initialized, when it is pulled of the free pool in the list array.
        /// </summary>
        /// <param name="pOwningNode">
        ///     The owning schedule node for this virtual processor
        /// </param>
        /// <param name="pOwningRoot">
        ///     The owning IVirtualProcessorRoot
        /// </param>
        virtual void Initialize(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

    private:

        friend class UMSSchedulingContext;

        // Indication that a critical context for this virtual processor is ready to run
        volatile LONG m_fCriticalIsReady;

        // The Scheduling context for this virtual processor.  Note that we *NEVER* deactivate the scheduling context.
        UMSSchedulingContext *m_pSchedulingContext;

        // The critically blocked context.
        UMSThreadInternalContext *m_pCriticalContext;

        // The startup scheduling group segment.  This may only be touched when m_pExecutingContext is NULL.
        ScheduleGroupSegmentBase *m_pStartingSegment;
    };
} // namespace details
} // namespace Concurrency
