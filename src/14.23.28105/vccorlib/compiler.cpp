//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"

// Defines all compiler depended data

//Verify the structure sizes and entries for WRL and MoCOM
//Verify factory cache sturcture
static_assert(sizeof(Microsoft::WRL::Details::FactoryCache) == sizeof(__abi___FactoryCache), "Wrong size of FactoryCache/__abi___FactoryCache");
static_assert(offsetof(Microsoft::WRL::Details::FactoryCache, factory) == offsetof(__abi___FactoryCache, __factory), "Wrong offset of factory in CreatorMap/__abi___classObjectEntry");
static_assert(offsetof(Microsoft::WRL::Details::FactoryCache, cookie) == offsetof(__abi___FactoryCache, __cookie), "Wrong offset of cookie in CreatorMap/__abi___classObjectEntry");

//Verify creator
static_assert(sizeof(Microsoft::WRL::Details::CreatorMap) == sizeof(__abi___classObjectEntry), "Wrong size of CreatorMap/__abi___classObjectEntry");
static_assert(offsetof(Microsoft::WRL::Details::CreatorMap, factoryCreator) == offsetof(__abi___classObjectEntry, __factoryCreator), "Wrong offset of factoryCreator in CreatorMap/__abi___classObjectEntry");
static_assert(offsetof(Microsoft::WRL::Details::CreatorMap, activationId) == offsetof(__abi___classObjectEntry, __getRuntimeName), "Wrong offset of getRuntimeName in CreatorMap/__abi___classObjectEntry");
static_assert(offsetof(Microsoft::WRL::Details::CreatorMap, getTrustLevel) == offsetof(__abi___classObjectEntry, __getTrustLevel), "Wrong offset of getTrustLevel in CreatorMap/__abi___classObjectEntry");
static_assert(offsetof(Microsoft::WRL::Details::CreatorMap, factoryCache) == offsetof(__abi___classObjectEntry, __factoryCache), "Wrong offset of factoryCache in CreatorMap/__abi___classObjectEntry");
static_assert(offsetof(Microsoft::WRL::Details::CreatorMap, serverName) == offsetof(__abi___classObjectEntry, __serverName), "Wrong offset of groupId in CreatorMap/__abi___classObjectEntry");

static_assert(sizeof(::Platform::Guid) == sizeof(::_GUID), "Incorect size for Platform::Guid");
static_assert(sizeof(__rcGUID_t) == sizeof(::_GUID), "Incorect size for __rcGUID_t");

HRESULT __stdcall GetIidsFn(int nIids, unsigned long* iidCount, const __s_GUID* pIids, ::Platform::Guid** ppDuplicated);

namespace __winRT
{
	long __stdcall __windowsCreateString(const __wchar_t* sourceString, int length, __abi_HSTRING* string)
	{
		return WindowsCreateString(sourceString, length, (HSTRING*)string);
	}

	long __stdcall __getIids(int nIids, unsigned long* iidCount, const __s_GUID* pIids, ::Platform::Guid** ppDuplicated)
	{
		return GetIidsFn(nIids, iidCount, pIids, ppDuplicated);
	}
}


