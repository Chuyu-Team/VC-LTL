//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "activation.h"
#pragma hdrstop
#include <strsafe.h>
#include <memory>
#include <combaseapi.h>

using namespace Microsoft::WRL;
using namespace std;

struct conditional_deleter
{
    conditional_deleter(bool bDelete = true) : _bDelete(bDelete)
    {
    }
    conditional_deleter(const conditional_deleter& other) : _bDelete(other._bDelete)
    {
    }
    conditional_deleter(conditional_deleter&& other) : _bDelete(other._bDelete)
    {
        other._bDelete = true;
    }
    conditional_deleter& operator=(conditional_deleter other)
    {
        _bDelete = other._bDelete;
    }
    void operator()(const wchar_t* p)
    {
        if (_bDelete)
        {
            delete p; // TRANSITION, should be delete[]
        }
        return;
    }
private:
    bool _bDelete;
};

template <typename deletor>
struct hash_unique_wchar
{
    size_t operator()(const unique_ptr<const wchar_t, deletor>& ele) const
    {
        return stdext::hash_value(reinterpret_cast<const wchar_t*>(ele.get())); // TRANSITION, should be hash<string_view>
    }
};

template <typename deletor>
struct equal_unique_wchar
{
    bool operator()(const unique_ptr<const wchar_t, deletor>& first, const unique_ptr<const wchar_t, deletor>& second) const
    {
        return wcscmp(first.get(), second.get()) == 0;
    }
};

CPPCLI_FUNC HRESULT __stdcall __getActivationFactoryByHSTRING(HSTRING str, ::Platform::Guid& riid, PVOID * ppActivationFactory)
{
    HRESULT hr = S_OK;
    IActivationFactory* pActivationFactory;
    hr = Windows::Foundation::GetActivationFactory(str, &pActivationFactory);
    if (SUCCEEDED(hr))
    {
        hr = pActivationFactory->QueryInterface(reinterpret_cast<REFIID>(riid), ppActivationFactory);
        pActivationFactory->Release();
    }

    return hr;
}

class PerApartmentFactoryCache : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IApartmentShutdown, FtmBase>
{
    UINT64 _apartmentId;
protected:
    typedef unique_ptr<const wchar_t, conditional_deleter> ActivatableID; // TRANSITION, should be const wchar_t[]
private:
    unordered_map<ActivatableID,
        ComPtr<IUnknown>,
            hash_unique_wchar<conditional_deleter>,
                equal_unique_wchar<conditional_deleter >> _factoryCache;
protected:
    virtual HRESULT AddFactoryInternal(std::pair<ActivatableID, ComPtr<IUnknown> > &&entry)
    {
        auto ret = _factoryCache.insert(std::move(entry));
        if (ret.second == false)
        {
            return S_FALSE;
        }

        return S_OK;
    }

    virtual HRESULT GetFactoryInternal(const wchar_t* acid, Platform::Guid& iid, void** pFactory, bool& addToCache)
    {
        __WRL_ASSERT__(addToCache == true); // The default value for addToCache should be true

        *pFactory = nullptr;
        // Don't delete buffer
        ActivatableID activatableID(acid, false);
        auto it = _factoryCache.find(std::move(activatableID));
        if (it == _factoryCache.end())
        {
            return E_FAIL;
        }

        // The factory pointer is nullptr only
        // in case that the factory has proxy
        IUnknown* unk = it->second.Get();
        if (unk == nullptr)
        {
            addToCache = false;
            return E_FAIL;
        }

        return unk->QueryInterface(iid, pFactory);
    }
public:
    HRESULT RuntimeClassInitialize(UINT64 appartmentId)
    {
        _apartmentId = appartmentId;
        return S_OK;
    }

