//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"

#pragma hdrstop

namespace Platform
{
	template <typename T, typename U>
	bool PropertyValueScalarCallback(Windows::Foundation::IPropertyValue^ value, T callback, U context)
	{
		switch (value->Type)
		{
		case Windows::Foundation::PropertyType::Boolean:
			return callback.Scalar(value->GetBoolean(), context);

		case Windows::Foundation::PropertyType::Char16:
			return callback.Scalar(value->GetChar16(), context);

		case Windows::Foundation::PropertyType::DateTime:
			return callback.Scalar(value->GetDateTime(), context);

		case Windows::Foundation::PropertyType::Double:
			return callback.Scalar(value->GetDouble(), context);

		case Windows::Foundation::PropertyType::Guid:
			return callback.Scalar(value->GetGuid(), context);

		case Windows::Foundation::PropertyType::Int16:
			return callback.Scalar(value->GetInt16(), context);

		case Windows::Foundation::PropertyType::Int32:
			return callback.Scalar(value->GetInt32(), context);

		case Windows::Foundation::PropertyType::Int64:
			return callback.Scalar(value->GetInt64(), context);

		case Windows::Foundation::PropertyType::Point:
			return callback.Scalar(value->GetPoint(), context);

		case Windows::Foundation::PropertyType::Rect:
			return callback.Scalar(value->GetRect(), context);

		case Windows::Foundation::PropertyType::Single:
			return callback.Scalar(value->GetSingle(), context);

		case Windows::Foundation::PropertyType::Size:
			return callback.Scalar(value->GetSize(), context);

		case Windows::Foundation::PropertyType::String:
			return callback.Scalar(value->GetString(), context);

		case Windows::Foundation::PropertyType::TimeSpan:
			return callback.Scalar(value->GetTimeSpan(), context);

		case Windows::Foundation::PropertyType::UInt8:
			return callback.Scalar(value->GetUInt8(), context);

		case Windows::Foundation::PropertyType::UInt16:
			return callback.Scalar(value->GetUInt16(), context);

		case Windows::Foundation::PropertyType::UInt32:
			return callback.Scalar(value->GetUInt32(), context);

		case Windows::Foundation::PropertyType::UInt64:
			return callback.Scalar(value->GetUInt64(), context);

			// Need these here due to:
			// c:\dd\wcmdev\src\vctools\winrt\vccorlib\object.cpp(91): warning C4061: enumerator '...' in switch of enum 'Windows::Foundation::PropertyType'
			// is not explicitly handled by a case label
			// Hope UTC is intelligent enough to optimize these out. But probably wouldn't matter much due to the log(n) search.
		case Windows::Foundation::PropertyType::OtherType:
		case Windows::Foundation::PropertyType::OtherTypeArray:
		case Windows::Foundation::PropertyType::BooleanArray:
		case Windows::Foundation::PropertyType::Char16Array:
		case Windows::Foundation::PropertyType::DateTimeArray:
		case Windows::Foundation::PropertyType::DoubleArray:
		case Windows::Foundation::PropertyType::Empty:
		case Windows::Foundation::PropertyType::GuidArray:
		case Windows::Foundation::PropertyType::Inspectable:
		case Windows::Foundation::PropertyType::InspectableArray:
		case Windows::Foundation::PropertyType::Int16Array:
		case Windows::Foundation::PropertyType::Int32Array:
		case Windows::Foundation::PropertyType::Int64Array:
		case Windows::Foundation::PropertyType::PointArray:
		case Windows::Foundation::PropertyType::RectArray:
		case Windows::Foundation::PropertyType::SingleArray:
		case Windows::Foundation::PropertyType::SizeArray:
		case Windows::Foundation::PropertyType::StringArray:
		case Windows::Foundation::PropertyType::TimeSpanArray:
		case Windows::Foundation::PropertyType::UInt16Array:
		case Windows::Foundation::PropertyType::UInt32Array:
		case Windows::Foundation::PropertyType::UInt64Array:
		case Windows::Foundation::PropertyType::UInt8Array:
			return false;
		}

		return false;
	}

	// The callback class to hand to PropertyValueScalarCallback. This is for handling the
	// ToString implementation of boxed types.
	// 'context' passed to PropertyValueScalarCallback in this case should be a String^* to receive
	// the output string.
	class ToStringCallback
	{
	public:
		template <typename T>
		bool Scalar(T value, String^* toString)
		{
			*toString = value.ToString();
			return true;
		}

		bool Scalar(Windows::Foundation::DateTime, String^*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::Point, String^*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::Rect, String^*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::Size, String^*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::TimeSpan, String^*)
		{
			return false;
		}

		// Groan. Needs to temporarily be here because Platform::Guid is turning into a const during a param,
		// meaning we can't call .ToString() on it, and we have no way to make .ToString() const.
		bool Scalar(Platform::Guid value, String^* toString)
		{
			*toString = const_cast<Platform::Guid&>(value).ToString();
			return true;
		}

		bool Scalar(Platform::String^ s, String^* toString)
		{
			*toString = s;
			return true;
		}
	};

