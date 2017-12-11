// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadVirtualProcessor.cpp
//
// Source file containing the ThreadVirtualProcessor implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructor
    /// </summary>
    ThreadVirtualProcessor::ThreadVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot)
    {
        VirtualProcessor::Initialize(pOwningNode, pOwningRoot);
    }

    /// <summary>
    ///     Destructor
    /// </summary>
    ThreadVirtualProcessor::~ThreadVirtualProcessor()
    {
    }
} // namespace details
} // namespace Concurrency
