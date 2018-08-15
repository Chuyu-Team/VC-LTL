// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Platform.cpp
//
// Platform API abstraction.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"
#include <process.h>
#include <awint.h>

#pragma warning (push)
#pragma warning (disable: 4702 4100)

namespace Concurrency { namespace details { namespace platform {

#if !(defined(_ONECORE) || defined(_KERNELX))
#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)

#define DYNAMIC_AFFINITY_FUNCTIONS

    // A pointer to a kernel32 function that exists only on versions of windows that are 6100 and higher.
    typedef VOID (WINAPI *PFnGetCurrentProcessorNumberEx)(PPROCESSOR_NUMBER);
    static PFnGetCurrentProcessorNumberEx s_pfnGetCurrentProcessorNumberEx = nullptr;

    // A pointer to a kernel32 function that exists only on Win7 and higher
    typedef BOOL (WINAPI *PFnSetThreadGroupAffinity)(HANDLE, const GROUP_AFFINITY *, PGROUP_AFFINITY);
    static PFnSetThreadGroupAffinity s_pfnSetThreadGroupAffinity = nullptr;

    // A pointer to a kernel32 function that exists only on Win7 and higher
    typedef BOOL (WINAPI *PFnGetThreadGroupAffinity)(HANDLE, PGROUP_AFFINITY);
    static PFnGetThreadGroupAffinity s_pfnGetThreadGroupAffinity = nullptr;
#endif  // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)
#endif // !(defined(_ONECORE) || defined(_KERNELX))

/// <summary>
/// Initializes function pointers that are only present on specific versions of the OS
/// </summary>
void InitializeSystemFunctionPointers()
{
#if defined(DYNAMIC_AFFINITY_FUNCTIONS)

#ifdef _ONECORE
#define THREAD_GROUP_DLL L"api-ms-win-core-processtopology-l1-1-0.dll"
#else
#define THREAD_GROUP_DLL L"kernel32.dll"
#endif

    HMODULE hmodThreadGroup = GetModuleHandleW(THREAD_GROUP_DLL);

    // Get/SetThreadGroupAffinity function for Win7
    PFnSetThreadGroupAffinity pfnSetFunction = (PFnSetThreadGroupAffinity) GetProcAddress(hmodThreadGroup,
                                                                                            "SetThreadGroupAffinity");

    PFnGetThreadGroupAffinity pfnGetFunction = (PFnGetThreadGroupAffinity) GetProcAddress(hmodThreadGroup,
                                                                                            "GetThreadGroupAffinity");

    if (pfnSetFunction == NULL || pfnGetFunction == NULL)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    s_pfnSetThreadGroupAffinity = (PFnSetThreadGroupAffinity) Security::EncodePointer(pfnSetFunction);
    s_pfnGetThreadGroupAffinity = (PFnGetThreadGroupAffinity) Security::EncodePointer(pfnGetFunction);

#ifdef _ONECORE
#define CURRENT_PROCESSOR_DLL L"api-ms-win-core-processthreads-l1-1-1.dll"
#else
#define CURRENT_PROCESSOR_DLL L"kernel32.dll"
#endif
    PFnGetCurrentProcessorNumberEx pfnFunction = (PFnGetCurrentProcessorNumberEx)GetProcAddress(GetModuleHandleW(CURRENT_PROCESSOR_DLL),
                                                                                                "GetCurrentProcessorNumberEx");
    if (pfnFunction == NULL)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }
    s_pfnGetCurrentProcessorNumberEx = (PFnGetCurrentProcessorNumberEx) Security::EncodePointer(pfnFunction);

#endif // defined(DYNAMIC_AFFINITY_FUNCTIONS)
}

/************** Events ***************************/

/// <summary>
/// Creates an auto reset event
/// </summary>
HANDLE __CreateAutoResetEvent(bool initialSet)
{
    DWORD flags = 0;

    if (initialSet)
    {
        flags |= CREATE_EVENT_INITIAL_SET;
    }

    HANDLE hEvent = __crtCreateEventExW(NULL, NULL, flags, STANDARD_RIGHTS_ALL | EVENT_MODIFY_STATE);
    if (hEvent == NULL)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    return hEvent;
}

/// <summary>
/// Creates a manual reset event
/// </summary>
HANDLE __CreateManualResetEvent(bool initialSet)
{
    DWORD flags = CREATE_EVENT_MANUAL_RESET;

    if (initialSet)
    {
        flags |= CREATE_EVENT_INITIAL_SET;
    }

    HANDLE hEvent = __crtCreateEventExW(NULL, NULL, flags, STANDARD_RIGHTS_ALL | EVENT_MODIFY_STATE);
    if (hEvent == NULL)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    return hEvent;
}

/************** Tickcount ***************************/

/// <summary>
/// Gets the current tick count
/// </summary>
ULONGLONG __GetTickCount64()
{
    return __crtGetTickCount64();
}

/************** Windows critical section ***************************/

/// <summary>
/// Initializes the critical section
/// </summary>
BOOL __InitializeCriticalSectionEx(CRITICAL_SECTION * cs, DWORD spinCount)
{
    return __crtInitializeCriticalSectionEx(cs, spinCount, 0);
}

/************** Thread Local Storage ***************************/

/// <summary>
/// Allocates a TLS slot
/// </summary>
DWORD __TlsAlloc()
{
#if defined(_ONECORE) || defined(_KERNELX)
    // We use Fls (Fiber local storage) as TLS is not supported for MSDK.
    DWORD index = __crtFlsAlloc(nullptr);
    if (index == FLS_OUT_OF_INDEXES)
#else
    // Use TLS for desktop because multiple schedulers are supported.
    DWORD index = TlsAlloc();
    if (index == TLS_OUT_OF_INDEXES)
#endif
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    return index;
}

/// <summary>
/// Frees a TLS slot
/// </summary>
void __TlsFree(DWORD index)
{
#if defined(_ONECORE) || defined(_KERNELX)
    if (index != FLS_OUT_OF_INDEXES)
    {
        __crtFlsFree(index);
        // Ignore error
    }
#else
    TlsFree(index);
#endif
}

/// <summary>
/// Gets the value stored in the specified TLS slot
/// </summary>
PVOID __TlsGetValue(DWORD index)
{
#if defined(_ONECORE) || defined(_KERNELX)
    // Leave it up to the caller to decide if there was an error when
    // the return value is NULL.
    return __crtFlsGetValue(index);
#else
    return TlsGetValue(index);
#endif
}

/// <summary>
/// Stores a value in the specified TLS slot
/// </summary>
void __TlsSetValue(DWORD index, PVOID value)
{
#if defined(_ONECORE) || defined(_KERNELX)
    if (!__crtFlsSetValue(index, value))
#else
    if (!TlsSetValue(index, value))
#endif
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }
}

/************** Thread Priority ***************************/

/// <summary>
/// Sets the thread priority
/// </summary>
void __SetThreadPriority(HANDLE hThread, int priority)
{
#if defined(_ONECORE) || defined(_KERNELX)
    // Dynamic thread priority modification is not supported under MSDK or _KERNELX
    ENSURE_NOT_APP_OR_KERNELX();
#else
    if (SetThreadPriority(hThread, priority) == 0)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }
#endif // _ONECORE
}

