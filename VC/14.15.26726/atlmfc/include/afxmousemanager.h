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

class CMouseManager : public CObject
{
	DECLARE_SERIAL(CMouseManager)

public:
	CMouseManager();
	virtual ~CMouseManager();

// Operations:
public:
	BOOL AddView(int iViewId, UINT uiViewNameResId, UINT uiIconId = 0);
	BOOL AddView(int iId, LPCTSTR lpszViewName, UINT uiIconId = 0);
	UINT GetViewDblClickCommand(int iId) const;

	BOOL LoadState(LPCTSTR lpszProfileName = NULL);
	BOOL SaveState(LPCTSTR lpszProfileName = NULL);

// Customization operations:
	void GetViewNames(CStringList& listOfNames) const;
	int GetViewIdByName(LPCTSTR lpszName) const;
	UINT GetViewIconId(int iViewId) const;

	void SetCommandForDblClk(int iViewId, UINT uiCmd);

// Overrides:
protected:
	virtual void Serialize(CArchive& ar);

// Attributes:
protected:
	CMap<CString, LPCTSTR, int, int> m_ViewsNames;
	CMap<int, int, UINT, UINT>       m_ViewsToCommands;
	CMap<int, int, UINT, UINT>       m_ViewsToIcons;
};

extern CMouseManager* afxMouseManager;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
