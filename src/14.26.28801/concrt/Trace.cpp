// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Trace.cpp
//
// Implementation of ConcRT tracing API.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

#pragma warning (push)
#pragma warning (disable : 4702) // unreachable code

namespace Concurrency
{

/// <summary>
///     The ETW provider GUID for the Concurrency Runtime.
/// </summary>
/**/
_CONCRTIMP const GUID ConcRT_ProviderGuid = { 0xF7B697A3, 0x4DB5, 0x4d3b, { 0xBE, 0x71, 0xC4, 0xD2, 0x84, 0xE6, 0x59, 0x2F } };

//
// GUIDS for events
//

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are not more specifically described by another category.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/**/
_CONCRTIMP const GUID ConcRTEventGuid = { 0x72B14A7D, 0x704C, 0x423e, { 0x92, 0xF8, 0x7E, 0x6D, 0x64, 0xBC, 0xB9, 0x2A } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to scheduler activity.
/// </summary>
/// <seealso cref="CurrentScheduler Class"/>
/// <seealso cref="Scheduler Class"/>
/**/
_CONCRTIMP const GUID SchedulerEventGuid = { 0xE2091F8A, 0x1E0A, 0x4731, { 0x84, 0xA2, 0x0D, 0xD5, 0x7C, 0x8A, 0x52, 0x61 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to schedule groups.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="ScheduleGroup Class"/>
/**/
_CONCRTIMP const GUID ScheduleGroupEventGuid = { 0xE8A3BF1F, 0xA86B, 0x4390, { 0x9C, 0x60, 0x53, 0x90, 0xB9, 0x69, 0xD2, 0x2C } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to contexts.
/// </summary>
/// <seealso cref="Context Class"/>
/**/
_CONCRTIMP const GUID ContextEventGuid = { 0x5727A00F, 0x50BE, 0x4519, { 0x82, 0x56, 0xF7, 0x69, 0x98, 0x71, 0xFE, 0xCB } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to chores or tasks.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="task_group Class"/>
/// <seealso cref="structured_task_group Class"/>
/**/
_CONCRTIMP const GUID ChoreEventGuid = { 0x7E854EC7, 0xCDC4, 0x405a, { 0xB5, 0xB2, 0xAA, 0xF7, 0xC9, 0xE7, 0xD4, 0x0C } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to virtual processors.
/// </summary>
/**/
_CONCRTIMP const GUID VirtualProcessorEventGuid = { 0x2f27805f, 0x1676, 0x4ecc, { 0x96, 0xfa, 0x7e, 0xb0, 0x9d, 0x44, 0x30, 0x2f } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to locks.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="critical_section Class"/>
/// <seealso cref="reader_writer_lock Class"/>
/**/
_CONCRTIMP const GUID LockEventGuid = { 0x79A60DC6, 0x5FC8, 0x4952, { 0xA4, 0x1C, 0x11, 0x63, 0xAE, 0xEC, 0x5E, 0xB8 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to the resource manager.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="IResourceManager Structure"/>
/**/
_CONCRTIMP const GUID ResourceManagerEventGuid = { 0x2718D25B, 0x5BF5, 0x4479, { 0x8E, 0x88, 0xBA, 0xBC, 0x64, 0xBD, 0xBF, 0xCA } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to usage of the <c>parallel_invoke</c>
///     function.
/// </summary>
/// <seealso cref="parallel_invoke Function"/>
/**/
_CONCRTIMP const GUID PPLParallelInvokeEventGuid = { 0xd1b5b133, 0xec3d, 0x49f4, { 0x98, 0xa3, 0x46, 0x4d, 0x1a, 0x9e, 0x46, 0x82 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to usage of the <c>parallel_for</c>
///     function.
/// </summary>
/// <seealso cref="parallel_for Function"/>
/**/
_CONCRTIMP const GUID PPLParallelForEventGuid = { 0x31c8da6b, 0x6165, 0x4042, { 0x8b, 0x92, 0x94, 0x9e, 0x31, 0x5f, 0x4d, 0x84 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to usage of the <c>parallel_for_each</c>
///     function.
/// </summary>
/// <seealso cref="parallel_for_each Function"/>
/**/
_CONCRTIMP const GUID PPLParallelForeachEventGuid = { 0x5cb7d785, 0x9d66, 0x465d, { 0xba, 0xe1, 0x46, 0x11, 0x6, 0x1b, 0x54, 0x34 } };

/// <summary>
///     A category GUID ({B9B5B78C-0713-4898-A21A-C67949DCED07}) describing ETW events fired by the Agents library in the Concurrency Runtime.
/// </summary>
/**/
_CONCRTIMP const GUID AgentEventGuid = {0xb9b5b78c, 0x713, 0x4898, { 0xa2, 0x1a, 0xc6, 0x79, 0x49, 0xdc, 0xed, 0x7 } };

namespace details
{
    TRACEHANDLE g_ConcRTPRoviderHandle;
    TRACEHANDLE g_ConcRTSessionHandle;

    _CONCRT_TRACE_INFO g_TraceInfo = {0};

    Etw* g_pEtw = NULL;

    Etw::Etw() noexcept
    {
#ifdef _ONECORE
#define TRACE_DLL L"api-ms-win-eventing-classicprovider-l1-1-0.dll"
#else
#define TRACE_DLL L"advapi32.dll"
#endif
        HMODULE hTraceapi = LoadLibraryExW(TRACE_DLL, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
#ifndef _ONECORE
        if (!hTraceapi && GetLastError() == ERROR_INVALID_PARAMETER)
        {
            // LOAD_LIBRARY_SEARCH_SYSTEM32 is not supported on this platfrom, but TRACE_DLL is a KnownDLL so it is safe to load
            // it without supplying the full path. On Windows 8 and higher, LOAD_LIBRARY_SEARCH_SYSTEM32 should be used as a best practice.
            hTraceapi = LoadLibraryW(TRACE_DLL);
        }
#endif
        if (hTraceapi != NULL)
        {
            m_pfnRegisterTraceGuidsW = (FnRegisterTraceGuidsW*) Security::EncodePointer(GetProcAddress(hTraceapi, "RegisterTraceGuidsW"));
            m_pfnUnregisterTraceGuids = (FnUnregisterTraceGuids*) Security::EncodePointer(GetProcAddress(hTraceapi, "UnregisterTraceGuids"));
            m_pfnTraceEvent = (FnTraceEvent*) Security::EncodePointer(GetProcAddress(hTraceapi, "TraceEvent"));
            m_pfnGetTraceLoggerHandle = (FnGetTraceLoggerHandle*) Security::EncodePointer(GetProcAddress(hTraceapi, "GetTraceLoggerHandle"));
            m_pfnGetTraceEnableLevel = (FnGetTraceEnableLevel*) Security::EncodePointer(GetProcAddress(hTraceapi, "GetTraceEnableLevel"));
            m_pfnGetTraceEnableFlags = (FnGetTraceEnableFlags*) Security::EncodePointer(GetProcAddress(hTraceapi, "GetTraceEnableFlags"));
        }
    }

    ULONG Etw::RegisterGuids(WMIDPREQUEST controlCallBack, LPCGUID providerGuid, ULONG guidCount, PTRACE_GUID_REGISTRATION eventGuidRegistration, PTRACEHANDLE providerHandle)
    {
        if (m_pfnRegisterTraceGuidsW != Security::EncodePointer(NULL))
        {
            FnRegisterTraceGuidsW* pfnRegisterTraceGuidsW = (FnRegisterTraceGuidsW*) Security::DecodePointer(m_pfnRegisterTraceGuidsW);
            return pfnRegisterTraceGuidsW(controlCallBack, NULL, providerGuid, guidCount, eventGuidRegistration, NULL, NULL, providerHandle);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    ULONG Etw::UnregisterGuids(TRACEHANDLE handle)
    {
        if (m_pfnUnregisterTraceGuids != Security::EncodePointer(NULL))
        {
            FnUnregisterTraceGuids* pfnUnregisterTraceGuids = (FnUnregisterTraceGuids*) Security::DecodePointer(m_pfnUnregisterTraceGuids);
            return pfnUnregisterTraceGuids(handle);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    ULONG Etw::Trace(TRACEHANDLE handle, PEVENT_TRACE_HEADER eventHeader)
    {
        if (m_pfnTraceEvent != Security::EncodePointer(NULL))
        {
            FnTraceEvent* pfnTraceEvent = (FnTraceEvent*) Security::DecodePointer(m_pfnTraceEvent);
            return pfnTraceEvent(handle, eventHeader);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    TRACEHANDLE Etw::GetLoggerHandle(PVOID buffer)
    {
        if (m_pfnGetTraceLoggerHandle != Security::EncodePointer(NULL))
        {
            FnGetTraceLoggerHandle* pfnGetTraceLoggerHandle = (FnGetTraceLoggerHandle*) Security::DecodePointer(m_pfnGetTraceLoggerHandle);
            return pfnGetTraceLoggerHandle(buffer);
        }

        SetLastError(ERROR_PROC_NOT_FOUND);
        return (TRACEHANDLE)INVALID_HANDLE_VALUE;
    }

    UCHAR Etw::GetEnableLevel(TRACEHANDLE handle)
    {
        if (m_pfnGetTraceEnableLevel != Security::EncodePointer(NULL))
        {
            FnGetTraceEnableLevel* pfnGetTraceEnableLevel = (FnGetTraceEnableLevel*) Security::DecodePointer(m_pfnGetTraceEnableLevel);
            return pfnGetTraceEnableLevel(handle);
        }

        SetLastError(ERROR_PROC_NOT_FOUND);
        return 0;
    }

    ULONG Etw::GetEnableFlags(TRACEHANDLE handle)
    {
        if (m_pfnGetTraceEnableFlags != Security::EncodePointer(NULL))
        {
            FnGetTraceEnableFlags* pfnGetTraceEnableFlags = (FnGetTraceEnableFlags*) Security::DecodePointer(m_pfnGetTraceEnableFlags);
            return pfnGetTraceEnableFlags(handle);
        }

        SetLastError(ERROR_PROC_NOT_FOUND);
        return 0;
    }


    /// <summary>WMI control call back</summary>
    ULONG WINAPI ControlCallback(WMIDPREQUESTCODE requestCode, void*, ULONG*, void* buffer)
    {
        DWORD rc;

        switch (requestCode)
        {
        case WMI_ENABLE_EVENTS:
            // Enable the provider
            {
                g_ConcRTSessionHandle = g_pEtw->GetLoggerHandle(buffer);
                if ((HANDLE)g_ConcRTSessionHandle == INVALID_HANDLE_VALUE)
                    return GetLastError();

                SetLastError(ERROR_SUCCESS);
                ULONG level = g_pEtw->GetEnableLevel(g_ConcRTSessionHandle);
                if (level == 0)
                {
                    rc = GetLastError();
                    if (rc == ERROR_SUCCESS)
                    {
                        // Enable level of 0 means TRACE_LEVEL_INFORMATION
                        level = TRACE_LEVEL_INFORMATION;
                    }
                    else
                    {
                        return rc;
                    }
                }

                ULONG flags = g_pEtw->GetEnableFlags(g_ConcRTSessionHandle);
                if (flags == 0)
                {
                    rc = GetLastError();
                    if (rc == ERROR_SUCCESS)
                    {
                        flags = static_cast<ULONG>(AllEventsFlag);
                    }
                    else
                    {
                        return rc;
                    }
                }

                // Tracing is now enabled.
                g_TraceInfo._EnableTrace((unsigned char)level, (unsigned long)flags);
            }

            break;

        case WMI_DISABLE_EVENTS:    // Disable the provider
            g_TraceInfo._DisableTrace();
            g_ConcRTSessionHandle = 0;
            break;

        case WMI_EXECUTE_METHOD:
        case WMI_REGINFO:
        case WMI_DISABLE_COLLECTION:
        case WMI_ENABLE_COLLECTION:
        case WMI_SET_SINGLE_ITEM:
        case WMI_SET_SINGLE_INSTANCE:
        case WMI_GET_SINGLE_INSTANCE:
        case WMI_GET_ALL_DATA:
        default:
            return ERROR_INVALID_PARAMETER;
        }

        return ERROR_SUCCESS;
    }

    void PPL_Trace_Event(const GUID& guid, ConcRT_EventType eventType, UCHAR level)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Guid = guid;
            concrtHeader.header.Class.Type = (UCHAR) eventType;
            concrtHeader.header.Class.Level = level;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }

    void _RegisterConcRTEventTracing()
    {
#if defined(_ONECORE)
        g_pEtw = NULL;
#else
        // Initialize ETW dynamically, and only once, to avoid a static dependency on Advapi32.dll.
        _StaticLock::_Scoped_lock lockHolder(Etw::s_lock);

        if (g_pEtw == NULL)
        {
            g_pEtw = _concrt_new Etw();

            static TRACE_GUID_REGISTRATION eventGuidRegistration[] = {
                { &Concurrency::ConcRTEventGuid, NULL },
                { &Concurrency::SchedulerEventGuid, NULL },
                { &Concurrency::ScheduleGroupEventGuid, NULL },
                { &Concurrency::ContextEventGuid, NULL },
                { &Concurrency::ChoreEventGuid, NULL },
                { &Concurrency::LockEventGuid, NULL },
                { &Concurrency::ResourceManagerEventGuid, NULL }
            };

            ULONG eventGuidCount = sizeof eventGuidRegistration / sizeof eventGuidRegistration[0];

            g_pEtw->RegisterGuids(Concurrency::details::ControlCallback, &ConcRT_ProviderGuid, eventGuidCount, eventGuidRegistration, &g_ConcRTPRoviderHandle);
        }
#endif // defined(_ONECORE)
    }

    void _UnregisterConcRTEventTracing()
    {
        if (g_pEtw != NULL)
        {
            ENSURE_NOT_APP();
            g_TraceInfo._DisableTrace();
            g_pEtw->UnregisterGuids(g_ConcRTPRoviderHandle);
            delete g_pEtw;
            g_pEtw = NULL;
        }
    }
} // namespace details

/// <summary>
///     Enable tracing
/// </summary>
/// <returns>
///     If tracing was correctly initiated, S_OK is returned, otherwise E_NOT_STARTED is returned
/// </returns>
_CONCRTIMP HRESULT EnableTracing()
{
    // Deprecated
    return S_OK;
}


/// <summary>
///     Disables tracing
/// </summary>
/// <returns>
///     If tracing was correctly disabled, S_OK is returned.  If tracing was not previously initiated,
///     E_NOT_STARTED is returned
/// </returns>
_CONCRTIMP HRESULT DisableTracing()
{
    // Deprecated
    return S_OK;
}

_CONCRTIMP const _CONCRT_TRACE_INFO* _GetConcRTTraceInfo()
{
    if (g_pEtw == NULL)
    {
        ::Concurrency::details::_RegisterConcRTEventTracing();
    }

    return &g_TraceInfo;
}

// Trace an event signaling the begin of a PPL function
_CONCRTIMP void _Trace_ppl_function(const GUID& guid, UCHAR level, ConcRT_EventType type)
{
    const _CONCRT_TRACE_INFO * traceInfo = _GetConcRTTraceInfo();

    if (traceInfo->_IsEnabled(level, PPLEventFlag))
        Concurrency::details::PPL_Trace_Event(guid, type, level);
}

// Trace an event from the Agents library
_CONCRTIMP void _Trace_agents(::Concurrency::Agents_EventType eventType, __int64 agentId, ...)
{
    va_list args;
    va_start(args, agentId);

    UCHAR level = TRACE_LEVEL_INFORMATION;
    const _CONCRT_TRACE_INFO * traceInfo = _GetConcRTTraceInfo();

    if (traceInfo->_IsEnabled(level, AgentEventFlag))
    {
        AGENTS_TRACE_EVENT_DATA agentsData = {0};

        // Header
        agentsData.header.Size = sizeof(agentsData);
        agentsData.header.Flags = WNODE_FLAG_TRACED_GUID;
        agentsData.header.Guid = AgentEventGuid;
        agentsData.header.Class.Type = (UCHAR) eventType;
        agentsData.header.Class.Level = level;

        // Payload
        agentsData.payload.AgentId1 = agentId;

        switch (eventType)
        {
        case AGENTS_EVENT_CREATE:
            // LWT id
            agentsData.payload.AgentId2 = va_arg(args, __int64);
            break;

        case AGENTS_EVENT_DESTROY:
            // No other payload
            break;

        case AGENTS_EVENT_LINK:
        case AGENTS_EVENT_UNLINK:
            // Target's id
            agentsData.payload.AgentId2 = va_arg(args, __int64);
            break;

        case AGENTS_EVENT_START:
            // No other payload
            break;

        case AGENTS_EVENT_END:
            // Number of messages processed
            agentsData.payload.Count = va_arg(args, long);
            break;

        case AGENTS_EVENT_SCHEDULE:
            // No other payload
            break;

        case AGENTS_EVENT_NAME:
            {
                wchar_t * name = va_arg(args, wchar_t*);

                if (name != NULL)
                {
                    wcsncpy_s(agentsData.payload.Name, _countof(agentsData.payload.Name), name, _TRUNCATE);
                }
            }
            break;

        default:
            break;
        };

        va_end(args);
        g_pEtw->Trace(g_ConcRTSessionHandle, &agentsData.header);
    }
}

} // namespace Concurrency

#pragma warning (pop)