/// <summary>
/// Retrieves the thread priority
/// </summary>
int __GetThreadPriority(HANDLE hThread)
{
#if defined(_CRT_APP) || defined(_KERNELX)
    // MSDK and _KERNELX do not support GetThreadPriority
    ENSURE_NOT_APP_OR_KERNELX();
    return THREAD_PRIORITY_ERROR_RETURN;
#else
    return GetThreadPriority(hThread);
#endif // defined(_CRT_APP) || defined(_KERNELX)
}

/************** Thread Affinity ***************************/

/// <summary>
/// Retrieves the thread group affinity
/// </summary>
BOOL __GetThreadGroupAffinity(HANDLE hThread, PGROUP_AFFINITY affinity)
{
    // Don't do anything when targetting OneCore or _KERNELX (We could set it to active processor mask in the future)
#if !(defined(_ONECORE) || defined(_KERNELX))
#if defined(DYNAMIC_AFFINITY_FUNCTIONS)
    if (::Concurrency::GetOSVersion() >= ::Concurrency::IResourceManager::Win7OrLater)
    {
        PFnGetThreadGroupAffinity fnDecodedFunction = (PFnGetThreadGroupAffinity) Security::DecodePointer(s_pfnGetThreadGroupAffinity);
        CONCRT_VERIFY(fnDecodedFunction(hThread, affinity));
    }
    else
    {
        // On operating systems older than Win7, we don't have access to the correct information about thread's affinity,
        // so we will assume that the affinity is that of the process.
        DWORD_PTR pProcessAffinityMask;
        DWORD_PTR pSystemAffinityMask;

        GetProcessAffinityMask(GetCurrentProcess(), &pProcessAffinityMask, &pSystemAffinityMask);
        affinity->Group = 0;
        affinity->Mask = pProcessAffinityMask;
    }
#else   // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
    CONCRT_VERIFY(GetThreadGroupAffinity(hThread, affinity));
#endif  // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
#endif // !(defined(_ONECORE) || defined(_KERNELX))

    return 1;
}

