// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// concrtinternal.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//
// If this is defined to 1, memory logging of a series of things related to UMS will be preserved in contexts and virtual
// processors.  This *SERIOUSLY AFFECTS* performance.  It is a debugging only facility.
//
#define _UMSTRACE 0

//
// If this is defined to 1, individual chores will be logged out to the memory trace buffer utilized by UMSTRACE.
//
#define _CHORETRACK 0

//
// If this is defined to 1, ConcRT will log out points at which UMS blocks into the UMSTRACE log as well as assert the validity of the blocking
// type given the address the thread blocked at.  Note that this only works on Win64.
//
#define _TRACK_BLOCKING_POINTS 0

//
// Operating systems < Vista treat a periodic timer with a period of INFINITE as a one shot meaning that you cannot correctly reschedule the timer.
// The throttling timer is a periodic timer which gets rescheduled with changing due times instead of getting recreated frequently.  If we fall below
// throttling threshold, it is scheduled for an infinite period/due time until we hit the throttling threshold again.
//
// Since infinite periods don't work on < Vista, we'll make the timer pseudo-infinite.
//
// Pseudo infinite would be defined to 0xFFFFFFFE; however, on WinXP, scheduling a timer with a timeout between 0x80000000 and 0xFFFFFFFE creates an issue
// if another timer is scheduled within 1 millisecond of such a long pole timer (the second timer never fires).  Hence, we define pseudo-infinite
// to be 0x7FFFFFFF to avoid both of these problems.  Yes this means that every 24 days of continuous computation on a scheduler,
// we'll get a spurious throttler awakening.  Oh well.
//
#define PSEUDOINFINITE 0x7FFFFFFF

//
// If this is defined to 1, we will not use the allocation routines that hide ConcRT's allocations from the CRT memory leak detection tool so
// that ConcRT leaks will be reported.
//
#define _DEBUG_MEMORY_LEAKS 0

// _ONECORE is defined if the CRT is targeting MSDK. For both cases, the surface area of the CRT
// is the reduced surface area that excludes methods and types under if defined(_CRT_USE_WINAPI_FAMILY_DESKTOP_APP)

#if defined(_ONECORE)
#define ENSURE_NOT_APP() throw invalid_operation()

// Concrt public headers hide certain APIs from MSDK compliant
// However, for internal builds we need those API declarations.
#ifndef _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#define _CRT_USE_WINAPI_FAMILY_DESKTOP_APP 1
#endif // _CRT_USE_WINAPI_FAMILY_DESKTOP_APP

#else
#define ENSURE_NOT_APP()
#endif // defined(_ONECORE)

#include "targetver.h"

// All the platform specific headers are included here
#include "Platform.h"

// C Runtime Header Files:

#include <internal_shared.h>
#include <stdio.h>
#include <stddef.h>
#include <sal.h>
#include <new.h>
#include <malloc.h>
#include <stdexcept>
#include <intsafe.h>

#pragma warning(disable :4127)

#ifndef _DEBUG
#define _concrt_new                        new
#else   /* _DEBUG */
#if !_DEBUG_MEMORY_LEAKS
// _ConcRTNewMoniker is used to make the signature of ConcRT operators new and delete globally unique,
// so that they don't clash with the operators defined in MFC (if statically linked) and user code.
class _ConcRTNewMoniker{};

inline void * __cdecl operator new(size_t _Size, _ConcRTNewMoniker, const char *_File, int _Line)
{
    return ::operator new(_Size, _CRT_BLOCK, _File, _Line);
}
inline void __cdecl operator delete(void * _P, _ConcRTNewMoniker, const char *, int) noexcept
{
    ::operator delete(_P);
}
inline void * __cdecl operator new[](size_t _Size, _ConcRTNewMoniker, const char *_File, int _Line)
{
    return ::operator new[](_Size, _CRT_BLOCK, _File, _Line);
}
inline void __cdecl operator delete[](void * _P, _ConcRTNewMoniker, const char *, int) noexcept
{
    ::operator delete[](_P);
}
#define _concrt_new                           new(_ConcRTNewMoniker(), __FILE__, __LINE__)
#else
#define _concrt_new                        new
#endif
#endif

// Forward declarations
namespace Concurrency
{
namespace details
{
    // From runtime

    class ScheduleGroupBase;
    class ScheduleGroupSegmentBase;
    class CacheLocalScheduleGroup;
    class FairScheduleGroup;
    class SchedulingRing;
    class SchedulingNode;
    class VirtualProcessor;
    class ThreadVirtualProcessor;
    class SchedulerBase;
    class ThreadScheduler;
    class ContextBase;
    class InternalContextBase;
    class ExternalContextBase;
    class ExternalStatistics;

