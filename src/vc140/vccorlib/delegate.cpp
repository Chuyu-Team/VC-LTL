//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"

#pragma hdrstop

namespace Platform
{
	VCCORLIB_API Platform::String^ Delegate::ToString()
	{
		return ref new Platform::String(L"Platform.Delegate");
	}

	VCCORLIB_API Delegate::Delegate()
	{
	}

	VCCORLIB_API int32 Delegate::GetHashCode()
	{
        // Disable pointer data truncation warnings for 64-bit builds.
        #pragma warning(push)
        #pragma warning(disable: 4302)
        #pragma warning(disable: 4311)
		return reinterpret_cast<int32>(reinterpret_cast<void*>(this));
        #pragma warning(pop)
	}

	VCCORLIB_API bool Delegate::Equals(Platform::Object^ other)
	{
		return (void*)this == (void*)other;
	}
}
