// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// CSTRINGT.H - Framework-independent, templateable string class

#ifndef __CSTRINGT_H__
#define __CSTRINGT_H__

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#pragma warning(disable : 4668)	// is not defined as a preprocessor macro, replacing with '0' for '#if/#elif
#pragma warning(disable : 4820)	// padding added after member
#pragma warning(disable : 4127)  // conditional expression constant
#endif //!_ATL_NO_PRAGMA_WARNINGS

#pragma warning(disable:4786)	// avoid 255-character limit warnings

#ifdef _MANAGED
#include <vcclr.h>  // For PtrToStringChars
#endif

#include <atlsimpstr.h>
#include <stddef.h>

#ifndef _INC_NEW
#include <new.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include <limits.h>
#include <mbstring.h>

#ifdef _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define CSTRING_EXPLICIT explicit
#else
#define CSTRING_EXPLICIT
#endif

#include <atlconv.h>
#include <atlmem.h>

struct tagVARIANT;
typedef tagVARIANT VARIANT;

#ifndef _AFX
#define _AFX_FUNCNAME(_Name) _Name
#endif

#pragma push_macro("new")
#undef new

#if defined(_AFX)
#pragma push_macro("FormatMessage")
#undef FormatMessage
#pragma push_macro("GetEnvironmentVariable")
#undef GetEnvironmentVariable
#endif

/////////////////////////////////////////////////////////////////////////////
// Naming conventions:
//    The term "length" can be confusing when dealing with ANSI, Unicode, and
//    MBCS character sets, so this file will use the following naming
//    conventions to differentiate between the different meanings of
//    "length":
//
//    'Byte Length' - Length of a buffer in bytes, regardless of character
//       size
//    'Char Length' - Number of distinct characters in string.  For wide-
//       character strings, this is equivalent to half the 'Byte Length'.
//       For ANSI strings, this is equivalent to the 'Byte Length'.  For MBCS
//       strings, 'Char Length' counts a lead-byte/trail-byte combination
//       as one character.
//    'Length' - When neither of the above terms is used, 'Length' refers to
//       length in XCHARs, which is equal to 'Byte Length'/sizeof(XCHAR).
/////////////////////////////////////////////////////////////////////////////

