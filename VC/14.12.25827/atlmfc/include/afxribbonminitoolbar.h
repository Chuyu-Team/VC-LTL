// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#include "afxribbonpanelmenu.h"
#include "afxribbonpanel.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCRibbonMiniToolBar window

class CMFCRibbonMiniToolBar : public CMFCRibbonPanelMenu
{
	DECLARE_DYNCREATE(CMFCRibbonMiniToolBar)

	friend class CMFCPopupMenu;

// Construction
public:
	CMFCRibbonMiniToolBar();

// Attributes
public:
	BOOL IsContextMenuMode() const { return m_bContextMenuMode; }

protected:
	AFX_IMPORT_DATA static CMFCRibbonMiniToolBar* m_pCurrent;

	BOOL m_bContextMenuMode;
	BYTE m_nTransparency;
	BOOL m_bWasHovered;
	BOOL m_bWasDroppedDown;

// Operations
public:
	void SetCommands(CMFCRibbonBar* pRibbonBar, const CList<UINT,UINT>& lstCommands);

	BOOL Show(int x, int y);
	BOOL ShowWithContextMenu(int x, int y, UINT uiMenuResID, CWnd* pWndOwner);

// Overrides
	virtual BOOL IsRibbonMiniToolBar() const { return TRUE; }

// Implementation
public:
	virtual ~CMFCRibbonMiniToolBar();

protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

	BOOL UpdateTransparency();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
