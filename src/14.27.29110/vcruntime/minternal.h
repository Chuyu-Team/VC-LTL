/***
*minternal.h - contains declarations of managed internal routines and variables
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Declares routines and variables used internally by the C run-time.
*
*       [Internal]
*
****/

//
//  _CPP_MSTARTUP indicates that the file is included from mstartup.cpp
//      _PARTIAL_TRUST_OBJ indicates that ptrust*.obj is being built
// !_CPP_MSTARTUP building msvcm*
//

#pragma once

#include <internal_shared.h>
#include <Unknwn.h>
// Don't pull have IID defns in the obj for mscoree.h
#ifdef EXTERN_GUID
#undef EXTERN_GUID
#endif  /* EXTERN_GUID */
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) EXTERN_C const IID itf
#include <mscoree.h>

// #using is not used in the Windows build.
#if !defined _CRT_WINDOWS && defined _DEBUG
    #using <system.dll>
#endif

#define _INC_MINTERNAL

#pragma warning(disable:4439)   // C4439: function with a managed parameter must have a __clrcall calling convention
#pragma warning(disable:4483)   // disable warning/error about __identifier(<string>)

#ifdef _M_CEE
    typedef void const* (__clrcall* _PVFVM)(void);
    typedef int         (__clrcall* _PIFVM)(void);
    typedef void        (__clrcall* _CPVFV)(void);
#endif

namespace __identifier("<CrtImplementationDetails>")
{
    using System::AppDomain;
    using System::Object;
    using System::String;
    using System::EventHandler;
    using System::EventArgs;
    using System::Threading::Interlocked;
    using System::SerializableAttribute;
#ifndef _CLR_NETCORE
    using System::CrossAppDomainDelegate;
#endif
    using System::Collections::Stack;
    using System::Threading::Interlocked;
    using System::Runtime::Serialization::SerializationInfo;
    using System::Runtime::Serialization::StreamingContext;

#ifdef _CPP_MSTARTUP
#if defined (_M_CEE_MIXED)
        typedef HRESULT (__stdcall* MixedFunctionType)(void*);
        void __cdecl DoCallBackInDefaultDomain(MixedFunctionType function, void* cookie);
#endif  /* defined (_M_CEE_MIXED) */
    void RegisterModuleUninitializer(EventHandler^ handler);
    void ThrowModuleLoadException(String^ errorMessage);
    void ThrowModuleLoadException(String^ errorMessage, System::Exception^ innerException);
    void ThrowNestedModuleLoadException(System::Exception^ innerException, System::Exception^ nestedException);
#else  /* _CPP_MSTARTUP */
    typedef HRESULT (__stdcall* MixedFunctionType)(void*);
#endif  /* _CPP_MSTARTUP */

#if defined (_DEBUG)
    #define MANAGED_ASSERT( test, message) System::Diagnostics::Debug::Assert(test, message)
#else  /* defined (_DEBUG) */
    #define MANAGED_ASSERT( test, message )
#endif  /* defined (_DEBUG) */

}   // namespace __identifier("<CrtImplementationDetails>")

///////////////////////////////////////////////////////////////////////////////
//
// building msvcm or ptrust*.obj
//