namespace ATL
{

// This struct have the same memory layout as CString and is used to enable
// const statically initialized CString without making a copy on the heap
template <class StringType,int t_nChars> struct CConstFixedStringT
{
	CStringData m_data;
	typename StringType::XCHAR m_achData[t_nChars];
};

#define IMPLEMENT_CONST_STRING_PTR(StringType, value, name) const CConstFixedStringT<StringType, sizeof(value)/sizeof(StringType::XCHAR)> _init##name ={ 	{NULL,		sizeof(value)/sizeof(StringType::XCHAR)-1, 	sizeof(value)/sizeof(StringType::XCHAR)-1, 	-1},			value	};	const StringType::XCHAR* const _value##name = _init##name.m_achData;	extern const StringType* const name = CONST_STRING_PTR(StringType, name);
#define DECLARE_CONST_STRING_PTR(StringType, name) extern const StringType* const name;
#define CONST_STRING_PTR(StringType, name) reinterpret_cast<const StringType* const>(&_value##name)

/////////////////////////////////////////////////////////////////////////////
// inline helpers

_Success_(return != 0)  
_When_(count != 0, _Post_equal_to_(_String_length_(mbstr)+1))  
inline int _wcstombsz(
	_Out_writes_(count) char* mbstr,
	_In_z_ const wchar_t* wcstr,
	_In_ ULONG count) throw()
{
	// count is number of bytes
	if (count == 0 && mbstr != NULL)
		return 0;

	int result = ::WideCharToMultiByte(_AtlGetConversionACP(), 0, wcstr, -1,
		mbstr, count, NULL, NULL);
	ATLASSERT(mbstr == NULL || result <= (int)count);
	return result;
}

_Success_(return != 0)  
_When_(count != 0, _Post_equal_to_(_String_length_(wcstr)+1))  
#pragma warning(suppress: 28196) // Return value = string_length + 1 (terminating null character)
inline int _mbstowcsz(
	_Out_writes_(count) wchar_t* wcstr,
	_In_z_ const char* mbstr,
	_In_ ULONG count)
{
	// count is number of wchar_t's
	if (count == 0 && wcstr != NULL)
		return 0;

	int result = ::MultiByteToWideChar(_AtlGetConversionACP(), 0, mbstr, -1,
		wcstr, count);
	ATLENSURE(wcstr == NULL || result <= (int)count);
	if ((result > 0) && (wcstr != NULL))
	{
		wcstr[result-1] = 0;
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////////
//
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
#pragma warning (push)
#pragma warning (disable: 6103)    

template< typename _CharType = char >
class ChTraitsCRT :
	public ChTraitsBase< _CharType >
{
public:
	static char* __cdecl CharNext(_In_ const char* p) throw()
	{
		return reinterpret_cast< char* >( _mbsinc( reinterpret_cast< const unsigned char* >( p ) ) );
	}

	static int __cdecl IsDigit(_In_ char ch) throw()
	{
		return _ismbcdigit( ch );
	}

	static int __cdecl IsSpace(_In_ char ch) throw()
	{
		return _ismbcspace( ch );
	}

	static int __cdecl StringCompare(
		_In_z_ LPCSTR pszA,
		_In_z_ LPCSTR pszB) throw()
	{
		return _mbscmp( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
	}

	static int __cdecl StringCompareIgnore(
		_In_z_ LPCSTR pszA,
		_In_z_ LPCSTR pszB) throw()
	{
		return _mbsicmp( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
	}

	static int __cdecl StringCollate(
		_In_z_ LPCSTR pszA,
		_In_z_ LPCSTR pszB) throw()
	{
		return _mbscoll( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
	}

	static int __cdecl StringCollateIgnore(
		_In_z_ LPCSTR pszA,
		_In_z_ LPCSTR pszB) throw()
	{
		return _mbsicoll( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
	}

	static LPCSTR __cdecl StringFindString(
		_In_z_ LPCSTR pszBlock,
		_In_z_ LPCSTR pszMatch) throw()
	{
		return reinterpret_cast< LPCSTR >( _mbsstr( reinterpret_cast< const unsigned char* >( pszBlock ),
			reinterpret_cast< const unsigned char* >( pszMatch ) ) );
	}

	static LPSTR __cdecl StringFindString(
		_In_z_ LPSTR pszBlock,
		_In_z_ LPCSTR pszMatch) throw()
	{
		return( const_cast< LPSTR >( StringFindString( const_cast< LPCSTR >( pszBlock ), pszMatch ) ) );
	}

	static LPCSTR __cdecl StringFindChar(
		_In_z_ LPCSTR pszBlock,
		_In_ char chMatch) throw()
	{
		return reinterpret_cast< LPCSTR >( _mbschr( reinterpret_cast< const unsigned char* >( pszBlock ), (unsigned char)chMatch ) );
	}

	static LPCSTR __cdecl StringFindCharRev(
		_In_z_ LPCSTR psz,
		_In_ char ch) throw()
	{
		return reinterpret_cast< LPCSTR >( _mbsrchr( reinterpret_cast< const unsigned char* >( psz ), (unsigned char)ch ) );
	}

	static LPCSTR __cdecl StringScanSet(
		_In_z_ LPCSTR pszBlock,
		_In_z_ LPCSTR pszMatch) throw()
	{
		return reinterpret_cast< LPCSTR >( _mbspbrk( reinterpret_cast< const unsigned char* >( pszBlock ),
			reinterpret_cast< const unsigned char* >( pszMatch ) ) );
	}

	static int __cdecl StringSpanIncluding(
		_In_z_ LPCSTR pszBlock,
		_In_z_ LPCSTR pszSet) throw()
	{
		return (int)_mbsspn( reinterpret_cast< const unsigned char* >( pszBlock ), reinterpret_cast< const unsigned char* >( pszSet ) );
	}

	static int __cdecl StringSpanExcluding(
		_In_z_ LPCSTR pszBlock,
		_In_z_ LPCSTR pszSet) throw()
	{
		return (int)_mbscspn( reinterpret_cast< const unsigned char* >( pszBlock ), reinterpret_cast< const unsigned char* >( pszSet ) );
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::StringUppercase")
	static LPSTR __cdecl StringUppercase(_Inout_z_ LPSTR psz) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		return reinterpret_cast< LPSTR >( _mbsupr( reinterpret_cast< unsigned char* >( psz ) ) );
#pragma warning (pop)
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::StringLowercase")
	static LPSTR __cdecl StringLowercase(_Inout_z_ LPSTR psz) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		return reinterpret_cast< LPSTR >( _mbslwr( reinterpret_cast< unsigned char* >( psz ) ) );
#pragma warning (pop)
	}

	static LPSTR __cdecl StringUppercase(
		_Inout_updates_z_(size) LPSTR psz,
		_In_ size_t size)
	{
		Checked::mbsupr_s(reinterpret_cast< unsigned char* >( psz ), size);
		return psz;
	}

	static LPSTR __cdecl StringLowercase(
		_Inout_updates_z_(size) LPSTR psz,
		_In_ size_t size)
	{
		Checked::mbslwr_s( reinterpret_cast< unsigned char* >( psz ), size );
		return psz;
	}

	static LPSTR __cdecl StringReverse(_Inout_z_ LPSTR psz) throw()
	{
		return reinterpret_cast< LPSTR >( _mbsrev( reinterpret_cast< unsigned char* >( psz ) ) );
	}

	static int __cdecl GetFormattedLength(
		_In_z_ _Printf_format_string_ LPCSTR pszFormat, va_list args) throw()
	{
		return _vscprintf( pszFormat, args );
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::Format")
	static int __cdecl Format(
		_Out_ _Post_z_ LPSTR pszBuffer,
		_In_z_ _Printf_format_string_ LPCSTR pszFormat,
		va_list args ) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
		return vsprintf( pszBuffer, pszFormat, args );
#pragma warning (pop)
	}
	static int __cdecl Format(
		_Out_writes_to_(nlength, return) LPSTR pszBuffer,
		_In_ size_t nlength,
		_In_z_ _Printf_format_string_ LPCSTR pszFormat, va_list args ) throw()
	{
		return vsprintf_s( pszBuffer, nlength, pszFormat, args );
	}

	static int __cdecl GetBaseTypeLength(_In_z_ LPCSTR pszSrc) throw()
	{
		// Returns required buffer length in XCHARs
		return AtlStrLen( pszSrc );
	}

	static int __cdecl GetBaseTypeLength(
		_In_reads_z_(nLength) LPCSTR pszSrc,
		_In_ int nLength) throw()
	{
		(void)pszSrc;
		// Returns required buffer length in XCHARs
		return nLength;
	}

	static int __cdecl GetBaseTypeLength(_In_z_ LPCWSTR pszSource) throw()
	{
		// Returns required buffer length in XCHARs
		return ::WideCharToMultiByte( _AtlGetConversionACP(), 0, pszSource, -1, NULL, 0, NULL, NULL )-1;
	}

	static int __cdecl GetBaseTypeLength(
		_In_reads_(nLength) LPCWSTR pszSource,
		_In_ int nLength) throw()
	{
		// Returns required buffer length in XCHARs
		return ::WideCharToMultiByte( _AtlGetConversionACP(), 0, pszSource, nLength, NULL, 0, NULL, NULL );
	}

	static void __cdecl ConvertToBaseType(
		_Out_writes_(nDestLength) LPSTR pszDest,
		_In_ int nDestLength,
		_In_z_ LPCSTR pszSrc,
		_In_ int nSrcLength = -1)
	{
		if (nSrcLength == -1) { nSrcLength=1 + GetBaseTypeLength(pszSrc); }
		// nLen is in XCHARs
		Checked::memcpy_s( pszDest, nDestLength*sizeof( char ),
			pszSrc, nSrcLength*sizeof( char ) );
	}

	static void __cdecl ConvertToBaseType(
		_Out_writes_(nDestLength) LPSTR pszDest,
		_In_ int nDestLength,
		_In_ LPCWSTR pszSrc,
		_In_ int nSrcLength = -1) throw()
	{
		// nLen is in XCHARs
		::WideCharToMultiByte( _AtlGetConversionACP(), 0, pszSrc, nSrcLength, pszDest, nDestLength, NULL, NULL );
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::ConvertToOem")
	static void ConvertToOem(_Inout_ _CharType* pstrString) throw()
	{
		BOOL fSuccess=::CharToOemA(pstrString, pstrString);
		// old version can't report error
		ATLASSERT(fSuccess);
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::ConvertToAnsi")
	static void ConvertToAnsi(_Inout_ _CharType* pstrString) throw()
	{
		BOOL fSuccess=::OemToCharA(pstrString, pstrString);
		// old version can't report error
		ATLASSERT(fSuccess);
	}

	static void ConvertToOem(
		_Inout_updates_(size) _CharType* pstrString,
		_In_ size_t size)
	{
		if(size>UINT_MAX)
		{
			// API only allows DWORD size
			AtlThrow(E_INVALIDARG);
		}
		DWORD dwSize=static_cast<DWORD>(size);
		BOOL fSuccess=::CharToOemBuffA(pstrString, pstrString, dwSize);
		if(!fSuccess)
		{
			AtlThrowLastWin32();
		}
	}

	static void ConvertToAnsi(
		_Inout_updates_(size) _CharType* pstrString,
		_In_ size_t size)
	{
		if(size>UINT_MAX)
		{
			// API only allows DWORD size
			AtlThrow(E_INVALIDARG);
		}
		DWORD dwSize=static_cast<DWORD>(size);
		BOOL fSuccess=::OemToCharBuffA(pstrString, pstrString, dwSize);
		if(!fSuccess)
		{
			AtlThrowLastWin32();
		}
	}

	static void __cdecl FloodCharacters(
		_In_ char ch,
		_In_ int nLength,
		_Out_writes_all_(nLength) char* pch) throw()
	{
		// nLength is in XCHARs
		memset( pch, ch, nLength );
	}

	static _Ret_maybenull_z_ BSTR __cdecl AllocSysString(
		_In_reads_(nDataLength) const char* pchData,
		_In_ int nDataLength) throw()
	{
		int nLen = ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength,
			NULL, NULL );
		BSTR bstr = ::SysAllocStringLen( NULL, nLen );
		if( bstr != NULL )
		{
			::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength, bstr, nLen );
		}

		return bstr;
	}

	_Success_(return != FALSE)
	static BOOL __cdecl ReAllocSysString(
		_In_reads_(nDataLength) const char* pchData,
		_Inout_ _Deref_post_opt_valid_ _Post_z_ BSTR* pbstr,
		_In_ int nDataLength) throw()
	{
		int nLen = ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength, NULL, NULL );
		BOOL bSuccess = ::SysReAllocStringLen( pbstr, NULL, nLen );
		if( bSuccess && nLen > 0 )
		{
			bSuccess = ( 0 != ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength, *pbstr, nLen ));
		}

		return bSuccess;
	}

	static DWORD __cdecl _AFX_FUNCNAME(FormatMessage)(
		_In_ DWORD dwFlags,
		_In_opt_ LPCVOID pSource,
		_In_ DWORD dwMessageID,
		_In_ DWORD dwLanguageID,
		_Out_writes_z_(nSize) LPSTR pszBuffer,
		_In_ DWORD nSize,
		_In_opt_ va_list* pArguments) throw()
	{
		return ::FormatMessageA( dwFlags, pSource, dwMessageID, dwLanguageID,
				pszBuffer, nSize, pArguments );
	}

#if defined(_AFX)
	static DWORD __cdecl FormatMessage(
		_In_ DWORD dwFlags,
		_In_opt_ LPCVOID pSource,
		_In_ DWORD dwMessageID,
		_In_ DWORD dwLanguageID,
		_Out_writes_z_(nSize) LPSTR pszBuffer,
		_In_ DWORD nSize,
		_In_opt_ va_list* pArguments) throw()
	{
		return _AFX_FUNCNAME(FormatMessage)(dwFlags, pSource, dwMessageID, dwLanguageID, pszBuffer, nSize, pArguments);
	}
#endif

	static int __cdecl SafeStringLen(_In_opt_z_ LPCSTR psz) throw()
	{
		// returns length in bytes
		return (psz != NULL) ? int( strlen( psz ) ) : 0;
	}

	static int __cdecl SafeStringLen(_In_opt_z_ LPCWSTR psz) throw()
	{
		// returns length in wchar_ts
		return (psz != NULL) ? int( wcslen( psz ) ) : 0;
	}

	static int __cdecl GetCharLen(_In_opt_z_ const wchar_t* pch) throw()
	{
		(void)pch;
		// returns char length
		return 1;
	}

	static int __cdecl GetCharLen(_In_z_ const char* pch) throw()
	{
		// returns char length
		return int( _mbclen( reinterpret_cast< const unsigned char* >( pch ) ) );
	}

	_Success_(return != 0 && return < dwSize)  
	static DWORD __cdecl _AFX_FUNCNAME(GetEnvironmentVariable)(
		_In_z_ LPCSTR pszVar,
		_Out_writes_opt_z_(dwSize) LPSTR pszBuffer,
		_In_ DWORD dwSize) throw()
	{
		return ::GetEnvironmentVariableA(pszVar, pszBuffer, dwSize);
	}

#if defined(_AFX)
	_Success_(return != 0 && return < dwSize)  
	static DWORD __cdecl GetEnvironmentVariable(
		_In_z_ LPCSTR pszVar,
		_Out_writes_opt_z_(dwSize) LPSTR pszBuffer,
		_In_ DWORD dwSize) throw()
	{
		return _AFX_FUNCNAME(GetEnvironmentVariable)(pszVar, pszBuffer, dwSize);
	}
#endif
};
#pragma warning (pop)
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP


// specialization for wchar_t
#ifndef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
template< typename _CharType = wchar_t >
class ChTraitsCRT :
	public ChTraitsBase< _CharType >
#else
template<>
class ChTraitsCRT< wchar_t > :
	public ChTraitsBase< wchar_t >
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
{
public:
	static LPWSTR __cdecl CharNext(_In_ LPCWSTR psz) throw()
	{
		return const_cast< LPWSTR >( psz+1 );
	}

	static int __cdecl IsDigit(_In_ wchar_t ch) throw()
	{
		return iswdigit( static_cast<unsigned short>(ch) );
	}

	static int __cdecl IsSpace(_In_ wchar_t ch) throw()
	{
		return iswspace( static_cast<unsigned short>(ch) );
	}

	static int __cdecl StringCompare(
		_In_z_ LPCWSTR pszA,
		_In_z_ LPCWSTR pszB) throw()
	{
		return wcscmp( pszA, pszB );
	}

	static int __cdecl StringCompareIgnore(
		_In_z_ LPCWSTR pszA,
		_In_z_ LPCWSTR pszB) throw()
	{
		return _wcsicmp( pszA, pszB );
	}

	static int __cdecl StringCollate(
		_In_z_ LPCWSTR pszA,
		_In_z_ LPCWSTR pszB) throw()
	{
		return wcscoll( pszA, pszB );
	}

	static int __cdecl StringCollateIgnore(
		_In_z_ LPCWSTR pszA,
		_In_z_ LPCWSTR pszB) throw()
	{
		return _wcsicoll( pszA, pszB );
	}

	static LPCWSTR __cdecl StringFindString(
		_In_z_ LPCWSTR pszBlock,
		_In_z_ LPCWSTR pszMatch) throw()
	{
		return wcsstr( pszBlock, pszMatch );
	}

	static LPWSTR __cdecl StringFindString(
		_In_z_ LPWSTR pszBlock,
		_In_z_ LPCWSTR pszMatch) throw()
	{
		return( const_cast< LPWSTR >( StringFindString( const_cast< LPCWSTR >( pszBlock ), pszMatch ) ) );
	}

	static LPCWSTR __cdecl StringFindChar(
		_In_z_ LPCWSTR pszBlock,
		_In_ wchar_t chMatch) throw()
	{
		return wcschr( pszBlock, chMatch );
	}

	static LPCWSTR __cdecl StringFindCharRev(
		_In_z_ LPCWSTR psz,
		_In_ wchar_t ch) throw()
	{
		return wcsrchr( psz, ch );
	}

	static LPCWSTR __cdecl StringScanSet(
		_In_z_ LPCWSTR pszBlock,
		_In_z_ LPCWSTR pszMatch) throw()
	{
		return wcspbrk( pszBlock, pszMatch );
	}

	static int __cdecl StringSpanIncluding(
		_In_z_ LPCWSTR pszBlock,
		_In_z_ LPCWSTR pszSet) throw()
	{
		return (int)wcsspn( pszBlock, pszSet );
	}

	static int __cdecl StringSpanExcluding(
		_In_z_ LPCWSTR pszBlock,
		_In_z_ LPCWSTR pszSet) throw()
	{
		return (int)wcscspn( pszBlock, pszSet );
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::StringUppercase")
	static LPWSTR __cdecl StringUppercase(_Inout_z_ LPWSTR psz) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		return _wcsupr( psz );
#pragma warning (pop)
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::StringLowercase")
	static LPWSTR __cdecl StringLowercase(_Inout_z_ LPWSTR psz) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		return _wcslwr( psz );
#pragma warning (pop)
	}

	static LPWSTR __cdecl StringUppercase(
		_Inout_updates_(size) LPWSTR psz,
		_In_ size_t size) throw()
	{
		errno_t err = _wcsupr_s( psz, size );
		return (err == 0) ? psz : NULL;
	}

	static LPWSTR __cdecl StringLowercase(
		_Inout_updates_(size) LPWSTR psz,
		_In_ size_t size) throw()
	{
		errno_t err = _wcslwr_s( psz, size );
		return (err == 0) ? psz : NULL;
	}

	static LPWSTR __cdecl StringReverse(_Inout_z_ LPWSTR psz) throw()
	{
		return _wcsrev( psz );
	}

	static int __cdecl GetFormattedLength(
		_In_z_ _Printf_format_string_ LPCWSTR pszFormat, va_list args) throw()
	{
#if _MSC_VER < 1900
		return _vscwprintf(pszFormat, args);
#else
		// Explicitly request the legacy wide format specifiers mode from the CRT,
		// for compatibility with previous versions.  While the CRT supports two
		// modes, the ATL and MFC functions that accept format strings only support
		// legacy mode format strings.
		int const result = __stdio_common_vswprintf(
			_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |
			_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR |
			_CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS,
			NULL, 0, pszFormat, NULL, args);

		return result < 0 ? -1 : result;
#endif
	}

	_ATL_INSECURE_DEPRECATE("You must pass an output size to ChTraitsCRT::Format")
	static int __cdecl Format(
		_Out_ _Post_z_ LPWSTR pszBuffer,
		_In_ _Printf_format_string_ LPCWSTR pszFormat, va_list args) throw()
	{
#pragma warning(suppress : 4995)
#pragma warning(suppress : 4996)
#pragma warning(suppress : 6386)
#pragma warning(suppress : 28719)
#if _MSC_VER < 1900
		return vswprintf(pszBuffer, pszFormat, args);
#else
		// Explicitly request the legacy wide format specifiers mode from the CRT,
		// for compatibility with previous versions.  While the CRT supports two
		// modes, the ATL and MFC functions that accept format strings only support
		// legacy mode format strings.
		int const result = __stdio_common_vswprintf(
			_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |
			_CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS,
			pszBuffer, INT_MAX, pszFormat, NULL, args);

		return result < 0 ? -1 : result;
#endif
	}
	static int __cdecl Format(
		_Out_writes_(nLength) LPWSTR pszBuffer,
		_In_ size_t nLength,
		_In_ _Printf_format_string_ LPCWSTR pszFormat, va_list args) throw()
	{
#if _MSC_VER < 1900
		return vswprintf_s(pszBuffer, nLength, pszFormat, args);
#else
		// Explicitly request the legacy wide format specifiers mode from the CRT,
		// for compatibility with previous versions.  While the CRT supports two
		// modes, the ATL and MFC functions that accept format strings only support
		// legacy mode format strings.
		int const result = __stdio_common_vswprintf_s(
			_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |
			_CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS,
			pszBuffer, nLength, pszFormat, NULL, args);

		return result < 0 ? -1 : result;
#endif
	}

	static int __cdecl GetBaseTypeLength(_In_z_ LPCSTR pszSrc) throw()
	{
		// Returns required buffer size in wchar_ts
		return ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pszSrc, -1, NULL, 0 )-1;
	}

	static int __cdecl GetBaseTypeLength(
		_In_reads_(nLength) LPCSTR pszSrc,
		_In_ int nLength) throw()
	{
		// Returns required buffer size in wchar_ts
		return ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pszSrc, nLength, NULL, 0 );
	}

	static int __cdecl GetBaseTypeLength(_In_z_ LPCWSTR pszSrc) throw()
	{
		// Returns required buffer size in wchar_ts
		return AtlStrLen( pszSrc );
	}

	static int __cdecl GetBaseTypeLength(
		_In_reads_(nLength) LPCWSTR pszSrc,
		_In_ int nLength) throw()
	{
		(void)pszSrc;
		// Returns required buffer size in wchar_ts
		return nLength;
	}

	static void __cdecl ConvertToBaseType(
		_Out_writes_(nDestLength) LPWSTR pszDest,
		_In_ int nDestLength,
		_In_ LPCSTR pszSrc,
		_In_ int nSrcLength = -1) throw()
	{
		// nLen is in wchar_ts
		::MultiByteToWideChar( _AtlGetConversionACP(), 0, pszSrc, nSrcLength, pszDest, nDestLength );
	}

	static void __cdecl ConvertToBaseType(
		_Out_writes_(nDestLength) LPWSTR pszDest,
		_In_ int nDestLength,
		_In_ LPCWSTR pszSrc,
		_In_ int nSrcLength = -1 )
	{
		if (nSrcLength == -1) { nSrcLength=1 + GetBaseTypeLength(pszSrc); }
		// nLen is in wchar_ts
		Checked::wmemcpy_s(pszDest, nDestLength, pszSrc, nSrcLength);
	}

	static void __cdecl FloodCharacters(
		_In_ wchar_t ch,
		_In_ int nLength,
		_Out_writes_all_(nLength) WCHAR *psz) throw()
	{
		// nLength is in XCHARs
		for( int i = 0; i < nLength; i++ )
		{
			psz[i] = ch;
		}
	}

	_Ret_maybenull_z_ static BSTR __cdecl AllocSysString(
		_In_reads_(nDataLength) const wchar_t* pchData,
		_In_ int nDataLength) throw()
	{
		return ::SysAllocStringLen( pchData, nDataLength );
	}

	static BOOL __cdecl ReAllocSysString(
		_In_reads_(nDataLength) const wchar_t* pchData,
		_Inout_ _Deref_post_opt_valid_ _Post_z_ BSTR* pbstr,
		_In_ int nDataLength) throw()
	{
		return ::SysReAllocStringLen( pbstr, pchData, nDataLength );
	}

	static int __cdecl SafeStringLen(_In_opt_z_ LPCSTR psz) throw()
	{
		// returns length in bytes
		return (psz != NULL) ? (int)strlen( psz ) : 0;
	}

	static int __cdecl SafeStringLen(_In_opt_z_ LPCWSTR psz) throw()
	{
		// returns length in wchar_ts
		return (psz != NULL) ? (int)wcslen( psz ) : 0;
	}

	static int __cdecl GetCharLen(_In_opt_z_ const wchar_t* pch) throw()
	{
		(void)pch;
		// returns char length
		return 1;
	}

	static int __cdecl GetCharLen(_In_z_ const char* pch) throw()
	{
		// returns char length
		return (int)( _mbclen( reinterpret_cast< const unsigned char* >( pch ) ) );
	}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
ATLPREFAST_SUPPRESS(6103)
	_Success_(return != 0 && return < dwSize)     
	static DWORD __cdecl _AFX_FUNCNAME(GetEnvironmentVariable)(
		_In_z_ LPCWSTR pszVar,
		_Out_writes_opt_z_(dwSize) LPWSTR pszBuffer,
		_In_ DWORD dwSize) throw()
	{
		return ::GetEnvironmentVariableW(pszVar, pszBuffer, dwSize);
	}
ATLPREFAST_UNSUPPRESS()

#if defined(_AFX)
ATLPREFAST_SUPPRESS(6103)
	_Success_(return != 0 && return < dwSize) 
	static DWORD __cdecl GetEnvironmentVariable(
		_In_z_ LPCWSTR pszVar,
		_Out_writes_opt_z_(dwSize) LPWSTR pszBuffer,
		_In_ DWORD dwSize) throw()
	{
		return _AFX_FUNCNAME(GetEnvironmentVariable)(pszVar, pszBuffer, dwSize);
	}
ATLPREFAST_UNSUPPRESS()
#endif

	static void __cdecl ConvertToOem(_In_opt_z_ LPWSTR /*psz*/)
	{
		ATLENSURE(FALSE); // Unsupported Feature
	}

	static void __cdecl ConvertToAnsi(_In_opt_z_ LPWSTR /*psz*/)
	{
		ATLENSURE(FALSE); // Unsupported Feature
	}

	static void __cdecl ConvertToOem(
		_In_reads_(nLen) LPWSTR /*psz*/,
		_In_ size_t nLen)
	{
		UNREFERENCED_PARAMETER(nLen);
		ATLENSURE(FALSE); // Unsupported Feature
	}

	static void __cdecl ConvertToAnsi(
		_In_reads_(nLen) LPWSTR /*psz*/,
		_In_ size_t nLen)
	{
		UNREFERENCED_PARAMETER(nLen);
		ATLENSURE(FALSE); // Unsupported Feature
	}

#ifdef _UNICODE
public:
	static DWORD __cdecl _AFX_FUNCNAME(FormatMessage)(
		_In_ DWORD dwFlags,
		_In_opt_ LPCVOID pSource,
		_In_ DWORD dwMessageID,
		_In_ DWORD dwLanguageID,
		_Out_writes_z_(nSize) LPWSTR pszBuffer,
		_In_ DWORD nSize,
		_In_opt_ va_list* pArguments) throw()
	{
		return ::FormatMessageW( dwFlags, pSource, dwMessageID, dwLanguageID,
				pszBuffer, nSize, pArguments );
	}

#if defined(_AFX)
	static DWORD __cdecl FormatMessage(
		_In_ DWORD dwFlags,
		_In_opt_ LPCVOID pSource,
		_In_ DWORD dwMessageID,
		_In_ DWORD dwLanguageID,
		_Out_writes_z_(nSize) LPWSTR pszBuffer,
		_In_ DWORD nSize,
		_In_opt_ va_list* pArguments) throw()
	{
		return _AFX_FUNCNAME(FormatMessage)(dwFlags, pSource, dwMessageID, dwLanguageID, pszBuffer, nSize, pArguments);
	}
#endif

#else
	static DWORD __cdecl _AFX_FUNCNAME(FormatMessage)(
		_In_ DWORD /*dwFlags*/,
		_In_opt_ LPCVOID /*pSource*/,
		_In_ DWORD /*dwMessageID*/,
		_In_ DWORD /*dwLanguageID*/,
		_Out_writes_z_(nSize) LPWSTR /*pszBuffer*/,
		_In_ DWORD nSize,
		_In_opt_ va_list* /*pArguments*/)
	{
		ATLENSURE(FALSE); // Unsupported Feature
		UNREFERENCED_PARAMETER(nSize);
		return 0;
	}

#if defined(_AFX)
	static DWORD __cdecl FormatMessage(
		_In_ DWORD dwFlags,
		_In_opt_ LPCVOID pSource,
		_In_ DWORD dwMessageID,
		_In_ DWORD dwLanguageID,
		_Out_writes_z_(nSize) LPWSTR pszBuffer,
		_In_ DWORD nSize,
		_In_opt_ va_list* pArguments)
	{
		return _AFX_FUNCNAME(FormatMessage)(dwFlags, pSource, dwMessageID, dwLanguageID, pszBuffer, nSize, pArguments);
	}
#endif

#endif

#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

};

}	// namespace ATL

// Forward declare
template< typename _CharType = char, class StringIterator = ATL::ChTraitsCRT< _CharType > >
class StrTraitMFC_DLL;

namespace ATL
{

namespace _CSTRING_IMPL_
{
template <typename _CharType, class StringTraits>
struct _MFCDLLTraitsCheck
{
	const static bool c_bIsMFCDLLTraits = false;
};

template<typename _CharType>
struct _MFCDLLTraitsCheck<_CharType, StrTraitMFC_DLL<_CharType, ATL::ChTraitsCRT< _CharType > > >
{
	const static bool c_bIsMFCDLLTraits = true;
};

}

// The CStringT class has a few varargs member functions that will cause 4793
// warnings if compiled /clr. Because of the way template classes are parsed,
// we need to disable the warning for the entire class.
//#pragma warning( push )
//#pragma warning( disable : 4793 )


template< typename BaseType, class StringTraits >
class CStringT :
	public CSimpleStringT< BaseType, _CSTRING_IMPL_::_MFCDLLTraitsCheck<BaseType, StringTraits>::c_bIsMFCDLLTraits >
{
public:
	typedef CSimpleStringT< BaseType, _CSTRING_IMPL_::_MFCDLLTraitsCheck<BaseType, StringTraits>::c_bIsMFCDLLTraits > CThisSimpleString;
	typedef StringTraits StrTraits;
	typedef typename CThisSimpleString::XCHAR XCHAR;
	typedef typename CThisSimpleString::PXSTR PXSTR;
	typedef typename CThisSimpleString::PCXSTR PCXSTR;
	typedef typename CThisSimpleString::YCHAR YCHAR;
	typedef typename CThisSimpleString::PYSTR PYSTR;
	typedef typename CThisSimpleString::PCYSTR PCYSTR;

public:
	CStringT() throw() :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
	}
	explicit CStringT(
			_In_ IAtlStringMgr* pStringMgr) throw() :
		CThisSimpleString( pStringMgr )
	{
	}

	CStringT(_In_ const VARIANT& varSrc);
	CStringT(
		_In_ const VARIANT& varSrc,
		_In_ IAtlStringMgr* pStringMgr);

	static void __cdecl Construct(_In_ CStringT* pString)
	{
		new( pString ) CStringT;
	}

	// Copy constructor
	CStringT(_In_ const CStringT& strSrc) :
		CThisSimpleString( strSrc )
	{
	}

	// Construct from CSimpleStringT
	operator CSimpleStringT<BaseType, !_CSTRING_IMPL_::_MFCDLLTraitsCheck<BaseType, StringTraits>::c_bIsMFCDLLTraits >&()
	{
		return *(CSimpleStringT<BaseType, !_CSTRING_IMPL_::_MFCDLLTraitsCheck<BaseType, StringTraits>::c_bIsMFCDLLTraits >*)this;
	}

	template <bool bMFCDLL>
	CStringT(_In_ const CSimpleStringT<BaseType, bMFCDLL>& strSrc) :
		CThisSimpleString( strSrc )
	{
	}

	CStringT(_In_opt_z_ const XCHAR* pszSrc) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		if( !CheckImplicitLoad( pszSrc ) )
		{
			*this = pszSrc;
		}
	}

	CStringT(
			_In_opt_z_ const XCHAR* pszSrc,
			_In_ IAtlStringMgr* pStringMgr) :
		CThisSimpleString( pStringMgr )
	{
		if( !CheckImplicitLoad( pszSrc ) )
		{
			*this = pszSrc;
		}
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CSTRING_EXPLICIT CStringT(_In_opt_z_ const YCHAR* pszSrc) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		if( !CheckImplicitLoad( pszSrc ) )
		{
			*this = pszSrc;
		}
	}

	CStringT(
			_In_opt_z_ const YCHAR* pszSrc,
			_In_ IAtlStringMgr* pStringMgr) :
		CThisSimpleString( pStringMgr )
	{
		if( !CheckImplicitLoad( pszSrc ) )
		{
			*this = pszSrc;
		}
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	// This template will compile only for
	// class SystemString == System::String

#if defined(__cplusplus_cli)

	template <class SystemString>
	CStringT(_In_ SystemString^ pString) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		cli::pin_ptr<const System::Char> pChar = PtrToStringChars( pString );
		const wchar_t *psz = pChar;
		*this = psz;
	}

#elif defined(_MANAGED)

	template<class SystemString>
	CStringT(_In_ SystemString __gc* pString) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		const wchar_t __pin* psz = PtrToStringChars( pString );
		*this = psz;
	}

#endif

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CSTRING_EXPLICIT CStringT(_In_z_ const unsigned char* pszSrc) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		*this = reinterpret_cast< const char* >( pszSrc );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

//ctors to prevent from oldSyntax template ctor (above) hijack certain types.
//MFC dll instantiate all CStringT methods inside the dll and declares dllimport for
//all methods in user build (see afxstr.h), so need to include the methods in MFC dll builds.
#if defined(_AFXDLL) && defined(_MFC_DLL_BLD) || !defined(__cplusplus_cli) && defined(_MANAGED)

