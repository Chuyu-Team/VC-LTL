// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSWrapper.cpp
//
// Dynamic wrapper of UMS functions.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    volatile LONG UMS::s_fInitialized = FALSE;

    /// <summary>
    ///     Initializes all static function pointers to UMS functions.  We do not call or link against these for distribution
    ///     on Windows Server 2008
    /// </summary>
    void UMS::Initialize()
    {
        GETPROCKERNEL32(CreateRemoteThreadEx);
        GETPROCKERNEL32(CreateUmsCompletionList);
        GETPROCKERNEL32(CreateUmsThreadContext);
        GETPROCKERNEL32(DeleteProcThreadAttributeList);
        GETPROCKERNEL32(DeleteUmsCompletionList);
        GETPROCKERNEL32(DeleteUmsThreadContext);
        GETPROCKERNEL32(DequeueUmsCompletionListItems);
        GETPROCKERNEL32(EnterUmsSchedulingMode);
        GETPROCKERNEL32(ExecuteUmsThread);
        GETPROCKERNEL32(GetCurrentUmsThread);
        GETPROCKERNEL32(GetNextUmsListItem);
        GETPROCKERNEL32(GetUmsCompletionListEvent);
        GETPROCKERNEL32(InitializeProcThreadAttributeList);
        GETPROCKERNEL32(QueryUmsThreadInformation);
        GETPROCKERNEL32(SetUmsThreadInformation);
        GETPROCKERNEL32(UmsThreadYield);
        GETPROCKERNEL32(UpdateProcThreadAttribute);

        InterlockedExchange(&s_fInitialized, TRUE);
    }

    /// <summary>
    ///     Returns whether or not the UMS wrappers have been initialized yet.
    /// </summary>
    bool UMS::Initialized()
    {
        return (s_fInitialized != FALSE);
    }

    DEFINE_STATIC_WRAPPER_FN_1(UMS, CreateUmsCompletionList, BOOL, PUMS_COMPLETION_LIST*);
    DEFINE_STATIC_WRAPPER_FN_3(UMS, DequeueUmsCompletionListItems, BOOL, PUMS_COMPLETION_LIST, DWORD, PUMS_CONTEXT*);
    DEFINE_STATIC_WRAPPER_FN_2(UMS, GetUmsCompletionListEvent, BOOL, PUMS_COMPLETION_LIST, PHANDLE);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, ExecuteUmsThread, BOOL, PUMS_CONTEXT);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, UmsThreadYield, BOOL, PVOID);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, DeleteUmsCompletionList, BOOL, PUMS_COMPLETION_LIST);
    DEFINE_STATIC_WRAPPER_FN(UMS, GetCurrentUmsThread, PUMS_CONTEXT);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, GetNextUmsListItem, PUMS_CONTEXT, PUMS_CONTEXT);
    DEFINE_STATIC_WRAPPER_FN_5(UMS, QueryUmsThreadInformation, BOOL, PUMS_CONTEXT, UMS_THREAD_INFO_CLASS, PVOID, ULONG, PULONG);
    DEFINE_STATIC_WRAPPER_FN_4(UMS, SetUmsThreadInformation, BOOL, PUMS_CONTEXT, UMS_THREAD_INFO_CLASS, PVOID, ULONG);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, DeleteUmsThreadContext, BOOL, PUMS_CONTEXT);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, CreateUmsThreadContext, BOOL, PUMS_CONTEXT*);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, EnterUmsSchedulingMode, BOOL, PUMS_SCHEDULER_STARTUP_INFO);
    DEFINE_STATIC_WRAPPER_FN_8(UMS, CreateRemoteThreadEx, HANDLE, HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPPROC_THREAD_ATTRIBUTE_LIST, LPDWORD);
    DEFINE_STATIC_WRAPPER_FN_4(UMS, InitializeProcThreadAttributeList, BOOL, LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
    DEFINE_STATIC_WRAPPER_FN_7(UMS, UpdateProcThreadAttribute, BOOL, LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
    DEFINE_STATIC_WRAPPER_FN_1(UMS, DeleteProcThreadAttributeList, VOID, LPPROC_THREAD_ATTRIBUTE_LIST);

} // namespace details
} // namespace Concurrency