    HRESULT AddFactory(const wchar_t* acid, IUnknown *factory)
    {
        try
        {
            auto len = wcslen(acid) + 1;
            wchar_t* acidCopy = new wchar_t[len];
            wcscpy_s(acidCopy, len, acid);
            ActivatableID activatableID(acidCopy);

            ComPtr<IMultiQI> multiQI;
			// In case of OOP component there is possibility of RPC errors
			// that can result in cache poisoning as there is no easy way
			// to detect that the factory encounterd RPC errors.
			// The QI for IMultiQI allows us to identify the proxy stub for OOP component
            if (factory->QueryInterface(__uuidof(::IMultiQI), &multiQI) == S_OK)
            {
                // Insert empty marker to indicate that it's out of proc factory
                return AddFactoryInternal(make_pair(std::move(activatableID), nullptr));
            }
            else
            {
                ComPtr<IUnknown> unk = factory;
                return AddFactoryInternal(make_pair(std::move(activatableID), std::move(unk)));
            }
        }
        catch (const bad_alloc&)
        {
            return E_OUTOFMEMORY;
        }
        catch (const exception&)
        {
            return E_FAIL;
        }
    }

    // addToCache parameter prevents from adding the factory into the cache
    // this is used for MTA appartment to reduce contention
    HRESULT GetFactory(const wchar_t* acid, Platform::Guid& iid, void** pFactory, bool& addToCache)
    {
        *pFactory = nullptr;
        try
        {
            return GetFactoryInternal(acid, iid, pFactory, addToCache);
        }
        catch (const bad_alloc&)
        {
            return E_OUTOFMEMORY;
        }
    }

    void __stdcall OnUninitialize(UINT64 apartmentIdentifier);
};

static bool IsMTA()
{
    APTTYPE AptType;
    APTTYPEQUALIFIER AptQualifier;
    HRESULT hr = CoGetApartmentType(&AptType, &AptQualifier);
    if (SUCCEEDED(hr) && (AptType == APTTYPE::APTTYPE_MTA || AptType == APTTYPE::APTTYPE_NA))
    {
        return true;
    }
    else
    {
        return false;
    }
}

class PerApartmentFactoryCacheMTA : public PerApartmentFactoryCache
{
    Wrappers::SRWLock _SRWLock;
protected:
    virtual HRESULT AddFactoryInternal(std::pair<ActivatableID, ComPtr<IUnknown> > &&entry)
    {
        __WRL_ASSERT__(IsMTA());

        // Lock exclusive on addition so we always make sure that we cache the value
        auto lock = _SRWLock.LockExclusive();
        return PerApartmentFactoryCache::AddFactoryInternal(std::move(entry));
    }

    virtual HRESULT GetFactoryInternal(const wchar_t* acid, Platform::Guid& iid, void** pFactory, bool& addToCache)
    {
        __WRL_ASSERT__(IsMTA());

        // If we cannot access cache because of addition or deletion
        // vccorlib will call windows to create new factory
        auto lock = _SRWLock.TryLockShared();
        if (!lock.IsLocked())
        {
            // The lock couldn't be taken so there must be either addition or removal happening
            // in this case we will not try to put the factory to the cache
            addToCache = false;
            return E_FAIL;
        }

        return PerApartmentFactoryCache::GetFactoryInternal(acid, iid, pFactory, addToCache);
    }
};

static HRESULT CreateFactoryCache(PerApartmentFactoryCache** pFactory, UINT64 apartmentID)
{
    if (IsMTA())
    {
        return MakeAndInitialize<PerApartmentFactoryCacheMTA>(reinterpret_cast<PerApartmentFactoryCacheMTA**>(pFactory), apartmentID);
    }
    else
    {
        return MakeAndInitialize<PerApartmentFactoryCache>(pFactory, apartmentID);
    }
}

class FactoryCache
{
    vector<pair<UINT64, pair<APARTMENT_SHUTDOWN_REGISTRATION_COOKIE, ComPtr<PerApartmentFactoryCache>>>> perApartmentCache;
    Wrappers::CriticalSection _criticalSection;
    static volatile long _cacheEnabled;
    static volatile long _cacheDestroyed;
public:
    static void Enable()
    {
        ::_InterlockedCompareExchange(&_cacheEnabled, 1, 0);
    }
    static void Disable()
    {
        ::_InterlockedCompareExchange(&_cacheEnabled, 0, 1);
    }
    static bool IsEnabled()
    {
        return (_cacheEnabled == 1);
    }
    static bool IsDestroyed()
    {
        return (_cacheDestroyed != 0);
    }