	// The callback class to hand to PropertyValueScalarCallback. This is for handling the
	// GetHashCode implementation of boxed types.
	// 'context' passed to PropertyValueScalarCallback in this case should be a int32* to receive
	// the output hashcode.
	class GetHashCodeCallback
	{
	public:
		template <typename T>
		bool Scalar(T value, int32* hashCode)
		{
			*hashCode = value.GetHashCode();
			return true;
		}

		bool Scalar(Windows::Foundation::DateTime, int32*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::Point, int32*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::Rect, int32*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::Size, int32*)
		{
			return false;
		}

		bool Scalar(Windows::Foundation::TimeSpan, int32*)
		{
			return false;
		}

		bool Scalar(Platform::Guid value, int32* hashCode)
		{
			*hashCode = const_cast<Platform::Guid&>(value).GetHashCode();
			return true;
		}

		bool Scalar(Platform::String^ s, int32* hashCode)
		{
			*hashCode = s->GetHashCode();
			return true;
		}
	};

	// A helper callback class to hand to PropertyValueScalarCallback used from EqualsCallback.
	// Don't call otherwise directly.
	// 'context' passed to PropertyValueScalarCallback in this case should be the value against which
	// the comparison is made (from EqualsCallback)
	class EqualsCallbackHelper
	{
	public:
		bool Scalar(float value1, float value2)
		{
			return value1 == value2;
		}

		bool Scalar(double value1, double value2)
		{
			return value1 == value2;
		}

		template <typename T>
		bool Scalar(T value1, T value2)
		{
			if (0 == memcmp(&value1, &value2, sizeof(value1)))
			{
				return true;
			}

			return false;
		}

		// Shouldn't ever be called because there is a check below that is:
		// if (propertyValue1->Type == propertyValue2->Type)
		// implying the types are the same.
		// But the compiler can't infer that, so it needs to be here.
		template <typename T, typename U>
		bool Scalar(T, U)
		{
			_ASSERT(false);
			return false;
		}
	};

	// The callback class to hand to PropertyValueScalarCallback. This is for handling the
	// GetHashCode implementation of boxed types.
	// 'context' passed to PropertyValueScalarCallback in this case should be the IPropertyValue
	// against which the comparison is made.
	class EqualsCallback
	{
	public:
		template <typename T>
		bool Scalar(T value, Windows::Foundation::IPropertyValue^ pv)
		{
			EqualsCallbackHelper cb;
			return PropertyValueScalarCallback(pv, cb, value);
		}
	};

	CPPCLI_FUNC Object::Object()
	{
	}

	CPPCLI_FUNC int32 Object::GetHashCode()
	{
		__abi_IUnknown* pUnk = reinterpret_cast<__abi_IUnknown*>(this);

		Platform::Details::IEquatable^ equatable;
		::Platform::Guid gdIEquatable(__uuidof(Platform::Details::IEquatable^));
		HRESULT hr = pUnk->__abi_QueryInterface(gdIEquatable, (void**)&equatable);
		if (SUCCEEDED(hr))
		{
			return equatable->GetHashCode();
		}

		Windows::Foundation::IPropertyValue^ propertyValue;
		::Platform::Guid gdIPropertyValue(__uuidof(Windows::Foundation::IPropertyValue^));
		hr = pUnk->__abi_QueryInterface(gdIPropertyValue, (void**)&propertyValue);
		if (SUCCEEDED(hr))
		{
			int32 hash;

			GetHashCodeCallback cb;
			if (PropertyValueScalarCallback(propertyValue, cb, &hash))
			{
				return hash;
			}
		}

		::Microsoft::WRL::ComPtr<IUnknown> pRealUnk;
		::Platform::Guid gdIUnknown(__uuidof(IUnknown));
		hr = pUnk->__abi_QueryInterface(gdIUnknown, (void**)&pRealUnk);
		if (SUCCEEDED(hr))
		{
            //Disable pointer data truncation warnings for 64-bit builds.
            #pragma warning(push)
            #pragma warning(disable: 4302)
            #pragma warning(disable: 4311)
			return (int32)pRealUnk.Get();
            #pragma warning(pop)
		}

		return 0;
	}

	CPPCLI_FUNC Type^ Object::GetType()
	{
		return ref new Type(this);
	}

