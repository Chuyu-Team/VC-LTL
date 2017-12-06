// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include <winreg.h>
#include <atlbase.h>

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CSettingsStore : public CObject
{
	friend class CSettingsStoreSP;
	DECLARE_DYNCREATE(CSettingsStore)

// Construction
public:
	CSettingsStore(BOOL bAdmin, BOOL bReadOnly);
	virtual ~CSettingsStore();

protected:
	CSettingsStore();

// Operations
public:
	virtual BOOL CreateKey(LPCTSTR lpszPath);
	virtual BOOL Open(LPCTSTR lpszPath);
	virtual void Close();

	virtual BOOL DeleteValue(LPCTSTR lpszValue);
	virtual BOOL DeleteKey(LPCTSTR lpszPath, BOOL bAdmin = FALSE);

	virtual BOOL Write(LPCTSTR lpszValueName, int nValue);
	virtual BOOL Write(LPCTSTR lpszValueName, DWORD dwVal);
	virtual BOOL Write(LPCTSTR lpszValueName, LPCTSTR lpszVal);
	virtual BOOL Write(LPCTSTR lpszValueName, const CRect& rect);
	virtual BOOL Write(LPCTSTR lpszValueName, LPBYTE pData, UINT nBytes);
	virtual BOOL Write(LPCTSTR lpszValueName, CObject& obj);
	virtual BOOL Write(LPCTSTR lpszValueName, CObject* pObj);

	virtual BOOL Read(LPCTSTR lpszValueName, int& nValue);
	virtual BOOL Read(LPCTSTR lpszValueName, DWORD& dwValue);
	virtual BOOL Read(LPCTSTR lpszValueName, CString& strValue);
	virtual BOOL Read(LPCTSTR lpszValueName, CRect& rect);
	virtual BOOL Read(LPCTSTR lpszValueName, BYTE** ppData, UINT* pcbData);
	virtual BOOL Read(LPCTSTR lpszValueName, CObject& obj);
	virtual BOOL Read(LPCTSTR lpszValueName, CObject*& pObj);

protected:
	ATL::CRegKey m_reg;
	CString m_strPath;
	BOOL    m_bReadOnly;
	BOOL    m_bAdmin;
	DWORD_PTR   m_dwUserData;
};

/*============================================================================*/
// CSettingsStoreSP - Helper class that manages "safe" CSettingsStore pointer

class CSettingsStoreSP
{
public:
	static BOOL __stdcall SetRuntimeClass(CRuntimeClass* pRTI);

	CSettingsStoreSP(DWORD_PTR dwUserData = 0) : m_pRegistry(NULL), m_dwUserData(dwUserData)
	{
	}

	~CSettingsStoreSP()
	{
		if (m_pRegistry != NULL)
		{
			ASSERT_VALID(m_pRegistry);
			delete m_pRegistry;
		}
	}

	CSettingsStore& Create(BOOL bAdmin, BOOL bReadOnly);

protected:
	CSettingsStore* m_pRegistry;
	DWORD_PTR      m_dwUserData;

	AFX_IMPORT_DATA static CRuntimeClass* m_pRTIDefault;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