    ~FactoryCache()
    {
        Disable();
        ::_InterlockedIncrement(&_cacheDestroyed);
        Flush();
    }
    void Flush()
    {
        auto lock  = _criticalSection.Lock();
        for(auto it = perApartmentCache.begin(); it != perApartmentCache.end(); it++)
        {
            ::RoUnregisterForApartmentShutdown(it->second.first);
            // Leak all per apartment factory cache as during shutdown
            // we may try to release the factories in the wrong context
            it->second.second.Detach();
        }
        perApartmentCache.clear();
    }
    HRESULT GetFactory(LPCWSTR acid, Platform::Guid& iid, void** pFactory)
    {
        UINT64 apartmentID;
        bool addToCache = true;

        HRESULT hr = ::RoGetApartmentIdentifier(&apartmentID);
        ComPtr<PerApartmentFactoryCache> apartmentCache;
        if (SUCCEEDED(hr) && IsEnabled())
        {
            auto lock  = _criticalSection.Lock();
            for(auto it = perApartmentCache.begin(); it != perApartmentCache.end(); it++)
            {
                if (it->first == apartmentID)
                {
                    apartmentCache = it->second.second;
                    lock.Unlock();
                    hr = apartmentCache->GetFactory(acid, iid, pFactory, addToCache);
                    if (SUCCEEDED(hr))
                    {
                        return hr;
                    }
                    break;
                }
            }
            if (apartmentCache == nullptr)
            {
                hr = CreateFactoryCache(&apartmentCache, apartmentID);
                if (SUCCEEDED(hr))
                {
                    UINT64 regAppartmentId;
                    APARTMENT_SHUTDOWN_REGISTRATION_COOKIE regCookie;
                    hr = ::RoRegisterForApartmentShutdown(apartmentCache.Get(), &regAppartmentId, &regCookie);
                    if (SUCCEEDED(hr))
                    {
                        __WRL_ASSERT__(regAppartmentId == apartmentID);
                        perApartmentCache.push_back(pair<UINT64,
                            pair<APARTMENT_SHUTDOWN_REGISTRATION_COOKIE, ComPtr<PerApartmentFactoryCache>>>
                                (apartmentID, pair<APARTMENT_SHUTDOWN_REGISTRATION_COOKIE, ComPtr<PerApartmentFactoryCache>>(regCookie, apartmentCache.Get())));
                    }
                }
            }
        }

        // Create Factory
        HSTRING className;
        HSTRING_HEADER classNameHeader;
        hr = ::WindowsCreateStringReference(acid, static_cast<UINT32>(wcslen(acid)), &classNameHeader, &className);
        if (FAILED(hr))
        {
            return hr;
        }

        ComPtr<IUnknown> factory;
        Platform::Guid riidUnknown(__uuidof(IUnknown));
        hr = __getActivationFactoryByHSTRING(className, riidUnknown, &factory);

        ::WindowsDeleteString(className);

        if (FAILED(hr))
        {
            return hr;
        }

        if (apartmentCache != nullptr && addToCache)
        {
            apartmentCache->AddFactory(acid, factory.Get());
        }

        return factory.CopyTo(iid, pFactory);
    }

    void RemoveApartmentCache(UINT64 apartmentIdentifier)
    {
        auto lock  = _criticalSection.Lock();

        for(auto it = perApartmentCache.begin(); it != perApartmentCache.end(); it++)
        {
            if (it->first == apartmentIdentifier)
            {
                perApartmentCache.erase(it);
                break;
            }
        }
    }
};

volatile long FactoryCache::_cacheEnabled = 0;
volatile long FactoryCache::_cacheDestroyed = 0;
FactoryCache g_FactoryCache;

