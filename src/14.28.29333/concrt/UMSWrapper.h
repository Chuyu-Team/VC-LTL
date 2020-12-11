// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSWrapper.h
//
// Dynamic wrappers around UMS functions
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#define DEFINE_STATIC_WRAPPER_FN(classname, name, rt) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name() \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn();\
    }

#define DECLARE_STATIC_WRAPPER_FN(name, rt) \
    public:\
        static rt name();\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)();\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_1(classname, name, rt, a1t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1);\
    }

#define DECLARE_STATIC_WRAPPER_FN_1(name, rt, a1t) \
    public:\
        static rt name(a1t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_2(classname, name, rt, a1t, a2t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2);\
    }

#define DECLARE_STATIC_WRAPPER_FN_2(name, rt, a1t, a2t) \
    public:\
        static rt name(a1t, a2t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_3(classname, name, rt, a1t, a2t, a3t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2, a3t a3) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2, a3);\
    }

#define DECLARE_STATIC_WRAPPER_FN_3(name, rt, a1t, a2t, a3t) \
    public:\
        static rt name(a1t, a2t, a3t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t, a3t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_4(classname, name, rt, a1t, a2t, a3t, a4t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2, a3t a3, a4t a4) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2, a3, a4);\
    }

#define DECLARE_STATIC_WRAPPER_FN_4(name, rt, a1t, a2t, a3t, a4t) \
    public:\
        static rt name(a1t, a2t, a3t, a4t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t, a3t, a4t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_5(classname, name, rt, a1t, a2t, a3t, a4t, a5t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2, a3t a3, a4t a4, a5t a5) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2, a3, a4, a5);\
    }

#define DECLARE_STATIC_WRAPPER_FN_5(name, rt, a1t, a2t, a3t, a4t, a5t) \
    public:\
        static rt name(a1t, a2t, a3t, a4t, a5t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t, a3t, a4t, a5t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_6(classname, name, rt, a1t, a2t, a3t, a4t, a5t, a6t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2, a3t a3, a4t a4, a5t a5, a6t a6) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2, a3, a4, a5, a6);\
    }

#define DECLARE_STATIC_WRAPPER_FN_6(name, rt, a1t, a2t, a3t, a4t, a5t, a6t) \
    public:\
        static rt name(a1t, a2t, a3t, a4t, a5t, a6t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t, a3t, a4t, a5t, a6t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_7(classname, name, rt, a1t, a2t, a3t, a4t, a5t, a6t, a7t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2, a3t a3, a4t a4, a5t a5, a6t a6, a7t a7) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2, a3, a4, a5, a6, a7);\
    }

#define DECLARE_STATIC_WRAPPER_FN_7(name, rt, a1t, a2t, a3t, a4t, a5t, a6t, a7t) \
    public:\
        static rt name(a1t, a2t, a3t, a4t, a5t, a6t, a7t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t, a3t, a4t, a5t, a6t, a7t);\
        static Pfn_ ## name s_pfn_ ## name;

#define DEFINE_STATIC_WRAPPER_FN_8(classname, name, rt, a1t, a2t, a3t, a4t, a5t, a6t, a7t, a8t) \
    classname ::Pfn_ ## name classname ::s_pfn_ ## name;\
    rt classname :: name(a1t a1, a2t a2, a3t a3, a4t a4, a5t a5, a6t a6, a7t a7, a8t a8) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name) Security::DecodePointer(s_pfn_ ## name); \
        ASSERT(pfn != NULL);\
        return pfn(a1, a2, a3, a4, a5, a6, a7, a8);\
    }

#define DECLARE_STATIC_WRAPPER_FN_8(name, rt, a1t, a2t, a3t, a4t, a5t, a6t, a7t, a8t) \
    public:\
        static rt name(a1t, a2t, a3t, a4t, a5t, a6t, a7t, a8t);\
    private:\
        typedef rt (WINAPI *Pfn_ ## name)(a1t, a2t, a3t, a4t, a5t, a6t, a7t, a8t);\
        static Pfn_ ## name s_pfn_ ## name;

#define GETPROC(module, name) \
    {\
        Pfn_ ## name pfn = (Pfn_ ## name)(GetProcAddress(GetModuleHandleW(module), #name));\
        if (pfn == NULL) throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));\
        s_pfn_ ## name = (Pfn_ ## name) Security::EncodePointer(pfn);\
    }

#define GETPROCKERNEL32(name) \
    GETPROC(L"kernel32.dll", name)

#ifndef _UMS_DISABLED
namespace Concurrency
{
namespace details
{
    class UMS
    {
    public:

        /// <summary>
        ///     Initializes all static function pointers to UMS functions.  We do not call or link against these for distribution
        ///     against OS's below Win7.
        /// </summary>
        static void Initialize();

        /// <summary>
        ///     Returns whether or not the UMS wrappers have been initialized yet.
        /// </summary>
        static bool Initialized();

        DECLARE_STATIC_WRAPPER_FN_1(CreateUmsCompletionList, BOOL, PUMS_COMPLETION_LIST*);
        DECLARE_STATIC_WRAPPER_FN_3(DequeueUmsCompletionListItems, BOOL, PUMS_COMPLETION_LIST, DWORD, PUMS_CONTEXT*);
        DECLARE_STATIC_WRAPPER_FN_2(GetUmsCompletionListEvent, BOOL, PUMS_COMPLETION_LIST, PHANDLE);
        DECLARE_STATIC_WRAPPER_FN_1(ExecuteUmsThread, BOOL, PUMS_CONTEXT);
        DECLARE_STATIC_WRAPPER_FN_1(UmsThreadYield, BOOL, PVOID);
        DECLARE_STATIC_WRAPPER_FN_1(DeleteUmsCompletionList, BOOL, PUMS_COMPLETION_LIST);
        DECLARE_STATIC_WRAPPER_FN(GetCurrentUmsThread, PUMS_CONTEXT);
        DECLARE_STATIC_WRAPPER_FN_1(GetNextUmsListItem, PUMS_CONTEXT, PUMS_CONTEXT);
        DECLARE_STATIC_WRAPPER_FN_5(QueryUmsThreadInformation, BOOL, PUMS_CONTEXT, UMS_THREAD_INFO_CLASS, PVOID, ULONG, PULONG);
        DECLARE_STATIC_WRAPPER_FN_4(SetUmsThreadInformation, BOOL, PUMS_CONTEXT, UMS_THREAD_INFO_CLASS, PVOID, ULONG);
        DECLARE_STATIC_WRAPPER_FN_1(DeleteUmsThreadContext, BOOL, PUMS_CONTEXT);
        DECLARE_STATIC_WRAPPER_FN_1(CreateUmsThreadContext, BOOL, PUMS_CONTEXT*);
        DECLARE_STATIC_WRAPPER_FN_1(EnterUmsSchedulingMode, BOOL, PUMS_SCHEDULER_STARTUP_INFO);

        DECLARE_STATIC_WRAPPER_FN_8(CreateRemoteThreadEx, HANDLE, HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPPROC_THREAD_ATTRIBUTE_LIST, LPDWORD);
        DECLARE_STATIC_WRAPPER_FN_4(InitializeProcThreadAttributeList, BOOL, LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
        DECLARE_STATIC_WRAPPER_FN_7(UpdateProcThreadAttribute, BOOL, LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
        DECLARE_STATIC_WRAPPER_FN_1(DeleteProcThreadAttributeList, VOID, LPPROC_THREAD_ATTRIBUTE_LIST);


    private:

        static volatile LONG s_fInitialized;

    };
} // namespace details
} // namespace Concurrency

#endif //_UMS_DISABLED
