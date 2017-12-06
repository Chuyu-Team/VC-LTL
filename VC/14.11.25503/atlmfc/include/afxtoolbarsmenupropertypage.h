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
#include "afxribbonres.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCMenuBar;
class CMFCPopupMenu;

/*============================================================================*/
// CMFCToolBarsMenuPropertyPage dialog

class CMFCToolBarsMenuPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCToolBarsMenuPropertyPage)

// Construction
public:
	CMFCToolBarsMenuPropertyPage(CFrameWnd* pParentFrame = NULL, BOOL bAutoSet = FALSE);
	~CMFCToolBarsMenuPropertyPage();

// Operations
public:
	void CloseContextMenu(CMFCPopupMenu* pMenu);
	BOOL SelectMenu(CDocTemplate* pTemplate, BOOL bSaveCurr = TRUE);

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE3 };
	CStatic   m_wndMenuAnimationsLabel;
	CStatic   m_wndContextHint;
	CStatic   m_wndContextMenuCaption;
	CStatic   m_wndIcon;
	CButton   m_wndMenuShadows;
	CButton   m_wndContextFrame;
	CButton   m_wndResetMenuButton;
	CComboBox m_wndMenuAnimations;
	CComboBox m_wndContextMenus;
	CComboBox m_wndMenuesList;
	CString   m_strMenuDescr;
	CString   m_strContextMenuName;
	CString   m_strMenuName;
	int       m_iMenuAnimationType;
	BOOL      m_bMenuShadows;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangeMenuList();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeContextMenuList();
	afx_msg void OnResetMenu();
	afx_msg void OnResetFrameMenu();

	DECLARE_MESSAGE_MAP()

protected:
	void SaveMenu();

// Attributes:
protected:
	CMFCMenuBar*   m_pMenuBar;
	CMFCPopupMenu* m_pContextMenu;
	CFrameWnd*  m_pParentFrame;

	HMENU m_hmenuCurr;
	HMENU m_hmenuSelected;
	UINT  m_uiContextMenuResId;
	BOOL  m_bIsDefaultMDIMenu;
	BOOL  m_bAutoSet;

	AFX_IMPORT_DATA static CPoint m_ptMenuLastPos;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