	/*CSTRING_EXPLICIT*/ CStringT(_In_opt_z_ char* pszSrc) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		const char *psz = reinterpret_cast< const char* >( pszSrc );
		if (!CheckImplicitLoad( psz ))
		{
			*this = psz;
		}
	}

	CSTRING_EXPLICIT CStringT(_In_opt_z_ unsigned char* pszSrc) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		const char *psz = reinterpret_cast< const char* >( pszSrc );
		if (!CheckImplicitLoad( psz ))
		{
			*this = psz;
		}
	}

	CSTRING_EXPLICIT CStringT(_In_opt_z_ wchar_t* pszSrc) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		const wchar_t *psz = reinterpret_cast< const wchar_t* >( pszSrc );
		if (!CheckImplicitLoad( psz ))
		{
			*this = psz;
		}
	}
#endif


#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CStringT(
			_In_opt_z_ const unsigned char* pszSrc,
			_In_ IAtlStringMgr* pStringMgr) :
		CThisSimpleString( pStringMgr )
	{
		*this = reinterpret_cast< const char* >( pszSrc );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

#ifdef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
#define _CSTRING_CHAR_T XCHAR
#else // def _CSTRING_NARROW_WIDE_CONVERSION
#define _CSTRING_CHAR_T char
#endif // def _CSTRING_NARROW_WIDE_CONVERSION

	CSTRING_EXPLICIT CStringT(
			_In_ _CSTRING_CHAR_T ch,
			_In_ int nLength = 1) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		ATLASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			PXSTR pszBuffer = GetBuffer( nLength );
			StringTraits::FloodCharacters( XCHAR( ch ), nLength, pszBuffer );
			ReleaseBufferSetLength( nLength );
		}
	}
