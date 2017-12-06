//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#include <roapi.h>

namespace Platform  {
	namespace Metadata {

		//Attribute
		VCCORLIB_API Attribute::Attribute()
		{
		}

		VCCORLIB_API int32 Attribute::GetHashCode()
		{
			return 0;
		}

		VCCORLIB_API Platform::String^ Attribute::ToString()
		{
			wchar_t *buf = L"Platform.Attribute";
			return ref new Platform::String(buf);
		}

		VCCORLIB_API bool Attribute::Equals(Platform::Object^ other)
		{
			return dynamic_cast<Platform::Metadata::Attribute^>(other) != nullptr;
		}

	} // Namespace Metadata

	//STAThreadAttribute
	VCCORLIB_API STAThreadAttribute::STAThreadAttribute()
	{
	}

	VCCORLIB_API int32 STAThreadAttribute::GetHashCode()
	{
		return 0; /* RO_INIT_SINGLETHREADED */
	}

	VCCORLIB_API Platform::String^ STAThreadAttribute::ToString()
	{
		wchar_t *buf = L"Platform.STAThreadAttribute";
		return ref new Platform::String(buf);
	}

	VCCORLIB_API bool STAThreadAttribute::Equals(Platform::Object^ other)
	{
		return dynamic_cast<Platform::STAThreadAttribute^>(other) != nullptr;
	}


	//MTAThreadAttribute
	VCCORLIB_API MTAThreadAttribute::MTAThreadAttribute()
	{
	}

	VCCORLIB_API int32 MTAThreadAttribute::GetHashCode()
	{
		return static_cast<int32>(RO_INIT_MULTITHREADED);
	}

	VCCORLIB_API Platform::String^ MTAThreadAttribute::ToString()
	{
		wchar_t *buf = L"Platform::MTAThreadAttribute";
		return ref new Platform::String(buf);
	}

	VCCORLIB_API bool MTAThreadAttribute::Equals(Platform::Object^ other)
	{
		return dynamic_cast<Platform::MTAThreadAttribute^>(other) != nullptr;
	}

	namespace Runtime
	{
		namespace CompilerServices
		{
			//OnePhaseConstructedAttribute
			VCCORLIB_API OnePhaseConstructedAttribute::OnePhaseConstructedAttribute()
			{
			}

			VCCORLIB_API int32 OnePhaseConstructedAttribute::GetHashCode()
			{
				return 1;
			}

			VCCORLIB_API Platform::String^ OnePhaseConstructedAttribute::ToString()
			{
				wchar_t *buf = L"Platform.Runtime.CompilerServices.OnePhaseConstructedAttribute";
				return ref new Platform::String(buf);
			}

			VCCORLIB_API bool OnePhaseConstructedAttribute::Equals(Platform::Object^ other)
			{
				return dynamic_cast<Platform::Runtime::CompilerServices::OnePhaseConstructedAttribute^>(other) != nullptr;
			}
		}
	}

} // namespace Platform