/// <summary>
/// Sets the thread group affinity
/// </summary>
BOOL __SetThreadGroupAffinity(HANDLE hThread, const GROUP_AFFINITY * affinity)
{
    // Don't do anything when targetting MSDK or _KERNELX
#if !(defined(_ONECORE) || defined(_KERNELX))
#if defined(DYNAMIC_AFFINITY_FUNCTIONS)
    if (::Concurrency::GetOSVersion() >= ::Concurrency::IResourceManager::Win7OrLater)
    {
        PFnSetThreadGroupAffinity fnDecodedFunction = (PFnSetThreadGroupAffinity) Security::DecodePointer(s_pfnSetThreadGroupAffinity);
        CONCRT_VERIFY(fnDecodedFunction(hThread, affinity, NULL));
    }
    else
    {
        CONCRT_VERIFY(SetThreadAffinityMask(hThread, affinity->Mask));
    }
#else   // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
    CONCRT_VERIFY(SetThreadGroupAffinity(hThread, affinity, NULL));
#endif  // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
#endif // !(defined(_ONECORE) || defined(_KERNELX))

#if defined(_KERNELX)
    CONCRT_VERIFY(SetThreadAffinityMask(hThread, affinity->Mask));
#endif //defined(_KERNELX)

    return 1;
}

/************** System Info ***************************/

/// <summary>
/// Retrieves the information about the relationships of logical processors and related hardware
/// </summary>
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX __GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP relation,  PDWORD retLength)
{
#if (defined(_ONECORE) || defined(_KERNELX))
    // MSDK and _KERNELX do not support this API. It is an error to call this API
    ENSURE_NOT_APP_OR_KERNELX();
#else

    ASSERT(retLength != nullptr);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pSysInfo = nullptr;

#if     defined(DYNAMIC_AFFINITY_FUNCTIONS)
    typedef BOOL (WINAPI *PFnGetLogicalProcessorInformationEx)(LOGICAL_PROCESSOR_RELATIONSHIP, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, PDWORD);
#ifdef _ONECORE
#define LOGICAL_PROCESSOR_DLL L"api-ms-win-core-processthreads-l1-1-1.dll"
#else
#define LOGICAL_PROCESSOR_DLL L"kernel32.dll"
#endif
    PFnGetLogicalProcessorInformationEx pfnGetLogicalProcessorInformationEx
        = (PFnGetLogicalProcessorInformationEx) GetProcAddress(GetModuleHandleW(LOGICAL_PROCESSOR_DLL),
                                                                "GetLogicalProcessorInformationEx");
    if (pfnGetLogicalProcessorInformationEx == NULL)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    pfnGetLogicalProcessorInformationEx(relation, NULL, retLength);
#else   // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
    GetLogicalProcessorInformationEx(relation, NULL, retLength);
#endif  // !defined(DYNAMIC_AFFINITY_FUNCTIONS)

    if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    DWORD len = *retLength;
    ASSERT(len > 0);

    pSysInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) malloc(len);

    if (pSysInfo == NULL)
    {
        throw std::bad_alloc();
    }

