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
#include "afxtoolbarbuttonslistbutton.h"
#include "afxtoolbarscommandslistbox.h"
#include "afxribbonres.h"
#include "afxtoolbarslistcheckbox.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCToolBarsCommandsPropertyPage dialog

class CMFCToolBarButton;
class CMFCToolBarImages;
class CMFCToolBar;

class CMFCToolBarsCommandsPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCToolBarsCommandsPropertyPage)

// Construction
public:
	CMFCToolBarsCommandsPropertyPage();
	~CMFCToolBarsCommandsPropertyPage();

// Operations:
	void SetUserCategory(LPCTSTR lpszCategory);
	void SetAllCategory(LPCTSTR lpszCategory);
	void OnChangeSelButton(CMFCToolBarButton* pButton);

protected:

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE1 };
	CListBox         m_wndCategory;
	CMFCToolBarsCommandsListBox m_wndTools;
	CString          m_strButtonDescription;

// Overrides
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnSelchangeUserTools();
	afx_msg void OnSelchangeCategory();

	DECLARE_MESSAGE_MAP()

// Attributes:
protected:
	CMFCToolBarButton* m_pSelButton;
	CString m_strUserCategory;
	CString m_strAllCategory;
};

/*============================================================================*/
// CMFCToolBarsListPropertyPage dialog

class CMFCToolBarsListPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCToolBarsListPropertyPage)

// Construction
public:
	CMFCToolBarsListPropertyPage(CFrameWnd* pParentFrame = NULL);
	~CMFCToolBarsListPropertyPage();

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE2 };
	CButton m_wndTextLabels;
	CButton m_bntRenameToolbar;
	CButton m_btnNewToolbar;
	CButton m_btnDelete;
	CButton m_btnReset;
	CMFCToolBarsListCheckBox m_wndToolbarList;
	BOOL m_bTextLabels;

// Operations:
public:
	void ShowToolBar(CMFCToolBar* pToolBar, BOOL bShow);
	void EnableUserDefinedToolbars(BOOL bEnable) { m_bUserDefinedToolbars = bEnable; }

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangeToolbarList();
	afx_msg void OnDblClkToolBarList();
	afx_msg void OnResetToolbar();
	afx_msg void OnResetAllToolbars();
	afx_msg void OnDeleteToolbar();
	afx_msg void OnNewToolbar();
	afx_msg void OnRenameToolbar();
	afx_msg void OnTextLabels();

	DECLARE_MESSAGE_MAP()

	CMFCToolBar* m_pSelectedToolbar;
	BOOL            m_bUserDefinedToolbars;
	CFrameWnd*      m_pParentFrame;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