    class _UnrealizedChore;

    // UMS
#ifndef _UMS_DISABLED
    class UMSThreadVirtualProcessor;
    class UMSThreadScheduler;
    class UMSThreadInternalContext;
#endif // _UMS_DISABLED

    // From resource manager

    class ThreadProxy;

    class SchedulerProxy;
    class ResourceManager;
    class ExecutionResource;
    class VirtualProcessorRoot;
    class FreeVirtualProcessorRoot;
    class ThreadProxy;
    class FreeThreadProxy;
    struct IThreadProxyFactory;
    class FreeThreadProxyFactory;
    class SubAllocator;

    // UMS
#ifndef _UMS_DISABLED
    class UMSBackgroundPoller;
    class UMSSchedulerProxy;
    class UMSThreadProxy;
    class UMSFreeVirtualProcessorRoot;
    class UMSFreeThreadProxy;
    class TransmogrifiedPrimary;
    class Transmogrificator;
#endif // _UMS_DISABLED

}

// From runtime


struct IExecutionContext;

// From resource manager

struct IScheduler;
struct IResourceManager;

} // namespace Concurrency

const int KB = 1024;
const size_t s_cacheLineSize = 64;

// Public ConcRT Header Files:
#include <concrt.h>
#include <concrtrm.h>

#include <condition_variable>
#include <mutex>

// Make sure that we never will use pplconcrt.h while compiling CRT
#define _PPLWIN_H
#include <pplcancellation_token.h>

#if defined(_DEBUG)
#define CTX_DEBUGBIT_ADDEDTORUNNABLES 0x00000001
#define CTX_DEBUGBIT_REMOVEDFROMRUNNABLES 0x00000002
#define CTX_DEBUGBIT_ADDEDTOLOCALRUNNABLECONTEXTS 0x00000004
#define CTX_DEBUGBIT_POPPEDFROMLOCALRUNNABLECONTEXTS 0x00000008
#define CTX_DEBUGBIT_STOLENFROMLOCALRUNNABLECONTEXTS 0x00000010
#define CTX_DEBUGBIT_PULLEDFROMCOMPLETIONLIST 0x00000020
#define CTX_DEBUGBIT_AFFINITIZED 0x00000040
#define CTX_DEBUGBIT_COOPERATIVEBLOCKED 0x00000080
#define CTX_DEBUGBIT_UMSBLOCKED 0x00000100
#define CTX_DEBUGBIT_CRITICALNOTIFY 0x00000200
#define CTX_DEBUGBIT_CHAINEDCRITICALBLOCK 0x00000400
#define CTX_DEBUGBIT_WAKEFROMCHAINEDCRITICALBLOCK 0x00000800
#define CTX_DEBUGBIT_LIKELYTOSTARTUPIDLEVPROCONOTHERCONTEXT 0x00001000
#define CTX_DEBUGBIT_STARTUPIDLEVPROCONADD 0x00002000
#define CTX_DEBUGBIT_ACTIVATEDAFTERRMAWAKEN 0x00004000
#define CTX_DEBUGBIT_RELEASED 0x00008000
#define CTX_DEBUGBIT_REINITIALIZED 0x00010000
#define CTX_DEBUGBIT_SWITCHTOWITHASSOCIATEDCHORE 0x00020000
#define CTX_DEBUGBIT_PRIMARYAFFINITIZEFROMSEARCH 0x00040000
#define CTX_DEBUGBIT_PRIMARYRESERVEDCONTEXT 0x00080000
#define CTX_DEBUGBIT_PRIMARYAFFINITIZEFROMCRITICAL 0x00100000
#define CTX_DEBUGBIT_PRIMARYSWITCHTOFAILED 0x00200000
#define CTX_DEBUGBIT_HOLDINGUMSBLOCKEDCONTEXT 0x00400000

namespace Concurrency
{
namespace details
{
    void SetContextDebugBits(::Concurrency::details::InternalContextBase *pContext, DWORD bits);
} // namespace details
} // namespace Concurrency
#endif // _DEBUG

// Namespaces we use internally

using namespace Concurrency;
using namespace Concurrency::details;

// Internal Header Files (Both):
#include "utils.h"
#include "collections.h"
#include "Trace.h"

