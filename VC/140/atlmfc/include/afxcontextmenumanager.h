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

class CMFCPopupMenu;

class CContextMenuManager : public CObject
{
	friend class CMFCPopupMenuBar;

public:
	CContextMenuManager();
	virtual ~CContextMenuManager();

// Opreations:
public:
	BOOL AddMenu(UINT uiMenuNameResId, UINT uiMenuResId);
	BOOL AddMenu(LPCTSTR lpszName, UINT uiMenuResId);

	virtual BOOL ShowPopupMenu(UINT uiMenuResId, int x, int y, CWnd* pWndOwner, 
		BOOL bOwnMessage = FALSE, BOOL bRightAlign = FALSE);
	virtual CMFCPopupMenu* ShowPopupMenu(HMENU hmenuPopup, int x, int y, CWnd* pWndOwner, BOOL bOwnMessage = FALSE,
		BOOL bAutoDestroy = TRUE, BOOL bRightAlign = FALSE);
	virtual UINT TrackPopupMenu(HMENU hmenuPopup, int x, int y, CWnd* pWndOwner, BOOL bRightAlign = FALSE);

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL);

// Customization operations:
	void GetMenuNames(CStringList& listOfNames) const;
	HMENU GetMenuByName(LPCTSTR lpszName, UINT* puiOrigResID = NULL) const;
	HMENU GetMenuById(UINT nMenuResId) const;

	virtual BOOL ResetState();

// Attributes:
public:
	void SetDontCloseActiveMenu(BOOL bSet = TRUE)
	{
		m_bDontCloseActiveMenu = bSet;
	}

protected:
	CMap<UINT, UINT, HMENU, HMENU>       m_Menus;
	CMap<CString, LPCTSTR, HMENU, HMENU> m_MenuNames;
	CMap<UINT, UINT, CObList*, CObList*> m_MenuOriginalItems;

	UINT m_nLastCommandID;
	BOOL m_bTrackMode;
	BOOL m_bDontCloseActiveMenu;

	void CopyOriginalMenuItemsToMenu(UINT uiResId, CMFCPopupMenuBar& menuBar);
	void CopyOriginalMenuItemsFromMenu(UINT uiResId, CMFCPopupMenuBar& menuBar);
};

extern CContextMenuManager* afxContextMenuManager;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
