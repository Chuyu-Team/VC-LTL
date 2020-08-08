// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// RealizedChore.h
//
// Header file containing the realized chore type declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     The class RealizedChore is used to implement light-weight tasks and Agents.
    /// </summary>

    class RealizedChore : public _Chore
    {

    public:

        /// <summary>
        ///     Constructor.
        /// </summary>
        RealizedChore(TaskProc pFunction, void* pParameters)
        {
            Initialize(pFunction, pParameters);
        }

        /// <summary>
        ///     Initializes a realized chore, on construction and reuse.
        /// </summary>
        void Initialize(TaskProc pFunction, void* pParameters)
        {
            m_pFunction = pFunction;
            m_pParameters = pParameters;
            m_pNext = NULL;
        }

        /// <summary>
        ///     Method that executes the realized chore.
        /// </summary>
        __declspec(noinline)
        void Invoke();

    private:
        template <typename T> friend class SQueue;
        template <class T> friend class LockFreeStack;

        // Parameter to the chore procedure.
        void *m_pParameters;

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
        union
        {
            // Next pointer for the locked runnables queue.
            RealizedChore *m_pNext;

            // List entry for lock free slist (free pool)
            SLIST_ENTRY m_slNext;
        };
#pragma warning(pop)
    };
} // namespace details
} // namespace Concurrency
