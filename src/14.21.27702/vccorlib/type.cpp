//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma hdrstop

#pragma warning(disable:4483)

size_t SafeAdd(size_t left, size_t right)
{
	if (static_cast<size_t>(-1) - left < right)
	{
		throw ref new ::Platform::COMException(HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
	}

	return left + right;
}

size_t SafeMultiply(size_t left, size_t right)
{
	if (static_cast<size_t>(-1) / left < right)
	{
		throw ref new ::Platform::COMException(HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
	}

	return left * right;
}

struct {
	const __wchar_t* typeName;
	const __wchar_t* platformName;
	::Platform::TypeCode typeId;
	::Windows::Foundation::PropertyType propertyType;
} primitiveMatchArr[] = {
	{ L"Boolean",  L"Boolean",                      ::Platform::TypeCode::Boolean  , ::Windows::Foundation::PropertyType::Boolean    },
	{ L"Char16",   L"Char16",                       ::Platform::TypeCode::Char16   , ::Windows::Foundation::PropertyType::Char16     },
	{ L"Int8",     L"Int8",                         ::Platform::TypeCode::Int8     , static_cast<::Windows::Foundation::PropertyType>(-1) },
	{ L"UInt8",    L"UInt8",                        ::Platform::TypeCode::UInt8    , ::Windows::Foundation::PropertyType::UInt8      },
	{ L"Int16",    L"Int16",                        ::Platform::TypeCode::Int16    , ::Windows::Foundation::PropertyType::Int16      },
	{ L"UInt16",   L"UInt16",                       ::Platform::TypeCode::UInt16   , ::Windows::Foundation::PropertyType::UInt16     },
	{ L"Int32",    L"Int32",                        ::Platform::TypeCode::Int32    , ::Windows::Foundation::PropertyType::Int32      },
	{ L"UInt32",   L"UInt32",                       ::Platform::TypeCode::UInt32   , ::Windows::Foundation::PropertyType::UInt32     },
	{ L"Int64",    L"Int64",                        ::Platform::TypeCode::Int64    , ::Windows::Foundation::PropertyType::Int64      },
	{ L"UInt64",   L"UInt64",                       ::Platform::TypeCode::UInt64   , ::Windows::Foundation::PropertyType::UInt64     },
	{ L"Single",   L"Single",                       ::Platform::TypeCode::Single   , ::Windows::Foundation::PropertyType::Single     },
	{ L"Double",   L"Double",                       ::Platform::TypeCode::Double   , ::Windows::Foundation::PropertyType::Double     },
	{ L"String",   L"Platform.String",              ::Platform::TypeCode::String   , ::Windows::Foundation::PropertyType::String     },
	{ L"Guid",     L"Platform.Guid",                ::Platform::TypeCode::Guid     , ::Windows::Foundation::PropertyType::Guid       },
	{ L"DateTime", L"Windows.Foundation.DateTime",  ::Platform::TypeCode::DateTime , ::Windows::Foundation::PropertyType::DateTime   },
	{ L"TimeSpan", L"Windows.Foundation.TimeSpan",  ::Platform::TypeCode::TimeSpan , ::Windows::Foundation::PropertyType::TimeSpan   },
	{ L"Point",    L"Windows.Foundation.Point",     ::Platform::TypeCode::Point    , ::Windows::Foundation::PropertyType::Point      },
	{ L"Rect",     L"Windows.Foundation.Rect",      ::Platform::TypeCode::Rect     , ::Windows::Foundation::PropertyType::Rect       },
	{ L"Size",     L"Windows.Foundation.Size",      ::Platform::TypeCode::Size     , ::Windows::Foundation::PropertyType::Size       },
	{ L"Object",   L"Platform.Object",              ::Platform::TypeCode::Object   , ::Windows::Foundation::PropertyType::Inspectable}
};

__abi_type_descriptor* CreateDescriptior(const __wchar_t* typeName, int typeId)
{
	size_t len = ::wcslen(typeName);

	len = SafeAdd(len, 4);

	size_t size = SafeMultiply(len, sizeof(__wchar_t));
	size = SafeAdd(size, sizeof(__abi_type_descriptor));

	__abi_type_descriptor* descriptor = reinterpret_cast<__abi_type_descriptor*>(Platform::Details::Heap::Allocate(size));
	descriptor->__typeName = reinterpret_cast<const __wchar_t*>(descriptor + 1);
	wcscpy_s( const_cast<__wchar_t*>(descriptor->__typeName), len, typeName);
	descriptor->__typeId = typeId;

	return descriptor;
}

namespace Platform
{
	/*
    extern "C" CPPCLI_FUNC void __identifier("?<Dispose>@Type@Platform@@U$AAAXXZ")(Platform::Type^ pThis)
    {
        // no-op. We don't need 'delete Type' to do anything.
    }
	*/

	CPPCLI_FUNC void Type::__identifier("<Dispose>")()
	{
		  if (_owndescriptor)
		  {
	 		 Platform::Details::Heap::Free(_descriptor);
		  }
	}

    // Add this back in when non-virtual private destructors are available.
	// CPPCLI_FUNC void Type::~Type()
	// {
	//	  if (_owndescriptor)
	//	  {
	// 		 Platform::Details::Heap::Free(_descriptor);
	//	  }
	// }

    CPPCLI_FUNC Type::Type(IntPtr descriptor) : _descriptor( (void *)descriptor ), _owndescriptor(false)
	{
	}

	CPPCLI_FUNC Type::Type(Windows::UI::Xaml::Interop::TypeName type) : _descriptor(nullptr), _owndescriptor(true)
	{
		const __wchar_t *typeName = type.Name->Data();
		int typeId = 0;

		switch(type.Kind)
		{
		case Windows::UI::Xaml::Interop::TypeKind::Metadata:
			typeId = static_cast<int>(Platform::TypeCode::Object);
			break;
		case Windows::UI::Xaml::Interop::TypeKind::Custom:
			typeId = static_cast<int>(Platform::TypeCode::Custom);
			break;
		case Windows::UI::Xaml::Interop::TypeKind::Primitive:
			{
				for(unsigned int i = 0; i < _countof(primitiveMatchArr); i++)
				{
					if (::wcscmp(typeName, primitiveMatchArr[i].typeName) == 0)
					{
						typeId = static_cast<int>(primitiveMatchArr[i].typeId);
						typeName = primitiveMatchArr[i].platformName;
						break;
					}
				}
				break;
			}
		default:
			throw ref new ::Platform::FailureException();
		}

		_descriptor = CreateDescriptior(typeName, typeId);
	}

	CPPCLI_FUNC Type::Type(Object^ obj) : _owndescriptor(true)
	{
		Windows::Foundation::IPropertyValue^ propertyValue;
		::Platform::Guid gdIPropertyValue(__uuidof(Windows::Foundation::IPropertyValue^));
		HRESULT hr = obj->__abi_QueryInterface(gdIPropertyValue, reinterpret_cast<void**>(&propertyValue));
		if (SUCCEEDED(hr))
		{
			auto propertyType = propertyValue->Type;

			for (unsigned int i = 0; i < _countof(primitiveMatchArr); i++)
			{
				if (propertyType == primitiveMatchArr[i].propertyType)
				{
					_descriptor = CreateDescriptior(primitiveMatchArr[i].platformName, static_cast<int>(primitiveMatchArr[i].typeId));
					return;
				}
			}
		}

		Platform::String^ typeName;
		hr = obj->__abi_GetRuntimeClassName(reinterpret_cast<HSTRING*>(&typeName));
		__abi_ThrowIfFailed(hr);

		int typeId = static_cast<int>(Platform::TypeCode::Object);

		_descriptor = CreateDescriptior(typeName->Data(), typeId);
	}

	CPPCLI_FUNC String^ Type::FullName::get()
	{
		__abi_type_descriptor* descriptor = (__abi_type_descriptor*)(void*)_descriptor;
		return ref new String(descriptor->__typeName);
	}

	CPPCLI_FUNC TypeCode Type::GetTypeCode(Type^ type)
	{
		__abi_type_descriptor* descriptor = (__abi_type_descriptor*)(void*)type->_descriptor;
		return (TypeCode)descriptor->__typeId;
	}


	CPPCLI_FUNC int32 Type::GetHashCode()
	{
		return Object::GetHashCode();
	}

	CPPCLI_FUNC String^ Type::ToString()
	{
		return FullName;
	}

	CPPCLI_FUNC bool Type::Equals(Object^ o)
	{
		Type^ type = dynamic_cast<Type^>(o);
		if (type == nullptr)
		{
			return this == nullptr;
		}

		return _descriptor == type->_descriptor;
	}

	CPPCLI_FUNC Platform::Type::operator class Windows::UI::Xaml::Interop::TypeName(class Platform::Type^ type)
	{
		Windows::UI::Xaml::Interop::TypeName typeName;
		typeName.Name = type->FullName;

		switch(::Platform::Type::GetTypeCode(type))
		{
		case ::Platform::TypeCode::Empty:
		case ::Platform::TypeCode::Object:
		case ::Platform::TypeCode::DateTime:
		case ::Platform::TypeCode::TimeSpan:
		case ::Platform::TypeCode::Point:
		case ::Platform::TypeCode::Size:
		case ::Platform::TypeCode::Rect:
			typeName.Kind = Windows::UI::Xaml::Interop::TypeKind::Metadata;
			break;
		case ::Platform::TypeCode::Custom:
			typeName.Kind = Windows::UI::Xaml::Interop::TypeKind::Custom;
			break;
		case ::Platform::TypeCode::Boolean:
		case ::Platform::TypeCode::Char16:
		case ::Platform::TypeCode::Int8:
		case ::Platform::TypeCode::UInt8:
		case ::Platform::TypeCode::Int16:
		case ::Platform::TypeCode::UInt16:
		case ::Platform::TypeCode::Int32:
		case ::Platform::TypeCode::UInt32:
		case ::Platform::TypeCode::Int64:
		case ::Platform::TypeCode::UInt64:
		case ::Platform::TypeCode::Single:
		case ::Platform::TypeCode::Double:
		case ::Platform::TypeCode::String:
		case ::Platform::TypeCode::Guid:
			typeName.Kind = Windows::UI::Xaml::Interop::TypeKind::Primitive;
			break;
		}

		// Gettting string name from typeName.Name because we hold reference to it
		const wchar_t *name = typeName.Name->Data();
		if (name != nullptr)
		{
			for(unsigned int i = 0; i < _countof(primitiveMatchArr); i++)
			{
				if (::wcscmp(name, primitiveMatchArr[i].platformName) == 0)
				{
					typeName.Name = ref new ::Platform::String(primitiveMatchArr[i].typeName);
					break;
				}
			}
		}

		return typeName;
	}

	CPPCLI_FUNC Platform::Type::operator class Platform::Type ^(class Windows::UI::Xaml::Interop::TypeName typeName)
	{
		return ref new ::Platform::Type(typeName);
	}

} // namespace Platform

CPPCLI_FUNC Platform::Type^ __stdcall __abi_make_type_id(const __abi_type_descriptor& typeInfo)
{
	Platform::IntPtr ptr = const_cast<__abi_type_descriptor*>(&typeInfo);
	return ref new Platform::Type(ptr);
}
