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
// AFXSTR.H - Framework-independent, templateable string class

#ifndef __AFXSTR_H__
#define __AFXSTR_H__

#pragma once

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef _AFX
#error afxstr.h can only be used in MFC projects.  Use atlstr.h
#endif

#include <mbstring.h>

HINSTANCE AFXAPI AfxGetResourceHandle();
HINSTANCE AFXAPI AfxFindStringResourceHandle(UINT nID);

UINT_PTR AFXAPI AfxReadStringLength(CArchive& ar, int& nCharSize);
void AFXAPI AfxWriteStringLength(CArchive& ar, UINT_PTR nLength, BOOL bUnicode);

#include <atlcore.h>
#include <cstringt.h>

ATL::IAtlStringMgr* AFXAPI AfxGetStringManager();

template< typename _CharType = char, class StringIterator = ATL::ChTraitsCRT< _CharType > >
class StrTraitMFC : 
	public StringIterator
{
public:
	static HINSTANCE FindStringResourceInstance( UINT nID ) throw()
	{
		return( AfxFindStringResourceHandle( nID ) );
	}

	static ATL::IAtlStringMgr* GetDefaultManager() throw()
	{
		return( AfxGetStringManager() );
	}
};

template< typename _CharType, class StringIterator>
class StrTraitMFC_DLL : public StringIterator
{
public:
	static HINSTANCE FindStringResourceInstance( UINT nID ) throw()
	{
		return( AfxFindStringResourceHandle( nID ) );
	}

	static ATL::IAtlStringMgr* GetDefaultManager() throw()
	{
		return( AfxGetStringManager() );
	}
};

// MFC-enabled compilation. Use MFC memory management and exceptions;
// also, use MFC module state.

// Don't import when MFC dll is being built
#if defined(_AFXDLL)

#if defined(_MFC_DLL_BLD)

template class ATL::CSimpleStringT< char, true >;
template class ATL::CStringT< char, StrTraitMFC_DLL< char > >;
template class ATL::CSimpleStringT< wchar_t, true >;
template class ATL::CStringT< wchar_t, StrTraitMFC_DLL< wchar_t > >;

#else

template class __declspec(dllimport) ATL::CSimpleStringT< char, true >;
template class __declspec(dllimport) ATL::CStringT< char, StrTraitMFC_DLL< char > >;
template class __declspec(dllimport) ATL::CSimpleStringT< wchar_t, true >;
template class __declspec(dllimport) ATL::CStringT< wchar_t, StrTraitMFC_DLL< wchar_t > >;
#if defined(_NATIVE_WCHAR_T)
template class __declspec(dllimport) ATL::CSimpleStringT< unsigned short, true >;
template class __declspec(dllimport) ATL::CStringT< unsigned short, StrTraitMFC_DLL< unsigned short > >;
#endif	// _NATIVE_WCHAR_T

#endif	// _MFC_DLL_BLD

typedef ATL::CStringT< wchar_t, StrTraitMFC_DLL< wchar_t > > CStringW;
typedef ATL::CStringT< char, StrTraitMFC_DLL< char > > CStringA;
typedef ATL::CStringT< TCHAR, StrTraitMFC_DLL< TCHAR > > CString;

#else

typedef ATL::CStringT< wchar_t, StrTraitMFC< wchar_t > > CStringW;
typedef ATL::CStringT< char, StrTraitMFC< char > > CStringA;
typedef ATL::CStringT< TCHAR, StrTraitMFC< TCHAR > > CString;

#endif	// !_WIN64 && _AFXDLL

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif	// __AFXSTR_H__ (whole file)