// Internal Header Files (Resource Manager):
#include "rminternal.h"
#include "ExecutionResource.h"
#include "VirtualProcessorRoot.h"
#include "FreeVirtualProcessorRoot.h"
#include "HillClimbing.h"

// Internal Header Files (Scheduler):
#include "Mailbox.h"
#include "WorkStealingQueue.h"
#include "StructuredWorkStealingQueue.h"
#include "workqueue.h"
#include "RealizedChore.h"

#include "SearchAlgorithms.h"

#include "VirtualProcessor.h"
#include "SchedulingNode.h"
#include "ThreadVirtualProcessor.h"

#ifndef _UMS_DISABLED
#include "UMSThreadVirtualProcessor.h"
#endif // _UMS_DISABLED

#include "SubAllocator.h"
#include "ContextBase.h"
#include "SchedulerBase.h"
#include "InternalContextBase.h"
#include "SchedulingRing.h"
#include "ScheduleGroupBase.h"
#include "CacheLocalScheduleGroup.h"
#include "FairScheduleGroup.h"

#include "ExternalContextBase.h"
#include "ThreadInternalContext.h"
#include "ThreadScheduler.h"

#ifndef _UMS_DISABLED
#include "UMSThreadScheduler.h"
#endif // _UMS_DISABLED

#include "align.h"
#include "TaskCollection.h"
#include "SchedulerProxy.h"

// We need this header for the DECLARE_STATIC* macros
// The UMS specific stuff is ifdef'ed inside the header
#include "UMSWrapper.h"


#include "WinRTWrapper.h"

#ifndef _UMS_DISABLED
#include "UMSThreadInternalContext.h"
#include "UMSSchedulingContext.h"
#include "UMSBackgroundPoller.h"
#include "TransmogrifiedPrimary.h"
#include "Transmogrificator.h"
#endif // _UMS_DISABLED

#include "ThreadProxy.h"
#include "FreeThreadProxy.h"
#include "Timer.h"

#ifndef _UMS_DISABLED
#include "UMSThreadProxy.h"
#include "UMSFreeThreadProxy.h"
#endif // _UMS_DISABLED

#include "ThreadProxyFactory.h"
#include "ResourceManager.h"

#ifndef _UMS_DISABLED
#include "UMSSchedulerProxy.h"
#include "UMSFreeVirtualProcessorRoot.h"
#endif // _UMS_DISABLED

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A _Condition_variable which is explicitly aware of the Concurrency Runtime.
    /// </summary>
    /**/
    class _Condition_variable
    {
    public:

        /// <summary>
        ///     Constructs a new _Condition_variable.
        /// </summary>
        /**/
        _CONCRTIMP _Condition_variable();

        /// <summary>
        ///     Destroys a _Condition_variable.
        /// </summary>
        /**/
        _CONCRTIMP ~_Condition_variable();

        /// <summary>
        ///     Waits for the _Condition_variable to become signaled. The lock argument passed in is unlocked by the _Condition_variable
        ///     and relocked before the wait returns.
        /// </summary>
        /// <param name="_Lck">
        ///     The critical_section to unlock before waiting and relock before the wait returns.
        /// </param>
        /// <seealso cref="critical_section Class"/>
        /**/
        _CONCRTIMP void wait(::Concurrency::critical_section& _Lck);

        /// <summary>
        ///     Waits for the _Condition_variable to become signaled. The lock argument passed in is unlocked by the _Condition_variable
        ///     and relocked before the wait returns.
        /// </summary>
        /// <param name="_Lck">
        ///     The critical_section to unlock before waiting and relock before the wait returns.
        /// </param>
        /// <param name="_Timeout">
        ///     A timeout, in milliseconds, for how long to wait for.
        /// </param>
        /// <seealso cref="critical_section Class"/>
        /**/
        _CONCRTIMP bool wait_for(::Concurrency::critical_section& _Lck, unsigned int _Timeout = COOPERATIVE_TIMEOUT_INFINITE);

        /// <summary>
        ///     Notify a single waiter of the _Condition_variable.
        /// </summary>
        /**/
        _CONCRTIMP void notify_one();

        /// <summary>
        ///     Notify all the waiters of the _Condition_variable.
        /// </summary>
        /**/
        _CONCRTIMP void notify_all();

    private:

        // Prevent bad usage of copy-constructor and copy-assignment
        _Condition_variable(const _Condition_variable& _Event);
        _Condition_variable& operator=(const _Condition_variable& _Event);

        void * volatile _M_pWaitChain;
        ::Concurrency::critical_section _M_lock;
    };
} } // namespace Concurrency::details
