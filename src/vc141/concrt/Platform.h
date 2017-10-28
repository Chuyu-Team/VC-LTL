// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Platform.h : abstracts the underlying platform APIs
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

// Windows headers that we need

#include <Windows.h>
#include <winnt.h>
#include <OAIdl.h>

#include <roapi.h>

#undef Yield    // The windows headers #define Yield, a name we want to use

#include <wmistr.h>
#include <evntrace.h>
#include <crtdefs.h>

namespace Concurrency { namespace details { namespace platform {

/// <summary>
/// Initializes function pointers that are only present on specific versions of the OS
/// </summary>
void InitializeSystemFunctionPointers();

/****************** Events ***************************/

/// <summary>
/// Creates an auto reset event
/// </summary>
HANDLE __CreateAutoResetEvent(bool initialSet = false);

/// <summary>
/// Creates a manual reset event
/// </summary>
HANDLE __CreateManualResetEvent(bool initialSet = false);

/************** Tickcount ***************************/

/// <summary>
/// Gets the current tick count
/// </summary>
ULONGLONG __GetTickCount64();

/************** Windows critical section ***************************/

/// <summary>
/// Initializes the critical section
/// </summary>
BOOL __InitializeCriticalSectionEx(CRITICAL_SECTION * cs, DWORD spinCount);

/************** Thread Local Storage *****************************/

/// <summary>
/// Allocates a TLS slot
/// </summary>
DWORD __TlsAlloc();

/// <summary>
/// Frees a TLS slot
/// </summary>
void __TlsFree(DWORD index);

/// <summary>
/// Gets the value stored in the specified TLS slot
/// </summary>
PVOID __TlsGetValue(DWORD index);

/// <summary>
/// Stores a value in the specified TLS slot
/// </summary>
void __TlsSetValue(DWORD index, PVOID value);

/************** Thread Priority ***************************/

/// <summary>
/// Sets the thread priority
/// </summary>
void __SetThreadPriority(HANDLE hThread, int priority);

/// <summary>
/// Retrieves the thread priority
/// </summary>
int __GetThreadPriority(HANDLE hThread);

/************** Thread Affinity ***************************/

/// <summary>
/// Retrieves the thread group affinity
/// </summary>
BOOL __GetThreadGroupAffinity(HANDLE hThread, PGROUP_AFFINITY affinity);

/// <summary>
/// Sets the thread group affinity
/// </summary>
BOOL __SetThreadGroupAffinity(HANDLE hThread, const GROUP_AFFINITY * affinity);

/************** Thread yield ***************************/

/// <summary>
/// Yield execution to another ready thread
/// </summary>
void __SwitchToThread();

/// <summary>
/// Yield execution to another ready thread (ms is assumed to be 0 or 1)
/// </summary>
void __Sleep(DWORD ms);

/************ Thread *********************************************/
/// <summary>
/// Creates a thread
/// </summary>
HANDLE __CreateThread(LPSECURITY_ATTRIBUTES lpAttributes,
    size_t stackSize,
    LPTHREAD_START_ROUTINE startAddress,
    LPVOID param,
    DWORD flags,
    LPDWORD threadId);

/// <summary>
/// Releases the thread handle
/// </summary>
void __CloseThreadHandle(HANDLE hThread);

/// <summary>
/// Waits for the thread to exit
/// </summary>
DWORD __WaitForThread(HANDLE hThread, DWORD timeout);

/// <summary>
/// Signals hSignal object and waits for hWait. Returns the reason for returning from wait
/// </summary>
DWORD __SignalObjectAndWait(HANDLE hSignal, HANDLE hWait, DWORD ms, BOOL alertable);

/************ Timer *********************************************/
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
);

/// <summary>
/// Deletes the timer
/// </summary>
void __DeleteTimerQueueTimer(HANDLE timerQueue, HANDLE hTimer, HANDLE completionEvent);

/// <summary>
/// Changes the due time of the timer.
/// </summary>
BOOL __ChangeTimerQueueTimer(HANDLE timerQueue, HANDLE hTimer, ULONG dueTime, ULONG period);

/************** RegisterWaitForsingleObject ***********/

/// <summary>
/// Registers a waiter for the given handle. The callback is invoked when the handle is signalled
/// </summary>
HANDLE __RegisterWaitForSingleObject(HANDLE hEvent, WAITORTIMERCALLBACK callback, PVOID context);

/// <summary>
/// Removes the waiter for the given handle. Pending callbacks are cancelled. If a callback is
/// already running then this will NOT wait for the callback to complete.
/// </summary>
void __UnregisterWait(HANDLE hWait);

/************** System Info ***************************/

/// <summary>
/// Retrieves the information about the relationships of logical processors and related hardware
/// </summary>
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX __GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP relation,  PDWORD retLength);

/// <summary>
/// Retrieves the information about logical processors and related hardware
/// </summary
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION __GetLogicalProcessorInformation(PDWORD retLength);

/// <summary>
/// Retrieves the processor group and number of the logical processor where the thread is running
/// </summary>
void __GetCurrentProcessorNumberEx(PPROCESSOR_NUMBER procNum);

/// <summary>
/// Returns the highest numa node number
/// </summary>
ULONG __GetNumaHighestNodeNumber();

/// <summary>
/// Returns current thread ID
/// </summary>
#if defined(_CRTBLD) && !defined(CRTDLL2)
_CONCRTIMP
#endif
long __cdecl GetCurrentThreadId();

}}} // namespace Concurrency::details::platform