#undef _CSTRING_CHAR_T

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CSTRING_EXPLICIT CStringT(
			_In_ wchar_t ch,
			_In_ int nLength = 1) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		ATLASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			//Convert ch to the BaseType
			wchar_t pszCh[2] = { ch , 0 };
			int nBaseTypeCharLen = 1;

			if(ch != L'\0')
			{
				nBaseTypeCharLen = StringTraits::GetBaseTypeLength(pszCh);
			}

			CTempBuffer<XCHAR,10> buffBaseTypeChar;
			buffBaseTypeChar.Allocate(nBaseTypeCharLen+1);
			StringTraits::ConvertToBaseType( buffBaseTypeChar, nBaseTypeCharLen+1, pszCh, 1 );
			//Allocate enough characters in String and flood (replicate) with the (converted character)*nLength
			PXSTR pszBuffer = GetBuffer( nLength*nBaseTypeCharLen );
			if (nBaseTypeCharLen == 1)
			{   //Optimization for a common case - wide char translates to 1 ansi/wide char.
				StringTraits::FloodCharacters( buffBaseTypeChar[0], nLength, pszBuffer );
			} else
			{
				XCHAR* p=pszBuffer;
				for (int i=0 ; i < nLength ;++i)
				{
					for (int j=0 ; j < nBaseTypeCharLen ;++j)
					{
						*p=buffBaseTypeChar[j];
						++p;
					}
				}
			}
			ReleaseBufferSetLength( nLength*nBaseTypeCharLen );
		}
	}
#else
private:
	CSTRING_EXPLICIT CStringT(
		_In_ YCHAR ch,
		_In_ int nLength = 1);
