// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLPATH_H__
#define __ATLPATH_H__

#pragma once

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#ifdef _AFX
#include <afxstr.h>
#else
#include <atlstr.h>
#endif

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "shlwapi.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS


#pragma pack(push,_ATL_PACKING)
namespace ATL
{

namespace ATLPath
{

_Ret_maybenull_z_ inline char* AddBackslash(
	_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	return ::PathAddBackslashA( pszPath );
}

_Ret_maybenull_z_ inline wchar_t* AddBackslash(
	_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	return ::PathAddBackslashW( pszPath );
}

inline BOOL AddExtension(
	_Inout_updates_z_(MAX_PATH) char* pszPath,
	_In_z_ const char* pszExtension)
{
	return ::PathAddExtensionA( pszPath, pszExtension );
}

inline BOOL AddExtension(
	_Inout_updates_z_(MAX_PATH) wchar_t* pszPath,
	_In_z_ const wchar_t* pszExtension)
{
	return ::PathAddExtensionW( pszPath, pszExtension );
}

inline BOOL Append(
	_Inout_updates_z_(MAX_PATH) char* pszPath,
	_In_z_ const char* pszMore)
{
	return ::PathAppendA( pszPath, pszMore );
}

inline BOOL Append(
	_Inout_updates_z_(MAX_PATH) wchar_t* pszPath,
	_In_z_ const wchar_t* pszMore)
{
	return ::PathAppendW( pszPath, pszMore );
}

_Ret_z_ inline char* BuildRoot(
	_Out_writes_z_(4) char* pszPath,
	_In_ int iDrive)
{
	return ::PathBuildRootA( pszPath, iDrive );
}

_Ret_z_ inline wchar_t* BuildRoot(
	_Out_writes_z_(4) wchar_t* pszPath,
	_In_ int iDrive)
{
	return ::PathBuildRootW( pszPath, iDrive );
}

inline BOOL Canonicalize(
	_Out_writes_z_(MAX_PATH) char* pszDest,
	_In_z_ const char* pszSrc)
{
	return ::PathCanonicalizeA( pszDest, pszSrc );
}

inline BOOL Canonicalize(
	_Out_writes_z_(MAX_PATH) wchar_t* pszDest,
	_In_z_ const wchar_t* pszSrc)
{
	return ::PathCanonicalizeW( pszDest, pszSrc );
}

_Ret_maybenull_z_ inline char* Combine(
	_Out_writes_z_(MAX_PATH) char* pszDest,
	_In_z_ const char* pszDir,
	_In_z_ const char* pszFile)
{
	return ::PathCombineA( pszDest, pszDir, pszFile );
}

_Ret_maybenull_z_ inline wchar_t* Combine(
	_Out_writes_z_(MAX_PATH) wchar_t* pszDest,
	_In_ const wchar_t* pszDir,
	_In_ const wchar_t* pszFile)
{
	return ::PathCombineW( pszDest, pszDir, pszFile );
}

inline int CommonPrefix(
	_In_z_ const char* pszFile1,
	_In_z_ const char* pszFile2,
	_Out_writes_z_(MAX_PATH) char* pszDest)
{
	return ::PathCommonPrefixA( pszFile1, pszFile2, pszDest );
}

inline int CommonPrefix(
	_In_z_ const wchar_t* pszFile1,
	_In_z_ const wchar_t* pszFile2,
	_Out_writes_z_(MAX_PATH) wchar_t* pszDest)
{
	return ::PathCommonPrefixW( pszFile1, pszFile2, pszDest );
}

inline BOOL FileExists(_In_z_ const char* pszPath)
{
	return ::PathFileExistsA( pszPath );
}

inline BOOL FileExists(_In_z_ const wchar_t* pszPath)
{
	return ::PathFileExistsW( pszPath );
}

_Ret_z_ inline char* FindExtension(_In_z_ const char* pszPath)
{
	return ::PathFindExtensionA( pszPath );
}

_Ret_z_ inline wchar_t* FindExtension(_In_z_ const wchar_t* pszPath)
{
	return ::PathFindExtensionW( pszPath );
}

inline char* FindFileName(_In_z_ const char* pszPath)
{
	return ::PathFindFileNameA( pszPath );
}

inline wchar_t* FindFileName(_In_z_ const wchar_t* pszPath)
{
	return ::PathFindFileNameW( pszPath );
}

inline int GetDriveNumber(_In_z_ const char* pszPath)
{
	return ::PathGetDriveNumberA( pszPath );
}

inline int GetDriveNumber(_In_z_ const wchar_t* pszPath)
{
	return ::PathGetDriveNumberW( pszPath );
}

inline BOOL IsDirectory(_In_z_ const char* pszPath)
{
	return ::PathIsDirectoryA( pszPath );
}

inline BOOL IsDirectory(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsDirectoryW( pszPath );
}

inline BOOL IsFileSpec(_In_z_ const char* pszPath)
{
	return ::PathIsFileSpecA( pszPath );
}

inline BOOL IsFileSpec(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsFileSpecW( pszPath );
}

inline BOOL IsPrefix(
	_In_z_ const char* pszPrefix,
	_In_z_ const char* pszPath)
{
	return ::PathIsPrefixA( pszPrefix, pszPath );
}

inline BOOL IsPrefix(
	_In_z_ const wchar_t* pszPrefix,
	_In_z_ const wchar_t* pszPath)
{
	return ::PathIsPrefixW( pszPrefix, pszPath );
}

inline BOOL IsRelative(_In_z_ const char* pszPath)
{
	return ::PathIsRelativeA( pszPath );
}

inline BOOL IsRelative(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsRelativeW( pszPath );
}

inline BOOL IsRoot(_In_z_ const char* pszPath)
{
	return ::PathIsRootA( pszPath );
}

inline BOOL IsRoot(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsRootW( pszPath );
}

inline BOOL IsSameRoot(
	_In_z_ const char* pszPath1,
	_In_z_ const char* pszPath2)
{
	return ::PathIsSameRootA( pszPath1, pszPath2 );
}

inline BOOL IsSameRoot(
	_In_z_ const wchar_t* pszPath1,
	_In_z_ const wchar_t* pszPath2)
{
	return ::PathIsSameRootW( pszPath1, pszPath2 );
}

inline BOOL IsUNC(_In_z_ const char* pszPath)
{
	return ::PathIsUNCA( pszPath );
}

inline BOOL IsUNC(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsUNCW( pszPath );
}

inline BOOL IsUNCServer(_In_z_ const char* pszPath)
{
	return ::PathIsUNCServerA( pszPath );
}

inline BOOL IsUNCServer(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsUNCServerW( pszPath );
}

inline BOOL IsUNCServerShare(_In_z_ const char* pszPath)
{
	return ::PathIsUNCServerShareA( pszPath );
}

inline BOOL IsUNCServerShare(_In_z_ const wchar_t* pszPath)
{
	return ::PathIsUNCServerShareW( pszPath );
}

inline BOOL MakePretty(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	return ::PathMakePrettyA( pszPath );
}

inline BOOL MakePretty(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	return ::PathMakePrettyW( pszPath );
}

inline BOOL MatchSpec(
	_In_z_ const char* pszPath,
	_In_z_ const char* pszSpec)
{
	return ::PathMatchSpecA( pszPath, pszSpec );
}

inline BOOL MatchSpec(
	_In_z_ const wchar_t* pszPath,
	_In_z_ const wchar_t* pszSpec)
{
	return ::PathMatchSpecW( pszPath, pszSpec );
}

inline void QuoteSpaces(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	::PathQuoteSpacesA( pszPath );
}

inline void QuoteSpaces(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	::PathQuoteSpacesW( pszPath );
}

inline BOOL RelativePathTo(
	_Out_writes_z_(MAX_PATH) char* pszPath,
	_In_z_ const char* pszFrom,
	_In_ DWORD dwAttrFrom,
	_In_z_ const char* pszTo,
	_In_ DWORD dwAttrTo)
{
	return ::PathRelativePathToA( pszPath, pszFrom, dwAttrFrom, pszTo, dwAttrTo );
}

inline BOOL RelativePathTo(
	_Out_writes_z_(MAX_PATH) wchar_t* pszPath,
	_In_z_ const wchar_t* pszFrom,
	_In_ DWORD dwAttrFrom,
	_In_z_ const wchar_t* pszTo,
	_In_ DWORD dwAttrTo)
{
	return ::PathRelativePathToW( pszPath, pszFrom, dwAttrFrom, pszTo, dwAttrTo );
}

inline void RemoveArgs(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	::PathRemoveArgsA( pszPath );
}

inline void RemoveArgs(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	::PathRemoveArgsW( pszPath );
}

inline char* RemoveBackslash(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	return ::PathRemoveBackslashA( pszPath );
}

inline wchar_t* RemoveBackslash(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	return ::PathRemoveBackslashW( pszPath );
}

inline void RemoveBlanks(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	::PathRemoveBlanksA( pszPath );
}

inline void RemoveBlanks(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	::PathRemoveBlanksW( pszPath );
}

inline void RemoveExtension(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	::PathRemoveExtensionA( pszPath );
}

inline void RemoveExtension(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	::PathRemoveExtensionW( pszPath );
}

inline BOOL RemoveFileSpec(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	return ::PathRemoveFileSpecA( pszPath );
}

inline BOOL RemoveFileSpec(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	return ::PathRemoveFileSpecW( pszPath );
}

inline BOOL RenameExtension(
	_Inout_updates_z_(MAX_PATH) char* pszPath,
	_In_z_ const char* pszExt)
{
	return ::PathRenameExtensionA( pszPath, pszExt );
}

inline BOOL RenameExtension(
	_Inout_updates_z_(MAX_PATH) wchar_t* pszPath,
	_In_z_ const wchar_t* pszExt)
{
	return ::PathRenameExtensionW( pszPath, pszExt );
}

inline char* SkipRoot(_In_z_ const char* pszPath)
{
	return ::PathSkipRootA( pszPath );
}

inline wchar_t* SkipRoot(_In_z_ const wchar_t* pszPath)
{
	return ::PathSkipRootW( pszPath );
}

inline void StripPath(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	::PathStripPathA( pszPath );
}

inline void StripPath(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	::PathStripPathW( pszPath );
}

inline BOOL StripToRoot(_Inout_updates_z_(MAX_PATH) char* pszPath)
{
	return ::PathStripToRootA( pszPath );
}

inline BOOL StripToRoot(_Inout_updates_z_(MAX_PATH) wchar_t* pszPath)
{
	return ::PathStripToRootW( pszPath );
}

inline void UnquoteSpaces(_Inout_z_ char* pszPath)
{
	::PathUnquoteSpacesA( pszPath );
}

inline void UnquoteSpaces(_Inout_z_ wchar_t* pszPath)
{
	::PathUnquoteSpacesW( pszPath );
}

inline BOOL CompactPath(
	_In_ HDC hDC,
	_Inout_updates_z_(MAX_PATH) char* pszPath,
	_In_ UINT dx)
{
	return ::PathCompactPathA( hDC, pszPath, dx );
}

inline BOOL CompactPath(
	_In_ HDC hDC,
	_Inout_updates_z_(MAX_PATH) wchar_t* pszPath,
	_In_ UINT dx)
{
	return ::PathCompactPathW( hDC, pszPath, dx );
}

inline BOOL CompactPathEx(
	_Out_writes_z_(nMaxChars) char* pszDest,
	_In_z_ const char* pszSrc,
	_In_ UINT nMaxChars,
	_In_ DWORD dwFlags)
{
	return ::PathCompactPathExA( pszDest, pszSrc, nMaxChars, dwFlags );
}

inline BOOL CompactPathEx(
	_Out_writes_z_(nMaxChars) wchar_t* pszDest,
	_In_z_ const wchar_t* pszSrc,
	_In_ UINT nMaxChars,
	_In_ DWORD dwFlags)
{
	return ::PathCompactPathExW( pszDest, pszSrc, nMaxChars, dwFlags );
}

};  // namespace ATLPath

template< typename StringType >
class CPathT
{
public:
	typedef typename StringType::XCHAR XCHAR;
	typedef typename StringType::PCXSTR PCXSTR;
	typedef typename StringType::PXSTR PXSTR;

public:
	CPathT() throw()
	{
	}
	CPathT(_In_ const CPathT< StringType >& path) :
		m_strPath( path.m_strPath )
	{
	}
	CPathT(_In_z_ PCXSTR pszPath) :
		m_strPath( pszPath )
	{
	}

