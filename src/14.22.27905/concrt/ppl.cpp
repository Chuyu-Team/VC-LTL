// ==++==
//
//   Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ppl.cpp
//
// Utility routines for use in PPL.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <ppl.h>

// Candidate primes... these must be sorted.  Note that this isn't a complete list of primes, and is
// capped at reasonable level (e.g., a size of 281 means you expect to have about that many threads).
static const unsigned int g_getCombinableCandidatePrimes[] = {
    11, 19, 37, 67, 73, 151, 281
};

namespace Concurrency
{

namespace details
{
    _CONCRTIMP size_t _GetCombinableSize()
    {
        unsigned int size = CurrentScheduler::GetNumberOfVirtualProcessors();
        if (size == static_cast<unsigned int>(-1))
        {
            // Choose a reasonable arbitrary (prime) size when ConcRT isn't around.
            return 11;
        }

        // ConcRT gave us the number of vprocs.
        size *= 2;
        for (unsigned int candidatePrime : g_getCombinableCandidatePrimes)
        {
            if (size < candidatePrime)
            {
                return candidatePrime;
            }
        }

        return g_getCombinableCandidatePrimes[sizeof(g_getCombinableCandidatePrimes)
            / sizeof(g_getCombinableCandidatePrimes[0]) - 1];
    }
} // namespace details

/// <summary>
///     Returns an indication of whether the task group which is currently executing inline on the current context
///     is in the midst of an active cancellation (or will be shortly).  Note that if there is no task group currently
///     executing inline on the current context, false will be returned.
/// </summary>
_CONCRTIMP bool is_current_task_group_canceling()
{
    return Context::IsCurrentTaskCollectionCanceling();
}

} // namespace concurrency