public:
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	CStringT(
			_In_reads_(nLength) const XCHAR* pch,
			_In_ int nLength) :
		CThisSimpleString( pch, nLength, StringTraits::GetDefaultManager() )
	{
	}

	CStringT(
			_In_reads_(nLength) const XCHAR* pch,
			_In_ int nLength,
			_In_ IAtlStringMgr* pStringMgr) :
		CThisSimpleString( pch, nLength, pStringMgr )
	{
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CStringT(
			_In_reads_(nLength) const YCHAR* pch,
			_In_ int nLength) :
		CThisSimpleString( StringTraits::GetDefaultManager() )
	{
		ATLASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			ATLASSERT( AtlIsValidAddress( pch, nLength*sizeof( YCHAR ), FALSE ) );
			if(pch == NULL)
				AtlThrow(E_INVALIDARG);

			int nDestLength = StringTraits::GetBaseTypeLength( pch, nLength );
			PXSTR pszBuffer = GetBuffer( nDestLength );
			StringTraits::ConvertToBaseType( pszBuffer, nDestLength, pch, nLength );
			ReleaseBufferSetLength( nDestLength );
		}
	}

	CStringT(
			_In_reads_(nLength) const YCHAR* pch,
			_In_ int nLength,
			_In_ IAtlStringMgr* pStringMgr) :
		CThisSimpleString( pStringMgr )
	{
		ATLASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			ATLASSERT( AtlIsValidAddress( pch, nLength*sizeof( YCHAR ), FALSE ) );
			if(pch == NULL)
				AtlThrow(E_INVALIDARG);

			int nDestLength = StringTraits::GetBaseTypeLength( pch, nLength );
			PXSTR pszBuffer = GetBuffer( nDestLength );
			StringTraits::ConvertToBaseType( pszBuffer, nDestLength, pch, nLength );
			ReleaseBufferSetLength( nDestLength );
		}
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	// Destructor
	~CStringT() throw()
	{
	}

	// Assignment operators
	CStringT& operator=(_In_ const CStringT& strSrc)
	{
		CThisSimpleString::operator=( strSrc );

		return( *this );
	}

	template <bool bMFCDLL>
	CStringT& operator=(_In_ const CSimpleStringT<BaseType, bMFCDLL>& strSrc)
	{
		CThisSimpleString::operator=( strSrc );

		return( *this );
	}

	CStringT& operator=(_In_opt_z_ PCXSTR pszSrc)
	{
		CThisSimpleString::operator=( pszSrc );

		return( *this );
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CStringT& operator=(_In_opt_z_ PCYSTR pszSrc)
	{
		// nDestLength is in XCHARs
		int nDestLength = (pszSrc != NULL) ? StringTraits::GetBaseTypeLength( pszSrc ) : 0;
		if( nDestLength > 0 )
		{
			PXSTR pszBuffer = GetBuffer( nDestLength );
			StringTraits::ConvertToBaseType( pszBuffer, nDestLength, pszSrc);
			ReleaseBufferSetLength( nDestLength );
		}
		else
		{
			Empty();
		}

		return( *this );
	}

	CStringT& operator=(_In_opt_z_ const unsigned char* pszSrc)
	{
		return( operator=( reinterpret_cast< const char* >( pszSrc ) ) );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	CStringT& operator=(_In_ XCHAR ch)
	{
		XCHAR ach[2] = { ch, 0 };

		return( operator=( ach ) );
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	CStringT& operator=(_In_ YCHAR ch)
	{
		YCHAR ach[2] = { ch, 0 };

		return( operator=( ach ) );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	CStringT& operator=(_In_ const VARIANT& var);

	CStringT& operator+=(_In_ const CThisSimpleString& str)
	{
		CThisSimpleString::operator+=( str );

		return( *this );
	}
	template <bool bMFCDLL>
	CStringT& operator+=(_In_ const CSimpleStringT<BaseType, bMFCDLL>& str)
	{
		CThisSimpleString::operator+=( str );

		return( *this );
	}

	CStringT& operator+=(_In_z_ PCXSTR pszSrc)
	{
		CThisSimpleString::operator+=( pszSrc );

		return( *this );
	}
	template< int t_nSize >
	CStringT& operator+=(_In_ const CStaticString< XCHAR, t_nSize >& strSrc)
	{
		CThisSimpleString::operator+=( strSrc );

		return( *this );
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	CStringT& operator+=(_In_opt_z_ PCYSTR pszSrc)
	{
		CStringT str( pszSrc, GetManager() );

		return( operator+=( str ) );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	CStringT& operator+=(_In_ XCHAR ch)
	{
		CThisSimpleString::operator+=( ch );

		return( *this );
	}

#ifdef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
private:
	CStringT& operator+=(_In_ YCHAR ch);
public:
#else
	CStringT& operator+=(_In_ unsigned char ch)
	{
		CThisSimpleString::operator+=( ch );

		return( *this );
	}

	CStringT& operator+=(_In_ YCHAR ch)
	{
		CThisSimpleString::operator+=( ch );

		return( *this );
	}
#endif // def _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	CStringT& operator+=(_In_ const VARIANT& var);

	// Override from base class
	IAtlStringMgr* GetManager() const throw()
	{
		IAtlStringMgr* pStringMgr = CThisSimpleString::GetManager();
		if(pStringMgr) { return pStringMgr; }

		pStringMgr = StringTraits::GetDefaultManager();
		return pStringMgr->Clone();
	}

	// Comparison

	int Compare(_In_z_ PCXSTR psz) const
	{
		ATLENSURE( AtlIsValidString( psz ) );
		_Analysis_assume_(psz); // AtlIsValidString guarantees that psz != NULL
		return( StringTraits::StringCompare( GetString(), psz ) );
	}

	int CompareNoCase(_In_z_ PCXSTR psz) const
	{
		ATLENSURE( AtlIsValidString( psz ) );
		_Analysis_assume_(psz); // AtlIsValidString guarantees that psz != NULL
		return( StringTraits::StringCompareIgnore( GetString(), psz ) );
	}

	int Collate(_In_z_ PCXSTR psz) const throw()
	{
		ATLASSERT( AtlIsValidString( psz ) );
		return( StringTraits::StringCollate( GetString(), psz ) );
	}

	int CollateNoCase(_In_z_ PCXSTR psz) const throw()
	{
		ATLASSERT( AtlIsValidString( psz ) );
		return( StringTraits::StringCollateIgnore( GetString(), psz ) );
	}

	// Advanced manipulation

	// Delete 'nCount' characters, starting at index 'iIndex'
	int Delete(
		_In_ int iIndex,
		_In_ int nCount = 1)
	{
		if( iIndex < 0 )
			iIndex = 0;

		if( nCount < 0 )
			nCount = 0;

		int nLength = GetLength();
		if( (::ATL::AtlAddThrow(nCount, iIndex)) > nLength )
		{
			nCount = nLength-iIndex;
		}
		if( nCount > 0 )
		{
			int nNewLength = nLength-nCount;
			int nXCHARsToCopy = nLength-(iIndex+nCount)+1;
			PXSTR pszBuffer = GetBuffer();
			Checked::memmove_s( pszBuffer+iIndex, nXCHARsToCopy*sizeof( XCHAR ),
				pszBuffer+iIndex+nCount, nXCHARsToCopy*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nNewLength );
		}

		return( GetLength() );
	}

	// Insert character 'ch' before index 'iIndex'
	int Insert(
		_In_ int iIndex,
		_In_ XCHAR ch)
	{
		if( iIndex < 0 )
			iIndex = 0;

		if( iIndex > GetLength() )
		{
			iIndex = GetLength();
		}
		int nNewLength = GetLength()+1;

		PXSTR pszBuffer = GetBuffer( nNewLength );

		// move existing bytes down
		Checked::memmove_s( pszBuffer+iIndex+1, (nNewLength-iIndex)*sizeof( XCHAR ),
			pszBuffer+iIndex, (nNewLength-iIndex)*sizeof( XCHAR ) );
		pszBuffer[iIndex] = ch;

		ReleaseBufferSetLength( nNewLength );

		return( nNewLength );
	}

	// Insert string 'psz' before index 'iIndex'
	int Insert(
		_In_ int iIndex,
		_In_z_ PCXSTR psz)
	{
		if( iIndex < 0 )
			iIndex = 0;

		if( iIndex > GetLength() )
		{
			iIndex = GetLength();
		}

		// nInsertLength and nNewLength are in XCHARs
		int nInsertLength = StringTraits::SafeStringLen( psz );
		int nNewLength = GetLength();
		if( nInsertLength > 0 )
		{
			nNewLength += nInsertLength;

			PXSTR pszBuffer = GetBuffer( nNewLength );
			// move existing bytes down
			Checked::memmove_s( pszBuffer+iIndex+nInsertLength, (nNewLength-iIndex-nInsertLength+1)*sizeof( XCHAR ),
				pszBuffer+iIndex, (nNewLength-iIndex-nInsertLength+1)*sizeof( XCHAR ) );
			Checked::memcpy_s( pszBuffer+iIndex, nInsertLength*sizeof( XCHAR ),
				psz, nInsertLength*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nNewLength );
		}

		return( nNewLength );
	}

	// Replace all occurrences of character 'chOld' with character 'chNew'
	int Replace(
		_In_ XCHAR chOld,
		_In_ XCHAR chNew)
	{
		int nCount = 0;

		// short-circuit the nop case
		if( chOld != chNew )
		{
			// otherwise modify each character that matches in the string
			bool bCopied = false;
			PXSTR pszBuffer = const_cast< PXSTR >( GetString() );  // We don't actually write to pszBuffer until we've called GetBuffer().

			int nLength = GetLength();
			int iChar = 0;
			while( iChar < nLength )
			{
				// replace instances of the specified character only
				if( pszBuffer[iChar] == chOld )
				{
					if( !bCopied )
					{
						bCopied = true;
						pszBuffer = GetBuffer( nLength );
					}
					pszBuffer[iChar] = chNew;
					nCount++;
				}
				iChar = int( StringTraits::CharNext( pszBuffer+iChar )-pszBuffer );
			}
			if( bCopied )
			{
				ReleaseBufferSetLength( nLength );
			}
		}

		return( nCount );
	}

	// Replace all occurrences of string 'pszOld' with string 'pszNew'
	int Replace(
		_In_z_ PCXSTR pszOld,
		_In_z_ PCXSTR pszNew)
	{
		// can't have empty or NULL lpszOld

		// nSourceLen is in XCHARs
		int nSourceLen = StringTraits::SafeStringLen( pszOld );
		if( nSourceLen == 0 )
			return( 0 );
		// nReplacementLen is in XCHARs
		int nReplacementLen = StringTraits::SafeStringLen( pszNew );

		// loop once to figure out the size of the result string
		int nCount = 0;
		{
			PCXSTR pszStart = GetString();
			PCXSTR pszEnd = pszStart+GetLength();
			while( pszStart < pszEnd )
			{
				PCXSTR pszTarget;
				while( (pszTarget = StringTraits::StringFindString( pszStart, pszOld ) ) != NULL)
				{
					nCount++;
					pszStart = pszTarget+nSourceLen;
				}
				pszStart += StringTraits::SafeStringLen( pszStart )+1;
			}
		}

		// if any changes were made, make them
		if( nCount > 0 )
		{
			// if the buffer is too small, just
			//   allocate a new buffer (slow but sure)
			int nOldLength = GetLength();
			int nNewLength = nOldLength+(nReplacementLen-nSourceLen)*nCount;

			PXSTR pszBuffer = GetBuffer( __max( nNewLength, nOldLength ) );

			PXSTR pszStart = pszBuffer;
			PXSTR pszEnd = pszStart+nOldLength;

			// loop again to actually do the work
			while( pszStart < pszEnd )
			{
				PXSTR pszTarget;
				while( (pszTarget = StringTraits::StringFindString( pszStart, pszOld ) ) != NULL )
				{
					int nBalance = nOldLength-int(pszTarget-pszBuffer+nSourceLen);
					Checked::memmove_s( pszTarget+nReplacementLen, nBalance*sizeof( XCHAR ),
						pszTarget+nSourceLen, nBalance*sizeof( XCHAR ) );
					Checked::memcpy_s( pszTarget, nReplacementLen*sizeof( XCHAR ),
						pszNew, nReplacementLen*sizeof( XCHAR ) );
					pszStart = pszTarget+nReplacementLen;
					pszTarget[nReplacementLen+nBalance] = 0;
					nOldLength += (nReplacementLen-nSourceLen);
				}
				pszStart += StringTraits::SafeStringLen( pszStart )+1;
			}
			ATLASSERT( pszBuffer[nNewLength] == 0 );
			ReleaseBufferSetLength( nNewLength );
		}

		return( nCount );
	}

	// Remove all occurrences of character 'chRemove'
	int Remove(_In_ XCHAR chRemove)
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );

		PXSTR pszSource = pszBuffer;
		PXSTR pszDest = pszBuffer;
		PXSTR pszEnd = pszBuffer+nLength;

		while( pszSource < pszEnd )
		{
			PXSTR pszNewSource = StringTraits::CharNext( pszSource );
			if( *pszSource != chRemove )
			{
				// Copy the source to the destination.  Remember to copy all bytes of an MBCS character
				// Copy the source to the destination.  Remember to copy all bytes of an MBCS character
				size_t NewSourceGap = (pszNewSource-pszSource);
				PXSTR pszNewDest = pszDest + NewSourceGap;
				size_t i = 0;
				for (i = 0;  pszDest != pszNewDest && i < NewSourceGap; i++)
				{
					*pszDest = *pszSource;
					pszSource++;
					pszDest++;
				}
			}
			pszSource = pszNewSource;
		}
		*pszDest = 0;
		int nCount = int( pszSource-pszDest );
		ReleaseBufferSetLength( nLength-nCount );

		return( nCount );
	}

	CStringT Tokenize(
		_In_z_ PCXSTR pszTokens,
		_Inout_ int& iStart) const
	{
		ATLASSERT( iStart >= 0 );

		if(iStart < 0)
			AtlThrow(E_INVALIDARG);

		if( (pszTokens == NULL) || (*pszTokens == (XCHAR)0) )
		{
			if (iStart < GetLength())
			{
				return( CStringT( GetString()+iStart, GetManager() ) );
			}
		}
		else
		{
			PCXSTR pszPlace = GetString()+iStart;
			PCXSTR pszEnd = GetString()+GetLength();
			if( pszPlace < pszEnd )
			{
				int nIncluding = StringTraits::StringSpanIncluding( pszPlace,
					pszTokens );

				if( (pszPlace+nIncluding) < pszEnd )
				{
					pszPlace += nIncluding;
					int nExcluding = StringTraits::StringSpanExcluding( pszPlace, pszTokens );

					int iFrom = iStart+nIncluding;
					int nUntil = nExcluding;
					iStart = iFrom+nUntil+1;

					return( Mid( iFrom, nUntil ) );
				}
			}
		}

		// return empty string, done tokenizing
		iStart = -1;

		return( CStringT( GetManager() ) );
	}

	// find routines

	// Find the first occurrence of character 'ch', starting at index 'iStart'
	int Find(
		_In_ XCHAR ch,
		_In_ int iStart = 0) const throw()
	{
		// iStart is in XCHARs
		ATLASSERT( iStart >= 0 );

		// nLength is in XCHARs
		int nLength = GetLength();
		if( iStart < 0 || iStart >= nLength)
		{
			return( -1 );
		}

		// find first single character
		PCXSTR psz = StringTraits::StringFindChar( GetString()+iStart, ch );

		// return -1 if not found and index otherwise
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// look for a specific sub-string

	// Find the first occurrence of string 'pszSub', starting at index 'iStart'
	int Find(
		_In_z_ PCXSTR pszSub,
		_In_ int iStart = 0) const throw()
	{
		// iStart is in XCHARs
		ATLASSERT( iStart >= 0 );
		ATLASSERT( AtlIsValidString( pszSub ) );

		if(pszSub == NULL)
		{
			return( -1 );
		}
		// nLength is in XCHARs
		int nLength = GetLength();
		if( iStart < 0 || iStart > nLength )
		{
			return( -1 );
		}

		// find first matching substring
		PCXSTR psz = StringTraits::StringFindString( GetString()+iStart, pszSub );

		// return -1 for not found, distance from beginning otherwise
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// Find the first occurrence of any of the characters in string 'pszCharSet'
	int FindOneOf(_In_z_ PCXSTR pszCharSet) const throw()
	{
		ATLASSERT( AtlIsValidString( pszCharSet ) );
		PCXSTR psz = StringTraits::StringScanSet( GetString(), pszCharSet );
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// Find the last occurrence of character 'ch'
	int ReverseFind(_In_ XCHAR ch) const throw()
	{
		// find last single character
		PCXSTR psz = StringTraits::StringFindCharRev( GetString(), ch );

		// return -1 if not found, distance from beginning otherwise
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// manipulation

	// Convert the string to uppercase
	CStringT& MakeUpper()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::StringUppercase( pszBuffer, nLength+1 );
		ReleaseBufferSetLength( nLength );

		return( *this );
	}

	// Convert the string to lowercase
	CStringT& MakeLower()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::StringLowercase( pszBuffer, nLength+1 );
		ReleaseBufferSetLength( nLength );

		return( *this );
	}

	// Reverse the string
	CStringT& MakeReverse()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::StringReverse( pszBuffer );
		ReleaseBufferSetLength( nLength );

		return( *this );
	}

	// trimming

	// Remove all trailing whitespace
	CStringT& TrimRight()
	{
		// find beginning of trailing spaces by starting
		// at beginning (DBCS aware)

		PCXSTR psz = GetString();
		PCXSTR pszLast = NULL;

		while( *psz != 0 )
		{
			if( StringTraits::IsSpace( *psz ) )
			{
				if( pszLast == NULL )
					pszLast = psz;
			}
			else
			{
				pszLast = NULL;
			}
			psz = StringTraits::CharNext( psz );
		}

		if( pszLast != NULL )
		{
			// truncate at trailing space start
			int iLast = int( pszLast-GetString() );

			Truncate( iLast );
		}

		return( *this );
	}

	// Remove all leading whitespace
	CStringT& TrimLeft()
	{
		// find first non-space character

		PCXSTR psz = GetString();

		while( StringTraits::IsSpace( *psz ) )
		{
			psz = StringTraits::CharNext( psz );
		}

		if( psz != GetString() )
		{
			// fix up data and length
			int iFirst = int( psz-GetString() );
			PXSTR pszBuffer = GetBuffer( GetLength() );
			psz = pszBuffer+iFirst;
			int nDataLength = GetLength()-iFirst;
			Checked::memmove_s( pszBuffer, (GetLength()+1)*sizeof( XCHAR ),
				psz, (nDataLength+1)*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nDataLength );
		}

		return( *this );
	}

	// Remove all leading and trailing whitespace
	CStringT& Trim()
	{
		return( TrimRight().TrimLeft() );
	}

	// Remove all leading and trailing occurrences of character 'chTarget'
	CStringT& Trim(_In_ XCHAR chTarget)
	{
		return( TrimRight( chTarget ).TrimLeft( chTarget ) );
	}

	// Remove all leading and trailing occurrences of any of the characters in the string 'pszTargets'
	CStringT& Trim(_In_z_ PCXSTR pszTargets)
	{
		return( TrimRight( pszTargets ).TrimLeft( pszTargets ) );
	}

	// trimming anything (either side)

	// Remove all trailing occurrences of character 'chTarget'
	CStringT& TrimRight(_In_ XCHAR chTarget)
	{
		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)

		PCXSTR psz = GetString();
		PCXSTR pszLast = NULL;

		while( *psz != 0 )
		{
			if( *psz == chTarget )
			{
				if( pszLast == NULL )
				{
					pszLast = psz;
				}
			}
			else
			{
				pszLast = NULL;
			}
			psz = StringTraits::CharNext( psz );
		}

		if( pszLast != NULL )
		{
			// truncate at left-most matching character
			int iLast = int( pszLast-GetString() );
			Truncate( iLast );
		}

		return( *this );
	}

	// Remove all trailing occurrences of any of the characters in string 'pszTargets'
	CStringT& TrimRight(_In_z_ PCXSTR pszTargets)
	{
		// if we're not trimming anything, we're not doing any work
		if( (pszTargets == NULL) || (*pszTargets == 0) )
		{
			return( *this );
		}

		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)

		PCXSTR psz = GetString();
		PCXSTR pszLast = NULL;

		while( *psz != 0 )
		{
			if( StringTraits::StringFindChar( pszTargets, *psz ) != NULL )
			{
				if( pszLast == NULL )
				{
					pszLast = psz;
				}
			}
			else
			{
				pszLast = NULL;
			}
			psz = StringTraits::CharNext( psz );
		}

		if( pszLast != NULL )
		{
			// truncate at left-most matching character
			int iLast = int( pszLast-GetString() );
			Truncate( iLast );
		}

		return( *this );
	}

	// Remove all leading occurrences of character 'chTarget'
	CStringT& TrimLeft(_In_ XCHAR chTarget)
	{
		// find first non-matching character
		PCXSTR psz = GetString();

		while( chTarget == *psz )
		{
			psz = StringTraits::CharNext( psz );
		}

		if( psz != GetString() )
		{
			// fix up data and length
			int iFirst = int( psz-GetString() );
			PXSTR pszBuffer = GetBuffer( GetLength() );
			psz = pszBuffer+iFirst;
			int nDataLength = GetLength()-iFirst;
			Checked::memmove_s( pszBuffer, (GetLength()+1)*sizeof( XCHAR ),
				psz, (nDataLength+1)*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nDataLength );
		}

		return( *this );
	}

	// Remove all leading occurrences of any of the characters in string 'pszTargets'
	CStringT& TrimLeft(_In_z_ PCXSTR pszTargets)
	{
		// if we're not trimming anything, we're not doing any work
		if( (pszTargets == NULL) || (*pszTargets == 0) )
		{
			return( *this );
		}

		PCXSTR psz = GetString();
		while( (*psz != 0) && (StringTraits::StringFindChar( pszTargets, *psz ) != NULL) )
		{
			psz = StringTraits::CharNext( psz );
		}

		if( psz != GetString() )
		{
			// fix up data and length
			int iFirst = int( psz-GetString() );
			PXSTR pszBuffer = GetBuffer( GetLength() );
			psz = pszBuffer+iFirst;
			int nDataLength = GetLength()-iFirst;
			Checked::memmove_s( pszBuffer, (GetLength()+1)*sizeof( XCHAR ),
				psz, (nDataLength+1)*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nDataLength );
		}

		return( *this );
	}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Convert the string to the OEM character set
	void AnsiToOem()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::ConvertToOem( pszBuffer, nLength+1 );
		ReleaseBufferSetLength( nLength );
	}

	// Convert the string to the ANSI character set

	void OemToAnsi()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::ConvertToAnsi( pszBuffer, nLength+1 );
		ReleaseBufferSetLength( nLength );
	}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

	// Very simple sub-string extraction

	// Return the substring starting at index 'iFirst'
	CStringT Mid(_In_ int iFirst) const
	{
		return( Mid( iFirst, GetLength()-iFirst ) );
	}

	// Return the substring starting at index 'iFirst', with length 'nCount'
	CStringT Mid(
		_In_ int iFirst,
		_In_ int nCount) const
	{
		// nCount is in XCHARs

		// out-of-bounds requests return sensible things
		if (iFirst < 0)
			iFirst = 0;
		if (nCount < 0)
			nCount = 0;

		if( (::ATL::AtlAddThrow(iFirst,nCount)) > GetLength() )
		{
			nCount = GetLength()-iFirst;
		}
		if( iFirst > GetLength() )
		{
			nCount = 0;
		}

		ATLASSERT( (nCount == 0) || ((iFirst+nCount) <= GetLength()) );

		// optimize case of returning entire string
		if( (iFirst == 0) && ((iFirst+nCount) == GetLength()) )
		{
			return( *this );
		}

		return( CStringT( GetString()+iFirst, nCount, GetManager() ) );
	}

	// Return the substring consisting of the rightmost 'nCount' characters
	CStringT Right(_In_ int nCount) const
	{
		// nCount is in XCHARs
		if (nCount < 0)
			nCount = 0;

		int nLength = GetLength();
		if( nCount >= nLength )
		{
			return( *this );
		}

		return( CStringT( GetString()+nLength-nCount, nCount, GetManager() ) );
	}

	// Return the substring consisting of the leftmost 'nCount' characters
	CStringT Left(_In_ int nCount) const
	{
		// nCount is in XCHARs
		if (nCount < 0)
			nCount = 0;

		int nLength = GetLength();
		if( nCount >= nLength )
		{
			return( *this );
		}

		return( CStringT( GetString(), nCount, GetManager() ) );
	}

	// Return the substring consisting of the leftmost characters in the set 'pszCharSet'
	CStringT SpanIncluding(_In_z_ PCXSTR pszCharSet) const
	{
		ATLASSERT( AtlIsValidString( pszCharSet ) );
		if(pszCharSet == NULL)
			AtlThrow(E_INVALIDARG);

		return( Left( StringTraits::StringSpanIncluding( GetString(), pszCharSet ) ) );
	}

	// Return the substring consisting of the leftmost characters not in the set 'pszCharSet'
	CStringT SpanExcluding(_In_z_ PCXSTR pszCharSet) const
	{
		ATLASSERT( AtlIsValidString( pszCharSet ) );
		if(pszCharSet == NULL)
			AtlThrow(E_INVALIDARG);

		return( Left( StringTraits::StringSpanExcluding( GetString(), pszCharSet ) ) );
	}

	// Format data using format string 'pszFormat'
	void __cdecl Format(_In_z_ _Printf_format_string_ PCXSTR pszFormat, ...);

	// Format data using format string loaded from resource 'nFormatID'
	void __cdecl Format(_In_ _FormatMessage_format_string_ UINT nFormatID, ...);

	// Append formatted data using format string loaded from resource 'nFormatID'
	void __cdecl AppendFormat(_In_ _FormatMessage_format_string_ UINT nFormatID, ...);

	// Append formatted data using format string 'pszFormat'
	void __cdecl AppendFormat(_In_z_ _Printf_format_string_ PCXSTR pszFormat, ...);

	void AppendFormatV(
		_In_z_ _Printf_format_string_ PCXSTR pszFormat, 
		_In_ va_list args)
	{
		ATLASSERT( AtlIsValidString( pszFormat ) );
		if(pszFormat == NULL)
			AtlThrow(E_INVALIDARG);

		int nCurrentLength = GetLength();
		int nAppendLength = StringTraits::GetFormattedLength( pszFormat, args );

		if (nAppendLength < 0)
			AtlThrow(E_FAIL);

		PXSTR pszBuffer = GetBuffer( nCurrentLength+nAppendLength );
		StringTraits::Format( pszBuffer+nCurrentLength,
			nAppendLength+1, pszFormat, args );
		ReleaseBufferSetLength( nCurrentLength+nAppendLength );
	}

	void FormatV(
		_In_z_ _Printf_format_string_ PCXSTR pszFormat, 
		_In_ va_list args)
	{
		ATLASSERT( AtlIsValidString( pszFormat ) );
		if(pszFormat == NULL)
			AtlThrow(E_INVALIDARG);

		int nLength = StringTraits::GetFormattedLength( pszFormat, args );

		if (nLength < 0)
			AtlThrow(E_FAIL);

		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::Format( pszBuffer, nLength+1, pszFormat, args );
		ReleaseBufferSetLength( nLength );
	}

	// Format a message using format string 'pszFormat'
	void __cdecl _AFX_FUNCNAME(FormatMessage)(_In_z_ _FormatMessage_format_string_ PCXSTR pszFormat, ...);

	// Format a message using format string loaded from resource 'nFormatID'
	void __cdecl _AFX_FUNCNAME(FormatMessage)(_In_ _FormatMessage_format_string_ UINT nFormatID, ...);

#if defined(_AFX)
	void __cdecl FormatMessage(_In_z_ _FormatMessage_format_string_ PCXSTR pszFormat, ...);

	void __cdecl FormatMessage(_In_ _FormatMessage_format_string_ UINT nFormatID, ...);
#endif

	// Format a message using format string 'pszFormat' and va_list
	void FormatMessageV(
		_In_z_ _FormatMessage_format_string_ PCXSTR pszFormat, 
		_In_opt_ va_list* pArgList)
	{
		// format message into temporary buffer pszTemp
		CHeapPtr< XCHAR, CLocalAllocator > pszTemp;
		/*
			FormatMessage returns zero in case of failure or the number of characters
			if it is success, but we may actually get 0 as a number of characters.
			So to avoid this situation use SetLastError and GetLastErorr to determine
			whether the function FormatMessage has failed.
		*/
		DWORD dwLastError = ::GetLastError();
		::SetLastError(0);

		DWORD dwResult = StringTraits::_AFX_FUNCNAME(FormatMessage)( FORMAT_MESSAGE_FROM_STRING|
			FORMAT_MESSAGE_ALLOCATE_BUFFER, pszFormat, 0, 0, reinterpret_cast< PXSTR >( &pszTemp ),
			0, pArgList );

		if ((dwResult == 0) && (::GetLastError() != 0))
		{
			ThrowMemoryException();
		}
		::SetLastError(dwLastError);

		*this = pszTemp;
	}
	// OLE BSTR support
	// Allocate a BSTR containing a copy of the string
	_Ret_z_ BSTR AllocSysString() const
	{
		BSTR bstrResult = StringTraits::AllocSysString( GetString(), GetLength() );

		if( bstrResult == NULL )
		{
			ThrowMemoryException();
		}
		return( bstrResult );
	}

	_Ret_maybenull_z_ BSTR SetSysString(_Inout_ _Deref_post_opt_valid_ _Post_z_ BSTR* pbstr) const
	{
		ATLASSERT( AtlIsValidAddress( pbstr, sizeof( BSTR ) ) );

		if( !StringTraits::ReAllocSysString( GetString(), pbstr,
			GetLength() ) )
		{
			ThrowMemoryException();
		}

		ATLASSERT( *pbstr != NULL );

		return( *pbstr );
	}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Set the string to the value of environment variable 'pszVar'
	_Check_return_ BOOL _AFX_FUNCNAME(GetEnvironmentVariable)(_In_z_ PCXSTR pszVar)
	{
		ULONG nLength = StringTraits::GetEnvironmentVariable( pszVar, NULL, 0 );
		BOOL bRetVal = FALSE;

		if( nLength == 0 )
		{
			Empty();
		}
		else
		{
			PXSTR pszBuffer = GetBuffer( nLength );
			StringTraits::GetEnvironmentVariable( pszVar, pszBuffer, nLength );
			ReleaseBuffer();
			bRetVal = TRUE;
		}

		return( bRetVal );
	}

#if defined(_AFX)
	_Check_return_ BOOL GetEnvironmentVariable(_In_z_ PCXSTR pszVar)
	{
		return _AFX_FUNCNAME(GetEnvironmentVariable)(pszVar);
	}
#endif

	// Load the string from resource 'nID'
	_Check_return_ BOOL LoadString(_In_ UINT nID)
	{
		HINSTANCE hInst = StringTraits::FindStringResourceInstance( nID );
		if( hInst == NULL )
		{
			return( FALSE );
		}

		return( LoadString( hInst, nID ) );
	}

	// Load the string from resource 'nID' in module 'hInstance'
	_Check_return_ BOOL LoadString(
		_In_ HINSTANCE hInstance,
		_In_ UINT nID)
	{
		const ATLSTRINGRESOURCEIMAGE* pImage = AtlGetStringResourceImage( hInstance, nID );
		if( pImage == NULL )
		{
			return( FALSE );
		}

		int nLength = StringTraits::GetBaseTypeLength( pImage->achString, pImage->nLength );
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::ConvertToBaseType( pszBuffer, nLength, pImage->achString, pImage->nLength );
		ReleaseBufferSetLength( nLength );

		return( TRUE );
	}

	// Load the string from resource 'nID' in module 'hInstance', using language 'wLanguageID'
	_Check_return_ BOOL LoadString(
		_In_ HINSTANCE hInstance,
		_In_ UINT nID,
		_In_ WORD wLanguageID)
	{
		const ATLSTRINGRESOURCEIMAGE* pImage = AtlGetStringResourceImage( hInstance, nID, wLanguageID );
		if( pImage == NULL )
		{
			return( FALSE );
		}

		int nLength = StringTraits::GetBaseTypeLength( pImage->achString, pImage->nLength );
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::ConvertToBaseType( pszBuffer, nLength, pImage->achString, pImage->nLength );
		ReleaseBufferSetLength( nLength );

		return( TRUE );
	}
#endif  //  _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

	friend CStringT operator+(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2)
	{
		CStringT strResult( str1.GetManager() );

		Concatenate( strResult, str1, str1.GetLength(), str2, str2.GetLength() );

		return( strResult );
	}

	friend CStringT operator+(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2)
	{
		CStringT strResult( str1.GetManager() );

		Concatenate( strResult, str1, str1.GetLength(), psz2, StringLength( psz2 ) );

		return( strResult );
	}

	friend CStringT operator+(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2)
	{
		CStringT strResult( str2.GetManager() );

		Concatenate( strResult, psz1, StringLength( psz1 ), str2, str2.GetLength() );

		return( strResult );
	}

#ifdef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
#define _CSTRING_CHAR_T XCHAR
#else // def _CSTRING_NARROW_WIDE_CONVERSION
#define _CSTRING_CHAR_T char
#endif // def _CSTRING_NARROW_WIDE_CONVERSION

	friend CStringT operator+(
		_In_ const CStringT& str1,
		_In_ _CSTRING_CHAR_T ch2)
	{
		CStringT strResult( str1.GetManager() );
		XCHAR chTemp = XCHAR( ch2 );

		Concatenate( strResult, str1, str1.GetLength(), &chTemp, 1 );

		return( strResult );
	}

	friend CStringT operator+(
		_In_ _CSTRING_CHAR_T ch1,
		_In_ const CStringT& str2)
	{
		CStringT strResult( str2.GetManager() );
		XCHAR chTemp = XCHAR( ch1 );

		Concatenate( strResult, &chTemp, 1, str2, str2.GetLength() );

		return( strResult );
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	friend CStringT operator+(
		_In_ const CStringT& str1,
		_In_ wchar_t ch2)
	{
		CStringT strResult( str1.GetManager() );
		XCHAR chTemp = XCHAR( ch2 );

		Concatenate( strResult, str1, str1.GetLength(), &chTemp, 1 );

		return( strResult );
	}

	friend CStringT operator+(
		_In_ wchar_t ch1,
		_In_ const CStringT& str2)
	{
		CStringT strResult( str2.GetManager() );
		XCHAR chTemp = XCHAR( ch1 );

		Concatenate( strResult, &chTemp, 1, str2, str2.GetLength() );

		return( strResult );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	friend bool operator==(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2) throw()
	{
		return( str1.Compare( str2 ) == 0 );
	}

	friend bool operator==(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2) throw()
	{
		return( str1.Compare( psz2 ) == 0 );
	}

	friend bool operator==(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2) throw()
	{
		return( str2.Compare( psz1 ) == 0 );
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	friend bool operator==(
		_In_ const CStringT& str1,
		_In_z_ PCYSTR psz2) throw( ... )
	{
		CStringT str2( psz2, str1.GetManager() );

		return( str1 == str2 );
	}

	friend bool operator==(
		_In_z_ PCYSTR psz1,
		_In_ const CStringT& str2) throw( ... )
	{
		CStringT str1( psz1, str2.GetManager() );

		return( str1 == str2 );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	friend bool operator!=(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) != 0 );
	}

	friend bool operator!=(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2) throw()
	{
		return( str1.Compare( psz2 ) != 0 );
	}

	friend bool operator!=(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2) throw()
	{
		return( str2.Compare( psz1 ) != 0 );
	}

#ifndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION
	friend bool operator!=(
		_In_ const CStringT& str1,
		_In_z_ PCYSTR psz2) throw( ... )
	{
		CStringT str2( psz2, str1.GetManager() );

		return( str1 != str2 );
	}

	friend bool operator!=(
		_In_z_ PCYSTR psz1,
		_In_ const CStringT& str2) throw( ... )
	{
		CStringT str1( psz1, str2.GetManager() );

		return( str1 != str2 );
	}
#endif // ndef _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

	friend bool operator<(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2) throw()
	{
		return( str1.Compare( str2 ) < 0 );
	}

	friend bool operator<(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) < 0 );
	}

	friend bool operator<(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2) throw()
	{
		return( str2.Compare( psz1 ) > 0 );
	}

	friend bool operator>(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2) throw()
	{
		return( str1.Compare( str2 ) > 0 );
	}

	friend bool operator>(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2) throw()
	{
		return( str1.Compare( psz2 ) > 0 );
	}

	friend bool operator>(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2) throw()
	{
		return( str2.Compare( psz1 ) < 0 );
	}

	friend bool operator<=(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2) throw()
	{
		return( str1.Compare( str2 ) <= 0 );
	}

	friend bool operator<=(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2) throw()
	{
		return( str1.Compare( psz2 ) <= 0 );
	}

	friend bool operator<=(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2) throw()
	{
		return( str2.Compare( psz1 ) >= 0 );
	}

	friend bool operator>=(
		_In_ const CStringT& str1,
		_In_ const CStringT& str2) throw()
	{
		return( str1.Compare( str2 ) >= 0 );
	}

	friend bool operator>=(
		_In_ const CStringT& str1,
		_In_z_ PCXSTR psz2) throw()
	{
		return( str1.Compare( psz2 ) >= 0 );
	}

	friend bool operator>=(
		_In_z_ PCXSTR psz1,
		_In_ const CStringT& str2) throw()
	{
		return( str2.Compare( psz1 ) <= 0 );
	}

	friend bool operator==(
		_In_ XCHAR ch1,
		_In_ const CStringT& str2) throw()
	{
		return( (str2.GetLength() == 1) && (str2[0] == ch1) );
	}

	friend bool operator==(
		_In_ const CStringT& str1,
		_In_ XCHAR ch2) throw()
	{
		return( (str1.GetLength() == 1) && (str1[0] == ch2) );
	}

	friend bool operator!=(
		_In_ XCHAR ch1,
		_In_ const CStringT& str2) throw()
	{
		return( (str2.GetLength() != 1) || (str2[0] != ch1) );
	}

	friend bool operator!=(
		_In_ const CStringT& str1,
		_In_ XCHAR ch2) throw()
	{
		return( (str1.GetLength() != 1) || (str1[0] != ch2) );
	}

private:
	bool CheckImplicitLoad(_In_opt_ const void* pv)
	{
		bool bRet = false;

		if( (pv != NULL) && IS_INTRESOURCE( pv ) )
		{
			UINT nID = LOWORD( reinterpret_cast< DWORD_PTR >( pv ) );
			(nID);
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
			if( !LoadString( nID ) )
			{
				ATLTRACE( atlTraceString, 2, _T( "Warning: implicit LoadString(%u) failed\n" ), nID );
			}
			bRet = true;
#else
			ATLTRACE( atlTraceString, 2, _T( "Warning: LoadString(%u) not supported under the current WINAPI_FAMILY.\n" ), nID );
#endif
		}

		return( bRet );
	}

};

