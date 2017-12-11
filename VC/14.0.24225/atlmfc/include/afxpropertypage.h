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
#include "afxdialogimpl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCPropertySheetCategoryInfo;

/*============================================================================*/
// CMFCPropertyPage window

class CMFCPropertyPage : public CPropertyPage
{
	friend class CMFCPopupMenu;
	friend class CContextMenuManager;
	friend class CMFCDropDownListBox;
	friend class CMFCPropertySheet;

	DECLARE_DYNCREATE(CMFCPropertyPage)

// Construction
public:
	CMFCPropertyPage();
	CMFCPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CMFCPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
	~CMFCPropertyPage();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

	void SetActiveMenu(CMFCPopupMenu* pMenu);
	void CommonInit();

	CDialogImpl      m_Impl;
	CMFCPropertySheetCategoryInfo* m_pCategory;

	int       m_nIcon;
	int       m_nSelIconNum;
	HTREEITEM m_hTreeNode;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