void __stdcall PerApartmentFactoryCache::OnUninitialize(UINT64 apartmentIdentifier)
{
    if (apartmentIdentifier == _apartmentId)
    {
        g_FactoryCache.RemoveApartmentCache(apartmentIdentifier);
    }
}

CPPCLI_FUNC void EnableFactoryCache()
{
    FactoryCache::Enable();
}

void DisableFactoryCache()
{
    FactoryCache::Disable();
}

CPPCLI_FUNC void __stdcall FlushFactoryCache()
{
    if (!FactoryCache::IsDestroyed())
    {
        g_FactoryCache.Flush();
    }
}

CPPCLI_FUNC HRESULT __stdcall GetActivationFactoryByPCWSTR(void* str, ::Platform::Guid& riid, void** ppActivationFactory)
{
    wchar_t* acid =  static_cast<wchar_t*>(str);
    if (!FactoryCache::IsEnabled())
    {
        HSTRING className;
        HSTRING_HEADER classNameHeader;
        HRESULT hr = ::WindowsCreateStringReference(acid, static_cast<UINT32>(wcslen(acid)), &classNameHeader, &className);
        if (SUCCEEDED(hr))
        {
            hr = __getActivationFactoryByHSTRING(className, riid, ppActivationFactory);
            ::WindowsDeleteString(className);
        }
        return hr;
    }
    return g_FactoryCache.GetFactory(acid, riid, ppActivationFactory);
}

CPPCLI_FUNC HRESULT __stdcall GetIidsFn(int nIids, unsigned long* iidCount, const __s_GUID* pIids, ::Platform::Guid** ppDuplicated)
{
    int nBytes = nIids * sizeof(::Platform::Guid);

    *ppDuplicated = (::Platform::Guid*)CoTaskMemAlloc(nBytes);
    if (*ppDuplicated)
    {
        memcpy(*ppDuplicated, pIids, nBytes);
        *iidCount = nIids;
        return S_OK;
    }

    *iidCount = 0;
    return E_OUTOFMEMORY;
}

#include "compiler.cpp"
#include "activation.cpp"


#pragma warning( disable: 4073 )  // initializers put in library initialization area
#pragma init_seg( lib )

#include "ehdata.h"
extern "C" void __cdecl _SetWinRTOutOfMemoryExceptionCallback(PGETWINRT_OOM_EXCEPTION pCallback);

namespace Platform { namespace Details {
    extern bool  __abi_firstAlloc;
    extern bool  __abi_is_global_oom_init;
    extern void* __abi_oom_controlblock;
    extern IUnknown* __abi_oom_singleton;
} }

void* __stdcall GetOutOfMemoryExceptionCallback()
{
    Platform::Details::__abi_oom_singleton->AddRef();
    return Platform::Details::__abi_oom_singleton;
}

class CInitExceptions
{
public:
    CInitExceptions()
    {
        // Ignore the effect on the ref new below on __abi_firstAlloc.
        bool wasFirstAlloc = Platform::Details::__abi_firstAlloc;

        Platform::Details::__abi_is_global_oom_init = true;

        // Would take down the process if it throws. This is fine.
        Platform::OutOfMemoryException^ oom = ref new Platform::OutOfMemoryException();

        Platform::Details::__abi_is_global_oom_init = false;

        // Recover firstalloc, so that user can set TrackingLevel in main.
        Platform::Details::__abi_firstAlloc = wasFirstAlloc;

        Platform::Details::__abi_oom_singleton = reinterpret_cast<IUnknown*>(oom);
        Platform::Details::__abi_oom_singleton->AddRef();

        _SetWinRTOutOfMemoryExceptionCallback(GetOutOfMemoryExceptionCallback);
    }

    ~CInitExceptions()
    {
        _SetWinRTOutOfMemoryExceptionCallback(nullptr);
        Platform::Details::__abi_oom_singleton->Release();
    }
};

CInitExceptions initExceptions;