#if     defined(DYNAMIC_AFFINITY_FUNCTIONS)
    if (!pfnGetLogicalProcessorInformationEx(relation, pSysInfo, retLength))
#else   // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
    if (!GetLogicalProcessorInformationEx(relation, pSysInfo, retLength))
#endif  // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    return pSysInfo;
#endif // (defined(_ONECORE) || defined(_KERNELX))
}

/// <summary>
/// Retrieves the information about logical processors and related hardware
/// </summary>
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION __GetLogicalProcessorInformation(PDWORD retLength)
{
#if (defined(_ONECORE) || defined(_KERNELX))
    // MSDK and _KERNELX do not support this API. It is an error to call this API
    ENSURE_NOT_APP_OR_KERNELX();
#else
#if (defined(_M_IX86) || defined(_M_X64))
    ASSERT(retLength != nullptr);

    GetLogicalProcessorInformation(NULL, retLength);
    if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    DWORD len = *retLength;
    ASSERT(len > 0);

    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pSysInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) malloc(len);
    if (pSysInfo == NULL)
    {
        throw std::bad_alloc();
    }
    if (!GetLogicalProcessorInformation(pSysInfo, retLength))
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    return pSysInfo;
#else
    throw invalid_operation();
#endif // (defined(_M_IX86) || defined(_M_X64))
#endif // (defined(_ONECORE) || defined(_KERNELX))
}

/// <summary>
/// Retrieves the processor group and number of the logical processor where the thread is running
/// </summary>
void __GetCurrentProcessorNumberEx(PPROCESSOR_NUMBER procNum)
{
#if (defined(_ONECORE) || defined(_KERNELX))
    ENSURE_NOT_APP_OR_KERNELX();
#else
#if defined(DYNAMIC_AFFINITY_FUNCTIONS)
    PFnGetCurrentProcessorNumberEx fnDecodedFunction = (PFnGetCurrentProcessorNumberEx) Security::DecodePointer(s_pfnGetCurrentProcessorNumberEx);
    fnDecodedFunction(procNum);
#else   // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
    GetCurrentProcessorNumberEx(procNum);
#endif  // !defined(DYNAMIC_AFFINITY_FUNCTIONS)
#endif // (defined(_ONECORE) || defined(_KERNELX))
}

/// <summary>
/// Returns the highest numa node number
/// </summary>
ULONG __GetNumaHighestNodeNumber()
{
    ULONG highestNodeNumber;
#if (defined(_ONECORE) || defined(_KERNELX))
    // For MSDK and _KERNELX we assume a single NUMA node
    highestNodeNumber = 0;
#else
    if (!GetNumaHighestNodeNumber(&highestNodeNumber))
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }
#endif // (defined(_ONECORE) || defined(_KERNELX))
    return highestNodeNumber;
}

/************** Thread yield ***************************/

/// <summary>
/// Yield execution to another ready thread
/// </summary>
void __SwitchToThread()
{
#if defined(_ONECORE)
    // TODO: Do we need to yield our time quantum?
#else
    SwitchToThread();
#endif
}

/// <summary>
/// Yield execution to another ready thread (ms is assumed to be 0 or 1)
/// </summary>
void __Sleep(DWORD ms)
{
    Sleep(ms);
}

