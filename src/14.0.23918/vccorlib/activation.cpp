//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
#include "pch.h"

HRESULT __stdcall GetActivationFactoryByPCWSTR(void* str, ::Platform::Guid& riid, void** ppActivationFactory);

namespace __winRT
{
	long __stdcall __getActivationFactoryByPCWSTR(void* str, ::Platform::Guid& pGuid, void** ppActivationFactory)
	{
		return GetActivationFactoryByPCWSTR(str, pGuid, ppActivationFactory);
	}
}
