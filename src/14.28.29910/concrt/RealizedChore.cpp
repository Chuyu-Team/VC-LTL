// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// RealizedChore.cpp
//
// Miscellaneous implementations of things related to realized chores
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
    /// <summary>
    ///     Method that executes the realized chore.  Not inline-able because debugger needs to
    ///     locate executing realized chores by looking for this method's signature in the call
    ///     frame.
    /// </summary>
    __declspec(noinline)
    void RealizedChore::Invoke()
    {
        m_pFunction(m_pParameters);
    }
}