//***********************************************/
//   Timer                                       /
//***********************************************/

/// <summary>
/// Creates a timer
/// </summary>
BOOL __CreateTimerQueueTimer(
    PHANDLE phNewTimer,
    HANDLE timerQueue,
    WAITORTIMERCALLBACK  lpStartAddress,
    PVOID lpParameter,
    DWORD dueTime,
    DWORD period,
    ULONG flags
)
{
#if defined(_ONECORE) || defined(_KERNELX)
    ENSURE_NOT_APP_OR_KERNELX();
#else
    return CreateTimerQueueTimer(phNewTimer,
                                timerQueue,
                                lpStartAddress,
                                lpParameter,
                                dueTime,
                                period,
                                flags);
#endif // defined(_ONECORE) || defined(_KERNELX)
}

/// <summary>
/// Deletes the timer
/// </summary>
void __DeleteTimerQueueTimer(HANDLE timerQueue, HANDLE hTimer, HANDLE completionEvent)
{
#if defined(_ONECORE) || defined(_KERNELX)
    ENSURE_NOT_APP_OR_KERNELX();
#else
    for(int maximalRetry = 16; maximalRetry > 0; --maximalRetry)
    {
        if (!DeleteTimerQueueTimer(timerQueue, hTimer, completionEvent))
        {
            if (GetLastError() == ERROR_IO_PENDING)
                break;
        }
        else
        {
            break;
        }
    }
#endif // defined(_ONECORE) || defined(_KERNELX)
}

/// <summary>
/// Changes the due time of the timer.
/// </summary>
BOOL __ChangeTimerQueueTimer(HANDLE timerQueue, HANDLE hTimer, ULONG dueTime, ULONG period)
{
#if defined(_ONECORE) || defined(_KERNELX)
    ENSURE_NOT_APP_OR_KERNELX();
#else
    return ChangeTimerQueueTimer(timerQueue, hTimer, dueTime, period);
#endif // defined(_ONECORE) || defined(_KERNELX)
}

//***********************************************/
//   CreateThread                                /
//***********************************************/

/// <summary>
/// Creates a thread
/// </summary>
HANDLE __CreateThread(LPSECURITY_ATTRIBUTES lpAttributes,
    size_t stackSize,
    LPTHREAD_START_ROUTINE startAddress,
    LPVOID param,
    DWORD flags,
    LPDWORD threadId)
{
    return CreateThread(lpAttributes, stackSize, startAddress, param, flags, threadId);
}

/// <summary>
/// Releases the thread handle
/// </summary>
void __CloseThreadHandle(HANDLE hThread)
{
    CloseHandle(hThread);
}

/// <summary>
/// Waits for the thread to exit
/// </summary>
DWORD __WaitForThread(HANDLE hThread, DWORD timeout)
{
    return WaitForSingleObjectEx(hThread, timeout, FALSE);
}

/// <summary>
/// Signals hSignal object and waits for hWait. Returns the reason for returning from wait
/// </summary>
DWORD __SignalObjectAndWait(HANDLE hSignal, HANDLE hWait, DWORD ms, BOOL alertable)
{
#if defined(_ONECORE)
    SetEvent(hSignal);
    return WaitForSingleObjectEx(hWait, ms, alertable);
#else
    return SignalObjectAndWait(hSignal, hWait, ms, alertable);
#endif
}


//***********************************************/
//   RegisterWaitForSingleObject                 /
//***********************************************/

/// <summary>
/// Represents a collection of events and a background thread for handling those event notifications. Essentially
/// it performs the equivalent functionality of registerwaitforsingleobect.
/// </summary>
class WaiterThread
{
public:
    WaiterThread() : m_numEvents(0), m_numWaiting(0), m_pendingRemove(0)
    {
        for (int i = 0; i < MAXIMUM_WAIT_OBJECTS; i++)
        {
            m_waitHandles[i] = &m_eventData[i];
        }
    }