namespace __identifier("<CrtImplementationDetails>")
{

///////////////////////////////////////////////////////////////////////////////
//
// Exception
//

// This is the base class for all managed exceptions thrown by the CRT
[System::SerializableAttribute]
ref class Exception : public System::Exception
{
public:
    Exception(System::String^ message)
        : System::Exception(message)
    {
    }

    Exception(System::String^ message, System::Exception^ innerException)
        : System::Exception(message, innerException)
    {
    }
protected:
    Exception(SerializationInfo^ info, StreamingContext context)
        : System::Exception(info, context)
    {
    }
};

// This is thrown by the CRT during module construction (errors thrown
// by the cctor)
[System::SerializableAttribute]
ref class ModuleLoadException : public System::Exception
{
public:
    ModuleLoadException(System::String^ message)
        : Exception(message)
    {
    }

    ModuleLoadException(System::String^ message, System::Exception^ innerException)
        : Exception(message, innerException)
    {
    }

    literal System::String^ Nested = "A nested exception occurred after the primary exception that caused the C++ module to fail to load.\n";

protected:
    ModuleLoadException(SerializationInfo^ info, StreamingContext context)
        : Exception(info, context)
    {
    }
};

// This is thrown when an exception occurs in the cleanup code executed while handling
// an exception thrown by the module constructor.
[System::SerializableAttribute]
ref class ModuleLoadExceptionHandlerException : public ModuleLoadException
{
private:
    typedef ModuleLoadException base;
    literal System::String^ formatString = "\n{0}: {1}\n--- Start of primary exception ---\n{2}\n--- End of primary exception ---\n\n--- Start of nested exception ---\n{3}\n--- End of nested exception ---\n";

public:

    ModuleLoadExceptionHandlerException(System::String^ message, System::Exception^ innerException, System::Exception^ nestedException)
        : base(message, innerException)
    {
        NestedException = nestedException;
    }

    property System::Exception^ NestedException;

    virtual System::String^ ToString() override
    {
        System::String^ inner = InnerException ? InnerException->ToString() : System::String::Empty;
        System::String^ nested = NestedException ? NestedException->ToString() : System::String::Empty;
        System::String^ result = System::String::Format(
            formatString,
            this->GetType(),
            Message ? Message : System::String::Empty,
            inner ? inner : System::String::Empty,
            nested ? nested : System::String::Empty
            );
        return result;
    }

    _CRT_SECURITYCRITICAL_ATTRIBUTE
    virtual void GetObjectData(SerializationInfo^ info, StreamingContext context) override
    {
        base::GetObjectData(info, context);
        info->AddValue("NestedException", NestedException, System::Exception::typeid);
    }

protected:
    ModuleLoadExceptionHandlerException(SerializationInfo^ info, StreamingContext context)
        : base(info, context)
    {
        NestedException = (System::Exception^)info->GetValue("NestedException", System::Exception::typeid);
    }

};

inline void ThrowNestedModuleLoadException(System::Exception^ innerException, System::Exception^ nestedException)
{
    throw gcnew ModuleLoadExceptionHandlerException(ModuleLoadException::Nested, innerException, nestedException);
}

inline void ThrowModuleLoadException(String^ errorMessage)
{
    throw gcnew ModuleLoadException(errorMessage);
}

inline void ThrowModuleLoadException(String^ errorMessage, System::Exception^ innerException)
{
    throw gcnew ModuleLoadException(errorMessage, innerException);
}

///////////////////////////////////////////////////////////////////////////////
//
// UnInitialize
//

/*
// A single instance of this class is stored in each AppDomain
// It registers a handler for the AppDomain::DomainUnload and AppDomain::ProcessExit events
// and maintains a list of managed C++ modules loaded in that domain. When an unload
// event occurs, it calls back on the event handler registered by each of the modules
// in the reverse order to which they were added to the list.
*/
ref class ModuleUninitializer : public Stack
{
private:
    static Object^ lock = gcnew Object();

internal:
        // Static members are AppDomain specific.
        // Instance for a new AppDomain is created when msvcm's .cctor tries to add its DomainUnload handler
    static ModuleUninitializer^ _ModuleUninitializer = gcnew ModuleUninitializer();

    // Safe for anyone to use because this class just relays the AppDomain::DomainUnload event
    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    void AddHandler(EventHandler^ handler)
    {
#ifndef _PARTIAL_TRUST_OBJ
        bool mustReleaseLock = false;
        System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
        try
        {
            System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
            System::Threading::Monitor::Enter(lock, mustReleaseLock);
#endif  /* _PARTIAL_TRUST_OBJ */
        System::Runtime::CompilerServices::RuntimeHelpers::PrepareDelegate(handler);
        Push(handler);
#ifndef _PARTIAL_TRUST_OBJ
        }
        finally
        {
            if (mustReleaseLock)
            {
                System::Threading::Monitor::Exit(lock);
            }
        }
#endif  /* _PARTIAL_TRUST_OBJ */
    }

private:
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static ModuleUninitializer(){}
    // We've agreed the DomainUnload event is safe to let anyone use
    _CRT_SECURITYSAFECRITICAL_ATTRIBUTE
    ModuleUninitializer()
    {
        // Protected by lock in Initialize
        // singletonHandler will be Prepared as a CER call when it's added to the DomainUnload event:
        // this makes it a CER root
        EventHandler^ singletonHandler = gcnew EventHandler(this, &ModuleUninitializer::SingletonDomainUnload);
        AppDomain::CurrentDomain->DomainUnload += singletonHandler;
        AppDomain::CurrentDomain->ProcessExit  += singletonHandler;
    }

    [System::Runtime::ConstrainedExecution::PrePrepareMethod]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    void SingletonDomainUnload(Object^ source, EventArgs^ arguments)
    {
#ifndef _PARTIAL_TRUST_OBJ
        bool mustReleaseLock = false;
        System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
        try
        {
            System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();
            System::Threading::Monitor::Enter(lock, mustReleaseLock);
#endif  /* _PARTIAL_TRUST_OBJ */
        // Stack iterator starts at top and moves down through stack
        for each (EventHandler^ handler in this)
        {
            handler(source, arguments);
        }
#ifndef _PARTIAL_TRUST_OBJ
        }
        finally
        {
            if (mustReleaseLock)
            {
                System::Threading::Monitor::Exit(lock);
            }
        }
#endif  /* _PARTIAL_TRUST_OBJ */
    }
};

inline void RegisterModuleUninitializer(EventHandler^ handler)
{
    ModuleUninitializer::_ModuleUninitializer->AddHandler(handler);
}

// GUIDs are hardly ever a secret, and memory safety should be ensured by the type system
_CRT_SECURITYSAFECRITICAL_ATTRIBUTE
inline
System::Guid FromGUID(GUID const & guid)
{
   return System::Guid( guid.Data1, guid.Data2, guid.Data3,
                        guid.Data4[ 0 ], guid.Data4[ 1 ],
                        guid.Data4[ 2 ], guid.Data4[ 3 ],
                        guid.Data4[ 4 ], guid.Data4[ 5 ],
                        guid.Data4[ 6 ], guid.Data4[ 7 ] );
}

}   // namespace __identifier("<CrtImplementationDetails>")