#ifndef _CSTRING_BUFFER_SIZE
#define _CSTRING_BUFFER_SIZE(_CStringObj) ((_CStringObj).GetAllocLength() + 1)
#endif


#pragma warning(push)
#pragma warning(disable : 4793)

// Format data using format string 'pszFormat'
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::Format(
	_In_z_ _Printf_format_string_ PCXSTR pszFormat,
	...)
{
	ATLASSERT( AtlIsValidString( pszFormat ) );

	va_list argList;
	va_start( argList, pszFormat );
	FormatV( pszFormat, argList );
	va_end( argList );
}

// Format data using format string loaded from resource 'nFormatID'
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::Format(
	_In_ _FormatMessage_format_string_ UINT nFormatID,
	...)
{
	CStringT strFormat( GetManager() );
	ATLENSURE( strFormat.LoadString( nFormatID ) );

	va_list argList;
	va_start( argList, nFormatID );
	FormatV( strFormat, argList );
	va_end( argList );
}

// Append formatted data using format string loaded from resource 'nFormatID'
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::AppendFormat(
	_In_ _FormatMessage_format_string_ UINT nFormatID,
	...)
{
	va_list argList;
	va_start( argList, nFormatID );

	CStringT strFormat( GetManager() );
	ATLENSURE( strFormat.LoadString( nFormatID ) );

	AppendFormatV( strFormat, argList );

	va_end( argList );
}