    /// <summary>
    /// Create the background thread
    /// </summary>
    void start()
    {
        // Create the background thread
        HANDLE threadHandle = __CreateThread(NULL,
                                            0,
                                            WaiterThread::wait_bridge,
                                            this,
                                            0,
                                            NULL);

        __CloseThreadHandle(threadHandle);
    }

    /// <summary>
    /// Indicate that the handler should be deleted when the background thread exits
    /// </summary>
    void stop()
    {
        auto waiterData = m_waitHandles[0];
        waiterData->handler = nullptr;
        notify(true);

        // The background thread will eventually exit and reclaim this handler
    }

    /// <summary>
    /// Indicates whether there are slots available in this handler
    /// </summary>
    bool is_available()
    {
        return m_numEvents < MAXIMUM_WAIT_OBJECTS;
    }

    /// <summary>
    /// Adds a waiter for the given handle
    /// </summary>
    HANDLE add_handle(HANDLE hEvent, WAITORTIMERCALLBACK callback, PVOID context)
    {
        HANDLE hWait = nullptr;
        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

            if (m_numEvents == 0)
            {
                // Create the wake event
                HANDLE hWake = __crtCreateEventExW(NULL, NULL, 0, STANDARD_RIGHTS_ALL | EVENT_MODIFY_STATE);
                if (hWake== NULL)
                {
                    throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
                }

                add_wait(hWake, wake_bridge, this);
            }

            // Add the user event
            hWait = add_wait(hEvent, callback, context);

            // Snap shot numWaiting
            if (m_numWaiting == 0)
            {
                // Not started yet
                m_numWaiting = m_numEvents;
            }
        }

        // Notify the background thread after releasing the lock
        notify(false);
        return hWait;
    }

    /// <summary>
    /// Remove the waiter for the given handle
    /// </summary>
    static void remove_handle(HANDLE hWait)
    {
        auto waiterData = static_cast<WAITER_DATA *>(hWait);
        auto handler = static_cast<WaiterThread *>(waiterData->handler);
        handler->remove_wait(waiterData);
    }

