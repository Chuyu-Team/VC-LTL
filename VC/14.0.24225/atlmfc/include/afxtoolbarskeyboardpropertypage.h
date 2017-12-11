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

#include "afxribbonres.h"
#include "afxacceleratorkeyassignctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarButton;

/*============================================================================*/
// CMFCToolBarsKeyboardPropertyPage dialog

class CMFCToolBarsKeyboardPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCToolBarsKeyboardPropertyPage)

// Construction
public:
	CMFCToolBarsKeyboardPropertyPage(CFrameWnd* pParentFrame = NULL, BOOL bAutoSet = FALSE);
	~CMFCToolBarsKeyboardPropertyPage();

	void SetAllCategory(LPCTSTR lpszCategory);

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE5 };
	CStatic    m_wndAssignedToTitle;
	CMFCAcceleratorKeyAssignCtrl m_wndNewKey;
	CComboBox  m_wndViewTypeList;
	CStatic    m_wndViewIcon;
	CButton    m_wndRemoveButton;
	CListBox   m_wndCurrentKeysList;
	CListBox   m_wndCommandsList;
	CComboBox  m_wndCategoryList;
	CButton    m_wndAssignButton;
	CString    m_strDescription;
	CString    m_strAssignedTo;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();

	afx_msg void OnAssign();
	afx_msg void OnSelchangeCategory();
	afx_msg void OnSelchangeCommandsList();
	afx_msg void OnSelchangeCurrentKeysList();
	afx_msg void OnRemove();
	afx_msg void OnResetAll();
	afx_msg void OnSelchangeViewType();
	afx_msg void OnUpdateNewShortcutKey();

	DECLARE_MESSAGE_MAP()

// Operations:
protected:
	void AddKeyEntry(LPACCEL pEntry);

// Attributes:
protected:
	HACCEL  m_hAccelTable;
	LPACCEL m_lpAccel;
	LPACCEL m_pSelEntry;

	int  m_nAccelSize;
	BOOL m_bAutoSet;

	CMultiDocTemplate* m_pSelTemplate;
	CMFCToolBarButton* m_pSelButton;
	CFrameWnd* m_pParentFrame;
	CString m_strAllCategory;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