// Append formatted data using format string 'pszFormat'
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::AppendFormat(
	_In_z_ _Printf_format_string_ PCXSTR pszFormat, 
	...)
{
	ATLASSERT( AtlIsValidString( pszFormat ) );

	va_list argList;
	va_start( argList, pszFormat );

	AppendFormatV( pszFormat, argList );

	va_end( argList );
}

// Format a message using format string 'pszFormat'
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::_AFX_FUNCNAME(FormatMessage)(
	_In_z_ _FormatMessage_format_string_ PCXSTR pszFormat, 
	...)
{
	if(pszFormat == NULL)
		AtlThrow(E_INVALIDARG);

	va_list argList;
	va_start( argList, pszFormat );

	FormatMessageV( pszFormat, &argList );

	va_end( argList );
}

#if defined(_AFX)
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::FormatMessage(
	_In_z_ _FormatMessage_format_string_ PCXSTR pszFormat, 
	...)
{
	if(pszFormat == NULL)
		AtlThrow(E_INVALIDARG);

	va_list argList;
	va_start( argList, pszFormat );

	FormatMessageV( pszFormat, &argList );

	va_end( argList );
}
#endif

// Format a message using format string loaded from resource 'nFormatID'
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::_AFX_FUNCNAME(FormatMessage)(
	_In_ _FormatMessage_format_string_ UINT nFormatID, 
	...)
{
	// get format string from string table
	CStringT strFormat( GetManager() );
	ATLENSURE( strFormat.LoadString( nFormatID ) );

	if(strFormat.GetLength() == 0)
		AtlThrow(E_INVALIDARG);

	va_list argList;
	va_start( argList, nFormatID );

	FormatMessageV( strFormat, &argList );

	va_end( argList );
}

#if defined(_AFX)
template< typename BaseType, class StringTraits >
inline void __cdecl CStringT<BaseType, StringTraits>::FormatMessage(
	_In_ _FormatMessage_format_string_ UINT nFormatID, 
	...)
{
	// get format string from string table
	CStringT strFormat( GetManager() );

	ATLENSURE( strFormat.LoadString( nFormatID ) );

	if(strFormat.GetLength() == 0)
		AtlThrow(E_INVALIDARG);

	va_list argList;
	va_start( argList, nFormatID );

	FormatMessageV( strFormat, &argList );

	va_end( argList );
}
#endif

#pragma warning( pop )