	operator const StringType& () const throw()
	{
		return m_strPath;
	}
	operator StringType& () throw()
	{
		return m_strPath;
	}
	operator PCXSTR() const throw()
	{
		return m_strPath;
	}
	CPathT< StringType >& operator+=(_In_z_ PCXSTR pszMore)
	{
		Append( pszMore );

		return *this;
	}

	void AddBackslash()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		ATLPath::AddBackslash( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL AddExtension(_In_z_ PCXSTR pszExtension)
	{		
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		BOOL bResult = ATLPath::AddExtension( pszBuffer, pszExtension );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL Append(_In_z_ PCXSTR pszMore)
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		BOOL bResult = ATLPath::Append( pszBuffer, pszMore );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	void BuildRoot(_In_ int iDrive)
	{
		ATLASSERT( iDrive >= 0 );
		ATLASSERT( iDrive <= 25 );

		PXSTR pszBuffer = m_strPath.GetBuffer( 4 );
		ATLPath::BuildRoot( pszBuffer, iDrive );
		m_strPath.ReleaseBuffer();
	}
	void Canonicalize()
	{
		StringType strResult;

		PXSTR pszBuffer = strResult.GetBuffer( MAX_PATH );
		ATLPath::Canonicalize( pszBuffer, m_strPath );
		strResult.ReleaseBuffer();

		m_strPath = strResult;
	}
	void Combine(
		_In_z_ PCXSTR pszDir,
		_In_z_ PCXSTR pszFile)
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		ATLPath::Combine( pszBuffer, pszDir, pszFile );
		m_strPath.ReleaseBuffer();
	}
	CPathT< StringType > CommonPrefix(_In_z_ PCXSTR pszOther)
	{
		CPathT< StringType > pathResult;

		PXSTR pszBuffer = pathResult.m_strPath.GetBuffer( __max( m_strPath.GetLength(), StringType::StringLength( pszOther ) ) );
		int nLength = ATLPath::CommonPrefix( m_strPath, pszOther, pszBuffer );
		pathResult.m_strPath.ReleaseBuffer( nLength );

		return pathResult;
	}
	BOOL CompactPath(
		_In_ HDC hDC,
		_In_ UINT nWidth)
	{
		// PathCompactPath can actually _increase_ the length of the path
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		BOOL bResult = ATLPath::CompactPath( hDC, pszBuffer, nWidth );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL CompactPathEx(
		_In_ UINT nMaxChars,
		_In_ DWORD dwFlags = 0)
	{
		StringType strResult;

		PXSTR pszBuffer = strResult.GetBuffer( nMaxChars );
		BOOL bResult = ATLPath::CompactPathEx( pszBuffer, m_strPath, nMaxChars,
			dwFlags );
		strResult.ReleaseBuffer();

		m_strPath = strResult;

		return bResult;
	}
	BOOL FileExists() const
	{
		return ATLPath::FileExists( m_strPath );
	}
	int FindExtension() const
	{
		PCXSTR pszBuffer = m_strPath;
		PCXSTR pszExtension = ATLPath::FindExtension( pszBuffer );
		if( *pszExtension == 0 )
			return -1;
		else
			return int( pszExtension-pszBuffer );
	}
	int FindFileName() const
	{
		PCXSTR pszBuffer = m_strPath;
		PCXSTR pszFileName = ATLPath::FindFileName( pszBuffer );
		if( *pszFileName == 0 )
			return -1;
		else
			return int( pszFileName-pszBuffer );
	}
	int GetDriveNumber() const
	{
		return ATLPath::GetDriveNumber( m_strPath );
	}
	StringType GetExtension() const
	{
		int iExtension;
		StringType strExtension;

		iExtension = FindExtension();
		if( iExtension != -1 )
			strExtension = m_strPath.Mid( iExtension );

		return strExtension;
	}
	BOOL IsDirectory() const
	{
		return ATLPath::IsDirectory( m_strPath );
	}

	BOOL IsFileSpec() const
	{
		return ATLPath::IsFileSpec( m_strPath );
	}
	BOOL IsPrefix(_In_z_ PCXSTR pszPrefix) const
	{
		return ATLPath::IsPrefix( pszPrefix, m_strPath );
	}
	BOOL IsRelative() const
	{
		return ATLPath::IsRelative( m_strPath );
	}
	BOOL IsRoot() const
	{
		return ATLPath::IsRoot( m_strPath );
	}
	BOOL IsSameRoot(_In_z_ PCXSTR pszOther) const
	{
		return ATLPath::IsSameRoot( m_strPath, pszOther );
	}
	BOOL IsUNC() const
	{
		return ATLPath::IsUNC( m_strPath );
	}
	BOOL IsUNCServer() const
	{
		return ATLPath::IsUNCServer( m_strPath );
	}
	BOOL IsUNCServerShare() const
	{
		return ATLPath::IsUNCServerShare( m_strPath );
	}
	BOOL MakePretty()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		BOOL bResult = ATLPath::MakePretty( pszBuffer );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL MatchSpec(_In_z_ PCXSTR pszSpec) const
	{
		return ATLPath::MatchSpec( m_strPath, pszSpec );
	}
	void QuoteSpaces()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		ATLPath::QuoteSpaces( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL RelativePathTo(
		_In_z_ PCXSTR pszFrom,
		_In_ DWORD dwAttrFrom,
		_In_z_ PCXSTR pszTo,
		_In_ DWORD dwAttrTo)
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		BOOL bResult = ATLPath::RelativePathTo( pszBuffer, pszFrom, dwAttrFrom,
			pszTo, dwAttrTo );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	void RemoveArgs()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::RemoveArgs( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	void RemoveBackslash()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::RemoveBackslash( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	void RemoveBlanks()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::RemoveBlanks( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	void RemoveExtension()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::RemoveExtension( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL RemoveFileSpec()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		BOOL bResult = ATLPath::RemoveFileSpec( pszBuffer );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL RenameExtension(_In_z_ PCXSTR pszExtension)
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		BOOL bResult = ATLPath::RenameExtension( pszBuffer, pszExtension );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	int SkipRoot() const
	{
		PCXSTR pszBuffer = m_strPath;
		PXSTR pszResult = ATLPath::SkipRoot( pszBuffer );

		return int( pszResult-pszBuffer );
	}
	void StripPath()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::StripPath( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL StripToRoot()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		BOOL bResult = ATLPath::StripToRoot( pszBuffer );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	void UnquoteSpaces()
	{
		PXSTR pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::UnquoteSpaces( pszBuffer );
		m_strPath.ReleaseBuffer();
	}

public:
	StringType m_strPath;
};

typedef CPathT< CString > CPath;
typedef CPathT< CStringA > CPathA;
typedef CPathT< CStringW > CPathW;

};  // namespace ATL
#pragma pack(pop)

#endif  //__ATLPATH_H__
