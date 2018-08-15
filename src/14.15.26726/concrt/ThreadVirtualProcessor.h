// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadVirtualProcessor.h
//
// Header file containing the metaphor for a thread based virtual processor
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    class ThreadVirtualProcessor : public VirtualProcessor
    {
    public:

        /// <summary>
        ///     Constructor
        /// </summary>
        ThreadVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

        /// <summary>
        ///     Destructor
        /// </summary>
        virtual ~ThreadVirtualProcessor();
    };
} // namespace details
} // namespace Concurrency