class IFixedStringLog
{
public:
	virtual void OnAllocateSpill(
		_In_ int nActualChars,
		_In_ int nFixedChars,
		_In_ const CStringData* pData) throw() = 0;
	virtual void OnReallocateSpill(
		_In_ int nActualChars,
		_In_ int nFixedChars,
		_In_ const CStringData* pData) throw() = 0;
};

class CFixedStringMgr :
	public IAtlStringMgr
{
public:
	CFixedStringMgr(
			_Inout_ CStringData* pData,
			_In_ int nChars,
			_In_opt_ IAtlStringMgr* pMgr = NULL) throw() :
		m_pData( pData ),
		m_pMgr( pMgr )
	{
		m_pData->nRefs = -1;
		m_pData->nDataLength = 0;
		m_pData->nAllocLength = nChars;
		m_pData->pStringMgr = this;
		*static_cast< wchar_t* >( m_pData->data() ) = 0;
	}
	virtual ~CFixedStringMgr() throw()
	{
	}

// IAtlStringMgr
public:
	_Ret_maybenull_ _Post_writable_byte_size_(sizeof(CStringData) + nChars*nCharSize)
	virtual CStringData* Allocate(
		_In_ int nChars,
		_In_ int nCharSize) throw()
	{
		ATLASSUME( m_pData->nRefs == -1 );
		ATLASSUME( m_pData->nDataLength == 0 );
		ATLASSERT(nChars>=0);

		/* no way to report problems here; allocate empty */
		if(nChars<0)
		{
			nChars=0;
		}

		if( nChars > m_pData->nAllocLength )
		{
			if( s_pLog != NULL )
			{
				s_pLog->OnAllocateSpill( nChars, m_pData->nAllocLength, m_pData );
			}
			CStringData* pData = m_pMgr->Allocate( nChars, nCharSize );
			if( pData != NULL )
			{
				pData->pStringMgr = this;
				pData->nRefs = -1;  // Locked
			}

			return pData;
		}

		m_pData->nRefs = -1;  // Locked
		m_pData->nDataLength = 0;
		m_pData->pStringMgr = this;

		return m_pData;
	}
	virtual void Free(_Inout_ CStringData* pData) throw()
	{
		ATLASSERT( pData->nRefs <= 0 );
		if( pData != m_pData )
		{
			// Must have been allocated from the backup manager
			pData->pStringMgr = m_pMgr;
			m_pMgr->Free( pData );
		}

		// Always make sure the fixed buffer is ready to be used as the nil string.
		m_pData->nRefs = -1;
		m_pData->nDataLength = 0;
		*static_cast< wchar_t* >( m_pData->data() ) = 0;
	}
	_Ret_maybenull_ _Post_writable_byte_size_(sizeof(CStringData) + nChars*nCharSize)
	virtual CStringData* Reallocate(
		_Inout_ CStringData* pData,
		_In_ int nChars,
		_In_ int nCharSize)
	{
		CStringData* pNewData;

		ATLASSERT( pData->nRefs < 0 );
		if( pData != m_pData )
		{
			pData->pStringMgr = m_pMgr;
			pNewData = m_pMgr->Reallocate( pData, nChars, nCharSize );
			if( pNewData == NULL )
			{
				pData->pStringMgr = this;
			}
			else
			{
				pNewData->pStringMgr = this;
			}
		}
		else
		{
			if( nChars > pData->nAllocLength )
			{
				if( s_pLog != NULL )
				{
					s_pLog->OnReallocateSpill( nChars, pData->nAllocLength, pData );
				}
				pNewData = m_pMgr->Allocate( nChars, nCharSize );
				if( pNewData == NULL )
				{
					return NULL;
				}

				// Copy the string data
				Checked::memcpy_s( pNewData->data(), nChars*nCharSize,
					pData->data(), (pData->nAllocLength+1)*nCharSize );
				pNewData->nRefs = pData->nRefs;  // Locked
				pNewData->pStringMgr = this;
				pNewData->nDataLength = pData->nDataLength;
			}
			else
			{
				// Don't do anything if the buffer is already big enough.
				pNewData = pData;
			}
		}

		return pNewData;
	}
	virtual CStringData* GetNilString() throw()
	{
		ATLASSUME( m_pData->nRefs == -1 );
		ATLASSUME( m_pData->nDataLength == 0 );

		return m_pData;
	}
	virtual IAtlStringMgr* Clone() throw()
	{
		return m_pMgr;
	}

public:
	static IFixedStringLog* s_pLog;

	IAtlStringMgr* GetBackupManager() const throw()
	{
		return m_pMgr;
	}

protected:
	IAtlStringMgr* m_pMgr;
	CStringData* m_pData;
};

__declspec( selectany ) IFixedStringLog* CFixedStringMgr::s_pLog = NULL;

#pragma warning( push )
#pragma warning( disable: 4355 )  // 'this' used in base member initializer list

template< class StringType, int t_nChars >
class CFixedStringT :
	private CFixedStringMgr,  // This class must be first, since it needs to be initialized before StringType
	public StringType
{
public:
	CFixedStringT() throw() :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( static_cast< IAtlStringMgr* >( this ) )
	{
	}

	explicit CFixedStringT(_In_ IAtlStringMgr* pStringMgr) throw() :
		CFixedStringMgr( &m_data, t_nChars, pStringMgr ),
		StringType( static_cast< IAtlStringMgr* >( this ) )
	{
	}

	CFixedStringT(_In_ const CFixedStringT< StringType, t_nChars >& str) :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( str.GetString(), str.GetLength(), static_cast< CFixedStringMgr* >( this ) )
	{
	}

	CFixedStringT(_In_ const StringType& str) :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( str.GetString(), str.GetLength(), static_cast< CFixedStringMgr* >( this ) )
	{
	}

	CFixedStringT(_In_z_ const typename StringType::XCHAR* psz) :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( psz, static_cast< CFixedStringMgr* >( this ) )
	{
	}

	CFixedStringT(_In_reads_(nLength) const typename StringType::XCHAR* psz, _In_ int nLength) :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( psz, nLength, static_cast< CFixedStringMgr* >( this ) )
	{
	}

	explicit CFixedStringT(_In_z_ const typename StringType::YCHAR* psz) :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( psz, static_cast< CFixedStringMgr* >( this ) )
	{
	}

	explicit CFixedStringT(_In_z_ const unsigned char* psz) :
		CFixedStringMgr( &m_data, t_nChars, StrTraits::GetDefaultManager() ),
		StringType( psz, static_cast< CFixedStringMgr* >( this ) )
	{
	}

	~CFixedStringT() throw()
	{
		Empty();
	}

	CFixedStringT< StringType, t_nChars >& operator=(
		_In_ const CFixedStringT< StringType, t_nChars >& str)
	{
		StringType::operator=( str );
		return *this;
	}

	CFixedStringT< StringType, t_nChars >& operator=(_In_z_ const char* psz)
	{
		StringType::operator=( psz );
		return *this;
	}

	CFixedStringT< StringType, t_nChars >& operator=(_In_z_ const wchar_t* psz)
	{
		StringType::operator=( psz );
		return *this;
	}

	CFixedStringT< StringType, t_nChars >& operator=(_In_z_ const unsigned char* psz)
	{
		StringType::operator=( psz );
		return *this;
	}

	CFixedStringT< StringType, t_nChars >& operator=(_In_ const StringType& str)
	{
		StringType::operator=( str );
		return *this;
	}

// Implementation
protected:
	CStringData m_data;
	typename StringType::XCHAR m_achData[t_nChars+1];
};

#pragma warning( pop )
class CFixedStringLog :
	public IFixedStringLog
{
public:
	CFixedStringLog() throw()
	{
		CFixedStringMgr::s_pLog = this;
	}
	virtual ~CFixedStringLog() throw()
	{
		CFixedStringMgr::s_pLog = NULL;
	}

public:
	void OnAllocateSpill(
		_In_ int nActualChars,
		_In_ int nFixedChars,
		_In_ const CStringData* pData) throw()
	{
		(void)nActualChars;
		(void)nFixedChars;
		(void)pData;
		ATLTRACE( atlTraceString, 0, _T( "CFixedStringMgr::Allocate() spilling to heap.  %d chars (fixed size = %d chars)\n" ), nActualChars, nFixedChars );
	}
	void OnReallocateSpill(
		_In_ int nActualChars,
		_In_ int nFixedChars,
		_In_ const CStringData* pData) throw()
	{
		(void)nActualChars;
		(void)nFixedChars;
		(void)pData;
		ATLTRACE( atlTraceString, 0, _T( "CFixedStringMgr::Reallocate() spilling to heap.  %d chars (fixed size = %d chars)\n" ), nActualChars, nFixedChars );
	}
};

template< typename T >
class CStringElementTraits
{
public:
	typedef typename T::PCXSTR INARGTYPE;
	typedef T& OUTARGTYPE;

	static void __cdecl CopyElements(
		_Out_writes_all_(nElements) T* pDest,
		_In_reads_(nElements) const T* pSrc,
		_In_ size_t nElements)
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void __cdecl RelocateElements(
		_Out_writes_all_(nElements) T* pDest,
		_In_reads_(nElements) T* pSrc,
		_In_ size_t nElements)
	{
		Checked::memmove_s( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ) );
	}

	static ULONG __cdecl Hash(_In_ INARGTYPE str)
	{
		ATLENSURE( str != NULL );
		ULONG nHash = 0;
		const T::XCHAR* pch = str;
		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+(*pch);
			pch++;
		}

		return( nHash );
	}

	static bool __cdecl CompareElements(
		_In_ INARGTYPE str1,
		_In_ INARGTYPE str2)
	{
		return( T::StrTraits::StringCompare( str1, str2 ) == 0 );
	}

	static int __cdecl CompareElementsOrdered(
		_In_ INARGTYPE str1,
		_In_ INARGTYPE str2)
	{
		return( T::StrTraits::StringCompare( str1, str2 ) );
	}
};

#pragma pop_macro("new")

#if defined(_AFX)
#pragma pop_macro("FormatMessage")
#pragma pop_macro("GetEnvironmentVariable")
#endif

};  // namespace ATL



#ifdef __ATLCOMCLI_H__

namespace ATL
{

template< typename BaseType, class StringTraits >
CStringT< BaseType, StringTraits >::CStringT(_In_ const VARIANT& varSrc) :
	CThisSimpleString( StringTraits::GetDefaultManager() )
{
	CComVariant varResult;
	HRESULT hr = ::VariantChangeType( &varResult, const_cast< VARIANT* >( &varSrc ), 0, VT_BSTR );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	*this = V_BSTR( &varResult );
}

template< typename BaseType, class StringTraits >
CStringT< BaseType, StringTraits >::CStringT(
		_In_ const VARIANT& varSrc,
		_In_ IAtlStringMgr* pStringMgr) :
	CThisSimpleString( pStringMgr )
{
	CComVariant varResult;
	HRESULT hr = ::VariantChangeType( &varResult, const_cast< VARIANT* >( &varSrc ), 0, VT_BSTR );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	*this = V_BSTR( &varResult );
}

template< typename BaseType, class StringTraits >
CStringT< BaseType, StringTraits >& CStringT< BaseType, StringTraits >::operator=(
	_In_ const VARIANT& var)
{
	CComVariant varResult;
	HRESULT hr = ::VariantChangeType( &varResult, const_cast< VARIANT* >( &var ), 0, VT_BSTR );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	*this = V_BSTR( &varResult );

	return( *this );
}

template< typename BaseType, class StringTraits >
CStringT< BaseType, StringTraits >& CStringT< BaseType, StringTraits >::operator+=(
	_In_ const VARIANT& var)
{
	CComVariant varResult;
	HRESULT hr = ::VariantChangeType( &varResult, const_cast< VARIANT* >( &var ), 0, VT_BSTR );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	*this += V_BSTR( &varResult );

	return( *this );
}

}	// namespace ATL

#endif	// __ATLCOMCLI_H__


#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (pop)
#endif


#endif	// __CSTRINGT_H__ (whole file)
