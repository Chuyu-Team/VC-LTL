// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXINET.H

#pragma once

/////////////////////////////////////////////////////////////////////////////
//

#ifdef _AFXINET_INLINE

_AFXINET_INLINE DWORD_PTR CInternetSession::GetContext() const
	{ return m_dwContext; }

_AFXINET_INLINE CString CInternetConnection::GetServerName() const
	{ return m_strServerName; }

_AFXINET_INLINE CInternetSession* CInternetConnection::GetSession() const
	{ return m_pSession; }

_AFXINET_INLINE CInternetSession::operator HINTERNET() const
	{ return m_hSession; }

_AFXINET_INLINE BOOL CInternetSession::SetOption(DWORD dwOption, DWORD dwValue,
	DWORD dwFlags /* = 0 */)
	{ ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0); return SetOption(dwOption, &dwValue, sizeof(dwValue), dwFlags); }


_AFXINET_INLINE CGopherLocator::operator LPCTSTR() const
	{ return (LPCTSTR) m_Locator; }

_AFXINET_INLINE BOOL CGopherLocator::GetLocatorType(DWORD& dwRef) const
	{ return GopherGetLocatorType((LPCTSTR) m_Locator, &dwRef); }

_AFXINET_INLINE CGopherLocator::CGopherLocator(const CGopherLocator& ref)
	{ m_Locator = ref.m_Locator; m_dwBufferLength = ref.m_dwBufferLength; }


#pragma push_macro("GetObject")
#undef GetObject
_AFXINET_INLINE CString CHttpFile::GetObject() const
	{ return _AFX_FUNCNAME(GetObject)(); }
#pragma pop_macro("GetObject")


_AFXINET_INLINE CInternetConnection::operator HINTERNET() const
	{ return m_hConnection; }

_AFXINET_INLINE DWORD_PTR CInternetConnection::GetContext() const
	{ return m_dwContext; }

_AFXINET_INLINE BOOL CInternetConnection::SetOption(DWORD dwOption,
	DWORD dwValue, DWORD dwFlags /* = 0 */)
	{ return SetOption(dwOption, &dwValue, sizeof(dwValue), dwFlags); }


_AFXINET_INLINE DWORD_PTR CInternetFile::GetContext() const
	{ return m_dwContext; }

_AFXINET_INLINE CInternetFile::operator HINTERNET() const
	{ return m_hFile; }

_AFXINET_INLINE BOOL CInternetFile::SetOption(DWORD dwOption, DWORD dwValue,
	DWORD dwFlags /* = 0 */)
	{ ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0); return SetOption(dwOption, &dwValue, sizeof(dwValue), dwFlags); }


#pragma push_macro("SetCurrentDirectory")
#undef SetCurrentDirectory
_AFXINET_INLINE BOOL CFtpConnection::SetCurrentDirectory(LPCTSTR pstrDirName)
	{ return _AFX_FUNCNAME(SetCurrentDirectory)(pstrDirName); }
#pragma pop_macro("SetCurrentDirectory")

#pragma push_macro("GetCurrentDirectory")
#undef GetCurrentDirectory
_AFXINET_INLINE BOOL CFtpConnection::GetCurrentDirectory(CString& strDirName) const
	{ return _AFX_FUNCNAME(GetCurrentDirectory)(strDirName); }
_AFXINET_INLINE BOOL CFtpConnection::GetCurrentDirectory(_Out_writes_to_(*lpdwLen, *lpdwLen) LPTSTR pstrDirName, _Inout_ LPDWORD lpdwLen) const
	{ return _AFX_FUNCNAME(GetCurrentDirectory)(pstrDirName, lpdwLen); }
#pragma pop_macro("GetCurrentDirectory")

#pragma push_macro("CreateDirectory")
#undef CreateDirectory
_AFXINET_INLINE BOOL CFtpConnection::CreateDirectory(LPCTSTR pstrDirName)
	{ return _AFX_FUNCNAME(CreateDirectory)(pstrDirName); }
#pragma pop_macro("CreateDirectory")


#endif //_AFXINET_INLINE

/////////////////////////////////////////////////////////////////////////////
