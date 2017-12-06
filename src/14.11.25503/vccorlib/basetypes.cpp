//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#include <limits>
#include <algorithm>

namespace default
{
	VCCORLIB_API Platform::String^ int8::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%hhd", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ uint8::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%hhu", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ int16::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%hd", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ uint16::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%hu", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ int32::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%I32d", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ uint32::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%I32u", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ int64::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%I64d", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ uint64::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%I64u", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ float32::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%G", _value);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::String^ float64::ToString()
	{
		wchar_t buf[32];
		swprintf_s(buf, _countof(buf), L"%G", _value);
		return ref new Platform::String(buf);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API Platform::String^ char16::ToString()
	{
		::default::char16 buf[2] = { _value, 0 };
		return ref new Platform::String(buf);
	}

	VCCORLIB_API int32 char16::GetHashCode()
	{
		return static_cast<int>(static_cast<unsigned short>(_value)) | (static_cast<int>(_value) << 16);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 int8::GetHashCode()
	{
		return _value;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 uint8::GetHashCode()
	{
		return static_cast<int>(_value);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 int16::GetHashCode()
	{
		return static_cast<int>(static_cast<unsigned short>(_value)) | (static_cast<int>(_value) << 16);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 uint16::GetHashCode()
	{
		return static_cast<int>(_value);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 int32::GetHashCode()
	{
		return _value;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 uint32::GetHashCode()
	{
		return static_cast<int>(_value);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 int64::GetHashCode()
	{
		return static_cast<int>(_value) ^ static_cast<int>(_value >> 32);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 uint64::GetHashCode()
	{
		return static_cast<int>(_value) ^ static_cast<int>(_value >> 32);
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 float32::GetHashCode()
	{
		if (_value == 0)
		{
			return 0;
		}
		return *(reinterpret_cast<const int*>(&_value));
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32 float64::GetHashCode()
	{
		if (_value == 0)
		{
			return 0;
		}
		__int64 temp_value = *(reinterpret_cast<const __int64*>(&_value));
		return static_cast<int>(temp_value) ^ static_cast<int>(temp_value >> 32);
	}


	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API char16::char16(::default::char16 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ char16::GetType()
	{
		return char16::typeid;
	}

	VCCORLIB_API Platform::Boolean char16::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			char16 other = propertyValue->GetChar16();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int8::int8(signed __int8 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ int8::GetType()
	{
		return int8::typeid;
	}

	VCCORLIB_API Platform::Boolean int8::Equals(Platform::Object^ otherObj)
	{
		/*
		int8 other = safe_cast<::Platform::Box<signed __int8 >^>(otherObj);
		return _value == other._value;
		*/
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API uint8::uint8(unsigned __int8 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ uint8::GetType()
	{
		return uint8::typeid;
	}

	VCCORLIB_API Platform::Boolean uint8::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			uint8 other = propertyValue->GetUInt8();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int16::int16(__int16 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ int16::GetType()
	{
		return int16::typeid;
	}

	VCCORLIB_API Platform::Boolean int16::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			int16 other = propertyValue->GetInt16();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API uint16::uint16(unsigned __int16 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ uint16::GetType()
	{
		return uint16::typeid;
	}

	VCCORLIB_API Platform::Boolean uint16::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			uint16 other = propertyValue->GetUInt16();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int32::int32(int val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ int32::GetType()
	{
		return int32::typeid;
	}

	VCCORLIB_API Platform::Boolean int32::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			int32 other = propertyValue->GetInt32();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API uint32::uint32(unsigned int val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ uint32::GetType()
	{
		return ::default::uint32::typeid;
	}

	VCCORLIB_API Platform::Boolean uint32::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			uint32 other = propertyValue->GetUInt32();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API int64::int64(__int64 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ int64::GetType()
	{
		return default::int64::typeid;
	}

	VCCORLIB_API Platform::Boolean int64::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			int64 other = propertyValue->GetInt64();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API uint64::uint64(unsigned __int64 val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ uint64::GetType()
	{
		return default::uint64::typeid;
	}

	VCCORLIB_API Platform::Boolean uint64::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			uint64 other = propertyValue->GetUInt64();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API float32::float32(float val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ float32::GetType()
	{
		return default::float32::typeid;
	}

	VCCORLIB_API Platform::Boolean float32::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			float32 other = propertyValue->GetSingle();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API float64::float64(double val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ float64::GetType()
	{
		return default::float64::typeid;
	}

	VCCORLIB_API Platform::Boolean float64::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			float64 other = propertyValue->GetDouble();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}

} // namespace default

namespace Platform
{
	////////////////////////////////////////////////////////////////////////////////
	VCCORLIB_API Boolean::Boolean(bool val) : _value(val)
	{
	}

	VCCORLIB_API Platform::Type^ Boolean::GetType()
	{
		return Platform::Boolean::typeid;
	}

	VCCORLIB_API Boolean Boolean::Equals(Platform::Object^ otherObj)
	{
		if (otherObj == nullptr)
		{
			return false;
		}
		try
		{
			Windows::Foundation::IPropertyValue^ propertyValue = safe_cast<Windows::Foundation::IPropertyValue^>(otherObj);
			Boolean other = propertyValue->GetBoolean();
			return _value == other;
		}
		catch(Platform::Exception^)
		{
		}
		return false;
	}


	VCCORLIB_API Platform::String^ Boolean::ToString()
	{
		::default::char16 *buf;
		if (_value)
		{
			buf = L"true";
		}
		else
		{
			buf = L"false";
		}
		return ref new Platform::String(buf);
	}

	VCCORLIB_API int32 Boolean::GetHashCode()
	{
		return (_value) ? 1 : 0;
	}

	VCCORLIB_API Platform::String^ Guid::ToString()
	{
		wchar_t buf[39];
		//{00000000-0000-0000-0000-000000000000}
		swprintf_s(buf, _countof(buf), L"{%08x-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}", __a, __b, __c, __d, __e, __f, __g, __h, __i, __j, __k);
		return ref new Platform::String(buf);
	}

	VCCORLIB_API Platform::Type^ Guid::GetType()
	{
		return Guid::typeid;
	}

	VCCORLIB_API int32 Guid::GetHashCode()
	{
		return this->__a;
	}

} // namespace Platform

namespace Windows { namespace Foundation
{

	VCCORLIB_API Size Size::Empty::get()
	{
		Size size;
		size.Width = -std::numeric_limits<float>::infinity();
		size.Height = -std::numeric_limits<float>::infinity();
		return size;
	}

	VCCORLIB_API Rect::Rect(Point point1, Point point2)
	{
		X = min(point1.X, point2.X);
		Y = min(point1.Y, point2.Y);

		Width = max(max(point1.X, point2.X) - X, 0);
		Height = max(max(point1.Y, point2.Y) - Y, 0);
	}

	VCCORLIB_API Rect Rect::Empty::get()
	{
		Rect rect;
		rect.Width = rect.Height = -std::numeric_limits<float>::infinity();
		rect.X = rect.Y = std::numeric_limits<float>::infinity();
		return rect;
	}

	VCCORLIB_API Rect::Rect(Point location, Size size)
	{
		if (size.IsEmpty)
		{
			Width = Height = -std::numeric_limits<float>::infinity();
			X = Y = std::numeric_limits<float>::infinity();
		}
		else
		{
			X = location.X;
			Y = location.Y;
			Width = size.Width;
			Height = size.Height;
		}
	}

	VCCORLIB_API float Rect::Right::get()
	{
		if (IsEmpty)
		{
			return -std::numeric_limits<float>::infinity();
		}

		return X + Width;
	}
	VCCORLIB_API float Rect::Bottom::get()
	{
		if (IsEmpty)
		{
			return -std::numeric_limits<float>::infinity();
		}

		return Y + Height;
	}

	VCCORLIB_API bool Rect::Contains(Point point)
	{
		return ((point.X >= X) && (point.X - Width <= X) &&
			(point.Y >= Y) && (point.Y - Height <= Y));
	}

	VCCORLIB_API void Rect::Union(Point point)
	{
		Union(Rect(point, point));
	}

	VCCORLIB_API bool Rect::IntersectsWith(Rect rect)
	{
		if (Width < 0 || rect.Width < 0)
		{
			return false;
		}

		return (rect.X <= X + Width) && (rect.X + rect.Width >= X) &&
			(rect.Y <= Y + Height) && (rect.Y + rect.Height >= Y);
	}

	VCCORLIB_API void Rect::Intersect(Rect rect)
	{
		if (!IntersectsWith(rect))
		{
			Width = Height = -1;
		}
		else
		{
			float left = max(X, rect.X);
			float top = max(Y, rect.Y);


			Width = max(min(X + Width, rect.X + rect.Width) - left, 0);
			Height = max(min(Y + Height, rect.Y + rect.Height) - top, 0);

			X = left;
			Y = top;
		}
	}

	VCCORLIB_API void Rect::Union(Rect rect)
	{
		if (IsEmpty)
		{
			X = Y = std::numeric_limits<float>::infinity();
			Width = Height = -std::numeric_limits<float>::infinity();
		}
		else if (!rect.IsEmpty)
		{
			float left = min(Left, rect.Left);
			float top = min(Top, rect.Top);


			// We need this check so that the math does not result in NaN
			if ((rect.Width == -std::numeric_limits<float>::infinity()) || (Width == -std::numeric_limits<float>::infinity()))
			{
				Width = -std::numeric_limits<float>::infinity();
			}
			else
			{
				//  Max with 0 to prevent double weirdness from causing us to be (-epsilon..0)
				float maxRight = max(Right, rect.Right);
				Width = max(maxRight - left, 0);
			}

			// We need this check so that the math does not result in NaN
			if ((rect.Height == -std::numeric_limits<float>::infinity()) || (Height == -std::numeric_limits<float>::infinity()))
			{
				Height = -std::numeric_limits<float>::infinity();
			}
			else
			{
				//  Max with 0 to prevent double weirdness from causing us to be (-epsilon..0)
				float maxBottom = max(Bottom, rect.Bottom);
				Height = max(maxBottom - top, 0);
			}

			X = left;
			Y = top;
		}
	}

} }

namespace Windows { namespace UI { namespace Xaml {

	VCCORLIB_API Duration Duration::operator +(Duration t1, Duration t2)
	{
		if (t1.HasTimeSpan && t2.HasTimeSpan)
		{
			Windows::Foundation::TimeSpan sum;
			sum.Duration = t1.__timeSpan.Duration + t2.__timeSpan.Duration;
			return Duration( sum );
		}
		else if (t1.__durationType != DurationType::Automatic
			&& t2.__durationType != DurationType::Automatic)
		{
			return Duration::Forever;
		}
		else
		{
			// Automatic + anything is Automatic
			return Duration::Automatic;
		}
	}

	VCCORLIB_API Duration Duration::operator -(Duration t1, Duration t2)
	{
		if (t1.HasTimeSpan && t2.HasTimeSpan)
		{
			Windows::Foundation::TimeSpan difference;
			difference.Duration = t1.__timeSpan.Duration - t2.__timeSpan.Duration;
			return Duration(difference);
		}
		else if (t1.__durationType == DurationType::Forever
			&& t2.HasTimeSpan)
		{
			return Duration::Forever;
		}
		else
		{
			return Duration::Automatic;
		}
	}

	VCCORLIB_API bool Duration::operator >(Duration t1, Duration t2)
	{
		if (t1.HasTimeSpan && t2.HasTimeSpan)
		{
			return t1.__timeSpan.Duration > t2.__timeSpan.Duration;
		}
		else if (t1.HasTimeSpan && t2.__durationType == DurationType::Forever)
		{
			return false;
		}
		else if (t1.__durationType == DurationType::Forever && t2.HasTimeSpan)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	VCCORLIB_API bool Duration::operator >=(Duration t1, Duration t2)
	{
		if (t1.__durationType == DurationType::Automatic && t2.__durationType == DurationType::Automatic)
		{
			return true;
		}
		else if (t1.__durationType == DurationType::Automatic || t2.__durationType == DurationType::Automatic)
		{
			return false;
		}
		else
		{
			return !(t1 < t2);
		}
	}

	VCCORLIB_API bool Duration::operator <(Duration t1, Duration t2)
	{
		if (t1.HasTimeSpan && t2.HasTimeSpan)
		{
			return t1.__timeSpan.Duration < t2.__timeSpan.Duration;
		}
		else if (t1.HasTimeSpan && t2.__durationType == DurationType::Forever)
		{
			return true;
		}
		else if (t1.__durationType == DurationType::Forever && t2.HasTimeSpan)
		{
			return false;
		}
		else
		{
			return false;
		}
	}

	VCCORLIB_API bool Duration::operator <=(Duration t1, Duration t2)
	{
		if (t1.__durationType == DurationType::Automatic && t2.__durationType == DurationType::Automatic)
		{
			return true;
		}
		else if (t1.__durationType == DurationType::Automatic || t2.__durationType == DurationType::Automatic)
		{
			return false;
		}
		else
		{
			return !(t1 > t2);
		}
	}

	VCCORLIB_API int Duration::Compare(Duration t1, Duration t2)
	{

		if (t1.__durationType == DurationType::Automatic)
		{
			if (t2.__durationType == DurationType::Automatic)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		else if (t2.__durationType == DurationType::Automatic)
		{
			return 1;
		}
		else
		{
			if (t1 < t2)
			{
				return -1;
			}
			else if (t1 > t2)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}



	namespace Media { namespace Media3D {

		static const double DBL_EPSILON_RELATIVE_1 = 1.1102230246251567e-016; /* smallest such that 1.0+DBL_EPSILON != 1.0 */

		static bool IsZero(double value)
		{
			return std::abs(value) < 10.0 * DBL_EPSILON_RELATIVE_1;
		}

		static bool IsAffine(Matrix3D& matrix)
		{
			return (matrix.M14 == 0.0 && matrix.M24 == 0.0 && matrix.M34 == 0.0 && matrix.M44 == 1.0);
		}

		static bool NormalizedAffineInvert(Matrix3D& matrix)
		{
			double z20 = matrix.M12 * matrix.M23 - matrix.M22 * matrix.M13;
			double z10 = matrix.M32 * matrix.M13 - matrix.M12 * matrix.M33;
			double z00 = matrix.M22 * matrix.M33 - matrix.M32 * matrix.M23;
			double det = matrix.M31 * z20 + matrix.M21 * z10 + matrix.M11 * z00;

			if (IsZero(det))
			{
				return false;
			}

			// Compute 3x3 non-zero cofactors for the 2nd column
			double z21 = matrix.M21 * matrix.M13 - matrix.M11 * matrix.M23;
			double z11 = matrix.M11 * matrix.M33 - matrix.M31 * matrix.M13;
			double z01 = matrix.M31 * matrix.M23 - matrix.M21 * matrix.M33;

			// Compute all six 2x2 determinants of 1st two columns
			double y01 = matrix.M11 * matrix.M22 - matrix.M21 * matrix.M12;
			double y02 = matrix.M11 * matrix.M32 - matrix.M31 * matrix.M12;
			double y03 = matrix.M11 * matrix.OffsetY - matrix.OffsetX * matrix.M12;
			double y12 = matrix.M21 * matrix.M32 - matrix.M31 * matrix.M22;
			double y13 = matrix.M21 * matrix.OffsetY - matrix.OffsetX * matrix.M22;
			double y23 = matrix.M31 * matrix.OffsetY - matrix.OffsetX * matrix.M32;

			// Compute all non-zero and non-one 3x3 cofactors for 2nd
			// two columns
			double z23 = matrix.M23 * y03 - matrix.OffsetZ * y01 - matrix.M13 * y13;
			double z13 = matrix.M13 * y23 - matrix.M33 * y03 + matrix.OffsetZ * y02;
			double z03 = matrix.M33 * y13 - matrix.OffsetZ * y12 - matrix.M23 * y23;
			double z22 = y01;
			double z12 = -y02;
			double z02 = y12;

			double rcp = 1.0 / det;

			// Multiply all 3x3 cofactors by reciprocal & transpose
			matrix.M11 = (z00 * rcp);
			matrix.M12 = (z10 * rcp);
			matrix.M13 = (z20 * rcp);

			matrix.M21 = (z01 * rcp);
			matrix.M22 = (z11 * rcp);
			matrix.M23 = (z21 * rcp);

			matrix.M31 = (z02 * rcp);
			matrix.M32 = (z12 * rcp);
			matrix.M33 = (z22 * rcp);

			matrix.OffsetX = (z03 * rcp);
			matrix.OffsetY = (z13 * rcp);
			matrix.OffsetZ = (z23 * rcp);

			return true;
		}

		// RETURNS true if has inverse & invert was done.  Otherwise returns false & leaves matrix unchanged.
		static bool InvertCore(Matrix3D& matrix)
		{
			if (IsAffine(matrix))
			{
				return NormalizedAffineInvert(matrix);
			}

			// compute all six 2x2 determinants of 2nd two columns
			double y01 = matrix.M13 * matrix.M24 - matrix.M23 * matrix.M14;
			double y02 = matrix.M13 * matrix.M34 - matrix.M33 * matrix.M14;
			double y03 = matrix.M13 * matrix.M44 - matrix.OffsetZ * matrix.M14;
			double y12 = matrix.M23 * matrix.M34 - matrix.M33 * matrix.M24;
			double y13 = matrix.M23 * matrix.M44 - matrix.OffsetZ * matrix.M24;
			double y23 = matrix.M33 * matrix.M44 - matrix.OffsetZ * matrix.M34;

			// Compute 3x3 cofactors for 1st the column
			double z30 = matrix.M22 * y02 - matrix.M32 * y01 - matrix.M12 * y12;
			double z20 = matrix.M12 * y13 - matrix.M22 * y03 + matrix.OffsetY * y01;
			double z10 = matrix.M32 * y03 - matrix.OffsetY * y02 - matrix.M12 * y23;
			double z00 = matrix.M22 * y23 - matrix.M32 * y13 + matrix.OffsetY * y12;

			// Compute 4x4 determinant
			double det = matrix.OffsetX * z30 + matrix.M31 * z20 + matrix.M21 * z10 + matrix.M11 * z00;

			if (IsZero(det))
			{
				return false;
			}

			// Compute 3x3 cofactors for the 2nd column
			double z31 = matrix.M11 * y12 - matrix.M21 * y02 + matrix.M31 * y01;
			double z21 = matrix.M21 * y03 - matrix.OffsetX * y01 - matrix.M11 * y13;
			double z11 = matrix.M11 * y23 - matrix.M31 * y03 + matrix.OffsetX * y02;
			double z01 = matrix.M31 * y13 - matrix.OffsetX * y12 - matrix.M21 * y23;

			// Compute all six 2x2 determinants of 1st two columns
			y01 = matrix.M11 * matrix.M22 - matrix.M21 * matrix.M12;
			y02 = matrix.M11 * matrix.M32 - matrix.M31 * matrix.M12;
			y03 = matrix.M11 * matrix.OffsetY - matrix.OffsetX * matrix.M12;
			y12 = matrix.M21 * matrix.M32 - matrix.M31 * matrix.M22;
			y13 = matrix.M21 * matrix.OffsetY - matrix.OffsetX * matrix.M22;
			y23 = matrix.M31 * matrix.OffsetY - matrix.OffsetX * matrix.M32;

			// Compute all 3x3 cofactors for 2nd two columns
			double z33 = matrix.M13 * y12 - matrix.M23 * y02 + matrix.M33 * y01;
			double z23 = matrix.M23 * y03 - matrix.OffsetZ * y01 - matrix.M13 * y13;
			double z13 = matrix.M13 * y23 - matrix.M33 * y03 + matrix.OffsetZ * y02;
			double z03 = matrix.M33 * y13 - matrix.OffsetZ * y12 - matrix.M23 * y23;
			double z32 = matrix.M24 * y02 - matrix.M34 * y01 - matrix.M14 * y12;
			double z22 = matrix.M14 * y13 - matrix.M24 * y03 + matrix.M44 * y01;
			double z12 = matrix.M34 * y03 - matrix.M44 * y02 - matrix.M14 * y23;
			double z02 = matrix.M24 * y23 - matrix.M34 * y13 + matrix.M44 * y12;

			double rcp = 1.0 / det;

			// matrix.Multiply all 3x3 cofactors by reciprocal & transpose
			matrix.M11 = (z00 * rcp);
			matrix.M12 = (z10 * rcp);
			matrix.M13 = (z20 * rcp);
			matrix.M14 = (z30 * rcp);

			matrix.M21 = (z01 * rcp);
			matrix.M22 = (z11 * rcp);
			matrix.M23 = (z21 * rcp);
			matrix.M24 = (z31 * rcp);

			matrix.M31 = (z02 * rcp);
			matrix.M32 = (z12 * rcp);
			matrix.M33 = (z22 * rcp);
			matrix.M34 = (z32 * rcp);

			matrix.OffsetX = (z03 * rcp);
			matrix.OffsetY = (z13 * rcp);
			matrix.OffsetZ = (z23 * rcp);
			matrix.M44 = (z33 * rcp);

			return true;
		}

		static double GetNormalizedAffineDeterminant(Matrix3D& matrix)
		{
			double z20 = matrix.M12 * matrix.M23 - matrix.M22 * matrix.M13;
			double z10 = matrix.M32 * matrix.M13 - matrix.M12 * matrix.M33;
			double z00 = matrix.M22 * matrix.M33 - matrix.M32 * matrix.M23;

			return matrix.M31 * z20 + matrix.M21 * z10 + matrix.M11 * z00;
		}

		static double Determinant(Matrix3D& matrix)
		{
			if (IsAffine(matrix))
			{
				return GetNormalizedAffineDeterminant(matrix);
			}

			// compute all six 2x2 determinants of 2nd two columns
			double y01 = matrix.M13 * matrix.M24 - matrix.M23 * matrix.M14;
			double y02 = matrix.M13 * matrix.M34 - matrix.M33 * matrix.M14;
			double y03 = matrix.M13 * matrix.M44 - matrix.OffsetZ * matrix.M14;
			double y12 = matrix.M23 * matrix.M34 - matrix.M33 * matrix.M24;
			double y13 = matrix.M23 * matrix.M44 - matrix.OffsetZ * matrix.M24;
			double y23 = matrix.M33 * matrix.M44 - matrix.OffsetZ * matrix.M34;

			// Compute 3x3 cofactors for 1st the column
			double z30 = matrix.M22 * y02 - matrix.M32 * y01 - matrix.M12 * y12;
			double z20 = matrix.M12 * y13 - matrix.M22 * y03 + matrix.OffsetY * y01;
			double z10 = matrix.M32 * y03 - matrix.OffsetY * y02 - matrix.M12 * y23;
			double z00 = matrix.M22 * y23 - matrix.M32 * y13 + matrix.OffsetY * y12;

			return matrix.OffsetX * z30 + matrix.M31 * z20 + matrix.M21 * z10 + matrix.M11 * z00;
		}

		VCCORLIB_API bool Matrix3D::HasInverse::get()
		{
			return !IsZero(Determinant(*this));
		}

		VCCORLIB_API void Matrix3D::Invert()
		{
			if (!InvertCore(*this))
			{
				throw ref new Platform::InvalidArgumentException();
			}
		}

		VCCORLIB_API Matrix3D Matrix3D::operator*(Matrix3D matrix1, Matrix3D matrix2)
		{
			Matrix3D matrix3D;

			matrix3D.M11 = matrix1.M11 * matrix2.M11 +
				matrix1.M12 * matrix2.M21 +
				matrix1.M13 * matrix2.M31 +
				matrix1.M14 * matrix2.OffsetX;
			matrix3D.M12 = matrix1.M11 * matrix2.M12 +
				matrix1.M12 * matrix2.M22 +
				matrix1.M13 * matrix2.M32 +
				matrix1.M14 * matrix2.OffsetY;
			matrix3D.M13 = matrix1.M11 * matrix2.M13 +
				matrix1.M12 * matrix2.M23 +
				matrix1.M13 * matrix2.M33 +
				matrix1.M14 * matrix2.OffsetZ;
			matrix3D.M14 = matrix1.M11 * matrix2.M14 +
				matrix1.M12 * matrix2.M24 +
				matrix1.M13 * matrix2.M34 +
				matrix1.M14 * matrix2.M44;
			matrix3D.M21 = matrix1.M21 * matrix2.M11 +
				matrix1.M22 * matrix2.M21 +
				matrix1.M23 * matrix2.M31 +
				matrix1.M24 * matrix2.OffsetX;
			matrix3D.M22 = matrix1.M21 * matrix2.M12 +
				matrix1.M22 * matrix2.M22 +
				matrix1.M23 * matrix2.M32 +
				matrix1.M24 * matrix2.OffsetY;
			matrix3D.M23 = matrix1.M21 * matrix2.M13 +
				matrix1.M22 * matrix2.M23 +
				matrix1.M23 * matrix2.M33 +
				matrix1.M24 * matrix2.OffsetZ;
			matrix3D.M24 = matrix1.M21 * matrix2.M14 +
				matrix1.M22 * matrix2.M24 +
				matrix1.M23 * matrix2.M34 +
				matrix1.M24 * matrix2.M44;
			matrix3D.M31 = matrix1.M31 * matrix2.M11 +
				matrix1.M32 * matrix2.M21 +
				matrix1.M33 * matrix2.M31 +
				matrix1.M34 * matrix2.OffsetX;
			matrix3D.M32 = matrix1.M31 * matrix2.M12 +
				matrix1.M32 * matrix2.M22 +
				matrix1.M33 * matrix2.M32 +
				matrix1.M34 * matrix2.OffsetY;
			matrix3D.M33 = matrix1.M31 * matrix2.M13 +
				matrix1.M32 * matrix2.M23 +
				matrix1.M33 * matrix2.M33 +
				matrix1.M34 * matrix2.OffsetZ;
			matrix3D.M34 = matrix1.M31 * matrix2.M14 +
				matrix1.M32 * matrix2.M24 +
				matrix1.M33 * matrix2.M34 +
				matrix1.M34 * matrix2.M44;
			matrix3D.OffsetX = matrix1.OffsetX * matrix2.M11 +
				matrix1.OffsetY * matrix2.M21 +
				matrix1.OffsetZ * matrix2.M31 +
				matrix1.M44 * matrix2.OffsetX;
			matrix3D.OffsetY = matrix1.OffsetX * matrix2.M12 +
				matrix1.OffsetY * matrix2.M22 +
				matrix1.OffsetZ * matrix2.M32 +
				matrix1.M44 * matrix2.OffsetY;
			matrix3D.OffsetZ = matrix1.OffsetX * matrix2.M13 +
				matrix1.OffsetY * matrix2.M23 +
				matrix1.OffsetZ * matrix2.M33 +
				matrix1.M44 * matrix2.OffsetZ;
			matrix3D.M44 = matrix1.OffsetX * matrix2.M14 +
				matrix1.OffsetY * matrix2.M24 +
				matrix1.OffsetZ * matrix2.M34 +
				matrix1.M44 * matrix2.M44;

			return matrix3D;

		}

	}

	namespace Animation
	{
		VCCORLIB_API RepeatBehavior::RepeatBehavior(double count)
		{
			if( count == std::numeric_limits<double>::infinity()
				|| count != count /*NaN*/
				|| count <= 0 )
			{
				__abi_WinRTraiseInvalidArgumentException();
			}

			__duration.Duration = 0;
			__count = count;
			__type = RepeatBehaviorType::Count;
		}

	} // Animation
	} // Media

	VCCORLIB_API GridLength::GridLength(double value, Windows::UI::Xaml::GridUnitType type)
	{
		if (value != value /*NaN*/
			|| value == std::numeric_limits<double>::infinity()
			|| value < 0.0 )
		{
			__abi_WinRTraiseInvalidArgumentException();
		}
		if (type != Windows::UI::Xaml::GridUnitType::Auto
			&& type != Windows::UI::Xaml::GridUnitType::Pixel
			&& type != Windows::UI::Xaml::GridUnitType::Star )
		{
			__abi_WinRTraiseInvalidArgumentException();
		}

		__unitValue = (type == Windows::UI::Xaml::GridUnitType::Auto) ? 1.0 : value;
		__unitType = type;
	}
} } }

namespace Platform
{
	CPPCLI_FUNC IntPtr::IntPtr(void* pv)
	{
		_value = pv;
	}

	CPPCLI_FUNC IntPtr::IntPtr(int32 n)
	{
		_value = (void*)n;
	}

	CPPCLI_FUNC IntPtr::operator IntPtr(void* pv)
	{
		return IntPtr(pv);
	}

	CPPCLI_FUNC IntPtr::operator IntPtr(int32 n)
	{
		return IntPtr(n);
	}

	CPPCLI_FUNC IntPtr::operator void*(IntPtr pv)
	{
		return pv._value;
	}

	CPPCLI_FUNC int32 IntPtr::ToInt32()
	{
        //Disable the pointer data truncation warning for 64-bit builds.
        #pragma warning(push)
        #pragma warning(disable: 4302)
        #pragma warning(disable: 4311)
		return (int32) _value;
        #pragma warning(pop)
	}
}