private:

    typedef struct _WAITER_DATA
    {
        void * handler;
        WAITORTIMERCALLBACK callback;
        PVOID context;
    } WAITER_DATA;

    /// <summary>
    /// Add the waiter to the list
    /// </summary>
    HANDLE add_wait(HANDLE hEvent, WAITORTIMERCALLBACK callback, PVOID context)
    {
        WAITER_DATA * waiterData = m_waitHandles[m_numEvents];
        waiterData->callback = callback;
        waiterData->context = context;
        waiterData->handler = this;

        m_hEvents[m_numEvents] = hEvent;
        m_numEvents++;

        return static_cast<HANDLE>(waiterData);
    }

    /// <summary>
    /// Indicates that the waiter is to be removed from the list. The background
    /// thread is notified which in turn would remove it from the list.
    /// </summary>
    void remove_wait(WAITER_DATA * waiterData)
    {
        waiterData->handler = nullptr;
        notify(true);
    }

    /// <summary>
    /// Notify the background thread
    /// </summary>
    void notify(bool isRemoval)
    {
        if (isRemoval)
        {
            // Wake up the background thread for the first removal
            if (_InterlockedIncrement(&m_pendingRemove) == 1)
            {
                SetEvent(m_hEvents[0]);
            }
        }
        else
        {
            // Avoid waking up the background thread for every event we add...
            if (m_numEvents - m_numWaiting == 1)
            {
                SetEvent(m_hEvents[0]);
            }
        }
    }

    /// <summary>
    /// Invokes the callback
    /// </summary>
    void invoke_handler(DWORD index)
    {
        ASSERT(index < MAXIMUM_WAIT_OBJECTS);

        auto waiterData = m_waitHandles[index];

        // Skip the callback if the callback was removed
        // Special case the wake handler
        if ((index == 0) || (waiterData->handler != nullptr))
        {
            waiterData->callback(waiterData->context, FALSE);
        }
    }

    /// <summary>
    /// The main wait loop
    /// </summary>
    void wait_handler()
    {
        while (m_numWaiting > 0)
        {
            // Wait for the array of events
            DWORD waitResult = WaitForMultipleObjectsEx((DWORD)m_numWaiting, m_hEvents, false, INFINITE, FALSE);
            if (waitResult == WAIT_FAILED)
            {
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
            }

            // Invoke the callback
            DWORD waitHandleIndex = waitResult - WAIT_OBJECT_0;
            ASSERT(waitHandleIndex < m_numWaiting);
            invoke_handler(waitHandleIndex);

            // If the callback removed a waiter or if it was already removed, process it here
            if ((m_numWaiting > 0) && (WaitForSingleObjectEx(m_hEvents[0], 0, FALSE) == WAIT_OBJECT_0))
            {
                invoke_handler(0);
            }
        }
    }

    /// <summary>
    /// static bridge that calls the wait loop
    /// </summary>
    static DWORD WINAPI wait_bridge(PVOID context)
    {
        auto handler = static_cast<WaiterThread *>(context);
        handler->wait_handler();
        delete handler;
        return 0;
    }

    /// <summary>
    /// The main handler for the back ground thread
    /// </summary>
    void wake_handler()
    {
        _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

        auto pendingRemove = _InterlockedExchange(&m_pendingRemove, 0);

        if (pendingRemove != 0)
        {
            process_remove();
        }

        // Update the num waiters (which is common for add and remove of handlers)
        m_numWaiting = m_numEvents;
    }

    /// <summary>
    /// Static bridge for the wake handler
    /// </summary>
    static void CALLBACK wake_bridge(PVOID context, BOOLEAN)
    {
        auto handler = static_cast<WaiterThread *>(context);
        handler->wake_handler();
    }

    /// <summary>
    /// Removes waiters and compacts the array of handles
    /// </summary>
    void process_remove()
    {
        // Walk all the handler and remove the ones that are marked (handler == nullptr)
        // Skip the first event which is our wake handler
        DWORD i = 1;
        while (i < m_numEvents)
        {
            auto waiterData = m_waitHandles[i];

            if (waiterData->handler == nullptr)
            {
                // Remove the event
                CloseHandle(m_hEvents[i]);
                m_numEvents--;

                if (i != m_numEvents)
                {
                    // Swap the last event
                    m_hEvents[i] = m_hEvents[m_numEvents];
                    m_hEvents[m_numEvents] = NULL;

                    m_waitHandles[i] = m_waitHandles[m_numEvents];
                    m_waitHandles[m_numEvents] = waiterData;
                }

                // Process this event again
                continue;
            }

            i++;
        }

        // If the last user event is removed attempt to remove the
        // wake handler. It is not safe to remove the wake event without
        // it being marked for removal by register_wait.
        if ((m_numEvents == 1) && (m_waitHandles[0]->handler == nullptr))
        {
            CloseHandle(m_hEvents[0]);
            m_hEvents[0] = NULL; // For debugging
            m_numEvents--;
        }

        m_numWaiting = m_numEvents;
    }


private:

    // Array of handles that is being waited on
    HANDLE m_hEvents[MAXIMUM_WAIT_OBJECTS];

    // The handlers corresponding to the event array (matching index)
    WAITER_DATA * m_waitHandles[MAXIMUM_WAIT_OBJECTS];

    // All the handles (including ones that are removed/not yet added etc).
    WAITER_DATA m_eventData[MAXIMUM_WAIT_OBJECTS];

    // Total number of events including the ones that are not yet waited upon
    DWORD m_numEvents;

    // The number of events that are being waited on
    DWORD m_numWaiting;

    // The number of pending remove requests
    volatile long m_pendingRemove;

    // Lock for insertion and deletion of handles
    _NonReentrantBlockingLock m_lock;
};