	CPPCLI_FUNC bool Object::Equals(Object^ o)
	{
		__abi_IUnknown* pUnkDerived1 = reinterpret_cast<__abi_IUnknown*>(this);
		__abi_IUnknown* pUnkDerived2 = reinterpret_cast<__abi_IUnknown*>(o);

		if (pUnkDerived1 == nullptr)
		{
			throw ref new ::Platform::NullReferenceException();
		}

		Platform::Details::IEquatable^ equatable1;
		::Platform::Guid gdIEquatable(__uuidof(Platform::Details::IEquatable^));
		HRESULT hr = pUnkDerived1->__abi_QueryInterface(gdIEquatable, (void**)&equatable1);
		if (SUCCEEDED(hr))
		{
			return equatable1->Equals(o);
		}

		if (pUnkDerived2 != nullptr)
		{
			Windows::Foundation::IPropertyValue^ propertyValue1;
			::Platform::Guid gdIPropertyValue(__uuidof(Windows::Foundation::IPropertyValue^));
			hr = pUnkDerived1->__abi_QueryInterface(gdIPropertyValue, (void**)&propertyValue1);
			if (SUCCEEDED(hr))
			{
				Windows::Foundation::IPropertyValue^ propertyValue2;
				hr = pUnkDerived2->__abi_QueryInterface(gdIPropertyValue, (void**)&propertyValue2);
				if (SUCCEEDED(hr))
				{
					if (propertyValue1->Type == propertyValue2->Type)
					{
						EqualsCallback cb;
						if (PropertyValueScalarCallback(propertyValue1, cb, propertyValue2))
						{
							return true;
						}
					}
				}
			}

			::Platform::Guid gdIUnknown(__uuidof(IUnknown));
			::Microsoft::WRL::ComPtr<IUnknown> pUnk1;
			::Microsoft::WRL::ComPtr<IUnknown> pUnk2;

			hr = pUnkDerived1->__abi_QueryInterface(gdIUnknown, (void**)&pUnk1);
			if (SUCCEEDED(hr))
			{
				hr = pUnkDerived2->__abi_QueryInterface(gdIUnknown, (void**)&pUnk2);
				if (SUCCEEDED(hr))
				{
					return pUnk1 == pUnk2;
				}
			}
		}

		return false;
	}

	CPPCLI_FUNC bool Object::ReferenceEquals(Object^ o1, Object^ o2)
	{
		return o1 == o2;
	}

	CPPCLI_FUNC bool Object::ReferenceEquals(String^ o1, String^ o2)
	{
		return o1 == o2;
	}

	CPPCLI_FUNC Enum::Enum()
	{
	}

	CPPCLI_FUNC ValueType::ValueType()
	{
	}

	CPPCLI_FUNC int32 ValueType::GetHashCode()
	{
		return static_cast<::Platform::Object^>(this)->GetHashCode();
	}

	CPPCLI_FUNC String^ ValueType::ToString()
	{
		return static_cast<::Platform::Object^>(this)->ToString();
	}

	CPPCLI_FUNC bool ValueType::Equals(::Platform::Object^ otherObj)
	{
		return static_cast<::Platform::Object^>(this)->Equals(otherObj);
	}

	CPPCLI_FUNC int32 Enum::GetHashCode()
	{
		return static_cast<::Platform::Object^>(this)->GetHashCode();
	}

	CPPCLI_FUNC String^ Enum::ToString()
	{
		return static_cast<::Platform::Object^>(this)->ToString();
	}

	CPPCLI_FUNC bool Enum::Equals(::Platform::Object^ otherObj)
	{
		return static_cast<::Platform::Object^>(this)->Equals(otherObj);
	}
}

// WINRT TODO: Update hardcoded GUID to __uuidof(Windows::Foundation::IStringable) when we get an updated SDK in VS.
::Platform::Guid gdIStringable(0x96369f54, 0x8eb6, 0x48f0, 0xab, 0xce, 0xc1, 0xb2, 0x11, 0xe6, 0x27, 0xc3);

namespace __abi_details
{
	CPPCLI_FUNC Platform::String^ __stdcall __abi_ObjectToString(Platform::Object^ o, bool useIPrintable)
	{
		if (o == nullptr)
		{
			return nullptr;
		}

		__abi_IUnknown* pUnk = reinterpret_cast<__abi_IUnknown*>(o);

		HRESULT hr;

		if (useIPrintable)
		{
			Platform::Details::IPrintable^ printable;

			hr = pUnk->__abi_QueryInterface(gdIStringable, (void**)&printable);
			if (SUCCEEDED(hr))
			{
				return printable->ToString();
			}

			::Platform::Guid gdIPrintable(__uuidof(Platform::Details::IPrintable^));
			hr = pUnk->__abi_QueryInterface(gdIPrintable, (void**)&printable);
			if (SUCCEEDED(hr))
			{
				return printable->ToString();
			}
		}

		Windows::Foundation::IPropertyValue^ propertyValue;
		::Platform::Guid gdIPropertyValue(__uuidof(Windows::Foundation::IPropertyValue^));
		hr = pUnk->__abi_QueryInterface(gdIPropertyValue, (void**)&propertyValue);
		if (SUCCEEDED(hr))
		{
			Platform::String^ s;

			Platform::ToStringCallback cb;
			if (Platform::PropertyValueScalarCallback(propertyValue, cb, &s))
			{
				return s;
			}
		}

		::Platform::Guid gdIInspectable(__uuidof(IInspectable));
		::Microsoft::WRL::ComPtr<IInspectable> pInspectable;
		hr = pUnk->__abi_QueryInterface(gdIInspectable, (void**)&pInspectable);
		if (SUCCEEDED(hr))
		{
			Platform::String^ str;
			pInspectable->GetRuntimeClassName(reinterpret_cast<HSTRING*>(&str));
			return str;
		}

		return nullptr;
	}
}
