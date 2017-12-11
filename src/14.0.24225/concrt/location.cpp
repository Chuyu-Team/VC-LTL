// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// location.cpp
//
// Implementation file of the metaphor for a location.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
    typedef BOOL (WINAPI *PFN_GetNumaNodeNumberFromHandle)(HANDLE, PUSHORT);

    /// <summary>
    ///     Constructs a specific location.
    /// </summary>
    _Use_decl_annotations_
    location::location(_Type _LocationType, unsigned int _Id, unsigned int _BindingId, void *_PBinding) :
        _M_type(_LocationType),
        _M_reserved(0),
        _M_bindingId(_BindingId),
        _M_pBinding(_PBinding)
    {
        //
        // Guarantee order per union.
        //
        _M_ptr = NULL;
        _M_id = _Id;
    }

    /// <summary>
    ///     Returns a location which represents a given NUMA node.
    /// </summary>
    /// <param name="_NumaNodeNumber">
    ///     The NUMA node number to construct a location for.
    /// </param>
    /// <returns>
    ///     A location representing the NUMA node specified by the <paramref name="_NumaNodeNumber"/> parameter.
    /// </returns>
    location location::from_numa_node(unsigned short _NumaNodeNumber)
    {
        ULONG highestNodeNumber = platform::__GetNumaHighestNodeNumber();

        if (_NumaNodeNumber > highestNodeNumber)
        {
            throw invalid_operation();
        }

        return location(location::_NumaNode, _NumaNodeNumber, 0, NULL);
    }

    /// <summary>
    ///     Determines whether two locations have an intersection.  This is a fast intersection which avoids certain checks by knowing that
    ///     the *this* pointer is a virtual processor location for a validly bound virtual processor.
    /// </summary>
    /// <param name="_Rhs">
    ///     The location to intersect with this.
    /// </param>
    /// <returns>
    ///     An indication as to whether the two locations intersect.
    /// </returns>
    bool location::_FastVPIntersects(const location& _Rhs) const
    {
        switch(_Rhs._M_type)
        {
            case _ExecutionResource:
                return _M_id == _Rhs._M_id;

            case _NumaNode:
                return _As<VirtualProcessor>()->GetOwningNode()->GetNumaNodeNumber() == (DWORD)_Rhs._M_id;

            case _SchedulingNode:
                return static_cast<unsigned int>(_As<VirtualProcessor>()->GetOwningNode()->Id()) == _Rhs._M_id;

            case _System:
                return true;

        }

        return false;
    }

    /// <summary>
    ///     Determines whether two locations have an intersection.  This is a fast intersection which avoids certain checks by knowing that
    ///     the *this* pointer is a node for a validly bound node.
    /// </summary>
    /// <param name="_Rhs">
    ///     The location to intersect with this.
    /// </param>
    /// <returns>
    ///     An indication as to whether the two locations intersect.
    /// </returns>
    bool location::_FastNodeIntersects(const location& _Rhs) const
    {
        switch(_Rhs._M_type)
        {
            case _ExecutionResource:
                return _As<SchedulingNode>()->ContainsResourceId(_Rhs._M_id);

            case _NumaNode:
                return _As<SchedulingNode>()->GetNumaNodeNumber() == (DWORD)_Rhs._M_id;

            case _SchedulingNode:
                return _M_id == _Rhs._M_id;

            case _System:
                return true;
        }

        return false;
    }

    /// <summary>
    ///     Returns a location representing the most specific place the calling thread is executing upon.
    /// </summary>
    location location::current()
    {
        location current;

        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        if (pCurrentContext != NULL && !pCurrentContext->IsExternal())
        {
            InternalContextBase *pCtx = static_cast<InternalContextBase *>(pCurrentContext);

            pCtx->EnterCriticalRegion();
            VirtualProcessor *pVProc = pCtx->m_pVirtualProcessor;
            current = location(location::_ExecutionResource, pVProc->GetExecutionResourceId(), pVProc->GetOwningNode()->GetScheduler()->Id(), pVProc);
            pCtx->ExitCriticalRegion();
        }

        return current;
    }

    location __cdecl location::_Current_node()
    {
        location current;

        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        if (pCurrentContext != NULL && !pCurrentContext->IsExternal())
        {
            InternalContextBase *pCtx = static_cast<InternalContextBase *>(pCurrentContext);

            pCtx->EnterCriticalRegion();
            VirtualProcessor *pVProc = pCtx->m_pVirtualProcessor;
            SchedulingNode *pNode = pVProc->GetOwningNode();
            current = location(location::_SchedulingNode, pNode->Id(), pNode->GetScheduler()->Id(), pNode);
            pCtx->ExitCriticalRegion();
        }

        return current;
    }
} // namespace Concurrency
