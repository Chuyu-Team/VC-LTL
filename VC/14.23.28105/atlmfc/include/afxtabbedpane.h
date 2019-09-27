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
#include "afxbasetabbedpane.h"
#include "afxtabctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CTabbedPane window

class CTabbedPane : public CBaseTabbedPane
{
	DECLARE_SERIAL(CTabbedPane)

// Construction
public:
	CTabbedPane(BOOL bAutoDestroy = FALSE);

// Attributes
public:
	virtual void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const;
	virtual BOOL IsTabLocationBottom() const;

	CMFCTabCtrl* GetTabWnd() const
	{
		ASSERT_KINDOF(CMFCTabCtrl, m_pTabWnd);
		return DYNAMIC_DOWNCAST(CMFCTabCtrl, m_pTabWnd);
	}

	virtual BOOL HasAutoHideMode() const { return TRUE; }

	AFX_IMPORT_DATA static CMFCTabCtrl::Style m_StyleTabWnd;
	AFX_IMPORT_DATA static BOOL m_bTabsAlwaysTop;
	AFX_IMPORT_DATA static CRuntimeClass* m_pTabWndRTC;

	static void __stdcall EnableTabAutoColor(BOOL bEnable = TRUE);
	static void __stdcall SetTabAutoColors(const CArray<COLORREF, COLORREF>& arColors);
	static void __stdcall ResetTabs();

// Operations
public:
	virtual BOOL FloatTab(CWnd* pBar, int nTabID, AFX_DOCK_METHOD dockMethod, BOOL bHide = FALSE);
	virtual BOOL DetachPane(CWnd* pBar, BOOL bHide = FALSE);

// Implementation
public:
	virtual ~CTabbedPane();

protected:
	virtual BOOL CheckTabbedBarAlignment();
	virtual void OnPressCloseButton();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

	// Tab Auto-colors support
	AFX_IMPORT_DATA static CArray<COLORREF, COLORREF> m_arTabsAutoColors;
	AFX_IMPORT_DATA static BOOL m_bIsTabsAutoColor;
	AFX_IMPORT_DATA static CList<HWND,HWND> m_lstTabbedControlBars;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