/// <summary>
/// Manages all the waiter threads. A waiter thread can only handle upto
/// MAXIMUM_WAIT_OBJECTS events. This class maintains multiple such waiter threads
/// </summary>
class WaiterThreadPool
{
public:

    WaiterThreadPool() : m_waiter(nullptr)
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~WaiterThreadPool()
    {
        if (m_waiter != nullptr)
        {
            m_waiter->stop();
        }
    }

    /// <summary>
    /// Creates an event handler if required and registers a waiter for the given event in that handler
    /// instance
    /// </summary>
    HANDLE add_waiter(HANDLE hSource, WAITORTIMERCALLBACK callback, PVOID context)
    {
        HANDLE hEvent;
        if (!DuplicateHandle(GetCurrentProcess(),
                            hSource,
                            GetCurrentProcess(),
                            &hEvent,
                            0,
                            FALSE,
                            DUPLICATE_SAME_ACCESS))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        HANDLE hWait = nullptr;
        WaiterThread * newWaiter = nullptr;
        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_lock);

            // Get the event handler
            if ((m_waiter == nullptr) || (!m_waiter->is_available()))
            {
                if (m_waiter != nullptr)
                {
                    m_waiter->stop();
                    m_waiter = nullptr;
                }

                m_waiter = new WaiterThread();
                newWaiter = m_waiter;
            }

            // Add the wait event under the lock
            hWait = m_waiter->add_handle(hEvent, callback, context);
        }

        // Start the handler after we release the lock
        if (newWaiter != nullptr)
        {
            newWaiter->start();
        }

        return hWait;
    }

private:

    WaiterThread * m_waiter;
    _NonReentrantBlockingLock m_lock;
};

// Maintains a default global waiter threadpool instance
// which will be released on process exit
static WaiterThreadPool * s_waiterPool = nullptr;
class DefaultWaiterPool
{
public:
    DefaultWaiterPool()
    {
    }

    ~DefaultWaiterPool()
    {
        if (s_waiterPool != nullptr)
        {
            delete s_waiterPool;
            s_waiterPool = nullptr;
        }
    }

    static WaiterThreadPool * get_waiter()
    {
        if (s_waiterPool == nullptr)
        {
            // Allocate on demand
            auto waiterPool = new WaiterThreadPool;
            if (_InterlockedCompareExchangePointer((volatile PVOID *)&s_waiterPool, waiterPool, nullptr) != nullptr)
            {
                delete waiterPool;
            }
        }

        return s_waiterPool;
    }
};

static DefaultWaiterPool s_defaultWaiterPool;

HANDLE __RegisterWaitForSingleObject(HANDLE hEvent, WAITORTIMERCALLBACK callback, PVOID context)
{
    HANDLE hWait;
#if defined(_KERNELX)
    // The pre-Vista wait APIs are not supported on _KERNELX
    ENSURE_NOT_APP_OR_KERNELX();
#elif defined(_ONECORE)
    auto waiterPool = DefaultWaiterPool::get_waiter();
    hWait = waiterPool->add_waiter(hEvent, callback, context);
#else // !(_ONECORE)
    // Request a thread pool thread to wait for this thread exit.
    if (!RegisterWaitForSingleObject(&hWait,
                    hEvent,
                    callback,
                    context, INFINITE, (WT_EXECUTEONLYONCE | WT_EXECUTEINWAITTHREAD)))
    {
        throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }
#endif // !(_ONECORE)
    return hWait;
}

void __UnregisterWait(HANDLE hWait)
{
#if defined(_KERNELX)
    // The pre-Vista wait APIs are not supported on _KERNELX
    ENSURE_NOT_APP_OR_KERNELX();
#elif defined(_ONECORE)
    WaiterThread::remove_handle(hWait);
#else
    UnregisterWait(hWait);
#endif // !(_ONECORE)
}

}}} // namespace Concurrency::details::platform

#pragma warning (pop)