#if defined (_M_CEE_MIXED)

_CRT_SECURITYCRITICAL_ATTRIBUTE
inline HRESULT __get_default_appdomain(IUnknown **ppUnk)
{
    using System::Exception;
    using System::Runtime::InteropServices::Marshal;
    using System::Runtime::InteropServices::RuntimeEnvironment;

    HRESULT hr = S_OK;
    ICorRuntimeHost *pHost = NULL;

    try
    {
        // Throws HR exception on failure.
        pHost = reinterpret_cast<ICorRuntimeHost*>(
            RuntimeEnvironment::GetRuntimeInterfaceAsIntPtr(
                __identifier("<CrtImplementationDetails>")::FromGUID(__uuidof(CorRuntimeHost)),
                __identifier("<CrtImplementationDetails>")::FromGUID(__uuidof(ICorRuntimeHost))).ToPointer());
    }
    catch (Exception^ ex)
    {
        hr = (HRESULT)Marshal::GetHRForException(ex);
    }

    if (SUCCEEDED(hr))
    {
        // GetDefaultDomain will not throw.
        hr = pHost->GetDefaultDomain(ppUnk);
        pHost->Release();
    }

    return hr;
}

inline void __release_appdomain(IUnknown *ppUnk)
{
    ppUnk->Release();
}

namespace __identifier("<CrtImplementationDetails>")
{

_CRT_SECURITYCRITICAL_ATTRIBUTE
inline AppDomain^ GetDefaultDomain()
{
    using System::Runtime::InteropServices::Marshal;
    IUnknown* pUnk = NULL;
    HRESULT hresult = __get_default_appdomain(&pUnk);

    if (SUCCEEDED(hresult))
    {
        try
        {
            Object^ pObject = Marshal::GetObjectForIUnknown(System::IntPtr(pUnk));

            AppDomain^ pDomain = (AppDomain^)pObject;

            MANAGED_ASSERT(pDomain->IsDefaultAppDomain(), "Expecting default appdomain");

            return pDomain;
        }
        finally
        {
            __release_appdomain(pUnk);
        }
    }
    else
    {
        Marshal::ThrowExceptionForHR(hresult);
    }

    return nullptr;
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
inline void DoCallBackInDefaultDomain(MixedFunctionType function, void* cookie)
{
    using System::Runtime::InteropServices::Marshal;
    using System::Runtime::InteropServices::RuntimeEnvironment;

    HRESULT hr = S_OK;

    // Throws HR exception on failure.
    ICLRRuntimeHost* pClrHost = nullptr;
    pClrHost = reinterpret_cast<ICLRRuntimeHost*>(
        RuntimeEnvironment::GetRuntimeInterfaceAsIntPtr(
            __identifier("<CrtImplementationDetails>")::FromGUID(__uuidof(CLRRuntimeHost)),
            __identifier("<CrtImplementationDetails>")::FromGUID(__uuidof(ICLRRuntimeHost))).ToPointer());

    try
    {
        AppDomain^ defaultDomain = GetDefaultDomain();

        hr = pClrHost->ExecuteInAppDomain(defaultDomain->Id, function, cookie);
        if (FAILED(hr))
        {
            Marshal::ThrowExceptionForHR(hr);
        }
    }
    finally
    {
        pClrHost->Release();
    }
}

[System::SerializableAttribute]
ref class OpenMPWithMultipleAppdomainsException
    : public System::Exception
{
public:
    OpenMPWithMultipleAppdomainsException() { }
protected:
    OpenMPWithMultipleAppdomainsException(SerializationInfo^ info, StreamingContext context)
        : System::Exception(info, context)
    {
    }
};

// Returns true if the current application is using OpenMP
inline bool IsUsingOpenMP()
{
    HINSTANCE hMod = GetModuleHandleW(L"vcomp120.dll");

    if ( hMod )
    {
        void * fnp = GetProcAddress(hMod, "_vcomp_fork");
        if ( fnp )
        {
            return true;
        }
    }

    hMod = GetModuleHandleW(L"vcomp120d.dll");
    if ( hMod )
    {
        void * fnp = GetProcAddress(hMod, "_vcomp_fork");
        if ( fnp )
        {
            return true;
        }
    }

    return false;
}

#ifndef _PARTIAL_TRUST_OBJ
[System::Diagnostics::DebuggerStepThroughAttribute]
inline void DoDllLanguageSupportValidation()
{
    if (!AppDomain::CurrentDomain->IsDefaultAppDomain() && IsUsingOpenMP())
    {
        throw gcnew OpenMPWithMultipleAppdomainsException();
    }
}
#endif  /* _PARTIAL_TRUST_OBJ */

}   // namespace __identifier("<CrtImplementationDetails>")

#endif  /* defined (_M_CEE_MIXED) */
