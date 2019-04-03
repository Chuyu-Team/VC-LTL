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

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMenuTearOffManager : public CObject
{

public:
	CMenuTearOffManager();
	virtual ~CMenuTearOffManager();

// Operations:
	BOOL Initialize(LPCTSTR lpszRegEntry, UINT uiTearOffMenuFirst, UINT uiTearOffMenuLast);
	void Reset(HMENU hmenu);

	void SetupTearOffMenus(HMENU hMenu);
	void SetInUse(UINT uiCmdId, BOOL bUse = TRUE);

	void Build(UINT uiTearOffBarID, CString& strText);
	UINT Parse(CString& str);

protected:
	UINT GetFreeTearOffID();

// Attributes:
public:
	LPCTSTR GetRegPath() const
	{
		return m_strTearOffBarRegEntry;
	}

	BOOL IsDynamicID(UINT uiID) const
	{
		return (uiID >= m_uiTearOffMenuFirst && uiID <= m_uiTearOffMenuLast);
	}

protected:
	UINT    m_uiTearOffMenuFirst;
	UINT    m_uiTearOffMenuLast;
	CString m_strTearOffBarRegEntry;

	CArray<int,int> m_arTearOffIDsUsage;
};

extern CMenuTearOffManager* g_pTearOffMenuManager;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
