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

namespace Concurrency
{

namespace details
{
    _CONCRTIMP size_t _GetCombinableSize()
    {
        // Candidate primes... these must be sorted.  Note that this isn't a complete list of primes, and is
        // capped at reasonable level (e.g., a size of 281 means you expect to have about that many threads).
        static const unsigned long candidatePrimes[] = {
            11, 19, 37, 67, 73, 151, 281
        };
        const int numCandidatePrimes = sizeof candidatePrimes / sizeof candidatePrimes[0];

        size_t size = CurrentScheduler::GetNumberOfVirtualProcessors();
        if (size != (size_t)(-1))
        {
            // ConcRT gave us the number of vprocs.
            size *= 2;
            int index;
            for (index = 0; index < numCandidatePrimes; ++index)
            {
                if (size < candidatePrimes[index])
                    break;
            }

            if (index < numCandidatePrimes)
                size = candidatePrimes[index];
            else
                size = candidatePrimes[numCandidatePrimes - 1];
        }
        else
        {
            // Choose a reasonable arbitrary (prime) size when ConcRT isn't around.
            size = 11;
        }

        return size;
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
