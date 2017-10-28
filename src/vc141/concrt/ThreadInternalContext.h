// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadInternalContext.h
//
// Header file containing the metaphor for an thread based internal execution context/stack.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{

    class ThreadInternalContext : public InternalContextBase
    {
    public:
        //
        // Public Methods
        //

        /// <summary>
        ///     Construct an internal thread based context.
        /// </summary>
        ThreadInternalContext(SchedulerBase *pScheduler) :
            InternalContextBase(pScheduler)
        {
        }

        /// <summary>
        ///     Destroys an internal thread based context.
        /// </summary>
        virtual ~ThreadInternalContext()
        {
        }

        /// <summary>
        ///     Returns the type of context
        /// </summary>
        virtual ContextKind GetContextKind() const { return ThreadContext; }

    private:
        friend class ThreadScheduler;
    };
} // namespace details
} // namespace Concurrency
